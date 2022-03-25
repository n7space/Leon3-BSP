/**@file
 * This file is part of the Leon3 BSP for the Test Environment.
 *
 * @copyright 2022 N7 Space Sp. z o.o.
 *
 * Test Environment was developed under a programme of,
 * and funded by, the European Space Agency (the "ESA").
 *
 *
 * Licensed under the ESA Public License (ESA-PL) Permissive,
 * Version 2.3 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://essr.esa.int/license/list
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/// \brief Uart hardware driver implementation.

#include <stdbool.h>
#include "Uart.h"
#include "UartRegisters.h"
#include "ByteFifo.h"
#include "SystemConfig.h"
#include <rtems.h>

#define GPTIMER_ADDRESS_BASE 0x80000300U

static inline UartRegisters_t
getAddressBase(Uart_Id id)
{
    switch(id) {
        case Uart_Id_0:
            return (UartRegisters_t)Uart0_address;
        case Uart_Id_1:
            return (UartRegisters_t)Uart1_address;
        case Uart_Id_2:
            return (UartRegisters_t)Uart2_address;
        case Uart_Id_3:
            return (UartRegisters_t)Uart3_address;
        case Uart_Id_4:
            return (UartRegisters_t)Uart4_address;
        case Uart_Id_5:
            return (UartRegisters_t)Uart5_address;
        default:
            return (UartRegisters_t)UartMax_address;
    }
}

static inline Uart_BaudRate
getBaudRate(const Uart* const uart)
{
    uint32_t freq = rtems_clock_get_ticks_per_second();
    uint16_t clkscl = (uart->reg->clkscl << UART_CLKSCL_SHIFT) + 1;
    uint32_t baud = freq / (UART_CLKSCL_DIV * clkscl);
    switch(baud) {
        case 300:
        case 600:
        case 1200:
        case 1800:
        case 2400:
        case 4800:
        case 9600:
        case 19200:
        case 28800:
        case 38400:
        case 57600:
        case 76800:
        case 115200:
            return (Uart_BaudRate)baud;
        default:
            return Uart_BaudRate_Max;
    }
}

static inline Uart_interrupt
interruptNumber(Uart_Id id)
{
    switch(id) {
        case Uart_Id_0:
            return Uart0_interrupt;
        case Uart_Id_1:
            return Uart1_interrupt;
#ifndef RTEMS_SIS
        case Uart_Id_2:
            return Uart2_interrupt;
        case Uart_Id_3:
            return Uart3_interrupt;
        case Uart_Id_4:
            return Uart4_interrupt;
        case Uart_Id_5:
            return Uart5_interrupt;
#endif
        default:
            return UartMax_interrupt;
    }
}

static inline void
emptyCallback(void* arg)
{
    (void)arg;
}

static Uart_TxHandler defaultTxHandler = { .callback = emptyCallback,
                                           .arg = 0 };

static Uart_RxHandler defaultRxHandler = { .lengthCallback = emptyCallback,
                                           .characterCallback = emptyCallback,
                                           .lengthArg = 0,
                                           .characterArg = 0,
                                           .targetCharacter = '\0',
                                           .targetLength = 0 };

static Uart_ErrorHandler defaultErrorHandler = { .callback = emptyCallback,
                                                 .arg = 0 };

void
Uart_setConfig(Uart* const uart, const Uart_Config* const config)
{
    if(config->isRxEnabled) {
        uart->reg->control |= (UART_CONTROL_RE | UART_CONTROL_RI);
        uart->reg->control &= ~UART_CONTROL_RF;
    }
    if(config->isTxEnabled) {
        uart->reg->control |= (UART_CONTROL_TE | UART_CONTROL_TI);
        uart->reg->control &= ~UART_CONTROL_TF;
    }
#ifndef RTEMS_SIS
    switch(config->parity) {
        case Uart_Parity_Even:
            uart->reg->control |= UART_CONTROL_PE;
            uart->reg->control &= ~UART_CONTROL_PS;
            break;
        case Uart_Parity_Odd:
            uart->reg->control |= (UART_CONTROL_PE | UART_CONTROL_PS);
            break;
        case Uart_Parity_None:
        default:
            uart->reg->control &= ~UART_CONTROL_PE;
            break;
    }
#endif
}

void
Uart_getConfig(const Uart* const uart, Uart_Config* const config)
{
    config->isTxEnabled = (uart->reg->control & UART_CONTROL_TE) != 0;
    config->isRxEnabled = (uart->reg->control & UART_CONTROL_RE) != 0;
    config->isTestModeEnabled = (uart->reg->control & UART_CONTROL_LB) != 0;
#ifndef RTEMS_SIS
    if((uart->reg->control & UART_CONTROL_PE) == 0) {
        config->parity = Uart_Parity_None;
    } else if((uart->reg->control & UART_CONTROL_PS) != 0) {
        config->parity = Uart_Parity_Odd;
    } else {
        config->parity = Uart_Parity_Even;
    }
    config->baudRate = getBaudRate(uart);
#endif
    config->baudRateClkSrc = GPTIMER_ADDRESS_BASE;
    config->baudRateClkFreq = rtems_clock_get_ticks_per_second();
}

void
Uart_startup(Uart* const uart)
{
    uart->interruptData.sentBytes = 0;
    uart->errorFlags = (Uart_ErrorFlags){ 0 };
    rtems_interrupt_entry_initialize(
            &(uart->interruptData.rtemsInterruptEntry),
            (rtems_interrupt_handler)Uart_handleInterrupt,
            uart,
            "Uart Interrupt");
    rtems_interrupt_entry_install(interruptNumber(uart->id),
                                  RTEMS_INTERRUPT_UNIQUE,
                                  &(uart->interruptData.rtemsInterruptEntry));
    rtems_interrupt_vector_enable(interruptNumber(uart->id));
}

void
Uart_shutdown(Uart* const uart)
{
    rtems_interrupt_vector_disable(interruptNumber(uart->id));
    rtems_interrupt_entry_remove(interruptNumber(uart->id),
                                 &(uart->interruptData.rtemsInterruptEntry));
    uart->reg->control = 0;
    uart->reg->status = 0;
    uart->interruptData.sentBytes = 0;
    uart->errorFlags = (Uart_ErrorFlags){ 0 };
}

void
Uart_init(Uart_Id id, Uart* const uart)
{
    uart->id = id;
    uart->reg = getAddressBase(id);
    uart->interruptData.rtemsInterruptEntry = (rtems_interrupt_entry){ 0 };
    uart->interruptData.sentBytes = 0;
    uart->errorFlags = (Uart_ErrorFlags){ 0 };
    uart->txHandler = defaultTxHandler;
    uart->rxHandler = defaultRxHandler;
    uart->errorHandler = defaultErrorHandler;
    uart->txFifo = NULL;
    uart->rxFifo = NULL;
    Uart_shutdown(uart);
    rtems_interrupt_clear(interruptNumber(uart->id));
}

bool
Uart_write(Uart* const uart,
           const uint8_t data,
           uint32_t const timeoutLimit,
           Uart_ErrorCode* const errCode)
{
    rtems_interrupt_vector_disable(interruptNumber(uart->id));
    uint32_t timeout = timeoutLimit;
    if(uart->txFifo == NULL) {
        if((uart->reg->status & UART_STATUS_TE) != 0) {
            uart->reg->data = data;
        } else {
            *errCode = Uart_ErrorCode_TxFifoFull;
            rtems_interrupt_vector_enable(interruptNumber(uart->id));
            return false;
        }
        while((timeoutLimit == 0) || timeout-- > 0) {
            if((uart->reg->status & UART_STATUS_TS) != 0) {
                rtems_interrupt_vector_enable(interruptNumber(uart->id));
                return true;
            }
        }
    } else {
        *errCode = Uart_ErrorCode_TxFifoNotNull;
        rtems_interrupt_vector_enable(interruptNumber(uart->id));
        return false;
    }
    *errCode = Uart_ErrorCode_Timeout;
    rtems_interrupt_vector_enable(interruptNumber(uart->id));
    return false;
}

bool
Uart_read(Uart* const uart,
          uint8_t* data,
          uint32_t const timeoutLimit,
          Uart_ErrorCode* const errCode)
{
    rtems_interrupt_vector_disable(interruptNumber(uart->id));
    uint32_t timeout = timeoutLimit;
    if(uart->rxFifo == NULL) {
        rtems_interrupt_vector_enable(interruptNumber(uart->id));
        do {
            if(uart->errorFlags.hasRxFifoFullErrorOccurred == true) {
                *errCode = Uart_ErrorCode_RxFifoFull;
                return false;
            }
            if((uart->reg->status & UART_STATUS_DR) != 0) {
                *data = uart->reg->data;
                return true;
            }
        } while((timeoutLimit == 0) || timeout-- > 0);
    } else {
        *errCode = Uart_ErrorCode_RxFifoNotNull;
        rtems_interrupt_vector_enable(interruptNumber(uart->id));
        return false;
    }
    *errCode = Uart_ErrorCode_Timeout;
    rtems_interrupt_vector_enable(interruptNumber(uart->id));
    return false;
}

void
Uart_writeAsync(Uart* const uart,
                ByteFifo* const fifo,
                const Uart_TxHandler handler)
{
    rtems_interrupt_vector_disable(interruptNumber(uart->id));
    uart->txFifo = fifo;
    uart->txHandler = handler;
    uint8_t byte = '\0';
    if((uart->reg->status & UART_STATUS_TE) != 0) {
        ByteFifo_pull(uart->txFifo, &byte);
        uart->reg->data = byte;
    }
    rtems_interrupt_vector_enable(interruptNumber(uart->id));
}

void
Uart_readAsync(Uart* const uart,
               ByteFifo* const fifo,
               const Uart_RxHandler handler)
{
    rtems_interrupt_vector_disable(interruptNumber(uart->id));
    uart->rxFifo = fifo;
    uart->rxHandler = handler;
    rtems_interrupt_vector_enable(interruptNumber(uart->id));
}

bool
Uart_isTxEmpty(const Uart* const uart)
{
    rtems_interrupt_vector_disable(interruptNumber(uart->id));
    bool result = ByteFifo_isEmpty(uart->txFifo);
    rtems_interrupt_vector_enable(interruptNumber(uart->id));
    return result;
}

bool
Uart_isRxEmpty(const Uart* const uart)
{
    rtems_interrupt_vector_disable(interruptNumber(uart->id));
    bool result = ByteFifo_isEmpty(uart->rxFifo);
    rtems_interrupt_vector_enable(interruptNumber(uart->id));
    return result;
}

uint32_t
Uart_getTxFifoCount(Uart* const uart)
{
    rtems_interrupt_vector_disable(interruptNumber(uart->id));
    bool result = ByteFifo_getCount(uart->txFifo);
    rtems_interrupt_vector_enable(interruptNumber(uart->id));
    return result;
}

uint32_t
Uart_getRxFifoCount(Uart* const uart)
{
    rtems_interrupt_vector_disable(interruptNumber(uart->id));
    bool result = ByteFifo_getCount(uart->rxFifo);
    rtems_interrupt_vector_enable(interruptNumber(uart->id));
    return result;
}

bool
Uart_handleError(Uart* const uart)
{
    if(Uart_getLinkErrors(uart->reg->status, &uart->errorFlags) == true) {
        uart->errorHandler.callback(uart->errorHandler.arg);
        return true;
    }
    return false;
}

bool
Uart_handleRx(Uart* const uart)
{
    if((uart->reg->control & UART_CONTROL_RE) != 0
       && (uart->reg->status & UART_STATUS_DR) != 0) {
        if(uart->rxFifo != NULL) {
            uint8_t buf = uart->reg->data;
            if(ByteFifo_isFull(uart->rxFifo) == true) {
                uart->errorFlags.hasRxFifoFullErrorOccurred = true;
                return true;
            }
            if(buf == uart->rxHandler.targetCharacter) {
                uart->rxHandler.characterCallback(uart->rxHandler.characterArg);
            }
            uart->interruptData.sentBytes++;
            if(uart->interruptData.sentBytes == uart->rxHandler.targetLength) {
                uart->interruptData.sentBytes = 0;
                uart->rxHandler.lengthCallback(uart->rxHandler.lengthArg);
            }
            ByteFifo_push(uart->rxFifo, buf);
        }
        return true;
    }
    return false;
}

bool
Uart_handleTx(Uart* const uart)
{
    if((uart->reg->control & UART_CONTROL_TE) != 0
       && (uart->reg->status & UART_STATUS_TE) != 0) {
        if(uart->txFifo != NULL) {
            uint8_t buf = '\0';
            if(ByteFifo_isEmpty(uart->txFifo) == false) {
                ByteFifo_pull(uart->txFifo, &buf);
                uart->reg->data = buf;
                if(ByteFifo_isEmpty(uart->txFifo) == true) {
                    uart->txHandler.callback(uart->txHandler.arg);
                }
            }
        }
        return true;
    }
    return false;
}

void
Uart_handleInterrupt(Uart* const uart)
{
    if(Uart_handleError(uart) == true) {
        return;
    }
    if(Uart_handleRx(uart) == true) {
        return;
    }
    if(Uart_handleTx(uart) == true) {
        return;
    }
}

void
Uart_registerErrorHandler(Uart* const uart, const Uart_ErrorHandler handler)
{
    rtems_interrupt_vector_disable(interruptNumber(uart->id));
    uart->errorHandler = handler;
    rtems_interrupt_vector_enable(interruptNumber(uart->id));
}

inline bool
Uart_getLinkErrors(uint32_t statusRegister, Uart_ErrorFlags* errFlags)
{
    if((statusRegister & UART_STATUS_OV) != 0) {
        errFlags->hasOverrunOccurred = true;
    }
    if((statusRegister & UART_STATUS_PE) != 0) {
        errFlags->hasParityErrorOccurred = true;
    }
    if((statusRegister & UART_STATUS_FE) != 0) {
        errFlags->hasFramingErrorOccurred = true;
    }
    if((statusRegister & UART_STATUS_RF) != 0) {
        errFlags->hasRxFifoFullErrorOccurred = true;
    }
    if(errFlags->hasOverrunOccurred || errFlags->hasParityErrorOccurred
       || errFlags->hasFramingErrorOccurred
       || errFlags->hasRxFifoFullErrorOccurred) {
        return true;
    }
    return false;
}

inline uint32_t
Uart_getStatusRegister(const Uart* const uart)
{
    return uart->reg->status;
}

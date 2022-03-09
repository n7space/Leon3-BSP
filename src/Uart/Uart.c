/**@file
 * This file is part of the Leon3 BSP for the Test Environment.
 *
 * @copyright 2022-2023 N7 Space Sp. z o.o.
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

#include "Uart.h"
#include "UartRegisters.h"
#include "ByteFifo.h"
#include "SystemConfig.h"
#include <rtems.h>

#define GPTIMER_ADDRESS_BASE 0x80000300U

#ifdef RTEMS_SIS
#define UART_INTERFACES_MAX 2
#else
#define UART_INTERFACES_MAX 6
#endif

#define UART_ERROR_CODE_OK 0U

#define UART_ERROR_CODE_TIMEOUT 1U

#define UART_ERROR_CODE_RX_FIFO_FULL 2U

#define UART_ERROR_CODE_TX_FIFO_FULL 3U

#define UART_FIFO_CAPACITY 4U

#define UART_FIFO_MEMORY_SIZE (UART_FIFO_CAPACITY * UART_INTERFACES_MAX)

static uint8_t rxFifoMemoryBlock[UART_FIFO_MEMORY_SIZE] = { 0 };

static uint8_t txFifoMemoryBlock[UART_FIFO_MEMORY_SIZE] = { 0 };

static Uart_ErrorFlags errorFlags[UART_INTERFACES_MAX];

static uint32_t sentBytesCounter[UART_INTERFACES_MAX];

static rtems_interrupt_entry uartIrqEntry[UART_INTERFACES_MAX];

static inline UartRegisters_t
addressBase(Uart_Id id)
{
    switch(id) {
        case Uart_Id_0:
            return (UartRegisters_t)Uart0_address;
        case Uart_Id_1:
            return (UartRegisters_t)Uart1_address;
#ifndef RTEMS_SIS
        case Uart_Id_2:
            return (UartRegisters_t)Uart2_address;
        case Uart_Id_3:
            return (UartRegisters_t)Uart3_address;
        case Uart_Id_4:
            return (UartRegisters_t)Uart4_address;
        case Uart_Id_5:
            return (UartRegisters_t)Uart5_address;
#endif
        default:
            return (UartRegisters_t)Error_address;
    }
}

static inline Uart_BaudRate
getBaudRate(const Uart* const uart)
{
    uint32_t freq = rtems_clock_get_ticks_per_second();
    uint32_t baud = freq / (8 * (uart->reg->clkscl + 1));
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
            return Error_BaudRate;
    }
}

static inline Uart_irqNum
irqNumber(Uart_Id id)
{
    switch(id) {
        case Uart_Id_0:
            return Uart0_irqNum;
        case Uart_Id_1:
            return Uart1_irqNum;
#ifndef RTEMS_SIS
        case Uart_Id_2:
            return Uart2_irqNum;
        case Uart_Id_3:
            return Uart3_irqNum;
        case Uart_Id_4:
            return Uart4_irqNum;
        case Uart_Id_5:
            return Uart5_irqNum;
#endif
        default:
            return Error_irqNum;
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
    uart->config.isTxEnabled = config->isTxEnabled;
    uart->config.isRxEnabled = config->isRxEnabled;
    uart->config.isTestModeEnabled = config->isTestModeEnabled;
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
    errorFlags[uart->id] = (Uart_ErrorFlags){ 0 };
    sentBytesCounter[uart->id] = uart->rxHandler.targetLength;
    rtems_interrupt_entry_initialize(&(uartIrqEntry[uart->id]),
                                     Uart_handleInterrupt,
                                     uart,
                                     "Uart Interrupt");
    rtems_interrupt_entry_install(irqNumber(uart->id),
                                  RTEMS_INTERRUPT_UNIQUE,
                                  &(uartIrqEntry[uart->id]));
    if(uart->config.isRxEnabled) {
        uart->reg->control |= (UART_CONTROL_RE | UART_CONTROL_RI);
        uart->reg->control &= ~UART_CONTROL_RF;
    }
    if(uart->config.isTxEnabled) {
        uart->reg->control |= (UART_CONTROL_TE | UART_CONTROL_TI);
        uart->reg->control &= ~UART_CONTROL_TF;
    }
    rtems_interrupt_vector_enable(irqNumber(uart->id));
}

void
Uart_shutdown(Uart* const uart)
{
    rtems_interrupt_vector_disable(irqNumber(uart->id));
    rtems_interrupt_entry_remove(irqNumber(uart->id),
                                 &(uartIrqEntry[uart->id]));
    uart->reg->control = 0;
    uart->reg->status = 0;
    uart->config.isTxEnabled = 0;
    uart->config.isRxEnabled = 0;
    uart->config.isTestModeEnabled = 0;
    ByteFifo_clear(uart->txFifo);
    ByteFifo_clear(uart->rxFifo);
    errorFlags[uart->id] = (Uart_ErrorFlags){ 0 };
}

void
Uart_init(Uart_Id id, Uart* const uart)
{
    errorFlags[uart->id] = (Uart_ErrorFlags){ 0 };
    uart->id = id;
    uart->reg = addressBase(id);
    *uart->rxFifo = (ByteFifo){ 0 };
    *uart->txFifo = (ByteFifo){ 0 };
    int fifoIndex = (int)uart->id * UART_FIFO_CAPACITY;
    ByteFifo_init(
            uart->rxFifo, rxFifoMemoryBlock + fifoIndex, UART_FIFO_CAPACITY);
    ByteFifo_init(
            uart->txFifo, txFifoMemoryBlock + fifoIndex, UART_FIFO_CAPACITY);
    ByteFifo_clear(uart->rxFifo);
    ByteFifo_clear(uart->txFifo);
    uart->config = (Uart_Config){ 0 };
    Uart_getConfig(uart, &uart->config);
    uart->txHandler = defaultTxHandler;
    uart->rxHandler = defaultRxHandler;
    uart->errorHandler = defaultErrorHandler;
    Uart_shutdown(uart);
    rtems_interrupt_clear(irqNumber(uart->id));
}

bool
Uart_write(Uart* const uart,
           const uint8_t data,
           uint32_t const timeoutLimit,
           int* const errCode)
{
    uint32_t timeout = timeoutLimit;
    if(ByteFifo_isFull(uart->txFifo) == 1) {
        *errCode = UART_ERROR_CODE_TX_FIFO_FULL;
        return 0;
    }
    ByteFifo_push(uart->txFifo, data);
    rtems_interrupt_raise(irqNumber(uart->id));
    while((timeoutLimit == 0) || timeout-- > 0) {
        if(ByteFifo_isEmpty(uart->txFifo) == 1) {
            return 1;
        }
    }
    ByteFifo_clear(uart->txFifo);
    *errCode = UART_ERROR_CODE_TIMEOUT;
    return 0;
}

bool
Uart_read(Uart* const uart,
          uint8_t* data,
          uint32_t const timeoutLimit,
          int* const errCode)
{
    uint8_t byte = '\0';
    uint32_t timeout = timeoutLimit;
    rtems_interrupt_raise(irqNumber(uart->id));
    do {
        if(errorFlags[uart->id].hasRxFifoFullErrorOccurred == 1) {
            *errCode = UART_ERROR_CODE_RX_FIFO_FULL;
            return 0;
        }
        if(ByteFifo_isEmpty(uart->rxFifo) == 0) {
            ByteFifo_pull(uart->rxFifo, &byte);
            *data = byte;
            return 1;
        }
    } while((timeoutLimit == 0) || timeout-- > 0);
    ByteFifo_clear(uart->rxFifo);
    *errCode = UART_ERROR_CODE_TIMEOUT;
    return 0;
}

void
Uart_writeAsync(Uart* const uart,
                ByteFifo* const fifo,
                const Uart_TxHandler handler)
{
    rtems_interrupt_vector_disable(irqNumber(uart->id));
    uart->txFifo = fifo;
    uart->txHandler = handler;
    rtems_interrupt_vector_enable(irqNumber(uart->id));
    rtems_interrupt_raise(irqNumber(uart->id));
}

void
Uart_readAsync(Uart* const uart,
               ByteFifo* const fifo,
               const Uart_RxHandler handler)
{
    rtems_interrupt_vector_disable(irqNumber(uart->id));
    uart->rxFifo = fifo;
    uart->rxHandler = handler;
    rtems_interrupt_vector_enable(irqNumber(uart->id));
    rtems_interrupt_raise(irqNumber(uart->id));
}

bool
Uart_isTxEmpty(const Uart* const uart)
{
    return ByteFifo_isEmpty(uart->txFifo);
}

bool
Uart_isRxEmpty(const Uart* const uart)
{
    return ByteFifo_isEmpty(uart->rxFifo);
}

uint32_t
Uart_getTxFifoCount(Uart* const uart)
{
    return ByteFifo_getCount(uart->txFifo);
}

uint32_t
Uart_getRxFifoCount(Uart* const uart)
{
    return ByteFifo_getCount(uart->rxFifo);
}

void
Uart_handleInterrupt(void* arg)
{
    uint8_t buf = '\0';
    Uart* uart = (Uart*)arg;
    if(Uart_getLinkErrors(uart->reg->status, &(errorFlags[uart->id])) != 0) {
        uart->errorHandler.callback(uart->errorHandler.arg);
        return;
    }
    if((uart->reg->control & UART_CONTROL_RE) != 0
       && (uart->reg->status & UART_STATUS_DR) != 0) {
        buf = uart->reg->data;
        if(ByteFifo_isFull(uart->rxFifo) == 1) {
            errorFlags[uart->id].hasRxFifoFullErrorOccurred = 1;
            return;
        }
        if(buf == uart->rxHandler.targetCharacter) {
            uart->rxHandler.characterCallback(uart->rxHandler.characterArg);
        }
        if(sentBytesCounter[uart->id]-- == 0) {
            uart->rxHandler.lengthCallback(uart->rxHandler.lengthArg);
            sentBytesCounter[uart->id] = uart->rxHandler.targetLength;
        }
        ByteFifo_push(uart->rxFifo, buf);
        return;
    }
    if((uart->reg->control & UART_CONTROL_TE) != 0
       && (uart->reg->status & UART_STATUS_TE) != 0) {
        if(ByteFifo_isEmpty(uart->txFifo) == 0) {
            ByteFifo_pull(uart->txFifo, &buf);
            uart->reg->data = buf;
            if(ByteFifo_isEmpty(uart->txFifo) == 1) {
                uart->txHandler.callback(uart->txHandler.arg);
            }
        }
    }
}

void
Uart_registerErrorHandler(Uart* const uart, const Uart_ErrorHandler handler)
{
    rtems_interrupt_vector_disable(irqNumber(uart->id));
    uart->errorHandler = handler;
    rtems_interrupt_vector_enable(irqNumber(uart->id));
}

inline bool
Uart_getLinkErrors(uint32_t statusRegister, Uart_ErrorFlags* errFlags)
{
    if((statusRegister & UART_STATUS_OV) != 0) {
        errFlags->hasOverrunOccurred = 1;
    }
    if((statusRegister & UART_STATUS_PE) != 0) {
        errFlags->hasParityErrorOccurred = 1;
    }
    if((statusRegister & UART_STATUS_FE) != 0) {
        errFlags->hasFramingErrorOccurred = 1;
    }
    if((statusRegister & UART_STATUS_RF) != 0) {
        errFlags->hasRxFifoFullErrorOccurred = 1;
    }
    if(errFlags->hasOverrunOccurred || errFlags->hasParityErrorOccurred
       || errFlags->hasFramingErrorOccurred
       || errFlags->hasRxFifoFullErrorOccurred) {
        return 1;
    }
    return 0;
}

inline uint32_t
Uart_getStatusRegister(const Uart* const uart)
{
    return ((UartRegisters_t)(uart->reg))->status;
}

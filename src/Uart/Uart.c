/**@file
 * This file is part of the Leon3 BSP for the Test Environment.
 *
 * @copyright 2022 N7 Space Sp. z o.o.
 * 
 * Leon3 BSP for the Test Environment was developed under the project AURORA.
 * This project has received funding from the European Union’s Horizon 2020
 * research and innovation programme under grant agreement No 101004291”
 *
 * Leon3 BSP for the Test Environment is free software: you can redistribute 
 * it and/or modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * Leon3 BSP for the Test Environment is distributed in the hope
 * that it will be useful, but WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Leon3 BSP for the Test Environment. If not,
 * see <http://www.gnu.org/licenses/>.
 */

/// \brief Uart hardware driver implementation.

#include <stdbool.h>
#include "Uart.h"
#include "UartRegisters.h"
#include "ByteFifo.h"
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
            return (UartRegisters_t)UartInvalid_address;
    }
}

static inline bool
setBaudRate(const Uart* const uart, const Uart_BaudRate baud)
{
    uint32_t baudValue = 0;
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
            baudValue = (uint32_t) baud;
            break;
        default:
            return false;
    }

    uint32_t freq = rtems_clock_get_ticks_per_second();
    uint16_t scaler = freq / (baudValue * 8) - 1;

    uart->reg->clkscl = (scaler << UART_SCALER_RELOAD_OFFSET) & UART_SCALER_RELOAD_MASK;

    return true;
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
            return (Uart_BaudRate) baud;
            break;
        default:
            return Uart_BaudRate_Invalid;
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
        case Uart_Id_2:
            return Uart2_interrupt;
        case Uart_Id_3:
            return Uart3_interrupt;
        case Uart_Id_4:
            return Uart4_interrupt;
        case Uart_Id_5:
            return Uart5_interrupt;
        default:
            return UartInvalid_interrupt;
    }
}

static inline void
emptyCallback(volatile void* arg)
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
    Uart_setFlag(&uart->reg->control, config->isRxEnabled, UART_CONTROL_RE);
    Uart_setFlag(&uart->reg->control, config->isRxEnabled, UART_CONTROL_RI);
    Uart_setFlag(&uart->reg->control, !config->isRxEnabled, UART_CONTROL_RF);
    Uart_setFlag(&uart->reg->control, config->isTxEnabled, UART_CONTROL_TE);
    Uart_setFlag(&uart->reg->control, config->isTxEnabled, UART_CONTROL_TI);
    Uart_setFlag(&uart->reg->control, !config->isTxEnabled, UART_CONTROL_TF);
    Uart_setFlag(&uart->reg->control, config->isLoopbackModeEnabled, UART_CONTROL_LB);

    setBaudRate(uart, config->baudRate);

    if (config->parity != Uart_Parity_None && config->parity != Uart_Parity_Invalid) {
        Uart_setFlag(&uart->reg->control, UART_FLAG_SET, UART_CONTROL_PE);
        Uart_setFlag(&uart->reg->control, config->parity == Uart_Parity_Odd, UART_CONTROL_PS);
    }
}

void
Uart_getConfig(const Uart* const uart, Uart_Config* const config)
{
    config->isRxEnabled = Uart_getFlag(uart->reg->control, UART_CONTROL_RE);
    config->isTxEnabled = Uart_getFlag(uart->reg->control, UART_CONTROL_TE);
    config->isLoopbackModeEnabled = Uart_getFlag(uart->reg->control, UART_CONTROL_LB);

    if (Uart_getFlag (uart->reg->control, UART_CONTROL_PE)) {
        config->parity = Uart_getFlag(uart->reg->control, UART_CONTROL_PS) ? Uart_Parity_Odd : Uart_Parity_Even;
    } else {
        config->parity = Uart_Parity_None;
    }

    config->baudRate = getBaudRate(uart);
    config->baudRateClkFreq = rtems_clock_get_ticks_per_second();
}

void
Uart_startup(Uart* const uart)
{
    uart->interruptData.sentBytes = 0;
    uart->errorFlags = (Uart_ErrorFlags){0};
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
    rtems_interrupt_entry_remove(interruptNumber(uart->id), &(uart->interruptData.rtemsInterruptEntry));
    uart->reg->control = 0;
    uart->reg->status = 0;
    uart->interruptData.sentBytes = 0;
    uart->errorFlags = (Uart_ErrorFlags){0};
}

void
Uart_init(Uart_Id id, Uart* const uart)
{
    uart->id = id;
#ifdef MOCK_REGISTERS
    uart->reg = malloc(sizeof(UartRegisters_t));
#else
    uart->reg = getAddressBase(id);
#endif
    uart->interruptData.rtemsInterruptEntry = (rtems_interrupt_entry){0};
    uart->interruptData.sentBytes = 0;
    uart->errorFlags = (Uart_ErrorFlags){0};
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
    if (uart->txFifo == NULL) {
        while ((timeoutLimit == 0) || timeout-- > 0) {
            if (Uart_getFlag(uart->reg->status, UART_STATUS_TS)) {
                uart->reg->data = data;
                rtems_interrupt_vector_enable(interruptNumber(uart->id));
                return true;
            }
        }
        return false;
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
    if (uart->rxFifo == NULL) {
        rtems_interrupt_vector_enable(interruptNumber(uart->id));
        do {
            if (uart->errorFlags.hasRxFifoFullErrorOccurred == true) {
                *errCode = Uart_ErrorCode_RxFifoFull;
                return false;
            }
            if (Uart_getFlag(uart->reg->status, UART_STATUS_DR)) {
                *data = uart->reg->data;
                return true;
            }
        } while ((timeoutLimit == 0) || timeout-- > 0);
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
    if (Uart_getFlag(uart->reg->status, UART_STATUS_TS)) {
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
    uint32_t result = ByteFifo_getCount(uart->txFifo);
    rtems_interrupt_vector_enable(interruptNumber(uart->id));
    return result;
}

uint32_t
Uart_getRxFifoCount(Uart* const uart)
{
    rtems_interrupt_vector_disable(interruptNumber(uart->id));
    uint32_t result = ByteFifo_getCount(uart->rxFifo);
    rtems_interrupt_vector_enable(interruptNumber(uart->id));
    return result;
}

bool
Uart_handleError(Uart* const uart)
{
    bool result = false;

    if (Uart_getLinkErrors(uart->reg->status, &uart->errorFlags) == true) {
        uart->errorHandler.callback(uart->errorHandler.arg);
        result = true;
    }

    return result;
}

bool
Uart_handleRx(Uart* const uart)
{
    bool result = false;

    if (Uart_getFlag(uart->reg->control, UART_CONTROL_RE) && Uart_getFlag(uart->reg->status, UART_STATUS_DR)) {
        if (uart->rxFifo != NULL) {
            uint8_t buf = uart->reg->data;
            if (ByteFifo_isFull(uart->rxFifo)) {
                uart->errorFlags.hasRxFifoFullErrorOccurred = true;
                result = true;
            } else {
                if (buf == uart->rxHandler.targetCharacter) {
                    uart->rxHandler.characterCallback(uart->rxHandler.characterArg);
                }
                uart->interruptData.sentBytes++;
                if (uart->interruptData.sentBytes == uart->rxHandler.targetLength) {
                    uart->interruptData.sentBytes = 0;
                    uart->rxHandler.lengthCallback(uart->rxHandler.lengthArg);
                }
                ByteFifo_push(uart->rxFifo, buf);
            }
        }
        result = true;
    }

    return result;
}

bool
Uart_handleTx(Uart* const uart)
{
    bool result = false;

    if (Uart_getFlag(uart->reg->control, UART_CONTROL_TE) && Uart_getFlag(uart->reg->status, UART_STATUS_TS)) {
        if (uart->txFifo != NULL) {
            uint8_t buf = '\0';
            if (!ByteFifo_isEmpty(uart->txFifo)) {
                ByteFifo_pull(uart->txFifo, &buf);
                uart->reg->data = buf;
                if (ByteFifo_isEmpty(uart->txFifo)) {
                    uart->txHandler.callback(uart->txHandler.arg);
                }
            }
        }
        result = true;
    }

    return result;
}

void
Uart_handleInterrupt(Uart* const uart)
{
    Uart_handleError(uart);
    Uart_handleRx(uart);
    Uart_handleTx(uart);
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
    bool result = false;

    errFlags->hasOverrunOccurred = Uart_getFlag(statusRegister, UART_STATUS_OV);
    errFlags->hasParityErrorOccurred = Uart_getFlag(statusRegister, UART_STATUS_PE);
    errFlags->hasFramingErrorOccurred = Uart_getFlag(statusRegister, UART_STATUS_FE);
    errFlags->hasRxFifoFullErrorOccurred = Uart_getFlag(statusRegister, UART_STATUS_RF);

    if (errFlags->hasOverrunOccurred ||
        errFlags->hasParityErrorOccurred ||
        errFlags->hasFramingErrorOccurred ||
        errFlags->hasRxFifoFullErrorOccurred) {
        result = true;
    }
    
    return result;
}

bool
Uart_getFlag(const uint32_t uartRegister, const uint32_t flag)
{
    return (bool) ((uartRegister >> flag) & UART_FLAG_MASK);
}

void
Uart_setFlag(volatile uint32_t *const uartRegister, const bool isSet, const uint32_t flag)
{
    if (isSet) {
        *uartRegister |= (UART_FLAG_MASK << flag);
    } else {
        *uartRegister &= ~(UART_FLAG_MASK << flag);
    }
}

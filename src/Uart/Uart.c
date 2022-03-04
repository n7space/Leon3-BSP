#include "Uart.h"
#include "UartRegisters.h"
#include "ByteFifo.h"
#include "SystemConfig.h"
#include <rtems.h>

#define GPTIMER_ADDRESS_BASE 0x80000300U
#define UART_ERROR_CODE_TIMEOUT 1
#define UART_ERROR_CODE_RX_FIFO_FULL 2

static struct
{
    uint8_t read;
    uint8_t write;
} iobuffer[UART_INTERFACES_MAX];

static uint32_t sentBytesCounter[UART_INTERFACES_MAX];

static Uart_ErrorFlags errorFlags[UART_INTERFACES_MAX];

static rtems_interrupt_entry uartIrqEntry[UART_INTERFACES_MAX];

static inline UartRegisters_t
addressBase(Uart_Id id)
{
    switch(id) {
        case Uart_Id_0:
            return (UartRegisters_t)Uart_address_0;
        case Uart_Id_1:
            return (UartRegisters_t)Uart_address_1;
#ifndef RTEMS_SIS
        case Uart_Id_2:
            return (UartRegisters_t)Uart_address_2;
        case Uart_Id_3:
            return (UartRegisters_t)Uart_address_3;
        case Uart_Id_4:
            return (UartRegisters_t)Uart_address_4;
        case Uart_Id_5:
            return (UartRegisters_t)Uart_address_5;
#endif
        default:
            return (UartRegisters_t)Error_address;
    }
}

static inline Uart_irqNum
irqNumber(Uart_Id id)
{
    switch(id) {
        case Uart_Id_0:
            return Uart_irqNum_0;
        case Uart_Id_1:
            return Uart_irqNum_1;
#ifndef RTEMS_SIS
        case Uart_Id_2:
            return Uart_irqNum_2;
        case Uart_Id_3:
            return Uart_irqNum_3;
        case Uart_Id_4:
            return Uart_irqNum_4;
        case Uart_Id_5:
            return Uart_irqNum_5;
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
    UartRegisters_t uartRegs = (UartRegisters_t)uart->reg;
    switch(config->parity) {
        case Uart_Parity_Even:
            uartRegs->control |= UART_CONTROL_PE;
            uartRegs->control &= ~UART_CONTROL_PS;
            break;
        case Uart_Parity_Odd:
            uartRegs->control |= (UART_CONTROL_PE | UART_CONTROL_PS);
            break;
        case Uart_Parity_None:
        default:
            uartRegs->control &= ~UART_CONTROL_PE;
            break;
    }
    uartRegs->clkscl = ((uart->config.baudRateClkFreq / uart->config.baudRate)
                        & UART_CLKSCL_MASK);
#endif
}

void
Uart_getConfig(const Uart* const uart, Uart_Config* const config)
{
    UartRegisters_t uartRegs = (UartRegisters_t)uart->reg;
    config->isTxEnabled = (uartRegs->control & UART_CONTROL_TE) != 0;
    config->isRxEnabled = (uartRegs->control & UART_CONTROL_RE) != 0;
    config->isTestModeEnabled = (uartRegs->control & UART_CONTROL_LB) != 0;
#ifndef RTEMS_SIS
    if((uartRegs->control & UART_CONTROL_PE) == 0) {
        config->parity = Uart_Parity_None;
    } else if((uartRegs->control & UART_CONTROL_PS) != 0) {
        config->parity = Uart_Parity_Odd;
    } else {
        config->parity = Uart_Parity_Even;
    }
    config->baudRateClkFreq = rtems_clock_get_ticks_per_second();
    config->baudRateClkSrc = GPTIMER_ADDRESS_BASE;
    config->baudRate = (rtems_clock_get_ticks_per_second() / uartRegs->clkscl);
#endif
}

void
Uart_startup(Uart* const uart)
{
    rtems_interrupt_entry_initialize(&(uartIrqEntry[uart->id]),
                                     Uart_handleInterrupt,
                                     uart,
                                     "Uart Interrupt");
    rtems_interrupt_entry_install(irqNumber(uart->id),
                                  RTEMS_INTERRUPT_UNIQUE,
                                  &(uartIrqEntry[uart->id]));
    UartRegisters_t uartRegs = (UartRegisters_t)uart->reg;
    if(uart->config.isRxEnabled) {
        uartRegs->control |= (UART_CONTROL_RE | UART_CONTROL_RI);
        uartRegs->control &= ~UART_CONTROL_RF;
    }
    if(uart->config.isTxEnabled) {
        uartRegs->control |= (UART_CONTROL_TE | UART_CONTROL_TI);
        uartRegs->control &= ~UART_CONTROL_TF;
    }
    rtems_interrupt_vector_enable(irqNumber(uart->id));
}

void
Uart_shutdown(Uart* const uart)
{
    UartRegisters_t uartRegs = (UartRegisters_t)uart->reg;
    uartRegs->control = 0;
    uartRegs->status = 0;
    uart->config.isTxEnabled = 0;
    uart->config.isRxEnabled = 0;
    uart->config.isTestModeEnabled = 0;
    rtems_interrupt_vector_disable(irqNumber(uart->id));
    rtems_interrupt_entry_remove(irqNumber(uart->id),
                                 &(uartIrqEntry[uart->id]));
    iobuffer[uart->id].read = '\0';
    iobuffer[uart->id].write = '\0';
    errorFlags[uart->id] = (Uart_ErrorFlags){ 0 };
}

void
Uart_init(Uart_Id id, Uart* const uart)
{
    iobuffer[uart->id].read = '\0';
    iobuffer[uart->id].write = '\0';
    errorFlags[uart->id] = (Uart_ErrorFlags){ 0 };
    uart->id = id;
    uart->reg = addressBase(id);
    uart->txFifo = (ByteFifo*)0;
    uart->rxFifo = (ByteFifo*)0;
    uart->config = (Uart_Config){ 0 };
    Uart_getConfig(uart, &uart->config);
    uart->txHandler = defaultTxHandler;
    uart->rxHandler = defaultRxHandler;
    sentBytesCounter[uart->id] = uart->rxHandler.targetLength;
    uart->errorHandler = defaultErrorHandler;
    Uart_shutdown(uart);
    rtems_interrupt_clear(irqNumber(uart->id));
}

void
Uart_write(Uart* const uart,
           const uint8_t data,
           uint32_t const timeoutLimit,
           int* const errCode)
{
    uint32_t timeout = timeoutLimit;
    iobuffer[uart->id].write = data;
    rtems_interrupt_raise(irqNumber(uart->id));
    while((timeoutLimit == 0) || timeout-- > 0) {
        if(iobuffer[uart->id].write == '\0') {
            return;
        }
    }
    iobuffer[uart->id].write = '\0';
    *errCode = UART_ERROR_CODE_TIMEOUT;
}

void
Uart_read(Uart* const uart,
          uint8_t* data,
          uint32_t const timeoutLimit,
          int* const errCode)
{
    uint32_t timeout = timeoutLimit;
    *data = '\0';
    rtems_interrupt_raise(irqNumber(uart->id));
    do {
        if(iobuffer[uart->id].read != '\0') {
            *data = iobuffer[uart->id].read;
            iobuffer[uart->id].read = '\0';
            return;
        }
        if(errorFlags[uart->id].hasRxFifoFullErrorOccurred) {
            *errCode = UART_ERROR_CODE_RX_FIFO_FULL;
            return;
        }
    } while((timeoutLimit == 0) || timeout-- > 0);
    *errCode = UART_ERROR_CODE_TIMEOUT;
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
}

inline uint8_t
Uart_isTxEmpty(const Uart* const uart)
{
    return ((uart->reg->status & UART_STATUS_TS) != 0);
}

inline void
Uart_readRxFifo(Uart* const uart, ByteFifo* const fifo)
{
    *fifo = *uart->rxFifo;
}

inline uint32_t
Uart_getTxFifoCount(Uart* const uart)
{
    return (uart->reg->status & UART_STATUS_TCNT);
}

inline uint32_t
Uart_getRxFifoCount(Uart* const uart)
{
    return (uart->reg->status & UART_STATUS_RCNT);
}

void
Uart_handleInterrupt(void* arg)
{
    Uart* uart = (Uart*)arg;
    if(Uart_getLinkErrors(uart->reg->status, &(errorFlags[uart->id])) != 0) {
        uart->errorHandler.callback(uart->errorHandler.arg);
        return;
    }
    if((uart->reg->control & UART_CONTROL_RE) != 0
       && (uart->reg->status & UART_STATUS_DR) != 0) {
        iobuffer[uart->id].read = uart->reg->data;
        if(iobuffer[uart->id].read == uart->rxHandler.targetCharacter) {
            uart->rxHandler.characterCallback(uart->rxHandler.characterArg);
        }
        if(sentBytesCounter[uart->id]-- == 0) {
            uart->rxHandler.lengthCallback(uart->rxHandler.lengthArg);
            sentBytesCounter[uart->id] = uart->rxHandler.targetLength;
        }
    }
    if((uart->reg->control & UART_CONTROL_TE) != 0
       && (uart->reg->status & UART_STATUS_TE) != 0) {
        if(iobuffer[uart->id].write != '\0') {
            uart->reg->data = iobuffer[uart->id].write;
            iobuffer[uart->id].write = '\0';
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

inline uint8_t
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

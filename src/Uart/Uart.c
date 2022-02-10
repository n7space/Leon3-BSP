#include "Uart.h"
#include "UartRegisters.h"
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

static inline uint32_t*
addressBase(Uart_Id id)
{
    if(id == Uart_Id_0) {
        return (uint32_t*)UART0_ADDRESS_BASE;
    }
#ifndef RTEMS_SIS
    if(id == Uart_Id_1) {
        return (uint32_t*)UART1_ADDRESS_BASE;
    }
    if(id == Uart_Id_2) {
        return (uint32_t*)UART2_ADDRESS_BASE;
    }
    if(id == Uart_Id_3) {
        return (uint32_t*)UART3_ADDRESS_BASE;
    }
    if(id == Uart_Id_4) {
        return (uint32_t*)UART4_ADDRESS_BASE;
    }
    if(id == Uart_Id_5) {
        return (uint32_t*)UART5_ADDRESS_BASE;
    }
#endif // RTEMS_SIS
    return (uint32_t*)UART_ERROR_ADDRESS;
}

static inline uint8_t
irqNumber(Uart_Id id)
{
    if(id == Uart_Id_0) {
        return UART0_IRQ;
    }
#ifndef RTEMS_SIS
    if(id == Uart_Id_1) {
        return UART1_IRQ;
    }
    if(id == Uart_Id_2) {
        return UART2_IRQ;
    }
    if(id == Uart_Id_3) {
        return UART3_IRQ;
    }
    if(id == Uart_Id_4) {
        return UART4_IRQ;
    }
    if(id == Uart_Id_5) {
        return UART5_IRQ;
    }
#endif // RTEMS_SIS
    return 0xFFU;
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
    UartRegisters_t regs = (UartRegisters_t)uart->reg;
    uart->txFifo = (uint32_t*)&(regs->data);
    uart->rxFifo = (uint32_t*)&(regs->data);
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
                uint32_t* const fifo,
                const Uart_TxHandler handler)
{
    rtems_interrupt_vector_disable(irqNumber(uart->id));
    uart->txFifo = fifo;
    uart->txHandler = handler;
    rtems_interrupt_vector_enable(irqNumber(uart->id));
}

void
Uart_readAsync(Uart* const uart,
               uint32_t* const fifo,
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
    UartRegisters_t regs = (UartRegisters_t)uart->reg;
    return ((regs->status & UART_STATUS_TS) != 0);
}

inline void
Uart_readRxFifo(Uart* const uart, uint8_t* const fifo)
{
    *fifo = *uart->rxFifo;
}

inline uint32_t
Uart_getTxFifoCount(Uart* const uart)
{
    UartRegisters_t regs = (UartRegisters_t)uart->reg;
    return (regs->status & UART_STATUS_TCNT);
}

inline uint32_t
Uart_getRxFifoCount(Uart* const uart)
{
    UartRegisters_t regs = (UartRegisters_t)uart->reg;
    return (regs->status & UART_STATUS_RCNT);
}

void
Uart_handleInterrupt(void* arg)
{
    Uart* uart = (Uart*)arg;
    UartRegisters_t regs = (UartRegisters_t)uart->reg;
    if(Uart_getLinkErrors(regs->status, &(errorFlags[uart->id])) != 0) {
        uart->errorHandler.callback(uart->errorHandler.arg);
        return;
    }
    if((regs->control & UART_CONTROL_RE) != 0
       && (regs->status & UART_STATUS_DR) != 0) {
        iobuffer[uart->id].read = *uart->rxFifo;
        if(iobuffer[uart->id].read == uart->rxHandler.targetCharacter) {
            uart->rxHandler.characterCallback(uart->rxHandler.characterArg);
        }
        if(sentBytesCounter[uart->id]-- == 0) {
            uart->rxHandler.lengthCallback(uart->rxHandler.lengthArg);
            sentBytesCounter[uart->id] = uart->rxHandler.targetLength;
        }
    }
    if((regs->control & UART_CONTROL_TE) != 0
       && (regs->status & UART_STATUS_TE) != 0) {
        if(iobuffer[uart->id].write != '\0') {
            *uart->txFifo = iobuffer[uart->id].write;
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

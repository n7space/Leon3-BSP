#ifndef BSP_UARTREGS_H
#define BSP_UARTREGS_H

#include "SystemConfig.h"
#include <stdint.h>

// clang-format off

#ifndef RTEMS_SIS

#define UART0_ADDRESS_BASE 0x80000100U
#define UART0_IRQ 2

#define UART1_ADDRESS_BASE 0x80100100U
#define UART1_IRQ 17

#define UART2_ADDRESS_BASE 0x80100200U
#define UART2_IRQ 18

#define UART3_ADDRESS_BASE 0x80100300U
#define UART3_IRQ 19

#define UART4_ADDRESS_BASE 0x80100400U
#define UART4_IRQ 20

#define UART5_ADDRESS_BASE 0x80100500U
#define UART5_IRQ 21

#else // RTEMS_SIS

#define UART0_ADDRESS_BASE 0x80000100U
#define UART0_IRQ 2

#endif // RTEMS_SIS

#define UART_ERROR_ADDRESS 0xA0000000U

typedef volatile struct UartRegisters
{
    uint32_t  data;
    uint32_t  status;
    uint32_t  control;
    uint32_t  clkscl;
    uint32_t  fifo;
} * UartRegisters_t;

#define UART_DATA_OFFSET    0x00U
#define UART_DATA_MASK      0xFFU

#define UART_STATUS_OFFSET  0x04U
#define UART_STATUS_RCNT    0xFC00000U // Receiver FIFO count
#define UART_STATUS_TCNT    0x3F00000U // Transmitter FIFO count
#define UART_STATUS_RF      0x400U     // Receiver FIFO full
#define UART_STATUS_TF      0x200U     // Transmitter FIFO full
#define UART_STATUS_RH      0x100U     // Receiver FIFO half-full
#define UART_STATUS_TH      0x80U      // Transmitter FIFO half-full
#define UART_STATUS_FE      0x40U      // Framing error
#define UART_STATUS_PE      0x20U      // Parity error
#define UART_STATUS_OV      0x10U      // Overrun
#define UART_STATUS_BR      0x8U       // Break received
#define UART_STATUS_TE      0x4U       // Transmitter FIFO empty
#define UART_STATUS_TS      0x2U       // Transmitter shift register empty
#define UART_STATUS_DR      0x1U       // Data ready

#define UART_CONTROL_OFFSET 0x08U
#define UART_CONTROL_FA     0x80000000U // FIFOs available
#define UART_CONTROL_DB     0x800U      // FIFO debug mode
#define UART_CONTROL_RF     0x400U      // Receiver FIFO interrupt
#define UART_CONTROL_TF     0x200U      // TransmitteR FIFO interrupt
#define UART_CONTROL_LB     0x80U       // Loopback
#define UART_CONTROL_PE     0x20U       // Parity
#define UART_CONTROL_PS     0x10U       // Parity select
#define UART_CONTROL_TI     0x8U        // Transmitter interrupt enable
#define UART_CONTROL_RI     0x4U        // Receiver interrupt enable
#define UART_CONTROL_TE     0x2U        // Transmitter enable
#define UART_CONTROL_RE     0x1U        // Receiver enable

#define UART_CLKSCL_OFFSET  0x0CU
#define UART_CLKSCL_MASK    0x0FFFU

#define UART_FIFO_OFFSET    0x10U
#define UART_FIFO_MASK      0xFFU

// clang-format on

#endif

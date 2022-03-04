#ifndef BSP_UARTREGS_H
#define BSP_UARTREGS_H

#include <stdint.h>

typedef enum
{
    Uart_address_0 = 0x80000100U,
    Uart_address_1 = 0x80100100U,
    Uart_address_2 = 0x80100200U,
    Uart_address_3 = 0x80100300U,
    Uart_address_4 = 0x80100400U,
    Uart_address_5 = 0x80100500U,
    Error_address = 0xFFFFFFFFU
} Uart_address;

typedef enum
{
    Uart_irqNum_0 = 2,
    Uart_irqNum_1 = 17,
    Uart_irqNum_2 = 18,
    Uart_irqNum_3 = 19,
    Uart_irqNum_4 = 20,
    Uart_irqNum_5 = 21,
    Error_irqNum = 0xFFU
} Uart_irqNum;

typedef volatile struct UartRegisters
{
    uint32_t data;
    uint32_t status;
    uint32_t control;
    uint32_t clkscl;
    uint32_t fifo;
} * UartRegisters_t;

// clang-format off

#define UART_DATA_OFFSET    0x00U
#define UART_DATA_MASK      0xFFU

#define UART_STATUS_OFFSET  0x04U
#define UART_STATUS_RCNT    0xFC000000U  // Receiver FIFO count
#define UART_STATUS_TCNT    0x3F000000U  // Transmitter FIFO count
#define UART_STATUS_RF      0x400U       // Receiver FIFO full
#define UART_STATUS_TF      0x200U       // Transmitter FIFO full
#define UART_STATUS_RH      0x100U       // Receiver FIFO half-full
#define UART_STATUS_TH      0x80U        // Transmitter FIFO half-full
#define UART_STATUS_FE      0x40U        // Framing error
#define UART_STATUS_PE      0x20U        // Parity error
#define UART_STATUS_OV      0x10U        // Overrun
#define UART_STATUS_BR      0x8U         // Break received
#define UART_STATUS_TE      0x4U         // Transmitter FIFO empty
#define UART_STATUS_TS      0x2U         // Transmitter shift register empty
#define UART_STATUS_DR      0x1U         // Data ready

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

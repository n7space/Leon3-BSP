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

/// \brief Uart hardware driver register addresses and datatypes.

/**
 * @defgroup Uart Uart
 * @ingroup Bsp
 * @{
 */

#ifndef BSP_UARTREGS_H
#define BSP_UARTREGS_H

#include <stdint.h>

/// \brief Structure representing UART memory addresses.
typedef enum
{
    Uart0_address = 0x80000100U, ///< UART0.
    Uart1_address = 0x80100100U, ///< UART1.
    Uart2_address = 0x80100200U, ///< UART2.
    Uart3_address = 0x80100300U, ///< UART3.
    Uart4_address = 0x80100400U, ///< UART4.
    Uart5_address = 0x80100500U, ///< UART5.
    Error_address = 0xFFFFFFFFU  ///< Error value.
} Uart_address;

/// \brief Structure representing UART interrupt vector numbers.
typedef enum
{
    Uart0_irqNum = 2,    ///< UART0 IRQ.
    Uart1_irqNum = 17,   ///< UART1 IRQ.
    Uart2_irqNum = 18,   ///< UART2 IRQ.
    Uart3_irqNum = 19,   ///< UART3 IRQ.
    Uart4_irqNum = 20,   ///< UART4 IRQ.
    Uart5_irqNum = 21,   ///< UART5 IRQ.
    Error_irqNum = 0xFFU ///< Error value.
} Uart_irqNum;

/// \brief Structure representing UART control and status registers (Big
///        Endian).
typedef volatile struct UartRegisters
{
    uint32_t data;    ///< Byte of data in hardware Rx/Tx FIFO.
    uint32_t status;  ///< 32-bit status register.
    uint32_t control; ///< 32-bit control register.
    uint32_t clkscl;  ///< 12-bit clock scaler reload value.
    uint32_t debug;   ///< Byte of data in hardware Rx/Tx FIFO debug register.
} * UartRegisters_t;

// clang-format off

#define UART_DATA_OFFSET    0x00U

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

#define UART_FIFO_OFFSET    0x10U

// clang-format on

#endif

/** @} */

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

/// \brief Uart hardware driver register addresses and datatypes.

/**
 * @defgroup Uart Uart
 * @ingroup Bsp
 * @{
 */

#ifndef BSP_UARTREGS_H
#define BSP_UARTREGS_H

#include <stdint.h>

/// \brief Enum representing UART memory addresses.
typedef enum
{
    Uart0_address = 0x80000100u,      ///< UART0
    Uart1_address = 0x80100100u,      ///< UART1
    Uart2_address = 0x80100200u,      ///< UART2
    Uart3_address = 0x80100300u,      ///< UART3
    Uart4_address = 0x80100400u,      ///< UART4
    Uart5_address = 0x80100500u,      ///< UART5
    UartInvalid_address = 0xFFFFFFFFu ///< Max value
} Uart_address;

/// \brief Enum representing UART interrupt vector numbers.
typedef enum
{
    Uart0_interrupt = 2,          ///< UART0 IRQ
    Uart1_interrupt = 17,         ///< UART1 IRQ
    Uart2_interrupt = 18,         ///< UART2 IRQ
    Uart3_interrupt = 19,         ///< UART3 IRQ
    Uart4_interrupt = 20,         ///< UART4 IRQ
    Uart5_interrupt = 21,         ///< UART5 IRQ
    UartInvalid_interrupt = 0xFFu ///< Max value
} Uart_interrupt;

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

#define UART_DATA_OFFSET    0x00u

#define UART_STATUS_OFFSET  0x04u
typedef enum { UART_STATUS_DR = 0, // Data ready: Indicates that new data is available in the receiver holding register.
               UART_STATUS_TS,     // Transmitter shift register empty: Indicates that the transmitter shift register is empty.
               UART_STATUS_TE,     // Transmitter FIFO empty: Indicates that the transmitter FIFO is empty.
               UART_STATUS_BR,     // Break received: Indicates that a BREAK has been received.
               UART_STATUS_OV,     // Overrun: Indicates that one or more character have been lost due to overrun.
               UART_STATUS_PE,     // Parity error: Indicates that a parity error was detected.
               UART_STATUS_FE,     // Framing error: Indicates that a framing error was detected.
               UART_STATUS_TH,     // Transmitter FIFO half-full: Indicates that the FIFO is less than half-full.
               UART_STATUS_RH,     // Receiver FIFO half-full: Indicates that at least half of the FIFO is holding data.
               UART_STATUS_TF,     // Transmitter FIFO full: Indicates that the Transmitter FIFO is full.
               UART_STATUS_RF,     // Receiver FIFO full: Indicates that the Receiver FIFO is full.
               UART_STATUS_TCNT,   // Transmitter FIFO count: Shows the number of data frames in the transmitter FIFO.
               UART_STATUS_RCNT    // Receiver FIFO count: Shows the number of data frames in the receiver FIFO.
} apbuart_status_register_flags;   // Status Register flags definition

#define UART_CONTROL_OFFSET 0x08u
typedef enum { UART_CONTROL_RE = 0,  // Receiver enable: If set, enables the receiver.
               UART_CONTROL_TE,      // Transmitter enable: If set, enables the transmitter.
               UART_CONTROL_RI,      // Receiver interrupt enable: If set, interrupts are generated when a frame is received.
               UART_CONTROL_TI,      // Transmitter interrupt enable: If set, interrupts are generated when a frame is transmitted.
               UART_CONTROL_PS,      // Parity select: Selects parity polarity (0 = even parity, 1 = odd parity) (when implemented).
               UART_CONTROL_PE,      // Parity enable: If set, enables parity generation and checking (when implemented).
               UART_CONTROL_LB = 7,  // Loop back: If set, loop back mode will be enabled.
               UART_CONTROL_TF = 9,  // Transmitter FIFO interrupt enable: When set, Transmitter FIFO level interrupts are enabled.
               UART_CONTROL_RF,      // Receiver FIFO interrupt enable: When set, Receiver FIFO level interrupts are enabled.
               UART_CONTROL_DB,      // FIFO debug mode enable: When set, it is possible to read and write the FIFO debug register.
               UART_CONTROL_FA = 31  // FIFOs available: Set to 1, read-only. Receiver and transmitter FIFOs are available.
} apbuart_control_register_flags;    // Control Register flags definition

#define UART_CLKSCL_OFFSET  0x0Cu
#define UART_CLKSCL_DIV     8u          // Clock divider value for baud rate generation
#define UART_CLKSCL_SHIFT   4u          // Byte shift to convert 12-bit clkscl register value to 16-bit

#define UART_SCALER_RELOAD_MASK     0X00000FFF
#define UART_SCALER_RELOAD_OFFSET   0

#define UART_FIFO_OFFSET    0x10u

#define UART_FLAG_SET 0x01u
#define UART_FLAG_RESET 0x00u
#define UART_FLAG_MASK 0x01u

// clang-format on

#endif

/** @} */

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
typedef enum { UART_DR = 0,      // Data ready: Indicates that new data is available in the receiver holding register.
               UART_TS,          // Transmitter shift register empty: Indicates that the transmitter shift register is empty.
               UART_TE,          // Transmitter FIFO empty: Indicates that the transmitter FIFO is empty.
               UART_BR,          // Break received: Indicates that a BREAK has been received.
               UART_OV,          // Overrun: Indicates that one or more character have been lost due to overrun.
               UART_PE,          // Parity error: Indicates that a parity error was detected.
               UART_FE,          // Framing error: Indicates that a framing error was detected.
               UART_TH,          // Transmitter FIFO half-full: Indicates that the FIFO is less than half-full.
               UART_RH,          // Receiver FIFO half-full: Indicates that at least half of the FIFO is holding data.
               UART_TF,          // Transmitter FIFO full: Indicates that the Transmitter FIFO is full.
               UART_RF,          // Receiver FIFO full: Indicates that the Receiver FIFO is full.
               UART_TCNT,        // Transmitter FIFO count: Shows the number of data frames in the transmitter FIFO.
               UART_RCNT         // Receiver FIFO count: Shows the number of data frames in the receiver FIFO.
} apbuart_status_register_flags; // Status Register flags definition

#define UART_CONTROL_OFFSET 0x08u
typedef enum { UART_RE = 0,       // Receiver enable: If set, enables the receiver.
               UART_CTRL_TE,      // Transmitter enable: If set, enables the transmitter.
               UART_RI,           // Receiver interrupt enable: If set, interrupts are generated when a frame is received.
               UART_TI,           // Transmitter interrupt enable: If set, interrupts are generated when a frame is transmitted.
               UART_PS,           // Parity select: Selects parity polarity (0 = even parity, 1 = odd parity) (when implemented).
               UART_CTRL_PE,      // Parity enable: If set, enables parity generation and checking (when implemented).
               UART_LB = 7,       // Loop back: If set, loop back mode will be enabled.
               UART_CRTL_TF = 9,  // Transmitter FIFO interrupt enable: When set, Transmitter FIFO level interrupts are enabled.
               UART_CTRL_RF,      // Receiver FIFO interrupt enable: When set, Receiver FIFO level interrupts are enabled.
               UART_DB,           // FIFO debug mode enable: When set, it is possible to read and write the FIFO debug register.
               UART_FA = 31       // FIFOs available: Set to 1, read-only. Receiver and transmitter FIFOs are available.
} apbuart_control_register_flags; // Control Register flags definition

#define UART_CLKSCL_OFFSET  0x0Cu
#define UART_CLKSCL_DIV     8u          // Clock divider value for baud rate generation
#define UART_CLKSCL_SHIFT   4u          // Byte shift to convert 12-bit clkscl register value to 16-bit

#define UART_FIFO_OFFSET    0x10u

#define FLAG_SET 0x01u
#define FLAG_RESET 0x00u
#define FLAG_MASK 0x01u

// clang-format on

#endif

/** @} */

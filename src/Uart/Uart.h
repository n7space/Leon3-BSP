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

/// \brief Uart hardware driver function prototypes and datatypes.

/**
 * @defgroup Uart Uart
 * @ingroup Bsp
 * @{
 */

#ifndef BSP_UART_H
#define BSP_UART_H

#include <UartRegisters.h>
#include <ByteFifo.h>
#include <stdint.h>
#include <stdbool.h>
#include <rtems.h>

/// \brief Uart device identifiers.
typedef enum
{
    Uart_Id_0 = 0,  ///< UART0 device
    Uart_Id_1 = 1,  ///< UART1 device
    Uart_Id_2 = 2,  ///< UART2 device
    Uart_Id_3 = 3,  ///< UART3 device
    Uart_Id_4 = 4,  ///< UART4 device
    Uart_Id_5 = 5,  ///< UART5 device
    Uart_Id_Max = 6 ///< Error value
} Uart_Id;

/// \brief Uart parity.
typedef enum
{
    Uart_Parity_Even = 0, ///< Assume even parity bit
    Uart_Parity_Odd = 1,  ///< Assume odd parity bit
    Uart_Parity_None = 4, ///< Assume no parity
    Uart_Parity_Max = 9   ///< Error value
} Uart_Parity;

/// \brief Uart baud rate values.
typedef enum
{
    Uart_BaudRate_300 = 300,       ///< 300 bauds
    Uart_BaudRate_600 = 600,       ///< 600 bauds
    Uart_BaudRate_1200 = 1200,     ///< 1200 bauds
    Uart_BaudRate_1800 = 1800,     ///< 1800 bauds
    Uart_BaudRate_2400 = 2400,     ///< 2400 bauds
    Uart_BaudRate_4800 = 4800,     ///< 4800 bauds
    Uart_BaudRate_9600 = 9600,     ///< 9600 bauds
    Uart_BaudRate_19200 = 19200,   ///< 19200 bauds
    Uart_BaudRate_28800 = 28800,   ///< 28800 bauds
    Uart_BaudRate_38400 = 38400,   ///< 38400 bauds
    Uart_BaudRate_57600 = 57600,   ///< 57600 bauds
    Uart_BaudRate_576800 = 76800,  ///< 76800 bauds
    Uart_BaudRate_115200 = 115200, ///< 115200 bauds
    Uart_BaudRate_Max = 0          ///< Error value
} Uart_BaudRate;

/// \brief Enum representing error codes
typedef enum
{
    Uart_ErrorCode_OK = 0,            ///< No error
    Uart_ErrorCode_Timeout = 1,       ///< Timeout
    Uart_ErrorCode_RxFifoFull = 2,    ///< Rx FIFO full
    Uart_ErrorCode_TxFifoFull = 3,    ///< Tx FIFO full
    Uart_ErrorCode_TxFifoNotNull = 4, ///< Tx FIFO enabled
    Uart_ErrorCode_RxFifoNotNull = 5  ///< Rx FIFO enabled
} Uart_ErrorCode;

/// \brief Uart configuration descriptor.
typedef struct
{
    /// \brief Flag indicating whether the transmitter should be enabled
    bool isTxEnabled;
    /// \brief Flag indicating whether the receiver should be enabled
    bool isRxEnabled;
    /// \brief Flag indicating whether to enable local loopback mode
    bool isTestModeEnabled;
    /// \brief Indicator of used parity bit
    Uart_Parity parity;
    /// \brief Target baud rate
    Uart_BaudRate baudRate;
    /// \brief Indicator of the baud rate clock source
    uint32_t baudRateClkSrc;
    /// \brief Baud rate clock source frequency
    uint32_t baudRateClkFreq;
} Uart_Config;

/// \brief Internal data used by interrupt handler
typedef struct
{
    rtems_interrupt_entry rtemsInterruptEntry; ///< RTEMS interrupt entry
    uint32_t sentBytes;                        ///< Bytes sent in async mode
} Uart_InterruptData;

/// \brief A function serving as a callback called at the end of transmission.
typedef void (*UartTxEndCallback)(void* arg);

/// \brief A descriptor of an end-of-transmission event handler.
typedef struct
{
    UartTxEndCallback callback; ///< Callback function
    void* arg;                  ///< Argument to the callback function
} Uart_TxHandler;

/// \brief A function serving as a callback called upon a reception of a byte
///        if the reception queue contains at least a number of bytes specified
///        in the handler descriptor.
typedef void (*UartRxEndLengthCallback)(void* arg);

/// \brief A function serving as a callback called upon a reception of a byte if
///        byte matches a target specified in the handler descriptor.
typedef void (*UartRxEndCharacterCallback)(void* arg);

/// \brief A descriptor of a byte reception event handler.
typedef struct
{
    /// \brief Callback called when reception queue data count is greater
    /// than or equal targetLength
    UartRxEndLengthCallback lengthCallback;
    /// \brief Callback called when a targetCharacter is received
    UartRxEndCharacterCallback characterCallback;
    /// \brief Argument for the length callback
    void* lengthArg;
    /// \brief Argument for the character callback
    void* characterArg;
    /// \brief Target character, upon reception of which character callback
    /// is called
    uint8_t targetCharacter;
    /// \brief Target length of reception queue, upon reaching of which
    /// length callback is called
    uint32_t targetLength;
} Uart_RxHandler;

/// \brief A function serving as a callback called upon detection of an error by
///        hardware.
typedef void (*UartErrorCallback)(void* arg);

/// \brief A descriptor of an error handler.
typedef struct
{
    UartErrorCallback callback; ///< Callback function
    void* arg;                  ///< Argument to the callback function
} Uart_ErrorHandler;

/// \brief Uart error flags.
typedef struct
{
    bool hasOverrunOccurred;         //< Hardware FIFO overrun detected
    bool hasFramingErrorOccurred;    //< Framing error detected
    bool hasParityErrorOccurred;     //< Parity error detected
    bool hasRxFifoFullErrorOccurred; //< Rx FIFO full error detected
} Uart_ErrorFlags;

/// \brief Uart device descriptor.
typedef struct
{
    Uart_Id id;                     ///< Device identifier
    Uart_TxHandler txHandler;       ///< End-of-transmission handler descriptor
    Uart_RxHandler rxHandler;       ///< Reception handler descriptor
    Uart_ErrorHandler errorHandler; ///< Error handler descriptor
    Uart_ErrorFlags errorFlags;     ///< Error flags
    Uart_InterruptData interruptData; ///< Interrupt handler internal data
    ByteFifo* txFifo;                 ///< Pointer to a transmission byte queue
    ByteFifo* rxFifo;                 ///< Pointer to a reception byte queue
    UartRegisters_t reg; ///< Pointer to memory-mapped device registers
} Uart;

/// \brief Configures an Uart device based on a configuration descriptor.
/// \param [in] uart Uart device descriptor.
/// \param [in] config A configuration descriptor.
void Uart_setConfig(Uart* const uart, const Uart_Config* const config);

/// \brief Retrieves configuration of an Uart device.
/// \param [in] uart Uart device descriptor.
/// \param [out] config A configuration descriptor.
void Uart_getConfig(const Uart* const uart, Uart_Config* const config);

/// \brief Performs a hardware startup procedure of an Uart device.
/// \param [in] uart Uart device descriptor.
void Uart_startup(Uart* const uart);

/// \brief Performs a hardware shutdown procedure of an Uart device.
/// \param [in] uart Uart device descriptor.
void Uart_shutdown(Uart* const uart);

/// \brief Intiializes a device descriptor for Uart.
/// \param [in] id Uart device identifier.
/// \param [out] uart Uart device descriptor.
void Uart_init(Uart_Id id, Uart* const uart);

/// \brief Synchronously sends a byte over Uart.
/// \param [in] uart Uart device descriptor.
/// \param [in] data Byte to send.
/// \param [in] timeoutLimit Timeout value.
/// \param [out] errCode An error code generated during the operation.
/// \retval true Sending was successful.
/// \retval false Sending timed out or failed.
bool Uart_write(Uart* const uart,
                const uint8_t data,
                uint32_t const timeoutLimit,
                Uart_ErrorCode* const errCode);

/// \brief Synchronously receives a byte over Uart.
/// \param [in] uart Uart device descriptor.
/// \param [in] data Received byte pointer.
/// \param [in] timeoutLimit Timeout value.
/// \param [out] errCode An error code generated during the operation.
/// \retval true Reception was successful.
/// \retval false Reception timed out or failed.
bool Uart_read(Uart* const uart,
               uint8_t* data,
               uint32_t timeoutLimit,
               Uart_ErrorCode* const errCode);

/// \brief Asynchronously sends a series of bytes over Uart.
/// \param [in] uart Uart device descriptor.
/// \param [in] fifo Pointer to the output byte queue.
/// \param [in] handler Descriptor of the transmission handler.
void Uart_writeAsync(Uart* const uart,
                     ByteFifo* const fifo,
                     const Uart_TxHandler handler);

/// \brief Asynchronously receives a series of bytes over Uart.
/// \param [in] uart Uart device descriptor.
/// \param [in] fifo Pointer to the input byte queue.
/// \param [in] handler Descriptor of the reception handler.
void Uart_readAsync(Uart* const uart,
                    ByteFifo* const fifo,
                    const Uart_RxHandler handler);

/// \brief Checks if all bytes were sent.
/// \param [in] uart Uart device descriptor.
/// \retval true Tx queue is empty.
/// \retval false Tx is busy.
bool Uart_isTxEmpty(const Uart* const uart);

/// \brief Checks if all bytes were received.
/// \param [in] uart Uart device descriptor.
/// \retval true Rx queue is empty.
/// \retval false Rx is busy.
bool Uart_isRxEmpty(const Uart* const uart);

/// \brief Gets transmission queue byte count.
/// \param [in] uart Uart device descriptor.
/// \returns The number of bytes in the sending queue, yet to be sent out.
uint32_t Uart_getTxFifoCount(Uart* const uart);

/// \brief Gets reception queue byte count.
/// \param [in] uart Uart device descriptor.
/// \returns The number of bytes in the reception queue, waiting to be pulled.
uint32_t Uart_getRxFifoCount(Uart* const uart);

/// \brief Registers a handler called upon detection of a hardware error.
/// \param [in] uart Uart device descriptor.
/// \param [in] handler Error handler descriptor.
void Uart_registerErrorHandler(Uart* const uart,
                               const Uart_ErrorHandler handler);

/// \brief Default interrupt handler for Uart devices.
/// \param [in] arg Uart device descriptor passed directly to RTEMS interrupt
///                  handler
/// \retval true   error handled by interrupt
/// \retval false  no error occured
bool Uart_handleError(Uart* const uart);

/// \brief Default interrupt handler for Uart devices.
/// \param [in] arg Uart device descriptor passed directly to RTEMS interrupt
///                  handler
/// \retval true   received byte handled by interrupt
/// \retval false  no byte received
bool Uart_handleRx(Uart* const uart);

/// \brief Default interrupt handler for Uart devices.
/// \param [in] arg Uart device descriptor passed directly to RTEMS interrupt
///                  handler
/// \retval true   sent byte handled by interrupt
/// \retval false  no byte sent
bool Uart_handleTx(Uart* const uart);

/// \brief Default interrupt handler for Uart devices.
/// \param [in] arg Uart device descriptor passed directly to RTEMS interrupt
///                  handler
void Uart_handleInterrupt(Uart* const uart);

/// \brief Checks status register for hardware errors.
/// \param [in] statusRegister Twihs statrus register value.
/// \param [out] errFlags Pointer to error flag structure.
/// \retval true   hardware errors occured
/// \retval false  no hardware errors
bool Uart_getLinkErrors(uint32_t statusRegister, Uart_ErrorFlags* errFlags);

/// \brief Reads Uart device status register.
/// \param [in] uart Uart device descriptor.
/// \returns The status register value.
uint32_t Uart_getStatusRegister(const Uart* const uart);

#endif // BSP_UART_H

/** @} */

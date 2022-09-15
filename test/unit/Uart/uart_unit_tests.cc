/**@file
 * This file is part of the Leon3 BSP for the Test Environment.
 *
 * @copyright 2022 N7 Space Sp. z o.o.
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

#include "CppUTest/TestHarness.h"

#include <string.h>
#include <stdint.h>

extern "C"
{
#include "Uart.h"
}

TEST_GROUP(UartTests)
{
    Uart uart;
    Uart_Config config;

    void setup() {
      Uart_init(Uart_Id_0, &uart);
    }
};

TEST(UartTests, Uart_setFlag_ShouldSetProperFlagTo32BitRegister)
{
    volatile uint32_t testRegister = 0;
    uint32_t flagOffset = 2;
    uint32_t properSetResult = 4;
    uint32_t properResetResult = 0;

    Uart_setFlag(&testRegister, UART_FLAG_SET, flagOffset);
    CHECK_EQUAL(properSetResult, testRegister);
    Uart_setFlag(&testRegister, UART_FLAG_RESET, flagOffset);
    CHECK_EQUAL(properResetResult, testRegister);
}

TEST(UartTests, Uart_getFlag_ShouldReturnProperFlagFrom32BitRegister)
{
    volatile uint32_t testRegister = 0b11010;

    CHECK_TRUE(Uart_getFlag(testRegister, 1));
    CHECK_TRUE(Uart_getFlag(testRegister, 3));
    CHECK_TRUE(Uart_getFlag(testRegister, 4));
    CHECK_FALSE(Uart_getFlag(testRegister, 0));
    CHECK_FALSE(Uart_getFlag(testRegister, 2));
}

TEST(UartTests, Uart_setConfig_ShouldWriteProperUartConfigurationToTheControlRegister)
{
    uint32_t expectedValue = 0x3F; // RE, TE, RI, TI, PS, PE

    config.isRxEnabled = true;
    config.isTxEnabled = true;
    config.parity = Uart_Parity_Odd;
    config.isLoopbackModeEnabled = false;

    CHECK_EQUAL(0, uart.reg->control);
    Uart_setConfig(&uart, &config);
    CHECK_EQUAL(expectedValue, uart.reg->control);
}

TEST(UartTests, Uart_getConfig_ShouldReadProperUartConfigurationFromTheControlRegister)
{
    uart.reg->control = 0x8A; // TE, TI, LB

    Uart_getConfig(&uart, &config);
    CHECK_FALSE(config.isRxEnabled);
    CHECK_TRUE(config.isTxEnabled);
    CHECK_TRUE(config.isLoopbackModeEnabled);
    CHECK_TRUE(config.parity == Uart_Parity_None);
}

TEST(UartTests, Uart_shutdown_ShouldClearStatusAndControlRegisterAndResetErrorFlags)
{
    uart.reg->control = 0x8A; // TE, TI, LB
    uart.reg->status = 0x1F;  // DR, TS, TE, BR, OV

    Uart_shutdown(&uart);

    CHECK_EQUAL(0, uart.reg->control);
    CHECK_EQUAL(0, uart.reg->status);
    CHECK_EQUAL(0, uart.interruptData.sentBytes);
}

TEST(UartTests, Uart_handleError_ShouldReturnTrueWhileAnyOfErrorFlagsAreSet)
{
    CHECK_FALSE(Uart_handleError(&uart));

    uart.reg->status = 0x10u;             // OV
    CHECK_TRUE(Uart_handleError(&uart));
    uart.reg->status = 0x20u;             // PE
    CHECK_TRUE(Uart_handleError(&uart));
    uart.reg->status = 0x40u;             // FE
    CHECK_TRUE(Uart_handleError(&uart));
    uart.reg->status = 0x400u;            // RF
    CHECK_TRUE(Uart_handleError(&uart));
    uart.reg->status = 0x00u;             // RF
    CHECK_FALSE(Uart_handleError(&uart));
}

TEST(UartTests, Uart_getRxFifoCount_ShouldReturnCorrectSizeOfTheBytesReadyToRead)
{
    BYTE_FIFO_CREATE(rxEmptyByteFifo, 1);
    BYTE_FIFO_CREATE_FILLED(rxFilledByteFifo, "Test string");
    size_t stringLen = strlen("Test string") + 1; // BYTE_FIFO adds 1 byte to the string
    uart.rxFifo = &rxEmptyByteFifo;

    CHECK_EQUAL(0, Uart_getRxFifoCount(&uart));
    uart.rxFifo = &rxFilledByteFifo;
    CHECK_EQUAL(stringLen, Uart_getRxFifoCount(&uart));
}

TEST(UartTests, Uart_getTxFifoCount_ShouldReturnCorrectSizeOfTheBytesReadyToSend)
{
    BYTE_FIFO_CREATE(txEmptyByteFifo, 1);
    BYTE_FIFO_CREATE_FILLED(txFilledByteFifo, "Test string");
    size_t stringLen = strlen("Test string") + 1; // BYTE_FIFO adds 1 byte to the string
    uart.txFifo = &txEmptyByteFifo;

    CHECK_EQUAL(0, Uart_getTxFifoCount(&uart));
    uart.txFifo = &txFilledByteFifo;
    CHECK_EQUAL(stringLen, Uart_getTxFifoCount(&uart));
}

TEST(UartTests, Uart_isRxEmpty_ShouldReturnIfReadFifoIsEmpty)
{
    BYTE_FIFO_CREATE(rxEmptyByteFifo, 1);
    BYTE_FIFO_CREATE_FILLED(rxFilledByteFifo, "Test string");
    uart.rxFifo = &rxEmptyByteFifo;

    CHECK_TRUE(Uart_isRxEmpty(&uart));
    uart.rxFifo = &rxFilledByteFifo;
    CHECK_FALSE(Uart_isRxEmpty(&uart));
}

TEST(UartTests, Uart_isTxEmpty_ShouldReturnIfTransmitFifoIsEmpty)
{
    BYTE_FIFO_CREATE(txEmptyByteFifo, 1);
    BYTE_FIFO_CREATE_FILLED(txFilledByteFifo, "Test string");
    uart.txFifo = &txEmptyByteFifo;

    CHECK_TRUE(Uart_isTxEmpty(&uart));
    uart.txFifo = &txFilledByteFifo;
    CHECK_FALSE(Uart_isTxEmpty(&uart));
}

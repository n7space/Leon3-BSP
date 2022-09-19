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

/// \brief Run functionality tests

#include <stdbool.h>
#include <string.h>
#include "SystemConfig.h"
#include "Uart.h"
#include <rtems.h>
#include <rtems/confdefs.h>
#include <rtems/bspIo.h>

#define SUCCESS_LENGTH 8
#define FAILED_LENGHT 7
#define TIMEOUT 10000

static Uart uart0;

static volatile bool charReceived;
static volatile bool lengthReceived;

static void
testLengthCallback(bool* result)
{
    *result = true;
}

static void
testCharacterCallback(bool* result)
{
    *result = true;
}

static Uart_RxHandler testHandler = {
    .lengthCallback = (UartRxEndLengthCallback)testLengthCallback,
    .characterCallback = (UartRxEndCharacterCallback)testCharacterCallback,
    .lengthArg = &lengthReceived,
    .characterArg = &charReceived,
    .targetLength = 20,
    .targetCharacter = 'q'
};

BYTE_FIFO_CREATE_FILLED(txByteFifoForReadTest, "Read text (async): q");
BYTE_FIFO_CREATE_FILLED(successByteFifo, "Success");
BYTE_FIFO_CREATE(rxByteFifoForReadTest, 40);


bool
test_Uart_readAsync(Uart* uart)
{
    bool result = false;

    Uart_init(Uart_Id_0, uart);
    Uart_Config config = (Uart_Config){ 0 };
    config.isTxEnabled = true;
    config.isRxEnabled = true;
    config.isLoopbackModeEnabled = true;
    Uart_setConfig(uart, &config);
    Uart_startup(uart);
    charReceived = false;
    lengthReceived = false;
    ByteFifo_clear(&rxByteFifoForReadTest);
    Uart_writeAsync(uart, &txByteFifoForReadTest, uart->txHandler);
    Uart_readAsync(uart, &rxByteFifoForReadTest, testHandler);
    while (!ByteFifo_isFull(&rxByteFifoForReadTest)) {
        if (lengthReceived == true || charReceived == true) {
            Uart_writeAsync(uart, &successByteFifo, uart->txHandler);
            while (!ByteFifo_isEmpty(&successByteFifo));
            result = true;
            break;
        }
    }
    Uart_shutdown(uart);
    
    return result;
}

rtems_task
Init(rtems_task_argument arg)
{
    (void)arg;
    rtems_fatal(RTEMS_FATAL_SOURCE_EXIT, test_Uart_readAsync(&uart0));
}

/** @} */

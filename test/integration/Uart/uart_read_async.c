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

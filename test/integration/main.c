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
#include "SystemConfig.h"
#include "Uart.h"
#include <rtems.h>
#include <rtems/confdefs.h>

static Uart uart0;

static volatile bool dataSent;

static void
testCallback(bool* result)
{
    *result = true;
}

static const Uart_TxHandler testHandler = {
    .callback = (UartTxEndCallback)testCallback,
    .arg = &dataSent
};

BYTE_FIFO_CREATE_FILLED(txByteFifoForWriteTest, "Write text (async)\r\n");

bool
test_Uart_writeAsync(Uart* uart)
{
    Uart_init(Uart_Id_0, uart);
    Uart_Config config = (Uart_Config){ 0 };
    config.isTxEnabled = 1;
    config.isRxEnabled = 1;
    Uart_setConfig(uart, &config);
    Uart_startup(uart);
    dataSent = false;
    Uart_writeAsync(uart, &txByteFifoForWriteTest, testHandler);
    for(int timeout = 0; timeout < 4000; timeout++) {
        if(dataSent) {
            Uart_shutdown(uart);
            return true;
        }
    }
    Uart_shutdown(uart);
    return false;
}


rtems_task
Init(rtems_task_argument arg)
{
    (void)arg;
    rtems_fatal(RTEMS_FATAL_SOURCE_EXIT, test_Uart_writeAsync(&uart0));
}

/** @} */
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


#define WRITE_TIMEOUT 100000
#define SUCCESS_LENGTH 8
#define FAILED_LENGHT 7

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
BYTE_FIFO_CREATE_FILLED(successByteFifo, "Success");

bool
test_Uart_writeAsync(Uart* uart)
{
    bool result = false;

    Uart_init(Uart_Id_0, uart);
    Uart_Config config = (Uart_Config){ 0 };
    config.isTxEnabled = true;
    config.isRxEnabled = true;
    Uart_setConfig(uart, &config);
    Uart_startup(uart);
    dataSent = false;
    Uart_writeAsync(uart, &txByteFifoForWriteTest, testHandler);
    for (int timeout = 0; timeout < WRITE_TIMEOUT; timeout++) {
        if (dataSent) {
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
    rtems_fatal(RTEMS_FATAL_SOURCE_EXIT, test_Uart_writeAsync(&uart0));
}

/** @} */

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
#define WRITE_TIMEOUT 100000

static Uart uart0;

static Uart_ErrorCode errCode = Uart_ErrorCode_OK;
static uint8_t data[] = "Write text (sync)\r\n";

void sendMsg (const char *msg, int size)
{
    for (size_t i = 0; i < size; i++) {
      Uart_write(&uart0, msg[i], WRITE_TIMEOUT, &errCode);
    }
}

bool
test_Uart_write(Uart* uart)
{
    bool result = false;

    Uart_init(Uart_Id_0, uart);
    Uart_Config config = (Uart_Config){0};
    config.isTxEnabled = true;
    config.isRxEnabled = true;
    Uart_setConfig(uart, &config);
    Uart_startup(uart);
    for (int i = 0; i < 20 && data[i] != 0; i++) {
        Uart_write(uart, data[i], WRITE_TIMEOUT, &errCode);
    }
    
    if (errCode == 0) {
        sendMsg("Success\n", SUCCESS_LENGTH);
        result = true;
    }
    Uart_shutdown(uart);

    return result;
}

rtems_task
Init(rtems_task_argument arg)
{
    (void)arg;
    rtems_fatal(RTEMS_FATAL_SOURCE_EXIT, test_Uart_write(&uart0));
}

/** @} */

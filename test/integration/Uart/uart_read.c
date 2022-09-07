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

#define TIMEOUT 100000
#define SUCCESS_LENGTH 7

static Uart uart0;

static Uart_ErrorCode errCode = Uart_ErrorCode_OK;
static uint8_t text[] = "Read text (sync) - data will be read until 'q' chars are received.";

void sendMsg (const char *msg, int size)
{
  for (size_t i = 0; i < size; i++) {
    Uart_write(&uart0, msg[i], TIMEOUT, &errCode);
  }
}

bool
test_Uart_read(Uart* uart)
{
    bool result = false;

    Uart_init(Uart_Id_0, uart);
    Uart_Config config = (Uart_Config){ 0 };
    config.isRxEnabled = true;
    config.isTxEnabled = true;
    config.isLoopbackModeEnabled = true;
    Uart_setConfig(uart, &config);
    Uart_startup(uart);
    uint8_t buf = '\0';
    for (int i = 0; i < 80 && text[i] != 0; i++) {
        Uart_write(uart, text[i], TIMEOUT, &errCode);
        Uart_read(uart, &buf, TIMEOUT, &errCode);
        if (buf == 'q') {
            break;
        }
    }
    if (errCode == Uart_ErrorCode_OK) {
        sendMsg("Success", SUCCESS_LENGTH);
        result = true;
    }
    Uart_shutdown(uart);

    return result;
}

rtems_task
Init(rtems_task_argument arg)
{
    (void)arg;
    rtems_fatal(RTEMS_FATAL_SOURCE_EXIT, test_Uart_read(&uart0));
}

/** @} */

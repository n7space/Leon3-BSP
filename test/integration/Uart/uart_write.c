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

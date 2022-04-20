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
#include "function_tests/Uart/test_Uart_init.h"
#include "function_tests/Uart/test_Uart_write.h"
#include "function_tests/Uart/test_Uart_read.h"
#include "function_tests/Uart/test_Uart_writeAsync.h"
#include "function_tests/Uart/test_Uart_readAsync.h"
#include "function_tests/Uart/test_Uart_setBaud9600.h"
#include "function_tests/Uart/test_Uart_setBaud19200.h"
#include <rtems.h>
#include <rtems/confdefs.h>

static Uart uart0;

int
main()
{
    if(!test_Uart_init(Uart_Id_0, &uart0)) {
        return -1;
    }
    if(!test_Uart_write(&uart0)) {
        return -1;
    }
    if(!test_Uart_writeAsync(&uart0)) {
        return -1;
    }
    if(!test_Uart_read(&uart0)) {
        return -1;
    }
    if(!test_Uart_readAsync(&uart0)) {
        return -1;
    }
    if(!test_Uart_setBaud9600(&uart0)) {
        return -1;
    }
    if(!test_Uart_setBaud19200(&uart0)) {
        return -1;
    }
    return 0;
}

rtems_task
Init(rtems_task_argument arg)
{
    (void)arg;
    rtems_fatal(RTEMS_FATAL_SOURCE_EXIT, main());
}

/** @} */

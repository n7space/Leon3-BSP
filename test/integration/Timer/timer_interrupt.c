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
#include "Timer.h"
#include <rtems.h>
#include <rtems/confdefs.h>
#include <rtems/bspIo.h>

#define TIMEOUT 100000
#define SUCCESS_LENGTH 7
#define SCALER_VALUE 1
#define RELOAD_VALUE 100

static Timer_Apbctrl2 timer;

static volatile bool isFinished;

void sendMsg (const char *msg, int size)
{
  for (size_t i = 0; i < size; i++) {
    rtems_putc(msg[i]);
  }
}

static inline void
irqCallback(bool* arg)
{
    *arg = true;
}

static Timer_InterruptHandler irqHandler = {
  .callback = (Timer_InterruptCallback)irqCallback,
  .arg = &isFinished
};

bool
test_Timer_interrupt(Timer_Apbctrl2* timer)
{
    bool result = false;
    isFinished = false;

    Timer_Config config;
    config.isEnabled = true;
    config.isAutoReloaded = false;
    config.isInterruptEnabled = true;
    config.isChained = false;
    config.reloadValue = RELOAD_VALUE;

    Timer_Apbctrl2_init(Timer_Id_1, timer, irqHandler);
    Timer_Apbctrl2_setBaseScalerReloadValue(timer, SCALER_VALUE);
    Timer_Apbctrl2_setConfigRegisters(timer, &config);
    Timer_Apbctrl2_start(timer);

    int i = 0;
    while (i++ < TIMEOUT) {
      if (isFinished) {
        result = true;
        break;
      }
    }

    Timer_Apbctrl2_stop(timer);
    Timer_Apbctrl2_shutdown(timer);

    if (result == true) {
      sendMsg("Success", SUCCESS_LENGTH);
    }

    return result;
}

rtems_task
Init(rtems_task_argument arg)
{
    (void)arg;
    rtems_fatal(RTEMS_FATAL_SOURCE_EXIT, test_Timer_interrupt(&timer));
}

/** @} */

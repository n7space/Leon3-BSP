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

#include <stdbool.h>
#include <stdint.h>
#include "TimerRegisters.h"
#include "Timer.h"
#include "SystemConfig.h"

static TimerBaseRegisters_t baseRegisters =
        (TimerBaseRegisters_t)GPTIMER_ADDRESS_BASE;

static inline TimerRegisters_t
getAddress(Timer_Id id)
{
    switch(id) {
        case Timer_Id_1:
            return (TimerRegisters_t)Timer1_address;
        case Timer_Id_2:
            return (TimerRegisters_t)Timer2_address;
        case Timer_Id_3:
            return (TimerRegisters_t)Timer3_address;
        case Timer_Id_4:
            return (TimerRegisters_t)Timer4_address;
        default:
            return (TimerRegisters_t)TimerMax_address;
    }
}

static inline Timer_Id
getTimerId(uint32_t address)
{
    if(address == Timer1_address) {
        return Timer_Id_1;
    }
    if(address == Timer2_address) {
        return Timer_Id_2;
    }
    if(address == Timer3_address) {
        return Timer_Id_3;
    }
    if(address == Timer4_address) {
        return Timer_Id_4;
    }
    return Timer_Id_Max;
}

void
Timer_setBaseScalerReloadValue(uint32_t scaler)
{
    baseRegisters->reload = scaler;
}

void
Timer_setConfig(Timer* const timer, const Timer_Config* const config)
{
    timer->id = getTimerId(config->clockSource);
    timer->regs = getAddress(timer->id);
    if(config->isEnabled) {
        timer->regs->control |= TIMER_CONTROL_EN;
    } else {
        timer->regs->control &= ~TIMER_CONTROL_EN;
    }
    if(config->isInterruptEnabled) {
        timer->regs->control |= TIMER_CONTROL_IE;
    } else {
        timer->regs->control &= ~TIMER_CONTROL_IE;
    }
    if(config->isAutoReloaded) {
        timer->regs->control |= TIMER_CONTROL_RS;
    } else {
        timer->regs->control &= ~TIMER_CONTROL_RS;
    }
    timer->regs->reload = config->reloadValue;
}

void
Timer_getConfig(const Timer* const timer, Timer_Config* const config)
{
    config->clockSource = (uint32_t)getAddress(timer->id);
    if((timer->regs->control & TIMER_CONTROL_EN) != 0) {
        config->isEnabled = true;
    } else {
        config->isEnabled = false;
    }
    if((timer->regs->control & TIMER_CONTROL_IE) != 0) {
        config->isInterruptEnabled = true;
    } else {
        config->isInterruptEnabled = false;
    }
    if((timer->regs->control & TIMER_CONTROL_RS) != 0) {
        config->isAutoReloaded = true;
    } else {
        config->isAutoReloaded = false;
    }
    config->reloadValue = timer->regs->reload;
}

void
Timer_startup(Timer* const timer)
{
    timer->regs->control |= TIMER_CONTROL_LD;
    timer->regs->control |= TIMER_CONTROL_IE;
    timer->regs->control |= TIMER_CONTROL_EN;
}

void
Timer_shutdown(Timer* const timer)
{
    timer->regs->control &= ~TIMER_CONTROL_IE;
    timer->regs->control &= ~TIMER_CONTROL_EN;
}

void
Timer_init(Timer_Id id, Timer* const timer)
{
    baseRegisters->configuration |= TIMER_CONFIGURATION_DF;
    baseRegisters->configuration |= TIMER_CONFIGURATION_SI;
    timer->id = id;
    timer->regs = getAddress(id);
}

void
Timer_clearCurrentValue(Timer* const timer)
{
    timer->regs->control &= ~TIMER_CONTROL_IP;
    timer->regs->control |= TIMER_CONTROL_LD;
}

uint32_t
Timer_getCurrentValue(const Timer* const timer)
{
    return timer->regs->counter;
}

bool
Timer_hasCountedToZero(const Timer* const timer)
{
    return ((timer->regs->control & TIMER_CONTROL_IP) != 0);
}

/// \brief Timer hardware driver implementation.

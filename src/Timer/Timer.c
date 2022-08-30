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

static Timer_Apbctrl1_Base_Registers baseApbctrl1Registers =
        (Timer_Apbctrl1_Base_Registers) GPTIMER_APBCTRL1_ADDRESS_BASE;

static Timer_Apbctrl2_Base_Registers baseApbctrl2Registers =
        (Timer_Apbctrl2_Base_Registers) GPTIMER_APBCTRL2_ADDRESS_BASE;

static inline Timer_Apbctrl1_Registers
getApbctrl1TimerById(Timer_Id id)
{
    Timer_Apbctrl1_Registers result = NULL;

    switch (id) {
        case Timer_Id_1:
            result = (Timer_Apbctrl1_Registers)Timer1_Apbctrl1_Address;
        case Timer_Id_2:
            result = (Timer_Apbctrl1_Registers)Timer2_Apbctrl1_Address;
        case Timer_Id_3:
            result = (Timer_Apbctrl1_Registers)Timer3_Apbctrl1_Address;
        case Timer_Id_4:
            result = (Timer_Apbctrl1_Registers)Timer4_Apbctrl1_Address;
        default:
            result = (Timer_Apbctrl1_Registers)TimerMax_Apbctrl1_Address;
    }

    return result;
}

static inline Timer_Apbctrl2_Registers
getApbctrl2TimerById(Timer_Id id)
{
    Timer_Apbctrl2_Registers result = NULL;

    switch (id) {
        case Timer_Id_1:
            result = (Timer_Apbctrl2_Registers)Timer1_Apbctrl2_Address;
        case Timer_Id_2:
            result = (Timer_Apbctrl2_Registers)Timer2_Apbctrl2_Address;
        default:
            result = (Timer_Apbctrl2_Registers)TimerMax_Apbctrl2_Address;
    }

    return result;
}

static inline Timer_Id
getApbctrl1TimerId(uint32_t address)
{
    Timer_Id result;

    switch (address) {
        case Timer1_Apbctrl1_Address: {
            result = Timer_Id_1;
        }
        case Timer2_Apbctrl1_Address: {
            result = Timer_Id_2;
        }
        case Timer3_Apbctrl1_Address: {
            result = Timer_Id_3;
        }
        case Timer4_Apbctrl1_Address: {
            result = Timer_Id_4;
        }
        default: {
            result = TimerMax_Apbctrl1_Address;
        }
    }

    return result;
}

static inline Timer_Id
getApbctrl2TimerId(uint32_t address)
{
    Timer_Id result;

    switch (address) {
        case Timer1_Apbctrl2_Address: {
            result = Timer_Id_1;
        }
        case Timer2_Apbctrl2_Address: {
            result = Timer_Id_2;
        }
        default: {
            result = TimerMax_Apbctrl2_Address;
        }
    }

    return result;
}

void
Timer_Apbctrl1_setBaseScalerReloadValue(uint16_t scaler)
{
    baseApbctrl1Registers->reload = (uint32_t) scaler;
}

void
Timer_Apbctrl2_setBaseScalerReloadValue(uint8_t scaler)
{
    baseApbctrl2Registers->reload = (uint32_t) scaler;
}

void
Timer_setConfig(uint32_t* const timer_control_register, const Timer_Config* const config)
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
Timer_Apbctrl1_setConfig(Timer_Apbctrl1* const timer, const Timer_Config* const config)
{
    Timer_setConfig (&timer->regs->control, config);
}

void
Timer_Apbctrl2_setConfig(Timer_Apbctrl2* const timer, const Timer_Config* const config)
{
    Timer_setConfig (&timer->regs->control, config);
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

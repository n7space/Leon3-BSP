/**@file
 * This file is part of the TASTE Leon3 BSP for the Test Environment.
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

#include "Timer.h"

Timer_Apbctrl1_Base_Registers baseApbctrl1Registers =
        (Timer_Apbctrl1_Base_Registers) GPTIMER_APBCTRL1_ADDRESS_BASE;

Timer_Apbctrl2_Base_Registers baseApbctrl2Registers =
        (Timer_Apbctrl2_Base_Registers) GPTIMER_APBCTRL2_ADDRESS_BASE;

void
Timer_baseInit(volatile uint32_t *const baseConfigurationRegister)
{
    Timer_setFlag (baseConfigurationRegister, FLAG_SET, DF);
    Timer_setFlag (baseConfigurationRegister, FLAG_SET, SI);
}

void
Timer_setConfig(volatile uint32_t *const timerControlRegister, volatile uint32_t *const timerReloadRegister, const Timer_Config *const config)
{
    Timer_setFlag (timerControlRegister, config->isEnabled, EN);
    Timer_setFlag (timerControlRegister, config->isInterruptEnabled, IE);
    Timer_setFlag (timerControlRegister, config->isAutoReloaded, RS);
    Timer_setFlag (timerControlRegister, config->isChained, CH);
    *timerReloadRegister = config->reloadValue;
}

void
Timer_getConfig(const uint32_t timerControlRegister, const uint32_t timerReloadRegister, Timer_Config *const config)
{
    config->isEnabled = Timer_getFlag (timerControlRegister, EN);
    config->isInterruptEnabled = Timer_getFlag (timerControlRegister, IE);
    config->isAutoReloaded = Timer_getFlag (timerControlRegister, RS);
    config->isChained = Timer_getFlag (timerControlRegister, CH);
    config->reloadValue = timerReloadRegister;
}

void
Timer_start(volatile uint32_t *const timerControlRegister)
{
    Timer_setFlag (timerControlRegister, FLAG_SET, IE);
    Timer_setFlag (timerControlRegister, FLAG_SET, EN);
}

void
Timer_restart(volatile uint32_t *const timerControlRegister)
{
    Timer_setFlag (timerControlRegister, FLAG_SET, LD);
    Timer_start (timerControlRegister);
}

void
Timer_stop(volatile uint32_t *const timerControlRegister)
{
    Timer_setFlag (timerControlRegister, FLAG_RESET, IE);
    Timer_setFlag (timerControlRegister, FLAG_RESET, EN);
}

bool
Timer_hasFinished(const uint32_t timerControlRegister, const uint32_t timerCounterRegister) {
    bool result = false;
    if (!Timer_getFlag (timerControlRegister, EN) && timerCounterRegister == TIMER_UNDERFLOWED) {
        result = true;
    }
    
    return result;
}

static inline Timer_Apbctrl1_Registers
getApbctrl1TimerAddressById(Timer_Id id)
{
    Timer_Apbctrl1_Registers result = NULL;

    switch (id) {
        case Timer_Id_1:
            result = (Timer_Apbctrl1_Registers) Timer1_Apbctrl1_Address;
            break;
        case Timer_Id_2:
            result = (Timer_Apbctrl1_Registers) Timer2_Apbctrl1_Address;
            break;
        case Timer_Id_3:
            result = (Timer_Apbctrl1_Registers) Timer3_Apbctrl1_Address;
            break;
        case Timer_Id_4:
            result = (Timer_Apbctrl1_Registers) Timer4_Apbctrl1_Address;
            break;
        default:
            result = (Timer_Apbctrl1_Registers) TimerInvalid_Apbctrl1_Address;
    }

    return result;
}

void
Timer_Apbctrl1_init(Timer_Id id, Timer_Apbctrl1 *const timer)
{
    Timer_baseInit (&baseApbctrl1Registers->configuration);
    timer->id = id;
    timer->regs = getApbctrl1TimerAddressById (id);
}

void
Timer_Apbctrl1_setBaseScalerReloadValue(uint16_t scalerReloadValue)
{
    baseApbctrl1Registers->reload = (uint32_t) scalerReloadValue;
}

void
Timer_Apbctrl1_setConfig(Timer_Apbctrl1 *const timer, const Timer_Config *const config)
{
    timer->regs = getApbctrl1TimerAddressById (timer->id);
    Timer_setConfig (&timer->regs->control, &timer->regs->reload, config);
}

void
Timer_Apbctrl1_getConfig(const Timer_Apbctrl1 *const timer, Timer_Config *const config)
{
    Timer_getConfig (timer->regs->control, timer->regs->reload, config);
}

void
Timer_Apbctrl1_start(Timer_Apbctrl1* const timer)
{
    Timer_start (&timer->regs->control);
}

void
Timer_Apbctrl1_restart(Timer_Apbctrl1 *const timer)
{
    Timer_restart (&timer->regs->control);
}

void
Timer_Apbctrl1_stop(Timer_Apbctrl1 *const timer)
{
    Timer_stop (&timer->regs->control);
}

uint32_t
Timer_Apbctrl1_getCounterValue(const Timer_Apbctrl1 *const timer)
{
    return timer->regs->counter;
}

bool
Timer_Apbctrl1_hasFinished(const Timer_Apbctrl1 *const timer)
{
    return Timer_hasFinished (timer->regs->control, timer->regs->counter);
}

static inline Timer_Apbctrl2_Registers
getApbctrl2TimerAddressById(Timer_Id id)
{
    Timer_Apbctrl2_Registers result = NULL;

    switch (id) {
        case Timer_Id_1:
            result = (Timer_Apbctrl2_Registers) Timer1_Apbctrl2_Address;
            break;
        case Timer_Id_2:
            result = (Timer_Apbctrl2_Registers) Timer2_Apbctrl2_Address;
            break;
        default:
            result = (Timer_Apbctrl2_Registers) TimerInvalid_Apbctrl2_Address;
    }

    return result;
}

void
Timer_Apbctrl2_init(Timer_Id id, Timer_Apbctrl2 *const timer)
{
    Timer_baseInit (&baseApbctrl2Registers->configuration);
    timer->id = id;
    timer->regs = getApbctrl2TimerAddressById (id);
}

void
Timer_Apbctrl2_setBaseScalerReloadValue(uint8_t scalerReloadValue)
{
    baseApbctrl2Registers->reload = (uint32_t) scalerReloadValue;
}

void
Timer_Apbctrl2_setConfig(Timer_Apbctrl2 *const timer, const Timer_Config *const config)
{
    timer->regs = getApbctrl2TimerAddressById (timer->id);
    Timer_setConfig (&timer->regs->control, &timer->regs->reload, config);
}

void
Timer_Apbctrl2_getConfig(const Timer_Apbctrl2 *const timer, Timer_Config *const config)
{
    Timer_getConfig (timer->regs->control, timer->regs->reload, config);
}

void
Timer_Apbctrl2_start(Timer_Apbctrl2 *const timer)
{
    Timer_start (&timer->regs->control);
}

void
Timer_Apbctrl2_restart(Timer_Apbctrl2 *const timer)
{
    Timer_restart (&timer->regs->control);
}

void
Timer_Apbctrl2_stop(Timer_Apbctrl2 *const timer)
{
    Timer_stop (&timer->regs->control);
}

uint32_t
Timer_Apbctrl2_getCounterValue(const Timer_Apbctrl2 *const timer)
{
    return timer->regs->counter;
}

bool
Timer_Apbctrl2_hasFinished(const Timer_Apbctrl2 *const timer)
{
    return Timer_hasFinished (timer->regs->control, timer->regs->counter);
}

bool
Timer_getFlag(const uint32_t timerRegister, const uint32_t flag)
{
    return (bool) ((timerRegister >> flag) & FLAG_MASK);
}

void
Timer_setFlag(volatile uint32_t *const timerRegister, const bool set, const uint32_t flag)
{
    if (set) {
        *timerRegister |= (FLAG_MASK << flag);
    } else {
        *timerRegister &= ~(FLAG_MASK << flag);
    }
}

/// \brief Timer hardware driver implementation.

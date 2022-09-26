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

#include "Timer_private.h"

void
Timer_baseInit(volatile uint32_t *const baseConfigurationRegister)
{
    Timer_setFlag(baseConfigurationRegister, TIMER_FLAG_SET, TIMER_CONFIG_DF);
    Timer_setFlag(baseConfigurationRegister, TIMER_FLAG_SET, TIMER_CONFIG_SI);
}

void
Timer_setConfigRegisters(volatile uint32_t *const timerControlRegister, volatile uint32_t *const timerReloadRegister, const Timer_Config *const config)
{
    Timer_setFlag(timerControlRegister, config->isEnabled, TIMER_CONTROL_EN);
    Timer_setFlag(timerControlRegister, config->isInterruptEnabled, TIMER_CONTROL_IE);
    Timer_setFlag(timerControlRegister, config->isAutoReloaded, TIMER_CONTROL_RS);
    Timer_setFlag(timerControlRegister, config->isChained, TIMER_CONTROL_CH);
    *timerReloadRegister = config->reloadValue;
}

void
Timer_getConfigRegisters(const uint32_t timerControlRegister, const uint32_t timerReloadRegister, Timer_Config *const config)
{
    config->isEnabled = Timer_getFlag(timerControlRegister, TIMER_CONTROL_EN);
    config->isInterruptEnabled = Timer_getFlag(timerControlRegister, TIMER_CONTROL_IE);
    config->isAutoReloaded = Timer_getFlag(timerControlRegister, TIMER_CONTROL_RS);
    config->isChained = Timer_getFlag(timerControlRegister, TIMER_CONTROL_CH);
    config->reloadValue = timerReloadRegister;
}

void
Timer_start(volatile uint32_t *const timerControlRegister)
{
    Timer_setFlag(timerControlRegister, TIMER_FLAG_SET, TIMER_CONTROL_LD);
    Timer_setFlag(timerControlRegister, TIMER_FLAG_SET, TIMER_CONTROL_EN);
}

void
Timer_stop(volatile uint32_t *const timerControlRegister)
{
    Timer_setFlag(timerControlRegister, TIMER_FLAG_RESET, TIMER_CONTROL_EN);
}

bool
Timer_hasFinished(const uint32_t timerControlRegister, const uint32_t timerCounterRegister) {
    bool result = false;
    if (!Timer_getFlag(timerControlRegister, TIMER_CONTROL_EN) && timerCounterRegister == TIMER_UNDERFLOWED) {
        result = true;
    }
    
    return result;
}

void
irqInit(rtems_interrupt_entry *entry, Timer_InterruptHandler *handler, const uint8_t irqNumber)
{
    rtems_interrupt_clear(irqNumber);
    rtems_interrupt_entry_initialize(
            entry,
            (rtems_interrupt_handler)Timer_handleIrq,
            handler,
            "Timer Interrupt");
    rtems_interrupt_entry_install(
        irqNumber,
        RTEMS_INTERRUPT_UNIQUE,
        entry);
    rtems_interrupt_vector_enable(irqNumber);
}

void
irqDeinit(rtems_interrupt_entry *entry, const uint8_t irqNumber)
{
    rtems_interrupt_vector_disable(irqNumber);
    rtems_interrupt_entry_remove(irqNumber, entry);
}

void Timer_handleIrq(Timer_InterruptHandler* const handler)
{
    handler->callback(handler->arg);
}

bool
Timer_getFlag(const uint32_t timerRegister, const uint32_t flag)
{
    return (bool) ((timerRegister >> flag) & TIMER_FLAG_MASK);
}

void
Timer_setFlag(volatile uint32_t *const timerRegister, const bool isSet, const uint32_t flag)
{
    if (isSet) {
        *timerRegister |= (TIMER_FLAG_MASK << flag);
    } else {
        *timerRegister &= ~(TIMER_FLAG_MASK << flag);
    }
}

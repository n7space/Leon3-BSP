/**@file
 * This file is part of the Leon3 BSP for the Test Environment.
 *
 * @copyright 2022 N7 Space Sp. z o.o.
 * 
 * Leon3 BSP for the Test Environment was developed under the project AURORA.
 * This project has received funding from the European Union’s Horizon 2020
 * research and innovation programme under grant agreement No 101004291”
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

#include "Timer.h"
#include "Timer_private.h"

static inline void
emptyCallback(volatile void* arg)
{
    (void)arg;
}

Timer_InterruptHandler defaultInterruptHandler = {.callback = emptyCallback, .arg = 0};

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
Timer_Apbctrl1_init(Timer_Id id, Timer_Apbctrl1 *const timer, const Timer_InterruptHandler handler)
{
#ifdef MOCK_REGISTERS
    if (timer->base == NULL) {
        timer->base = malloc(sizeof(Timer_Apbctrl1_Base_Registers));
    }
    timer->regs = malloc(sizeof(Timer_Apbctrl1_Registers));
#else
    timer->base = (Timer_Apbctrl1_Base_Registers) GPTIMER_APBCTRL1_ADDRESS_BASE;
    timer->regs = getApbctrl1TimerAddressById(id);
#endif
    Timer_baseInit(&timer->base->configuration);
    timer->id = id;
    timer->irqHandler = handler;
    timer->rtemsInterruptEntry = (rtems_interrupt_entry){0};
    timer->regs->control = 0;
    timer->regs->counter = 0;
    timer->regs->reload = 0;

    uint8_t irqNumber = Timer_getApbctrl1InterruptNumber(timer->id);
    irqInit(&timer->rtemsInterruptEntry, &timer->irqHandler, irqNumber);
}

void
Timer_Apbctrl1_setBaseScalerReloadValue(Timer_Apbctrl1 *const timer, uint16_t scalerReloadValue)
{
    timer->base->reload = (uint32_t) scalerReloadValue;
    timer->base->scaler = (uint32_t) scalerReloadValue;
}

void
Timer_Apbctrl1_setConfigRegisters(Timer_Apbctrl1 *const timer, const Timer_Config *const config)
{
    rtems_interrupt_vector_disable(Timer_getApbctrl1InterruptNumber(timer->id));
    Timer_setConfigRegisters(&timer->regs->control, &timer->regs->reload, config);
    rtems_interrupt_vector_enable(Timer_getApbctrl1InterruptNumber(timer->id));
}

void
Timer_Apbctrl1_getConfigRegisters(const Timer_Apbctrl1 *const timer, Timer_Config *const config)
{
    Timer_getConfigRegisters(timer->regs->control, timer->regs->reload, config);
}

void
Timer_Apbctrl1_start(Timer_Apbctrl1* const timer)
{
    rtems_interrupt_vector_disable(Timer_getApbctrl1InterruptNumber(timer->id));
    Timer_start(&timer->regs->control);
    rtems_interrupt_vector_enable(Timer_getApbctrl1InterruptNumber(timer->id));
}

void
Timer_Apbctrl1_stop(Timer_Apbctrl1 *const timer)
{
    rtems_interrupt_vector_disable(Timer_getApbctrl1InterruptNumber(timer->id));
    Timer_stop(&timer->regs->control);
}

void
Timer_Apbctrl1_shutdown(Timer_Apbctrl1 *const timer)
{
    rtems_interrupt_vector_disable(Timer_getApbctrl1InterruptNumber(timer->id));
    Timer_stop(&timer->regs->control);
    irqDeinit(&timer->rtemsInterruptEntry, Timer_getApbctrl1InterruptNumber(timer->id));
}

uint32_t
Timer_Apbctrl1_getCounterValue(const Timer_Apbctrl1 *const timer)
{
    return timer->regs->counter;
}

bool
Timer_Apbctrl1_hasFinished(const Timer_Apbctrl1 *const timer)
{
    return Timer_hasFinished(timer->regs->control, timer->regs->counter);
}

Timer_Apbctrl1_Interrupt Timer_getApbctrl1InterruptNumber(Timer_Id id)
{
    switch (id) {
        case Timer_Id_1: {
            return Timer_Apbctrl1_Interrupt_1;
        }
        case Timer_Id_2: {
            return Timer_Apbctrl1_Interrupt_2;
        }
        case Timer_Id_3: {
            return Timer_Apbctrl1_Interrupt_3;
        }
        case Timer_Id_4: {
            return Timer_Apbctrl1_Interrupt_4;
        }
        default: {
            return Timer_Apbctrl1_Interrupt_Invalid;
        }
    }
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
Timer_Apbctrl2_init(Timer_Id id, Timer_Apbctrl2 *const timer, const Timer_InterruptHandler handler)
{
#ifdef MOCK_REGISTERS
    if (timer->base == NULL) {
        timer->base = malloc(sizeof(Timer_Apbctrl2_Base_Registers));
    }
    timer->regs = malloc(sizeof(Timer_Apbctrl2_Registers));
#else
    timer->base = (Timer_Apbctrl2_Base_Registers) GPTIMER_APBCTRL2_ADDRESS_BASE;
    timer->regs = getApbctrl2TimerAddressById(id);
#endif
    Timer_baseInit(&timer->base->configuration);
    timer->id = id;
    timer->irqHandler = handler;
    timer->rtemsInterruptEntry = (rtems_interrupt_entry) {0};
    timer->regs->control = 0;
    timer->regs->counter = 0;
    timer->regs->reload = 0;

    uint8_t irqNumber = Timer_getApbctrl2InterruptNumber(timer->id);
    irqInit(&timer->rtemsInterruptEntry, &timer->irqHandler, irqNumber);
}

void
Timer_Apbctrl2_setBaseScalerReloadValue(Timer_Apbctrl2 *const timer, uint8_t scalerReloadValue)
{
    timer->base->reload = (uint32_t) scalerReloadValue;
    timer->base->scaler = (uint32_t) scalerReloadValue;
}

void
Timer_Apbctrl2_setConfigRegisters(Timer_Apbctrl2 *const timer, const Timer_Config *const config)
{
    rtems_interrupt_vector_disable(Timer_getApbctrl2InterruptNumber(timer->id));
    Timer_setConfigRegisters(&timer->regs->control, &timer->regs->reload, config);
    rtems_interrupt_vector_enable(Timer_getApbctrl2InterruptNumber(timer->id));
}

void
Timer_Apbctrl2_getConfigRegisters(const Timer_Apbctrl2 *const timer, Timer_Config *const config)
{
    Timer_getConfigRegisters(timer->regs->control, timer->regs->reload, config);
}

void
Timer_Apbctrl2_start(Timer_Apbctrl2 *const timer)
{
    rtems_interrupt_vector_disable(Timer_getApbctrl2InterruptNumber(timer->id));
    Timer_start(&timer->regs->control);
    rtems_interrupt_vector_enable(Timer_getApbctrl2InterruptNumber(timer->id));
}

void
Timer_Apbctrl2_stop(Timer_Apbctrl2 *const timer)
{
    rtems_interrupt_vector_disable(Timer_getApbctrl2InterruptNumber(timer->id));
    Timer_stop(&timer->regs->control);
}

void
Timer_Apbctrl2_shutdown(Timer_Apbctrl2 *const timer)
{
    rtems_interrupt_vector_disable(Timer_getApbctrl2InterruptNumber(timer->id));
    Timer_stop(&timer->regs->control);
    irqDeinit(&timer->rtemsInterruptEntry, Timer_getApbctrl2InterruptNumber(timer->id));
}

uint32_t
Timer_Apbctrl2_getCounterValue(const Timer_Apbctrl2 *const timer)
{
    return timer->regs->counter;
}

bool
Timer_Apbctrl2_hasFinished(const Timer_Apbctrl2 *const timer)
{
    return Timer_hasFinished(timer->regs->control, timer->regs->counter);
}

Timer_Apbctrl2_Interrupt Timer_getApbctrl2InterruptNumber(Timer_Id id)
{
    switch (id) {
        case Timer_Id_1: {
            return Timer_Apbctrl2_Interrupt_1;
        }
        case Timer_Id_2: {
            return Timer_Apbctrl2_Interrupt_1;
        }
        default: {
            return Timer_Apbctrl2_Interrupt_Invalid;
        }
    }
}

/// \brief Timer hardware driver implementation.

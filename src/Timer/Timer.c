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

static inline void
emptyCallback(volatile void* arg)
{
    (void)arg;
}

Timer_InterruptHandler defaultInterruptHandler = {.callback = emptyCallback, .arg = 0};

Timer_Apbctrl1_Base_Registers baseApbctrl1Registers;
Timer_Apbctrl2_Base_Registers baseApbctrl2Registers;

void
Timer_baseInit(volatile uint32_t *const baseConfigurationRegister)
{
    Timer_setFlag (baseConfigurationRegister, TIMER_FLAG_SET, TIMER_CONFIG_DF);
    Timer_setFlag (baseConfigurationRegister, TIMER_FLAG_SET, TIMER_CONFIG_SI);
}

void
Timer_setConfig(volatile uint32_t *const timerControlRegister, volatile uint32_t *const timerReloadRegister, const Timer_Config *const config)
{
    Timer_setFlag (timerControlRegister, config->isEnabled, TIMER_CONTROL_EN);
    Timer_setFlag (timerControlRegister, config->isInterruptEnabled, TIMER_CONTROL_IE);
    Timer_setFlag (timerControlRegister, config->isAutoReloaded, TIMER_CONTROL_RS);
    Timer_setFlag (timerControlRegister, config->isChained, TIMER_CONTROL_CH);
    *timerReloadRegister = config->reloadValue;
}

void
Timer_getConfig(const uint32_t timerControlRegister, const uint32_t timerReloadRegister, Timer_Config *const config)
{
    config->isEnabled = Timer_getFlag (timerControlRegister, TIMER_CONTROL_EN);
    config->isInterruptEnabled = Timer_getFlag (timerControlRegister, TIMER_CONTROL_IE);
    config->isAutoReloaded = Timer_getFlag (timerControlRegister, TIMER_CONTROL_RS);
    config->isChained = Timer_getFlag (timerControlRegister, TIMER_CONTROL_CH);
    config->reloadValue = timerReloadRegister;
}

void
Timer_start(volatile uint32_t *const timerControlRegister)
{
    Timer_setFlag (timerControlRegister, TIMER_FLAG_SET, TIMER_CONTROL_LD);
    Timer_setFlag (timerControlRegister, TIMER_FLAG_SET, TIMER_CONTROL_EN);
}

void
Timer_stop(volatile uint32_t *const timerControlRegister)
{
    Timer_setFlag (timerControlRegister, TIMER_FLAG_RESET, TIMER_CONTROL_EN);
}

bool
Timer_hasFinished(const uint32_t timerControlRegister, const uint32_t timerCounterRegister) {
    bool result = false;
    if (!Timer_getFlag (timerControlRegister, TIMER_CONTROL_EN) && timerCounterRegister == TIMER_UNDERFLOWED) {
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
    rtems_interrupt_entry_remove(irqNumber, &entry);
}

void Timer_handleIrq(Timer_InterruptHandler* const handler)
{
    handler->callback(handler->arg);
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
Timer_Apbctrl1_init(Timer_Id id, Timer_Apbctrl1 *const timer, const Timer_InterruptHandler handler)
{
#ifdef MOCK_REGISTERS
    baseApbctrl1Registers = malloc(sizeof(Timer_Apbctrl1_Base_Registers));
    timer->regs = malloc(sizeof(Timer_Apbctrl1_Registers));
#else
    baseApbctrl1Registers = (Timer_Apbctrl1_Base_Registers) GPTIMER_APBCTRL1_ADDRESS_BASE;
    timer->regs = getApbctrl1TimerAddressById (id);
#endif
    Timer_baseInit (&baseApbctrl1Registers->configuration);
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
Timer_Apbctrl1_setBaseScalerReloadValue(uint16_t scalerReloadValue)
{
    baseApbctrl1Registers->reload = (uint32_t) scalerReloadValue;
}

void
Timer_Apbctrl1_setConfig(Timer_Apbctrl1 *const timer, const Timer_Config *const config)
{
    rtems_interrupt_vector_disable(Timer_getApbctrl1InterruptNumber(timer->id));
    Timer_setConfig (&timer->regs->control, &timer->regs->reload, config);
    rtems_interrupt_vector_enable(Timer_getApbctrl1InterruptNumber(timer->id));
}

void
Timer_Apbctrl1_getConfig(const Timer_Apbctrl1 *const timer, Timer_Config *const config)
{
    Timer_getConfig (timer->regs->control, timer->regs->reload, config);
}

void
Timer_Apbctrl1_start(Timer_Apbctrl1* const timer)
{
    rtems_interrupt_vector_disable(Timer_getApbctrl1InterruptNumber(timer->id));
    Timer_start (&timer->regs->control);
    rtems_interrupt_vector_enable(Timer_getApbctrl1InterruptNumber(timer->id));
}

void
Timer_Apbctrl1_stop(Timer_Apbctrl1 *const timer)
{
    rtems_interrupt_vector_disable(Timer_getApbctrl1InterruptNumber(timer->id));
    Timer_stop (&timer->regs->control);
    irqDeinit(&timer->rtemsInterruptEntry, Timer_getApbctrl1InterruptNumber(timer->id));
}

uint32_t
Timer_Apbctrl1_getCounterValue(const Timer_Apbctrl1 *const timer)
{
    uint32_t result = 0;

    rtems_interrupt_vector_disable(Timer_getApbctrl1InterruptNumber(timer->id));
    result = timer->regs->counter;
    rtems_interrupt_vector_enable(Timer_getApbctrl1InterruptNumber(timer->id));

    return result;
}

bool
Timer_Apbctrl1_hasFinished(const Timer_Apbctrl1 *const timer)
{
    bool result = false;

    rtems_interrupt_vector_disable(Timer_getApbctrl1InterruptNumber(timer->id));
    result = Timer_hasFinished (timer->regs->control, timer->regs->counter);
    rtems_interrupt_vector_enable(Timer_getApbctrl1InterruptNumber(timer->id));

    return result;
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
    baseApbctrl2Registers = malloc(sizeof(Timer_Apbctrl2_Base_Registers));
    timer->regs = malloc(sizeof(Timer_Apbctrl2_Registers));
#else
    baseApbctrl2Registers = (Timer_Apbctrl2_Base_Registers) GPTIMER_APBCTRL2_ADDRESS_BASE;
    timer->regs = getApbctrl2TimerAddressById (id);
#endif
    Timer_baseInit (&baseApbctrl2Registers->configuration);
    timer->id = id;
    timer->irqHandler = handler;
    timer->rtemsInterruptEntry = (rtems_interrupt_entry){0};
    timer->regs->control = 0;
    timer->regs->counter = 0;
    timer->regs->reload = 0;

    uint8_t irqNumber = Timer_getApbctrl2InterruptNumber(timer->id);
    irqInit(&timer->rtemsInterruptEntry, &timer->irqHandler, irqNumber);
}

void
Timer_Apbctrl2_setBaseScalerReloadValue(uint8_t scalerReloadValue)
{
    baseApbctrl2Registers->reload = (uint32_t) scalerReloadValue;
}

void
Timer_Apbctrl2_setConfig(Timer_Apbctrl2 *const timer, const Timer_Config *const config)
{
    rtems_interrupt_vector_disable(Timer_getApbctrl2InterruptNumber(timer->id));
    Timer_setConfig (&timer->regs->control, &timer->regs->reload, config);
    rtems_interrupt_vector_enable(Timer_getApbctrl2InterruptNumber(timer->id));
}

void
Timer_Apbctrl2_getConfig(const Timer_Apbctrl2 *const timer, Timer_Config *const config)
{
    Timer_getConfig (timer->regs->control, timer->regs->reload, config);
}

void
Timer_Apbctrl2_start(Timer_Apbctrl2 *const timer)
{
    rtems_interrupt_vector_disable(Timer_getApbctrl2InterruptNumber(timer->id));
    Timer_start (&timer->regs->control);
    rtems_interrupt_vector_enable(Timer_getApbctrl2InterruptNumber(timer->id));
}

void
Timer_Apbctrl2_stop(Timer_Apbctrl2 *const timer)
{
    rtems_interrupt_vector_disable(Timer_getApbctrl2InterruptNumber(timer->id));
    Timer_stop (&timer->regs->control);
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
    bool result = false;

    rtems_interrupt_vector_disable(Timer_getApbctrl2InterruptNumber(timer->id));
    result = Timer_hasFinished (timer->regs->control, timer->regs->counter);
    rtems_interrupt_vector_enable(Timer_getApbctrl2InterruptNumber(timer->id));

    return result;
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

/// \brief Timer hardware driver implementation.

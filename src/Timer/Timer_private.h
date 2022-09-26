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

/// \brief Timer hardware driver function prototypes and datatypes.

/**
 * @defgroup Timer Timer
 * @ingroup Bsp
 * @{
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "TimerTypedefs.h"

/// \brief Configures an Timer base configuration flags.
/// \param [in] baseConfigurationRegister Base configuration register for a specific timer group.
void Timer_baseInit(volatile uint32_t *const baseConfigurationRegister);

/// \brief Configures an Timer device based on a configuration descriptor.
/// \param [in] timer Timer device descriptor.
/// \param [in] config A configuration descriptor.
void Timer_setConfigRegisters(volatile uint32_t *const timerControlRegister, volatile uint32_t *const timerReloadRegister, const Timer_Config *const config);

/// \brief Retrieves configuration of an Timer device.
/// \param [in] timer Timer device descriptor.
/// \param [out] config A configuration descriptor.
void Timer_getConfigRegisters(const uint32_t timerControlRegister, const uint32_t timerReloadRegister, Timer_Config *const config);

/// \brief Performs a hardware startup procedure of Timer device.
/// \param [in] timer Timer device descriptor.
void Timer_start(volatile uint32_t *const timerControlRegister);

/// \brief Performs a hardware shutdown procedure of Timer device.
/// \param [in] timer Timer device descriptor.
void Timer_stop(volatile uint32_t *const timerControlRegister);

/// \brief Returns whether the counter has counted to 0 since the last read.
/// \param [in] timer Pointer to a structure representing Timer.
/// \returns Whether the counter has counted to 0.
bool Timer_hasFinished(const uint32_t timerControlRegister, const uint32_t timerCounterRegister);

/// \brief Timer interrupts initialization function.
/// \param [in] entry Pointer to a rtems interrupt entry structure.
/// \param [in] handler Pointer to a interrupt handler structure.
/// \param [in] irqNumber Timer specific interrupt number.
void irqInit(rtems_interrupt_entry *entry, Timer_InterruptHandler *handler, const uint8_t irqNumber);

/// \brief Timer interrupts deinitialization function.
/// \param [in] entry Pointer to a rtems interrupt entry structure.
/// \param [in] irqNumber Timer specific interrupt number.
void irqDeinit(rtems_interrupt_entry *entry, const uint8_t irqNumber);

/// \brief Timer interrupts handling function. It runs callback functions given by the interrupt handler parameter.
/// \param [in] handler Pointer to a interrupt handler structure.
void Timer_handleIrq(Timer_InterruptHandler* const handler);

/// \brief Checks flag status in provided timer register.
/// \param [in] timerRegister timer register.
/// \param [in] flag checked register flag offset.
/// \returns whether flag is set or not.
bool Timer_getFlag(const uint32_t timerRegister, const uint32_t flag);

/// \brief Sets flag status in provided timer register.
/// \param [in] timerRegister timer register.
/// \param [in] isSet flag value to be set.
/// \param [in] flag register flag offset.
void Timer_setFlag(volatile uint32_t *const timerRegister, const bool iSet, const uint32_t flag);

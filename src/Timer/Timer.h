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

#ifndef BSP_TIMER_H
#define BSP_TIMER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "TimerTypedefs.h"

/// \brief Default interrupt handler with no action when the interrupt is called.
extern Timer_InterruptHandler defaultInterruptHandler;

/// \brief Configures an Timer base configuration flags.
/// \param [in] baseConfigurationRegister Base configuration register for a specific timer group.
void Timer_baseInit(volatile uint32_t *const baseConfigurationRegister);

/// \brief Configures an Timer device based on a configuration descriptor.
/// \param [in] timer Timer device descriptor.
/// \param [in] config A configuration descriptor.
void Timer_setConfig(volatile uint32_t *const timerControlRegister, volatile uint32_t *const timerReloadRegister, const Timer_Config *const config);

/// \brief Retrieves configuration of an Timer device.
/// \param [in] timer Timer device descriptor.
/// \param [out] config A configuration descriptor.
void Timer_getConfig(const uint32_t timerControlRegister, const uint32_t timerReloadRegister, Timer_Config *const config);

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

/// \brief Intiializes a device descriptor for Timer.
/// \param [in] id Timer device identifier.
/// \param [out] timer Pointer to a timer device descriptor.
/// \param [in] handler Pointer to a interrupt handler structure.
void Timer_Apbctrl1_init(Timer_Id id, Timer_Apbctrl1 *const timer, const Timer_InterruptHandler handler);

/// \brief Sets Apbctrl1 base scaler value for all timers relative to systick.
/// \param [in] scalerReloadValue Reload register value (minimum 5).
void Timer_Apbctrl1_setBaseScalerReloadValue(uint16_t scalerReloadValue);

/// \brief Sets Timer control and reload registers with the configure struct given.
/// \param [out] timer Pointer to a timer device descriptor.
/// \param [in] config Pointer to timer configuration structure.
void Timer_Apbctrl1_setConfig(Timer_Apbctrl1 *const timer, const Timer_Config *const config);

/// \brief Gets Timer configuration settings from the timer registers.
/// \param [in] timer Pointer to a timer device descriptor.
/// \param [out] config Pointer to timer configuration structure.
void Timer_Apbctrl1_getConfig(const Timer_Apbctrl1 *const timer, Timer_Config *const config);

/// \brief Runs timer.
/// \param [in] timer Pointer to a timer device descriptor.
void Timer_Apbctrl1_start(Timer_Apbctrl1 *const timer);

/// \brief Stops timer.
/// \param [in] timer Pointer to a timer device descriptor.
void Timer_Apbctrl1_stop(Timer_Apbctrl1 *const timer);

/// \brief Stops timer and resets interrupt vector.
/// \param [in] timer Pointer to a timer device descriptor.
void Timer_Apbctrl1_shutdown(Timer_Apbctrl1 *const timer);

/// \brief Returns the current Timer counter value.
/// \param [in] timer Pointer to a structure representing Timer.
/// \returns Current counter value.
uint32_t Timer_Apbctrl1_getCounterValue(const Timer_Apbctrl1 *const timer);

/// \brief Returns if a Timer countdown to zero.
/// \param [in] timer Pointer to a timer device descriptor.
bool Timer_Apbctrl1_hasFinished(const Timer_Apbctrl1 *const timer);

/// \brief Returns interrupt number depends on Timer id given.
/// \param [in] id Timer specyfic id.
/// \returns Timer specific interrupt number.
Timer_Apbctrl1_Interrupt Timer_getApbctrl1InterruptNumber(Timer_Id id);

/// \brief Intiializes a device descriptor for Timer.
/// \param [in] id Timer device identifier.
/// \param [out] timer Pointer to a timer device descriptor.
/// \param [in] handler Pointer to a interrupt handler structure.
void Timer_Apbctrl2_init(Timer_Id id, Timer_Apbctrl2 *const timer, const Timer_InterruptHandler handler);

/// \brief Sets Apbctrl2 base scaler value for all timers relative to systick
/// \param [in] scaler Reload register value (minimum 5)
void Timer_Apbctrl2_setBaseScalerReloadValue(uint8_t scalerReloadValue);

/// \brief Sets Timer control and reload registers with the configure struct given.
/// \param [out] timer Pointer to a timer device descriptor.
/// \param [in] config Pointer to timer configuration structure.
void Timer_Apbctrl2_setConfig(Timer_Apbctrl2 *const timer, const Timer_Config *const config);

/// \brief Gets Timer configuration settings from the timer registers.
/// \param [in] timer Pointer to a timer device descriptor.
/// \param [out] config Pointer to timer configuration structure.
void Timer_Apbctrl2_getConfig(const Timer_Apbctrl2 *const timer, Timer_Config *const config);

/// \brief Runs timer.
/// \param [in] timer Pointer to a timer device descriptor.
void Timer_Apbctrl2_start(Timer_Apbctrl2 *const timer);

/// \brief Stops timer.
/// \param [in] timer Pointer to a timer device descriptor.
void Timer_Apbctrl2_stop(Timer_Apbctrl2 *const timer);

/// \brief Stops timer and resets interrupt vector.
/// \param [in] timer Pointer to a timer device descriptor.
void Timer_Apbctrl2_shutdown(Timer_Apbctrl2 *const timer);

/// \brief Returns the current Timer counter value.
/// \param [in] timer Pointer to a structure representing Timer.
/// \returns Current counter value.
uint32_t Timer_Apbctrl2_getCounterValue(const Timer_Apbctrl2 *const timer);

/// \brief Returns if a Timer countdown to zero.
/// \param [in] timer Pointer to a timer device descriptor.
bool Timer_Apbctrl2_hasFinished(const Timer_Apbctrl2 *const timer);

/// \brief Returns interrupt number depends on Timer id given.
/// \param [in] id Timer specyfic id.
/// \returns Timer specific interrupt number.
Timer_Apbctrl2_Interrupt Timer_getApbctrl2InterruptNumber(Timer_Id id);

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

#endif // BSP_TIMER_H

/** @} */

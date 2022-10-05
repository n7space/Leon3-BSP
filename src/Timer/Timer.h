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

/// \brief Intiializes a device descriptor for Timer.
/// \param [in] id Timer device identifier.
/// \param [out] timer Pointer to a timer device descriptor.
/// \param [in] handler Pointer to a interrupt handler structure.
void Timer_Apbctrl1_init(Timer_Id id, Timer_Apbctrl1 *const timer, const Timer_InterruptHandler handler);

/// \brief Sets Apbctrl1 base scaler value for all timers relative to systick.
/// \param [in] scalerReloadValue Reload register value (minimum 5).
void Timer_Apbctrl1_setBaseScalerReloadValue(Timer_Apbctrl1 *const timer, uint16_t scalerReloadValue);

/// \brief Sets Timer control and reload registers with the configure struct given.
/// \param [out] timer Pointer to a timer device descriptor.
/// \param [in] config Pointer to timer configuration structure.
void Timer_Apbctrl1_setConfigRegisters(Timer_Apbctrl1 *const timer, const Timer_Config *const config);

/// \brief Gets Timer configuration settings from the timer registers.
/// \param [in] timer Pointer to a timer device descriptor.
/// \param [out] config Pointer to timer configuration structure.
void Timer_Apbctrl1_getConfigRegisters(const Timer_Apbctrl1 *const timer, Timer_Config *const config);

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
void Timer_Apbctrl2_setBaseScalerReloadValue(Timer_Apbctrl2 *const timer, uint8_t scalerReloadValue);

/// \brief Sets Timer control and reload registers with the configure struct given.
/// \param [out] timer Pointer to a timer device descriptor.
/// \param [in] config Pointer to timer configuration structure.
void Timer_Apbctrl2_setConfigRegisters(Timer_Apbctrl2 *const timer, const Timer_Config *const config);

/// \brief Gets Timer configuration settings from the timer registers.
/// \param [in] timer Pointer to a timer device descriptor.
/// \param [out] config Pointer to timer configuration structure.
void Timer_Apbctrl2_getConfigRegisters(const Timer_Apbctrl2 *const timer, Timer_Config *const config);

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

#endif // BSP_TIMER_H

/** @} */

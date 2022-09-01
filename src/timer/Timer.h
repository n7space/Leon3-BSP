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
void Timer_restart(volatile uint32_t *const timerControlRegister);
/// \brief Performs a hardware shutdown procedure of Timer device.
/// \param [in] timer Timer device descriptor.
void Timer_stop(volatile uint32_t *const timerControlRegister);
/// brief Returns whether the counter has counted to 0 since the last read.
/// \param [in] timer Pointer to a structure representing Timer.
/// \returns Whether the counter has counted to 0.
bool Timer_hasFinished(const uint32_t timerControlRegister, const uint32_t timerCounterRegister);

/// \brief Intiializes a device descriptor for Timer.
/// \param [in] id Timer device identifier.
/// \param [out] timer Timer device descriptor.
void Timer_Apbctrl1_init(Timer_Id id, Timer_Apbctrl1 *const timer);
/// \brief Sets Apbctrl1 base scaler value for all timers relative to systick
/// \param [in] scaler Reload register value (minimum 5)
void Timer_Apbctrl1_setBaseScalerReloadValue(uint16_t scalerReloadValue);
void Timer_Apbctrl1_setConfig(Timer_Apbctrl1 *const timer, const Timer_Config *const config);
void Timer_Apbctrl1_getConfig(const Timer_Apbctrl1 *const timer, Timer_Config *const config);
void Timer_Apbctrl1_start(Timer_Apbctrl1 *const timer);
/// \brief Clears the current Timer counter value.
/// \param [in] timer Pointer to a structure representing Timer.
void Timer_Apbctrl1_restart(Timer_Apbctrl1 *const timer);
void Timer_Apbctrl1_stop(Timer_Apbctrl1 *const timer);
/// \brief Returns the current Timer counter value.
/// \param [in] timer Pointer to a structure representing Timer.
/// \returns Current counter value.
uint32_t Timer_Apbctrl1_getCounterValue(const Timer_Apbctrl1 *const timer);
bool Timer_Apbctrl1_hasFinished(const Timer_Apbctrl1 *const timer);

void Timer_Apbctrl2_init(Timer_Id id, Timer_Apbctrl2 *const timer);
/// \brief Sets Apbctrl2 base scaler value for all timers relative to systick
/// \param [in] scaler Reload register value (minimum 5)
void Timer_Apbctrl2_setBaseScalerReloadValue(uint8_t scalerReloadValue);
void Timer_Apbctrl2_setConfig(Timer_Apbctrl2 *const timer, const Timer_Config *const config);
void Timer_Apbctrl2_getConfig(const Timer_Apbctrl2 *const timer, Timer_Config *const config);
void Timer_Apbctrl2_start(Timer_Apbctrl2 *const timer);
void Timer_Apbctrl2_restart(Timer_Apbctrl2 *const timer);
void Timer_Apbctrl2_stop(Timer_Apbctrl2 *const timer);
uint32_t Timer_Apbctrl2_getCounterValue(const Timer_Apbctrl2 *const timer);
bool Timer_Apbctrl2_hasFinished(const Timer_Apbctrl2 *const timer);

bool Timer_getFlag(const uint32_t timerRegister, const uint32_t flag);
void Timer_setFlag(volatile uint32_t *const timerRegister, const bool set, const uint32_t flag);

#endif // BSP_TIMER_H

/** @} */

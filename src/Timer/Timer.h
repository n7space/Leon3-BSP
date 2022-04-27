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

/// \brief Timer hardware driver function prototypes and datatypes.

/**
 * @defgroup Timer Timer
 * @ingroup Bsp
 * @{
 */

#ifndef BSP_TIMER_H
#define BSP_TIMER_H

#include <TimerRegisters.h>
#include <stdbool.h>
#include <stdint.h>

/// \brief Timer device identifiers.
typedef enum
{
    Timer_Id_0 = 0,   ///< Base system timer
    Timer_Id_1 = 1,   ///< Timer 1
    Timer_Id_2 = 2,   ///< Timer 2
    Timer_Id_3 = 3,   ///< Timer 3
    Timer_Id_4 = 4,   ///< Timer 4
    Timer_Id_Max = 5, ///< Error value
} Timer_Id;

/// \brief Timer configuration descriptor.
typedef struct
{
    uint32_t clockSource;    ///< Clock source
    bool isInterruptEnabled; ///< Is interrupt enabled
    bool isEnabled;          ///< Is timer enabled
    uint32_t reloadValue;    ///< Reload value
} Timer_Config;

/// \brief Timer device descriptor.
typedef struct
{
    Timer_Id id;           ///< Timer device id
    TimerRegisters_t regs; ///< Hardware timer registers
} Timer;

/// \brief Configures an Timer device based on a configuration descriptor.
/// \param [in] timer Timer device descriptor.
/// \param [in] config A configuration descriptor.
void Timer_setConfig(Timer* const timer, const Timer_Config* const config);

/// \brief Retrieves configuration of an Timer device.
/// \param [in] timer Timer device descriptor.
/// \param [out] config A configuration descriptor.
void Timer_getConfig(const Timer* const timer, Timer_Config* const config);

#endif // BSP_TIMER_H

/** @} */

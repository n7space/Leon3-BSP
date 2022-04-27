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

/// \brief Timer hardware driver register addresses and datatypes.

/**
 * @defgroup Timer Timer
 * @ingroup Bsp
 * @{
 */

#ifndef BSP_TIMERREGS_H
#define BSP_TIMERREGS_H

#define GPTIMER_ADDRESS_BASE 0x80000300U

/// \brief Enum representing Timer memory addresses.
typedef enum
{
    Timer1_address = 0x80000310U,  ///< General Purpose Timer 1
    Timer2_address = 0x80000320U,  ///< General Purpose Timer 2
    Timer3_address = 0x80000330U,  ///< General Purpose Timer 3
    Timer4_address = 0x80000340U,  ///< General Purpose Timer 4
    TimerMax_address = 0xFFFFFFFFU ///< Max value
} Timer_address;

/// \brief Enum representing Timer interrupt vector numbers.
typedef enum
{
    Timer1_interrupt = 8,      ///< Timer 1 IRQ
    Timer2_interrupt = 9,      ///< Timer 2 IRQ
    Timer3_interrupt = 10,     ///< Timer 3 IRQ
    Timer4_interrupt = 11,     ///< Timer 4 IRQ
    TimerMax_interrupt = 0xFFU ///< Max value
} Timer_interrupt;

/// \brief Structure representing global scaler and configuration registers
typedef volatile struct TimerBaseRegisters
{
    uint32_t scaler;        ///< scaler value register
    uint32_t reload;        ///< scaler reload value register
    uint32_t configuration; ///< configuration register
} * TimerBaseRegisters_t;

/// \brief Structure representing Timer control registers (Big Endian).
typedef volatile struct TimerRegisters
{
    uint32_t counter; ///< counter value register
    uint32_t reload;  ///< reload value register
    uint32_t control; ///< control register
} * TimerRegisters_t;

// clang-format off

#define TIMER_CONFIGURATION_OFFSET  0x08U
#define TIMER_CONFIGURATION_DF      0x09U // Disable timer freeze
#define TIMER_CONFIGURATION_SI      0x08U // Separate interrupts
#define TIMER_CONFIGURATION_IRQ     0x03U // Interrupt ID of first timer
#define TIMER_CONFIGURATION_TIMERS  0x00U // Number of implemented timers

#define TIMER_CONTROL_OFFSET        0x08U
#define TIMER_CONTROL_DH            0x06U // Debug Halt
#define TIMER_CONTROL_CH            0x05U // Chain with preceding time
#define TIMER_CONTROL_IP            0x04U // Interrupt Pending
#define TIMER_CONTROL_IE            0x03U // Interrupt Enable
#define TIMER_CONTROL_LD            0x02U // Load value from reload register to counter
#define TIMER_CONTROL_RS            0x01U // Restart
#define TIMER_CONTROL_EN            0x00U // Enable the timer

// clang-format on

#endif // BSP_TIMERREGS_H

/** @} */

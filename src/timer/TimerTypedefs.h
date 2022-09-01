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

/// \brief Timer hardware driver register addresses and datatypes.

/**
 * @defgroup Timer Timer
 * @ingroup Bsp
 * @{
 */

#ifndef BSP_TIMERREGS_H
#define BSP_TIMERREGS_H

#include <stdint.h>

#define GPTIMER_APBCTRL1_ADDRESS_BASE 0x80000300u
#define GPTIMER_APBCTRL2_ADDRESS_BASE 0x80100600u

#define FLAG_MASK 0x01
#define FLAG_SET 0x01
#define FLAG_RESET 0x00

#define TIMER_UNDERFLOWED (uint32_t) (-1)

/// \brief Enum representing Apbctrl1 Timer memory addresses.
typedef enum
{
    Timer1_Apbctrl1_Address = 0x80000310u,      ///< General Purpose Timer 1
    Timer2_Apbctrl1_Address = 0x80000320u,      ///< General Purpose Timer 2
    Timer3_Apbctrl1_Address = 0x80000330u,      ///< General Purpose Timer 3
    Timer4_Apbctrl1_Address = 0x80000340u,      ///< General Purpose Timer 4
    TimerInvalid_Apbctrl1_Address = 0xFFFFFFFFu ///< Invalid value
} Timer_Apbctrl1_Address;

/// \brief Enum representing Apbctrl2 Timer memory addresses.
typedef enum
{
    Timer1_Apbctrl2_Address = 0x80100610u,      ///< General Purpose Timer with latch cabability 1
    Timer2_Apbctrl2_Address = 0x80100620u,      ///< General Purpose Timer with latch cabability 2
    TimerInvalid_Apbctrl2_Address = 0xFFFFFFFFu ///< Invalid value
} Timer_Apbctrl2_Address;

/// \brief Enum representing Timer interrupt vector numbers.
typedef enum
{
    Timer1_Interrupt = 8,      ///< Timer 1 IRQ
    Timer2_Interrupt = 9,      ///< Timer 2 IRQ
    Timer3_Interrupt = 10,     ///< Timer 3 IRQ
    Timer4_Interrupt = 11,     ///< Timer 4 IRQ
    TimerMax_Interrupt = 0xFFu ///< Max value
} Timer_Apbctrl1_Interrupt;

/// \brief Enum representing Timer control register flags.
typedef enum
{
    EN = 0,     ///< Enable the timer
    RS,         ///< Restart
    LD,         ///< Load value from reload register to counter
    IE,         ///< Interrupt Enable
    IP,         ///< Interrupt Pending
    CH,         ///< Chain with preceding time
    DH          ///< Debug Halt
} Timer_Control_Register_Flags;

/// \brief Enum representing config register TIMER flags.
typedef enum
{
    TIMERS = 0, ///< Number of implemented timers
    IRQ = 3,    ///< Interrupt ID of first timer
    SI = 8,     ///< Separate interrupts
    DF = 9,     ///< Disable timer freeze
    EL = 11     ///< Enable latching
} Timer_Config_Register_Flags;

/// \brief Structure representing global scaler and configuration registers for Apbctrl1 timers
typedef volatile struct
{
    uint32_t scaler;        ///< scaler value register
    uint32_t reload;        ///< scaler reload value register
    uint32_t configuration; ///< configuration register
} * Timer_Apbctrl1_Base_Registers;

/// \brief Structure representing global scaler and configuration registers for Apbctrl2 timers
typedef volatile struct
{
    uint32_t scaler;             ///< scaler value register
    uint32_t reload;             ///< scaler reload value register
    uint32_t configuration;      ///< configuration register
    uint32_t latchConfiguration; ///< latch configuration register
} * Timer_Apbctrl2_Base_Registers;

/// \brief Structure representing Apbctrl1 Timer control registers (Big Endian).
typedef volatile struct
{
    uint32_t counter; ///< counter value register
    uint32_t reload;  ///< reload value register
    uint32_t control; ///< control register
} * Timer_Apbctrl1_Registers;

/// \brief Structure representing Apbctrl2 Timer control registers (Big Endian).
typedef volatile struct
{
    uint32_t counter; ///< counter value register
    uint32_t reload;  ///< reload value register
    uint32_t control; ///< control register
    uint32_t latch;   ///< latch register
} * Timer_Apbctrl2_Registers;

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
    bool isInterruptEnabled; ///< Is interrupt enabled
    bool isEnabled;          ///< Is timer enabled
    bool isAutoReloaded;     ///< Is timer automatically reloaded
    bool isChained;          ///< Is timer chained to the preceding timer, creating a one 64 bit timer
    uint32_t reloadValue;    ///< Reload value
} Timer_Config;

/// \brief Timer device descriptor.
typedef struct
{
    Timer_Id id;                   ///< Timer device id
    Timer_Apbctrl1_Registers regs; ///< Hardware timer registers
} Timer_Apbctrl1;

/// \brief Timer device descriptor.
typedef struct
{
    Timer_Id id;                   ///< Timer device id
    Timer_Apbctrl2_Registers regs; ///< Hardware timer registers
} Timer_Apbctrl2;

#endif // BSP_TIMERREGS_H

/** @} */

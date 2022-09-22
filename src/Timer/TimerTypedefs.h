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

/// \brief Timer hardware driver register addresses and datatypes.

/**
 * @defgroup Timer Timer
 * @ingroup Bsp
 * @{
 */

#ifndef BSP_TIMERREGS_H
#define BSP_TIMERREGS_H

#include <stdint.h>
#include <rtems.h>

#define GPTIMER_APBCTRL1_ADDRESS_BASE 0x80000300u
#define GPTIMER_APBCTRL2_ADDRESS_BASE 0x80100600u

#define TIMER_FLAG_MASK 0x1
#define TIMER_FLAG_SET 0x1
#define TIMER_FLAG_RESET 0x0

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

/// \brief Enum representing Apbctrl1 Timer interrupt vector numbers.
typedef enum
{
    Timer_Apbctrl1_Interrupt_1 = 8,          ///< Timer 1 IRQ
    Timer_Apbctrl1_Interrupt_2 = 9,          ///< Timer 2 IRQ
    Timer_Apbctrl1_Interrupt_3 = 10,         ///< Timer 3 IRQ
    Timer_Apbctrl1_Interrupt_4 = 11,         ///< Timer 4 IRQ
    Timer_Apbctrl1_Interrupt_Invalid = 0xFFu ///< Invalid value
} Timer_Apbctrl1_Interrupt;

/// \brief Enum representing Apbctrl2 Timer interrupt vector numbers.
typedef enum
{
    Timer_Apbctrl2_Interrupt_1 = 7,          ///< Timer IRQ
    Timer_Apbctrl2_Interrupt_Invalid = 0xFFu ///< Invalid value
} Timer_Apbctrl2_Interrupt;

/// \brief Enum representing Timer control register flags.
typedef enum
{
    TIMER_CONTROL_EN = 0, ///< Enable the timer
    TIMER_CONTROL_RS,     ///< Restart
    TIMER_CONTROL_LD,     ///< Load value from reload register to counter
    TIMER_CONTROL_IE,     ///< Interrupt Enable
    TIMER_CONTROL_IP,     ///< Interrupt Pending
    TIMER_CONTROL_CH,     ///< Chain with preceding time
    TIMER_CONTROL_DH      ///< Debug Halt
} Timer_Control_Register_Flags;

/// \brief Enum representing config register TIMER flags.
typedef enum
{
    TIMER_CONFIG_TIMERS = 0, ///< Number of implemented timers
    TIMER_CONFIG_IRQ = 3,    ///< Interrupt ID of first timer
    TIMER_CONFIG_SI = 8,     ///< Separate interrupts
    TIMER_CONFIG_DF = 9,     ///< Disable timer freeze
    TIMER_CONFIG_EL = 11     ///< Enable latching
} Timer_Config_Register_Flags;

/// \brief Structure representing global scaler and configuration registers for Apbctrl1 timers.
typedef volatile struct
{
    uint32_t scaler;        ///< scaler value register
    uint32_t reload;        ///< scaler reload value register
    uint32_t configuration; ///< configuration register
} * Timer_Apbctrl1_Base_Registers;

/// \brief Structure representing global scaler and configuration registers for Apbctrl2 timers.
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

/// \brief Timer function callback for interrupt usage.
typedef void (*Timer_InterruptCallback)(volatile void* arg);

/// \brief Structure representing Timer interrupt handler.
typedef struct
{
    Timer_InterruptCallback callback; ///< Interupt callback
    volatile void* arg;               ///< Interrupt callback parameter
} Timer_InterruptHandler;

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

/// \brief Apbctrl1 timer device descriptor.
typedef struct
{
    Timer_Id id;                               ///< Timer device id
    Timer_Apbctrl1_Registers regs;             ///< Hardware timer registers
    Timer_InterruptHandler irqHandler;         ///< Timer interrupt handler
    rtems_interrupt_entry rtemsInterruptEntry; ///< RTEMS interrupt entry
} Timer_Apbctrl1;

/// \brief Apbctrl2 timer device descriptor.
typedef struct
{
    Timer_Id id;                               ///< Timer device id
    Timer_Apbctrl2_Registers regs;             ///< Hardware timer registers
    Timer_InterruptHandler irqHandler;         ///< Timer interrupt handler
    rtems_interrupt_entry rtemsInterruptEntry; ///< RTEMS interrupt entry
} Timer_Apbctrl2;

#endif // BSP_TIMERREGS_H

/** @} */

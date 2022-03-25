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

/// \brief System configuration.

/**
 * @defgroup SystemConfig SystemConfig
 * @ingroup Bsp
 * @{
 */

#ifndef BSP_SYSTEMCONFIG_H
#define BSP_SYSTEMCONFIG_H

#define IRQMP_BASE 0x80000200U

#define IRQ_LEVEL(n) (((IRQMP_BASE) >> n) | 1U)

#define MAX_TLS_SIZE RTEMS_ALIGN_UP(64, RTEMS_TASK_STORAGE_ALIGNMENT)

#define TASK_STORAGE_SIZE                                                      \
    RTEMS_TASK_STORAGE_SIZE(MAX_TLS_SIZE + RTEMS_MINIMUM_STACK_SIZE,           \
                            RTEMS_DEFAULT_ATTRIBUTES)

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 3

#define CONFIGURE_MINIMUM_TASKS_WITH_USER_PROVIDED_STORAGE                     \
    CONFIGURE_MAXIMUM_TASKS

#define CONFIGURE_MAXIMUM_TIMERS 3

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 0

#define CONFIGURE_DISABLE_NEWLIB_REENTRANCY

#define CONFIGURE_APPLICATION_DISABLE_FILESYSTEM

#define CONFIGURE_MAXIMUM_THREAD_LOCAL_STORAGE_SIZE MAX_TLS_SIZE

#define CONFIGURE_INTERRUPT_STACK_SIZE                                         \
    RTEMS_ALIGN_UP((4 * CPU_STACK_MINIMUM_SIZE) + CONTEXT_FP_SIZE,             \
                   CPU_STACK_ALIGNMENT)

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_DEFAULT_ATTRIBUTES

#define CONFIGURE_INIT_TASK_INITIAL_MODES                                      \
    (RTEMS_DEFAULT_MODES | RTEMS_INTERRUPT_LEVEL(0))

#define CONFIGURE_INIT_TASK_CONSTRUCT_STORAGE_SIZE TASK_STORAGE_SIZE

#define CONFIGURE_INIT

#endif // BSP_SYSCONFIG_H

/** @} */

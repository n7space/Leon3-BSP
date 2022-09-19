#pragma once

#include <stdint.h>

#define RTEMS_INTERRUPT_UNIQUE 1u

typedef struct rtems_interrupt_entry {
  uint32_t mock;
} rtems_interrupt_entry;

typedef enum { MOCK
} rtems_status_code;

typedef uint32_t rtems_option;
typedef uint32_t rtems_vector_number;
typedef void ( *rtems_interrupt_handler )( void * );

uint32_t rtems_clock_get_ticks_per_second();
void rtems_interrupt_entry_initialize(rtems_interrupt_entry *entry, rtems_interrupt_handler routine, void *arg, const char *info);
void rtems_interrupt_entry_install( rtems_vector_number vector, rtems_option options, rtems_interrupt_entry *entry);
rtems_status_code rtems_interrupt_vector_enable(rtems_vector_number vector);
rtems_status_code rtems_interrupt_vector_disable(rtems_vector_number vector);
rtems_status_code rtems_interrupt_entry_remove(rtems_vector_number vector, rtems_interrupt_entry *entry);
rtems_status_code rtems_interrupt_clear( rtems_vector_number vector );
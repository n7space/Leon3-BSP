#include "rtems.h"

uint32_t rtems_clock_get_ticks_per_second()
{
  return 0;
}

void rtems_interrupt_entry_initialize(rtems_interrupt_entry *entry, rtems_interrupt_handler routine, void *arg, const char *info) {
  (void)entry;
  (void)routine;
  (void)arg;
  (void)info;
}

void rtems_interrupt_entry_install( rtems_vector_number vector, rtems_option options, rtems_interrupt_entry *entry) {
  (void)vector;
  (void)options;
  (void)entry;
}

rtems_status_code rtems_interrupt_vector_enable(rtems_vector_number vector)
{
  (void)vector;
  return MOCK;
}

rtems_status_code rtems_interrupt_vector_disable(rtems_vector_number vector)
{
  (void)vector;
  return MOCK;
}

rtems_status_code rtems_interrupt_entry_remove(rtems_vector_number vector, rtems_interrupt_entry *entry)
{
  (void)vector;
  (void)entry;
  return MOCK;
}

rtems_status_code rtems_interrupt_clear( rtems_vector_number vector )
{
  (void)vector;
  return MOCK;
}

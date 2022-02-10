#include "Startup_Leon3.h"
#include "SystemConfig.h"
#include <rtems.h>
#include <rtems/confdefs.h>

extern int main();

rtems_task
Init(rtems_task_argument arg)
{
    (void)arg;
    rtems_fatal(RTEMS_FATAL_SOURCE_EXIT, main());
}

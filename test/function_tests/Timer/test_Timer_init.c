#include <stdbool.h>
#include "Timer.h"

#define TIMER_DEF_SCALER_RELOAD 0x0AU

bool
test_Timer_init(Timer_Id id, Timer* timer)
{
    Timer_setBaseScalerReloadValue(TIMER_DEF_SCALER_RELOAD);
    Timer_init(id, timer);
    Timer_Config config = (Timer_Config){ 0 };
    Timer_getConfig(timer, &config);
    config.isEnabled = 1;
    config.isInterruptEnabled = 1;
    config.isAutoReloaded = 0;
    Timer_setConfig(timer, &config);
    Timer_startup(timer);
    Timer_shutdown(timer);
    return true;
}

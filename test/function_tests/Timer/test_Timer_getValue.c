#include <stdbool.h>
#include <stdint.h>
#include "Uart.h"
#include "Timer.h"

#define TIMER_DEF_SCALER_RELOAD 0x0AU

static uint32_t value;

static uint32_t maxReadAttempts = 0xFFFFU;

static uint32_t testReloadValue = 0xAAFFU;

static int timeoutLimit = 200000;

static uint8_t dataOK[] = "\r\nRead counter value is OK";

static uint8_t dataNOK[] = "\r\nRead counter value is not OK";

bool
test_Timer_getValue(Timer_Id idTimer, Timer* timer, Uart_Id idUart, Uart* uart)
{
    Timer_setBaseScalerReloadValue(TIMER_DEF_SCALER_RELOAD);
    Timer_init(idTimer, timer);
    Timer_Config config = (Timer_Config){ 0 };
    Timer_getConfig(timer, &config);
    config.isEnabled = 1;
    config.isInterruptEnabled = 1;
    config.isAutoReloaded = 0;
    config.reloadValue = testReloadValue;
    Timer_setConfig(timer, &config);
    Timer_startup(timer);
    uint32_t i = 0;
    while(i < maxReadAttempts) {
        value = Timer_getCurrentValue(timer);
        if(Timer_hasCountedToZero(timer)) {
            value = 0;
            break;
        }
        ++i;
    }
    Timer_shutdown(timer);
    Uart_init(idUart, uart);
    Uart_Config configUart = (Uart_Config){ 0 };
    configUart.isTxEnabled = 1;
    configUart.isRxEnabled = 1;
    Uart_setConfig(uart, &configUart);
    Uart_startup(uart);
    Uart_ErrorCode errCode = Uart_ErrorCode_OK;
    if(value == 0) {
        for(int j = 0; j < 40 && dataOK[j] != 0; j++) {
            Uart_write(uart, dataOK[j], timeoutLimit, &errCode);
        }
        Uart_shutdown(uart);
        return true;
    }
    for(int j = 0; j < 40 && dataNOK[j] != 0; j++) {
        Uart_write(uart, dataNOK[j], timeoutLimit, &errCode);
    }
    Uart_shutdown(uart);
    return false;
}

#include <stdbool.h>
#include "Uart.h"

static int timeoutLimit = 2000000;

static Uart_ErrorCode errCode = Uart_ErrorCode_OK;

static uint8_t data[] = "Write text (sync)\r\n";

bool
test_Uart_write(Uart* uart)
{
    Uart_init(Uart_Id_0, uart);
    Uart_Config config = (Uart_Config){ 0 };
    config.isTxEnabled = 1;
    config.isRxEnabled = 1;
    Uart_setConfig(uart, &config);
    Uart_startup(uart);
    for(int i = 0; i < 20 && data[i] != 0; i++) {
        Uart_write(uart, data[i], timeoutLimit, &errCode);
    }
    Uart_shutdown(uart);
    if(errCode == 0) {
        return true;
    }
    return false;
}

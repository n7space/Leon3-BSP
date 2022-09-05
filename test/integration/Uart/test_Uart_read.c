#include <stdbool.h>
#include "Uart.h"

static int timeoutLimit = 0;

static Uart_ErrorCode errCode = Uart_ErrorCode_OK;

static uint8_t text[] = "Read text (sync) - data will be read until 'q' or 20 "
                        "chars are received:\r\n";

bool
test_Uart_read(Uart* uart)
{
    Uart_init(Uart_Id_0, uart);
    Uart_Config config = (Uart_Config){ 0 };
    config.isTxEnabled = 1;
    config.isRxEnabled = 1;
    Uart_setConfig(uart, &config);
    Uart_startup(uart);
    uint8_t buf = '\0';
    for(int i = 0; i < 80 && text[i] != 0; i++) {
        Uart_write(uart, text[i], timeoutLimit, &errCode);
    }
    for(int i = 0; i < 20; i++) {
        Uart_read(uart, &buf, timeoutLimit, &errCode);
        Uart_write(uart, buf, timeoutLimit, &errCode);
        if(buf == 'q') {
            break;
        }
    }
    Uart_shutdown(uart);
    if(errCode == 0) {
        return true;
    }
    return false;
}

#include <stdbool.h>
#include "Uart.h"

static int timeoutLimit = 40000;

static int errCode = 0;

static uint8_t data[] = "Write text (sync)\r\n";

bool
test_Uart_write(Uart* uart)
{
    for(int i = 0; i < 20 && data[i] != 0; i++) {
        Uart_write(uart, data[i], timeoutLimit, &errCode);
    }
    if(errCode == 0) {
        return true;
    }
    return false;
}

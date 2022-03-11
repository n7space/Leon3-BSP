#include <stdbool.h>
#include "Uart.h"

bool
test_Uart_init(Uart* uart)
{
    Uart_init(Uart_Id_0, uart);
    uart->config.isTxEnabled = 1;
    uart->config.isRxEnabled = 1;
    Uart_startup(uart);
    return true;
}

#include <stdbool.h>
#include "Uart.h"

bool
test_Uart_init(Uart_Id id, Uart* uart)
{
    Uart_init(id, uart);
    Uart_Config config = (Uart_Config){ 0 };
    config.isTxEnabled = 1;
    config.isRxEnabled = 1;
    Uart_setConfig(uart, &config);
    Uart_startup(uart);
    return true;
}

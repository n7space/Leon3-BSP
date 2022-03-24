#include <stdbool.h>
#include "Uart.h"

static volatile bool dataSent;

static void
testCallback(bool* result)
{
    *result = true;
}

static const Uart_TxHandler testHandler = {
    .callback = (UartTxEndCallback)testCallback,
    .arg = &dataSent
};

BYTE_FIFO_CREATE_FILLED(txByteFifoForWriteTest, "Write text (async)\r\n");

bool
test_Uart_writeAsync(Uart* uart)
{
    Uart_init(Uart_Id_0, uart);
    Uart_Config config = (Uart_Config){ 0 };
    config.isTxEnabled = 1;
    config.isRxEnabled = 1;
    Uart_setConfig(uart, &config);
    Uart_startup(uart);
    dataSent = false;
    Uart_writeAsync(uart, &txByteFifoForWriteTest, testHandler);
    for(int timeout = 0; timeout < 4000; timeout++) {
        if(dataSent) {
            Uart_shutdown(uart);
            return true;
        }
    }
    Uart_shutdown(uart);
    return false;
}

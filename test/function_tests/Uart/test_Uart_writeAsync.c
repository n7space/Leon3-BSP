#include <stdbool.h>
#include "Uart.h"

static bool dataSent = false;

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
    Uart_writeAsync(uart, &txByteFifoForWriteTest, testHandler);
    for(int i = 0; i < 4000; i++) {
        if(dataSent) {
            return true;
        }
    }
    return false;
}

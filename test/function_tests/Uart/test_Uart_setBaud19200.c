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

BYTE_FIFO_CREATE_FILLED(newTxFifoBaud19200_1, "Setting baud rate 19200:\r\n");

BYTE_FIFO_CREATE_FILLED(newTxFifoBaud19200_2, "Baud rate set to 19200.\r\n");

bool
test_Uart_setBaud19200(Uart* uart)
{
    bool testResult = false;
    Uart_init(Uart_Id_0, uart);
    Uart_Config config1 = (Uart_Config){ 0 };
    config1.isTxEnabled = 1;
    config1.isRxEnabled = 1;
    config1.baudRate = Uart_BaudRate_19200;
    Uart_setConfig(uart, &config1);
    Uart_startup(uart);
    dataSent = false;
    Uart_writeAsync(uart, &newTxFifoBaud19200_1, testHandler);
    int timeout = 0;
    while(dataSent != true && timeout < 400000) {
        timeout++;
    }
    Uart_Config config2;
    Uart_getConfig(uart, &config2);
    if(config2.baudRate == Uart_BaudRate_19200) {
        Uart_writeAsync(uart, &newTxFifoBaud19200_2, testHandler);
        timeout = 0;
        dataSent = false;
        while(dataSent != true && timeout < 400000) {
            timeout++;
        };
        testResult = true;
    }
    Uart_shutdown(uart);
    return testResult;
}

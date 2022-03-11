#include <stdbool.h>
#include "Uart.h"
#include "ByteFifo.h"

static bool dataReceived = false;

static void
testLengthCallback(bool* result)
{
    *result = true;
}

static void
testCharacterCallback(bool* result)
{
    *result = true;
}

static Uart_RxHandler testHandler = {
    .lengthCallback = (UartRxEndLengthCallback)testLengthCallback,
    .characterCallback = (UartRxEndCharacterCallback)testCharacterCallback,
    .lengthArg = &dataReceived,
    .characterArg = &dataReceived,
    .targetLength = 20,
    .targetCharacter = 'q'
};

BYTE_FIFO_CREATE_FILLED(txByteFifoForReadTest, "\r\nRead text (async):\r\n");

BYTE_FIFO_CREATE(rxByteFifoForReadTest, 40);

bool
test_Uart_readAsync(Uart* uart)
{
    ByteFifo_clear(&rxByteFifoForReadTest);
    Uart_writeAsync(uart, &txByteFifoForReadTest, uart->txHandler);
    Uart_readAsync(uart, &rxByteFifoForReadTest, testHandler);
    while(!ByteFifo_isFull(&rxByteFifoForReadTest)) {
        if(dataReceived) {
            return true;
        }
    }
    return false;
}

#include <stdbool.h>
#include "Uart.h"
#include "ByteFifo.h"

static volatile bool charReceived;

static volatile bool lengthReceived;

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
    .lengthArg = &lengthReceived,
    .characterArg = &charReceived,
    .targetLength = 20,
    .targetCharacter = 'q'
};

BYTE_FIFO_CREATE_FILLED(txByteFifoForReadTest, "\r\nRead text (async):\r\n");

BYTE_FIFO_CREATE(rxByteFifoForReadTest, 40);

bool
test_Uart_readAsync(Uart* uart)
{
    charReceived = false;
    lengthReceived = false;
    ByteFifo_clear(&rxByteFifoForReadTest);
    Uart_writeAsync(uart, &txByteFifoForReadTest, uart->txHandler);
    Uart_readAsync(uart, &rxByteFifoForReadTest, testHandler);
    while(!ByteFifo_isFull(&rxByteFifoForReadTest)) {
        if(lengthReceived == true || charReceived == true) {
            Uart_writeAsync(uart, &rxByteFifoForReadTest, uart->txHandler);
            return true;
        }
    }
    return false;
}

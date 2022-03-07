#ifndef BSP_UART_H
#define BSP_UART_H

#include <UartRegisters.h>
#include <ByteFifo.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    Uart_Id_0 = 0,
    Uart_Id_1 = 1,
    Uart_Id_2 = 2,
    Uart_Id_3 = 3,
    Uart_Id_4 = 4,
    Uart_Id_5 = 5,
    Error_Id = 6
} Uart_Id;

typedef enum
{
    Uart_Parity_Even = 0,
    Uart_Parity_Odd = 1,
    Uart_Parity_None = 4,
    Error_Parity = 9
} Uart_Parity;

typedef enum
{
    Uart_BaudRate_300 = 300,
    Uart_BaudRate_600 = 600,
    Uart_BaudRate_1200 = 1200,
    Uart_BaudRate_1800 = 1800,
    Uart_BaudRate_2400 = 2400,
    Uart_BaudRate_4800 = 4800,
    Uart_BaudRate_9600 = 9600,
    Uart_BaudRate_19200 = 19200,
    Uart_BaudRate_28800 = 28800,
    Uart_BaudRate_38400 = 38400,
    Uart_BaudRate_57600 = 57600,
    Uart_BaudRate_576800 = 76800,
    Uart_BaudRate_115200 = 115200,
    Error_BaudRate = 0
} Uart_BaudRate;

typedef struct
{
    bool isTxEnabled;
    bool isRxEnabled;
    bool isTestModeEnabled;
    Uart_Parity parity;
    Uart_BaudRate baudRate;
    uint32_t baudRateClkSrc;
    uint32_t baudRateClkFreq;
} Uart_Config;

typedef void (*UartTxEndCallback)(void* arg);

typedef struct
{
    UartTxEndCallback callback;
    void* arg;
} Uart_TxHandler;

typedef void (*UartRxEndLengthCallback)(void* arg);

typedef void (*UartRxEndCharacterCallback)(void* arg);

typedef struct
{
    UartRxEndLengthCallback lengthCallback;
    UartRxEndCharacterCallback characterCallback;
    void* lengthArg;
    void* characterArg;
    uint8_t targetCharacter;
    uint32_t targetLength;
} Uart_RxHandler;

typedef void (*UartErrorCallback)(void* arg);

typedef struct
{
    UartErrorCallback callback;
    void* arg;
} Uart_ErrorHandler;

typedef struct
{
    bool hasOverrunOccurred;
    bool hasFramingErrorOccurred;
    bool hasParityErrorOccurred;
    bool hasRxFifoFullErrorOccurred;
} Uart_ErrorFlags;

typedef struct
{
    Uart_Id id;
    Uart_TxHandler txHandler;
    Uart_RxHandler rxHandler;
    Uart_ErrorHandler errorHandler;
    ByteFifo* txFifo;
    ByteFifo* rxFifo;
    UartRegisters_t reg;
    Uart_Config config;
} Uart;

void Uart_setConfig(Uart* const uart, const Uart_Config* const config);

void Uart_getConfig(const Uart* const uart, Uart_Config* const config);

void Uart_startup(Uart* const uart);

void Uart_shutdown(Uart* const uart);

void Uart_init(Uart_Id id, Uart* const uart);

void Uart_write(Uart* const uart,
                const uint8_t data,
                uint32_t const timeoutLimit,
                int* const errCode);

void Uart_read(Uart* const uart,
               uint8_t* data,
               uint32_t timeoutLimit,
               int* const errCode);

void Uart_writeAsync(Uart* const uart,
                     ByteFifo* const fifo,
                     const Uart_TxHandler handler);

void Uart_readAsync(Uart* const uart,
                    ByteFifo* const fifo,
                    const Uart_RxHandler handler);

bool Uart_isTxEmpty(const Uart* const uart);

bool Uart_isRxEmpty(const Uart* const uart);

uint32_t Uart_getTxFifoCount(Uart* const uart);

uint32_t Uart_getRxFifoCount(Uart* const uart);

void Uart_registerErrorHandler(Uart* const uart,
                               const Uart_ErrorHandler handler);

void Uart_handleInterrupt(void* arg);

uint8_t Uart_getLinkErrors(uint32_t statusRegister, Uart_ErrorFlags* errFlags);

uint32_t Uart_getStatusRegister(const Uart* const uart);

#endif // BSP_UART_H

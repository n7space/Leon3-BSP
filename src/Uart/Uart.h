#ifndef BSP_UART_H
#define BSP_UART_H

#include <stdint.h>

#ifndef RTEMS_SIS

#define UART_INTERFACES_MAX 6

typedef enum
{
    Uart_Id_0 = 0,
    Uart_Id_1 = 1,
    Uart_Id_2 = 2,
    Uart_Id_3 = 3,
    Uart_Id_4 = 4,
    Uart_Id_5 = 5,
    Uart_Id_Max = UART_INTERFACES_MAX
} Uart_Id;

typedef enum
{
    Uart_Parity_Even = 0,
    Uart_Parity_Odd = 1,
    Uart_Parity_None = 4,
} Uart_Parity;

typedef struct
{
    uint8_t isTxEnabled;
    uint8_t isRxEnabled;
    uint8_t isTestModeEnabled;
    Uart_Parity parity;
    uint32_t baudRate;
    uint32_t baudRateClkSrc;
    uint32_t baudRateClkFreq;
} Uart_Config;

#else // RTEMS_SIS

#define UART_INTERFACES_MAX 1

typedef enum
{
    Uart_Id_0 = 0,
    Uart_Id_Max = UART_INTERFACES_MAX
} Uart_Id;

typedef struct
{
    uint8_t isTxEnabled;
    uint8_t isRxEnabled;
    uint8_t isTestModeEnabled;
} Uart_Config;

#endif // RTEMS_SIS

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
    uint8_t hasOverrunOccurred;
    uint8_t hasFramingErrorOccurred;
    uint8_t hasParityErrorOccurred;
    uint8_t hasRxFifoFullErrorOccurred;
} Uart_ErrorFlags;

typedef struct
{
    Uart_Id id;
    Uart_TxHandler txHandler;
    Uart_RxHandler rxHandler;
    Uart_ErrorHandler errorHandler;
    uint32_t* txFifo;
    uint32_t* rxFifo;
    volatile uint32_t* reg;
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
                     uint32_t* const fifo,
                     const Uart_TxHandler handler);

void Uart_readAsync(Uart* const uart,
                    uint32_t* const fifo,
                    const Uart_RxHandler handler);

uint8_t Uart_isTxEmpty(const Uart* const uart);

void Uart_readRxFifo(Uart* const uart, uint8_t* const fifo);

uint32_t Uart_getTxFifoCount(Uart* const uart);

uint32_t Uart_getRxFifoCount(Uart* const uart);

void Uart_registerErrorHandler(Uart* const uart,
                               const Uart_ErrorHandler handler);

void Uart_handleInterrupt(void* arg);

uint8_t Uart_getLinkErrors(uint32_t statusRegister, Uart_ErrorFlags* errFlags);

uint32_t Uart_getStatusRegister(const Uart* const uart);

#endif // BSP_UART_H

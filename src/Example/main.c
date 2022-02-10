#include "Uart.h"
#include "UartRegisters.h"
#include "SystemConfig.h"
#include "Startup_Leon3.h"
#include <rtems.h>

#define REG_BIT(reg, n) (((reg) >> n) & 1U)
#define REG_BYTE(reg, n) (((reg) >> (n * 4)) & 0x0FU)
#define TIMEOUT 10000000

static Uart uart0;

static Uart_Config config;

static int* err;

static inline char
itoc(uint8_t c)
{
    switch(c) {
        case 10:
            return 'A';
        case 11:
            return 'B';
        case 12:
            return 'C';
        case 13:
            return 'D';
        case 14:
            return 'E';
        case 15:
            return 'F';
        default:
            return '0' + c;
    }
}

static void
debugAddr(const uint32_t addr)
{
    char buff[10] = "0x00000000";
    for(unsigned char n = 0; n < 8; n++) {
        uint8_t byte = 0;
        byte += REG_BYTE(addr, n);
        buff[9 - n] = itoc(byte);
    }
    for(unsigned char n = 0; n < 10; n++) {
        Uart_write(&uart0, buff[n], TIMEOUT, err);
    }
}

static void
debugReg(const uint32_t reg)
{
    for(char n = 0; n < 32; n++) {
        uint8_t byte = 0;
        byte += REG_BIT(reg, n);
        Uart_write(&uart0, '0' + byte, TIMEOUT, err);
    }
}

static uint8_t i;
static void
testCallback()
{
    ++i;
}

int
main()
{
    i = 0;
    Uart_init(Uart_Id_0, &uart0);
    uart0.config.isTxEnabled = 1;
    uart0.config.isRxEnabled = 1;
    uart0.rxHandler.targetLength = 5;
    uart0.rxHandler.lengthCallback = testCallback;
    Uart_startup(&uart0);
    Uart_getConfig(&uart0, &config);
    Uart_write(&uart0, '\n', TIMEOUT, err);
    debugAddr(((UartRegisters_t)uart0.reg)->status);
    Uart_write(&uart0, '\n', TIMEOUT, err);
    debugReg(((UartRegisters_t)uart0.reg)->status);
    Uart_write(&uart0, '\n', TIMEOUT, err);
    Uart_write(&uart0, '_', TIMEOUT, err);
    Uart_write(&uart0, '\r', TIMEOUT, err);
    char buf = '\0';
    do {
        Uart_write(&uart0, itoc(i), TIMEOUT, err);
        Uart_write(&uart0, '\r', TIMEOUT, err);
        Uart_read(&uart0, (uint8_t*)&buf, TIMEOUT, err);
    } while(buf != 'q' && *err == 0);
    if(*err != 0) {
        Uart_write(&uart0, '!', TIMEOUT, err);
    }
    Uart_write(&uart0, '\n', TIMEOUT, err);
    Uart_shutdown(&uart0);
    Uart_write(&uart0, 'x', TIMEOUT, err);
    return 0;
}

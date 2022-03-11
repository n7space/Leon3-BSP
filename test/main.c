/**@file
 * This file is part of the Leon3 BSP for the Test Environment.
 *
 * @copyright 2022-2023 N7 Space Sp. z o.o.
 *
 * Test Environment was developed under a programme of,
 * and funded by, the European Space Agency (the "ESA").
 *
 *
 * Licensed under the ESA Public License (ESA-PL) Permissive,
 * Version 2.3 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://essr.esa.int/license/list
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/// \brief Example program for UART interface

#include "Uart.h"
#include "UartRegisters.h"
#include "ByteFifo.h"
#include "SystemConfig.h"
#include "Startup_Leon3.h"
#include <rtems.h>

#define REG_BIT(reg, n) (((reg) >> n) & 1U)
#define REG_BYTE(reg, n) (((reg) >> (n * 4)) & 0x0FU)
#define TIMEOUT 0
#define FIFO_SIZE 256

static Uart uart0;

static Uart_Config config;

static int err;

static uint8_t length;

BYTE_FIFO_CREATE(rxByteFifo, FIFO_SIZE);

BYTE_FIFO_CREATE(txByteFifo, FIFO_SIZE);

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
        Uart_write(&uart0, buff[n], TIMEOUT, &err);
    }
}

static void
debugReg(const uint32_t reg)
{
    for(char n = 0; n < 32; n++) {
        uint8_t byte = 0;
        byte += REG_BIT(reg, n);
        Uart_write(&uart0, '0' + byte, TIMEOUT, &err);
    }
}

static void
write(const char* text)
{
    int n = 0;
    while(*(text + n) != 0) {
        Uart_write(&uart0, *(text + n), TIMEOUT, &err);
        n++;
        if(n > 32) {
            return;
        }
    }
}

static void
testCallback()
{
    ++length;
}

static void
testSync(Uart* uart)
{
    err = 0;
    length = 0;
    Uart_startup(uart);
    Uart_getConfig(uart, &config);
    uart->rxHandler.targetLength = 5;
    uart->rxHandler.lengthCallback = testCallback;
    write("uart status: \n");
    debugAddr(uart->reg->status);
    write("\n");
    debugReg(uart->reg->status);
    write("\n\ncounting characters (x5)\n");
    write("type q to end:\n_\r");
    char buf = '\0';
    do {
        Uart_write(uart, itoc(length), TIMEOUT, &err);
        Uart_write(uart, '\r', TIMEOUT, &err);
        Uart_read(uart, (uint8_t*)&buf, TIMEOUT, &err);
    } while(buf != 'q' && err == 0);
    if(err != 0) {
        Uart_write(uart, '!', TIMEOUT, &err);
        Uart_write(uart, itoc(err), TIMEOUT, &err);
    }
    Uart_write(uart, '\n', TIMEOUT, &err);
    Uart_shutdown(uart);
}

static void
testAsync(Uart* uart)
{
    ByteFifo_clear(&txByteFifo);
    ByteFifo_clear(&rxByteFifo);
    Uart_startup(uart);
    Uart_getConfig(uart, &config);
    const char text1[] = "\ntxByteFifo contains this text\n";
    for(int i = 0; i < (int)sizeof(text1) && i < FIFO_SIZE; i++) {
        ByteFifo_push(&txByteFifo, text1[i]);
    }
    Uart_writeAsync(uart, &txByteFifo, uart->txHandler);
    while(ByteFifo_isEmpty(&txByteFifo) == 0);
    const char text2[] = "now 32 chars will by read via Uart:\n";
    for(int i = 0; i < (int)sizeof(text2) && i < FIFO_SIZE; i++) {
        ByteFifo_push(&txByteFifo, text2[i]);
    }
    Uart_writeAsync(uart, &txByteFifo, uart->txHandler);
    while(ByteFifo_isEmpty(&txByteFifo) == 0);
    do {
        Uart_readAsync(uart, &rxByteFifo, uart->rxHandler);
    } while(Uart_getRxFifoCount(uart) < 32);
    ByteFifo_push(&rxByteFifo, '\n');
    Uart_writeAsync(uart, &rxByteFifo, uart->txHandler);
    while(ByteFifo_isEmpty(&rxByteFifo) == 0);
    Uart_shutdown(uart);
}

int
main()
{
    Uart_init(Uart_Id_0, &uart0);
    uart0.config.isTxEnabled = 1;
    uart0.config.isRxEnabled = 1;
    testSync(&uart0);
    testAsync(&uart0);
    return 0;
}

/** @} */

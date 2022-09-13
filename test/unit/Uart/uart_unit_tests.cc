#include "CppUTest/TestHarness.h"

extern "C"
{
#include "Uart.h"
}

TEST_GROUP(UartTests)
{
    Uart uart;
    Uart_Config config;

    void setup() {
      Uart_init(Uart_Id_0, &uart);
    }
};

TEST(UartTests, Uart_setFlag_ShouldSetProperFlagTo32BitRegister)
{
    volatile uint32_t testRegister = 0;
    uint32_t flagOffset = 2;
    uint32_t properSetResult = 4;
    uint32_t properResetResult = 0;

    Uart_setFlag(&testRegister, FLAG_SET, flagOffset);
    CHECK_EQUAL(properSetResult, testRegister);
    Uart_setFlag(&testRegister, FLAG_RESET, flagOffset);
    CHECK_EQUAL(properResetResult, testRegister);
}

TEST(UartTests, Uart_getFlag_ShouldReturmProperFlagFrom32BitRegister)
{
    volatile uint32_t testRegister = 0b11010;

    CHECK_TRUE(Uart_getFlag(testRegister, 1));
    CHECK_TRUE(Uart_getFlag(testRegister, 3));
    CHECK_TRUE(Uart_getFlag(testRegister, 4));
    CHECK_FALSE(Uart_getFlag(testRegister, 0));
    CHECK_FALSE(Uart_getFlag(testRegister, 2));
}

TEST(UartTests, Uart_setConfig_ShouldWriteProperUartConfigurationToTheControlRegister)
{
    uint32_t expectedValue = 0x3F; // RE, TE, RI, TI, PS, PE

    config.isRxEnabled = true;
    config.isTxEnabled = true;
    config.parity = Uart_Parity_Odd;
    config.isLoopbackModeEnabled = false;

    CHECK_EQUAL(0, uart.reg->control);
    Uart_setConfig(&uart, &config);
    CHECK_EQUAL(expectedValue, uart.reg->control);
}

TEST(UartTests, Uart_getConfig_ShouldReadProperUartConfigurationFromTheControlRegister)
{
    uart.reg->control = 0x8A; // TE, TI, LB

    Uart_getConfig(&uart, &config);
    CHECK_FALSE(config.isRxEnabled);
    CHECK_TRUE(config.isTxEnabled);
    CHECK_TRUE(config.isLoopbackModeEnabled);
    CHECK_TRUE(config.parity == Uart_Parity_None);
}

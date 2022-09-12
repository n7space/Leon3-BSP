#include "CppUTest/TestHarness.h"

extern "C" {
#include "Uart.h"
}

TEST_GROUP(UartTests)
{
};

TEST(UartTests, ShouldSetProperFlagTo32BitRegister)
{
    volatile uint32_t testRegister = 0;
    uint32_t flagOffset = 2;
    uint32_t properSetResult = 4;
    uint32_t properResetResult = 0;

    Uart_setFlag(&testRegister, FLAG_SET, flagOffset);
    CHECK_EQUAL(testRegister, properSetResult);
    Uart_setFlag(&testRegister, FLAG_RESET, flagOffset);
    CHECK_EQUAL(testRegister, properResetResult);
}

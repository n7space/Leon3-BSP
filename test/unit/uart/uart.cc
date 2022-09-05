#include "CppUTest/TestHarness.h"

extern "C" {
#include "Uart.h"
}

TEST_GROUP(UartGroup)
{
};

TEST(UartGroup, testTest)
{
    CHECK_EQUAL(1, 1);
}
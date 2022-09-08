#include "CppUTest/TestHarness.h"

extern "C" {
#include "Uart.h"
}

TEST_GROUP(UartTest)
{
};

TEST(UartTest, ExampleTest)
{
    CHECK_EQUAL(1, 1);
}

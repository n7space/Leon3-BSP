#include "CppUTest/TestHarness.h"

extern "C" {
#include "Timer.h"
}

TEST_GROUP(TimersGroup)
{
};

TEST(TimersGroup, testTest)
{
    CHECK_EQUAL(1, 1);
}

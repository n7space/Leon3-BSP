#include "CppUTest/TestHarness.h"

#include <stdint.h>

extern "C" {
#include "Timer.h"
}

TEST_GROUP(TimerTests)
{
};

TEST(TimerTests, Uart_setFlag_ShouldSetProperFlagTo32BitRegister)
{
    volatile uint32_t testRegister = 0;
    uint32_t flagOffset = 2;
    uint32_t properSetResult = 4;
    uint32_t properResetResult = 0;

    Timer_setFlag(&testRegister, TIMER_FLAG_SET, flagOffset);
    CHECK_EQUAL(properSetResult, testRegister);
    Timer_setFlag(&testRegister, TIMER_FLAG_RESET, flagOffset);
    CHECK_EQUAL(properResetResult, testRegister);
}

TEST(TimerTests, Uart_getFlag_ShouldReturnProperFlagFrom32BitRegister)
{
    volatile uint32_t testRegister = 0b01101;

    CHECK_TRUE(Timer_getFlag(testRegister, 0));
    CHECK_TRUE(Timer_getFlag(testRegister, 2));
    CHECK_TRUE(Timer_getFlag(testRegister, 3));
    CHECK_FALSE(Timer_getFlag(testRegister, 1));
    CHECK_FALSE(Timer_getFlag(testRegister, 4));
}

TEST(TimerTests, Timer_baseInit_shouldInitTimerBaseRegister)
{
    uint32_t baseConfigurationRegister = 0;
    
    Timer_baseInit(&baseConfigurationRegister);

    CHECK_EQUAL(true, Timer_getFlag(baseConfigurationRegister, TIMER_CONFIG_DF));
    CHECK_EQUAL(true, Timer_getFlag(baseConfigurationRegister, TIMER_CONFIG_SI));
    CHECK_EQUAL(false, Timer_getFlag(baseConfigurationRegister, TIMER_CONFIG_EL));
}

TEST(TimerTests, Timer_setConfig_shouldSetSetProperFlagsInTimerRegistersFromTheGivenConfig)
{
    uint32_t testControlRegister;
    uint32_t testReloadRegister;
    uint32_t testReloadValue = 100;
    Timer_Config testConfig = {
        .isAutoReloaded = true,
        .isChained = false,
        .isEnabled = true,
        .isInterruptEnabled = false,
        .reloadValue = testReloadValue
    };

    Timer_setConfig(&testControlRegister, &testReloadRegister, &testConfig);


}

// void Timer_getConfig(const uint32_t timerControlRegister, const uint32_t timerReloadRegister, Timer_Config *const config);
// void Timer_start(volatile uint32_t *const timerControlRegister);
// void Timer_stop(volatile uint32_t *const timerControlRegister);
// bool Timer_hasFinished(const uint32_t timerControlRegister, const uint32_t timerCounterRegister);
// void Timer_handleIrq(Timer_InterruptHandler* const handler);
// void Timer_Apbctrl1_init(Timer_Id id, Timer_Apbctrl1 *const timer, const Timer_InterruptHandler handler);
// void Timer_Apbctrl2_init(Timer_Id id, Timer_Apbctrl2 *const timer, const Timer_InterruptHandler handler);
// uint32_t Timer_Apbctrl1_getCounterValue(const Timer_Apbctrl1 *const timer);
// uint32_t Timer_Apbctrl2_getCounterValue(const Timer_Apbctrl2 *const timer);
// Timer_Apbctrl1_Interrupt Timer_getApbctrl1InterruptNumber(Timer_Id id);
// Timer_Apbctrl2_Interrupt Timer_getApbctrl2InterruptNumber(Timer_Id id);
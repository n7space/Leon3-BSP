#include "CppUTest/TestHarness.h"

#include <stdint.h>

extern "C" {
#include "Timer_private.h"
#include "Timer.h"
}

static inline void
testCallback(bool* arg)
{
    *arg = true;
}

TEST_GROUP(TimerTests)
{
    volatile bool testArg;
    Timer_InterruptHandler testHandler;
    Timer_Apbctrl1 testApbctrl1Timer;
    Timer_Apbctrl2 testApbctrl2Timer;
    Timer_Id id;

    void setup() {
        id = Timer_Id_0;
        testHandler.callback = (Timer_InterruptCallback)testCallback;
        testHandler.arg = &testArg;
        testArg = false;
    }
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

    CHECK_TRUE(Timer_getFlag(baseConfigurationRegister, TIMER_CONFIG_DF));
    CHECK_TRUE(Timer_getFlag(baseConfigurationRegister, TIMER_CONFIG_SI));
    CHECK_FALSE(Timer_getFlag(baseConfigurationRegister, TIMER_CONFIG_EL));
}

TEST(TimerTests, Timer_setConfigRegisters_shouldSetProperFlagsInTimerRegistersFromTheGivenConfig)
{
    uint32_t testControlRegister = 0;
    uint32_t testReloadRegister = 0;
    uint32_t expectedControlValue = 0x03; // EN, RS 
    uint32_t expectedReloadValue = 100;
    Timer_Config testConfig = {
        .isInterruptEnabled = false,
        .isEnabled = true,
        .isAutoReloaded = true,
        .isChained = false,
        .reloadValue = expectedReloadValue
    };

    Timer_setConfigRegisters(&testControlRegister, &testReloadRegister, &testConfig);

    CHECK_EQUAL(expectedControlValue, testControlRegister);
    CHECK_EQUAL(expectedReloadValue, testReloadRegister);
}

TEST(TimerTests, Timer_getConfigRegisters_shouldReturnProperConfigFromTheTimerRegisters)
{
    uint32_t testControlRegister = 0x23; // EN, RS, CH 
    uint32_t testReloadRegister = 15;
    Timer_Config testConfig;

    Timer_getConfigRegisters(testControlRegister, testReloadRegister, &testConfig);

    CHECK_TRUE(testConfig.isEnabled);
    CHECK_TRUE(testConfig.isAutoReloaded);
    CHECK_TRUE(testConfig.isChained);
    CHECK_FALSE(testConfig.isInterruptEnabled);
    CHECK_EQUAL(testReloadRegister, testConfig.reloadValue);
}

TEST(TimerTests, Timer_start_shouldSetTheReloadFlagAndTheEnableFlagToStartTheTimer)
{
    uint32_t testControlRegister = 0;
    uint32_t expectedControlValue = 5; // EN, LD

    Timer_start(&testControlRegister);

    CHECK_EQUAL(expectedControlValue, testControlRegister);
}

TEST(TimerTests, Timer_stop_shouldResetThTheEnableFlagToStopTheTimer)
{
    uint32_t testControlRegister = 3; // EN, RS
    uint32_t expectedControlValue = 2; // RS

    Timer_stop(&testControlRegister);

    CHECK_EQUAL(expectedControlValue, testControlRegister);
}

TEST(TimerTests, Timer_hasFinished_shouldResetThTheEnableFlagToStopTheTimer)
{
    uint32_t testControlRegister = 1; // EN
    uint32_t testCounterRegister = 50; // Non TIMER_UNDERFLOWED value

    CHECK_FALSE(Timer_hasFinished(testControlRegister, testCounterRegister));
    testControlRegister = 0;
    CHECK_FALSE(Timer_hasFinished(testControlRegister, testCounterRegister));
    testControlRegister = 1;
    testCounterRegister = TIMER_UNDERFLOWED;
    CHECK_FALSE(Timer_hasFinished(testControlRegister, testCounterRegister));
    testControlRegister = 0;
    testCounterRegister = TIMER_UNDERFLOWED;
    CHECK_TRUE(Timer_hasFinished(testControlRegister, testCounterRegister));
}

TEST(TimerTests, Timer_handleIrq_shouldRunInterruptCallbackFunction)
{
    CHECK_FALSE(testArg);
    Timer_handleIrq(&testHandler);
    CHECK_TRUE(testArg);
}

TEST(TimerTests, Timer_Apbctrl1_init_shouldInitializeTheTimerRegistersAndResetThem)
{
    Timer_Id id = Timer_Id_0;

    Timer_Apbctrl1_init(id, &testApbctrl1Timer, testHandler);

    CHECK_EQUAL(testHandler.callback, testApbctrl1Timer.irqHandler.callback);
    CHECK_EQUAL(testHandler.arg, testApbctrl1Timer.irqHandler.arg);
    CHECK_EQUAL(id, testApbctrl1Timer.id);
    CHECK_EQUAL(0, testApbctrl1Timer.regs->control);
    CHECK_EQUAL(0, testApbctrl1Timer.regs->counter);
    CHECK_EQUAL(0, testApbctrl1Timer.regs->reload);
}

TEST(TimerTests, Timer_Apbctrl2_init_shouldInitializeTheTimerRegistersAndResetThem)
{
    Timer_Apbctrl2_init(id, &testApbctrl2Timer, testHandler);

    CHECK_EQUAL(testHandler.callback, testApbctrl2Timer.irqHandler.callback);
    CHECK_EQUAL(testHandler.arg, testApbctrl2Timer.irqHandler.arg);
    CHECK_EQUAL(id, testApbctrl2Timer.id);
    CHECK_EQUAL(0, testApbctrl2Timer.regs->control);
    CHECK_EQUAL(0, testApbctrl2Timer.regs->counter);
    CHECK_EQUAL(0, testApbctrl2Timer.regs->reload);
}

TEST(TimerTests, Timer_Apbctrl1_getCounterValue_shouldReturnCorrectTimerCounterValue)
{
    uint32_t testCounterValue = 123;

    Timer_Apbctrl1_init(id, &testApbctrl1Timer, testHandler);
    testApbctrl1Timer.regs->counter = testCounterValue;

    CHECK_EQUAL(testCounterValue, Timer_Apbctrl1_getCounterValue(&testApbctrl1Timer));
}

TEST(TimerTests, Timer_Apbctrl2_getCounterValue_shouldReturnCorrectTimerCounterValue)
{
    uint32_t testCounterValue = 456;

    Timer_Apbctrl2_init(id, &testApbctrl2Timer, testHandler);
    testApbctrl2Timer.regs->counter = testCounterValue;

    CHECK_EQUAL(testCounterValue, Timer_Apbctrl2_getCounterValue(&testApbctrl2Timer));
}

TEST(TimerTests, Timer_getApbctrl1InterruptNumber_shouldReturnProperInterruptNumber)
{
    CHECK_EQUAL(Timer_Apbctrl1_Interrupt_Invalid, Timer_getApbctrl1InterruptNumber(Timer_Id_0));
    CHECK_EQUAL(Timer_Apbctrl1_Interrupt_1, Timer_getApbctrl1InterruptNumber(Timer_Id_1));
    CHECK_EQUAL(Timer_Apbctrl1_Interrupt_2, Timer_getApbctrl1InterruptNumber(Timer_Id_2));
    CHECK_EQUAL(Timer_Apbctrl1_Interrupt_3, Timer_getApbctrl1InterruptNumber(Timer_Id_3));
    CHECK_EQUAL(Timer_Apbctrl1_Interrupt_4, Timer_getApbctrl1InterruptNumber(Timer_Id_4));
    CHECK_EQUAL(Timer_Apbctrl1_Interrupt_Invalid, Timer_getApbctrl1InterruptNumber((Timer_Id)15));
}

TEST(TimerTests, Timer_getApbctrl2InterruptNumber_shouldReturnProperInterruptNumber)
{
    CHECK_EQUAL(Timer_Apbctrl2_Interrupt_Invalid, Timer_getApbctrl2InterruptNumber(Timer_Id_0));
    CHECK_EQUAL(Timer_Apbctrl2_Interrupt_1, Timer_getApbctrl2InterruptNumber(Timer_Id_1));
    CHECK_EQUAL(Timer_Apbctrl2_Interrupt_1, Timer_getApbctrl2InterruptNumber(Timer_Id_2));
    CHECK_EQUAL(Timer_Apbctrl2_Interrupt_Invalid, Timer_getApbctrl2InterruptNumber((Timer_Id)15));
}

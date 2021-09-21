#include "system.h"

#include <stdio.h>
#include "main.h"
#include "globals.h"
#include "util.h"
#include "analogSensors.h"
#include "led.h"
#include "apa102.h"
#include "spi.h"
#include "button.h"
#include "adc.h"

// used to indicate if all conditions are met to wake up again (filtering out spurious button presses etc.)
bool systemValidWakeup = false;

systemState_t curSysState = SysState::Idle;

// TODO: calibrate values
const uint32_t autoBrightLUT_sensor[] =    {    0,  100, 3000, 4095 };
const uint32_t autoBrightLUT_led[] =       {    1,    1,   31,   31 };
const size_t   autoBrightLUT_numVals = sizeof(autoBrightLUT_sensor) / sizeof(autoBrightLUT_sensor[0]);

uint32_t lastAnalogSensorUpdate = 0;

void systemLoop() {
    // check battery for undervoltage
    if (getVcc() < BATTERY_CRITICAL_THRESHHOLD && HAL_GetTick() > ANALOG_SENSORS_INIT_TIME) {
        systemShutdown();
    }

    // do automatic brightness
    if (HAL_GetTick() - lastAnalogSensorUpdate >= ANALOG_SENSOR_SAMPLE_RATE) {
        lastAnalogSensorUpdate = HAL_GetTick();
        // brightness value is already smoothed, so only need to apply it
        uint16_t measuredBrightness = getAmbientLight();
        uint8_t brightness = linearInterpolate(measuredBrightness, autoBrightLUT_sensor, autoBrightLUT_led, autoBrightLUT_numVals);
        // setGlobalBrightness(brightness);
        printf("%8ld [Bright] Sensor: %4d, LED: %2d\n", HAL_GetTick(), measuredBrightness, brightness);
    }
}

void systemDisableModules() {
    // disable ADC (saves about 1mA)
    __HAL_ADC_DISABLE(&hadc1);
    // disable LEDs
    GPIO_InitTypeDef GPIO_InitStruct = {.Pin = GPIO_PIN_13 | GPIO_PIN_15, .Mode = GPIO_MODE_INPUT}; 
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);     // disable SPI pins so it doesn't draw power from there
    HAL_GPIO_WritePin(LED_ENABLE_GPIO_Port, LED_ENABLE_Pin, GPIO_PIN_RESET);
    // disable mic amp
    HAL_GPIO_WritePin(MIC_ENABLE_GPIO_Port, MIC_ENABLE_Pin, GPIO_PIN_RESET);
    // TODO disable IMU
    // TODO disable NRF
}

void systemEnableModules() {
    __HAL_ADC_ENABLE(&hadc1);
    // enable LEDs
    GPIO_InitTypeDef GPIO_InitStruct = {.Pin = GPIO_PIN_13 | GPIO_PIN_15, .Mode = GPIO_MODE_AF_PP, .Pull = GPIO_NOPULL, .Speed = GPIO_SPEED_FREQ_HIGH};
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);     // re-enable SPI pins
    HAL_GPIO_WritePin(LED_ENABLE_GPIO_Port, LED_ENABLE_Pin, GPIO_PIN_SET);
    // enable mic amp
    HAL_GPIO_WritePin(MIC_ENABLE_GPIO_Port, MIC_ENABLE_Pin, GPIO_PIN_SET);
    // TODO enable IMU
    // TODO enable NRF
}

void systemShutdown() {
    printf("[SYS] UNDEVOLTAGE DETECTED! (%d mV) SHUTTING DOWN!\n", getVcc());

    // blink leds 3 times
    ledBlink(3, 250, 0xFF0000);

    systemDisableModules();
    
    // shutdown STM32
    HAL_PWR_EnterSTANDBYMode();
}

void systemStandby() {
    printf("%8ld [SYS] Entering Standby\n", HAL_GetTick());

    systemDisableModules();
    systemValidWakeup = false;
    curSysState = SysState::InStandby;

    do {
        HAL_SuspendTick();                      // suspend tick, otherwise the SysTick interrupt would wake the controller

        HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI); // actually go to sleep

        SystemClock_Config();                   // re-configure clock, because after STOP the STM32 will start up with HSI at 8MHz
        HAL_ResumeTick();                       // resume tick again

        // check if the received interrupt was a valid wakeup reason
        uint32_t backToSleepTime = HAL_GetTick();
        while (HAL_GetTick() - backToSleepTime < BACK_TO_STANDBY_TIMEOUT && systemValidWakeup == false) {
            buttonLoop();   // will set validWakeup to true, when conditions are met
        }
        if (HAL_GetTick() - backToSleepTime >= BACK_TO_STANDBY_TIMEOUT && systemValidWakeup == false) {
            printf("%8ld [SYS] No valid wakeup reason found, going back to sleep...\n", HAL_GetTick());
        }
    }  while (systemValidWakeup == false);

    printf("%8ld [SYS] Exiting Standby\n", HAL_GetTick());
    curSysState = SysState::Idle;
    systemEnableModules();
}

void systemSetValidWakeup(bool state) {
    systemValidWakeup = state;
}

systemState_t systemGetState() {
    return curSysState;
}
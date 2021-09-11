#include "system.h"

#include <stdio.h>
#include "main.h"
#include "globals.h"
#include "util.h"
#include "analogSensors.h"
#include "led.h"
#include "apa102.h"

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
        setGlobalBrightness(brightness);
    }
}

void systemShutdown() {
    printf("UNDEVOLTAGE DETECTED! (%d mV) SHUTTING DOWN!\n", getVcc());

    // blink leds 3 times
    ledBlink(3, 250, 0xFF0000);

    // disable LEDs
    HAL_GPIO_WritePin(LED_ENABLE_GPIO_Port, LED_ENABLE_Pin, GPIO_PIN_RESET);
    // disable mic amp
    HAL_GPIO_WritePin(MIC_ENABLE_GPIO_Port, MIC_ENABLE_Pin, GPIO_PIN_RESET);
    // TODO disable IMU
    // TODO disable NRF
    // shutdown STM32
    HAL_PWR_EnterSTANDBYMode();
}

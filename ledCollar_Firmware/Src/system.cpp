#include "system.h"

#include <stdio.h>
#include "main.h"
#include "globals.h"
#include "analogSensors.h"
#include "led.h"

void systemLoop() {
    if (getVcc() < BATTERY_CRITICAL_THRESHHOLD && HAL_GetTick() > ANALOG_SENSORS_INIT_TIME) {
        systemShutdown();
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

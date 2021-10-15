#pragma once

#include <stdint.h>
#include "gpio.h"

#define DEBOUNCE_TIME   1
#define CLICK_TIME      300     // ms, idle time until a click to registers (to wait for multiple clicks)
#define HOLD_TIME       1000    // ms, time until the button counts as hold
#define SHUTDOWN_TIME   3000    // ms, time until long press turns off the device

// because of EXTI behaviour, currently only supports differentiating based on pin number, not port
GPIO_TypeDef* const btnPort[] =   {BTN_GPIO_Port};
const uint16_t btnPin[] =         {BTN_Pin}; 
const uint8_t btnCount = sizeof(btnPort) / sizeof(btnPort[0]);

typedef enum {
    none = 0,
    single_pressed,
    single_released,
    double_pressed,
    double_released,
    triple_pressed,
    triple_released,
    still_held
} click_t;

void buttonInit();
void buttonLoop();
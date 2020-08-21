#pragma once

#include <stdint.h>
#include "gpio.h"

#define DEBOUNCE_TIME   5
#define CLICK_TIME      300     // maximum hold time in ms for a click to register
#define HOLD_TIME       1000    // time until the button counts as hold

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
    hold_released
} click_t;

void buttonInit();
void buttonLoop();
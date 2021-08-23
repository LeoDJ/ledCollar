#pragma once
#include "stm32f1xx_hal.h"

#define BATTERY_CRITICAL_THRESHHOLD 3000    // mV, battery voltage limit under which everything goes into complete shutdown, to draw as little power as possible

#define PRINTF_UART huart2
#define DEBUG_UART  huart1
#define LED_SPI     hspi2

extern UART_HandleTypeDef   PRINTF_UART;
extern UART_HandleTypeDef   DEBUG_UART;

#define PI 3.141592653589793
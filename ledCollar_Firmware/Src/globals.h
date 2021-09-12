#pragma once
#include "stm32f1xx_hal.h"

#define BATTERY_CRITICAL_THRESHHOLD 3300    // mV, battery voltage limit under which everything goes into complete shutdown, to draw as little power as possible
#define BACK_TO_STANDBY_TIMEOUT     3000    // ms, time until system goes back to sleep when no valid wakeup reason was found

#define PRINTF_UART huart2
#define DEBUG_UART  huart1
#define LED_SPI     hspi2

extern UART_HandleTypeDef   PRINTF_UART;
extern UART_HandleTypeDef   DEBUG_UART;

#define PI 3.141592653589793
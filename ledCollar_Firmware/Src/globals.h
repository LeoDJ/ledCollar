#pragma once
#include "stm32f1xx_hal.h"

#define PRINTF_UART huart1
#define LED_SPI     hspi2

extern UART_HandleTypeDef   PRINTF_UART;
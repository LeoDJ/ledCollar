#pragma once
#include "ledAnimation.h"

#define UPDATE_RATE 120 // FPS
#define INITIAL_BATTERY_SHOW_TIME 5000

void initLed();
void loopLed();
void ledNextAnimation();
void ledRandomAnimation();
void ledBlink(uint32_t blinks, uint32_t delay, uint32_t rgb);   // blocking
void ledDisplayBatteryPercent(uint8_t battPercent);

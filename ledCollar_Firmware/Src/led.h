#pragma once
#include "ledAnimation.h"

#define UPDATE_RATE 120 // FPS

void initLed();
void loopLed();
void ledNextAnimation();
void ledRandomAnimation();
void ledBlink(uint32_t blinks, uint32_t delay, uint32_t rgb);
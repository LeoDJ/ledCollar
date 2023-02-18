#include "led.h"
#include "apa102.h"
#include "analogSensors.h"

#include <stdlib.h>

// TODO: remove function when using LED strip with correct length
// void setLedFunc(uint16_t index, uint32_t rgb) {
//     setLed(index + 103, rgb);
// }

uint8_t currentAnimation = 4;


void initLed() {
    initApa102();
    setGlobalBrightness(1);
    initAnimation(NUM_LEDS, setLed);
    setAnimation(currentAnimation);
    // setAnimation("vu1");
    // setAnimationIntensity(255);
    // stepAnimation();
}

uint32_t lastLedUpdate = 0;
uint32_t lastAnimationNext = 0;
bool shownBatteryStatus = false;

void loopLed() {
    // after boot, show battery state for a few seconds
    if (!shownBatteryStatus) {
        uint8_t linearBattPercent = (getVcc() - 3000) / 12;
        ledDisplayBatteryPercent(linearBattPercent);

        if (HAL_GetTick() > INITIAL_BATTERY_SHOW_TIME) {
            shownBatteryStatus = true;
        }
    }
    // TODO: replace this with timer
    else if (HAL_GetTick() - lastLedUpdate > 1000 / UPDATE_RATE) { // only do update if update rate allows it
        lastLedUpdate = HAL_GetTick();
        
        stepAnimation();
        doLedTransfer();
    }

    if (HAL_GetTick() - lastAnimationNext >= 10000) {
        // resetting lastAnimationNext happens in the function
        // ledNextAnimation();
        ledRandomAnimation();
    }
}

void ledNextAnimation() {
    lastAnimationNext = HAL_GetTick();  // also reset next animation counter on manual call of this function
    currentAnimation++;
    if (currentAnimation >= getAnimationCount()) {
        currentAnimation = 0;
    }
    setAnimation(currentAnimation);
}

void ledRandomAnimation() {
    lastAnimationNext = HAL_GetTick();  // also reset next animation counter on manual call of this function
    uint8_t randNum = rand() % getAnimationCount();
    if (randNum == currentAnimation) {  // if it choses the same animation, try again
        ledRandomAnimation();
        return;
    }
    currentAnimation = randNum;
    setAnimation(currentAnimation);
}

void ledBlink(uint32_t blinks, uint32_t delay, uint32_t rgb) {
    for (uint32_t i = 0; i < blinks; i++) {
        HAL_Delay(delay);
        fillLed(rgb);
        doLedTransfer();
        HAL_Delay(delay);
        fillLed(0x000000);
        doLedTransfer();
    }
}

void ledDisplayBatteryPercent(uint8_t battPercent) {
    uint8_t numberOfLedsToShow = NUM_LEDS *  battPercent / 100;
    fillLed(0);
    for (int i = 0; i < numberOfLedsToShow; i++) {
        uint8_t iScaled = i * 255 / (NUM_LEDS-1);
        setLed(i, (255 - iScaled) << 16 | iScaled << 8);
    }
    doLedTransfer();
}
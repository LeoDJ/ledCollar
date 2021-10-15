#include "led.h"
#include "apa102.h"

// TODO: remove function when using LED strip with correct length
// void setLedFunc(uint16_t index, uint32_t rgb) {
//     setLed(index + 103, rgb);
// }

uint8_t currentAnimation = 5;


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

void loopLed() {
    // TODO: replace this with timer
    if(HAL_GetTick() - lastLedUpdate > 1000 / UPDATE_RATE) { // only do update if update rate allows it
        lastLedUpdate = HAL_GetTick();
        
        stepAnimation();
        doLedTransfer();
    }

    if (HAL_GetTick() - lastAnimationNext >= 10000) {
        lastAnimationNext = HAL_GetTick();
        ledNextAnimation();
    }
}

void ledNextAnimation() {
    currentAnimation++;
    if(currentAnimation >= getAnimationCount()) {
        currentAnimation = 0;
    }
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
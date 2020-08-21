#include "led.h"
#include "apa102.h"

// TODO: remove function when using LED strip with correct length
// void setLedFunc(uint16_t index, uint32_t rgb) {
//     setLed(index + 103, rgb);
// }

uint8_t currentAnimation = 0;


void initLed() {
    initApa102();
    setGlobalBrightness(3);
    initAnimation(NUM_LEDS, setLed);
    setAnimation(currentAnimation);
    // setAnimation("vu1");
    // setAnimationIntensity(255);
    // stepAnimation();
}

uint32_t lastLedUpdate = 0;

void loopLed() {
    // TODO: replace this with timer
    if(HAL_GetTick() - lastLedUpdate > 1000 / UPDATE_RATE) { // only do update if update rate allows it
        lastLedUpdate = HAL_GetTick();
        
        stepAnimation();
        doLedTransfer();
    }
}

void ledNextAnimation() {
    currentAnimation++;
    if(currentAnimation >= getAnimationCount()) {
        currentAnimation = 0;
    }
    setAnimation(currentAnimation);
}
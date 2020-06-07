#include "led.h"
#include "apa102.h"
#include "ledAnimation.h"

// TODO: remove function when using LED strip with correct length
void setLedFunc(uint16_t index, uint32_t rgb) {
    setLed(index + 102, rgb);
}

void initLed() {
    initApa102();
    setGlobalBrightness(4);
    initAnimation(26, setLedFunc);
    setAnimation((uint8_t)7);
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
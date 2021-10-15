#include "button.h"
#include "led.h"
#include "system.h"
#include <stdio.h>
#include "apa102.h"

uint32_t btnLastChange[btnCount];
click_t btnLastState[btnCount];


const uint8_t brightnessSteps[] = {1, 4, 8, 12, 20, 31};
const uint8_t brightnessStepsNum = sizeof(brightnessSteps) / sizeof(brightnessStepsNum);
uint8_t curBrightnessStep = 1; // start with 1 as brightness 1 is already start value

// TODO refactor (so that the differentiation by system state is on the first level)
void handleButton(uint8_t btnIndex, click_t clickType = single_released, uint32_t holdTime = 0) {
    if (btnIndex == 0) {
        switch (clickType) {
            case single_released:
                if (holdTime < HOLD_TIME) { // button was pressed and released
                    if (systemGetState() == SysState::InStandby) {
                        systemSetValidWakeup(true); // for now simply wake up after a single press
                    }
                    else {
                        printf("%8d [BTN] nextAnimation\n", HAL_GetTick());
                        ledNextAnimation();
                    }
                }
                else if (holdTime < SHUTDOWN_TIME) { // button was held and released
                    
                }
                else {  // button was held and released long enough to shut down

                }
                break;
            case single_pressed:
                if (holdTime > HOLD_TIME) { // button was held
                    printf("%8d [BTN] increaseBrightness\n", HAL_GetTick());
                    setGlobalBrightness(brightnessSteps[curBrightnessStep]);
                    curBrightnessStep++;
                    if (curBrightnessStep >= brightnessStepsNum) {
                        curBrightnessStep = 0;
                    }
                }
                break;
            case still_held:
                if (holdTime > SHUTDOWN_TIME) { // button was held long enough for shutdown (still triggers the normal hold-action before)
                    /*if (systemGetState() != SysState::InStandby) {
                        systemStandby();
                        // TODO: show standby animation instead
                    }*/
                }
                break;
            default:
                break;
        }
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t pin) {
    // search button pin in button definition
    for(uint8_t i = 0; i < btnCount; i++) {
        if(btnPin[i] == pin) {
            uint8_t btnState = HAL_GPIO_ReadPin(btnPort[i], btnPin[i]);
            // printf("%8d [BTN] %d, ", HAL_GetTick(), btnState);
                if(HAL_GetTick() - btnLastChange[i] > DEBOUNCE_TIME) {
                    if(!btnState && (btnLastState[i] % 2 == 0) && btnLastState[i] < triple_released) { // button just got pressed
                        btnLastState[i] = (click_t)(btnLastState[i] + 1);
                    }
                    else if(btnState && (btnLastState[i] % 2 == 1) && btnLastState[i] < triple_released) { // button just got released
                        btnLastState[i] = (click_t)(btnLastState[i] + 1);
                    }
                    else if(btnState && btnLastState[i] == still_held) { // reset held state upon release of button
                        btnLastState[i] = none;
                    }

                    // printf(" last: %d %8d", btnLastState[i], btnLastChange[i]);

                }
                btnLastChange[i] = HAL_GetTick();
        }
    }
    // printf("\n");
}

void buttonInit() {
    
}

void buttonLoop() {
    for(uint8_t i = 0; i < btnCount; i++) {
        click_t s = btnLastState[i];
        if(s == single_released || s == double_released || s == triple_released) {
            if(HAL_GetTick() - btnLastChange[i] > CLICK_TIME) {
                btnLastState[i] = none;
                handleButton(i, s, HAL_GetTick() - btnLastChange[i]);
            }
        }
        else if(s == single_pressed && HAL_GetTick() - btnLastChange[i] > HOLD_TIME) {
            btnLastState[i] = still_held;
            handleButton(i, s, HAL_GetTick() - btnLastChange[i]);
        }
        else if(s == still_held && HAL_GetTick() - btnLastChange[i] > SHUTDOWN_TIME) {
            btnLastState[i] = none;
            handleButton(i, s, HAL_GetTick() - btnLastChange[i]);
        }
    }
}
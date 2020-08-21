#include "button.h"
#include "led.h"

uint32_t btnLastChange[btnCount];
click_t btnLastState[btnCount];


void handleButton(uint8_t btnIndex, click_t clickType = single_released, uint32_t holdTime = 0) {
    if(btnIndex == 0) {
        switch(clickType) {
            case single_pressed:
                if(holdTime > HOLD_TIME) {
                    
                }
                break;
            case single_released:
                if(holdTime < HOLD_TIME) {
                    ledNextAnimation();
                }
                else { // button was held and released
                    
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
            // if(btnState != btnLastState[i]) {
                // ledNextAnimation();
                if(HAL_GetTick() - btnLastChange[i] > DEBOUNCE_TIME) {
                    if(!btnState && (btnLastState[i] % 2 == 0) && btnLastState[i] < triple_released) { // button just got pressed
                        btnLastState[i] = (click_t)(btnLastState[i] + 1);
                    }
                    else if(btnState && (btnLastState[i] % 2 == 1) && btnLastState[i] < triple_released) { // button just got released
                        btnLastState[i] = (click_t)(btnLastState[i] + 1);
                    }

                    btnLastChange[i] = HAL_GetTick();
                }
            // }
        }
    }
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
            btnLastState[i] = none;
            handleButton(i, s, HAL_GetTick() - btnLastChange[i]);
        }
    }
}
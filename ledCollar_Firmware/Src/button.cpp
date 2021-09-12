#include "button.h"
#include "led.h"
#include "system.h"
#include <stdio.h>

uint32_t btnLastChange[btnCount];
click_t btnLastState[btnCount];


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
                    
                }
                break;
            case still_held:
                if (holdTime > SHUTDOWN_TIME) { // button was held long enough for shutdown (still triggers the normal hold-action before)
                    if (systemGetState() != SysState::InStandby) {
                        systemStandby();
                        // TODO: show standby animation instead
                    }
                }
                break;
            default:
                break;
        }
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t pin) {
    // search button pin in button definition
    // printf("%8d %d", HAL_GetTick(), HAL_GPIO_ReadPin(btnPort[0], btnPin[0]));
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

                    // printf(" %d",  btnLastState[0]);

                    btnLastChange[i] = HAL_GetTick();
                }
            // }
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
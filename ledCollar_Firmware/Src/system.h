#pragma once

struct SysState {
    enum SystemStatesEnum {
        Idle,
        InStandby
    };
};
typedef enum SysState::SystemStatesEnum systemState_t;

void systemLoop();
void systemShutdown();
void systemStandby();
void systemSetValidWakeup(bool state);
systemState_t systemGetState();

#pragma once
#include <stdint.h>
#include <stdbool.h>
#define NUM_LEDS        128
#define MAX_UPDATE_RATE 250 // FPS

#pragma pack(push, 1)
typedef union {
    struct {
        uint8_t global : 5;
        uint8_t start : 3;  // always 111
        union {
            struct {
                uint8_t b;
                uint8_t g;
                uint8_t r;
            };
            uint32_t rawColor : 24;
        };
    };
    uint32_t raw;
} ledData_t;
#pragma pack(pop)

void initApa102();
void doLedTransfer();

// sets APA102 brightness register 0-31
void setGlobalBrightness(uint8_t brightness);

void setLed(uint16_t index, uint32_t rgb);
void setLed(uint16_t index, uint8_t r, uint8_t g, uint8_t b);
void setLed(uint16_t index, ledData_t data, bool onlyColor);
// void setLed(uint16_t index, ledData_t data) {setLed(index, data, true);}
#include "apa102.h"

#include "spi.h"
#include "string.h"

#include "globals.h"
#include "sound.h"

ledData_t ledBuf[NUM_LEDS + 2];

uint8_t ledBufBytes[sizeof(ledBuf)];

void setGlobalBrightness(uint8_t brightness) {
    if(brightness > 31) {
        brightness = 31;
    }

    for(int i = 1; i < NUM_LEDS + 1; i++) {
        ledBuf[i].global = brightness;
    }
}

void setLed(uint16_t index, uint32_t rgb) {
    if(index < NUM_LEDS) {
        ledBuf[index + 1].rawColor = rgb;
    }
}

void setLed(uint16_t index, uint8_t r, uint8_t g, uint8_t b) {
    if(index < NUM_LEDS) {
        ledBuf[index + 1].r = r;
        ledBuf[index + 1].g = g;
        ledBuf[index + 1].b = b;
    }
}

void setLed(uint16_t index, ledData_t data, bool onlyColor) {
    if(index < NUM_LEDS) {
        if(onlyColor) {
            ledBuf[index + 1].rawColor = data.rawColor;
        }
        else {
            ledBuf[index + 1] = data;
        }
    }
}

uint16_t ledIndex = 0;
uint32_t lastTransfer = -1000; // ensure first ledTransfer succeeds
uint16_t maxVal = 0;

void doLedTransfer() {

    if(HAL_GetTick() - lastTransfer < 1000 / MAX_UPDATE_RATE) { // only do update if update rate allows it
        return;
    }
    lastTransfer = HAL_GetTick();

    // test animation
    // setLed(ledIndex % NUM_LEDS, 0);
    // setLed((ledIndex + 1) % NUM_LEDS, 0xFFFFFF);
    // ledIndex++;
    // if(ledIndex == NUM_LEDS) {
    //     ledIndex = 0;
    // }

    // super basic music reactive
    // int16_t ledPeak = (micMaxVal - 2048) / 4;
    // if(ledPeak < 0) ledPeak = 0;
    // for(int i = 0; i < NUM_LEDS; i++) {
    //     setLed(i, i < ledPeak ? 0xFFFFFF : 0);
    // }
    // micMaxVal = (float)micMaxVal / 1.02;
    
    // start new transfer
    HAL_SPI_Transmit_DMA(&LED_SPI, (uint8_t *) ledBuf, sizeof(ledBuf));
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
    if(hspi == &LED_SPI) {
        // doLedTransfer();
    }
}

void initApa102() {
    ledBuf[0].raw = 0x00000000;
    ledBuf[NUM_LEDS + 1].raw = 0xFFFFFFFF;

    for(int i = 1; i < NUM_LEDS + 1; i++) {
        ledBuf[i].start = 0b111;
    }

    setGlobalBrightness(31);
    
    doLedTransfer();
    // HAL_SPI_Transmit_DMA(&LED_SPI, (uint8_t *) ledBuf, sizeof(ledBuf));


    // test frame
    // setGlobalBrightness(2);
    // setLed(0, 0xFF0000);
    // setLed(1, 0x00FF00);
    // setLed(2, 0x0000FF);
}
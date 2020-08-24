#include "apa102.h"

#include "spi.h"
#include "string.h"

#include "globals.h"
#include "sound.h"

// trailing zeroes protocol quirks: https://cpldcpu.wordpress.com/2014/11/30/understanding-the-apa102-superled/
// tl;dr clock gets inverted, so for every 2 LEDs, have to shift in additional irrelevant bit (all 0 or 1) so last LEDs get clock pulses too
#define PADDING_BYTES   (((NUM_LEDS + 15) / 2 / 8) + 4) // +15 so it always gets rounded up to the next byte, +4 for the end frame
uint8_t ledByteBuf[4 + PADDING_BYTES + sizeof(ledData_t) * NUM_LEDS];
ledData_t* ledBuf = (ledData_t*)&ledByteBuf[4]; // skip first 4 init bytes in raw buffer

void setGlobalBrightness(uint8_t brightness) {
    if(brightness > 31) {
        brightness = 31;
    }

    for(int i = 0; i < NUM_LEDS; i++) {
        ledBuf[i].global = brightness;
    }
}

void setLed(uint16_t index, uint32_t rgb) {
    if(index < NUM_LEDS) {
        ledBuf[index].rawColor = rgb;
    }
}

void setLed(uint16_t index, uint8_t r, uint8_t g, uint8_t b) {
    if(index < NUM_LEDS) {
        ledBuf[index].r = r;
        ledBuf[index].g = g;
        ledBuf[index].b = b;
    }
}

void setLed(uint16_t index, ledData_t data, bool onlyColor) {
    if(index < NUM_LEDS) {
        if(onlyColor) {
            ledBuf[index].rawColor = data.rawColor;
        }
        else {
            ledBuf[index] = data;
        }
    }
}

uint32_t getLed(uint16_t index) {
    if(index < NUM_LEDS) {
        return ledBuf[index].rawColor;
    }
    else {
        return 0;
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
    HAL_SPI_Transmit_DMA(&LED_SPI, (uint8_t *) ledByteBuf, sizeof(ledByteBuf));
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
    if(hspi == &LED_SPI) {
        // doLedTransfer();
    }
}

void initApa102() {
    memset(ledByteBuf, 0, 4);
    memset(ledByteBuf + 4 + (sizeof(ledData_t) * NUM_LEDS), 0, PADDING_BYTES);
    for(int i = 0; i < NUM_LEDS; i++) {
        ledBuf[i].start = 0b111;
    }

    setGlobalBrightness(2);
    
    doLedTransfer(); // initialize LED strip

    // test frame
    // setGlobalBrightness(2);
    // setLed(0, 0xFF0000);
    // setLed(1, 0x00FF00);
    // setLed(2, 0x0000FF);
}
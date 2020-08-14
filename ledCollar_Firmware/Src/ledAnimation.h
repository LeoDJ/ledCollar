/* Portable LED strip animation "library"
 * <strike>Written</strike> Cobbled together by LeoDJ
 */

#pragma once
#include <stdint.h>
#include <string.h>
#include "globals.h" // only used to check for FastLED flag

#define ANIMATION_USE_INTERNAL_BUFFER // consumes more memory, but enables linear scaling of complete strip fades

#ifdef ANIMATION_USE_FASTLED
#include <FastLED.h>
void initAnimation(uint16_t numLeds);
#else
// The setLed and getLed parameters expect function pointers using the color format 0x00RRGGBB
// If using a different LED library, you have to provide functions for hsv 2 rgb conversion and LED setting yourself
// hsv2rgb expects its parameters in the range of 0-255 and returns a 32 bit RGB value (0x00RRGGBB)
void initAnimation( uint16_t numLeds, 
                    void (*setLed)(uint16_t index, uint32_t rgb),
                    uint32_t (*hsv2rgb)(uint8_t hue, uint8_t sat, uint8_t val));
// use internal hsv2rgb
void initAnimation( uint16_t numLeds, 
                    void (*setLed)(uint16_t index, uint32_t rgb));
#endif

typedef struct {
    const char *animName;
    void (*funcPtr)();
} anim_t;

// Set the currently active animation by id, returns false if id is outside bounds
bool setAnimation(uint8_t animId);

// Set the currently active animation by name, returns false if no animation was found
bool setAnimation(const char* animName);

// Convert the identifying name of an animation to its id, returns -1 if no animation was found
int animationNameToId(const char* name);

// Step the currently running animation
void stepAnimation();

// Return the currently running animation name
const char* getCurAnimationName();

// Set base color of animations supporting it, 0xRRGGBB
void setAnimationBaseColor(uint32_t rgb);

// Set animation intensity, used for e.g. music reactive animations
void setAnimationIntensity(uint8_t val);

// Set animation intensity optimized for beat detection, used for e.g. music reactive animations
void setAnimationIntensityBeatDetect(uint8_t val);

anim_t *getAnimations();

uint8_t getAnimationCount();


#ifndef PROGMEM // ignore PROGMEM keyword if it's not used on the current platform
#define PROGMEM
#endif

#ifndef PI
#define PI 3.1415926535897932
#endif
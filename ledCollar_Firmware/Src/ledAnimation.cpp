#include "ledAnimation.h"

uint16_t animationStepIdx = 0;
uint8_t curAnim = 0;
uint16_t animNumLeds = 0;
uint32_t (*_hsv2rgb)(uint8_t hue, uint8_t sat, uint8_t val);
void (*_setLed)(uint16_t index, uint32_t rgb);
uint32_t animBaseColor = 0xFFFFFF;
void _fillLeds(uint32_t rgb);
extern const uint8_t PROGMEM gamma8[];
uint32_t scaleRGBW(uint32_t rgbw, uint8_t scale);


/////////////////////////////////////////////
///////    Define animations here    ////////
/////////////////////////////////////////////

void rainbowAnimation() {
    if(animationStepIdx >= 256) {
        animationStepIdx = 0;
    }
    for(uint16_t i = 0; i < animNumLeds; i++) {
        uint16_t ledOffset = (255UL * i) / (animNumLeds - 1);
        uint8_t hue = (animationStepIdx + ledOffset) % 256;
        _setLed(i, _hsv2rgb(hue, 255, 255));
    }
}

void rainbowFade() {
    if(animationStepIdx >= 1024) {
        animationStepIdx = 0;
    }
    _fillLeds(_hsv2rgb(animationStepIdx / 4, 255, 255));
}

void blink() {
    if(animationStepIdx >= 2) {
        animationStepIdx = 0;
    }
    _fillLeds((animationStepIdx) ? 0 : 0xFFFFFFFF);
}

void fadeUpDown() {
    if(animationStepIdx >= 512) {
        animationStepIdx = 0;
    }
    uint8_t scale = animationStepIdx < 256 ? animationStepIdx : 511 - animationStepIdx; //count up to 255, then back to zero
    scale = gamma8[scale];
    _fillLeds(scaleRGBW(animBaseColor, scale));
}

anim_t anims[] = {
    {"rainbowScroll", rainbowAnimation},
    {"rainbowFade", rainbowFade},
    {"strobo", blink},
    {"fadeUpDown", fadeUpDown}
};





/////////////////////////////////////////
///////    Internal functions    ////////
/////////////////////////////////////////

int numAnims = sizeof(anims) / sizeof(anims[0]);

bool setAnimation(uint8_t animId) {
    if(animId >= 0 && animId < numAnims) {
        curAnim = animId;
        return true;
    }
    return false;
}

bool setAnimation(const char* animName) {
    return setAnimation(animationNameToId(animName));
}

int animationNameToId(const char* animName) {
    int result = -1;
    for(uint8_t i = 0; i < numAnims; i++) {
        if(strcmp(anims[i].animName, animName) == 0) {
            result = i;
            break;
        }
    }
    return result;
}

void stepAnimation() {
    anims[curAnim].funcPtr();
    animationStepIdx++;
}

const char* getCurAnimationName() {
    return anims[curAnim].animName;
}

void setAnimationBaseColor(uint32_t rgb) {
    animBaseColor = rgb;
}

anim_t *getAnimations() {
    return anims;
}

uint8_t getAnimationCount() {
    return numAnims;
}

#ifdef ANIMATION_USE_FASTLED
uint32_t FastLED_hsv2rgb(uint8_t hue, uint8_t sat, uint8_t val) {
    CRGB crgb = CHSV(hue, sat, val);
    uint32_t rgb = crgb.r << 16 | crgb.g << 8 | crgb.b;
    return rgb;
}

void FastLED_setLed(uint16_t idx, uint32_t rgb) {
    leds[idx] = CRGBW(rgb);
}

void initAnimation(uint16_t numLeds) {
    animNumLeds = numLeds;
    _hsv2rgb = FastLED_hsv2rgb;
    _setLed = FastLED_setLed;
}
#else
void initAnimation(uint16_t numLeds, void (*setLed)(uint16_t index, uint32_t rgb), uint32_t (*hsv2rgb)(uint8_t hue, uint8_t sat, uint8_t val)) {
    _hsv2rgb = hsv2rgb;
    _setLed = setLed;
}
#endif

void _fillLeds(uint32_t rgb) {
    for(uint16_t i = 0; i < animNumLeds; i++) {
        _setLed(i, rgb);
    }
}

uint32_t scaleRGBW(uint32_t rgbw, uint8_t scale) {
    uint32_t bd = rgbw & 0x00FF00FF;
    uint32_t ac = (rgbw >> 8) & 0x00FF00FF;
    bd *= scale;
    ac *= scale;
    uint32_t out = ((bd >> 8) & 0x00FF00FF) | (ac & 0xFF00FF00);
    return out;
}

const uint8_t PROGMEM gamma8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
    10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
    17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
    25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
    37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
    51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
    69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
    90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
    115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
    144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
    177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
    215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };
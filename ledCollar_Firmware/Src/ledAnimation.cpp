#include "ledAnimation.h"
#include <math.h>

uint16_t animationStepIdx = 0;
uint8_t curAnim = 0;
uint16_t animNumLeds = 0;
uint32_t (*_hsv2rgb)(uint8_t hue, uint8_t sat, uint8_t val);
uint32_t *animLedBuf = NULL;
void _setLed(uint16_t index, uint32_t rgb, bool doGamma = true);
void (*_setLedFunc)(uint16_t index, uint32_t rgb);
uint32_t animBaseColor = 0xFFFFFF;
void _fillLeds(uint32_t rgb);
void _scaleAll(uint8_t scale);
extern const uint8_t PROGMEM gamma8[];
uint32_t scaleRGBW(uint32_t rgbw, uint8_t scale);
inline uint32_t internalHsv2rgb(uint8_t hue, uint8_t sat, uint8_t val);

#ifdef ANIMATION_USE_INTERNAL_BUFFER
void _setBufLed(uint16_t index, uint32_t rgb);
void _updateLeds(bool doGamma = true);
#endif

uint32_t randomColors[] = {
    0xFF0000,
    0xFFFF00,
    0x00FF00,
    0x00FFFF,
    0x0000FF,
    0xFF00FF,
    0xFFFFFF
};
uint8_t randomColorsSize = sizeof(randomColors) / sizeof(randomColors[0]);


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
    // scale = gamma8[scale];
    _fillLeds(scaleRGBW(animBaseColor, scale));
}

// like larsson scanner, but only in one direction
void scanner() {
    if(animationStepIdx >= 256) {
        animationStepIdx = 0;
    }

    for(uint16_t i = 0; i < animNumLeds; i++) {
        uint16_t ledOffset = (255UL * i) / (animNumLeds - 1);           // calculate individual led offset in animationSteps 
        uint8_t animStepByLED = (animationStepIdx + ledOffset) % 256;   // calc index at led position with offset and wrap-around
        float sinVal = cos((animStepByLED / 256.0) * 2 * PI);
        uint8_t redValue = ((sinVal - 0.75) * 4) * 255.0;
        _setLed(i, (uint32_t)redValue << 16);
    }
}

#ifdef ANIMATION_USE_INTERNAL_BUFFER

void larssonScanner(bool rainbow) {
    uint16_t aIdx = animationStepIdx % 256;
    // count up to 255 and then back down, in increments of 2
    uint8_t index = aIdx % 256 < 128 ? aIdx : 255 - aIdx;
    index *= 2;

    // dim whole strip to create a trail effect
    _scaleAll(255 - 20);

    uint16_t curPos = (index * animNumLeds) / 256;
    if(rainbow) {
        uint8_t hue = (animationStepIdx / 8) % 255;
        _setBufLed(curPos, _hsv2rgb(hue, 255, 255));
    }
    else {
        _setBufLed(curPos, 0xFF0000);
    }

    _updateLeds(false);

    // for(uint16_t i = 0; i < animNumLeds; i++) {
        // uint16_t ledOffset = ((255UL-64) * i) / (animNumLeds - 1) + 32;   // calculate individual led offset in animationSteps 
        // uint8_t animStepByLED = (index + ledOffset) % 256;      // calc index at led position with offset and wrap-around
        // float sinVal = cos((animStepByLED / 256.0) * 2 * PI);
        // uint8_t redValue = ((sinVal - 0.75) * 4) * 255.0;
        // _setLed(i, (uint32_t)redValue << 16);
    // }
}

void larssonScanner() {
    larssonScanner(false);
}

void larssonScannerRainbow() {
    larssonScanner(true);
}

void twinkleStars() {
    _scaleAll(254);
    if(animationStepIdx % 2 == 0) {
        if(rand() % 8 == 0) {
            uint8_t randColIdx = rand() % randomColorsSize;
            uint32_t color = randomColors[randColIdx];
            uint16_t randLedPos = rand() % animNumLeds;
            _setBufLed(randLedPos, color);
        }
    }
    _updateLeds(false);
}

#endif


anim_t anims[] = {
    {"rainbowScroll", rainbowAnimation},
    {"rainbowFade", rainbowFade},
    {"strobo", blink},
    {"fadeUpDown", fadeUpDown},
    {"scanner", scanner},
    #ifdef ANIMATION_USE_INTERNAL_BUFFER // those animations require the internal buffer
    {"larsson", larssonScanner},
    {"larssonRainbow", larssonScannerRainbow},
    {"twinkleStars", twinkleStars},
    #endif
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

#ifdef ANIMATION_USE_INTERNAL_BUFFER

void initBuffer(uint16_t numLeds) {
    #ifdef ANIMATION_USE_INTERNAL_BUFFER
        // animLedBuf = malloc(numLeds * sizeof(uint32_t));
        animLedBuf = new uint32_t[numLeds];
        if(animLedBuf != NULL) {
            memset(animLedBuf, 0, animNumLeds * sizeof(uint32_t));
        }
    #endif
}

void _setBufLed(uint16_t index, uint32_t rgb) {
    if(animLedBuf != NULL && index < animNumLeds) {
        animLedBuf[index] = rgb;
    }
}

void _updateLeds(bool doGamma) {
    if(animLedBuf != NULL) {
        for(uint16_t i = 0; i < animNumLeds; i++) {
            _setLed(i, animLedBuf[i], doGamma);
        }
    }
}

void _scaleAll(uint8_t scale) {
    if(animLedBuf != NULL) {
        for(uint16_t i = 0; i < animNumLeds; i++) {
            animLedBuf[i] = scaleRGBW(animLedBuf[i], scale);
        }
    }
}

#endif

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
    _setLedFunc = FastLED_setLed;
}
#else
void initAnimation( uint16_t numLeds, 
                    void (*setLed)(uint16_t index, uint32_t rgb), 
                    uint32_t (*hsv2rgb)(uint8_t hue, uint8_t sat, uint8_t val)) {
    animNumLeds = numLeds;
    _hsv2rgb = hsv2rgb;
    _setLedFunc = setLed;
    initBuffer(numLeds);
}

// use internal hsv2rgb
void initAnimation( uint16_t numLeds, 
                    void (*setLed)(uint16_t index, uint32_t rgb)) {
    initAnimation(numLeds, setLed, internalHsv2rgb);
}
#endif

void _setLed(uint16_t index, uint32_t rgb, bool doGamma) {
    if(doGamma) {
        uint32_t gammaColor = 0;
        for(uint8_t i = 0; i < 4; i++) {
            uint8_t val = (rgb >> (i * 8)) & 0xFF;
            gammaColor |= gamma8[val] << (i * 8);
        }
        _setLedFunc(index, gammaColor);
    }
    else {
        _setLedFunc(index, rgb);
    }
}


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

// color2hsv copied from https://github.com/adafruit/Adafruit_NeoPixel
/*!
  @brief   Convert hue, saturation and value into a packed 32-bit RGB color
           that can be passed to setPixelColor() or other RGB-compatible
           functions.
  @param   hue  An unsigned 16-bit value, 0 to 65535, representing one full
                loop of the color wheel, which allows 16-bit hues to "roll
                over" while still doing the expected thing (and allowing
                more precision than the wheel() function that was common to
                prior NeoPixel examples).
  @param   sat  Saturation, 8-bit value, 0 (min or pure grayscale) to 255
                (max or pure hue). Default of 255 if unspecified.
  @param   val  Value (brightness), 8-bit value, 0 (min / black / off) to
                255 (max or full brightness). Default of 255 if unspecified.
  @return  Packed 32-bit RGB with the most significant byte set to 0 -- the
           white element of WRGB pixels is NOT utilized. Result is linearly
           but not perceptually correct, so you may want to pass the result
           through the gamma32() function (or your own gamma-correction
           operation) else colors may appear washed out. This is not done
           automatically by this function because coders may desire a more
           refined gamma-correction function than the simplified
           one-size-fits-all operation of gamma32(). Diffusing the LEDs also
           really seems to help when using low-saturation colors.
*/
uint32_t adafruitColorHSV(uint16_t hue, uint8_t sat, uint8_t val) {

  uint8_t r, g, b;

  // Remap 0-65535 to 0-1529. Pure red is CENTERED on the 64K rollover;
  // 0 is not the start of pure red, but the midpoint...a few values above
  // zero and a few below 65536 all yield pure red (similarly, 32768 is the
  // midpoint, not start, of pure cyan). The 8-bit RGB hexcone (256 values
  // each for red, green, blue) really only allows for 1530 distinct hues
  // (not 1536, more on that below), but the full unsigned 16-bit type was
  // chosen for hue so that one's code can easily handle a contiguous color
  // wheel by allowing hue to roll over in either direction.
  hue = (hue * 1530L + 32768) / 65536;
  // Because red is centered on the rollover point (the +32768 above,
  // essentially a fixed-point +0.5), the above actually yields 0 to 1530,
  // where 0 and 1530 would yield the same thing. Rather than apply a
  // costly modulo operator, 1530 is handled as a special case below.

  // So you'd think that the color "hexcone" (the thing that ramps from
  // pure red, to pure yellow, to pure green and so forth back to red,
  // yielding six slices), and with each color component having 256
  // possible values (0-255), might have 1536 possible items (6*256),
  // but in reality there's 1530. This is because the last element in
  // each 256-element slice is equal to the first element of the next
  // slice, and keeping those in there this would create small
  // discontinuities in the color wheel. So the last element of each
  // slice is dropped...we regard only elements 0-254, with item 255
  // being picked up as element 0 of the next slice. Like this:
  // Red to not-quite-pure-yellow is:        255,   0, 0 to 255, 254,   0
  // Pure yellow to not-quite-pure-green is: 255, 255, 0 to   1, 255,   0
  // Pure green to not-quite-pure-cyan is:     0, 255, 0 to   0, 255, 254
  // and so forth. Hence, 1530 distinct hues (0 to 1529), and hence why
  // the constants below are not the multiples of 256 you might expect.

  // Convert hue to R,G,B (nested ifs faster than divide+mod+switch):
  if(hue < 510) {         // Red to Green-1
    b = 0;
    if(hue < 255) {       //   Red to Yellow-1
      r = 255;
      g = hue;            //     g = 0 to 254
    } else {              //   Yellow to Green-1
      r = 510 - hue;      //     r = 255 to 1
      g = 255;
    }
  } else if(hue < 1020) { // Green to Blue-1
    r = 0;
    if(hue <  765) {      //   Green to Cyan-1
      g = 255;
      b = hue - 510;      //     b = 0 to 254
    } else {              //   Cyan to Blue-1
      g = 1020 - hue;     //     g = 255 to 1
      b = 255;
    }
  } else if(hue < 1530) { // Blue to Red-1
    g = 0;
    if(hue < 1275) {      //   Blue to Magenta-1
      r = hue - 1020;     //     r = 0 to 254
      b = 255;
    } else {              //   Magenta to Red-1
      r = 255;
      b = 1530 - hue;     //     b = 255 to 1
    }
  } else {                // Last 0.5 Red (quicker than % operator)
    r = 255;
    g = b = 0;
  }

  // Apply saturation and value to R,G,B, pack into 32-bit result:
  uint32_t v1 =   1 + val; // 1 to 256; allows >>8 instead of /255
  uint16_t s1 =   1 + sat; // 1 to 256; same reason
  uint8_t  s2 = 255 - sat; // 255 to 0
  return ((((((r * s1) >> 8) + s2) * v1) & 0xff00) << 8) |
          (((((g * s1) >> 8) + s2) * v1) & 0xff00)       |
         ( ((((b * s1) >> 8) + s2) * v1)           >> 8);
}

inline uint32_t internalHsv2rgb(uint8_t hue, uint8_t sat, uint8_t val) {
    return adafruitColorHSV(hue << 8, sat, val);
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
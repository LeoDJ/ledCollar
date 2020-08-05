#pragma once
#include <stdint.h>


#define SAMPLING_DIV    4
#define SAMPLING_RATE   (35750 / SAMPLING_DIV)
#define TARGET_FREQ     75 // 45
#define BLOCK_SIZE      32 // block size
// #define ADC_CENTER      1430

#define MIDPOINT_AVG_COEFF      0.002
#define BASS_PEAK_COEFF_INC     0.2
#define BASS_PEAK_COEFF_DEC     (BLOCK_SIZE * 0.000001) // 0.0001 worked good at 50 binSize
#define BASS_PEAK_MIN_VAL       (BLOCK_SIZE * 200) // 10000 worked good at 50 binSize
#define BASS_PEAK_HOLD_TIME     600 // ms
#define BASS_TROUGH_COEFF_DEC   0.2
#define BASS_TROUGH_COEFF_INC   (BLOCK_SIZE * 0.00005)
#define BASS_SMOOTH_COUNT       12

#define NUM_MIC_SAMPLES (SAMPLING_DIV * BLOCK_SIZE)
extern uint16_t micSamples[NUM_MIC_SAMPLES];

extern uint16_t micMaxVal;

void initSound();
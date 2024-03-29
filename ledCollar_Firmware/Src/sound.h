#pragma once
#include <stdint.h>

#define ADC_CLOCK 8000000
#define ADC_SAMPLING_CYCLES  240
#define SAMPLING_DIV    4
#define SAMPLING_RATE   (ADC_CLOCK / ADC_SAMPLING_CYCLES / SAMPLING_DIV)
#define TARGET_FREQ     45 // 45
#define BLOCK_SIZE      32 // block size
// #define ADC_CENTER      1430

#define MIDPOINT_AVG_COEFF      0.002
#define BASS_PEAK_COEFF_INC     0.2
#define BASS_PEAK_COEFF_DEC     (BLOCK_SIZE * 0.000015) //(BLOCK_SIZE * 0.0000015) // 0.0001 worked good at 50 binSize
#define BASS_PEAK_MIN_VAL       (BLOCK_SIZE * 25) // 10000 worked good at 50 binSize
#define BASS_PEAK_HOLD_TIME     600 // ms
#define BASS_TROUGH_COEFF_DEC   0.2
#define BASS_TROUGH_COEFF_INC   (BLOCK_SIZE * 0.00005)
#define BASS_SMOOTH_COUNT       12

#define NUM_MIC_SAMPLES (SAMPLING_DIV * BLOCK_SIZE)
extern uint16_t micSamples[NUM_MIC_SAMPLES];

extern uint16_t micMaxVal;

void initSound();
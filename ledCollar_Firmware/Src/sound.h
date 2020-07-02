#pragma once
#include <stdint.h>


#define SAMPLING_DIV    4
#define SAMPLING_RATE   (37500 / SAMPLING_DIV)
#define TARGET_FREQ     47
#define BLOCK_SIZE      25 // block size
#define ADC_CENTER      1430

#define NUM_MIC_SAMPLES (SAMPLING_DIV * BLOCK_SIZE)
extern uint16_t micSamples[NUM_MIC_SAMPLES];

extern uint16_t micMaxVal;

void initSound();
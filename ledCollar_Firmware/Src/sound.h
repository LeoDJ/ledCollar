#pragma once
#include <stdint.h>

#define NUM_MIC_SAMPLES 1024
extern uint16_t micSamples[NUM_MIC_SAMPLES];

extern uint16_t micMaxVal;

void initSound();
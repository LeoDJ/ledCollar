#include "sound.h"
#include "adc.h"

uint16_t micSamples[NUM_MIC_SAMPLES];

void adcTransferComplete() {
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
}

void initSound() {
    hadc1.DMA_Handle->XferCpltCallback  = &adcTransferComplete; // doesn't work yet
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)micSamples, NUM_MIC_SAMPLES);
}
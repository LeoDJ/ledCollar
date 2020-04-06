#include "sound.h"
#include "adc.h"
#include "globals.h"
#include "apa102.h"

uint16_t micSamples[NUM_MIC_SAMPLES];
uint16_t micMaxVal = 0;

// 1khz tone = 48 samples @ 240 cycles / sample @ 12MHz ADC clock = 50ksps

void adcTransferComplete() {
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    for(int i = 0; i < NUM_MIC_SAMPLES; i++) {
        if(micSamples[i] > micMaxVal) {
            micMaxVal = micSamples[i];
        }
    }
    for(int i = 0; i < NUM_MIC_SAMPLES; i+=2) {
        HAL_UART_Transmit(&DEBUG_UART, (uint8_t *)&micSamples[i], 2, 100);
        HAL_UART_Transmit(&DEBUG_UART, (uint8_t *)&micMaxVal, 2, 100);
    }

    // doLedTransfer();
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    if(hadc == &hadc1) {
        adcTransferComplete();
    }
}

void initSound() {
    // hadc1.DMA_Handle->XferCpltCallback  = adcTransferComplete; // doesn't work yet
    // __HAL_DMA_ENABLE_IT(hadc1.DMA_Handle, DMA_IT_TC);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)micSamples, NUM_MIC_SAMPLES);
}
#include "sound.h"
#include "adc.h"
#include "globals.h"
#include "apa102.h"
#include "math.h"
#include "goertzel.h"
#include "util.h"

uint16_t micSamples[NUM_MIC_SAMPLES];
uint16_t micMaxVal = 0;

GoertzelFilter filter = GoertzelFilter(TARGET_FREQ, BLOCK_SIZE, SAMPLING_RATE);

// 240 cycles / sample @ 9MHz ADC clock = 37.5ksps
// the measured sample count is more like 35750 samples/s
void adcTransferComplete() {
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    for(int i = 0; i < NUM_MIC_SAMPLES; i++) {
        // if(micSamples[i] > micMaxVal) {
        //     micMaxVal = micSamples[i];
        // }
        if(i % SAMPLING_DIV == 0) {
            filter.processSample(micSamples[i]);
        }
    }
    float filterVal = filter.calculateMagnitude() / 20;
    HAL_UART_Transmit(&DEBUG_UART, (uint8_t *)&filterVal, 4, 1);
    float micVal = micSamples[NUM_MIC_SAMPLES-1];
    HAL_UART_Transmit(&DEBUG_UART, (uint8_t *)&micVal, 4, 1);
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
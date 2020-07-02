#include "sound.h"
#include "adc.h"
#include "globals.h"
#include "apa102.h"
#include "math.h"

uint16_t micSamples[NUM_MIC_SAMPLES];
uint16_t micMaxVal = 0;



float gCoeff, gQ1, gQ2, gSine, gCosine;

void goertzelReset() {
    gQ1 = 0;
    gQ2 = 0;
}

void goertzelInit() {
    int k;
    float floatN, omega;

    floatN = (float)BLOCK_SIZE;
    // k = (int)(0.5 + ((floatN * TARGET_FREQ) / SAMPLING_RATE));
    // omega = (2.0 * PI * k) / floatN;
    // gSine = sin(omega);
    omega = (2.0 * PI * TARGET_FREQ) / SAMPLING_RATE;
    gCosine = cos(omega);
    gCoeff = 2.0 * gCosine;

    goertzelReset();
}

// call after every sample
void goertzelProcessSample(uint16_t sample) {
    float Q0;
    Q0 = gCoeff * gQ1 - gQ2 + (float)(sample) - ADC_CENTER;
    gQ2 = gQ1;
    gQ1 = Q0;
}

// optimized Goertzel, call after every block, returns relative magnitude squared
float goertzelGetMagnitudeSquared() {
    float result = gQ1 * gQ1 + gQ2 * gQ2 - gQ1 * gQ2 * gCoeff;
    return result;
}

float goertzelVal = 0;

// 240 cycles / sample @ 9MHz ADC clock = 37.5ksps
void adcTransferComplete() {
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    for(int i = 0; i < NUM_MIC_SAMPLES; i++) {
        // if(micSamples[i] > micMaxVal) {
        //     micMaxVal = micSamples[i];
        // }
        if(i % SAMPLING_DIV == 0) {
            goertzelProcessSample(micSamples[i]);
        }
    }
    goertzelVal = sqrt(goertzelGetMagnitudeSquared());
    goertzelReset();
    uint16_t value = goertzelVal;
    HAL_UART_Transmit(&DEBUG_UART, (uint8_t *)&value, 2, 1);
    HAL_UART_Transmit(&DEBUG_UART, (uint8_t *)&micSamples[NUM_MIC_SAMPLES-1], 2, 1);

    // for(int i = 0; i < NUM_MIC_SAMPLES; i+=3) {
        // HAL_UART_Transmit(&DEBUG_UART, (uint8_t *)&micMaxVal, 2, 100);
    // }

    

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
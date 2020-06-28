#include "sound.h"
#include "adc.h"
#include "globals.h"
#include "apa102.h"
#include "math.h"

uint16_t micSamples[NUM_MIC_SAMPLES];
uint16_t micMaxVal = 0;


#define SAMPLING_RATE   64000
#define TARGET_FREQ     75
#define N               NUM_MIC_SAMPLES // block size

float gCoeff, gQ1, gQ2, gSine, gCosine;

void goertzelReset() {
    gQ1 = 0;
    gQ2 = 0;
}

void goertzelInit() {
    int k;
    float floatN, omega;

    floatN = (float)N;
    k = (int)(0.5 + ((floatN * TARGET_FREQ) / SAMPLING_RATE));
    omega = (2.0 * PI * k) / floatN;
    gSine = sin(omega);
    gCosine = cos(omega);
    gCoeff = 2.0 * gCosine;

    goertzelReset();
}

// call after every sample
void goertzelProcessSample(uint16_t sample) {
    float Q0;
    Q0 = gCoeff * gQ1 - gQ2 + (float)sample;
    gQ2 = gQ1;
    gQ1 = Q0;
}

// optimized Goertzel, call after every block, returns relative magnitude squared
float goertzelGetMagnitudeSquared() {
    float result = gQ1 * gQ1 + gQ2 * gQ2 - gQ1 * gQ2 * gCoeff;
    return result;
}

// 1khz tone = 48 samples @ 240 cycles / sample @ 12MHz ADC clock = 50ksps

void adcTransferComplete() {
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    for(int i = 0; i < NUM_MIC_SAMPLES; i++) {
        if(micSamples[i] > micMaxVal) {
            micMaxVal = micSamples[i];
        }
        goertzelProcessSample(micSamples[i] << 4);
    }
    float magn = sqrt(goertzelGetMagnitudeSquared());
    goertzelReset();
    for(int i = 0; i < NUM_MIC_SAMPLES; i+=3) {
        HAL_UART_Transmit(&DEBUG_UART, (uint8_t *)&micSamples[i], 2, 100);
        // HAL_UART_Transmit(&DEBUG_UART, (uint8_t *)&micMaxVal, 2, 100);
        uint16_t value = magn;
        HAL_UART_Transmit(&DEBUG_UART, (uint8_t *)&value, 2, 100);
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
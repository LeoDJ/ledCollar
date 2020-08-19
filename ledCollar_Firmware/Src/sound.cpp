#include "sound.h"
#include "adc.h"
#include "globals.h"
#include "apa102.h"
#include "math.h"
#include "goertzel.h"
#include "util.h"
#include "ledAnimation.h"

uint16_t micSamples[NUM_MIC_SAMPLES];
uint16_t micMaxVal = 0;

GoertzelFilter filter = GoertzelFilter(TARGET_FREQ, BLOCK_SIZE, SAMPLING_RATE);
float midpointAvg = 0, peakVal = 0, troughVal = 0;
uint32_t lastPeak;
uint16_t bassSmoothValues[BASS_SMOOTH_COUNT];
uint8_t bassSmoothIndex = 0;

// 240 cycles / sample @ 9MHz ADC clock = 37.5ksps
// the measured sample count is more like 35750 samples/s
// 240 cycles / sample @ 8MHz ADC clock = 33333 samples/s
void adcTransferComplete() {
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);


    // do a "low pass" Goertzel filter to get the magnitude of low frequencies
    for(int i = 0; i < NUM_MIC_SAMPLES; i++) {
        if(i % SAMPLING_DIV == 0) {
            filter.processSample(micSamples[i]);
        }
    }
    float filterVal = filter.calculateMagnitude();

    // keep the midpoint of the waveform
    if(midpointAvg == 0) {
        // initialize average faster
        midpointAvg = filterVal;
    }
    else {
        midpointAvg = midpointAvg * (1 - MIDPOINT_AVG_COEFF ) + filterVal * MIDPOINT_AVG_COEFF;
    }

    // flip the negative part of the waveform to the top and make it begin at 0
    float absVal = filterVal > midpointAvg ? filterVal - midpointAvg : midpointAvg - filterVal;

    // smooth between the values so the LED strip has more stable data, basically storing a local maximum
    bassSmoothValues[bassSmoothIndex] = absVal;
    bassSmoothIndex++;
    if(bassSmoothIndex >= BASS_SMOOTH_COUNT) {
        bassSmoothIndex = 0;
    }
    float smoothVal = 0;
    for(int i = 0; i < BASS_SMOOTH_COUNT; i++) {
        // smoothVal += bassSmoothValues[i];
        smoothVal = MAX(smoothVal, bassSmoothValues[i]);
    }
    // smoothVal /= BASS_SMOOTH_COUNT;

    // keep a maximum peak value to scale the output
    if(absVal > peakVal) {
        lastPeak = HAL_GetTick();
        peakVal = peakVal * (1 - BASS_PEAK_COEFF_INC) + absVal * BASS_PEAK_COEFF_INC;
    }
    else if(HAL_GetTick() - lastPeak > BASS_PEAK_HOLD_TIME) {
        peakVal = peakVal * (1 - BASS_PEAK_COEFF_DEC) + absVal * BASS_PEAK_COEFF_DEC;
    }
    // keep the scale at a reasonable level in very quiet environments to prevent noise
    if(peakVal < BASS_PEAK_MIN_VAL) {
        peakVal = BASS_PEAK_MIN_VAL;
    }

    // keep a minimum trough value to react better to a constant bassline
    if(smoothVal < troughVal) {
        troughVal = troughVal * (1 - BASS_TROUGH_COEFF_DEC) + smoothVal * BASS_TROUGH_COEFF_DEC;
    }
    else {
        troughVal = troughVal * (1 - BASS_TROUGH_COEFF_INC) + smoothVal * BASS_TROUGH_COEFF_INC;
    }

    // scale the output to a single byte without overshoot without taking trough value into consideration
    float scaledOutput = (MIN(smoothVal, peakVal) / peakVal) * 255.0;  
    // with trough value
    float scaledOutputForBeatDetection = (MIN(smoothVal - troughVal, peakVal - troughVal) / (peakVal - troughVal)) * 255.0;

    setAnimationIntensity(scaledOutput);
    setAnimationIntensityBeatDetect(scaledOutputForBeatDetection);

    

    float micVal = micSamples[NUM_MIC_SAMPLES-1] * 25;
    // HAL_UART_Transmit(&DEBUG_UART, (uint8_t *)&micVal, 4, 1);
    // HAL_UART_Transmit(&DEBUG_UART, (uint8_t *)&filterVal, 4, 1);
    // HAL_UART_Transmit(&DEBUG_UART, (uint8_t *)&midpointAvg, 4, 1);
    // HAL_UART_Transmit(&DEBUG_UART, (uint8_t *)&absVal, 4, 1);
    // HAL_UART_Transmit(&DEBUG_UART, (uint8_t *)&peakVal, 4, 1);
    // HAL_UART_Transmit(&DEBUG_UART, (uint8_t *)&troughVal, 4, 1);
    // HAL_UART_Transmit(&DEBUG_UART, (uint8_t *)&smoothVal, 4, 1);
    // HAL_UART_Transmit(&DEBUG_UART, (uint8_t *)&scaledOutput, 4, 1);

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
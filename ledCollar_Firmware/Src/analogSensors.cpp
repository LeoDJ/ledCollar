#include "analogSensors.h"

#include <stm32f1xx_hal.h>
#include <adc.h>
#include <stdio.h>


uint32_t vddSum = 0, lightSenseSum = 0;

void analogSensorsConvCplt() {
    uint32_t lightSensorVal = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1);
    uint32_t voltageSenseVal = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_2);
    uint32_t vRefIntVal = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_3);

    uint32_t vdda = VREFINT_VOLTAGE * 4096 / vRefIntVal;                // calculate MCU supply voltage in mV
    uint32_t vdd = voltageSenseVal * VIN_DIVIDER_RATIO * vdda / 4096;   // calculate battery voltage in mV

    if (vddSum == 0) {
        // initialize moving average
        vddSum = vdd * AVERAGING_FACTOR;
    }
    else {
        // calculate moving average
        vddSum -= vddSum / AVERAGING_FACTOR;
        vddSum += vdd;
    }

    if (lightSenseSum == 0) {
        // initialize moving average
        lightSenseSum = lightSensorVal * AVERAGING_FACTOR;
    }
    else {
        // calculate moving average
        lightSenseSum -= lightSenseSum / AVERAGING_FACTOR;
        lightSenseSum += lightSensorVal; 
    }

    // printf("[Analog Sensors] Light: %4d (%4d), Voltage: %4d, vRefInt: %4d, VDDA: %4d, VDD: %4d (%4d)\n", lightSenseSum / AVERAGING_FACTOR, lightSensorVal, voltageSenseVal, vRefIntVal, vdda, vddSum / AVERAGING_FACTOR, vdd);
}

void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef* hadc) {
    if(hadc == &hadc1) {
        analogSensorsConvCplt();
    }
}

uint32_t lastAnalogSensorsPoll = 0;

void analogSensorsLoop() {
    if (HAL_GetTick() - lastAnalogSensorsPoll >= ANALOG_SENSOR_SAMPLE_RATE) {
        lastAnalogSensorsPoll = HAL_GetTick();
        HAL_ADCEx_InjectedStart_IT(&hadc1);
    }
}

uint16_t getVcc() {
    return vddSum / AVERAGING_FACTOR;
}

uint16_t getAmbientLight() {
    return lightSenseSum / AVERAGING_FACTOR;
}
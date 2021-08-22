#include "analogSensors.h"

#include <stm32f1xx_hal.h>
#include <adc.h>
#include <stdio.h>

void analogSensorsConvCplt() {
    uint32_t lightSensorVal = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1);
    uint32_t voltageSenseVal = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_2);
    uint32_t vRefIntVal = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_3);

    printf("[Analog Sensors] Light: %4d, Voltage: %4d, vRefInt: %4d\n", lightSensorVal, voltageSenseVal, vRefIntVal);
}

void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef* hadc) {
    if(hadc == &hadc1) {
        analogSensorsConvCplt();
    }
}

void analogSensorsInit() {


}

uint32_t lastAnalogSensorsPoll = 0;

void analogSensorsLoop() {
    if (HAL_GetTick() - lastAnalogSensorsPoll >= ANALOG_SENSOR_SAMPLE_RATE) {
        lastAnalogSensorsPoll = HAL_GetTick();
        HAL_ADCEx_InjectedStart_IT(&hadc1);
    }
}
/**
 ******************************************************************************
 * @author  MartianZ @ UESTC - ICKX
 * @brief   
 ******************************************************************************
 **/

#include "battery_adc.h"

void ADC_Configuration() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    ADC_InitTypeDef ADC_InitStruct;
    ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStruct.ADC_ScanConvMode = DISABLE;
    ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStruct.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStruct);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while ((ADC_GetResetCalibrationStatus(ADC1))); // && (timeOut--));
    ADC_StartCalibration(ADC1);
    while ((ADC_GetCalibrationStatus(ADC1))); // && (timeOut--));

}

float GetAdcResult(void) {
    float _adcResult;

    _adcResult = 0x0;

    ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_1Cycles5);

    ADC_Cmd(ADC1, ENABLE);
    ADC_TempSensorVrefintCmd(ENABLE);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    Delay_us(200);
    _adcResult = ADC_GetConversionValue(ADC1) / 4096.0 * 3.28 * 2;
    ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
    ADC_TempSensorVrefintCmd(DISABLE);
    ADC_SoftwareStartConvCmd(ADC1, DISABLE);
    ADC_Cmd(ADC1, DISABLE);

    return _adcResult;

}

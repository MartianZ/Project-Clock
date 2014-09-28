/**
 ******************************************************************************
 * @author  MartianZ @ UESTC - ICKX
 * @brief   Battery Voltage Monitor
 ******************************************************************************
 **/


#ifndef BATTERY_ADC_H
#define	BATTERY_ADC_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

void ADC_Configuration();
float GetAdcResult(void);

#ifdef	__cplusplus
}
#endif

#endif	/* BATTERY_ADC_H */


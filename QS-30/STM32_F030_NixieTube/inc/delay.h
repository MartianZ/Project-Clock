#ifndef __DELAY_H
#define __DELAY_H 			   
#include "stm32f0xx_it.h"
 
void configuration_delay(uint8_t SYSCLK);
void delay_ms(uint16_t nms);
void delay_us(uint32_t nus);

#endif






























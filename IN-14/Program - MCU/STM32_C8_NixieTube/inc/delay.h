#ifndef __DELAY_H
#define __DELAY_H 			   
#include "stm32f10x.h"
 
void Delay_init(u8 SYSCLK);
void Delay_ms(u16 nms);
void Delay_us(u32 nus);

#endif






























/*
 * File:   nixietube.h
 * Author: MartianZ
 *
 * Created on 2014年4月1日, 下午1:13
 */

#ifndef NIXIETUBE_H
#define	NIXIETUBE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "stm32f10x.h"
#include "ds3231.h"
#include "delay.h"
#include "usart.h"

#define SET_CPLD_SEL_LOW GPIO_ResetBits(GPIOA, GPIO_Pin_7)
#define SET_CPLD_SEL_HIGH GPIO_SetBits(GPIOA, GPIO_Pin_7)

#ifdef HARDWAREVERSION1
#define SET_CPLD_EN_LOW GPIO_ResetBits(GPIOB, GPIO_Pin_5)
#define SET_CPLD_EN_HIGH GPIO_SetBits(GPIOB, GPIO_Pin_5)
#else
#ifdef HARDWAREVERSION2
#define SET_CPLD_EN_LOW GPIO_ResetBits(GPIOA, GPIO_Pin_3)
#define SET_CPLD_EN_HIGH GPIO_SetBits(GPIOA, GPIO_Pin_3)
#else
#error "Please specify hardware version!"
#endif
#endif

#define SET_CPLD_EN_LOW GPIO_ResetBits(GPIOB, GPIO_Pin_5)
#define SET_CPLD_EN_HIGH GPIO_SetBits(GPIOB, GPIO_Pin_5)

#define SET_CPLD_DATA_LOW GPIO_ResetBits(GPIOA, GPIO_Pin_6)
#define SET_CPLD_DATA_HIGH GPIO_SetBits(GPIOA, GPIO_Pin_6)

#define SET_CPLD_CLK_LOW GPIO_ResetBits(GPIOA, GPIO_Pin_5)
#define SET_CPLD_CLK_HIGH GPIO_SetBits(GPIOA, GPIO_Pin_5)

typedef struct {
    uint8_t R;
    uint8_t G;
    uint8_t B;
} LED5050;


void NixieTube_Normal_SetNewTime(DS3231_date_TypeDef date);
void NixieTube_LED_Set_RGB(LED5050 LED);



#ifdef	__cplusplus
}
#endif

#endif	/* NIXIETUBE_H */

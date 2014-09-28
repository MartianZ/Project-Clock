#include "nixietube.h"
#include "delay.h"
#include <string.h>
#include <math.h>

#define HC595_STCP_HIGH GPIO_SetBits(GPIOA, GPIO_Pin_13)
#define HC595_SHCP_HIGH GPIO_SetBits(GPIOA, GPIO_Pin_14)
#define HC595_DS_HIGH GPIO_SetBits(GPIOA, GPIO_Pin_7)

#define HC595_STCP_LOW GPIO_ResetBits(GPIOA, GPIO_Pin_13)
#define HC595_SHCP_LOW GPIO_ResetBits(GPIOA, GPIO_Pin_14)
#define HC595_DS_LOW GPIO_ResetBits(GPIOA, GPIO_Pin_7)

const uint8_t V1[] = {3, 8, 6, 5, 9, 7, 0, 2, 4, 1};
const uint8_t V2[] = {9, 4, 3, 10, 12, 11, 0, 1, 2, 8};

void configuration_nixietube(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    //PA14 SHCP PA13 STCP 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void configuration_led(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_SetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1); // BLUE YELLOW
    GPIO_SetBits(GPIOF, GPIO_Pin_0 | GPIO_Pin_1);
    //GPIO_ResetBits(GPIOF, GPIO_Pin_0); // RED  GREEN


}

void nixietube_send_595_new_number(uint8_t v1, uint8_t v2) {
    uint8_t i;
    volatile uint8_t t;

    HC595_STCP_LOW;

    t = V2[v2];
    for (i = 0; i < 4; i++) {
        HC595_SHCP_LOW;
        if (t & 8) HC595_DS_HIGH;
        else HC595_DS_LOW;
        t = (t << 1) & 0xF;
        //delay_ms(1);
        HC595_SHCP_HIGH;
        //delay_ms(1);
    }

    t = V1[v1];
    for (i = 0; i < 4; i++) {
        HC595_SHCP_LOW;
        if (t & 8) HC595_DS_HIGH;
        else HC595_DS_LOW;
        t = (t << 1) & 0xF;
        //delay_ms(1);
        HC595_SHCP_HIGH;
        //delay_ms(1);
    }

    HC595_STCP_HIGH;
}
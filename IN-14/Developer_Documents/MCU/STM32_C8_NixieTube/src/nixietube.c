/**
 ******************************************************************************
 * @author  MartianZ @ UESTC - ICKX
 * @brief   NixieTube & RGB-LED Driver
 ******************************************************************************
 **/


#include "nixietube.h"
#include <string.h>

#define random(x) (rand()%x)

DS3231_date_TypeDef previousDate;
uint8_t cpldSEL = 0;
extern char *DOW[];
extern __IO uint16_t CCR1_Val;
extern uint8_t NixieTube_EN;
extern uint8_t AutoPowerOff;
extern LED5050 LED;

void NixieTube_Normal_SetNewTime(DS3231_date_TypeDef date) {


    //First: Is this date a new one?
    if (previousDate.seconds == date.seconds && previousDate.minutes == date.minutes && previousDate.hours == date.hours) {
        //same as the last date
        return;
    }


    char st_tm[9] = "00:00:00";
    char st_dt[11] = "0000.00.00";

    //DS3231_DateToTimeStr(&date, st_tm);
    //DS3231_DateToDateStr(&date, st_dt);

#ifdef DEBUG
    USART_SendStr("Now: ");
    USART_SendStr(st_dt);
    USART_SendStr(", ");
    USART_SendStr(st_tm);
    USART_SendStr(", ");
    USART_SendBuf(&DOW[date.day_of_week - 1][0], strlen(&DOW[date.day_of_week - 1][0]));
    USART_SendStr("\r\n");
    USART_SendStr("Update Nixue Tube...\r\n");
#endif


    //Second: Reverse CPLD SEL pin
    if (!cpldSEL) {
        SET_CPLD_SEL_HIGH;
        cpldSEL = 1;
    } else {
        SET_CPLD_SEL_LOW;
        cpldSEL = 0;
    }

    //Third: Send the new data to CPLD
    uint8_t i, j, k;
    uint16_t t;
    uint8_t time[7];

    time[1] = date.seconds & 0x0f;
    time[2] = date.seconds >> 4;
    time[3] = date.minutes & 0x0f;
    time[4] = date.minutes >> 4;
    time[5] = date.hours & 0x0f;
    time[6] = date.hours >> 4;

    if (!NixieTube_EN || (AutoPowerOff && (time[6] * 10 + time[5]) >= 1 && (time[6] * 10 + time[5]) <= 6)) {
        SET_CPLD_EN_LOW;
        LED.R = 0;
        LED.G = 0;
        LED.B = 0;
        NixieTube_LED_Set_RGB(LED);


    } else {
        Flash_ReadData(&LED);
        NixieTube_LED_Set_RGB(LED);
        SET_CPLD_EN_HIGH;
    }



    for (i = 1; i <= 6; i++) {
        k = time[i] == 0 ? 2 : 12 - time[i];

        for (j = 12; j >= 1; j--) {
            if (j == k) SET_CPLD_DATA_HIGH;
            else SET_CPLD_DATA_LOW;
            SET_CPLD_CLK_HIGH;
            Delay_us(15);
            SET_CPLD_CLK_LOW;
        }
    }

    //Forth: Reverse CPLD PWM pin
    //Five: 5050 LED

    static float r = 0, g = 0, b = 0;
    i = random(100);
    j = random(50);
    k = random(70);

    if (cpldSEL) {
        for (t = 0; t <= 1000; t++) {
            TIM_SetCompare1(TIM4, t);

            /*

                        r += (i - r) / 1000.0;
                        g += (j - g) / 1000.0;
                        b += (k - b) / 1000.0;
                        NixieTube_LED_Set_RGB(r, g, b);
             */

            Delay_us(500);

            if (!(!NixieTube_EN || (AutoPowerOff && (time[6] * 10 + time[5]) >= 1 && (time[6] * 10 + time[5]) <= 6))) {
                TIM_SetCompare2(TIM4, (500 - abs(t - 500)) / 500.0 * 1000 * 0.015);
            }


        }
    } else {
        for (t = 1000; t > 0; t--) {
            TIM_SetCompare1(TIM4, t);
            /*

                        r += (i - r) / 1000.0;
                        g += (j - g) / 1000.0;
                        b += (k - b) / 1000.0;
                        NixieTube_LED_Set_RGB(r, g, b);
             */
            Delay_us(500);

            if (!(!NixieTube_EN || (AutoPowerOff && (time[6] * 10 + time[5]) >= 1 && (time[6] * 10 + time[5]) <= 6))) {
                TIM_SetCompare2(TIM4, (500 - abs(t - 500)) / 500.0 * 1000 * 0.015);
            }

        }
    }

    previousDate = date;





}

void NixieTube_LED_Set_RGB(LED5050 LED) {
    TIM_SetCompare3(TIM4, LED.R / 100.0 * 1000);
    TIM_SetCompare4(TIM4, LED.B / 100.0 * 1000);
    TIM_SetCompare1(TIM1, LED.G / 100.0 * 1000);

}
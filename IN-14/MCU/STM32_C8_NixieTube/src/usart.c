/**
 ******************************************************************************
 * @author  MartianZ @ UESTC - ICKX
 * @brief   USART Terminal
 ******************************************************************************
 **/


#include "usart.h"
#include "ds3231.h"
#include <string.h>
#include "battery_adc.h"
#include <stdlib.h>
#include <stdio.h>
#include "nixietube.h"
#include "flash.h"

extern char *DOW[];
extern LED5050 LED;
extern uint8_t NixieTube_EN;
extern uint8_t AutoPowerOff;

void SPEAKER_BEEP_THREE_UP() {


    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t) (SystemCoreClock / 1000000) - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

    TIM_TimeBaseStructure.TIM_Period = 500;
    TIM_SetCompare2(TIM2, 500 * 0.5);
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //BEEP

    Delay_ms(100);

    TIM_TimeBaseStructure.TIM_Period = 400;
    TIM_SetCompare2(TIM2, 400 * 0.5);
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //BEEP
    Delay_ms(100);

    TIM_TimeBaseStructure.TIM_Period = 250;
    TIM_SetCompare2(TIM2, 250 * 0.5);
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //BEEP


    Delay_ms(100);
    TIM_SetCompare2(TIM2, 0);


}

void SPEAKER_BEEP_THREE_DOWN() {

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t) (SystemCoreClock / 1000000) - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

    TIM_TimeBaseStructure.TIM_Period = 250;
    TIM_SetCompare2(TIM2, 250 * 0.5);
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //BEEP

    Delay_ms(100);

    TIM_TimeBaseStructure.TIM_Period = 400;
    TIM_SetCompare2(TIM2, 400 * 0.5);
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //BEEP
    Delay_ms(100);

    TIM_TimeBaseStructure.TIM_Period = 500;
    TIM_SetCompare2(TIM2, 500 * 0.5);
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //BEEP


    Delay_ms(100);
    TIM_SetCompare2(TIM2, 0);

}

void inline USART_SendConfigurationInfo() {
    USART_SendStr(USART1, "Configuration Mode:\r\n");
    USART_SendStr(USART1, " h - Set hour\r\n");
    USART_SendStr(USART1, " m - Set minute\r\n");
    USART_SendStr(USART1, " s - Set second\r\n");
    USART_SendStr(USART1, " y - Set year\r\n");
    USART_SendStr(USART1, " o - Set month\r\n");
    USART_SendStr(USART1, " d - Set day\r\n");
    USART_SendStr(USART1, " w - Set day of week\r\n");
    USART_SendStr(USART1, " r - Set the positive duty cycle of red leds\r\n");
    USART_SendStr(USART1, " g - Set the positive duty cycle of green leds\r\n");
    USART_SendStr(USART1, " b - Set the positive duty cycle of blue leds\r\n");
}

void USART_Terminal() {
    static uint16_t conf_mode = 0;
    static uint8_t waiting_for_number = 0, i;
    static uint16_t new_value = 0;

    uint16_t ch = USART_ReceiveData(USART1);
    DS3231_date_TypeDef date;
    char st_tm[9] = "00:00:00";
    char st_dt[11] = "0000.00.00";

    if (!conf_mode) {
        switch (ch) {
            case '?':
                TIM_SetCompare2(TIM2, 250 * 0.5);
                Delay_ms(200);
                TIM_SetCompare2(TIM2, 250 * 0);
                USART_SendStr(USART1, "Sekaisen Hendoritsu Meta (Rev 1.1)\r\n");

                USART_SendStr(USART1, " p - Print date\r\n");
                USART_SendStr(USART1, " c - Configuration mode\r\n");
                USART_SendStr(USART1, " e - Enable/Disable NixieTube\r\n");
                USART_SendStr(USART1, " z - Enable/Disable automatically ");
                USART_SendStr(USART1, "turn off NixieTube between 1:00 to 7:00 am\r\n");
                USART_SendStr(USART1, " l - Print 'ILWT' 100 times!\r\n");

                break;
            case 'p':
                DS3231_ReadDateRAW(&date);

                DS3231_DateToTimeStr(&date, st_tm);
                DS3231_DateToDateStr(&date, st_dt);
                USART_SendStr(USART1, "Now: ");
                USART_SendStr(USART1, st_dt);
                USART_SendStr(USART1, ", ");
                USART_SendStr(USART1, st_tm);
                USART_SendStr(USART1, ", ");
                USART_SendStr(USART1, DOW[date.day_of_week - 1]);
                USART_SendStr(USART1, "\r\n");
                break;
            case 'c':
                USART_SendConfigurationInfo();
                conf_mode = 1;
                break;

            case 'e':
                if (NixieTube_EN) {
                    SPEAKER_BEEP_THREE_DOWN();
                } else {
                    SPEAKER_BEEP_THREE_UP();
                }
                NixieTube_EN = 1 - NixieTube_EN;
                break;

            case 'z':
                if (AutoPowerOff) {
                    SPEAKER_BEEP_THREE_DOWN();
                } else {
                    SPEAKER_BEEP_THREE_UP();
                }
                AutoPowerOff = 1 - AutoPowerOff;
                Flash_WriteConfiguration(AutoPowerOff);
                break;

            case 'l':
                for (i = 0; i < 100; i++)USART_SendStr(USART1, "In Longer We Trust!\r\n");
                break;
            default:
                USART_SendStr(USART1, "Sekai-sen Hendo-Ritsu Me-Ta: invalid command.\r\n");
                break;
        }

        return;
    }

    if (conf_mode == 1) {
        switch (ch) {
            case '?':
                USART_SendConfigurationInfo();
                break;
            case 'h':
                USART_SendStr(USART1, "Enter TWO digits to set a new HOUR value...\r\n");
                conf_mode = 'h';
                waiting_for_number = 1;
                break;
            case 'm':
                USART_SendStr(USART1, "Enter TWO digits to set a new MINUTE value...\r\n");
                conf_mode = 'm';
                waiting_for_number = 1;

                break;
            case 's':
                USART_SendStr(USART1, "Enter TWO digits to set a new SECOND value...\r\n");
                conf_mode = 's';
                waiting_for_number = 1;

                break;
            case 'y':
                USART_SendStr(USART1, "Enter TWO digits(20XX) to set a new YEAR value...\r\n");
                conf_mode = 'y';
                waiting_for_number = 1;

                break;
            case 'o':
                USART_SendStr(USART1, "Enter TWO digits to set a new MONTH value...\r\n");
                conf_mode = 'o';
                waiting_for_number = 1;

                break;
            case 'd':
                USART_SendStr(USART1, "Enter TWO digits to set a new DAY value...\r\n");
                conf_mode = 'd';
                waiting_for_number = 1;

                break;
            case 'w':
                USART_SendStr(USART1, "Enter ONE digit to set a new DAY of WEEK value...\r\n");
                conf_mode = 'w';
                new_value = 0;
                waiting_for_number = 2;

                break;
            case 'r':
                USART_SendStr(USART1, "Enter TWO digits(00-99) to set a new duty cycle value...\r\n");
                conf_mode = 'r';
                waiting_for_number = 1;

                break;
            case 'g':
                USART_SendStr(USART1, "Enter TWO digits(00-99) to set a new duty cycle value...\r\n");
                conf_mode = 'g';
                waiting_for_number = 1;

                break;
            case 'b':
                USART_SendStr(USART1, "Enter TWO digits(00-99) to set a new duty cycle value...\r\n");
                conf_mode = 'b';
                waiting_for_number = 1;

                break;
            default:
                USART_SendStr(USART1, "invalid command.\r\n");
                USART_SendStr(USART1, "Leving configuration mode...\r\n");
                conf_mode = 0;
                break;
        }

        return;
    }

    if (waiting_for_number) {
        if (waiting_for_number == 1) {
            new_value = (ch - '0') << 4;
            waiting_for_number = 2;
        } else {
            new_value |= (ch - '0');
            //set new value
            switch (conf_mode) {
                case 'h':
                    if (((new_value >> 4) * 10 + (new_value & 0xF)) > 23) {
                        USART_SendStr(USART1, "Invalid value (accept 0..23)\r\n");
                    } else {
                        DS3231_ReadDateRAW(&date);
                        date.hours = new_value;
                        DS3231_WriteDateRAW(&date);
                        USART_SendStr(USART1, "Value set\r\n");
                    }
                    break;
                case 'm':
                    if (((new_value >> 4) * 10 + (new_value & 0xF)) > 59) {
                        USART_SendStr(USART1, "Invalid value (accept 0..59)\r\n");
                    } else {
                        USART_SendStr(USART1, "Setting...\r\n");

                        DS3231_ReadDateRAW(&date);
                        date.minutes = new_value;
                        DS3231_WriteDateRAW(&date);
                        USART_SendStr(USART1, "Value set\r\n");
                    }
                    break;
                case 's':
                    if (((new_value >> 4) * 10 + (new_value & 0xF)) > 59) {
                        USART_SendStr(USART1, "Invalid value (accept 0..59)\r\n");
                    } else {
                        DS3231_ReadDateRAW(&date);
                        date.seconds = new_value;
                        DS3231_WriteDateRAW(&date);
                        USART_SendStr(USART1, "Value set\r\n");
                    }
                    break;

                case 'y':

                    if (((new_value >> 4) * 10 + (new_value & 0xF)) > 99 | ((new_value >> 4) * 10 + (new_value & 0xF)) < 0) {
                        USART_SendStr(USART1, "Invalid value (accept 0..99)\r\n");
                    } else {
                        DS3231_ReadDateRAW(&date);
                        date.year = new_value;
                        DS3231_WriteDateRAW(&date);
                        USART_SendStr(USART1, "Value set\r\n");
                    }
                    break;
                case 'o':

                    if (((new_value >> 4) * 10 + (new_value & 0xF)) > 12 | ((new_value >> 4) * 10 + (new_value & 0xF)) < 1) {
                        USART_SendStr(USART1, "Invalid value (accept 1..12)\r\n");
                    } else {
                        DS3231_ReadDateRAW(&date);
                        date.month = new_value;
                        DS3231_WriteDateRAW(&date);
                        USART_SendStr(USART1, "Value set\r\n");
                    }
                    break;
                case 'd':

                    if (((new_value >> 4) * 10 + (new_value & 0xF)) > 31 | ((new_value >> 4) * 10 + (new_value & 0xF)) < 1) {
                        USART_SendStr(USART1, "Invalid value (accept 1..31)\r\n");
                    } else {
                        DS3231_ReadDateRAW(&date);
                        date.date = new_value;
                        DS3231_WriteDateRAW(&date);
                        USART_SendStr(USART1, "Value set\r\n");
                    }
                    break;
                case 'w':

                    if (new_value > 7 | new_value < 1) {
                        USART_SendStr(USART1, "Invalid value (accept 1..7)\r\n");
                    } else {
                        DS3231_ReadDateRAW(&date);
                        date.day_of_week = new_value;
                        DS3231_WriteDateRAW(&date);
                        USART_SendStr(USART1, "Value set\r\n");
                    }
                    break;
                case 'r':
                    new_value = ((new_value >> 4) * 10 + (new_value & 0xF));

                    if (new_value > 99 | new_value < 0) {
                        USART_SendStr(USART1, "Invalid value (accept 0..99)\r\n");
                    } else {
                        LED.R = new_value;
                        NixieTube_LED_Set_RGB(LED);
                        Flash_WriteData(LED);
                        USART_SendStr(USART1, "Value set\r\n");
                    }
                    break;
                case 'g':
                    new_value = ((new_value >> 4) * 10 + (new_value & 0xF));

                    if (new_value > 99 | new_value < 0) {
                        USART_SendStr(USART1, "Invalid value (accept 0..99)\r\n");
                    } else {
                        LED.G = new_value;
                        NixieTube_LED_Set_RGB(LED);
                        Flash_WriteData(LED);

                        USART_SendStr(USART1, "Value set\r\n");
                    }
                    break;
                case 'b':
                    new_value = ((new_value >> 4) * 10 + (new_value & 0xF));

                    if (new_value > 99 | new_value < 0) {
                        USART_SendStr(USART1, "Invalid value (accept 0..99)\r\n");
                    } else {
                        LED.B = new_value;
                        NixieTube_LED_Set_RGB(LED);
                        Flash_WriteData(LED);

                        USART_SendStr(USART1, "Value set\r\n");
                    }
                    break;
            }

            conf_mode = 0;
            waiting_for_number = 0;

        }

    }


}

/*
void USART_Terminal(USART_TypeDef *USARTx, uint8_t USB) {

    
    uint16_t ch;

    char st_tm[9] = "00:00:00";
    char st_dt[11] = "0000.00.00";

    DS3231_date_TypeDef date;

    float voltage;                  
              
    char tmp[128];

    


    if (USB || USART_GetFlagStatus(USARTx, USART_SR_RXNE)) {
        ch = USART_ReceiveData(USARTx);
        if (conf_mode) {
            switch (ch) {
                case '?':
                    USART_SendStr(USARTx, "Configure Mode:\r\n");
                    USART_SendStr(USARTx, " h - Set hour\r\n");
                    USART_SendStr(USARTx, " m - Set minute\r\n");
                    USART_SendStr(USARTx, " s - Set second\r\n");
                    USART_SendStr(USARTx, " y - Set year\r\n");
                    USART_SendStr(USARTx, " o - Set month\r\n");
                    USART_SendStr(USARTx, " d - Set day\r\n");
                    USART_SendStr(USARTx, " w - Set day of week\r\n");
                    break;
                case 'h':
                    USART_SendStr(USARTx, "Enter two digit HOURS value...\r\n");
                    //while (!USART_GetFlagStatus(USARTx, USART_SR_RXNE)); // wait for byte from UART
                    
                    ch = (USART_ReceiveData(USARTx) - '0') << 4;

                    //while (!USART_GetFlagStatus(USARTx, USART_SR_RXNE)); // wait for byte from UART
                    ch |= (USART_ReceiveData(USARTx) - '0');

                    if (((ch >> 4) * 10 + (ch & 0xF)) > 23) {
                        USART_SendStr(USARTx, "Invalid hours value (acceptable 0..23)\r\n");
                    } else {
                        DS3231_ReadDateRAW(&date);
                        date.hours = ch;
                        DS3231_WriteDateRAW(&date);
                        USART_SendStr(USARTx, "Value set\r\n");
                    }
                    break;
                case 'm':
                    USART_SendStr(USARTx, "Enter two digit MINUTES value...\r\n");
                    while (!USART_GetFlagStatus(USARTx, USART_SR_RXNE)); // wait for byte from UART
                    ch = (USART_ReceiveData(USARTx) - '0') << 4;

                    while (!USART_GetFlagStatus(USARTx, USART_SR_RXNE)); // wait for byte from UART
                    ch |= (USART_ReceiveData(USARTx) - '0');

                    if (((ch >> 4) * 10 + (ch & 0xF)) > 59) {
                        USART_SendStr(USARTx, "Invalid minutes value (acceptable 0..59)\r\n");
                    } else {
                        USART_SendStr(USARTx, "Setting...\r\n");

                        DS3231_ReadDateRAW(&date);
                        date.minutes = ch;
                        DS3231_WriteDateRAW(&date);
                        USART_SendStr(USARTx, "Value set\r\n");
                    }
                    break;
                case 's':
                    USART_SendStr(USARTx, "Enter two digit SECONDS value...\r\n");
                    while (!USART_GetFlagStatus(USARTx, USART_SR_RXNE)); // wait for byte from UART
                    ch = (USART_ReceiveData(USARTx) - '0') << 4;

                    while (!USART_GetFlagStatus(USARTx, USART_SR_RXNE)); // wait for byte from UART
                    ch |= (USART_ReceiveData(USARTx) - '0');

                    if (((ch >> 4) * 10 + (ch & 0xF)) > 59) {
                        USART_SendStr(USARTx, "Invalid seconds value (acceptable 0..59)\r\n");
                    } else {
                        DS3231_ReadDateRAW(&date);
                        date.seconds = ch;
                        DS3231_WriteDateRAW(&date);
                        USART_SendStr(USARTx, "Value set\r\n");
                    }
                    break;

                case 'y':
                    USART_SendStr(USARTx, "Enter two digit(20XX) YEAR value...\r\n");
                    while (!USART_GetFlagStatus(USARTx, USART_SR_RXNE)); // wait for byte from UART
                    ch = (USART_ReceiveData(USARTx) - '0') << 4;

                    while (!USART_GetFlagStatus(USARTx, USART_SR_RXNE)); // wait for byte from UART
                    ch |= (USART_ReceiveData(USARTx) - '0');

                    if (((ch >> 4) * 10 + (ch & 0xF)) > 99 | ((ch >> 4) * 10 + (ch & 0xF)) < 0) {
                        USART_SendStr(USARTx, "Invalid seconds value (acceptable 0..99)\r\n");
                    } else {
                        DS3231_ReadDateRAW(&date);
                        date.year = ch;
                        DS3231_WriteDateRAW(&date);
                        USART_SendStr(USARTx, "Value set\r\n");
                    }
                    break;
                case 'o':
                    USART_SendStr(USARTx, "Enter two digit MONTH value...\r\n");
                    while (!USART_GetFlagStatus(USARTx, USART_SR_RXNE)); // wait for byte from UART
                    ch = (USART_ReceiveData(USARTx) - '0') << 4;

                    while (!USART_GetFlagStatus(USARTx, USART_SR_RXNE)); // wait for byte from UART
                    ch |= (USART_ReceiveData(USARTx) - '0');

                    if (((ch >> 4) * 10 + (ch & 0xF)) > 12 | ((ch >> 4) * 10 + (ch & 0xF)) < 1) {
                        USART_SendStr(USARTx, "Invalid seconds value (acceptable 1..12)\r\n");
                    } else {
                        DS3231_ReadDateRAW(&date);
                        date.month = ch;
                        DS3231_WriteDateRAW(&date);
                        USART_SendStr(USARTx, "Value set\r\n");
                    }
                    break;
                case 'd':
                    USART_SendStr(USARTx, "Enter two digit DAY value...\r\n");
                    while (!USART_GetFlagStatus(USARTx, USART_SR_RXNE)); // wait for byte from UART
                    ch = (USART_ReceiveData(USARTx) - '0') << 4;

                    while (!USART_GetFlagStatus(USARTx, USART_SR_RXNE)); // wait for byte from UART
                    ch |= (USART_ReceiveData(USARTx) - '0');

                    if (((ch >> 4) * 10 + (ch & 0xF)) > 31 | ((ch >> 4) * 10 + (ch & 0xF)) < 1) {
                        USART_SendStr(USARTx, "Invalid seconds value (acceptable 1..31)\r\n");
                    } else {
                        DS3231_ReadDateRAW(&date);
                        date.date = ch;
                        DS3231_WriteDateRAW(&date);
                        USART_SendStr(USARTx, "Value set\r\n");
                    }
                    break;
                case 'w':
                    USART_SendStr(USARTx, "Enter one digit DAY of WEEK value...\r\n");

                    while (!USART_GetFlagStatus(USARTx, USART_SR_RXNE)); // wait for byte from UART
                    ch = (USART_ReceiveData(USARTx) - '0');

                    if (ch > 7 | ch < 1) {
                        USART_SendStr(USARTx, "Invalid seconds value (acceptable 1..7)\r\n");
                    } else {
                        DS3231_ReadDateRAW(&date);
                        date.day_of_week = ch;
                        DS3231_WriteDateRAW(&date);
                        USART_SendStr(USARTx, "Value set\r\n");
                    }
                    break;
                default:
                    USART_SendStr(USARTx, "Sekai-sen Hendo-Ritsu Me-Ta: invalid command.\r\n");
                    break;
            }
            USART_SendStr(USARTx, "Configure mode out\r\n");
            conf_mode = 0;
        } else {
            switch (ch) {
                case '?':
                    TIM_SetCompare2(TIM2, 600 * 0.5);
                    Delay_ms(200);
                    TIM_SetCompare2(TIM2, 600 * 0);
                    USART_SendStr(USARTx, "Sekai-sen Hendo-Ritsu Me-Ta\r\n");
                    USART_SendStr(USARTx, " p - Print date\r\n");
                    USART_SendStr(USARTx, " r - Print RAW data\r\n");
                    USART_SendStr(USARTx, " c - Configure mode\r\n");
                    //USART_SendStr(USARTx, " b - Query battery voltage\r\n");
                    break;
                case 'p':
                    DS3231_ReadDateRAW(&date);

                    DS3231_DateToTimeStr(&date, st_tm);
                    DS3231_DateToDateStr(&date, st_dt);
                    USART_SendStr(USARTx, "Now: ");
                    USART_SendStr(USARTx, st_dt);
                    USART_SendStr(USARTx, ", ");
                    USART_SendStr(USARTx, st_tm);
                    USART_SendStr(USARTx, ", ");
                    USART_SendBuf(USARTx, &DOW[date.day_of_week - 1][0], strlen(&DOW[date.day_of_week - 1][0]));
                    USART_SendStr(USARTx, "\r\n");
                    break;
                case 'r':
                    DS3231_ReadDateRAW(&date);

                    USART_SendStr(USARTx, "RAW date time: ");
                    USART_SendBufHex(USARTx, (char*) &date, 7);
                    USART_SendStr(USARTx, "\r\n");
                    break;
                case 'c':
                    USART_SendStr(USARTx, "Configure Mode:\r\n");
                    USART_SendStr(USARTx, " h - Set hour\r\n");
                    USART_SendStr(USARTx, " m - Set minute\r\n");
                    USART_SendStr(USARTx, " s - Set second\r\n");
                    USART_SendStr(USARTx, " y - Set year\r\n");
                    USART_SendStr(USARTx, " o - Set month\r\n");
                    USART_SendStr(USARTx, " d - Set day\r\n");
                    USART_SendStr(USARTx, " w - Set day of week\r\n");
                    conf_mode = 1;
                    break;
                case 'b':
                    voltage = GetAdcResult();
                    sprintf(tmp, "%.2fV\r\n", voltage);
                    USART_SendStr(USARTx, "Battery Voltage: ");
                    USART_SendStr(USARTx, tmp);


                    break;
                default:
                    USART_SendStr(USARTx, "Sekai-sen Hendo-Ritsu Me-Ta: invalid command.\r\n");
                    break;
            }
        }

    }

}
 */


void USART_SendChar(USART_TypeDef* USARTx, char ch) {
    USART_SendData(USARTx, ch);
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET) {
    }
}

void USART_SendStr(USART_TypeDef* USARTx, char *str) {
    //while (*str) USART_SendChar(USARTx, *str++);
    SendString(str);
}

void USART_SendBuf(USART_TypeDef* USARTx, char *buf, unsigned int bufsize) {
    unsigned int i;
    for (i = 0; i < bufsize; i++) USART_SendChar(USARTx, *buf++);
}

void USART_SendBufHex(USART_TypeDef* USARTx, char *buf, unsigned int bufsize) {
    unsigned int i;
    char ch;
    for (i = 0; i < bufsize; i++) {
        ch = *buf++;
        USART_SendChar(USARTx, "0123456789ABCDEF"[(ch >> 4) % 0x10]);
        USART_SendChar(USARTx, "0123456789ABCDEF"[(ch & 0x0f) % 0x10]);
    }
}

void USART1_Init(void) {

    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure); //TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // RX as in without pull-up
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);


    /*
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART2, &USART_InitStructure);
    USART_Cmd(USART2, ENABLE);
     */

}
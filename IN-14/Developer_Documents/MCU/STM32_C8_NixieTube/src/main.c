#include "stm32f10x.h"
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include <string.h>
#include "stdio.h"
#include "delay.h"
#include "ds3231.h"
#include "nixietube.h"
#include "battery_adc.h"
#include "flash.h"
#include <stdlib.h>

unsigned char USB_Tx_Buffer[VIRTUAL_COM_PORT_DATA_SIZE]; /* Send Buffer: 64 Bytes*/

//__IO uint16_t CCR1_Val = 0x0;
LED5050 LED;
uint8_t NixieTube_EN = 1;
uint8_t AutoPowerOff = 0;

void GPIO_Configuration(void);
void TIM3_Configuration();

void SendString(const unsigned char *string) {
    strcpy(USB_Tx_Buffer, string);
    UserToPMABufferCopy(USB_Tx_Buffer, ENDP1_TXADDR, strlen(string)); /* Send Data */
    SetEPTxCount(ENDP1, strlen(string));
    SetEPTxValid(ENDP1);
    while (!((_GetENDPOINT(ENDP1) & EPTX_STAT) == EP_TX_NAK));
}

void SendMessage(const unsigned char *message, size_t length) {
    memcpy(USB_Tx_Buffer, message, length);
    UserToPMABufferCopy(USB_Tx_Buffer, ENDP1_TXADDR, length); /* Send Data */
    SetEPTxCount(ENDP1, length);
    SetEPTxValid(ENDP1);
    while (!((_GetENDPOINT(ENDP1) & EPTX_STAT) == EP_TX_NAK));
}

void SPEAKER_BEEP_ONE() {
    TIM_SetCompare2(TIM2, 250 * 0.5);
    Delay_ms(200);
    TIM_SetCompare2(TIM2, 250 * 0);

}

void PWM_Configuration() {

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    uint16_t PrescalerValue = 0;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO | RCC_APB2Periph_TIM1, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* TIM4 TIM2 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4 | RCC_APB1Periph_TIM2, ENABLE);




    /* -----------------------------------------------------------------------
      The TIM4 is running at 36 KHz: TIM4 Frequency = TIM4 counter clock/(ARR + 1)
                                                    = 24 MHz / 666 = 36 KHz
      TIM4 Channel1 duty cycle = (TIM3_CCR1/ TIM3_ARR)* 100 = 50%
      TIM4 Channel2 duty cycle = (TIM3_CCR2/ TIM3_ARR)* 100 = 37.5%
      TIM4 Channel3 duty cycle = (TIM3_CCR3/ TIM3_ARR)* 100 = 25%
      TIM4 Channel4 duty cycle = (TIM3_CCR4/ TIM3_ARR)* 100 = 12.5%
    ----------------------------------------------------------------------- */

    /* Compute the prescaler value */
    PrescalerValue = (uint16_t) (SystemCoreClock / 1000000) - 1;
    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = 1000;
    TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    TIM_TimeBaseStructure.TIM_Period = 250;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //BEEP

    /* PWM1 Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 1000;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM4, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);


    /*TIMER 1*/
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 100;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;

    TIM_OC1Init(TIM1, &TIM_OCInitStructure);


    /* PWM1 Mode configuration: Channel2 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;

    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 1000 * 0;

    TIM_OC2Init(TIM4, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);

    /* PWM1 Mode configuration: Channel3 */
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 1000 * 0;

    TIM_OC3Init(TIM4, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);


    /* PWM1 Mode configuration: Channel4 */
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 1000 * 0;

    TIM_OC4Init(TIM4, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);

    /* PWM1 Mode configuration: TIM2 Channel2 */
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 250 * 0;

    TIM_OC2Init(TIM2, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);



    TIM_ARRPreloadConfig(TIM4, ENABLE);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_ARRPreloadConfig(TIM1, ENABLE);

    /* TIM2 TIM4 TIM1 enable counter */
    TIM_Cmd(TIM2, ENABLE);
    TIM_Cmd(TIM4, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

void NVIC_Configuration() {
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);

    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable the USB Wake-up interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USBWakeUp_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

    NVIC_Init(&NVIC_InitStructure);

    /* Enable USART Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;

    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void TIM3_Configuration() {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 50000 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = (72 - 1);
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    TIM_ITConfig(TIM3, TIM_IT_Update | TIM_IT_Trigger, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
}

void IWDG_Configuration(void) {
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_256); //6.4ms
    IWDG_SetReload(350);
    IWDG_ReloadCounter();
    IWDG_Enable();
}

int main(void) {
    
    SystemInit();
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x08004000);
    
    Delay_init(72);

    GPIO_Configuration();



    Set_System();
    Set_USBClock();
    NVIC_Configuration(); //USB_Interrupts_Config();
    USB_Init();


    USART1_Init();
    PWM_Configuration();
    Flash_ReadData(&LED);
    AutoPowerOff = Flash_ReadConfiguration() & 0x1;
    NixieTube_LED_Set_RGB(LED);


    DS3231_Init();

    uint8_t i, j, k;
    volatile uint32_t t;

    if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 0) {
        //Enter self-check mode
        SET_CPLD_EN_HIGH;
        SET_CPLD_SEL_HIGH;
        while (1) {
            for (k = 1; k <= 12; k++) {
                Delay_ms(500);
                SPEAKER_BEEP_ONE();
                for (i = 1; i <= 6; i++) {

                    for (j = 12; j >= 1; j--) {
                        if (j == k) SET_CPLD_DATA_HIGH;
                        else SET_CPLD_DATA_LOW;
                        SET_CPLD_CLK_HIGH;
                        Delay_us(10);
                        SET_CPLD_CLK_LOW;
                    }
                }
            }
        }
    }

    DS3231_date_TypeDef date;
    //DS3231_ReadDateRAW(&date);
    //srand(*(int *) &date); //Generate seed for rand

    SPEAKER_BEEP_ONE();
    TIM3_Configuration();

    IWDG_Configuration();
    while (1) {
        IWDG_ReloadCounter();

        if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 0) { //S2: Cancel auto poweroff and enable/disable nixie tube
            TIM_Cmd(TIM3, DISABLE);
            
            if (NixieTube_EN) {
                SPEAKER_BEEP_THREE_DOWN();
            } else {
                SPEAKER_BEEP_THREE_UP();
            }
            NixieTube_EN = 1 - NixieTube_EN;
            AutoPowerOff = 0;
            TIM_Cmd(TIM3, ENABLE);



        }

        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0) {
            TIM_Cmd(TIM3, DISABLE);

            SPEAKER_BEEP_ONE();

            DS3231_ReadDateRAW(&date);
            int8_t new_second = ((date.seconds >> 4) * 10 + (date.seconds & 0xF)) - 1;
            new_second = new_second >= 60 ? new_second = 59 : new_second <= 0 ? new_second = 0 : new_second;


            date.seconds = ((new_second / 10) << 4) | (new_second % 10);
            DS3231_WriteDateRAW(&date);
            TIM_Cmd(TIM3, ENABLE);
        }

        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) == 0) {
            TIM_Cmd(TIM3, DISABLE);

            SPEAKER_BEEP_ONE();

            DS3231_ReadDateRAW(&date);
            int8_t new_second = ((date.seconds >> 4) * 10 + (date.seconds & 0xF)) + 1;
            new_second = new_second >= 60 ? new_second = 59 : new_second <= 0 ? new_second = 0 : new_second;
            date.seconds = ((new_second / 10) << 4) | (new_second % 10);
            DS3231_WriteDateRAW(&date);
            TIM_Cmd(TIM3, ENABLE);


        }

    }


}

void GPIO_Configuration(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

}

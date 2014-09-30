
#include "stm32f0xx.h"
#include "pid.h"
#include "nixietube.h"
#include "delay.h"
#include "ds1302.h"

#define ADC1_DR_Address    0x40012440

__IO int16_t pwm_duty_cycle_power_170v; //Default: 1534 / 10
__IO uint16_t ccr1_val_tim2 = 3000;
__IO uint16_t conv_data_adc;
PID pid_power_170v;
uint8_t ds1302_time[8] = {0, 1, 1, 1, 0, 0, 0, 1};

//Var for Software PWM, Set MAX = 1000
//R G B Y
__IO uint16_t pwm_counter;
__IO uint16_t pwm_target[4] = {800, 1000, 1000, 1000};
uint8_t current_state = 0;

static void configuration_adc_dma(void) {
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    ADC_DeInit(ADC1);

    /* ADC1 Periph clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    

    /* Initialize ADC structure */
    ADC_StructInit(&ADC_InitStructure);

    /* Configure the ADC1 in continuous mode withe a resolution equal to 12 bits  */
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Backward;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_ChannelConfig(ADC1, ADC_Channel_3, ADC_SampleTime_1_5Cycles);

    /* ADC Calibration */
    ADC_GetCalibrationFactor(ADC1);

    /* ADC DMA request in circular mode */
    ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);

    /* Enable ADC_DMA */
    ADC_DMACmd(ADC1, ENABLE);

    /* Enable the ADC peripheral */
    ADC_Cmd(ADC1, ENABLE);

    /* Wait the ADRDY flag */
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY));

    /* ADC1 regular Software Start Conv */
    ADC_StartOfConversion(ADC1);

    /* DMA1 clock enable */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* DMA1 Channel1 Config */
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) ADC1_DR_Address;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) & conv_data_adc;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 1;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    /* DMA1 Channel1 enable */
    DMA_Cmd(DMA1_Channel1, ENABLE);
}

static void configuration_gpio(void) {
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOF, ENABLE);

}

static void configuration_irq_tim1(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //时钟使能

    TIM_TimeBaseStructure.TIM_Period = 120;
    TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock / 12000000) - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM1, TIM_FLAG_Update);
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);


    NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_UP_TRG_COM_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM1, ENABLE);

}

static void configuration_irq_tim2(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* TIM2 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /* Enable the TIM2 gloabal Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    uint16_t PrescalerValue = (uint16_t) (SystemCoreClock / 6000000) - 1;

    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = 65535;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    /* Prescaler configuration */
    TIM_PrescalerConfig(TIM2, PrescalerValue, TIM_PSCReloadMode_Immediate);

    /* Output Compare Timing Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = ccr1_val_tim2;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    TIM_OC1Init(TIM2, &TIM_OCInitStructure);

    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);
    TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);


    /* TIM2 enable counter */
    TIM_Cmd(TIM2, ENABLE);

}

static void configuration_hardware_pwm(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;


    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_1);


    uint16_t TimerPeriod = (SystemCoreClock / 31250) - 1;
    /* Compute CCR4 value to generate a duty cycle at 50% for channel 1 and 1N */
    pwm_duty_cycle_power_170v = (uint16_t) (((uint32_t) 1 * (TimerPeriod - 1)) / 10);

    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
    TIM_OCInitStructure.TIM_Pulse = pwm_duty_cycle_power_170v;
    TIM_OC4Init(TIM3, &TIM_OCInitStructure);

    TIM_Cmd(TIM3, ENABLE);


}

static void configuration_key(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

}

int main(void) {

    configuration_delay(48);
    configuration_gpio();
    configuration_key();
    configuration_hardware_pwm();
    configuration_adc_dma();
    configuration_pid(&pid_power_170v, 1, 0, 0, 20); //2085
    configuration_irq_tim2();
    configuration_led();
    configuration_ds1302();

    delay_ms(500);
    configuration_nixietube();
    configuration_irq_tim1();


    //ds1302_time[6] = 20;
    //ds1302_writetime(ds1302_time);


    uint8_t now_second = 0, i;

    while (1) {

        delay_ms(50);
        ds1302_readtime(ds1302_time);

        while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9) == Bit_RESET) {
            ds1302_time[4] += 1;
            if (ds1302_time[4] >= 24) ds1302_time[4] = 0;
            ds1302_writetime(ds1302_time);
            nixietube_send_595_new_number(ds1302_time[4] / 10, ds1302_time[4] % 10);
            delay_ms(200);

        }

        while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_10) == Bit_RESET) {
            ds1302_time[5] += 1;
            if (ds1302_time[5] >= 60) ds1302_time[5] = 0;
            ds1302_writetime(ds1302_time);
            nixietube_send_595_new_number(ds1302_time[5] / 10, ds1302_time[5] % 10);
            delay_ms(200);
        }

        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == Bit_RESET) {
            if (pid_power_170v.SetPoint == 20) pid_power_170v.SetPoint = 0; else pid_power_170v.SetPoint = 20;
            delay_ms(2000);
        }


        if (now_second != ds1302_time[6]) {
            now_second = ds1302_time[6];
            current_state++;
            current_state = current_state % 8;
            if (current_state <= 3) {

                pwm_target[1] = pwm_target[3] = 0;

                //Display Hour
                if (current_state % 2 == 0) {
                    for (i = 0; i <= 100; i++) {
                        delay_ms(5);
                        pwm_target[2] = pwm_target[0] = i * 10;
                    }
                } else {
                    for (i = 100; i >= 1; i--) {
                        delay_ms(5);
                        pwm_target[2] = pwm_target[0] = i * 10;
                    }
                }



            } else {

                //Display Minute
                pwm_target[2] = pwm_target[0] = 0;

                if (current_state % 2 == 0) {

                    for (i = 0; i <= 100; i++) {
                        delay_ms(5);
                        pwm_target[1] = pwm_target[3] = i * 10;
                    }
                } else {
                    for (i = 100; i >= 1; i--) {
                        delay_ms(5);
                        pwm_target[1] = pwm_target[3] = i * 10;
                    }
                }

            }
        } //end update digits




    }
}

#ifdef  USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line) {
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1) {
    }
}
#endif

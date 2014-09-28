/**
 ******************************************************************************
 * @file    TIM/TIM_7PWMOutputs/stm32f0xx_it.c 
 * @author  MCD Application Team
 * @version V1.3.0
 * @date    16-January-2014
 * @brief   Main Interrupt Service Routines.
 *          This file provides template for all exceptions handler and 
 *          peripherals interrupt service routine.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
 *
 * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
 *
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" BASIS, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_it.h"
#include "pid.h"
extern __IO uint16_t ccr1_val_tim2;
extern __IO uint16_t conv_data_adc;
extern PID pid_power_170v;
extern __IO int16_t pwm_duty_cycle_power_170v;
extern __IO uint16_t pwm_counter;
extern __IO uint16_t pwm_target[4];
extern uint8_t ds1302_time[8];
extern uint8_t current_state;

uint16_t capture = 0;

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void) {
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1) {
    }
}

void SysTick_Handler(void) {
}

void TIM1_BRK_UP_TRG_COM_IRQHandler(void) {
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM1, TIM_FLAG_Update);
        //GPIO_WriteBit(GPIOF, GPIO_Pin_0, 1 - GPIO_ReadOutputDataBit(GPIOF, GPIO_Pin_0));

        if (++pwm_counter >= 1000) pwm_counter = 0;

        if (pwm_counter >= pwm_target[0]) GPIO_SetBits(GPIOF, GPIO_Pin_0);
        else GPIO_ResetBits(GPIOF, GPIO_Pin_0);

        if (pwm_counter >= pwm_target[1]) GPIO_SetBits(GPIOF, GPIO_Pin_1);
        else GPIO_ResetBits(GPIOF, GPIO_Pin_1);

        if (pwm_counter >= pwm_target[2]) {
            GPIO_SetBits(GPIOA, GPIO_Pin_0);
            if (current_state == 0 && pwm_counter % 10 == 0) nixietube_send_595_new_number(ds1302_time[5] / 10, ds1302_time[5] % 10);

        } else {
            GPIO_ResetBits(GPIOA, GPIO_Pin_0);
            if (current_state == 0 && pwm_counter % 10 == 0) nixietube_send_595_new_number(ds1302_time[4] / 10, ds1302_time[4] % 10);

        }

        if (pwm_counter >= pwm_target[3]) {
            GPIO_SetBits(GPIOA, GPIO_Pin_1);
            if (current_state == 4 && pwm_counter % 10 == 0) nixietube_send_595_new_number(ds1302_time[4] / 10, ds1302_time[4] % 10);

        } else {
            GPIO_ResetBits(GPIOA, GPIO_Pin_1);
            if (current_state == 4 && pwm_counter % 10 == 0) nixietube_send_595_new_number(ds1302_time[5] / 10, ds1302_time[5] % 10);

        }


    }
}

void TIM2_IRQHandler(void) {


    if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET) {
        TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
        TIM_SetCompare1(TIM2, ((uint16_t) TIM_GetCapture1(TIM2) + ccr1_val_tim2) % 65535);


        if (pid_power_170v.Enable) {
            int16_t new_pwm_duty_cycle = pwm_duty_cycle_power_170v + pid_calc(&pid_power_170v, conv_data_adc / 100);

            if (new_pwm_duty_cycle > 650) pwm_duty_cycle_power_170v = 650;
            else //767
                if (new_pwm_duty_cycle < 0) pwm_duty_cycle_power_170v = 0;
            else
                if (new_pwm_duty_cycle - pwm_duty_cycle_power_170v > 50) pwm_duty_cycle_power_170v += 50;
            else
                if (new_pwm_duty_cycle - pwm_duty_cycle_power_170v < -50) pwm_duty_cycle_power_170v -= 50;
            else
                pwm_duty_cycle_power_170v = new_pwm_duty_cycle;


            TIM_SetCompare4(TIM3, pwm_duty_cycle_power_170v);
        }



    }

}

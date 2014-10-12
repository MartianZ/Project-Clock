/**
 ******************************************************************************
 * @file    SPI/CRC/stm32f10x_it.c 
 * @author  MCD Application Team
 * @version V3.5.0
 * @date    08-April-2011
 * @brief   Main Interrupt Service Routines.
 *          This file provides template for all exceptions handler and peripherals
 *          interrupt service routine.
 ******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "usb_lib.h"
#include "usb_istr.h"
#include "hw_config.h"
#include "usb_prop.h"
#include <string.h>
#include "math.h"
#include "usart.h"
#include "ds3231.h"

//extern __IO uint16_t CCR1_Val;

DS3231_date_TypeDef date;


void USB_LP_CAN1_RX0_IRQHandler(void) {
    USB_Istr();
}

void USBWakeUp_IRQHandler(void) {

    EXTI_ClearITPendingBit(EXTI_Line18);
}

void USART1_IRQHandler(void) {


    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {

        //USART_To_USB_Send_Data();
        //USART_Terminal();
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);

    }

    if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET) {
        //(void) USART_ReceiveData(USART1);
    }
}

void TIM3_IRQHandler(void) {
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的 TIM 中断发生与否 
    {
        DS3231_ReadDateRAW(&date);
        NixieTube_Normal_SetNewTime(date);
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //清除TIMx的中断待处理位:TIM 中断源 

    }
}


/*
void TIM3_IRQHandler(void) {
    static uint8_t pwm = 0;
    if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET) {
        TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);        
        GPIOA->ODR ^= GPIO_Pin_4;  //GREEN LED
        
        if (pwm) TIM_SetCompare1(TIM3,  CCR1_Val);
        else TIM_SetCompare1(TIM3,  0xFFFF - CCR1_Val);
        pwm = 1 - pwm;
        
        TIM_SetCounter(TIM3, 0x0000);
    }
    
    
}
 */

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief  This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void) {
}

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

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler(void) {
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1) {
    }
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler(void) {
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1) {
    }
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler(void) {
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1) {
    }
}

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler(void) {
}

/**
 * @brief  This function handles SVCall exception.
 * @param  None
 * @retval None
 */
void SVC_Handler(void) {
}

/**
 * @brief  This function handles PendSV_Handler exception.
 * @param  None
 * @retval None
 */
void PendSV_Handler(void) {
}

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */
void SysTick_Handler(void) {
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
 * @brief  This function handles PPP interrupt request.
 * @param  None
 * @retval None
 */
/*void PPP_IRQHandler(void)
{
}*/

/**
 * @}
 */

/**
 * @}
 */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

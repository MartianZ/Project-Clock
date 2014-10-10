/**
 ******************************************************************************
 * @file    hw_config.c
 * @author  MCD Application Team
 * @version V4.0.0
 * @date    21-January-2013
 * @brief   Hardware Configuration & Setup
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
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

#include "stm32f10x_it.h"
#include "usb_lib.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "usb_pwr.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ErrorStatus HSEStartUpStatus;
USART_InitTypeDef USART_InitStructure;
EXTI_InitTypeDef EXTI_InitStructure;
uint8_t USART_Rx_Buffer [USART_RX_DATA_SIZE];
uint32_t USART_Rx_ptr_in = 0;
uint32_t USART_Rx_ptr_out = 0;
uint32_t USART_Rx_length = 0;

uint8_t USB_Tx_State = 0;
static void IntToUnicode(uint32_t value, uint8_t *pbuf, uint8_t len);
/* Extern variables ----------------------------------------------------------*/

extern LINE_CODING linecoding;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
 * Function Name  : Set_System
 * Description    : Configures Main system clocks & power
 * Input          : None.
 * Return         : None.
 *******************************************************************************/
void Set_System(void) {
    //GPIO_InitTypeDef GPIO_InitStructure;

    /*!< At this stage the microcontroller clock setting is already configured, 
   this is done through SystemInit() function which is called from startup
   file (startup_stm32f10x_xx.s) before to branch to application main.
   To reconfigure the default setting of SystemInit() function, refer to
   system_stm32f10x.c file
     */
    /* Enable USB_DISCONNECT GPIO clock */
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_DISCONNECT, ENABLE);

    /* Configure USB pull-up pin */
    //GPIO_InitStructure.GPIO_Pin = USB_DISCONNECT_PIN;
    //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    //GPIO_Init(USB_DISCONNECT, &GPIO_InitStructure);

    /* Configure the EXTI line 18 connected internally to the USB IP */
    EXTI_ClearITPendingBit(EXTI_Line18);
    EXTI_InitStructure.EXTI_Line = EXTI_Line18;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

/*******************************************************************************
 * Function Name  : Set_USBClock
 * Description    : Configures USB Clock input (48MHz)
 * Input          : None.
 * Return         : None.
 *******************************************************************************/
void Set_USBClock(void) {
    /* Select USBCLK source */
    RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);

    /* Enable the USB clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
}

/*******************************************************************************
 * Function Name  : Enter_LowPowerMode
 * Description    : Power-off system clocks and power while entering suspend mode
 * Input          : None.
 * Return         : None.
 *******************************************************************************/
void Enter_LowPowerMode(void) {
    /* Set the device state to suspend */
    bDeviceState = SUSPENDED;
}

/*******************************************************************************
 * Function Name  : Leave_LowPowerMode
 * Description    : Restores system clocks and power while exiting suspend mode
 * Input          : None.
 * Return         : None.
 *******************************************************************************/
void Leave_LowPowerMode(void) {
    DEVICE_INFO *pInfo = &Device_Info;

    /* Set the device state to the correct state */
    if (pInfo->Current_Configuration != 0) {
        /* Device configured */
        bDeviceState = CONFIGURED;
    } else {
        bDeviceState = ATTACHED;
    }
    /*Enable SystemCoreClock*/
    SystemInit();
}

/*******************************************************************************
 * Function Name  : USB_Interrupts_Config
 * Description    : Configures the USB interrupts
 * Input          : None.
 * Return         : None.
 *******************************************************************************/
void USB_Interrupts_Config(void) {

}

/*******************************************************************************
 * Function Name  : USB_Cable_Config
 * Description    : Software Connection/Disconnection of USB Cable
 * Input          : None.
 * Return         : Status
 *******************************************************************************/
void USB_Cable_Config(FunctionalState NewState) {
    //if (NewState != DISABLE)
    //{
    //  GPIO_ResetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
    //}
    //else
    //{
    //  GPIO_SetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
    //}
}

/*******************************************************************************
 * Function Name  :  USART_Config_Default.
 * Description    :  configure the EVAL_COM1 with default values.
 * Input          :  None.
 * Return         :  None.
 *******************************************************************************/
void USART_Config_Default(void) {
    /* STM32F103C8T6_COM1 default configuration */
    /* STM32F103C8T6_COM1 configured as follow:
                                                                          - BaudRate = 9600 baud  
                                                                          - Word Length = 8 Bits
                                                                          - One Stop Bit
                                                                          - Parity Odd
                                                                          - Hardware flow control disabled
                                                                          - Receive and transmit enabled
     */
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIO clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    /* Enable USART1 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    /* Configure USART Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    /* Configure USART Rx as input floating */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure and enable the USART */
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_Odd;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    /* USART configuration */
    USART_Init(USART1, &USART_InitStructure);

    /* Enable USART */
    USART_Cmd(USART1, ENABLE);

    /* Enable the USART Receive interrupt */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}/*���������ִ��USB_Init();ʱ������*/

/*******************************************************************************
 * Function Name  :  USART_Config.
 * Description    :  Configure the EVAL_COM1 according to the line coding structure.
 * Input          :  None.
 * Return         :  Configuration status
                    TRUE : configuration done with success
                    FALSE : configuration aborted.
 *******************************************************************************/
bool USART_Config(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    /* set the Stop bit*/
    switch (linecoding.format) {
        case 0:
            USART_InitStructure.USART_StopBits = USART_StopBits_1; //1λֹͣλ
            break;
        case 1:
            USART_InitStructure.USART_StopBits = USART_StopBits_1_5; //1.5λֹͣλ
            break;
        case 2:
            USART_InitStructure.USART_StopBits = USART_StopBits_2; //2λֹͣλ
            break;
        default:
        {
            USART_Config_Default(); //���ò������������֮
            return (FALSE);
        }
    }

    /* set the parity bit*/ //��żУ��λ������
    switch (linecoding.paritytype) {
        case 0:
            USART_InitStructure.USART_Parity = USART_Parity_No;
            break;
        case 1:
            USART_InitStructure.USART_Parity = USART_Parity_Even;
            break;
        case 2:
            USART_InitStructure.USART_Parity = USART_Parity_Odd;
            break;
        default:
        {
            USART_Config_Default();
            return (FALSE);
        }
    }

    /*set the data type : only 8bits and 9bits is supported */ //���λ������
    switch (linecoding.datatype) {
        case 0x07:
            /* With this configuration a parity (Even or Odd) should be set */
            USART_InitStructure.USART_WordLength = USART_WordLength_8b;
            break;
        case 0x08:
            if (USART_InitStructure.USART_Parity == USART_Parity_No) {
                USART_InitStructure.USART_WordLength = USART_WordLength_8b;
            } else {
                USART_InitStructure.USART_WordLength = USART_WordLength_9b;
            }

            break;
        default:
        {
            USART_Config_Default();
            return (FALSE);
        }
    }

    /* Enable GPIO clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    /* Enable USART1 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    /* Configure USART Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    /* Configure USART Rx as input floating */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = linecoding.bitrate; //���ò�����
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    /* Configure and enable the USART */
    //STM_EVAL_COMInit(COM1, &USART_InitStructure);
    /* USART configuration */
    USART_Init(USART1, &USART_InitStructure);

    /* Enable USART */
    USART_Cmd(USART1, ENABLE);

    return (TRUE);
}//���������������USBת���ڵ�����

/*******************************************************************************
 * Function Name  : USB_To_USART_Send_Data.
 * Description    : send the received data from USB to the UART 0.
 * Input          : data_buffer: data address.
                   Nb_bytes: number of bytes to send.
 * Return         : none.
 *******************************************************************************/
void USB_To_USART_Send_Data(uint8_t* data_buffer, uint8_t Nb_bytes) {
    uint32_t i;

    for (i = 0; i < Nb_bytes; i++) {
        USART_SendData(USART1, *(data_buffer + i));
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    }
}//���ʹ�������ܵ���ݵ������ϣ�������˵�3�Ļص�������ʵ�ֵ�

/*******************************************************************************
 * Function Name  : Handle_USBAsynchXfer.
 * Description    : send data to USB.
 * Input          : None.
 * Return         : none.
 *******************************************************************************/
void Handle_USBAsynchXfer(void) {
    uint16_t USB_Tx_ptr;
    uint16_t USB_Tx_length;

    if (USB_Tx_State != 1) {
        if (USART_Rx_ptr_out == USART_RX_DATA_SIZE) {
            USART_Rx_ptr_out = 0;
        }

        if (USART_Rx_ptr_out == USART_Rx_ptr_in) {
            USB_Tx_State = 0;
            return;
        }

        if (USART_Rx_ptr_out > USART_Rx_ptr_in) /* rollback */ {
            USART_Rx_length = USART_RX_DATA_SIZE - USART_Rx_ptr_out;
        } else {
            USART_Rx_length = USART_Rx_ptr_in - USART_Rx_ptr_out;
        }

        if (USART_Rx_length > VIRTUAL_COM_PORT_DATA_SIZE) {
            USB_Tx_ptr = USART_Rx_ptr_out;
            USB_Tx_length = VIRTUAL_COM_PORT_DATA_SIZE;

            USART_Rx_ptr_out += VIRTUAL_COM_PORT_DATA_SIZE;
            USART_Rx_length -= VIRTUAL_COM_PORT_DATA_SIZE;
        } else {
            USB_Tx_ptr = USART_Rx_ptr_out;
            USB_Tx_length = USART_Rx_length;

            USART_Rx_ptr_out += USART_Rx_length;
            USART_Rx_length = 0;
        }
        USB_Tx_State = 1;
        UserToPMABufferCopy(&USART_Rx_Buffer[USB_Tx_ptr], ENDP1_TXADDR, USB_Tx_length);
        SetEPTxCount(ENDP1, USB_Tx_length);
        SetEPTxValid(ENDP1);
    }
}//���ܴ�USB���ͻ��������

/*******************************************************************************
 * Function Name  : UART_To_USB_Send_Data.
 * Description    : send the received data from UART 0 to USB.
 * Input          : None.
 * Return         : none.
 *******************************************************************************/
void USART_To_USB_Send_Data(void)//�����ڽ����жϵ��ñ�����յ���ݵ�������
{

    if (linecoding.datatype == 7) {
        USART_Rx_Buffer[USART_Rx_ptr_in] = USART_ReceiveData(USART1) & 0x7F;
    } else if (linecoding.datatype == 8) {
        USART_Rx_Buffer[USART_Rx_ptr_in] = USART_ReceiveData(USART1);
    }

    USART_Rx_ptr_in++;

    /* To avoid buffer overflow */
    if (USART_Rx_ptr_in == USART_RX_DATA_SIZE) {
        USART_Rx_ptr_in = 0;
    }
}//���ʹ��ڽ��ܵ���ݵ�USB

/*******************************************************************************
 * Function Name  : Get_SerialNum.
 * Description    : Create the serial number string descriptor.
 * Input          : None.
 * Output         : None.
 * Return         : None.
 *******************************************************************************/
void Get_SerialNum(void) {
    uint32_t Device_Serial0, Device_Serial1, Device_Serial2;

    Device_Serial0 = *(uint32_t*) ID1;
    Device_Serial1 = *(uint32_t*) ID2;
    Device_Serial2 = *(uint32_t*) ID3;

    Device_Serial0 += Device_Serial2;

    if (Device_Serial0 != 0) {
        IntToUnicode(Device_Serial0, &Virtual_Com_Port_StringSerial[2], 8);
        IntToUnicode(Device_Serial1, &Virtual_Com_Port_StringSerial[18], 4);
    }
}

/*******************************************************************************
 * Function Name  : HexToChar.
 * Description    : Convert Hex 32Bits value into char.
 * Input          : None.
 * Output         : None.
 * Return         : None.
 *******************************************************************************/
static void IntToUnicode(uint32_t value, uint8_t *pbuf, uint8_t len) {
    uint8_t idx = 0;

    for (idx = 0; idx < len; idx++) {
        if (((value >> 28)) < 0xA) {
            pbuf[ 2 * idx] = (value >> 28) + '0';
        } else {
            pbuf[2 * idx] = (value >> 28) + 'A' - 10;
        }

        value = value << 4;

        pbuf[ 2 * idx + 1] = 0;
    }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

/**
 ******************************************************************************
 * @author  MartianZ @ UESTC - ICKX
 * @brief   
 ******************************************************************************
 **/


#include "ds3231.h"
#include <string.h>

const char *DOW[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

void DS3231_ReadDateRAW(DS3231_date_TypeDef* date) {


    unsigned int i;
    char buffer[7];


    I2C_AcknowledgeConfig(I2C2, ENABLE); // Enable I2C acknowledge

    I2C_GenerateSTART(I2C2, ENABLE); // Send START condition
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)); // Wait for EV5

    I2C_Send7bitAddress(I2C2, DS3231_Addr, I2C_Direction_Transmitter); // Send DS3231 slave address
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)); // Wait for EV6
    I2C_Cmd(I2C2, ENABLE);
    //eGPIO_SetBits(GPIOA, GPIO_Pin_0);

    I2C_SendData(I2C2, DS3231_Seconds); // Send DS3231 seconds register address
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED)); // Wait for EV8

    I2C_GenerateSTART(I2C2, ENABLE); // Send repeated START condition (aka Re-START)
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)); // Wait for EV5

    I2C_Send7bitAddress(I2C2, DS3231_Addr, I2C_Direction_Receiver); // Send DS3231 slave address for READ
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)); // Wait for EV6

    for (i = 0; i < 6; i++) {
        while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED)); // Wait for EV7 (Byte received from slave)
        buffer[i] = I2C_ReceiveData(I2C2); // Receive byte
    }

    I2C_AcknowledgeConfig(I2C2, DISABLE); // Disable I2C acknowledgement
    I2C_GenerateSTOP(I2C2, ENABLE); // Send STOP condition

    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED)); // Wait for EV7 (Byte received from slave)
    //GPIO_ResetBits(GPIOA, GPIO_Pin_0);

    buffer[i] = I2C_ReceiveData(I2C2); // Receive last byte

    memcpy(date, &buffer[0], 7);


}

void DS3231_WriteDateRAW(DS3231_date_TypeDef* date) {
    unsigned int i;
    char buffer[7];

    memcpy(&buffer[0], date, 7);

    I2C_AcknowledgeConfig(I2C2, ENABLE); // Enable I2C acknowledge

    I2C_GenerateSTART(I2C2, ENABLE); // Send START condition
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)); // Wait for EV5

    I2C_Send7bitAddress(I2C2, DS3231_Addr, I2C_Direction_Transmitter); // Send DS3231 slave address
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)); // Wait for EV6

    I2C_SendData(I2C2, DS3231_Seconds); // Send DS3231 seconds register address
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED)); // Wait for EV8

    for (i = 0; i < 7; i++) {
        I2C_SendData(I2C2, buffer[i]); // Send DS3231 seconds register address
        while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED)); // Wait for EV8
    }

    I2C_GenerateSTOP(I2C2, ENABLE);
}

void DS3231_ReadDate(HRF_date_TypeDef* hrf_date) {
    DS3231_date_TypeDef raw_date;

    DS3231_ReadDateRAW(&raw_date);

    hrf_date->Seconds = (raw_date.seconds >> 4) * 10 + (raw_date.seconds & 0x0f);
    hrf_date->Minutes = (raw_date.minutes >> 4) * 10 + (raw_date.minutes & 0x0f);
    hrf_date->Hours = (raw_date.hours >> 4) * 10 + (raw_date.hours & 0x0f);
    hrf_date->Day = (raw_date.date >> 4) * 10 + (raw_date.date & 0x0f);
    hrf_date->Month = (raw_date.month >> 4) * 10 + (raw_date.month & 0x0f);
    hrf_date->Year = (raw_date.year >> 4) * 10 + (raw_date.year & 0x0f) + 2000;
    hrf_date->DOW = raw_date.day_of_week;
}

void DS3231_DateToTimeStr(DS3231_date_TypeDef* raw_date, char *str) {
    *str++ = (raw_date->hours >> 4) + '0';
    *str++ = (raw_date->hours & 0x0f) + '0';
    *str++ = ':';
    *str++ = (raw_date->minutes >> 4) + '0';
    *str++ = (raw_date->minutes & 0x0f) + '0';
    *str++ = ':';
    *str++ = (raw_date->seconds >> 4) + '0';
    *str++ = (raw_date->seconds & 0x0f) + '0';
    *str++ = 0;
}

void DS3231_DateToDateStr(DS3231_date_TypeDef* raw_date, char *str) {

    *str++ = '2';
    *str++ = '0';
    *str++ = (raw_date->year >> 4) + '0';
    *str++ = (raw_date->year & 0x0f) + '0';
    *str++ = '.';
    *str++ = (raw_date->month >> 4) + '0';
    *str++ = (raw_date->month & 0x0f) + '0';
    *str++ = '.';
    *str++ = (raw_date->date >> 4) + '0';
    *str++ = (raw_date->date & 0x0f) + '0';
    *str++ = 0;




}

uint8_t DS3231_ReadTemp(void) {
    I2C_AcknowledgeConfig(I2C2, ENABLE); // Enable I2C acknowledge

    I2C_GenerateSTART(I2C2, ENABLE); // Send START condition
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)); // Wait for EV5

    I2C_Send7bitAddress(I2C2, DS3231_Addr, I2C_Direction_Transmitter); // Send DS3231 slave address
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)); // Wait for EV6

    I2C_SendData(I2C2, DS3231_Tmp_MSB); // Send DS3231 temperature MSB register address
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED)); // Wait for EV8

    I2C_GenerateSTART(I2C2, ENABLE); // Send repeated START condition (aka Re-START)
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)); // Wait for EV5

    I2C_Send7bitAddress(I2C2, DS3231_Addr, I2C_Direction_Receiver); // Send DS3231 slave address for READ
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)); // Wait for EV6

    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED)); // Wait for EV7 (Byte received from slave)
    uint8_t temperature = I2C_ReceiveData(I2C2); // Receive temperature MSB

    I2C_AcknowledgeConfig(I2C2, DISABLE); // Disable I2C acknowledgement

    I2C_GenerateSTOP(I2C2, ENABLE); // Send STOP condition
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED)); // Wait for EV7 (Byte received from slave)

    return temperature;
}

void DS3231_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //IIC2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    I2C_InitTypeDef I2C_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE); // Enable I2C clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    I2C_DeInit(I2C2); // I2C reset to initial state
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C; // I2C mode is I2C ^_^
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2; // I2C fast mode duty cycle
    I2C_InitStructure.I2C_OwnAddress1 = 1; // Device address (7-bit or 10-bit)
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable; // Acknowledgement enable
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // choose 7-bit address for acknowledgement
    I2C_InitStructure.I2C_ClockSpeed = 400000; // 400kHz
    I2C_Cmd(I2C2, ENABLE); // Enable I2C
    I2C_Init(I2C2, &I2C_InitStructure); // Configure I2C

    while (I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY)); // Wait until I2C free

    // Check connection to DS3231
    I2C_GenerateSTART(I2C2, ENABLE); // Send START condition
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)); // Wait for EV5
    I2C_Send7bitAddress(I2C2, DS3231_Addr, I2C_Direction_Transmitter); // Send DS3231 slave address
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)); // Wait for EV6
    I2C_GenerateSTOP(I2C2, ENABLE);



}

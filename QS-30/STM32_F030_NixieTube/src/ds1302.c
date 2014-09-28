#include "ds1302.h"
#include "delay.h"
#define DS1302_GPIO	GPIOA
#define DS1302_SCLK	GPIO_Pin_4
#define DS1302_SDA	GPIO_Pin_6
#define DS1302_RST	GPIO_Pin_5	

#define ds1302_sec_add			0x80
#define ds1302_min_add			0x82
#define ds1302_hr_add			0x84
#define ds1302_date_add			0x86
#define ds1302_month_add		0x88
#define ds1302_day_add			0x8a
#define ds1302_year_add			0x8c
#define ds1302_control_add		0x8e
#define ds1302_charger_add		0x90
#define ds1302_clkburst_add		0xbe

#define HEX2BCD(v)	((v) % 10 + (v) / 10 * 16)
#define BCD2HEX(v)	((v) % 16 + (v) / 16 * 10)

static inline void configuration_ds1302_read_data(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = DS1302_SDA;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DS1302_GPIO, &GPIO_InitStructure);

}

static inline void configuration_ds1302_write_data(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = DS1302_SDA;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DS1302_GPIO, &GPIO_InitStructure);

}

static void ds1302_writebyte(uint8_t addr, uint8_t d) {
    uint8_t i;

    //	DS1302_RST = 1;
    GPIO_WriteBit(DS1302_GPIO, DS1302_RST, Bit_SET);

    addr = addr & 0xFE;

    configuration_ds1302_write_data();

    for (i = 0; i < 8; i++) {
        //		DS1302_SDA = (bit)(addr & 1); 
        GPIO_WriteBit(DS1302_GPIO, DS1302_SDA, (addr & 1) ? Bit_SET : Bit_RESET);
        //		DS1302_SCK = 1;
        GPIO_WriteBit(DS1302_GPIO, DS1302_SCLK, Bit_SET);
        //		DS1302_SCK = 0;
        GPIO_WriteBit(DS1302_GPIO, DS1302_SCLK, Bit_RESET);
        addr >>= 1;
    }

    for (i = 0; i < 8; i++) {
        //		DS1302_SDA = (bit)(d & 1);
        GPIO_WriteBit(DS1302_GPIO, DS1302_SDA, (d & 1) ? Bit_SET : Bit_RESET);
        //		DS1302_SCK = 1;
        GPIO_WriteBit(DS1302_GPIO, DS1302_SCLK, Bit_SET);
        //		DS1302_SCK = 0;
        GPIO_WriteBit(DS1302_GPIO, DS1302_SCLK, Bit_RESET);
        d >>= 1;
    }

    //	DS1302_RST = 0;
    GPIO_WriteBit(DS1302_GPIO, DS1302_RST, Bit_RESET);
}

static uint8_t ds1302_readbyte(uint8_t addr) {
    uint8_t i;
    uint8_t temp = 0;

    //	DS1302_RST = 1;
    GPIO_WriteBit(DS1302_GPIO, DS1302_RST, Bit_SET);
    addr = addr | 0x01; //最低位置高
    configuration_ds1302_write_data();

    for (i = 0; i < 8; i++) {
        //		DS1302_SDA = (bit)(addr & 1);
        GPIO_WriteBit(DS1302_GPIO, DS1302_SDA, (addr & 1) ? Bit_SET : Bit_RESET);
        //		DS1302_SCK = 1;
        GPIO_WriteBit(DS1302_GPIO, DS1302_SCLK, Bit_SET);
        //		DS1302_SCK = 0; 
        GPIO_WriteBit(DS1302_GPIO, DS1302_SCLK, Bit_RESET);
        addr >>= 1;
    }
    configuration_ds1302_read_data();
    for (i = 0; i < 8; i++) {
        temp >>= 1;
        //		if(DS1302_SDA)
        if (GPIO_ReadInputDataBit(DS1302_GPIO, DS1302_SDA))
            temp |= 0x80;
        //		DS1302_SCK = 1;
        GPIO_WriteBit(DS1302_GPIO, DS1302_SCLK, Bit_SET);
        //		DS1302_SCK = 0;
        GPIO_WriteBit(DS1302_GPIO, DS1302_SCLK, Bit_RESET);
    }

    //	DS1302_RST = 0;
    GPIO_WriteBit(DS1302_GPIO, DS1302_RST, Bit_RESET);
    return temp;
}

void ds1302_writetime(uint8_t *buf) {
    ds1302_writebyte(ds1302_control_add, 0x00); //关闭写保护 
    ds1302_writebyte(ds1302_sec_add, 0x80); //暂停 
    //ds1302_Write_Byte(ds1302_charger_add,0xa9);			//涓流充电 
    ds1302_writebyte(ds1302_year_add, HEX2BCD(buf[1])); //年 
    ds1302_writebyte(ds1302_month_add, HEX2BCD(buf[2])); //月 
    ds1302_writebyte(ds1302_date_add, HEX2BCD(buf[3])); //日  
    ds1302_writebyte(ds1302_hr_add, HEX2BCD(buf[4])); //时 
    ds1302_writebyte(ds1302_min_add, HEX2BCD(buf[5])); //分
    ds1302_writebyte(ds1302_sec_add, HEX2BCD(buf[6])); //秒
    ds1302_writebyte(ds1302_day_add, HEX2BCD(buf[7])); //周 
    ds1302_writebyte(ds1302_control_add, 0x80); //打开写保护 
}

void ds1302_readtime(uint8_t *buf) {
    uint8_t tmp;

    tmp = ds1302_readbyte(ds1302_year_add);
    buf[1] = BCD2HEX(tmp); //年

    tmp = ds1302_readbyte(ds1302_month_add);
    buf[2] = BCD2HEX(tmp); //月

    tmp = ds1302_readbyte(ds1302_date_add);
    buf[3] = BCD2HEX(tmp); //日

    tmp = ds1302_readbyte(ds1302_hr_add);
    buf[4] = BCD2HEX(tmp); //时

    tmp = ds1302_readbyte(ds1302_min_add);
    buf[5] = BCD2HEX(tmp); //分

    tmp = ds1302_readbyte((ds1302_sec_add))&0x7F;
    buf[6] = BCD2HEX(tmp); //秒

    tmp = ds1302_readbyte(ds1302_day_add);
    buf[7] = BCD2HEX(tmp); //周 
}

void configuration_ds1302(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = DS1302_SCLK | DS1302_SDA | DS1302_RST;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DS1302_GPIO, &GPIO_InitStructure);

    GPIO_WriteBit(DS1302_GPIO, DS1302_RST, Bit_SET);
    delay_ms(10);
    GPIO_WriteBit(DS1302_GPIO, DS1302_RST, Bit_RESET);

    GPIO_WriteBit(DS1302_GPIO, DS1302_SCLK, Bit_RESET);
    ds1302_writebyte(ds1302_sec_add, 0x00);

}

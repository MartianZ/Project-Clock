#ifndef __DS12C887_H__
#define __DS12C887_H__


#define DS_SEC  0x00
#define DS_MIN  0x02
#define DS_HOR  0x04
#define DS_WEK  0x06
#define DS_DAY  0x07
#define DS_MON  0x08
#define DS_YER  0x09
#define DS_R_A  0x0A
#define DS_R_B  0x0B
#define DS_R_C  0x0C
#define DS_R_D  0x0D
#define DS_Y_S  0x0E

#define DEC_BCD(X)  (((X / 10) << 4) | (X % 10))
#define BCD_DEC(X)  (((X & 0xF0) >> 4) * 10 + (X & 0x0F))

sbit DS_DS = P2^4;
sbit DS_RW = P5^0;
sbit DS_AS = P3^5;
sbit DS_CS = P3^4;

sbit P10 = P1^0;
sbit P11 = P1^1;
sbit P12 = P1^2;
sbit P13 = P1^3;
sbit P14 = P1^4;
sbit P15 = P1^5;
sbit P16 = P1^6;
sbit P52 = P5^2;

void DS12887_Write(unsigned char address,unsigned char dat);
void DS12887_Init(void);
unsigned char DS12887_Read(unsigned char address);

void DS12887_Write(unsigned char address,unsigned char dat)
{
    DS_AS = 1;
    DS_DS = 1;
    DS_RW = 1;
    DS_CS = 0;
    //DS_DAT = address;

	P10 = address & 1;
	P11 = (address >> 1) & 1;
	P12 = (address >> 2) & 1;
	P13 = (address >> 3) & 1;
	P14 = (address >> 4) & 1;
	P15 = (address >> 5) & 1;
	P16 = (address >> 6) & 1;
	P52 = (address >> 7) & 1;



    DS_AS = 0;  //add will be write when AS full down
    DS_RW = 0;
    //DS_DAT = dat;
	P10 = dat & 1;
	P11 = (dat >> 1) & 1;
	P12 = (dat >> 2) & 1;
	P13 = (dat >> 3) & 1;
	P14 = (dat >> 4) & 1;
	P15 = (dat >> 5) & 1;
	P16 = (dat >> 6) & 1;
	P52 = (dat >> 7) & 1;

    DS_RW = 1;
    DS_AS = 1;
    DS_CS = 1;
}

unsigned char DS12887_Read(unsigned char address)
{
    unsigned char d = 0;
    DS_AS = 1;
    DS_DS = 1;
    DS_RW = 1;
    DS_CS = 0;
    //DS_DAT = addres;
	P10 = address & 1;
	P11 = (address >> 1) & 1;
	P12 = (address >> 2) & 1;
	P13 = (address >> 3) & 1;
	P14 = (address >> 4) & 1;
	P15 = (address >> 5) & 1;
	P16 = (address >> 6) & 1;
	P52 = (address >> 7) & 1;

    DS_AS = 0;
    DS_DS = 0;
    //DS_DAT = 0xFF;
	P10 = 1;
	P11 = 1;
	P12 = 1;
	P13 = 1;
	P14 = 1;
	P15 = 1;
	P16 = 1;
	P52 = 1;

	if (P10 == 1) d+=1;
	if (P11 == 1) d+=2;
	if (P12 == 1) d+=4;
	if (P13 == 1) d+=8;
	if (P14 == 1) d+=16;
	if (P15 == 1) d+=32;
	if (P16 == 1) d+=64;
	if (P52 == 1) d+=128;
   

    DS_DS = 1;
    DS_AS = 1;
    DS_CS = 1;


    return d;
}

void DS12887_Init(void)
{
    if (DS12887_Read(DS_Y_S) == 0)
    {
        DS12887_Write(DS_Y_S, 0x20);
        DS12887_Write(DS_HOR, 0x12);
        DS12887_Write(DS_R_A, 0x20);
        DS12887_Write(DS_R_B, 0x02);  //BCD
    }
}

#endif

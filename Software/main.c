#include <STC_NEW_8051.h>
#include <INTRINS.h>
#include "delay.h"
#include "DS12C887.h"
#include "DS18B20.h"
#include "EEPROM.h"
sbit H1 = P0 ^ 0;
sbit H2 = P0 ^ 1;
sbit H3 = P0 ^ 2;
sbit H4 = P0 ^ 3;
sbit H5 = P0 ^ 4;
sbit H6 = P0 ^ 5;
sbit H7 = P0 ^ 6;

sbit BEEP = P4 ^ 3;

sbit INT0 = P3^2;
sbit INT1 = P3^3;

sbit HC595_DATA = P2 ^ 2;
sbit HC595_STCP = P2 ^ 1;
sbit HC595_SHCP = P2 ^ 0;

volatile unsigned char line = 1;
volatile unsigned long line_data[7]; 
volatile unsigned char busy = 0;
volatile unsigned char menu1 = 0;
volatile unsigned char menu2 = 0;

volatile unsigned char brightness = 2;

code unsigned char code_dark[7] = {0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F};

code unsigned char code_aa[7] = {0x1F, 0x1F, 0x1F, 0x11, 0xE, 0x0, 0xE};
code unsigned char code_pp[7] = {0x1F, 0x1F, 0x1F, 0x1, 0xE, 0x1, 0xF};
code unsigned char code_0_1[7] = {0x0, 0x4, 0x4, 0x4, 0x4, 0x4, 0x0};
code unsigned char code_1_1[7] = {0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19};
code unsigned char code_2_1[7] = {0x10, 0x1C, 0x1C, 0x0, 0x7, 0x7, 0x0};
code unsigned char code_3_1[7] = {0x0, 0x1C, 0x1C, 0x10, 0x1C, 0x1C, 0x0};
code unsigned char code_4_1[7] = {0x4, 0x4, 0x4, 0x0, 0x1C, 0x1C, 0x1C};
code unsigned char code_5_1[7] = {0x1, 0x7, 0x7, 0x0, 0x1C, 0x1C, 0x0};
code unsigned char code_6_1[7] = {0x1, 0x7, 0x7, 0x0, 0x4, 0x4, 0x0};
code unsigned char code_7_1[7] = {0x0, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C};
code unsigned char code_8_1[7] = {0x0, 0x4, 0x4, 0x0, 0x4, 0x4, 0x0};
code unsigned char code_9_1[7] = {0x0, 0x4, 0x4, 0x0, 0x1C, 0x1C, 0x10};
code unsigned char code_dot_1[7] = {0x1F, 0x13, 0x13, 0x1F, 0x13, 0x13, 0x1F};
code unsigned char code_dot_2[7] = {0x1F, 0x1F, 0x13, 0x13, 0x13, 0x1F, 0x1F};
code unsigned char code_dot_3[7] = {0x1F, 0x1F, 0x1F, 0x13, 0x1F, 0x1F, 0x1F};
code unsigned char code_num[][7] = {{0x0, 0x4, 0x4, 0x4, 0x4, 0x4, 0x0}, {0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19}, {0x10, 0x1C, 0x1C, 0x0, 0x7, 0x7, 0x0},
		  {0x0, 0x1C, 0x1C, 0x10, 0x1C, 0x1C, 0x0}, {0x4, 0x4, 0x4, 0x0, 0x1C, 0x1C, 0x1C}, {0x1, 0x7, 0x7, 0x0, 0x1C, 0x1C, 0x0}, {0x1, 0x7, 0x7, 0x0, 0x4, 0x4, 0x0}, {0x0, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C},
		  {0x0, 0x4, 0x4, 0x0, 0x4, 0x4, 0x0}, {0x0, 0x4, 0x4, 0x0, 0x1C, 0x1C, 0x10}};   

code unsigned char code_H[7] = {0xE, 0xE, 0xE, 0x0, 0xE, 0xE, 0xE};
code unsigned char code_M[7] = {0xE, 0x4, 0xA, 0xA, 0xE, 0xE, 0xE};
code unsigned char code_S[7] = {0x10, 0xF, 0xF, 0x11, 0x1E, 0x1E, 0x1};
code unsigned char code_Y[7] = {0xE, 0xE, 0x15, 0x1B, 0x1B, 0x1B, 0x1B};
code unsigned char code_D[7] = {0x3, 0xD, 0xE, 0xE, 0xE, 0xD, 0x3};
code unsigned char code_colon[7] = {0x1F, 0x1F, 0x1F, 0x1B, 0x1F, 0x1B, 0x1F};
code unsigned char code_small_dot[7] = {0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x13, 0x13};
code unsigned char code_celsius[7]	= {0x7, 0x7, 0x18, 0x17, 0x17, 0x17, 0x18};
code unsigned char code_P[7] = {0x1, 0xE, 0xE, 0x1, 0xF, 0xF, 0xF};
code unsigned char code_B[7] = {0x1, 0xE, 0xE, 0x0, 0xE, 0xE, 0x1};
code unsigned char code_O[7] = {0x11, 0xE, 0xE, 0xE, 0xE, 0xE, 0x11};
code unsigned char code_E[7] = {0x0, 0xF, 0xF, 0x0, 0xF, 0xF, 0x0}; 
code unsigned char code_T[7] = {0x0, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B};

code unsigned char code_oo[7] = {0x1F, 0x1F, 0x11, 0xE, 0xE, 0xE, 0x11};
code unsigned char code_nn[7] = {0x1F, 0x1F, 0x9, 0x6, 0xE, 0xE, 0xE};
code unsigned char code_yy[7] = {0x1F, 0x1F, 0xE, 0x15, 0x1B, 0x1B, 0x1B};
code unsigned char code_dash[7] = {0x1F, 0x1F, 0x1F, 0x11, 0x1F, 0x1F, 0x1F};

code unsigned char code_uu[7] = {0x1F, 0x1F, 0xE, 0xE, 0xE, 0xC, 0x12};
code unsigned char code_ee[7] = {0x1F, 0x1F, 0x11, 0xE, 0x0, 0xF, 0x11};
code unsigned char code_ss[7] = {0x1F, 0x1F, 0x10, 0xF, 0x11, 0x1E, 0x1};
code unsigned char code_W[7] = {0xE, 0xE, 0xE, 0xA, 0xA, 0x4, 0xE};
code unsigned char code_dd[7] = {0x1E, 0x1E, 0x12, 0xC, 0xE, 0xE, 0x10};
code unsigned char code_hh[7] = {0xF, 0xF, 0x9, 0x6, 0xE, 0xE, 0xE};
code unsigned char code_rr[7] = {0x1F, 0x1F, 0xC, 0xB, 0x7, 0xF, 0xF};
code unsigned char code_F[7] = {0x0, 0xF, 0xF, 0x0, 0xF, 0xF, 0xF};
code unsigned char code_ii[7] = {0x1F, 0x1F, 0x1B, 0x1F, 0x1B, 0x1B, 0x1B};
code unsigned char code_tt[7] = {0x1F, 0x1F, 0x1F, 0x1B, 0x0, 0x1B, 0x18};
  
   

void Timer0Init(void)
{
	AUXR |= 0x7F;   //12T
	TMOD &= 0x01;
	TL0 = 0xFF;
	TH0 = 0xFF;
	TF0 = 0;	
	TR0 = 1;
	ET0 = 1;
}

void IntInit(void)
{
	IT1 = 1;
	EX1 = 1;
	IT0 = 1;
	EX0 = 1;
}

void DynamicDisplay6(unsigned char *a, unsigned char *b, unsigned char *c, unsigned char *d, unsigned char *e, unsigned char *f)
{
	unsigned char i,j;
	for (i=1; i<=32; i++)
	{
		for (j=0; j<7; j++)
		{
			 line_data[j] = line_data[j] << 1;
			 if (i <= 5) line_data[j] |= (unsigned long)a[j] >> (5-i) & 1;
			 else if (i <= 10) line_data[j] |= (unsigned long)b[j] >> (10-i) & 1;
			 else if (i <= 15) line_data[j] |= (unsigned long)c[j] >> (15-i) & 1;
			 else if (i <= 20) line_data[j] |= (unsigned long)d[j] >> (20-i) & 1;
			 else if (i <= 25) line_data[j] |= (unsigned long)e[j] >> (25-i) & 1;
			 else if (i <= 30) line_data[j] |= (unsigned long)f[j] >> (30-i) & 1;
			 Delay1ms();
		}
	}
}

void Display6(unsigned char *a, unsigned char *b, unsigned char *c, unsigned char *d, unsigned char *e, unsigned char *f, unsigned char dynamic)
{
	if (!dynamic)
	{
		unsigned char i;
		for (i=0; i<7; i++) 
			line_data[i] = ((unsigned long)a[i] << 27) | ((unsigned long)b[i] << 22) | ((unsigned long)c[i] << 17) | ((unsigned long)d[i] << 12) | ((unsigned long)e[i] << 7) | ((unsigned long)f[i] << 2);	
	} else {
		DynamicDisplay6(a,b,c,d,e,f);
	}
}

/*
void DisplayPos(unsigned char *a, unsigned char pos)
{
	unsigned char i;
	for (i=0; i<7; i++)
	{
		line_data[i] &= ~(unsigned long)0x1F << ((6-pos) * 5 + 2);
		line_data[i] |= (unsigned long)a[i] << ((6-pos) * 5 + 2);	
	}
}
*/

void ADCInit()
{
	P1ASF = 0x80;
	AUXR1 = 0x04;
	ADC_CONTR = 0xEF;

	_nop_(); _nop_(); _nop_(); _nop_(); 
	PADC = 1;
	EADC = 1;	
}

unsigned char getMaxDay(unsigned int YYYY, unsigned char MON)
{
	switch (MON)
    {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:return 31;
    case 4:
    case 6:
    case 9:
    case 11:return 30;
    case 2:             
        if (YYYY%400==0 || (YYYY%100!=0 && YYYY%4==0))
            return 29;
		else
			return 28; 
    default:    return 0;
    }
}

unsigned char getWeek(unsigned int YYYY, unsigned char MON, unsigned char DAY)
{
	unsigned char y,m;
	y = ((YYYY % 100) / 4 + (YYYY % 100)) % 7;
	switch (MON)
	{
		case 2:
		case 3:
		case 11: m = 2; break;
		case 5: m = 0; break;
		case 8: m = 1; break;
		case 6: m = 3; break;
		case 9:
		case 12: m = 4; break;
		case 4:
		case 7: m = 5; break;
		case 1:
		case 10: m = 6; break;
	}
	if (YYYY % 400 == 0 || (YYYY%100!=0 && YYYY%4==0))
	{
		if (MON == 2) m = 1;
		if (MON == 1) m = 5;
	}
	return ((y + m + DAY) % 7);
}

void main()
{	
	unsigned char i,HH,MM,SS,MON,DAY,m1 = 0,m2 = 0,mode,dynamic = 0,is24 = 0, enableBEEP = 1,WEEK;
	unsigned int YYYY,w,ww = 0;


	IntInit();
	Timer0Init();
	DS12887_Init();
	ADCInit();

	WDT_CONTR = 0x3E;

	PX0 = 0;
	PX1 = 0;
	PT0 = 1;

	P0 = 0xFF;
	BEEP = 1;

	is24 = IapReadByte(0);
	enableBEEP = IapReadByte(1);

	for (i=0; i<30; i++)
	{
		HC595_DATA = 1;
		HC595_SHCP = 0;
		HC595_SHCP = 1;	
	}

	HC595_STCP = 0;
	HC595_STCP = 1;
	
	
	EA = 1;


	while (1)
	{
		//Display6(code_num[ADCResult / 10000], code_num[ADCResult % 10000 / 1000], code_num[ADCResult % 1000 /100], code_num[ADCResult % 100 /10], code_num[ADCResult % 10], code_small_dot, 0);
		//Delayx10ms(255); 
		

		WDT_CONTR = 0x3E;
		if (menu2 == 0 &&  menu1 == 0) ADC_CONTR = 0xEF; else { brightness = 2; }
		HH = DS12887_Read(DS_HOR) & 0x7F;
		HH = (HH >> 4) * 10 + (HH & 0x0F);
		MM = DS12887_Read(DS_MIN);


		if (menu1 == 0 && menu2 == 0)
		{	
			if (!is24)
			{
				if (HH >= 12)
				{
					if (HH > 12) HH -= 12;
					Display6(code_pp, code_num[HH / 10], code_num[HH % 10], code_dot_1, code_num[MM >> 4], code_num[MM & 0x0F],0);
					if (brightness == 0) Delayx10ms(2); else Delayx10ms(5);
					Display6(code_pp, code_num[HH / 10], code_num[HH % 10], code_dot_2, code_num[MM >> 4], code_num[MM & 0x0F],0);
					if (brightness == 0) Delayx10ms(2); else Delayx10ms(5);
					Display6(code_pp, code_num[HH / 10], code_num[HH % 10], code_dot_3, code_num[MM >> 4], code_num[MM & 0x0F],0);
					if (brightness == 0) Delayx10ms(2); else Delayx10ms(5);
					Display6(code_pp, code_num[HH / 10], code_num[HH % 10], code_dot_2, code_num[MM >> 4], code_num[MM & 0x0F],0);
					if (brightness == 0) Delayx10ms(2); else Delayx10ms(5);
					Display6(code_pp, code_num[HH / 10], code_num[HH % 10], code_dot_1, code_num[MM >> 4], code_num[MM & 0x0F],0);
			   	
				} else {
					Display6(code_aa, code_num[HH / 10], code_num[HH % 10], code_dot_1, code_num[MM >> 4], code_num[MM & 0x0F],0);
					if (brightness == 0) Delayx10ms(2); else Delayx10ms(5);
					Display6(code_aa, code_num[HH / 10], code_num[HH % 10], code_dot_2, code_num[MM >> 4], code_num[MM & 0x0F],0);
					if (brightness == 0) Delayx10ms(2); else Delayx10ms(5);
					Display6(code_aa, code_num[HH / 10], code_num[HH % 10], code_dot_3, code_num[MM >> 4], code_num[MM & 0x0F],0);
					if (brightness == 0) Delayx10ms(2); else Delayx10ms(5);
					Display6(code_aa, code_num[HH / 10], code_num[HH % 10], code_dot_2, code_num[MM >> 4], code_num[MM & 0x0F],0);
					if (brightness == 0) Delayx10ms(2); else Delayx10ms(5);
					Display6(code_aa, code_num[HH / 10], code_num[HH % 10], code_dot_1, code_num[MM >> 4], code_num[MM & 0x0F],0);
				}
			} else {
				Display6(code_dark, code_num[HH / 10], code_num[HH % 10], code_dot_1, code_num[MM >> 4], code_num[MM & 0x0F],0);
				if (brightness == 0) Delayx10ms(2); else Delayx10ms(5);
				Display6(code_dark, code_num[HH / 10], code_num[HH % 10], code_dot_2, code_num[MM >> 4], code_num[MM & 0x0F],0);
				if (brightness == 0) Delayx10ms(2); else Delayx10ms(5);
				Display6(code_dark, code_num[HH / 10], code_num[HH % 10], code_dot_3, code_num[MM >> 4], code_num[MM & 0x0F],0);
				if (brightness == 0) Delayx10ms(2); else Delayx10ms(5);
				Display6(code_dark, code_num[HH / 10], code_num[HH % 10], code_dot_2, code_num[MM >> 4], code_num[MM & 0x0F],0);
				if (brightness == 0) Delayx10ms(2); else Delayx10ms(5);
				Display6(code_dark, code_num[HH / 10], code_num[HH % 10], code_dot_1, code_num[MM >> 4], code_num[MM & 0x0F],0);	
			}

		} 
		else if (menu1)
		{	
			if (menu1 != m1)
			{
				if (enableBEEP) BEEP = 0;
				Delayx10ms(10);
				m1 = menu1;
				mode++; dynamic = 1;
				BEEP = 1;
			} else {
				Delayx10ms(5);
			}

			//设置星期
			YYYY = BCD_DEC(DS12887_Read(DS_Y_S)) * 100 + BCD_DEC(DS12887_Read(DS_YER));
			MON = BCD_DEC(DS12887_Read(DS_MON));
			DAY = BCD_DEC(DS12887_Read(DS_DAY));

			WEEK = getWeek(YYYY, MON, DAY);
			DS12887_Write(DS_WEK, DEC_BCD(WEEK));

			//首先设置时间：
			if (mode == 1) //小时
			{
				HH = BCD_DEC(DS12887_Read(DS_HOR));
				Display6(code_H, code_H, code_colon, code_dark, code_num[HH / 10], code_num[HH % 10], dynamic);
				dynamic = 0;
				Delay10ms();
				if (menu2 != m2)
				{
					if (enableBEEP) BEEP = 0;
				    Delayx10ms(7);
					m2 = menu2;
					if (++HH > 23) HH = 0;
					DS12887_Write(DS_HOR, DEC_BCD(HH));
					BEEP = 1;	
				}	
			}
			if (mode == 2) //分钟
			{
				MM = BCD_DEC(DS12887_Read(DS_MIN));

				Display6(code_M, code_M, code_colon, code_dark, code_num[MM / 10], code_num[MM % 10], dynamic);
				dynamic = 0;
				Delay10ms();
				if (menu2 != m2)
				{
					if (enableBEEP) BEEP = 0;
					Delayx10ms(7);
					m2 = menu2;
					if (++MM > 59) MM = 0;
					DS12887_Write(DS_MIN, DEC_BCD(MM));	
					BEEP = 1;
						
				}
			}
			if (mode == 3) //秒
			{
				SS = BCD_DEC(DS12887_Read(DS_SEC));

				Display6(code_S, code_S, code_colon, code_dark, code_num[SS / 10], code_num[SS % 10], dynamic);
				dynamic = 0;
				Delay10ms();
				if (menu2 != m2)
				{
					if (enableBEEP) BEEP = 0;
					Delayx10ms(7);
					m2 = menu2;	
					if (SS > 0) SS--;
					DS12887_Write(DS_SEC, DEC_BCD(SS));
					BEEP = 1;	
				}	
			}

			if (mode == 4) //年
			{
				Display6(code_Y, code_colon, code_num[YYYY / 1000], code_num[YYYY / 100 % 10], code_num[YYYY / 10 % 10], code_num[YYYY % 10], dynamic);
				dynamic = 0;
				Delay10ms();
				if (menu2 != m2)
				{
					if (enableBEEP) BEEP = 0;
					Delayx10ms(7);
					m2 = menu2;	
					if (++YYYY > 2100) YYYY = 2013;
					if (DAY > getMaxDay(YYYY, MON)) DAY = 1;
					DS12887_Write(DS_Y_S, DEC_BCD((YYYY / 100)));
					DS12887_Write(DS_YER, DEC_BCD((YYYY % 100)));
					DS12887_Write(DS_DAY, DEC_BCD(DAY));
					BEEP = 1;	
				}	
			}

			if (mode == 5) //月
			{

				Display6(code_M, code_oo, code_nn, code_colon, code_num[MON / 10], code_num[MON % 10], dynamic);
				dynamic = 0;
				Delay10ms();
				if (menu2 != m2)
				{
					if (enableBEEP) BEEP = 0;
				    Delayx10ms(7);
					m2 = menu2;	
					if (++MON > 12) MON = 1;
					if (DAY > getMaxDay(YYYY, MON)) DAY = 1;
					DS12887_Write(DS_MON, DEC_BCD(MON));
					DS12887_Write(DS_DAY, DEC_BCD(DAY));
					BEEP = 1;	
				}	
			}

			if (mode == 6) //日
			{
				Display6(code_D, code_aa , code_yy, code_colon, code_num[DAY / 10], code_num[DAY % 10], dynamic);
				dynamic = 0;
				Delay10ms();
				if (menu2 != m2)
				{
					if (enableBEEP) BEEP = 0;
					Delayx10ms(7);
					m2 = menu2;	
					if (++DAY > getMaxDay(YYYY, MON)) DAY = 1;
					DS12887_Write(DS_DAY, DEC_BCD(DAY));
					BEEP = 1;	
				}	
			}

			if (mode == 7) //12/24
			{
				if (is24)
				{
					Display6(code_dark, code_dark, code_dark, code_num[2], code_num[4], code_H, dynamic);
				} else {
					Display6(code_dark, code_dark, code_dark, code_num[1], code_num[2], code_H, dynamic);
				}
				dynamic = 0;
				Delay10ms();
				if (menu2 != m2)
				{
					if (enableBEEP) BEEP = 0;
					Delayx10ms(7);
					m2 = menu2;	
					if(is24) is24 = 0; else is24 = 1;
					IapEraseSector(0);
					IapProgramByte(0, is24);
					IapProgramByte(1, enableBEEP);					
					BEEP = 1;	
				}	
			}

			if (mode == 8) //Enable Beep
			{
				if (enableBEEP)
				{
					Display6(code_B, code_E, code_E, code_P, code_colon, code_num[1], dynamic);
				} else {
					Display6(code_B, code_E, code_E, code_P, code_colon, code_num[0], dynamic);
				}
				dynamic = 0;
				Delay10ms();
				if (menu2 != m2)
				{
					if (enableBEEP) BEEP = 0;
					Delayx10ms(7);
					m2 = menu2;	
					if(enableBEEP) enableBEEP = 0; else enableBEEP = 1;
					IapEraseSector(0);
					IapProgramByte(0, is24);
					IapProgramByte(1, enableBEEP);					
					BEEP = 1;	
				}	
			}
			

			if (mode > 8) 
			{
				menu1 = 0;
				menu2 = 0;
				m1 = 0;
				m2 = 0;
				mode = 0;
			}


						
		} else if (menu2)
		{
			

			if (menu2 != m2)
			{
				if (enableBEEP) BEEP = 0;
				Delayx10ms(10);
				m2 = menu2;
				mode++; dynamic = 1;
				BEEP = 1;
			};


			YYYY = BCD_DEC(DS12887_Read(DS_Y_S)) * 100 + BCD_DEC(DS12887_Read(DS_YER));
			MON = BCD_DEC(DS12887_Read(DS_MON));
			DAY = BCD_DEC(DS12887_Read(DS_DAY));
			WEEK = BCD_DEC(DS12887_Read(DS_WEK));

			if (mode == 1)
			{
				//温度
				
				w = DS18B20_Read();
				if (!ww || w < ww || (w % 1000 - ww % 1000 <= 20) || ww % 1000 > 400) ww = w;

				Display6(code_dark, code_num[(ww % 1000) / 100], code_num[(ww % 100) / 10], code_small_dot, code_num[ww % 10], code_celsius, dynamic);
				dynamic = 0;
				Delayx10ms(10);
			}

			if (mode == 2)
			{
				//WEEK
				switch(WEEK)
				{
					case 1:Display6(code_dark, code_dark, code_M, code_oo, code_nn, code_small_dot, dynamic); break;	
					case 2:Display6(code_dark, code_T, code_uu, code_ee, code_ss, code_small_dot, dynamic); break;	
					case 3:Display6(code_dark, code_dark, code_W, code_ee, code_dd, code_small_dot, dynamic); break;	
					case 4:Display6(code_dark, code_T, code_hh, code_uu, code_rr, code_small_dot, dynamic); break;	
					case 5:Display6(code_dark, code_dark, code_F, code_rr, code_ii, code_small_dot, dynamic); break;	
					case 6:Display6(code_dark, code_dark, code_S, code_aa, code_tt, code_small_dot, dynamic); break;	
					case 0:Display6(code_dark, code_dark, code_S, code_uu, code_nn, code_small_dot, dynamic); break;	
				}
				
				dynamic = 0;
				Delayx10ms(4);
			}

			if (mode == 3)
			{
				//月日
				Display6(code_dark, code_num[MON / 10], code_num[MON % 10], code_dash, code_num[DAY / 10], code_num[DAY % 10], dynamic);
				dynamic = 0;
				Delayx10ms(4);

			}

			if (mode == 4)
			{
				//年
				Display6(code_dark, code_dark, code_num[YYYY / 1000], code_num[YYYY / 100 % 10], code_num[YYYY / 10 % 10], code_num[YYYY % 10], dynamic);
				dynamic = 0;
				Delayx10ms(4);
			}



			if (mode > 4) 
			{
				menu1 = 0;
				menu2 = 0;
				m1 = 0;
				m2 = 0;
				mode = 0;
				ww = 0;
			}
			
		}  


		for (i=0; i<=25; i++)
		{
			if (brightness == 0) Delayx10ms(1); else Delayx10ms(2);
			WDT_CONTR = 0x3E;
			if (menu1 || menu2) break;
		}
			
	}
}

void int0_interrupt (void) interrupt 0
{
	busy = 1;
	if (++menu1 > 200) menu1 = 1;
	busy = 0;
	
}

void int1_interrupt (void) interrupt 2
{
    busy = 1;
	if (++menu2 > 200) menu2 = 1;
	busy = 0;	
}

void time0_interrupt (void) interrupt 1 
{

	unsigned char i;
	P0 = 0xFF;

	for (i=0; i<30; i++)
	{
		HC595_DATA = ((line_data[line-1] << i) & 0x80000000) == 0x80000000;	
		HC595_SHCP = 0;
		HC595_SHCP = 1;	
	}
   
	HC595_STCP = 0;
	HC595_STCP = 1;

    switch(line)
	{
		case 1:H1=0; break;
		case 2:H2=0; break;
		case 3:H3=0; break;
		case 4:H4=0; break;
		case 5:H5=0; break;
		case 6:H6=0; break;
		case 7:H7=0; break;
	}
	if (++line == 8) line = 1;

	if (brightness == 0)
	{
		TL0 = 0xD7;	
		TH0 = 0xFD;
	} else if (brightness == 1)
	{
		TL0 = 0x33;	
		TH0 = 0xE3;
	} else// if (brightness == 2)
	{
		TL0 = 0x00;	
		TH0 = 0xB8;
	}

}

void ADC_interrupt (void) interrupt 5
{
    unsigned int ADCResult;
	ADC_CONTR = 0x0;	
	_nop_(); _nop_();_nop_();_nop_();_nop_();_nop_();
    ADCResult = ADC_RES * 256 + ADC_RESL;
    if (ADCResult > 950) brightness = 0;
	else if (ADCResult > 800) brightness = 1; else brightness = 2;

	//while(1);	

}



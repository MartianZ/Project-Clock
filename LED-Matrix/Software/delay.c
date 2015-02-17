#include <INTRINS.h>
#include "delay.h"

void Delay1ms()		//@22.1184MHz
{
	unsigned char i, j;

	_nop_();
	_nop_();
	i = 22;
	j = 128;
	do
	{
		while (--j);
	} while (--i);
}

void Delay10ms()		//@22.1184MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 1;
	j = 216;
	k = 35;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}


void Delayx10ms(unsigned char j)
{
	unsigned char i;
	for (i=1; i<=j; i++) Delay10ms();
}

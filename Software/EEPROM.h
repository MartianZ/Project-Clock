#ifndef __EEPROM_H__
#define __EEPROM_H__



#define CMD_IDLE 0
#define CMD_READ 1
#define CMD_PROGRAM 2
#define CMD_ERASE 3

#define ENABLE_IAP 0x81

#define IAP_ADDRESS 0x0000

void IapIdle()
{
	IAP_CONTR = 0;
	IAP_CMD = 0;
	IAP_TRIG = 0;
	IAP_ADDRH = 0x80;
	IAP_ADDRL = 0;
}

unsigned char IapReadByte(unsigned int addr)
{
	unsigned char dat;

	IAP_CONTR = ENABLE_IAP;
	IAP_CMD = CMD_READ;
	IAP_ADDRL = addr;
	IAP_ADDRH = addr >> 8;
	IAP_TRIG = 0x5a;
	IAP_TRIG = 0xa5;
	_nop_();

	dat = IAP_DATA;
	IapIdle();
	return dat;
}

void IapProgramByte(unsigned int addr, unsigned char dat)
{
	IAP_CONTR = ENABLE_IAP;
	IAP_CMD = CMD_PROGRAM;
	IAP_ADDRL = addr;
	IAP_ADDRH = addr >> 8;
	IAP_DATA = dat;
	IAP_TRIG = 0x5a;
	IAP_TRIG = 0xa5;
	_nop_();
	IapIdle();
}

void IapEraseSector(unsigned int addr)
{
	IAP_CONTR = ENABLE_IAP;
	IAP_CMD = CMD_ERASE;
	IAP_ADDRL = addr;
	IAP_ADDRH = addr >> 8;
	IAP_TRIG = 0x5a;
	IAP_TRIG = 0xa5;
	_nop_();
	IapIdle();
}

#endif
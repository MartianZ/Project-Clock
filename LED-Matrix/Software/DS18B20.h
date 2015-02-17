#ifndef __DS18B20_H__
#define __DS18B20_H__

typedef unsigned char BYTE;

sbit DQ = P0 ^ 7;

void DelayXus(BYTE n); 
void DS18B20_Reset(); 
void DS18B20_WriteByte(BYTE dat); 
BYTE DS18B20_ReadByte();
unsigned int DS18B20_Read();


void DelayXus(BYTE n) 
{ 
    while (n--) 
    {
		_nop_();
		_nop_();		
    } 
} 


void DS18B20_Reset() 
{ 
    CY = 1; 
    while (CY) 
    { 
        DQ = 0;                     
        DelayXus(240); 
        DelayXus(240); 
        DQ = 1; 
        DelayXus(60);
        CY = DQ;
        DelayXus(240);
        DelayXus(180); 
    } 
} 
 
BYTE DS18B20_ReadByte() 
{ 
    BYTE i; 
    BYTE dat = 0; 

    for (i=0; i<8; i++)
    { 
        dat >>= 1; 
        DQ = 0;
        DelayXus(1);
        DQ = 1;  
        DelayXus(1);
        if (DQ) dat |= 0x80;
        DelayXus(60);
    } 

    return dat; 
} 

void DS18B20_WriteByte(BYTE dat) 
{ 
    char i; 

    for (i=0; i<8; i++)
    { 
        DQ = 0; 
        DelayXus(1);  
        dat >>= 1;
        DQ = CY; 
        DelayXus(60); 
        DQ = 1;   
        DelayXus(1);        
    } 
}

unsigned int DS18B20_Read()
{
	unsigned int T;
	BYTE TPH;
	BYTE TPL; 
	DS18B20_Reset();     
    DS18B20_WriteByte(0xCC);  
    DS18B20_WriteByte(0x44);  
    while (!DQ);              

    DS18B20_Reset();          
    DS18B20_WriteByte(0xCC);  
    DS18B20_WriteByte(0xBE);  
    TPL = DS18B20_ReadByte(); 
    TPH = DS18B20_ReadByte(); 
	T = TPL + TPH * 256;    
   	if(T==0xFFFF) return 0xFFFF;
	if(T>0x8000)
	{
  		T=~T+1;
		return(0x8000+T*5/8);
	}
	else
	{
   		return(T*5/8);
	}
	
}
 


	
#endif
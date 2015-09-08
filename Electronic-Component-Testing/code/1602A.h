#include<intrins.h>
#include<reg52.h>
#define uchar unsigned char
#define uint unsigned int
//16024A
#define lcd P0
sbit lcdrs = P2^6;
sbit lcdrw = P2^5;
sbit lcde = P2^7;
void DelayLCD(uint t)
{
	do
	{
		_nop_();
	} while(t--);
}
 void LCDcom(uchar com)
{
	lcdrs = 0;
	lcd = com;
	DelayLCD(5);  //最好有些延时
	lcde = 1;
	DelayLCD(10);
	lcde = 0;	
}
void LCDdat(uchar dat)
{
	lcdrs = 1;
	lcd = dat;
	DelayLCD(5);
	lcde = 1;
	DelayLCD(10);
	lcde = 0;	
}

//LCD 1602A 
//init
/*	lcde = 0;
	lcdrw = 0; // 默认写
	LCDcom(0x38);
	LCDcom(0x0f);
	LCDcom(0x06);
	LCDcom(0x01);//延时	*/
#include<intrins.h>
#include<reg52.h>
#include<stdio.h>
#define uchar unsigned char
#define uint unsigned int
// AD7895
sbit convst = P1^6;
sbit busy = P1^7;
sbit sdata = P3^0;
sbit sclk = P3^1;

// AD7895
void initAD7895()
{
	convst = 1;
	busy = 0;
	sclk = 0;
}

void convertAD7895()   //Mode 1 : high sampling performance
{	int i = 1000;
	convst = 0;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	convst = 1;
	while(busy&&--i);//busy remain 3.8us
	_nop_();
	_nop_();//
	_nop_(); //
}
short readAD7895()
{
	short dat = 0;
	uchar i;
	for(i = 0; i< 16; i++)
	{
		dat = dat << 1;
		sclk = 1;
		_nop_();  //
		_nop_();
		sclk = 0;
		_nop_();	//
		_nop_();
		dat = dat | sdata; 
	}
	dat = dat >> 1;
	_nop_();
	return dat;
}
float getvoltAD7895(short dat)
{
	float volt = 0.0;
	if(!(dat & 0xf000))//dat is accurate only when first 4 bits are zero
	{
		if(dat & 0x0800)
			volt = -10.0 + 0.004883 * (dat & 0x07ff);
		else
			volt = 0.0 + 0.004883 * (dat & 0x07ff);
		return volt;
	}
	return 99.99;	
}
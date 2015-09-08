#include<intrins.h>
#include<reg52.h>
#include<math.h>
#define uchar unsigned char
#define uint unsigned int
sbit io1 = P3^6;
sbit io2 = P3^5;
sbit io3 = P2^7;
void delay74()
{
	char ch = 50;
	while(ch--);
}
void init74()
{
	io1 = 0;
	io2 = 0;
	io3 = 0;
	delay74();
}
int judge74()
{
	io1 = 0;
	if(io2 == 1)
	{
		//74hc04
		return 4;
	}
	else if(io3 == 1)
	{
		//74hc00
		return 0;
	}
	io1 = 1;
	delay74();
	if(io3 == 1)
	{
		io2 = 1;
		delay74();
		if(io3 == 1)
		{
			//74hc32
			return 2;
		}
		else
		{
			//74hc86
			return 6;
		}
		
	}
	io2 = 1;
	delay74();
	if(io3 == 1)
	{
		//74hc08
		return 8;
	}
	else
	{
		//74hc245
		return 5;
	}
}
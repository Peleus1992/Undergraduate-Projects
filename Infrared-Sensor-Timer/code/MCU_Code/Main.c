#include <reg52.h>
#include <intrins.h>
#include <stdlib.h>
//unsigned char code   dula[] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x80}; 
sbit a = P2^7;
sbit b = P2^6;
unsigned int  numBit[4], iCount;
unsigned int num1 = 0;
unsigned char ch;
unsigned int sig = 0;

void delay(unsigned int n)
{
	unsigned int a, b;
	for(a = n; a > 0; a--)
		for(b = n; b > 0; b--);
}
void init()
{
//interrupt enable
	EA = 1;
	ET0 = 1;
	ES = 1;

//timer 0 for counting, timer 1 for Serial Port	
	TMOD = 0x21;
	TH0 = (65535 - 10000) / 256;
	TL0 = (65535 - 10000) % 256;
	TH1 = 0xfd;
	TL1 = 0xfd;
	TR1 = 1;
//	TR0 = 1;

//interrupt0
	EX0 = 1;
	IT0 = 1;

//reset control	
	a = 1;
	b = 0;

//SCON
	SM0 = 0; 
	SM1 = 1;
	REN = 1;
}
 
void main()
{
	init();	
	while(1)
	{	
		if(sig)
		{
			sig++;
			if(sig == 200)
			{
				sig = 0;
			}
		} 
		if(num1 > 9999)
		{
			num1 = 0;
	//		return ;
		}
		if(a == b || ch == 'r')
		{
			num1 = 0;
			TR0 = 0;	
			delay(2);
			SBUF = 2;
			ch = 0;
		}
		if(ch == 's')
		{
			TR0 = ~TR0;
			ch = 0;
		}
		numBit[0] = num1 / 1000;
		numBit[1] = (num1 % 1000) / 100;
		numBit[2] = (num1 % 100) / 10;
		numBit[3] = (num1 % 10);
		
		P0 = ((numBit[0] << 4)| numBit[1]) & 0x00ff;
		
		P1 = ((numBit[2] << 4)| numBit[3]) & 0x00ff;
							
	}
}

void AnswerTIME0() interrupt 1// using 1
{
	TR0 = 0;
	TH0 = (65535 - 9216) / 256;
	TL0 = (65535 - 9216) % 256;
	 num1++;
	 SBUF = 1;

//		delay(1);

	TR0 = 1;
}
 void AnswerINT0() interrupt 0// using 1
{
	EX0 = 0;
	if(P3^2 == 0)
	{	
		delay(3);
		if(P3^2 == 0 && sig == 0)
		{
				sig = 1;
				TR0 = ~TR0;
		}
	}
	EX0 = 1;
}
void ser() interrupt 4 
{
	if(RI == 1)
	{	
		RI = 0;
		ch = SBUF;
	}
} 

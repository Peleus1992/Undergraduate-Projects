#include<intrins.h>
#include<reg52.h>
#include<math.h>
#define uchar unsigned char
#define uint unsigned int
sbit ud10 = P1^0;
sbit inc10 = P1^1;
sbit cs10 = P1^2;
sbit ud2 = P3^2;
sbit inc2 = P3^3;
sbit cs2 = P3^4;
#define DOWN10 0
#define UP10 1
#define MAX_10 9999        //////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MIN_10 30
#define STEP_10 330        //////////////////////////////////////////////////////////////////////////////////////////////////////////
int curVal_10;
// VCC = 5V; GND(VSS) = 0V; Rh , Rw as two sides of resistence.
void delay_10(int t)
{
	int a, b;
	for(a = t; a > 0; a --)
		for(b = t; b > 0; b--);
}
void Changex9313_10(uchar offset, uchar flag, uchar store)
{
	uchar i;
	cs10 = 1;
	cs2 = 1;
	inc10 = 1;
	inc2 = 1;
	if(flag == UP10)
	{
		ud10 = 0;
		ud2 = 0;
		curVal_10 += i * STEP_10;
	}
	else
	{
		ud10 = 1;
		ud2 = 1;
		curVal_10 -= i * STEP_10;
	}
	cs10 = 0;
	cs2 = 0;
	for(i = offset; i > 0; i--)
	{
		_nop_();
		_nop_();
		inc10 = 1;
		inc2 = 1;
		_nop_();
		_nop_();
		inc10 = 0;
		inc2 = 0;
	}
	if(store)
	{
		_nop_();
		_nop_();
		inc10 = 1;
		inc2 = 1;
	}
	_nop_();
	_nop_();
	cs10 = 1;
	cs2 = 1;
	if(store)
		delay_10(50);	 //45
}

int Setx9313_10(int r)
{
	int i = 0;
	if(r > curVal_10)
	{
		while(r>curVal_10)
		{
			curVal_10 += STEP_10;
			i++;
		}
		if(curVal_10-r > r-curVal_10+STEP_10)
		{
			curVal_10 -= STEP_10;
			i--;
		}
		if(i)
			Changex9313_10(i, UP10, 10);	
	}
	else if(r < curVal_10)
	{
		while(r<curVal_10)
		{
			curVal_10 -= STEP_10;
			i++;
		}
		if(r-curVal_10 > curVal_10-r+STEP_10)
		{
			curVal_10 += STEP_10;
			i--;
		}
		if(i)
			Changex9313_10(i, DOWN10, 10);
	}
	return curVal_10;
}
void initx9313_10()
{
	Changex9313_10(32, UP10, 10);
//	Changex9313_10(32, UP10, 10);
	curVal_10 = MAX_10;
//	Setx9313_10(MAX_10);
}





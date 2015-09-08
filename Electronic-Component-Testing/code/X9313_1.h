#include<intrins.h>
#include<reg52.h>
#include<math.h>
#define uchar unsigned char
#define uint unsigned int
sbit ud1 = P1^3;
sbit inc1 = P1^4;
sbit cs1 = P1^5;
#define DOWN1 0
#define UP1 1
#define MAX_1 998        //////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MIN_1 26        //////////////////////////////////////////////////////////////////////////////////////////////////////////
#define STEP_1 31        //////////////////////////////////////////////////////////////////////////////////////////////////////////
int curVal_1;
// VCC = 5V; GND(VSS) = 0V; Rh , Rw as two sides of resistence.
void delay_1(int t)
{
	int a, b;
	for(a = t; a > 0; a --)
		for(b = t; b > 0; b--);
}

void Changex9313_1(uchar offset, uchar flag, uchar store)
{
	uchar i;
	cs1 = 1;
	inc1 = 1;
	if(flag == UP1)
	{
		ud1 = 0;
		curVal_1 += STEP_1*i;
	}
	else
	{
		ud1 = 1;
		curVal_1 -= STEP_1*i;
	}
	cs1 = 0;
	for(i = offset; i > 0; i--)
	{
		_nop_();
		_nop_();
		inc1 = 1;
		_nop_();
		_nop_();
		inc1 = 0;
	}
	if(store)
	{
		_nop_();
		_nop_();
		inc1 = 1;
	}
	_nop_();
	_nop_();
	cs1 = 1;
	if(store)
		delay_1(50);	 //45
}
void initx9313_1()
{
	Changex9313_1(32, DOWN1, 1);
	curVal_1 = MIN_1;
}
int Setx9313_1(int r)
{
	int i = 0;
	if(r > curVal_1)
	{
		while(r>curVal_1)
		{
			curVal_1 += STEP_1;
			i++;
		}
		if(curVal_1-r > r-curVal_1+STEP_1)
		{
			curVal_1 -= STEP_1;
			i--;
		}
		if(i)
			Changex9313_1(i, UP1, 1);	
	}
	else if(r < curVal_1)
	{
		while(r<curVal_1)
		{
			curVal_1 -= STEP_1;
			i++;
		}
		if(r-curVal_1 > curVal_1-r+STEP_1)
		{
			curVal_1 += STEP_1;
			i--;
		}
		if(i)
			Changex9313_1(i, DOWN1, 1);
	}
	return curVal_1;
}



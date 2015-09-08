#include<intrins.h>
#include<reg52.h>
#include<stdio.h>
#include"74.h"
#include"x9313_10.h"
#include"x9313_1.h"
#include"AD7895.h"
#include"tftm.h"
#define uchar unsigned char
#define uint unsigned int
 
//digital
int code Rb[5] = {0, 0, 6130, 3539, 2492};//base risisitance
int code Rb2[5] = {0, 0, 4430, 2539, 1302};//base risisitance	
#define Vd  8      //dead Volt 
#define Vd2 9
#define SampleNum 10
#define Rc		  0


//key
sbit key = P2^6;
//relay
sbit relaycon = P3^7;
//tmpt
short s_volt;
float f_volt;

uchar isNPN = 1;
float curUce;//v
float curIc; //mA
int beta;

//main
void init()
{
//	init1602A();
	ILI9325_Initial();
	init74();
	initAD7895();
	initx9313_10();
	initx9313_1();
	relaycon = 1;
	key = 1;
	isNPN = 1;
}
void delaymain(int t)
{
	while(t--);
}

float getUce()
{
	convertAD7895();
	s_volt = readAD7895();
	f_volt = getvoltAD7895(s_volt);
	if(f_volt < 0)
		curUce = -f_volt;
	else
		curUce = f_volt;
	return curUce;
}
float getIc()
{
	if(isNPN)//////////////////////////////////////////////////////////////
		curIc = (9.28 - curUce) / (curVal_1 + Rc)*1000 * 0.67;
	else
		curIc = ((10.0 - curUce) / (curVal_1 + Rc)*1000 + 2.0)*0.90	;
	return curIc;
}
void judgeNPN()
{
	
	Setx9313_1(500); //Rc	800
	Setx9313_10(Rb[4]);
	if(getUce() > Vd)
	{
		isNPN = 0;
		relaycon = 0;
		delaymain(200);
		initx9313_10();
		Setx9313_10(Rb2[4]);
		if(getUce() > Vd2)
		{
			isNPN = 2;
			relaycon = 1;
			delaymain(200);
		}
	}	   
}
void showChart()
{
	int i;
	int b[5];
	b[3] = 0;b[4] = 9999;
	coordinate();
	initx9313_1();
	Setx9313_1(500); //Rc	800	/////////////////////////
	for(i = 2; i < 5; i++)	 
	{
		initx9313_10();
		if(isNPN)
			Setx9313_10(Rb[i]);	
		else
			Setx9313_10(Rb2[i]);
//		Changex9313_10((i-2)*2,DOWN10,1);
//		while(key1);//test///////////////////////////////////////////////////////////////////////////
//		while(!key1)
//		for(j = 0; j < SampleNum; j++)
//		{
//			Changex9313_1(1, UP1, 1);
			delaymain(200);
			getUce();
			getIc();
//		}
		if(curIc < 10)
			printcurve(curIc,i*10);//show Uce and Ic according to Ib//NPNor PNP///
		b[i-2] = curIc * 1000 / (i*10);
		if(b[i-2] > b[3])
			b[3] = b[i-2];
		if(b[i-2] < b[4])
			b[4] = b[i-2];
	}

//	beta = (b[0]+b[1]+b[2])/3;
//	P0 = beta;////////////////////////////////////////////////////////////////
	if(isNPN == 1)
		printtype('N', b[4], b[3]);
	else
		printtype('P', b[4], b[3]);
	//show beta//remember -//////////////////////////////////////////////////////////////////////////////////

} 
void main()
{
	
	while(1)
	{
		init();
		wait();//wait
		while(key);
		delaymain(30);
		while(!key);
		judgeNPN();
		if(isNPN != 2)
			showChart();
		else
			printchip(judge74());
		while(key);
		delaymain(30);
		while(!key);
		
	}
	while(1);
}	  



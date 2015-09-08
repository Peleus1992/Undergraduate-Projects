#include<reg52.h>
#include"9325tp.h"
#include<math.h>

/*-----------------------------------------------
                  全局变量
-----------------------------------------------*/
/* LCD color */
#define White          0xFFFF
#define Black          0x0000
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
sbit DATA=P2^0;		//语音芯片的数据口
sbit RST=P2^3;		//语音芯片的复位口
extern void ILI9325_Initial();
extern void coordinate();
extern void printcurve(float ic,int ib);
extern void printtype(char n,int bmin,int bmax);
extern void printchip(int bmin);
extern void IniOTP();
extern void wait();
void delay_ms(unsigned int count)//语音芯片专用延时命令 建议不要改 不然语音芯片的延时全都需要重新修改
{
    int i,j;                                                                                
    for(i=0;i<count;i++)                                                                    
       {
	     for(j=0;j<260;j++);
       }                                                                                     
}
/*------------------------------------------------
                      主函数
------------------------------------------------*/
void IniOTP()
{
	RST=0;
	delay_ms(2);
}

//屏幕初始化函数
void coordinate()
{ 
	int i; //初始化LCD
       CLR_Screen(White); //用背景色清屏
	//建立坐标轴
			 Line(20,295,230,295,Black);//x轴
	     Line(20,295,20,15,Black);//y轴
			 for(i=1;i<9;i++)
	       Line(20+22*i,295,20+22*i,290,Black);//x轴单位长度
				for(i=1;i<11;i++)
				  Line(20,295-27*i,27,295-27*i,Black);//Y轴单位长度
					for(i=1;i<9;i++)
					{
	        LCD_PutChar8x16(18+22*i,302,i+'0',Blue2,White);
					LCD_PutChar8x16(8,285-27*i,i+'0',Blue2,White);
					}
					LCD_PutString(8,285-27*9,"9",Blue2,White);
					LCD_PutString(1,290-27*10,"10",Blue2,White);
					LCD_PutString(16,298,"0",Blue2,White);
			for(i=1;i<6;i++)
				  {Put_pixel(20+i,15+i,Black);
					 Put_pixel(20-i,15+i,Black);
					 Put_pixel(230-i,295+i,Black);
					 Put_pixel(230-i,295-i,Black);
				 }
	 LCD_PutString(200,300,"Uce/V",Green,White);
	 LCD_PutString(0,0,"Ic/mA",Green,White);
}

//输出特性曲线函数
void printcurve(float ic,int ib)
{
int j,k;
				     k=(27*ic-1)/(log(22*1.3));
				    //  Line(20,295,21,295-k*(log(1)+1),Red);//先画出原点到第一个点的线
						for(j=21;j<20+22*1.3;j++)
						   Line(j,295-k*(log((j-20)+1)),j+1,295-k*(log((j-19)+1)),Red);
				    Line(48.6,295-ic*26.5,180,295-ic*26.5,Red);	
					switch(ib)
					 {
					 case 20:
					 LCD_PutString(180,287-ic*26.5,"Ib=20uA",Black,White);
					 break;
					 case 30:
					 LCD_PutString(180,287-ic*26.5,"Ib=30uA",Black,White);
					 break;
					 case 40:
					 LCD_PutString(180,287-ic*26.5,"Ib=40uA",Black,White);
					 break;
				 }
}

//语音播报测试函数DATA和RST要定义IO口
void OTPT(int n)
{
	RST=1;
	delay_ms(2);
	RST=0;
	delay_ms(2);
	while(n>0)
	{
	  DATA=1;
	  delay_ms(1);
		DATA=0;
		delay_ms(1);
		n--;
  }
}
void sb(int bmin)//播报放大倍数
{
int a,b,c;
	a=bmin/100;
	b=(bmin-a*100)/10;
	c=bmin%10;
	if(a==0)
	{	OTPT(20);
		delay_ms(1000);
		OTPT(b+2);
		delay_ms(330);
		OTPT(c+2);
		delay_ms(1000); 
		IniOTP()  ;
	}
	else
	{   OTPT(20);
		delay_ms(1000);
		OTPT(a+2);
		delay_ms(300);
		OTPT(b+2);
		delay_ms(330);
		OTPT(c+2);
		delay_ms(1000); 
		IniOTP();
	}
}
void printtype(char n,int bmin,int bmax)//输出 NPN还是PNP 同时输出B的范围 要区分大小
{
	
	char s[3];int a,b,c; char s1[2];
	a=bmin/100;
	b=(bmin-a*100)/10;
	c=bmin%10;
	
	s[0]=a+'0';
	s[1]=b+'0';
	s[2]=c+'0';
	if(a==0)
	{s1[0]=s[1];s1[1]=s[2];
		LCD_PutString(105,10,"B=",Magenta,White);
	  LCD_PutString(130,10,s1,Magenta,White);
	}
	else
	{	LCD_PutString(105,10,"B=",Magenta,White);
	  LCD_PutString(122,10,s,Magenta,White);
	}
	
	a=bmax/100;
	b=(bmax-a*100)/10;
	c=bmax%10;

	s[0]=a+'0';
	s[1]=b+'0';
	s[2]=c+'0';
	if(a==0)
	{s1[0]=s[1];s1[1]=s[2];
	 LCD_PutString(148,10,"--",Magenta,White);
	 LCD_PutString(164,10,s1,Magenta,White);
	}
	else{
	LCD_PutString(148,10,"--",Magenta,White);
	LCD_PutString(164,10,s,Magenta,White);}
	if(n=='N')
	{LCD_PutString(60,10,"NPN",Magenta,White);
		sb(bmin);//播放放大倍数
		IniOTP()  ;
	 	sb(bmax);
		IniOTP() ;
		OTPT(16);
		delay_ms(400);
		OTPT(12);
		delay_ms(180);
		OTPT(13);
		delay_ms(180);
		OTPT(12);
		delay_ms(180);
		OTPT(17);
		delay_ms(240);
		OTPT(19);
		delay_ms(2000);  
		IniOTP() ;
		/* 输出 "这是NPN型三极管"
		IniOTP()
//		  */  
	
	}
	else
	{
		LCD_PutString(60,10,"PNP",Magenta,White);
		sb(bmin);//播放放大倍数
		IniOTP()   ;
	 	sb(bmax);
		IniOTP()  ;
		OTPT(16);
		delay_ms(400);
		OTPT(13);
		delay_ms(180);
		OTPT(12);
		delay_ms(180);
		OTPT(13);
		delay_ms(180);
		OTPT(17);
		delay_ms(240);
		OTPT(19);
		delay_ms(2000); 
		IniOTP();/* 输出 "这是NPN型三极管"	*/
	}
}
void sbc(int bmin)//播报芯片类型
{
	//74 LS 245 04  373 74HC377 74HC00 74HC08  00 04 08 32 86 245 
	switch(bmin)
	{
		case 0:
			OTPT(9);
			delay_ms(330);
			OTPT(6);
			delay_ms(330);
			OTPT(14);
			delay_ms(330);
			OTPT(15);
			delay_ms(330);
			OTPT(2);
			delay_ms(330);
			OTPT(2);
			delay_ms(330);
			OTPT(18);
		  delay_ms(2500); IniOTP()  ;
			break;
		case 4:
			OTPT(9);
			delay_ms(330);
			OTPT(6);
			delay_ms(330);
			OTPT(14);
			delay_ms(330);
			OTPT(15);
			delay_ms(330);
			OTPT(2);
			delay_ms(330);
			OTPT(6);
			delay_ms(330);
			OTPT(18);
		  delay_ms(2500);  IniOTP()  ;
			break;
		case 5:
			OTPT(9);
			delay_ms(330);
			OTPT(6);
			delay_ms(330);
			OTPT(14);
			delay_ms(330);
			OTPT(15);
			delay_ms(330);
			OTPT(4);
			delay_ms(330);
			OTPT(6);
			delay_ms(330);
			OTPT(7);
			delay_ms(330);
			OTPT(18);
		  delay_ms(2500); 	 IniOTP()  ;
			break;
		case 8:
			OTPT(9);
			delay_ms(330);
			OTPT(6);
			delay_ms(330);
			OTPT(14);
			delay_ms(330);
			OTPT(15);
			delay_ms(330);
			OTPT(2);
			delay_ms(330);
			OTPT(10);
			delay_ms(330);
		  OTPT(18);
		  delay_ms(2500);  IniOTP()  ;
			break;
		case 2:
			OTPT(9);
			delay_ms(330);
			OTPT(6);
			delay_ms(330);
			OTPT(14);
			delay_ms(330);
			OTPT(15);
			delay_ms(330);
			OTPT(5);
			delay_ms(330);
			OTPT(4);
			delay_ms(330); 
		  OTPT(18);
		  delay_ms(2500); 	  IniOTP()  ;
			break;
		case 6:
			OTPT(9);
			delay_ms(330);
			OTPT(6);
			delay_ms(330);
			OTPT(14);
			delay_ms(330);
			OTPT(15);
			delay_ms(330);
			OTPT(10);
			delay_ms(330);
			OTPT(8);
			delay_ms(330);
		  OTPT(18);
		  delay_ms(2500); 	IniOTP()  ;
			break;
	}
	
	
	
}
void printchip(int bmin)
{ 
	
	CLR_Screen(White); //用背景色清屏
	LCD_PutString(56,160,"7 4 H C ",Magenta,White);
	switch(bmin)
	{
		case 0:
			LCD_PutString(120,160,"0 0",Magenta,White);
			break;
		case 4:
			LCD_PutString(120,160,"0 4",Magenta,White);
			break;
		case 5:
			LCD_PutString(120,160,"2 4 5",Magenta,White);
			break;
		case 8:
			LCD_PutString(120,160,"0 8",Magenta,White);
			break;
		case 2:
			LCD_PutString(120,160,"3 2",Magenta,White);
			break;
		case 6:
			LCD_PutString(120,160,"8 6",Magenta,White);
			break;
	}
	sbc(bmin);		   IniOTP()  ;

}
void wait()
{
	LCD_PutString(37,140,"if you want to test",Magenta,White);
	LCD_PutString(10,167,"please put the determinand",Magenta,White);
	LCD_PutString(40,190,"then press the key",Magenta,White);
}




/*main()
{ 
	
  IniTFT();
	printcurve(3,40);
	printtype('N',43,133);

  while(1)
       {
	     }
 
}*/




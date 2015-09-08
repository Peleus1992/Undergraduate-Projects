#include"X9313.h"
sbit keyu = P0^6;
sbit keyd = P0^7;
void main()
{
	while(1)
	{
		if(keyu == 0)
		{
			Changex9313(1, UP, 1);
			delay(200);
		}
		if(keyd == 0)
		{
			Changex9313(1, DOWN, 1);
			delay(200);
		}
	}
}
#include <STC8.H>
#include <Delay.h>
#include <ds1302.h>
unsigned char xdata Nixie_Flag = 0;

void NixieTube_FlowTenHour(unsigned char e,unsigned char f,unsigned char g)   	//四位一起滚动
{
	unsigned char a = 0;
	unsigned char i = 0;
	for(i = 0;i<10;i++)
	{
		P2 = i | i<<4;
		P0 = i | i<<4;
		Delay1ms(25);
	}

	for(i = 0;i<10;i++)
	{
		P2 = e | i<<4;
		P0 = i | i<<4;
		Delay1ms(25);
	}

	for(i = 0;i<10;i++)
	{
		P2 = e | f<<4;
		P0 = i | i<<4;
		Delay1ms(25);
	}

	for(i = 0;i<10;i++)
	{
		P2 = e | f<<4;
		P0 = g | i<<4;
		Delay1ms(25);
	}
	P0 = DS1302_Time[4]>>4 | DS1302_Time[4]<<4;			//辉光管显示分
	P2 = DS1302_Time[3]>>4 | DS1302_Time[3]<<4;			//辉光管显示时
}

void NixieTube_FlowHour(unsigned char e,unsigned char f,unsigned char g)		//三位一起滚动
{
	unsigned char a = 0;
	unsigned char i = 0;
	
	for(i = 0;i<10;i++)
	{
		P2 = e | i<<4;
		P0 = i | i<<4;
		Delay1ms(40);
	}



	for(i = 0;i<10;i++)
	{
		P2 = e | f<<4;
		P0 = i | i<<4;
		Delay1ms(30);
	}


	for(i = 0;i<10;i++)
	{
		P2 = e | f<<4;
		P0 = g | i<<4;
		Delay1ms(30);
	}

	P0 = DS1302_Time[4]>>4 | DS1302_Time[4]<<4;			//辉光管显示分
	P2 = DS1302_Time[3]>>4 | DS1302_Time[3]<<4;			//辉光管显示时	
}

void NixieTube_FlowTenMin(unsigned char e,unsigned char f,unsigned char g)		//两个分位一起滚动
{
	unsigned char a = 0;
	unsigned char i = 0;
	
	
	for(i = 0;i<10;i++)
	{
		P2 = e | f<<4;
		P0 = i | i<<4;
		Delay1ms(50);
	}


	for(i = 0;i<10;i++)
	{
		P2 = e | f<<4;
		P0 = g | i<<4;
		Delay1ms(50);
	}
	
	P0 = DS1302_Time[4]>>4 | DS1302_Time[4]<<4;			//辉光管显示分
	P2 = DS1302_Time[3]>>4 | DS1302_Time[3]<<4;			//辉光管显示时	
}

void NixieTube_FlowMin(unsigned char e,unsigned char f,unsigned char g)			//分位滚动
{
	unsigned char a = 0;
	unsigned char i = 0;
	
	
	for(a = 0; a<2;a++)
	{
		for(i = 0;i<10;i++)
		{
			P2 = e | f<<4;
			P0 = g | i<<4;
			Delay1ms(50);
		}
	}

	P0 = DS1302_Time[4]>>4 | DS1302_Time[4]<<4;			//辉光管显示分
	P2 = DS1302_Time[3]>>4 | DS1302_Time[3]<<4;			//辉光管显示时	
}

void NixieTube_Flag()
{
	if(DS1302_Time[5] == 0)									//以下4个if为辉光管设定进位标志，方便后续滚动操作，当前位秒向分进位
	{
		Nixie_Flag = 0;
	}
	
	if((DS1302_Time[5] == 0) && ((DS1302_Time[4] & 0x0f) == 0))      //分向十分进位
	{
		Nixie_Flag = 1;
	} 
	
	if((DS1302_Time[5] == 0) && ((DS1302_Time[4] & 0x0f) == 0) && ((DS1302_Time[4]>>4) == 0))			//十分向小时进位
	{
		Nixie_Flag = 2;
	}
	
	if((DS1302_Time[5] == 0) && ((DS1302_Time[4] & 0x0f) == 0) && ((DS1302_Time[4]>>4) == 0) && ((DS1302_Time[3] & 0x0f) == 0) && ((DS1302_Time[3]>>4) == 0))  //小时向十小时进位
	{
		Nixie_Flag = 3;
	}
}
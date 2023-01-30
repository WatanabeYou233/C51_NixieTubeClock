#include <STC8.H>
#include <Delay.h>
#include <DS1302.h>
#include <oled12864.h>
#include <dht11.h>
#include <uart.h>
#include <NixieTube.h>


unsigned int xdata e = 0;
unsigned int xdata n = 0;
unsigned int xdata x = 0;

unsigned char Nixie_Time[4];

void Timer0Init(void)		//1毫秒@22.1184MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x9A;		//设置定时初始值
	TH0 = 0xA9;		//设置定时初始值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	
	PPCA = 1;
	IPH = PPCAH; //CCP中断优先级设置为zuigao级
	
	IP2 = PS2;
	IP2H = 0X00;  //设置串口2中断优先级为较gao级
}


void Timer0Interrupt() interrupt 1
{
	e++;
	n++;
	x++;
	if(e > 20000)	//(计数60000次)
	{
		e = 0;
		send2Str("AT+CIPSNTPTIME?\r\n"); //向ntp服务器发送请求对时
	}
}



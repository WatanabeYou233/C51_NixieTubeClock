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

void Timer0Init(void)		//1����@22.1184MHz
{
	AUXR |= 0x80;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0x9A;		//���ö�ʱ��ʼֵ
	TH0 = 0xA9;		//���ö�ʱ��ʼֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	
	PPCA = 1;
	IPH = PPCAH; //CCP�ж����ȼ�����Ϊzuigao��
	
	IP2 = PS2;
	IP2H = 0X00;  //���ô���2�ж����ȼ�Ϊ��gao��
}


void Timer0Interrupt() interrupt 1
{
	e++;
	n++;
	x++;
	if(e > 20000)	//(����60000��)
	{
		e = 0;
		send2Str("AT+CIPSNTPTIME?\r\n"); //��ntp���������������ʱ
	}
}



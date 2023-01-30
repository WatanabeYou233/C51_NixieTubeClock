#include <string.h>
#include <stdio.h>
#include <STC8.H>
#include <uart.h>
#include <DS1302.H>

unsigned char xdata ReceiveData[len];   //串口中断接受数据长度len
char code DS1302_Month[12][5] ={{"Jan"},{"Feb"},{"Mar"},{"Apr"},{"May"},{"Jun"},{"Jul"},{"Aug"},{"Sep"},{"Oct"},{"Nov"},{"Dec"}};
char xdata DS1302_MonthRec[5] = {'\0','\0','\0','\0','\0'};
char code DS1302_Day[7][4] = {{"Mon"},{"Tue"},{"Wed"},{"Thu"},{"Fri"},{"Sat"},{"Sun"}};
char xdata DS1302_DayRec[4] = {'\0','\0','\0','\0'};
void Uart2Init(void)		//115200bps@22.1184MHz 初始化串口2
{
	S2CON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x04;		//定时器2时钟为Fosc,即1T
	T2L = 0xD0;		//设定定时初值
	T2H = 0xFF;		//设定定时初值
	AUXR |= 0x10;		//启动定时器2
	IE2 = 0;
	EA = 1;
}


void UartInit(void)		//115200bps@22.1184MHz  初始化串口1
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设定定时器1为16位自动重装方式
	TL1 = 0xD0;		//设定定时初值
	TH1 = 0xFF;		//设定定时初值
	ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1
	ES = 0;     //禁用串口1中断
	EA = 1;    //总中断
	
}


void sendByte(unsigned char b)  // 串口1发送单个字符
{
	ES=0;
	SBUF = b;
	while(TI == 0);
	TI = 0;
	ES=1;
}

void send2Byte(unsigned char b)  // 串口2发送单个字符
{
	IE2=0;
	S2BUF = b;
	while(!(S2CON&S2TI));
	S2CON &= ~0X02;
	IE2 = ES2;
}


void sendStr(char *s)  //串口1发送字符串
{
	while(*s != '\0')
	{
		sendByte(*s);
		s++;
	}
}

void send2Str(char *s)  //串口2发送字符串 格式:send2Str("指令\r\n")
{
	while(*s != '\0')
	{
		send2Byte(*s);
		s++;
	}
}



void Uart_Interrupt() interrupt 8			//串口2（连接eps8266）中断服务程序
{
	char ReceiveDataByte;
	static unsigned char i=0;
	unsigned char a,c,d = 0;
	if(S2CON & 0X01 ) //判断中断类型
	{
		S2CON &= ~0X01; //标志位清除
		ReceiveDataByte = S2BUF;
		if(ReceiveDataByte == ':' || i>0) // 数据取第一个":"之后的值
		{
			ReceiveData[i] = ReceiveDataByte;
			i++;
			if(ReceiveDataByte == '\n' || i>=len)    //判断接受字符完成
			{
				i = 0;
				//此处填写接受数据后相应的动作
				sendStr(ReceiveData);
				DS1302_Time[0] =  (ReceiveData[23]-0x30)<<4 | (ReceiveData[24]-0x30);
				DS1302_Time[2] = (ReceiveData[9]-0x30)<<4 | (ReceiveData[10]-0x30);
				DS1302_Time[3] = (ReceiveData[12]-0x30)<<4 | (ReceiveData[13]-0x30);
				DS1302_Time[4] = (ReceiveData[15]-0x30)<<4 | (ReceiveData[16]-0x30);
				DS1302_Time[5] = (ReceiveData[18]-0x30)<<4 | (ReceiveData[19]-0x30);
				
				DS1302_DayRec[0] = ReceiveData[1];
				DS1302_DayRec[1] = ReceiveData[2];
				DS1302_DayRec[2] = ReceiveData[3];
				
				DS1302_MonthRec[0] = ReceiveData[5];
				DS1302_MonthRec[1] = ReceiveData[6];
				DS1302_MonthRec[2] = ReceiveData[7];   //以上、下将MCU接收的ASCII码时间转换为BCD码时间
				memset(ReceiveData,'\0',sizeof(ReceiveData));
				for (d = 0; d < 7; d++)						//通过Receivedata判断星期并转化为BCD码
				{
					if(strstr(DS1302_Day[d],DS1302_DayRec) != NULL)
					{
						c = d+1;
						DS1302_Time[6] = c/10*16+c%10;
					}
				}
				
				for (a = 0; a < 12; a++)					//通过Receivedata判断月份并转化为BCD码
				{
					if(strstr(DS1302_Month[a],DS1302_MonthRec) != NULL)
					{
						c = a+1;
						DS1302_Time[1] = c/10*16+c%10;
					}
				}
				
				if(DS1302_Time[0] != 0x70)			//判断获取时间是否有效避免1970年
				{
					DS1302_SetTimeBCD();  //使用BCD码设置DS1302时间
				}
			}
		}
	}
	else
	{
		S2CON &= ~0X02;  ////标志位清除
	}
}


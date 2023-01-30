#include <string.h>
#include <stdio.h>
#include <STC8.H>
#include <uart.h>
#include <DS1302.H>

unsigned char xdata ReceiveData[len];   //�����жϽ������ݳ���len
char code DS1302_Month[12][5] ={{"Jan"},{"Feb"},{"Mar"},{"Apr"},{"May"},{"Jun"},{"Jul"},{"Aug"},{"Sep"},{"Oct"},{"Nov"},{"Dec"}};
char xdata DS1302_MonthRec[5] = {'\0','\0','\0','\0','\0'};
char code DS1302_Day[7][4] = {{"Mon"},{"Tue"},{"Wed"},{"Thu"},{"Fri"},{"Sat"},{"Sun"}};
char xdata DS1302_DayRec[4] = {'\0','\0','\0','\0'};
void Uart2Init(void)		//115200bps@22.1184MHz ��ʼ������2
{
	S2CON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x04;		//��ʱ��2ʱ��ΪFosc,��1T
	T2L = 0xD0;		//�趨��ʱ��ֵ
	T2H = 0xFF;		//�趨��ʱ��ֵ
	AUXR |= 0x10;		//������ʱ��2
	IE2 = 0;
	EA = 1;
}


void UartInit(void)		//115200bps@22.1184MHz  ��ʼ������1
{
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x40;		//��ʱ��1ʱ��ΪFosc,��1T
	AUXR &= 0xFE;		//����1ѡ��ʱ��1Ϊ�����ʷ�����
	TMOD &= 0x0F;		//�趨��ʱ��1Ϊ16λ�Զ���װ��ʽ
	TL1 = 0xD0;		//�趨��ʱ��ֵ
	TH1 = 0xFF;		//�趨��ʱ��ֵ
	ET1 = 0;		//��ֹ��ʱ��1�ж�
	TR1 = 1;		//������ʱ��1
	ES = 0;     //���ô���1�ж�
	EA = 1;    //���ж�
	
}


void sendByte(unsigned char b)  // ����1���͵����ַ�
{
	ES=0;
	SBUF = b;
	while(TI == 0);
	TI = 0;
	ES=1;
}

void send2Byte(unsigned char b)  // ����2���͵����ַ�
{
	IE2=0;
	S2BUF = b;
	while(!(S2CON&S2TI));
	S2CON &= ~0X02;
	IE2 = ES2;
}


void sendStr(char *s)  //����1�����ַ���
{
	while(*s != '\0')
	{
		sendByte(*s);
		s++;
	}
}

void send2Str(char *s)  //����2�����ַ��� ��ʽ:send2Str("ָ��\r\n")
{
	while(*s != '\0')
	{
		send2Byte(*s);
		s++;
	}
}



void Uart_Interrupt() interrupt 8			//����2������eps8266���жϷ������
{
	char ReceiveDataByte;
	static unsigned char i=0;
	unsigned char a,c,d = 0;
	if(S2CON & 0X01 ) //�ж��ж�����
	{
		S2CON &= ~0X01; //��־λ���
		ReceiveDataByte = S2BUF;
		if(ReceiveDataByte == ':' || i>0) // ����ȡ��һ��":"֮���ֵ
		{
			ReceiveData[i] = ReceiveDataByte;
			i++;
			if(ReceiveDataByte == '\n' || i>=len)    //�жϽ����ַ����
			{
				i = 0;
				//�˴���д�������ݺ���Ӧ�Ķ���
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
				DS1302_MonthRec[2] = ReceiveData[7];   //���ϡ��½�MCU���յ�ASCII��ʱ��ת��ΪBCD��ʱ��
				memset(ReceiveData,'\0',sizeof(ReceiveData));
				for (d = 0; d < 7; d++)						//ͨ��Receivedata�ж����ڲ�ת��ΪBCD��
				{
					if(strstr(DS1302_Day[d],DS1302_DayRec) != NULL)
					{
						c = d+1;
						DS1302_Time[6] = c/10*16+c%10;
					}
				}
				
				for (a = 0; a < 12; a++)					//ͨ��Receivedata�ж��·ݲ�ת��ΪBCD��
				{
					if(strstr(DS1302_Month[a],DS1302_MonthRec) != NULL)
					{
						c = a+1;
						DS1302_Time[1] = c/10*16+c%10;
					}
				}
				
				if(DS1302_Time[0] != 0x70)			//�жϻ�ȡʱ���Ƿ���Ч����1970��
				{
					DS1302_SetTimeBCD();  //ʹ��BCD������DS1302ʱ��
				}
			}
		}
	}
	else
	{
		S2CON &= ~0X02;  ////��־λ���
	}
}


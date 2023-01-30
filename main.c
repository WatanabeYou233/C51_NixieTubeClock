#include <STC8.H>
#include <timer.h>
#include <uart.h>
#include <Delay.h>
#include <DS1302.h>
#include <oled12864.h>
#include <DHT11.h>
#include <NixieTube.h>

unsigned char DHT11_RH[10];
unsigned char DHT11_Temp[10];

unsigned char Str_DHT11[16];
DHT11_Data	Get_Data;//�ṹ�壬�洢��ȡ����DHT11����

unsigned char xdata DS_1302TimeYear[3];
unsigned char xdata DS_1302TimeMonth[3];
unsigned char xdata DS_1302TimeDay[3];
unsigned char xdata DS_1302TimeHour[3];
unsigned char xdata DS_1302TimeMin[3];
unsigned char xdata DS_1302TimeSec[3];


void main()
{
	OLED_Init();	//oled��ʼ��
	UartInit();		//����1��ʼ��
	Uart2Init();	//����2��ʼ��
	//send2Str("AT+CWJAP_DEF=\"306\",\"zhongheng.123\"\r\n");			//����WIFI
	OLED_ShowStr(0,0,"Please Wait...",2);
	Delay1ms(5000);		//�ȴ�esp01��ʼ�����
	Init_CCP0();
	Timer0Init();		//��ʱ��0��ʼ�� + �ж����ȼ�����
	DS1302_Init();	//DS1302��ʼ��
	send2Str("AT+CIPSNTPCFG=1,8,\"ntp1.aliyun.com\"\r\n"); //����ʱ�������aliyun
	OLED_Clean();
	OLED_ShowCN(0,0,0);  						//��	(0,0)
	OLED_ShowCN(17,0,1);						//��	(17,0)
	OLED_ShowStr(33,0,":",2); 			//��	(33,0)
	OLED_ShowCN(82,0,7); 						//��	(81,0)
	OLED_ShowCN(0,2,2);							//ʪ	(0,2)
	OLED_ShowCN(17,2,3);						//��	(17,2)
	OLED_ShowStr(33,2,":",2);				//��	(33,2)
	OLED_ShowCN(17,4,4);							//��	(17,4)
	OLED_ShowCN(49,4,5);							//��	(49,4)
	OLED_ShowCN(81,4,6);							//��	(49,4)
	OLED_ShowStr(33,6,":",2); 			//��	(33,0)
	OLED_ShowStr(57,6,":",2); 			//��	(57,6)
	IE2 = ES2;  //���������ж�2
	send2Str("AT+CIPSNTPTIME?\r\n"); //��ntp��������������
	ET0 = 1;		//ʹ�ܶ�ʱ��0�ж�
	DS1302_ReadTimeBCD();						
	
	Nixie_Time[0] = DS1302_Time[3]>>4;
	Nixie_Time[1] = DS1302_Time[3] & 0x0f;
	Nixie_Time[2] = DS1302_Time[4]>>4;
	Nixie_Time[3] = DS1302_Time[4] & 0x0f;
	
	NixieTube_FlowTenHour(Nixie_Time[0],Nixie_Time[1],Nixie_Time[2]);
	while(1)
	{
		if(n >2000)
		{
			n = 0;
			if(Get_DHT11(&Get_Data) == 0)//��ȡ�ɹ�
			{
				Data_To_Str(&Get_Data);//ת������ΪASCII�������Ķ�
				sendStr(Str_DHT11);//�������ݵ�����
				DHT11_RH[0] = Str_DHT11[0];			//ת������ΪASCII�������Ķ�
				DHT11_RH[1]	= Str_DHT11[1];
				DHT11_RH[2]	= '%';
				DHT11_RH[3]	= 0;
				DHT11_Temp[0] = Str_DHT11[5];
				DHT11_Temp[1] = Str_DHT11[6];
				DHT11_Temp[2] = Str_DHT11[7];
				DHT11_Temp[3] = Str_DHT11[8];
				DHT11_Temp[4] = Str_DHT11[9];
				DHT11_Temp[5] = 0;
				OLED_ShowStr(41,2,DHT11_RH,2);
				OLED_ShowStr(41,0,DHT11_Temp,2);
			}
		}
		
		if(x >30000)		//һ����һ��,��ֹ�����ж�
		{
			x = 0;
			NixieTube_FlowTenHour(Nixie_Time[0],Nixie_Time[1],Nixie_Time[2]);
		}
		
		DS1302_ReadTimeASCII();									//��ȡDS1302�ڵ�ʱ����ASCii��ʽ�洢��DS1302_Time����
		
		DS_1302TimeYear[0] = DS1302_Time[0]; 		//���¶�DS1302��������������ʾ��OLED
		DS_1302TimeYear[1] = DS1302_Time[1];
		DS_1302TimeYear[2] = 0;
			
		DS_1302TimeMonth[0] = DS1302_Time[2];
		DS_1302TimeMonth[1] = DS1302_Time[3];
		DS_1302TimeMonth[2] = 0;
			
		DS_1302TimeDay[0] = DS1302_Time[4];
		DS_1302TimeDay[1] = DS1302_Time[5];
		DS_1302TimeDay[2] = 0;
			
		DS_1302TimeHour[0] = DS1302_Time[6];
		DS_1302TimeHour[1] = DS1302_Time[7];
		DS_1302TimeHour[2] = 0;
			
		DS_1302TimeMin[0] = DS1302_Time[8];
		DS_1302TimeMin[1] = DS1302_Time[9];
		DS_1302TimeMin[2] = 0;
			
		DS_1302TimeSec[0] = DS1302_Time[10];
		DS_1302TimeSec[1] = DS1302_Time[11];
		DS_1302TimeSec[2] = 0;
			
			
		OLED_ShowStr(0,4,DS_1302TimeYear,2); 						//��ʱ��������ʾ��OLED��Ļ
		OLED_ShowStr(33,4,DS_1302TimeMonth,2);
		OLED_ShowStr(65,4,DS_1302TimeDay,2);
			
		OLED_ShowStr(17,6,DS_1302TimeHour,2);
		OLED_ShowStr(41,6,DS_1302TimeMin,2);
		OLED_ShowStr(65,6,DS_1302TimeSec,2);
		
		DS1302_ReadTimeBCD();	
		P0 = DS1302_Time[4]>>4 | DS1302_Time[4]<<4;			//�Թ����ʾ��
		P2 = DS1302_Time[3]>>4 | DS1302_Time[3]<<4;			//�Թ����ʾʱ
		DS1302_ReadTimeBCD();						
		Nixie_Time[0] = DS1302_Time[3]>>4;
		Nixie_Time[1] = DS1302_Time[3] & 0x0f;
		Nixie_Time[2] = DS1302_Time[4]>>4;
		if(DS1302_Time[5] == 0)   		//��������ֽ�λ���ж��ĸ�Flag��������Ӧ����Ч��
		{
			NixieTube_Flag();			//��ȡflag
			switch(Nixie_Flag)
			{
				case 0:
					NixieTube_FlowMin(Nixie_Time[0],Nixie_Time[1],Nixie_Time[2]);
					break;
				case 1:
					NixieTube_FlowTenMin(Nixie_Time[0],Nixie_Time[1],Nixie_Time[2]);
					break;
				case 2:
					NixieTube_FlowHour(Nixie_Time[0],Nixie_Time[1],Nixie_Time[2]);
					break;
				case 3:
					NixieTube_FlowTenHour(Nixie_Time[0],Nixie_Time[1],Nixie_Time[2]);
					break;
				default:
					break;
			}
		}
	}
}



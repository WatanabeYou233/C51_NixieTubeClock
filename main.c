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
DHT11_Data	Get_Data;//结构体，存储读取到的DHT11数据

unsigned char xdata DS_1302TimeYear[3];
unsigned char xdata DS_1302TimeMonth[3];
unsigned char xdata DS_1302TimeDay[3];
unsigned char xdata DS_1302TimeHour[3];
unsigned char xdata DS_1302TimeMin[3];
unsigned char xdata DS_1302TimeSec[3];


void main()
{
	OLED_Init();	//oled初始化
	UartInit();		//串口1初始化
	Uart2Init();	//串口2初始化
	//send2Str("AT+CWJAP_DEF=\"306\",\"zhongheng.123\"\r\n");			//设置WIFI
	OLED_ShowStr(0,0,"Please Wait...",2);
	Delay1ms(5000);		//等待esp01初始化完成
	Init_CCP0();
	Timer0Init();		//定时器0初始化 + 中断优先级设置
	DS1302_Init();	//DS1302初始化
	send2Str("AT+CIPSNTPCFG=1,8,\"ntp1.aliyun.com\"\r\n"); //设置时间服务器aliyun
	OLED_Clean();
	OLED_ShowCN(0,0,0);  						//温	(0,0)
	OLED_ShowCN(17,0,1);						//度	(17,0)
	OLED_ShowStr(33,0,":",2); 			//：	(33,0)
	OLED_ShowCN(82,0,7); 						//℃	(81,0)
	OLED_ShowCN(0,2,2);							//湿	(0,2)
	OLED_ShowCN(17,2,3);						//度	(17,2)
	OLED_ShowStr(33,2,":",2);				//：	(33,2)
	OLED_ShowCN(17,4,4);							//年	(17,4)
	OLED_ShowCN(49,4,5);							//月	(49,4)
	OLED_ShowCN(81,4,6);							//日	(49,4)
	OLED_ShowStr(33,6,":",2); 			//：	(33,0)
	OLED_ShowStr(57,6,":",2); 			//：	(57,6)
	IE2 = ES2;  //开启串口中断2
	send2Str("AT+CIPSNTPTIME?\r\n"); //向ntp服务器发送请求
	ET0 = 1;		//使能定时器0中断
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
			if(Get_DHT11(&Get_Data) == 0)//读取成功
			{
				Data_To_Str(&Get_Data);//转化数据为ASCII，便于阅读
				sendStr(Str_DHT11);//发送数据到串口
				DHT11_RH[0] = Str_DHT11[0];			//转化数据为ASCII，便于阅读
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
		
		if(x >30000)		//一分钟一次,防止阴极中毒
		{
			x = 0;
			NixieTube_FlowTenHour(Nixie_Time[0],Nixie_Time[1],Nixie_Time[2]);
		}
		
		DS1302_ReadTimeASCII();									//读取DS1302内的时间以ASCii形式存储到DS1302_Time数组
		
		DS_1302TimeYear[0] = DS1302_Time[0]; 		//以下对DS1302数组做处理方便显示在OLED
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
			
			
		OLED_ShowStr(0,4,DS_1302TimeYear,2); 						//将时间日期显示在OLED屏幕
		OLED_ShowStr(33,4,DS_1302TimeMonth,2);
		OLED_ShowStr(65,4,DS_1302TimeDay,2);
			
		OLED_ShowStr(17,6,DS_1302TimeHour,2);
		OLED_ShowStr(41,6,DS_1302TimeMin,2);
		OLED_ShowStr(65,6,DS_1302TimeSec,2);
		
		DS1302_ReadTimeBCD();	
		P0 = DS1302_Time[4]>>4 | DS1302_Time[4]<<4;			//辉光管显示分
		P2 = DS1302_Time[3]>>4 | DS1302_Time[3]<<4;			//辉光管显示时
		DS1302_ReadTimeBCD();						
		Nixie_Time[0] = DS1302_Time[3]>>4;
		Nixie_Time[1] = DS1302_Time[3] & 0x0f;
		Nixie_Time[2] = DS1302_Time[4]>>4;
		if(DS1302_Time[5] == 0)   		//产生秒向分进位后判断四个Flag，产生相应滚动效果
		{
			NixieTube_Flag();			//获取flag
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



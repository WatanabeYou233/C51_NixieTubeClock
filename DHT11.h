//测试温湿度传感器 DHT11的读取   使用CCP功能，接收传感器发送的电平信号到数组，然后开始解码
#ifndef __DHT11_H__
#define __DHT11_H__
#include <STC8.H>
extern unsigned char Str_DHT11[16];//一帧有效数据，ASICII格式的。15个有效数据。

/*定义结构体，保存读取的温度值*/
typedef struct
{
	unsigned char Humi_H;		//湿度的整数部分
	unsigned char Humi_L;	 	//湿度的小数部分
	unsigned char Temp_H;	 	//温度的整数部分
	unsigned char Temp_L;	 	//温度的小数部分
	unsigned char Check_Sum;	 	//校验和
} DHT11_Data;

//初始化CCP0  使用CCP0管脚，采样脉冲信号，利用捕获功能，计算高电平宽度和低电平宽度
void Init_CCP0(void);
//获取一次DHT11的数据 ,成功返回 0 ，通信协议失败返回1 ，校验错误返回2
unsigned char Get_DHT11(DHT11_Data *Data);
//把一个结构体里面的温湿度数据，转化为便于识别的字符串
void Data_To_Str(DHT11_Data *Data);

#endif
#include "DHT11.h"
#include "Uart.h"


#define	DHT11_DAT		P17

//定义一个联合体，用于转换捕捉寄存器里面的数据 0-65535
static union 
{
	unsigned int U16;
	unsigned char U8[2];
}High;//表示，处理高电平中断和处理低电平中断的临时变量
static unsigned int xdata DHT11_Buffer[90];//接收到的脉冲宽度
static unsigned char DTH11_Buff_Count=0;//CCP中断里面进行计数操作


//软件延时函数，用于发送DHT11起始信号使用
void Delay_1ms(unsigned char a)	// 1ms @22.1184MHz
{
	unsigned char i, j;
	unsigned char b=0;
	for(b=0;b<a;b++)
	{
		i = 29;
		j = 183;
		do
		{
			while (--j);
		} while (--i);
	}
}

//获取一次DHT11的数据 ,成功返回 0 ，通信协议失败返回1 ，校验错误返回2
unsigned char Get_DHT11(DHT11_Data *Data)
{
	unsigned char i=0,j=0;
	
	DHT11_DAT	=	0;//通过拉低数据线25mS，发送一个通信的起始信号
	Delay_1ms(25);
	DHT11_DAT	=	1;//拉高数据线，等待DHT11发送数据。如果接收数据，则会进入CCP中断处理
	CL = 0;CH = 0;//捕获定时器清零
	CR = 1;
	DTH11_Buff_Count = 0;//捕获次数清零
	Delay_1ms(7);//软件延时7mS，等待接收完成数据。根据DHT11时序图，接收的时间一定小于 5344uS
	CR = 0;//关闭PCA阵列，下面程序开解解码接收到的脉冲
	for(i=0;i<90;i++)//接收到的脉冲，这里换算成uS,主频30M，所以除以30就是1微妙
		DHT11_Buffer[i] /= 22;
	
	//根据DHT11协议，检测一个83uS的应答和一个87uS的开始信号，下一个信号就是数据
	for(i=0;i<90;i++)
	{
		if((DHT11_Buffer[i]>78)&&(DHT11_Buffer[i+1]>78))
		{
			j=i+2;
			break;
		}
	}
	if(i > 90)//表示没有检测到起始信号，通信失败
		return 1;
	i = j;
	j = 0;
	Data->Humi_H = 0;Data->Humi_L = 0;
	Data->Temp_H = 0;Data->Temp_L = 0;
	Data->Check_Sum = 0;
	for(i;i<90;i+=2)//从有效数据开始解码
	{
		if(j < 8)
		{
			Data->Humi_H <<= 1;
			if(DHT11_Buffer[i]<DHT11_Buffer[i+1])//根据DHT11协议，这表示第1个数据 
				Data->Humi_H++;
		}
		if((j > 7) && (j < 16))
		{
			Data->Humi_L <<= 1;
			if(DHT11_Buffer[i]<DHT11_Buffer[i+1])//根据DHT11协议，这表示第2个数据 
				Data->Humi_L++;
		}
		if((j > 15) && (j < 24))
		{
			Data->Temp_H <<= 1;
			if(DHT11_Buffer[i]<DHT11_Buffer[i+1])//根据DHT11协议，这表示第3个数据 
				Data->Temp_H++;
		}
		if((j > 23) && (j < 32))
		{
			Data->Temp_L <<= 1;
			if(DHT11_Buffer[i]<DHT11_Buffer[i+1])//根据DHT11协议，这表示第4个数据 
				Data->Temp_L++;
		}
		if((j > 31) && (j < 40))
		{
			Data->Check_Sum <<= 1;
			if(DHT11_Buffer[i]<DHT11_Buffer[i+1])//根据DHT11协议，这表示第5个数据 
				Data->Check_Sum++;
		}
		j++;
	}
	//进行校验，如果出错返回2
	if((Data->Humi_H + Data->Humi_L + Data->Temp_H + Data->Temp_L) == Data->Check_Sum)
		return 0;//成功读取数据
	else
		return 2;
}
//初始化CCP0  使用CCP0管脚，采样脉冲信号，利用捕获功能，捕获高电平宽度和低电平宽度     板子上面A4排母
void Init_CCP0(void)
{
	unsigned char i=0;
	//P_SW1 = 0X20;//CCP 全部分布到_3
	CF = 0;//PCA计数器阵列溢出标志位
	CR = 0;//PCA计数器阵列开关
	CCF2 = 0;//PCA2中断标志位
	CCF1 = 0;//PCA1中断标志位
	CCF0 = 0;//PCA0中断标志位
  CMOD = 0x08;//设置PCA时钟源&禁止PCA定时器溢出中断    时钟源是 1/1 ，若时钟30MHZ，那么CCP时钟是30MHZ ，每30个脉冲1uS       
	CCAPM0 = 0X71;//设置CCP0的参数，捕获模式，上升沿下降沿都捕获，允许CCP0中断
	CL = 0;CH = 0;//复位PCA寄存器
	CCAP0L = 0;CCAP0H = 0;//复位捕捉寄存器
	CR = 0;//PCA阵列开关
	for(i=0;i<90;i++)//接收到的脉冲清零
		DHT11_Buffer[i] = 0;
}

//把一个结构体里面的温湿度数据，转化为便于识别的字符串
void Data_To_Str(DHT11_Data *Data)
{
	Str_DHT11[0] = Data->Humi_H / 10 + 0X30;
	Str_DHT11[1] = Data->Humi_H % 10 + 0X30;
	Str_DHT11[2] = '.';
	Str_DHT11[3] = Data->Humi_L % 10 + 0X30;
	Str_DHT11[4] = '%';
	
	Str_DHT11[6] = Data->Temp_H / 10 + 0X30;
	Str_DHT11[7] = Data->Temp_H % 10 + 0X30;
	Str_DHT11[8] = '.';
	if((Data->Temp_L & 0X80) == 0X80)//表示接受到的温度是负数
	{
		Data->Temp_L &= 0X7F;
		Str_DHT11[5] = '-';
	}
	else//接收到的温度是正数
	{
		Str_DHT11[5] = '+';
	}
	Str_DHT11[9] = Data->Temp_L % 10 + 0X30;
	Str_DHT11[10] = ' ';
	
	Str_DHT11[11] = 0X0D;
	Str_DHT11[12] = 0X0A;
	
	Str_DHT11[13] = 0;
}
//PCA中断入口，具体是哪一个中断需要判断标志位
void PCA_isr(void) interrupt 7
{
	CL = 0;CH = 0;//复位捕捉寄存器
	High.U8[0] = CCAP0H;
	High.U8[1] = CCAP0L;
	DHT11_Buffer[DTH11_Buff_Count++] = High.U16;//把读到的捕获数据给了数据数组
	CCF0 = 0;//由于程序仅仅使用了一个CCF0，所以进入中断后不用判断
}
//




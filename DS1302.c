#include <STC8.H>
#include <Delay.h>
#include <uart.h>
#include <ds1302.h>


//引脚定义
sbit DS1302_SCLK=P1^3;
sbit DS1302_IO=P1^2;
sbit DS1302_CE=P1^6;



//时间数组，索引0~6分别为年、月、日、时、分、秒、星期
unsigned char DS1302_Time[13]={0x21,0x11,0x14,0x01,0x27,0x40,0x06,'\0'};

/**
  * @brief  DS1302初始化
  * @param  无
  * @retval 无
  */
void DS1302_Init(void)
{
	DS1302_CE=0;
	DS1302_SCLK=0;
}

/**
  * @brief  DS1302写一个字节
  * @param  Command 命令字/地址
  * @param  Data 要写入的数据
  * @retval 无
  */
void DS1302_WriteByte(unsigned char Command,Data)
{
	unsigned char i;
	DS1302_CE=1;
	for(i=0;i<8;i++)
	{
		DS1302_IO=Command&(0x01<<i);
		DS1302_SCLK=1;
		Delay1us(1);
		DS1302_SCLK=0;
		Delay1us(1);
	}
	for(i=0;i<8;i++)
	{
		DS1302_IO=Data&(0x01<<i);
		DS1302_SCLK=1;
		Delay1us(1);
		DS1302_SCLK=0;
		Delay1us(1);
	}
	DS1302_CE=0;
}

/**
  * @brief  DS1302读一个字节
  * @param  Command 命令字/地址
  * @retval 读出的数据
  */
unsigned char DS1302_ReadByte(unsigned char Command) 
{
	unsigned char i,Data=0x00;
	Command|=0x01;	//将指令转换为读指令
	DS1302_CE=1;
	for(i=0;i<8;i++)
	{
		DS1302_IO=Command&(0x01<<i);
		DS1302_SCLK=0;
		Delay1us(1);
		DS1302_SCLK=1;
		Delay1us(1);
	}
	for(i=0;i<8;i++)
	{
		DS1302_SCLK=1;
		Delay1us(1);
		DS1302_SCLK=0;
		Delay1us(1);
		if(DS1302_IO){Data|=(0x01<<i);}
	}
	DS1302_CE=0;
	DS1302_IO=0;	//读取后将IO设置为0，否则读出的数据会出错
	return Data;
}

/**
  * @brief  DS1302设置时间，调用之后，DS1302_Time数组的数字会被设置到DS1302中
  * @param  无
  * @retval 无
  */
void DS1302_SetTime(void)
{
	DS1302_WriteByte(DS1302_WP,0x00);
	DS1302_WriteByte(DS1302_YEAR,DS1302_Time[0]/10*16+DS1302_Time[0]%10);//十进制转BCD码后写入
	DS1302_WriteByte(DS1302_MONTH,DS1302_Time[1]/10*16+DS1302_Time[1]%10);
	DS1302_WriteByte(DS1302_DATE,DS1302_Time[2]/10*16+DS1302_Time[2]%10);
	DS1302_WriteByte(DS1302_HOUR,DS1302_Time[3]/10*16+DS1302_Time[3]%10);
	DS1302_WriteByte(DS1302_MINUTE,DS1302_Time[4]/10*16+DS1302_Time[4]%10);
	DS1302_WriteByte(DS1302_SECOND,DS1302_Time[5]/10*16+DS1302_Time[5]%10);
	DS1302_WriteByte(DS1302_DAY,DS1302_Time[6]/10*16+DS1302_Time[6]%10);
	DS1302_WriteByte(DS1302_WP,0x80);
}

void DS1302_SetTimeBCD(void)
{
	DS1302_WriteByte(DS1302_WP,0x00);
	DS1302_WriteByte(DS1302_YEAR,DS1302_Time[0]);//   BCD码写入
	DS1302_WriteByte(DS1302_MONTH,DS1302_Time[1]);
	DS1302_WriteByte(DS1302_DATE,DS1302_Time[2]);
	DS1302_WriteByte(DS1302_HOUR,DS1302_Time[3]);    //DS1302_Time[3] = DS1302_TimeBCD[0];
	DS1302_WriteByte(DS1302_MINUTE,DS1302_Time[4]);  //DS1302_Time[4] = DS1302_TimeBCD[1];
	DS1302_WriteByte(DS1302_SECOND,DS1302_Time[5]);	//DS1302_Time[5] = DS1302_TimeBCD[0];
	DS1302_WriteByte(DS1302_DAY,DS1302_Time[6]);
	DS1302_WriteByte(DS1302_WP,0x80);
}


/**
  * @brief  DS1302读取时间，调用之后，DS1302中的数据会被读取到DS1302_Time数组中
  * @param  无
  * @retval 无
  */
void DS1302_ReadTime(void)
{
	unsigned char Temp;
	Temp=DS1302_ReadByte(DS1302_YEAR);
	DS1302_Time[0]=Temp/16*10+Temp%16;//BCD码转十进制后读取
	Temp=DS1302_ReadByte(DS1302_MONTH);
	DS1302_Time[1]=Temp/16*10+Temp%16;
	Temp=DS1302_ReadByte(DS1302_DATE);
	DS1302_Time[2]=Temp/16*10+Temp%16;
	Temp=DS1302_ReadByte(DS1302_HOUR);
	DS1302_Time[3]=Temp/16*10+Temp%16;
	Temp=DS1302_ReadByte(DS1302_MINUTE);
	DS1302_Time[4]=Temp/16*10+Temp%16;
	Temp=DS1302_ReadByte(DS1302_SECOND);
	DS1302_Time[5]=Temp/16*10+Temp%16;
	Temp=DS1302_ReadByte(DS1302_DAY);
	DS1302_Time[6]=Temp/16*10+Temp%16;
}


void DS1302_ReadTimeBCD(void)    //读取BCD码到DS1302_Time数组
{
	unsigned char Temp;
	Temp=DS1302_ReadByte(DS1302_YEAR);
	DS1302_Time[0]=Temp;
	Temp=DS1302_ReadByte(DS1302_MONTH);
	DS1302_Time[1]=Temp;
	Temp=DS1302_ReadByte(DS1302_DATE);
	DS1302_Time[2]=Temp;
	Temp=DS1302_ReadByte(DS1302_HOUR);
	DS1302_Time[3]=Temp;
	Temp=DS1302_ReadByte(DS1302_MINUTE);
	DS1302_Time[4]=Temp;
	Temp=DS1302_ReadByte(DS1302_SECOND);
	DS1302_Time[5]=Temp;
	Temp=DS1302_ReadByte(DS1302_DAY);
	DS1302_Time[6]=Temp;
}

void DS1302_ReadTimeASCII(void)			//读取ASCII码到DS1302_Time数组
{
	unsigned char Temp;
	Temp=DS1302_ReadByte(DS1302_YEAR);
	DS1302_Time[0]=((Temp&0XF0)>>4)+0X30;
	DS1302_Time[1]=(Temp&0X0F)+0X30;
	Temp=DS1302_ReadByte(DS1302_MONTH);
	DS1302_Time[2]=((Temp&0XF0)>>4)+0X30;
	DS1302_Time[3]=(Temp&0X0F)+0X30;
	Temp=DS1302_ReadByte(DS1302_DATE);
	DS1302_Time[4]=((Temp&0XF0)>>4)+0X30;
	DS1302_Time[5]=(Temp&0X0F)+0X30;
	Temp=DS1302_ReadByte(DS1302_HOUR);
	DS1302_Time[6]=((Temp&0XF0)>>4)+0X30;
	DS1302_Time[7]=(Temp&0X0F)+0X30;
	Temp=DS1302_ReadByte(DS1302_MINUTE);
	DS1302_Time[8]=((Temp&0XF0)>>4)+0X30;
	DS1302_Time[9]=(Temp&0X0F)+0X30;
	Temp=DS1302_ReadByte(DS1302_SECOND);
	DS1302_Time[10]=((Temp&0XF0)>>4)+0X30;
	DS1302_Time[11]=(Temp&0X0F)+0X30;
}



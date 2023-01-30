#include <STC8.H>
#include <Delay.h>
#include <uart.h>
#include <ds1302.h>


//���Ŷ���
sbit DS1302_SCLK=P1^3;
sbit DS1302_IO=P1^2;
sbit DS1302_CE=P1^6;



//ʱ�����飬����0~6�ֱ�Ϊ�ꡢ�¡��ա�ʱ���֡��롢����
unsigned char DS1302_Time[13]={0x21,0x11,0x14,0x01,0x27,0x40,0x06,'\0'};

/**
  * @brief  DS1302��ʼ��
  * @param  ��
  * @retval ��
  */
void DS1302_Init(void)
{
	DS1302_CE=0;
	DS1302_SCLK=0;
}

/**
  * @brief  DS1302дһ���ֽ�
  * @param  Command ������/��ַ
  * @param  Data Ҫд�������
  * @retval ��
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
  * @brief  DS1302��һ���ֽ�
  * @param  Command ������/��ַ
  * @retval ����������
  */
unsigned char DS1302_ReadByte(unsigned char Command) 
{
	unsigned char i,Data=0x00;
	Command|=0x01;	//��ָ��ת��Ϊ��ָ��
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
	DS1302_IO=0;	//��ȡ��IO����Ϊ0��������������ݻ����
	return Data;
}

/**
  * @brief  DS1302����ʱ�䣬����֮��DS1302_Time��������ֻᱻ���õ�DS1302��
  * @param  ��
  * @retval ��
  */
void DS1302_SetTime(void)
{
	DS1302_WriteByte(DS1302_WP,0x00);
	DS1302_WriteByte(DS1302_YEAR,DS1302_Time[0]/10*16+DS1302_Time[0]%10);//ʮ����תBCD���д��
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
	DS1302_WriteByte(DS1302_YEAR,DS1302_Time[0]);//   BCD��д��
	DS1302_WriteByte(DS1302_MONTH,DS1302_Time[1]);
	DS1302_WriteByte(DS1302_DATE,DS1302_Time[2]);
	DS1302_WriteByte(DS1302_HOUR,DS1302_Time[3]);    //DS1302_Time[3] = DS1302_TimeBCD[0];
	DS1302_WriteByte(DS1302_MINUTE,DS1302_Time[4]);  //DS1302_Time[4] = DS1302_TimeBCD[1];
	DS1302_WriteByte(DS1302_SECOND,DS1302_Time[5]);	//DS1302_Time[5] = DS1302_TimeBCD[0];
	DS1302_WriteByte(DS1302_DAY,DS1302_Time[6]);
	DS1302_WriteByte(DS1302_WP,0x80);
}


/**
  * @brief  DS1302��ȡʱ�䣬����֮��DS1302�е����ݻᱻ��ȡ��DS1302_Time������
  * @param  ��
  * @retval ��
  */
void DS1302_ReadTime(void)
{
	unsigned char Temp;
	Temp=DS1302_ReadByte(DS1302_YEAR);
	DS1302_Time[0]=Temp/16*10+Temp%16;//BCD��תʮ���ƺ��ȡ
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


void DS1302_ReadTimeBCD(void)    //��ȡBCD�뵽DS1302_Time����
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

void DS1302_ReadTimeASCII(void)			//��ȡASCII�뵽DS1302_Time����
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



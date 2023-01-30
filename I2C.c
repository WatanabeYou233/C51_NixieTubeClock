#include <I2C.h>
#include <STC8.H>

void IIC_Init() //设置为主机模式
{
	P_SW2 = 0X80;
	
	P1PU = 0XFF;		//设置P1内部上拉电阻
	
	I2CCFG = 0Xe0;
	I2CMSST = 0X00;
}
void IIC_Wait()
{
	while(!(I2CMSST & 0X40));
	I2CMSST &= ~0X40;
}

void IIC_Start()
{
	I2CMSCR = 0X01;
	IIC_Wait();
}

void IIC_SendData(char dat)
{
	I2CTXD = dat;
	I2CMSCR = 0X02;
	IIC_Wait();
}

void IIC_RecvAck()
{
	I2CMSCR = 0X03;
	IIC_Wait();
}

char IIC_RecvData()
{
	I2CMSCR = 0X04;
	IIC_Wait();
	return I2CRXD;
}

void IIC_SendAck()
{
	I2CMSST = 0X00;
	I2CMSCR = 0X05;
	IIC_Wait();
}

void IIC_SendNak()
{
	I2CMSST = 0X01;
	I2CMSCR = 0X05;
	IIC_Wait();
}

void IIC_Stop()
{
	I2CMSCR = 0X06;
	IIC_Wait();
}


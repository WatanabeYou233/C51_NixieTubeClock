#ifndef __I2C__
#define __I2C__

#include <rega8.h>

sbit SDA = P1^4;
sbit SCL = P1^5;

void IIC_Init();
void IIC_Wait();
void IIC_Start();
void IIC_SendData(char dat);
void IIC_RecvAck();
char IIC_RecvData();
void IIC_SendAck();
void IIC_SendNak();
void IIC_Stop();


#endif
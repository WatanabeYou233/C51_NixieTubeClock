#ifndef __UART_H_
#define __UART_H_


#define len 25  //接收数组长度

void UartInit(void );
void Uart2Init(void);
void sendByte(unsigned char b);
void send2Byte(unsigned char b);
void sendStr(char *s);
void send2Str(char *s);
//unsigned char data_compare(unsigned char *p);

extern unsigned char xdata ReceiveData[len]; 

#endif
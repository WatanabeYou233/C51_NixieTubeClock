#include <I2C.h>
#include <Delay.h>
#include <codetab.h>



void Oled_Write_Cmd(unsigned char cmd)   //写命令
{
	IIC_Start();
	IIC_SendData(0X78);  //oled i2c address
	IIC_RecvAck();
	IIC_SendData(0X00);
	IIC_RecvAck();
	IIC_SendData(cmd);
	IIC_RecvAck();
	IIC_Stop();
}

void Oled_Write_Data(unsigned char DataWrite)  //写数据
{
	IIC_Start();
	IIC_SendData(0X78);  //oled i2c address
	IIC_RecvAck();
	IIC_SendData(0X40);
	IIC_RecvAck();
	IIC_SendData(DataWrite);
	IIC_RecvAck();
	IIC_Stop();
}

void OLED_Fill(unsigned char fill_Data)//全屏填充
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		Oled_Write_Cmd(0xb0+m);		//page0-page1
		Oled_Write_Cmd(0x00);		//low column start address
		Oled_Write_Cmd(0x10);		//high column start address
		for(n=0;n<128;n++)
			{
				Oled_Write_Data(fill_Data);
			}
	}
}

void OLED_Clean(void)//清屏
{
	OLED_Fill(0x00);
}


void OLED_Init() //初始化函数
{
	Delay1ms(200);
	IIC_Init();
	Oled_Write_Cmd(0xAE);   //display off
	Oled_Write_Cmd(0x20);	//Set Memory Addressing Mode	
	Oled_Write_Cmd(0x02);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	Oled_Write_Cmd(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
	Oled_Write_Cmd(0xc8);	//Set COM Output Scan Direction
	Oled_Write_Cmd(0x00);//---set low column address
	Oled_Write_Cmd(0x10);//---set high column address
	Oled_Write_Cmd(0x40);//--set start line address
	Oled_Write_Cmd(0x81);//--set contrast controlregister2
	Oled_Write_Cmd(0xff);
	Oled_Write_Cmd(0xa1);//--set segment re-map 0 to 127
	Oled_Write_Cmd(0xa6);//--set normal display
	Oled_Write_Cmd(0xa8);//--set multiplex ratio(1 to 64)
	Oled_Write_Cmd(0x3F);//
	Oled_Write_Cmd(0xa4);//0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	Oled_Write_Cmd(0xd3);//-set display offset
	Oled_Write_Cmd(0x00);//-not offset
	Oled_Write_Cmd(0xd5);//--set display clock divide ratio/oscillator frequency
	Oled_Write_Cmd(0xf0);//--set divide ratio
	Oled_Write_Cmd(0xd9);//--set pre-charge period
	Oled_Write_Cmd(0x22); //
	Oled_Write_Cmd(0xda);//--set com pins hardware configuration
	Oled_Write_Cmd(0x12);
	Oled_Write_Cmd(0xdb);//--set vcomh
	Oled_Write_Cmd(0x20);//0x20,0.77xVcc
	Oled_Write_Cmd(0x8d);//--set DC-DC enable
	Oled_Write_Cmd(0x14);//
	OLED_Clean();
	Oled_Write_Cmd(0xaf);//--turn on oled panel 
}


void OLED_SetPos(unsigned char x, unsigned char y)  //设置起始点坐标
{ 
	Oled_Write_Cmd(0xb0+y);
	Oled_Write_Cmd(((x&0xf0)>>4)|0x10);
	Oled_Write_Cmd((x&0x0f)|0x01);
}

//--------------------------------------------------------------
// Prototype      : void OLED_ON(void)
// Calls          : 
// Parameters     : none
// Description    : 将OLED从休眠中唤醒
//--------------------------------------------------------------
void OLED_ON(void)
{
	Oled_Write_Cmd(0X8D);  //设置电荷泵
	Oled_Write_Cmd(0X14);  //开启电荷泵
	Oled_Write_Cmd(0XAF);  //OLED唤醒
}

//--------------------------------------------------------------
// Prototype      : void OLED_OFF(void)
// Calls          : 
// Parameters     : none
// Description    : 让OLED休眠 -- 休眠模式下,OLED功耗不到10uA
//--------------------------------------------------------------
void OLED_OFF(void)
{
	Oled_Write_Cmd(0X8D);  //设置电荷泵
	Oled_Write_Cmd(0X10);  //关闭电荷泵
	Oled_Write_Cmd(0XAE);  //OLED休眠
}


//--------------------------------------------------------------
// Prototype      : void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
// Calls          : 
// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); ch[] -- 要显示的字符串; TextSize -- 字符大小(1:6*8 ; 2:8*16)
// Description    : 显示codetab.h中的ASCII字符,有6*8和8*16可选择
//--------------------------------------------------------------

void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize) 
{
	unsigned char c = 0,i = 0,j = 0;
	switch(TextSize)
	{
		case 1:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 126)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<6;i++)
					Oled_Write_Data(F6x8[c][i]);
				x += 6;
				j++;
			}
		}break;
		case 2:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 120)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<8;i++)
					Oled_Write_Data(F8X16[c*16+i]);
				OLED_SetPos(x,y+1);
				for(i=0;i<8;i++)
					Oled_Write_Data(F8X16[c*16+i+8]);
				x += 8;
				j++;
			}
		}break;
	}
}

//--------------------------------------------------------------
// Prototype      : void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);
// Calls          : 
// Parameters     : x0,y0 -- 起始点坐标(x0:0~127, y0:0~7); x1,y1 -- 起点对角线(结束点)的坐标(x1:1~128,y1:1~8)
// Description    : 显示BMP位图
//--------------------------------------------------------------
	
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[])
{
	unsigned int j=0;
	unsigned char x,y;

  if(y1%8==0)
		y = y1/8;
  else
		y = y1/8 + 1;
	for(y=y0;y<y1;y++)
	{
		OLED_SetPos(x0,y);
    for(x=x0;x<x1;x++)
		{
			Oled_Write_Data(BMP[j++]);
		}
	}
}





//--------------------------------------------------------------
// Prototype      : void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N)
// Calls          : 
// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); N:汉字在codetab.h中的索引
// Description    : 显示codetab.h中的汉字,16*16点阵
//--------------------------------------------------------------
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N)
{
	unsigned char wm=0;
	unsigned int  adder=32*N;
	OLED_SetPos(x , y);
	for(wm = 0;wm < 16;wm++)
	{
		Oled_Write_Data(F16x16[adder]);
		adder += 1;
	}
	OLED_SetPos(x,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		Oled_Write_Data(F16x16[adder]);
		adder += 1;
	}
}

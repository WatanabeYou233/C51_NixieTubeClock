#ifndef __oled12864__
#define __oled12864__

Oled_Write_Cmd(unsigned char cmd) ;
Oled_Send_Data(unsigned char DataWrite) ;
void OLED_Fill(unsigned char fill_Data);
void OLED_Init();
void OLED_SetPos(unsigned char x, unsigned char y) ;
void OLED_Clean(void);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize) ;
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N);
#endif
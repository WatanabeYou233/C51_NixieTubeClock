#include "DHT11.h"
#include "Uart.h"


#define	DHT11_DAT		P17

//����һ�������壬����ת����׽�Ĵ������������ 0-65535
static union 
{
	unsigned int U16;
	unsigned char U8[2];
}High;//��ʾ������ߵ�ƽ�жϺʹ���͵�ƽ�жϵ���ʱ����
static unsigned int xdata DHT11_Buffer[90];//���յ���������
static unsigned char DTH11_Buff_Count=0;//CCP�ж�������м�������


//�����ʱ���������ڷ���DHT11��ʼ�ź�ʹ��
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

//��ȡһ��DHT11������ ,�ɹ����� 0 ��ͨ��Э��ʧ�ܷ���1 ��У����󷵻�2
unsigned char Get_DHT11(DHT11_Data *Data)
{
	unsigned char i=0,j=0;
	
	DHT11_DAT	=	0;//ͨ������������25mS������һ��ͨ�ŵ���ʼ�ź�
	Delay_1ms(25);
	DHT11_DAT	=	1;//���������ߣ��ȴ�DHT11�������ݡ�����������ݣ�������CCP�жϴ���
	CL = 0;CH = 0;//����ʱ������
	CR = 1;
	DTH11_Buff_Count = 0;//�����������
	Delay_1ms(7);//�����ʱ7mS���ȴ�����������ݡ�����DHT11ʱ��ͼ�����յ�ʱ��һ��С�� 5344uS
	CR = 0;//�ر�PCA���У�������򿪽������յ�������
	for(i=0;i<90;i++)//���յ������壬���ﻻ���uS,��Ƶ30M�����Գ���30����1΢��
		DHT11_Buffer[i] /= 22;
	
	//����DHT11Э�飬���һ��83uS��Ӧ���һ��87uS�Ŀ�ʼ�źţ���һ���źž�������
	for(i=0;i<90;i++)
	{
		if((DHT11_Buffer[i]>78)&&(DHT11_Buffer[i+1]>78))
		{
			j=i+2;
			break;
		}
	}
	if(i > 90)//��ʾû�м�⵽��ʼ�źţ�ͨ��ʧ��
		return 1;
	i = j;
	j = 0;
	Data->Humi_H = 0;Data->Humi_L = 0;
	Data->Temp_H = 0;Data->Temp_L = 0;
	Data->Check_Sum = 0;
	for(i;i<90;i+=2)//����Ч���ݿ�ʼ����
	{
		if(j < 8)
		{
			Data->Humi_H <<= 1;
			if(DHT11_Buffer[i]<DHT11_Buffer[i+1])//����DHT11Э�飬���ʾ��1������ 
				Data->Humi_H++;
		}
		if((j > 7) && (j < 16))
		{
			Data->Humi_L <<= 1;
			if(DHT11_Buffer[i]<DHT11_Buffer[i+1])//����DHT11Э�飬���ʾ��2������ 
				Data->Humi_L++;
		}
		if((j > 15) && (j < 24))
		{
			Data->Temp_H <<= 1;
			if(DHT11_Buffer[i]<DHT11_Buffer[i+1])//����DHT11Э�飬���ʾ��3������ 
				Data->Temp_H++;
		}
		if((j > 23) && (j < 32))
		{
			Data->Temp_L <<= 1;
			if(DHT11_Buffer[i]<DHT11_Buffer[i+1])//����DHT11Э�飬���ʾ��4������ 
				Data->Temp_L++;
		}
		if((j > 31) && (j < 40))
		{
			Data->Check_Sum <<= 1;
			if(DHT11_Buffer[i]<DHT11_Buffer[i+1])//����DHT11Э�飬���ʾ��5������ 
				Data->Check_Sum++;
		}
		j++;
	}
	//����У�飬���������2
	if((Data->Humi_H + Data->Humi_L + Data->Temp_H + Data->Temp_L) == Data->Check_Sum)
		return 0;//�ɹ���ȡ����
	else
		return 2;
}
//��ʼ��CCP0  ʹ��CCP0�ܽţ����������źţ����ò����ܣ�����ߵ�ƽ��Ⱥ͵͵�ƽ���     ��������A4��ĸ
void Init_CCP0(void)
{
	unsigned char i=0;
	//P_SW1 = 0X20;//CCP ȫ���ֲ���_3
	CF = 0;//PCA���������������־λ
	CR = 0;//PCA���������п���
	CCF2 = 0;//PCA2�жϱ�־λ
	CCF1 = 0;//PCA1�жϱ�־λ
	CCF0 = 0;//PCA0�жϱ�־λ
  CMOD = 0x08;//����PCAʱ��Դ&��ֹPCA��ʱ������ж�    ʱ��Դ�� 1/1 ����ʱ��30MHZ����ôCCPʱ����30MHZ ��ÿ30������1uS       
	CCAPM0 = 0X71;//����CCP0�Ĳ���������ģʽ���������½��ض���������CCP0�ж�
	CL = 0;CH = 0;//��λPCA�Ĵ���
	CCAP0L = 0;CCAP0H = 0;//��λ��׽�Ĵ���
	CR = 0;//PCA���п���
	for(i=0;i<90;i++)//���յ�����������
		DHT11_Buffer[i] = 0;
}

//��һ���ṹ���������ʪ�����ݣ�ת��Ϊ����ʶ����ַ���
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
	if((Data->Temp_L & 0X80) == 0X80)//��ʾ���ܵ����¶��Ǹ���
	{
		Data->Temp_L &= 0X7F;
		Str_DHT11[5] = '-';
	}
	else//���յ����¶�������
	{
		Str_DHT11[5] = '+';
	}
	Str_DHT11[9] = Data->Temp_L % 10 + 0X30;
	Str_DHT11[10] = ' ';
	
	Str_DHT11[11] = 0X0D;
	Str_DHT11[12] = 0X0A;
	
	Str_DHT11[13] = 0;
}
//PCA�ж���ڣ���������һ���ж���Ҫ�жϱ�־λ
void PCA_isr(void) interrupt 7
{
	CL = 0;CH = 0;//��λ��׽�Ĵ���
	High.U8[0] = CCAP0H;
	High.U8[1] = CCAP0L;
	DHT11_Buffer[DTH11_Buff_Count++] = High.U16;//�Ѷ����Ĳ������ݸ�����������
	CCF0 = 0;//���ڳ������ʹ����һ��CCF0�����Խ����жϺ����ж�
}
//




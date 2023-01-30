//������ʪ�ȴ����� DHT11�Ķ�ȡ   ʹ��CCP���ܣ����մ��������͵ĵ�ƽ�źŵ����飬Ȼ��ʼ����
#ifndef __DHT11_H__
#define __DHT11_H__
#include <STC8.H>
extern unsigned char Str_DHT11[16];//һ֡��Ч���ݣ�ASICII��ʽ�ġ�15����Ч���ݡ�

/*����ṹ�壬�����ȡ���¶�ֵ*/
typedef struct
{
	unsigned char Humi_H;		//ʪ�ȵ���������
	unsigned char Humi_L;	 	//ʪ�ȵ�С������
	unsigned char Temp_H;	 	//�¶ȵ���������
	unsigned char Temp_L;	 	//�¶ȵ�С������
	unsigned char Check_Sum;	 	//У���
} DHT11_Data;

//��ʼ��CCP0  ʹ��CCP0�ܽţ����������źţ����ò����ܣ�����ߵ�ƽ��Ⱥ͵͵�ƽ���
void Init_CCP0(void);
//��ȡһ��DHT11������ ,�ɹ����� 0 ��ͨ��Э��ʧ�ܷ���1 ��У����󷵻�2
unsigned char Get_DHT11(DHT11_Data *Data);
//��һ���ṹ���������ʪ�����ݣ�ת��Ϊ����ʶ����ַ���
void Data_To_Str(DHT11_Data *Data);

#endif
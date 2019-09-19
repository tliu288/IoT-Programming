#include "nbiot.h"
#include "stm32f10x.h"
#include "usart.h"	 
#include "crc16.h"
#include "string.h"

extern u16 USART2_RX_STA;//��usart.c�ж���
extern u8 USART2_RX_BUF[USART_REC_LEN];//��usart.c�ж���

//ͨ������2����ATָ���NB-IoTģ��
void SendData(u16 sdata)
{
	int i = 0;
	char t[50];
	char data[10];
	char datastr[21];
	char res[5];
	
	//��ʼ��t
	for(i = 0; i < 50; i++)
	{
		t[i] = '\0';
	}
	
	//׼������
	data[0] = 0x01;//���豸��ַ
	data[1] = 0x46;//�Ĵ�����Ԫ����
	data[2] = 0x00;//�Ĵ�����Ԫ����
	data[3] = 0x00;
	data[4] = 0x00;
	data[5] = 0x01;
	data[6] = 0x02;//���ݳ���
	data[7] = (u8)((sdata & 0xFF00)>> 8);//����
	data[8] = (u8)(sdata & 0x00FF);
	
	//��ȡ�ֽ����ݵ��ַ�����ʽ
	getString(data, 9, datastr);
	//��ȡ�ֽ����ݵ�crc16У����
	getCrc16(data, 9, res);
	
	//�ַ���ƴ��
	strcpy(t, "AT+NMGS=11,");
	strcat(t, datastr);
	strcat(t, res);
	strcat(t, "\r\n");
	
	//����
	Usart_SendString(USART2, t);
}

//����ATָ��,���ͳɹ�����1������ʧ�ܷ���-1
int sendATCmd(char * cmd)
{
	int j = 0, flag = 0;
	do
	{
		Usart_SendString(USART2, cmd);
		//�ȴ����ճɹ����Ӧ�Ľ��
		delay_ms(3000);
		printf("send status:");
		//�ж��Ƿ��ͳɹ�
		while(j < USART2_RX_STA)
		{
			printf("%c", USART2_RX_BUF[j]);
			if((USART2_RX_BUF[j-1] == 'O' || USART2_RX_BUF[j-1] == 'o') && (USART2_RX_BUF[j] == 'K' || USART2_RX_BUF[j] == 'k'))
			{
				flag = 1;//���ͳɹ�
				USART2_RX_STA = 0;
				j = 0;
				break;
			}
			if(((USART2_RX_BUF[j-1] == 'O' || USART2_RX_BUF[j-1] == 'o') && (USART2_RX_BUF[j] == 'R' || USART2_RX_BUF[j] == 'r')))
			{
				flag = -1;//����ʧ��
				USART2_RX_STA = 0;
				j = 0;
				break;
			}
			j++;
		}
		
		printf("\r\n");
	}while(flag == 0);
	return flag;
}

//������ʱָ��ģʽ
void IntoTempCmdMode()
{
	int j = 0, flag = 0;
	
	//ȷ���˳���ʱָ��ģʽ
	while(sendATCmd("AT+ENTM\r\n") == 1);
	
	do
	{
		Usart_SendString(USART2, "+++");
		delay_ms(1000);
		
		//�ж��Ƿ��ͳɹ�
		while(j < USART2_RX_STA)
		{
			if(USART2_RX_BUF[j] == 'a')
			{
				USART2_RX_STA = 0;
				j = 0;
				break;
			}
			j++;
		}
		
		Usart_SendString(USART2, "a");
		delay_ms(1000);
		//�ж��Ƿ��ͳɹ�
		while(j < USART2_RX_STA)
		{
			if((USART2_RX_BUF[j-1] == 'O' || USART2_RX_BUF[j-1] == 'o') && (USART2_RX_BUF[j] == 'K' || USART2_RX_BUF[j] == 'k'))
			{
				flag = 1;//���ͳɹ�
				USART2_RX_STA = 0;
				j = 0;
				break;
			}
			j++;
		}
	}while(flag == 0);
	printf("NBģ�������ʱָ��ģʽ...\r\n");
}


void NB_Init()
{
	int i = 0;
	//������ʱָ��ģʽ
	IntoTempCmdMode();
	
	//����
	while(sendATCmd("AT+Z\r\n") == -1);
	
	printf("NBģ�����������ɹ�...\r\n");
}

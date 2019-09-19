#include "stm32f10x.h"

//���ڶ���ʼ������
void usart2_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//����GPIOʱ�Ӻʹ���2ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	//���ô���2����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//PA2:����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3:����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//���ڲ�������
	USART_InitStructure.USART_BaudRate = 9600;//������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_Init(USART2, &USART_InitStructure);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�,�жϴ��������ڽ�������
  USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 
	
	//NVIC�ڲ��ж�������������
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
}

//����һ���ַ�
void Usart_SendByte(USART_TypeDef* pUSARTx, uint8_t ch)
{
	//����һ���ֽ����ݵ�USART
	USART_SendData(pUSARTx, ch);
	
	//�ȴ��������ݼĴ���Ϊ��
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}

//����һ���ַ���
void Usart_SendString(USART_TypeDef* pUSARTx, char* str)
{
	unsigned int k = 0;
	do
	{
		//����ֽڷ���
		Usart_SendByte(pUSARTx, *(str + k));
		k++;
	}while(*(str + k) != '\0');
	
	//�ȴ��������
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET);
}

#include "stm32f10x.h"


//��һ���ֽ�����ת��Ϊ�����ַ�
char byteToChar(u8 b)
{
	char res[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
	
	return (char)res[b];
}

/* ���ֽ�����ת��Ϊ�ַ���
* data���ֽ�����
* str���ַ����飨�����
*/
void getString(u8 *data, int len, char *str)
{
	u8 i;
	u8 res1,res2;
	for(i = 0; i < len; i++)
	{
		res1 = ((data[i] & 0xF0) >> 4);
		res2 = data[i] & 0x0F;
		
		str[2*i] = byteToChar(res1);
		str[2*i+1] = byteToChar(res2);
	}
	str[2 * len] = '\0';
}


/* ��ȡcrc16����������֡
* data:�Ѿ����õ�����֡����Ҫ����У����
* crcstr:crc16���������ɵ��ַ�������
*/
void getCrc16(u8 *data, int len, char* crcstr)
{
	//����crc16�����ɶ���ʽ
	u16 gx = 0xA001;
	u8 i, j;
	u16 crcreg = 0xFFFF;
	u8 d[2];
	
	for(i = 0; i < len; i++)
	{
		crcreg = (u16)(crcreg ^ data[i]);
		for(j = 0; j < 8; j++)
		{
			crcreg = (crcreg & 1) != 0 ? (u16)((crcreg >> 1) ^ gx) : (u16)(crcreg >> 1);;
		}
	}
	
	d[0] = (u8)(crcreg & 0x00FF);
	d[1] = (u8)((crcreg & 0xFF00) >> 8);
	getString(d, 2, crcstr);
}
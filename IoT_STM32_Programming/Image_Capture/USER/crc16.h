#ifndef CRC16_H
#define CRC16_H
#include "stm32f10x.h"

/* ���ֽ�����ת��Ϊ�ַ���
* data���ֽ�����
* str���ַ����飨�����
*/
void getString(u8 *data, int len, char *str);

/* ��ȡcrc16����������֡
* data:�Ѿ����õ�����֡����Ҫ����У����
* crcstr:crc16���������ɵ��ַ�������
*/
void getCrc16(u8 *data, int len, char* crcstr);



#endif
#ifndef NB_IOT_H
#define NB_IOT_H
#include "stm32f10x.h"

void NB_Init();

//����ATָ��,���ͳɹ�����1������ʧ�ܷ���-1
int sendATCmd(char * cmd);

//ͨ������2����ATָ���NB-IoTģ��
void SendData(u16 sdata);

#endif
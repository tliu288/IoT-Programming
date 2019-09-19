#ifndef __IMAGEPROCESS__
#define __IMAGEPROCESS__
#include "ff.h"
#include "stm32f10x.h"

/* ͼ��ҶȻ�
 * ˼·�����������ļ�ָ�룬
 * ��һ���ļ�ָ�����ڶ�ȡԭͼ���е�����ֵ��
 * �ڶ�������ָ���д����ļ�
 * RGB565��ʽ����
			bmp.RGB_MASK[0] = 0X00F800;
			bmp.RGB_MASK[1] = 0X0007E0;
			bmp.RGB_MASK[2] = 0X00001F;
 */
void Graying(const TCHAR* src, const TCHAR* dist);

//Otsu��ֵ���㷨
/*
 * ˼·�����������ļ�ָ�룬
 * ��һ���ļ�ָ�����ڶ�ȡԭͼ���е�����ֵ��
 * �ڶ�������ָ���д����ļ�
 * f3:�����ļ�
 */
void Otsu(const TCHAR* src, const TCHAR* dist);

/* BP������ʶ������
 * ˼·����֪Ȩֵ����w��v���Լ���ֵ����b1��b2
 * ��Ҫʹ���ĸ��ļ���
 *	��1���ļ�ָ��ָ��Ȩֵ����w
 * 	��2������ָ��ָ��Ȩֵ����v
 * 	��3������ָ��ָ��ֵ����b1
 * 	��4������ָ��ָ��ֵ����b2
 * ����srcΪ�ָ��ĵ�����ʶ��ͼƬ��·��
 */
u8 BP_Recongnization(const TCHAR* src);



#endif
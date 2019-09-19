#include "ff.h"
#include "bmp.h"
#include "math.h"
#include "stdio.h"
#include "imageprocess.h"

//������ɫ����
#define BLACK 0x0000
#define WHITE 0xFFFF

//�ļ�ָ�루һ��Ҫ�������ⲿ��������ܳ��ֶ�ջ������������������
FIL f1, f2, f3, f4, f5, f6, f7, f8;
BITMAPINFO bmp;

/* ͼ��ҶȻ�
 * ˼·�����������ļ�ָ�룬
 * ��һ���ļ�ָ�����ڶ�ȡԭͼ���е�����ֵ��
 * �ڶ�������ָ���д����ļ�
 * RGB565��ʽ����
			bmp.RGB_MASK[0] = 0X00F800;
			bmp.RGB_MASK[1] = 0X0007E0;
			bmp.RGB_MASK[2] = 0X00001F;
 */
void Graying(const TCHAR* src, const TCHAR* dist)
{
	UINT num = 0;
	u16 i,j;
	u16 color;
	u8 r,g,b,ret;
	u16 width, height;
	u8 t = 0;
	u8 res1, res2;
	
	//���ļ�
	do
	{
		res1 = f_open(&f1, src, FA_READ | FA_WRITE);
		res2 = f_open(&f2, dist, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
	}while((FR_OK != res1) || (FR_OK != res2));
	
	f_read(&f1, &bmp, sizeof(bmp), &num);
	f_write(&f2, &bmp, sizeof(bmp), &num);
		
	height = bmp.bmiHeader.biHeight;
	width = bmp.bmiHeader.biWidth;
	
	for(i = 0; i < width; i++)
	{
		for(j = 0; j < height; j++)
		{
			f_read(&f1, &color, sizeof(color), &num);
			r = (color & 0XF800) >> 11;
			g = (color & 0X07E0) >> 6;
			b = color & 0X001F;
			ret = r * 0.299 + g * 0.587 + b * 0.114;
			color = (ret << 11) + (ret << 6) + ret;
			f_write(&f2, &color, sizeof(color), &num);
		}
	}
	//�ر��ļ�
	f_close(&f2);
	f_close(&f1);
}

//Otsu��ֵ���㷨
/*
 * ˼·�����������ļ�ָ�룬
 * ��һ���ļ�ָ�����ڶ�ȡԭͼ���е�����ֵ��
 * �ڶ�������ָ���д����ļ�
 * f3:�����ļ�
 */
void Otsu(const TCHAR* src, const TCHAR* dist)
{
	UINT num = 0;
	u32 i,j;
	u16 color;
	u8 gray;
	u16 width, height;
	u8 t = 0;
	//�ļ��򿪱�־
	u8 res1, res2, res3; 
	u32 dcount = 0;//���ݼ���
	int Th = 0;//��ֵ����
	//���屳����Ŀ��������Ŀ����N1,N2���Ҷȱ���U1,U2
	int N1 = 0, N2 = 0;
	//�ҶȺͱ���Sum1,Sum2
  int Sum1 = 0, Sum2 = 0;
	//ͼ������ƽ���Ҷȱ���U
  double U1 = 0, U2 = 0;
	//�������gr���Ա���ֵ����TT
  long long g = 0, TT = 0;
	
	//���ļ�
	do
	{
		res1 = f_open(&f1, src, FA_READ | FA_WRITE);
		res2 = f_open(&f2, dist, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
		res3 = f_open(&f3, "datafile.dat", FA_CREATE_ALWAYS | FA_READ | FA_WRITE);//ÿ�δ򿪶��½�,������ʱ��ŻҶ����ݣ�һ���ֽڴ���һ���Ҷ�ֵ��
	}while((FR_OK != res1) || (FR_OK != res2) || (FR_OK != res3));
	
	f_read(&f1, &bmp, sizeof(bmp), &num);
	f_write(&f2, &bmp, sizeof(bmp), &num);
	
	height = bmp.bmiHeader.biHeight;
	width = bmp.bmiHeader.biWidth;
	dcount = height * width;
	
	//��ȡ�Ҷ�ֵ����д�������ļ�
	for(i = 0; i < width; i++)
	{
		for(j = 0; j < height; j++)
		{
			f_read(&f1, &color, sizeof(color), &num);
			gray = color & 0X001F;
			f_write(&f3, &gray, sizeof(gray), &num);
		}
	}
	
	//Ѱ�������䷽��
	for (i = 0; i <= 255; i++)
  {
		//ͳ���������Ԫ�ظ��������ػҶȺ�
		for (j = 0; j < dcount; j++)
		{
			//��ȡ��j���ֽڵ�����
			f_lseek(&f3, j);
			f_read(&f3, &gray, sizeof(gray), &num);
			if(gray > i)
			{
				N2++;
				Sum2 += gray;
			}
			else
			{
				N1++;
				Sum1 += gray;
			}
		}
		U1 = (N1 == 0 ? 0.0 : (Sum1 / N1));//��1���ؾ�ֵ
		U2 = (N2 == 0 ? 0.0 : (Sum2 / N2));
		g = N1 * N2 * (U1 - U2) * (U1 - U2);//������������֮��ķ���
		if (g > TT)//�������ķ���
		{
			TT = g;//�������ķ���
			Th = i;//�Դ�ʱ�ĻҶ�ֵΪ��ֵ
		}
		N1 = 0; N2 = 0;
		Sum1 = 0; Sum2 = 0; U1 = 0.0; U2 = 0.0; g = 0.0;
	}
	
	f_lseek(&f3, 0);
	//��ȡ�Ҷ�ֵ����д�������ļ�
	for(i = 0; i < width; i++)
	{
		for(j = 0; j < height; j++)
		{
			f_read(&f3, &gray, sizeof(gray), &num);
			if(gray < Th)
			{
				color = BLACK;
			}
			else
			{
				color = WHITE;
			}
			f_write(&f2, &color, sizeof(color), &num);
		}
	}
	
	//�ر��ļ�
	f_close(&f2);
	f_close(&f1);
	f_close(&f3);
}

/* ��txt�ļ��ж�ȡһ������������
 * fp: Pointer to the blank file object
 */
void readDouble(FIL *fp, double* ret)
{
	u8 s[2];
	UINT rc;
	char p[30];
	int i = 0, j = 0;
	double tmp1 = 0;
	double tmp2 = 0;
	double sum1 = 0;
	double sum2 = 0;
	int count = 0;
	
	//��ʼ��
	for(i = 0; i < 30; i++)
	{
		p[i] = ' ';
	}
	
	i = 0;
	//1.��ȡ���е�����
	do
	{
		f_read(fp, s, 1, &rc);
		if(rc != 1) //EOF�����
		{
			break;
		}
		
		//���з�����
		if(s[0] == '\n' || s[0] == '\r')
		{
			continue;
		}
		
		p[i] = s[0];
		if (p[i] == ' ')
		{
			break;
		}
		i++;
	}while(1);
	i = 0;
	//2.���ַ�ת��Ϊ������
	if(p[0] == '-')
	{
		i = 1;
		//1.��ȡ�������֣�ֱ������.����ѭ��
		while(p[i] != '.')
		{
			tmp1 = p[i] - '0';
			sum1 *= 10;//sum��ǰ��λ
			sum1 += tmp1;//���ϵ�ǰλ
			i++;
		}
		i++;//����.
		//2.��ȡС������
		while(p[i] != ' ')
		{
			count++;
			tmp2 = p[i] - '0';
			j = 0;
			while(j < count)
			{
				tmp2 *= 0.1;
				j++;
			}
			sum2 += tmp2;
			i++;
		}
		//3.�ϳɸ�����
		*ret = -(sum1 + sum2);
	}
	else
	{
		//1.��ȡ�������֣�ֱ������.����ѭ��
		while(p[i] != '.')
		{
			tmp1 = p[i] - '0';
			sum1 *= 10;//sum��ǰ��λ
			sum1 += tmp1;//���ϵ�ǰλ
			i++;
		}
		i++;//����.
		//2.��ȡС������
		while(p[i] != ' ')
		{
			count++;
			tmp2 = p[i] - '0';
			j = 0;
			while(j < count)
			{
				tmp2 *= 0.1;
				j++;
			}
			sum2 += tmp2;
			i++;
		}
		//3.�ϳɸ�����
		*ret = sum1 + sum2;
	}
}

/* ��txt�ļ��������ȡһ������������
 * n: �ڼ�������������
 */
void readDoubleRandom(FIL *fp, double* ret, int n)
{
	u8 s[2];
	UINT rc;
	int count = 0;
	f_lseek(fp, 0);//�ض�λ���ļ���ͷ
	while(count < n)
	{
		f_read(fp, s, 1, &rc);
		if(rc != 1) //EOF�����
		{
			break;
		}
		if (s[0] == ' ')
		{
			count++;
		}
	}
	readDouble(fp, ret);
}

/* BP������ʶ������
 * ˼·����֪Ȩֵ����w��v���Լ���ֵ����b1��b2
 * ��Ҫʹ���ĸ��ļ���
 *	��1���ļ�ָ��ָ��Ȩֵ����w
 * 	��2������ָ��ָ��Ȩֵ����v
 * 	��3������ָ��ָ��ֵ����b1
 * 	��4������ָ��ָ��ֵ����b2
 * ����srcΪ�ָ��ĵ�����ʶ��ͼƬ��·��
 */
u8 BP_Recongnization(const TCHAR* src)
{
	//�ļ��򿪱�־
	u8 res1, res2, res3, res4, res5, res6, res7, res8;
	u8 color[4];
	u8 gray;
	double pMax;//���ĻҶ�ֵ
	double tmp;
	UINT num = 0;
	double d;
	u32 height, width, i, j;
	int hideNum, inNum, outNum, t;
	double t1, t2, t3, t4, t5;
	
	double max;
	int maxi = 0;
	
	//���ļ�
	do
	{
		res1 = f_open(&f1, "0:BP/w.txt", FA_READ);
		res2 = f_open(&f2, "0:BP/v.txt", FA_READ);
		res3 = f_open(&f3, "0:BP/b1.txt", FA_READ);
		res4 = f_open(&f4, "0:BP/b2.txt", FA_READ);
		res5 = f_open(&f5, "0:BP/x1.dat", FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
		res6 = f_open(&f6, "0:BP/x2.dat", FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
		res7 = f_open(&f7, src, FA_READ);
		res8 = f_open(&f8, "0:BP/x.dat", FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
	}while((FR_OK != res1) || (FR_OK != res2) || (FR_OK != res3) || (FR_OK != res4) || (FR_OK != res5) || (FR_OK != res6) || (FR_OK != res7) || (FR_OK != res8));
	//1.���ݹ�һ��
	f_read(&f7, &bmp, sizeof(bmp), &num);
	
	height = bmp.bmiHeader.biHeight;
	width = bmp.bmiHeader.biWidth;
	inNum = height * width;
	outNum = 10;
	t = sqrt(0.43*inNum*outNum + 0.12*outNum*outNum + 2.54*inNum + 0.77*outNum + 0.35) + 0.51;//������ڵ���������ڵ�������õ�
	hideNum = (int)(t);
	hideNum = (t - hideNum * 1.0) > 0.5 ? hideNum +1 : hideNum;
	
	//��ӡλͼ��Ϣ
	//printfBmpFileInfo(bmp.bmfHeader);
	//printfBmpInfo(bmp.bmiHeader);
	
	//��λ��λͼ��������
	f_lseek(&f7, bmp.bmfHeader.bfOffBits); 
	//�������ֵ
	for(i = 0; i < height; i++)
	{
		for(j = 0; j < width; j++)
		{
			f_read(&f7, &color, sizeof(color), &num);//����
			gray = color[0];
			if(gray > pMax)
			{
				pMax = gray;
			}
		}
	}
	
	//��λ��λͼ��������
	f_lseek(&f7, bmp.bmfHeader.bfOffBits);
	//�������ֵ��һ����ͬʱ���浽һ����ʱ���ļ���
	for(i = 0; i < height; i++)
	{
		for(j = 0; j < width; j++)
		{
			f_read(&f7, &color, sizeof(color), &num);//����
			gray = color[0];//r
			
			tmp = gray / pMax;
			//printf("%f ", tmp);
			f_write(&f8, &tmp, sizeof(tmp), &num);//x
		}
	}
	
	//2.�������ز���������
	for(i = 0; i < hideNum; i++)
	{
		tmp = 0.0;
		f_lseek(&f8, 0);
		for(j = 0; j < inNum; j++)
		{
			//��ȡһ��˫���ȸ���������
			readDouble(&f1, &t1);//w
			f_read(&f8, &t2, sizeof(t2), &num);//x
			
			tmp += t1 * t2;
			
		}
		readDouble(&f3, &t3);//b1
		
		t5 = 1.0 / (1.0 + exp(-t3 - tmp));
		f_write(&f5, &t5, sizeof(t5), &num);//x1
	}
	
	f_lseek(&f5, 0);
	//3.�����������������
	for(i = 0; i < outNum; i++)
	{
		tmp = 0.0;
		f_lseek(&f5, 0);
		for(j = 0; j < hideNum; j++)
		{
			//��ȡһ��˫���ȸ���������
			readDouble(&f2, &t1);//v
			f_read(&f5, &t2, sizeof(t2), &num);//x1
			
			tmp += t1 * t2;
		}
		readDouble(&f4, &t3);//b2
		t5 = 1.0 / (1.0 + exp(-t3 - tmp));
		f_write(&f6, &t5, sizeof(t5), &num);//x2
	}
	
	f_lseek(&f6, 0);
	//4.����������ֵ���ڵĽڵ�
	f_read(&f6, &max, sizeof(max), &num);//x2
	for(i = 1; i < outNum; i++)
	{
		f_read(&f6, &tmp, sizeof(tmp), &num);//x2
		if(tmp > max)
		{
			max = tmp;
			maxi = i;
		}
	}
	//�ر��ļ�
	f_close(&f1);
	f_close(&f2);
	f_close(&f3);
	f_close(&f4);
	f_close(&f5);
	f_close(&f6);
	f_close(&f7);
	f_close(&f8);
	return maxi;
}
#ifndef _RTWDOG_H
#define _RTWDOG_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//RTWDOG��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2012/12/19
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	


void MYRTWDOG_Init(u8 src,u8 pscen,u16 toval,u16 win);
void RTWDOG_Feed(void);
#endif

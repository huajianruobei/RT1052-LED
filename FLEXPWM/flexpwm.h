#ifndef _FLEXPWM_H
#define _FLEXPWM_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//FLEXPWM��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/1/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

void PWM2_SM3_PWMAB_Init(u16 psc,u32 fre,u8 duty);	//PWM2 SM3 PWM�����ʼ������
void PWM2_SM3_DutySet(u8 duty);     //����PWM2_SM3����ͨ����ռ�ձ�
#endif


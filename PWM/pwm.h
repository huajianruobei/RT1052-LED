#ifndef _PWM_H
#define _PWM_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//PWM��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/1/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//ͨ���ı�TMR4->CHANNEL[3].CMPLD1��ֵ���ı�͵�ƽ��ռ�ձȣ��Ӷ�����LED1������
#define LED1_PWM_VAL TMR4->CHANNEL[3].CMPLD1

void QTMR4_CH3_PWM_Init(u8 prisrc,u32 clk, u8 duty);
void QTMER4CH3_PWM_DutySet(u8 prisrc,u32 clk, u8 duty);
#endif


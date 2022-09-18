#ifndef _PWM_H
#define _PWM_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK I.MX RT1052开发板
//PWM驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/1/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//通过改变TMR4->CHANNEL[3].CMPLD1的值来改变低电平的占空比，从而控制LED1的亮度
#define LED1_PWM_VAL TMR4->CHANNEL[3].CMPLD1

void QTMR4_CH3_PWM_Init(u8 prisrc,u32 clk, u8 duty);
void QTMER4CH3_PWM_DutySet(u8 prisrc,u32 clk, u8 duty);
#endif


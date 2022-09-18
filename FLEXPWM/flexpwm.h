#ifndef _FLEXPWM_H
#define _FLEXPWM_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK I.MX RT1052开发板
//FLEXPWM驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/1/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

void PWM2_SM3_PWMAB_Init(u16 psc,u32 fre,u8 duty);	//PWM2 SM3 PWM输出初始化函数
void PWM2_SM3_DutySet(u8 duty);     //设置PWM2_SM3两个通道的占空比
#endif


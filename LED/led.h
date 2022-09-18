#ifndef _LED_H
#define _LED_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK I.MX RT1052开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2012/12/10
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#define LED0(n)		(n?GPIO_PinWrite(GPIO1,3,1):GPIO_PinWrite(GPIO1,3,0))
#define LED0_Toggle (GPIO1->DR ^= (1<<3)) 	//LED0输出电平翻转

#define LED1(n)		(n?GPIO_PinWrite(GPIO3,3,1):GPIO_PinWrite(GPIO3,3,0))
#define LED1_Toggle (GPIO3->DR ^= (1<<3)) 	//LED1输出电平翻转


void LED_Init(void);
#endif

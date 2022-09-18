#ifndef _WDOG_H
#define _WDOG_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK I.MX RT1052开发板
//看门狗驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2012/12/19
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

void WDOG1_Init(u8 wdgtimeout, u8 inttimeout);
void WDOG1_Feed(void);
#endif


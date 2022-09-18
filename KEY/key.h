#ifndef _KEY_H
#define _KEY_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK I.MX RT1052开发板
//按键驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2012/12/15
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

/*******************************************  
    KEY_UP    ->  WAKEUP(GPIO5_IO00)
    KEY0      ->  GPIO_AD_B0_05(GPIO1_IO05)
    KEY1      ->  PMIC_ON_REQ(GPIO5_IO01)
    KEY2      ->  GPIO_EMC_40(GPIO3_IO26)
*******************************************/

#define KEY0	GPIO_PinRead(GPIO1,5)  //KEY0按键GPIO1_05
#define KEY1	GPIO_PinRead(GPIO5,1)  //KEY1按键GPIO5_01
#define KEY2	GPIO_PinRead(GPIO3,26) //KEY2按键GPIO3_26
#define WK_UP	GPIO_PinRead(GPIO5,0)  //WK_UP按键GPIO5_00

#define KEY0_PRES 	1  	//KEY0按下后返回值
#define KEY1_PRES	2	//KEY1按下后返回值
#define KEY2_PRES	3	//KEY2按下后返回值
#define WKUP_PRES   4	//WKUP按下后返回值

void KEY_Init(void);
u8 KEY_Scan(u8 mode);
#endif

#include "wdog.h"
#include "lpuart.h"
#include "led.h"
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

wdog_config_t wdog1_config; //WDOG配置结构体
 
//初始化WDOG1
//wdgtimeout:看门狗超时时间，实际时间为(wdgtimeout+1)*0.5s
//			 比如如果要设置超时时间为2s的话wdgtimeout=2/0.5-1=3
//inttimeout:在看门狗超时发生前几秒产生中断？实际时间为inttimeout*0.5s
void WDOG1_Init(u8 wdgtimeout, u8 inttimeout)
{
	//WDOG_Init()函数里面会使能看门狗时钟，这里写出来是提醒大家使用看门狗
	//的时候看门狗时钟是要使能的。
	CLOCK_EnableClock(kCLOCK_Wdog1); 			//使能看门狗时钟
	
	WDOG_GetDefaultConfig(&wdog1_config);		//先将WDOG1配置为默认值
	wdog1_config.workMode.enableWait=false;		//等待模式关闭
	wdog1_config.workMode.enableStop=false;		//stop模式关闭wdog
	wdog1_config.workMode.enableDebug=false;	//debug模式关闭wdog	
	wdog1_config.timeoutValue=wdgtimeout;		//设置超时值，周期为(wdgtimeout+1)*0.5
	wdog1_config.interruptTimeValue=inttimeout;	//设置wdog超时前几秒产生中断? inttimeout*0.5
	wdog1_config.enableInterrupt=true;			//使能wdog中断
	wdog1_config.enableWdog=true;				//使能wdog
	WDOG_Init(WDOG1,&wdog1_config);				//初始化看门狗
	
	//配置WDOG中断
	RT1052_NVIC_SetPriority(WDOG1_IRQn,4,0);	//抢占优先级4，子优先级0
	EnableIRQ(WDOG1_IRQn);						//使能WDOG1中断
}

//喂狗
void WDOG1_Feed(void)
{  
	WDOG_Refresh(WDOG1);//重装载	
}

//看门狗中断服务函数
void WDOG1_IRQHandler(void)
{
	if(WDOG_GetStatusFlags(WDOG1) & kWDOG_InterruptFlag)	//超时要发生
	{
        WDOG1_Feed();	//喂狗
        LED1_Toggle;
	}
	WDOG_ClearInterruptStatus(WDOG1,kWDOG_InterruptFlag);	//清除中断标志
	__DSB();				//数据同步屏蔽指令
}

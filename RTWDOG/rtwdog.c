#include "rtwdog.h"
#include "lpuart.h"
#include "delay.h"
#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK I.MX RT1052开发板
//RTWDOG驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/4/25
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

rtwdog_config_t rtwdog_config;	//RTWDOG配置寄存器

//初始化RTWDOG
//presc: 分频值，0 不分频，1 256分频
//cnt:	计数值

// src : 时钟源选择.0,bus clock;1,LPO clock;2,INTCLK;3,ERCLK;
//       bus clock:150Mhz; LPO clock和INTCLK都是32.768Khz; ERCLK约500Khz 
//pscen: 分频器使能.0,不使能.1,使能固定256分频.
//toval: 看门狗计数器溢出最大值(cnt超过该值将产生复位).范围:0~65535
// win : 看门狗计数器窗口值.范围:0~65535,0表示不使用窗口模式.
//       注意:刷新CNT必须是在win≤刷新时≤toval,这个区间,否则将产生复位.
//溢出时间=(256^pscen)*toval/src;
//一般建议用
void MYRTWDOG_Init(u8 src,u8 pscen,u16 toval,u16 win)
{
    rtwdog_clock_prescaler_t prescal;
    
    CLOCK_EnableClock(kCLOCK_Wdog3);			    //使能RTWDOG
    
	//初始化RTWDOG
	if(pscen==0)prescal=kRTWDOG_ClockPrescalerDivide1;	//不分频
	else if(pscen==1)prescal=kRTWDOG_ClockPrescalerDivide256;//256分频
	
	delay_ms(10);									//切换时钟前要等待一段时间
    RTWDOG_GetDefaultConfig(&rtwdog_config);		//先使用默认参数配置RTWDOG
    rtwdog_config.testMode=kRTWDOG_UserModeEnabled;	//用户模式
    rtwdog_config.clockSource=(rtwdog_clock_source_t)src;//时钟源选择
    rtwdog_config.prescaler=prescal;				//分频
    rtwdog_config.timeoutValue=toval;			    //超时值
    rtwdog_config.enableWindowMode =false;			//关闭窗口功能
	rtwdog_config.enableInterrupt=false;			//关闭中断
    rtwdog_config.enableRtwdog=true;				//使能看门狗
    rtwdog_config.windowValue=win;                  //窗口值(默认上面关闭了窗口功能)
    RTWDOG_Init(RTWDOG,&rtwdog_config);				//初始化看门狗
    
	RT1052_NVIC_SetPriority(RTWDOG_IRQn,4,0);	    //抢占优先级4，子优先级0
    NVIC_EnableIRQ(RTWDOG_IRQn);				    //使能RTWDOG中断
}

//RTWDOG中断服务函数
void RTWDOG_IRQHandler(void)
{
    //FLG=1,产生了超时中断
    if((RTWDOG_GetStatusFlags(RTWDOG)&kRTWDOG_InterruptFlag)==kRTWDOG_InterruptFlag)
	{ 
		//无法在此处喂狗,只能做善后工作,且务必要短.
	}	
    RTWDOG_ClearStatusFlags(RTWDOG,kRTWDOG_InterruptFlag); //清除中断标志位
}

//喂狗
void RTWDOG_Feed(void)
{
	RTWDOG_Refresh(RTWDOG);
}

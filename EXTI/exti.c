#include "exti.h"
#include "led.h"
#include "delay.h"
#include "key.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK I.MX RT1052开发板
//外部中断驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/3/23
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//外部中断初始化
void EXTIX_Init(void)
{
	gpio_pin_config_t int_config;
	
    KEY_Init();                                     //初始化按键
    
	//KEY_UP
	int_config.direction=kGPIO_DigitalInput;		//输入
	int_config.interruptMode=kGPIO_IntFallingEdge;	//下降沿触发中断
	int_config.outputLogic=1;						//默认高电平
	GPIO_PinInit(GPIO5,0,&int_config); 				//初始化GPIO5_00 

    //KEY1
    int_config.direction=kGPIO_DigitalInput;		//输入
	int_config.interruptMode=kGPIO_IntFallingEdge;	//下降沿触发中断
	int_config.outputLogic=1;						//默认高电平
	GPIO_PinInit(GPIO5,1,&int_config); 				//初始化GPIO5_01

    //KEY0
    int_config.direction=kGPIO_DigitalInput;		//输入
	int_config.interruptMode=kGPIO_IntFallingEdge;	//下降沿触发中断
	int_config.outputLogic=1;						//默认高电平
	GPIO_PinInit(GPIO1,5,&int_config); 				//初始化GPIO1_05
    
    //KEY2
    int_config.direction=kGPIO_DigitalInput;		//输入
	int_config.interruptMode=kGPIO_IntFallingEdge;	//下降沿触发中断
	int_config.outputLogic=1;						//默认高电平
	GPIO_PinInit(GPIO3,26,&int_config);             //初始化GPIO3_26
    
	//使能WKUP(GPIO5_00)和KEY1(GPIO5_01)中断
	GPIO_PortEnableInterrupts(GPIO5,1<<0);			//GPIO5_00中断使能
    GPIO_PortEnableInterrupts(GPIO5,1<<1);			//GPIO5_01中断使能
	RT1052_NVIC_SetPriority(GPIO5_Combined_0_15_IRQn,3,0);//抢占优先级位3，0位子优先级
	EnableIRQ(GPIO5_Combined_0_15_IRQn);			//使能GPIO5_0~15IO的中断
    
    //使能KEY0(GPIO1_05)中断
	GPIO_PortEnableInterrupts(GPIO1,1<<5);			//GPIO1_05中断使能
	RT1052_NVIC_SetPriority(GPIO1_Combined_0_15_IRQn,4,0);//抢占优先级位3，0位子优先级
	EnableIRQ(GPIO1_Combined_0_15_IRQn);			//使能GPIO1_0~15IO的中断

    //使能KEY2(GPIO3_26)中断
	GPIO_PortEnableInterrupts(GPIO3,1<<26);			//GPIO3_26中断使能
	RT1052_NVIC_SetPriority(GPIO3_Combined_16_31_IRQn,5,0);//抢占优先级位5，0位子优先级
	EnableIRQ(GPIO3_Combined_16_31_IRQn);			//使能GPIO3_16~31 IO的中断
}

//GPIO1_0~15共用的中断服务函数
void GPIO1_Combined_0_15_IRQHandler(void)
{
    static u8 led0sta=1,led1sta=1;
    
    if(GPIO_GetPinsInterruptFlags(GPIO1)&(1<<5))  //判断是否为GPIO1_5中断
    {  
        //消抖延时，这里为了方便演示例程所以在中断中使用了延时函数来做消抖，
        //在实际的项目中绝对禁止在中断中使用延时函数！！！！！！！
        delay_ms(10);         
        if(GPIO_PinRead(GPIO1,5)==0) //KEY0(GPIO1_5)是否保持按下状态
        {
            led1sta=!led1sta;
            led0sta=!led0sta;
            LED1(led1sta);
            LED0(led0sta); 
        }
    }
    GPIO_PortClearInterruptFlags(GPIO1,1<<5);		//清除中断标志位
	__DSB();				//数据同步屏蔽指令 
}


//GPIO3_16~31共用的中断服务函数
void GPIO3_Combined_16_31_IRQHandler(void)
{
    if(GPIO_GetPinsInterruptFlags(GPIO3)&(1<<26))    //判断是否为GPIO3_26中断
	{
        //消抖延时，这里为了方便演示例程所以在中断中使用了延时函数来做消抖，
        //在实际的项目中绝对禁止在中断中使用延时函数！！！！！！！
        delay_ms(10); 
        if(GPIO_PinRead(GPIO3,26)==0)   //KEY2(GPIO3_25)是否保持按下状态
        {
            LED0_Toggle;
        }
	}
    GPIO_PortClearInterruptFlags(GPIO3,1<<26);		//清除中断标志位
	__DSB();				//数据同步屏蔽指令  
}

//GPIO5_0~15共用的中断服务函数
void GPIO5_Combined_0_15_IRQHandler(void)
{
     static u8 led0sta=1,led1sta=1;
	
    if(GPIO_GetPinsInterruptFlags(GPIO5)&(1<<0))    //判断是否为GPIO5_0中断
	{
        delay_ms(10); 
        if(GPIO_PinRead(GPIO5,0)==0) //KEYUP(GPIO5_00)是否保持按下状态
        {
            led1sta=!led1sta;
            led0sta=!led1sta;
            LED1(led1sta);
            LED0(led0sta);
        }
	}
    if(GPIO_GetPinsInterruptFlags(GPIO5)&(1<<1))   //判断是否为GPIO5_1中断  
    {
        delay_ms(10); 
        if(GPIO_PinRead(GPIO5,1)==0)  //(KEY1)GPIO5_01是否保持按下状态 
        {
            LED1_Toggle;
        }
    }
     GPIO_PortClearInterruptFlags(GPIO5,1<<0);		//清除中断标志位
     GPIO_PortClearInterruptFlags(GPIO5,1<<1);		//清除中断标志位
	__DSB();				//数据同步屏蔽指令
}


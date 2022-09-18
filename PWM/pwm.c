#include "pwm.h"
#include "lpuart.h"
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

//QTIMER4配置结构体
qtmr_config_t qtimer4pwm_config;

//计算2的乘方
//time:多少个2相乘，time不能大于7
//返回值：2^time,结果不能大于255;
u8 Calcu_2invo(u8 time)
{
    u8 i=0;
    u8 value=1;
    
    if(time>7)time=7;
    
    if(time==0)
        value=1;
    else
    {
        for(i=0;i<time;i++)
        {
            value*=2;
        }
    }
        
    return value;
}

//初始化QTIMER4通道3作为PWM输出通道，
//QTIMER4时钟源为IPG_CLK_ROOT=150MHz
//prisrc : 第一时钟源选择
//         0000~0011,通道0~3的输入引脚.
//         0100~0111,通道0~3的输出.可用于级联.
//         1000~1111,IPG_CLK_ROOT时钟的:1,2,4,8,16,32,64,128分频.
//clk: PWM频率
//duty: 占空比，百分比
void QTMR4_CH3_PWM_Init(u8 prisrc,u32 clk, u8 duty)
{
    u8 fredivi=1;
    
    qtmr_primary_count_source_t qtimer_source;
    qtimer_source=(qtmr_primary_count_source_t)prisrc;
    
	//配置GPIO_B1_11为QTIMER3_TIMER1的输出引脚
	IOMUXC_SetPinMux(IOMUXC_GPIO_B1_11_QTIMER4_TIMER3,0);	
	
	//配置IO引脚GPIO_AD_B1_11的功能
	//低转换速度,驱动能力为R0/6,速度为100Mhz，关闭开路功能，使能pull/keepr
	//选择keeper功能，下拉100K Ohm，关闭Hyst
	IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_11_QTIMER4_TIMER3,0x10B0);
    
    fredivi=Calcu_2invo(prisrc-8);
    //初始化QTIMER4
    QTMR_GetDefaultConfig(&qtimer4pwm_config);                  //先设置为默认配置，后面在根据实际情况配置
    qtimer4pwm_config.primarySource=qtimer_source;              //设置第一时钟源
    QTMR_Init(TMR4,kQTMR_Channel_3,&qtimer4pwm_config);         //初始化TIM4通道3
    QTMR_SetupPwm(TMR4,kQTMR_Channel_3,clk,duty,false,CLOCK_GetFreq(kCLOCK_IpgClk)/fredivi); //初始化PWM
    QTMR_StartTimer(TMR4,kQTMR_Channel_3,kQTMR_PriSrcRiseEdge); //通道3在primary时钟源的上升沿计数
}

//设置占空比
//prisrc : 第一时钟源选择
//         0000~0011,通道0~3的输入引脚.
//         0100~0111,通道0~3的输出.可用于级联.
//         1000~1111,IPG_CLK_ROOT时钟的:1,2,4,8,16,32,64,128分频.
//clk: PWM频率
//duty: 占空比，百分比
void QTMER4CH3_PWM_DutySet(u8 prisrc,u32 clk, u8 duty)
{
    u8 fredivi=1;
    u32 srcclk,period,hightime,lowtime;
    
    fredivi=Calcu_2invo(prisrc-8);
    srcclk=CLOCK_GetFreq(kCLOCK_IpgClk)/fredivi;
    
    period=(srcclk/clk);                //一个PWM周期需要的计数值
    hightime=(period*duty)/100;         //一个PWM周期中高电平时间(计数值)
    lowtime=period-hightime;            //一个PWM周期中低电平时间(计数值)
    
    TMR4->CHANNEL[kQTMR_Channel_3].CMPLD1=lowtime;
    TMR4->CHANNEL[kQTMR_Channel_3].CMPLD2=hightime;
}


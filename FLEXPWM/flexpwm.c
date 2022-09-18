#include "flexpwm.h"
#include "lpuart.h"
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

pwm_config_t pwm2sm3_config;    //PWM2模块3配置结构体

//初始化FLEXPWM,PWM2的模块3的通道A和B
//通过PWM2的模块3在通道A和B上面产生2路PWM输出.
//即在GPIO3_IO02和GPIO3_IO03上面输出PWM.
//psc : 预分频器,0~7,表示2^psc分频.
//fre : 频率
//duty:占空比
void PWM2_SM3_PWMAB_Init(u16 psc,u32 fre,u8 duty)
{
    u32 sourceclock;
    pwm_signal_param_t pwm_ignal[2];
    pwm_clock_prescale_t pwm_prescale=(pwm_clock_prescale_t)psc; //分频
    
	//IO功能设置
	IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_02_FLEXPWM2_PWMA03,0);	 
	IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_03_FLEXPWM2_PWMB03,0);   
    
	//配置IO引脚GPIO_SD_B1_02和GPIO_SD_B1_03的功能
	//低转换速度,驱动能力为R0/6,速度为100Mhz，关闭开路功能，使能pull/keepr
	//选择keeper功能，下拉100K Ohm，关闭Hyst
	IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_02_FLEXPWM2_PWMA03,0x10B0);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_03_FLEXPWM2_PWMB03,0x10B0);
    
    //初始化PWM2 模块3的通道A和B
    PWM_GetDefaultConfig(&pwm2sm3_config);              //先初始化为默认配置
    pwm2sm3_config.clockSource=kPWM_BusClock;           //时钟源为IP BUS=IPG_CLK_ROOT=150MHz
    pwm2sm3_config.prescale=pwm_prescale;               //设置分频
    pwm2sm3_config.reloadLogic=kPWM_ReloadPwmFullCycle; //全周期更新
    pwm2sm3_config.pairOperation=kPWM_Independent;      //PMWA PWMB独立模式
	PWM_Init(PWM2,kPWM_Module_3,&pwm2sm3_config);       //初始化PWM2模块3

    //设置PWM的两个通道
    sourceclock=CLOCK_GetFreq(kCLOCK_IpgClk);
  
    //PWMA
    pwm_ignal[0].pwmChannel=kPWM_PwmA;                  //PWM通道A
    pwm_ignal[0].level=kPWM_HighTrue;                   //高电平有效
    pwm_ignal[0].dutyCyclePercent=duty;                 //占空比

    //PWMB
    pwm_ignal[1].pwmChannel=kPWM_PwmB;                  //PWM通道B
    pwm_ignal[1].level=kPWM_HighTrue;                   //高电平有效
    pwm_ignal[1].dutyCyclePercent=duty;                 //占空比

    //设置PWM2，中央对齐模式
    PWM_SetupPwm(PWM2,kPWM_Module_3,pwm_ignal,2,kPWM_SignedCenterAligned,fre,sourceclock);
    
    PWM_SetPwmLdok(PWM2,kPWM_Control_Module_3,true);    //设置PWM的load ok位
    PWM_StartTimer(PWM2,kPWM_Control_Module_3);         //开启定时器

}

//更新PWM2占空比
//duty:占空比
void PWM2_SM3_DutySet(u8 duty) 
{
    PWM_UpdatePwmDutycycle(PWM2,kPWM_Module_3,kPWM_PwmA,kPWM_SignedCenterAligned,duty); //更新PWMA占空比
    PWM_UpdatePwmDutycycle(PWM2,kPWM_Module_3,kPWM_PwmB,kPWM_SignedCenterAligned,duty); //更新PWMB占空比
    PWM_SetPwmLdok(PWM2,kPWM_Control_Module_3,true);    //设置PWM的load ok位
}

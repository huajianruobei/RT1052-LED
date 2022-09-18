#include "flexpwm.h"
#include "lpuart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//FLEXPWM��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/1/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

pwm_config_t pwm2sm3_config;    //PWM2ģ��3���ýṹ��

//��ʼ��FLEXPWM,PWM2��ģ��3��ͨ��A��B
//ͨ��PWM2��ģ��3��ͨ��A��B�������2·PWM���.
//����GPIO3_IO02��GPIO3_IO03�������PWM.
//psc : Ԥ��Ƶ��,0~7,��ʾ2^psc��Ƶ.
//fre : Ƶ��
//duty:ռ�ձ�
void PWM2_SM3_PWMAB_Init(u16 psc,u32 fre,u8 duty)
{
    u32 sourceclock;
    pwm_signal_param_t pwm_ignal[2];
    pwm_clock_prescale_t pwm_prescale=(pwm_clock_prescale_t)psc; //��Ƶ
    
	//IO��������
	IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_02_FLEXPWM2_PWMA03,0);	 
	IOMUXC_SetPinMux(IOMUXC_GPIO_SD_B1_03_FLEXPWM2_PWMB03,0);   
    
	//����IO����GPIO_SD_B1_02��GPIO_SD_B1_03�Ĺ���
	//��ת���ٶ�,��������ΪR0/6,�ٶ�Ϊ100Mhz���رտ�·���ܣ�ʹ��pull/keepr
	//ѡ��keeper���ܣ�����100K Ohm���ر�Hyst
	IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_02_FLEXPWM2_PWMA03,0x10B0);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_SD_B1_03_FLEXPWM2_PWMB03,0x10B0);
    
    //��ʼ��PWM2 ģ��3��ͨ��A��B
    PWM_GetDefaultConfig(&pwm2sm3_config);              //�ȳ�ʼ��ΪĬ������
    pwm2sm3_config.clockSource=kPWM_BusClock;           //ʱ��ԴΪIP BUS=IPG_CLK_ROOT=150MHz
    pwm2sm3_config.prescale=pwm_prescale;               //���÷�Ƶ
    pwm2sm3_config.reloadLogic=kPWM_ReloadPwmFullCycle; //ȫ���ڸ���
    pwm2sm3_config.pairOperation=kPWM_Independent;      //PMWA PWMB����ģʽ
	PWM_Init(PWM2,kPWM_Module_3,&pwm2sm3_config);       //��ʼ��PWM2ģ��3

    //����PWM������ͨ��
    sourceclock=CLOCK_GetFreq(kCLOCK_IpgClk);
  
    //PWMA
    pwm_ignal[0].pwmChannel=kPWM_PwmA;                  //PWMͨ��A
    pwm_ignal[0].level=kPWM_HighTrue;                   //�ߵ�ƽ��Ч
    pwm_ignal[0].dutyCyclePercent=duty;                 //ռ�ձ�

    //PWMB
    pwm_ignal[1].pwmChannel=kPWM_PwmB;                  //PWMͨ��B
    pwm_ignal[1].level=kPWM_HighTrue;                   //�ߵ�ƽ��Ч
    pwm_ignal[1].dutyCyclePercent=duty;                 //ռ�ձ�

    //����PWM2���������ģʽ
    PWM_SetupPwm(PWM2,kPWM_Module_3,pwm_ignal,2,kPWM_SignedCenterAligned,fre,sourceclock);
    
    PWM_SetPwmLdok(PWM2,kPWM_Control_Module_3,true);    //����PWM��load okλ
    PWM_StartTimer(PWM2,kPWM_Control_Module_3);         //������ʱ��

}

//����PWM2ռ�ձ�
//duty:ռ�ձ�
void PWM2_SM3_DutySet(u8 duty) 
{
    PWM_UpdatePwmDutycycle(PWM2,kPWM_Module_3,kPWM_PwmA,kPWM_SignedCenterAligned,duty); //����PWMAռ�ձ�
    PWM_UpdatePwmDutycycle(PWM2,kPWM_Module_3,kPWM_PwmB,kPWM_SignedCenterAligned,duty); //����PWMBռ�ձ�
    PWM_SetPwmLdok(PWM2,kPWM_Control_Module_3,true);    //����PWM��load okλ
}

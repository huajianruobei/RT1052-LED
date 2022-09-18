#include "pwm.h"
#include "lpuart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//PWM��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/1/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//QTIMER4���ýṹ��
qtmr_config_t qtimer4pwm_config;

//����2�ĳ˷�
//time:���ٸ�2��ˣ�time���ܴ���7
//����ֵ��2^time,������ܴ���255;
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

//��ʼ��QTIMER4ͨ��3��ΪPWM���ͨ����
//QTIMER4ʱ��ԴΪIPG_CLK_ROOT=150MHz
//prisrc : ��һʱ��Դѡ��
//         0000~0011,ͨ��0~3����������.
//         0100~0111,ͨ��0~3�����.�����ڼ���.
//         1000~1111,IPG_CLK_ROOTʱ�ӵ�:1,2,4,8,16,32,64,128��Ƶ.
//clk: PWMƵ��
//duty: ռ�ձȣ��ٷֱ�
void QTMR4_CH3_PWM_Init(u8 prisrc,u32 clk, u8 duty)
{
    u8 fredivi=1;
    
    qtmr_primary_count_source_t qtimer_source;
    qtimer_source=(qtmr_primary_count_source_t)prisrc;
    
	//����GPIO_B1_11ΪQTIMER3_TIMER1���������
	IOMUXC_SetPinMux(IOMUXC_GPIO_B1_11_QTIMER4_TIMER3,0);	
	
	//����IO����GPIO_AD_B1_11�Ĺ���
	//��ת���ٶ�,��������ΪR0/6,�ٶ�Ϊ100Mhz���رտ�·���ܣ�ʹ��pull/keepr
	//ѡ��keeper���ܣ�����100K Ohm���ر�Hyst
	IOMUXC_SetPinConfig(IOMUXC_GPIO_B1_11_QTIMER4_TIMER3,0x10B0);
    
    fredivi=Calcu_2invo(prisrc-8);
    //��ʼ��QTIMER4
    QTMR_GetDefaultConfig(&qtimer4pwm_config);                  //������ΪĬ�����ã������ڸ���ʵ���������
    qtimer4pwm_config.primarySource=qtimer_source;              //���õ�һʱ��Դ
    QTMR_Init(TMR4,kQTMR_Channel_3,&qtimer4pwm_config);         //��ʼ��TIM4ͨ��3
    QTMR_SetupPwm(TMR4,kQTMR_Channel_3,clk,duty,false,CLOCK_GetFreq(kCLOCK_IpgClk)/fredivi); //��ʼ��PWM
    QTMR_StartTimer(TMR4,kQTMR_Channel_3,kQTMR_PriSrcRiseEdge); //ͨ��3��primaryʱ��Դ�������ؼ���
}

//����ռ�ձ�
//prisrc : ��һʱ��Դѡ��
//         0000~0011,ͨ��0~3����������.
//         0100~0111,ͨ��0~3�����.�����ڼ���.
//         1000~1111,IPG_CLK_ROOTʱ�ӵ�:1,2,4,8,16,32,64,128��Ƶ.
//clk: PWMƵ��
//duty: ռ�ձȣ��ٷֱ�
void QTMER4CH3_PWM_DutySet(u8 prisrc,u32 clk, u8 duty)
{
    u8 fredivi=1;
    u32 srcclk,period,hightime,lowtime;
    
    fredivi=Calcu_2invo(prisrc-8);
    srcclk=CLOCK_GetFreq(kCLOCK_IpgClk)/fredivi;
    
    period=(srcclk/clk);                //һ��PWM������Ҫ�ļ���ֵ
    hightime=(period*duty)/100;         //һ��PWM�����иߵ�ƽʱ��(����ֵ)
    lowtime=period-hightime;            //һ��PWM�����е͵�ƽʱ��(����ֵ)
    
    TMR4->CHANNEL[kQTMR_Channel_3].CMPLD1=lowtime;
    TMR4->CHANNEL[kQTMR_Channel_3].CMPLD2=hightime;
}


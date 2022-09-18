#include "rtwdog.h"
#include "lpuart.h"
#include "delay.h"
#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//RTWDOG��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/4/25
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

rtwdog_config_t rtwdog_config;	//RTWDOG���üĴ���

//��ʼ��RTWDOG
//presc: ��Ƶֵ��0 ����Ƶ��1 256��Ƶ
//cnt:	����ֵ

// src : ʱ��Դѡ��.0,bus clock;1,LPO clock;2,INTCLK;3,ERCLK;
//       bus clock:150Mhz; LPO clock��INTCLK����32.768Khz; ERCLKԼ500Khz 
//pscen: ��Ƶ��ʹ��.0,��ʹ��.1,ʹ�̶ܹ�256��Ƶ.
//toval: ���Ź�������������ֵ(cnt������ֵ��������λ).��Χ:0~65535
// win : ���Ź�����������ֵ.��Χ:0~65535,0��ʾ��ʹ�ô���ģʽ.
//       ע��:ˢ��CNT��������win��ˢ��ʱ��toval,�������,���򽫲�����λ.
//���ʱ��=(256^pscen)*toval/src;
//һ�㽨����
void MYRTWDOG_Init(u8 src,u8 pscen,u16 toval,u16 win)
{
    rtwdog_clock_prescaler_t prescal;
    
    CLOCK_EnableClock(kCLOCK_Wdog3);			    //ʹ��RTWDOG
    
	//��ʼ��RTWDOG
	if(pscen==0)prescal=kRTWDOG_ClockPrescalerDivide1;	//����Ƶ
	else if(pscen==1)prescal=kRTWDOG_ClockPrescalerDivide256;//256��Ƶ
	
	delay_ms(10);									//�л�ʱ��ǰҪ�ȴ�һ��ʱ��
    RTWDOG_GetDefaultConfig(&rtwdog_config);		//��ʹ��Ĭ�ϲ�������RTWDOG
    rtwdog_config.testMode=kRTWDOG_UserModeEnabled;	//�û�ģʽ
    rtwdog_config.clockSource=(rtwdog_clock_source_t)src;//ʱ��Դѡ��
    rtwdog_config.prescaler=prescal;				//��Ƶ
    rtwdog_config.timeoutValue=toval;			    //��ʱֵ
    rtwdog_config.enableWindowMode =false;			//�رմ��ڹ���
	rtwdog_config.enableInterrupt=false;			//�ر��ж�
    rtwdog_config.enableRtwdog=true;				//ʹ�ܿ��Ź�
    rtwdog_config.windowValue=win;                  //����ֵ(Ĭ������ر��˴��ڹ���)
    RTWDOG_Init(RTWDOG,&rtwdog_config);				//��ʼ�����Ź�
    
	RT1052_NVIC_SetPriority(RTWDOG_IRQn,4,0);	    //��ռ���ȼ�4�������ȼ�0
    NVIC_EnableIRQ(RTWDOG_IRQn);				    //ʹ��RTWDOG�ж�
}

//RTWDOG�жϷ�����
void RTWDOG_IRQHandler(void)
{
    //FLG=1,�����˳�ʱ�ж�
    if((RTWDOG_GetStatusFlags(RTWDOG)&kRTWDOG_InterruptFlag)==kRTWDOG_InterruptFlag)
	{ 
		//�޷��ڴ˴�ι��,ֻ�����ƺ���,�����Ҫ��.
	}	
    RTWDOG_ClearStatusFlags(RTWDOG,kRTWDOG_InterruptFlag); //����жϱ�־λ
}

//ι��
void RTWDOG_Feed(void)
{
	RTWDOG_Refresh(RTWDOG);
}

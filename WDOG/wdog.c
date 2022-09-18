#include "wdog.h"
#include "lpuart.h"
#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//���Ź���������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2012/12/19
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

wdog_config_t wdog1_config; //WDOG���ýṹ��
 
//��ʼ��WDOG1
//wdgtimeout:���Ź���ʱʱ�䣬ʵ��ʱ��Ϊ(wdgtimeout+1)*0.5s
//			 �������Ҫ���ó�ʱʱ��Ϊ2s�Ļ�wdgtimeout=2/0.5-1=3
//inttimeout:�ڿ��Ź���ʱ����ǰ��������жϣ�ʵ��ʱ��Ϊinttimeout*0.5s
void WDOG1_Init(u8 wdgtimeout, u8 inttimeout)
{
	//WDOG_Init()���������ʹ�ܿ��Ź�ʱ�ӣ�����д���������Ѵ��ʹ�ÿ��Ź�
	//��ʱ���Ź�ʱ����Ҫʹ�ܵġ�
	CLOCK_EnableClock(kCLOCK_Wdog1); 			//ʹ�ܿ��Ź�ʱ��
	
	WDOG_GetDefaultConfig(&wdog1_config);		//�Ƚ�WDOG1����ΪĬ��ֵ
	wdog1_config.workMode.enableWait=false;		//�ȴ�ģʽ�ر�
	wdog1_config.workMode.enableStop=false;		//stopģʽ�ر�wdog
	wdog1_config.workMode.enableDebug=false;	//debugģʽ�ر�wdog	
	wdog1_config.timeoutValue=wdgtimeout;		//���ó�ʱֵ������Ϊ(wdgtimeout+1)*0.5
	wdog1_config.interruptTimeValue=inttimeout;	//����wdog��ʱǰ��������ж�? inttimeout*0.5
	wdog1_config.enableInterrupt=true;			//ʹ��wdog�ж�
	wdog1_config.enableWdog=true;				//ʹ��wdog
	WDOG_Init(WDOG1,&wdog1_config);				//��ʼ�����Ź�
	
	//����WDOG�ж�
	RT1052_NVIC_SetPriority(WDOG1_IRQn,4,0);	//��ռ���ȼ�4�������ȼ�0
	EnableIRQ(WDOG1_IRQn);						//ʹ��WDOG1�ж�
}

//ι��
void WDOG1_Feed(void)
{  
	WDOG_Refresh(WDOG1);//��װ��	
}

//���Ź��жϷ�����
void WDOG1_IRQHandler(void)
{
	if(WDOG_GetStatusFlags(WDOG1) & kWDOG_InterruptFlag)	//��ʱҪ����
	{
        WDOG1_Feed();	//ι��
        LED1_Toggle;
	}
	WDOG_ClearInterruptStatus(WDOG1,kWDOG_InterruptFlag);	//����жϱ�־
	__DSB();				//����ͬ������ָ��
}

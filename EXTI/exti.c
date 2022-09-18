#include "exti.h"
#include "led.h"
#include "delay.h"
#include "key.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//�ⲿ�ж���������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/3/23
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//�ⲿ�жϳ�ʼ��
void EXTIX_Init(void)
{
	gpio_pin_config_t int_config;
	
    KEY_Init();                                     //��ʼ������
    
	//KEY_UP
	int_config.direction=kGPIO_DigitalInput;		//����
	int_config.interruptMode=kGPIO_IntFallingEdge;	//�½��ش����ж�
	int_config.outputLogic=1;						//Ĭ�ϸߵ�ƽ
	GPIO_PinInit(GPIO5,0,&int_config); 				//��ʼ��GPIO5_00 

    //KEY1
    int_config.direction=kGPIO_DigitalInput;		//����
	int_config.interruptMode=kGPIO_IntFallingEdge;	//�½��ش����ж�
	int_config.outputLogic=1;						//Ĭ�ϸߵ�ƽ
	GPIO_PinInit(GPIO5,1,&int_config); 				//��ʼ��GPIO5_01

    //KEY0
    int_config.direction=kGPIO_DigitalInput;		//����
	int_config.interruptMode=kGPIO_IntFallingEdge;	//�½��ش����ж�
	int_config.outputLogic=1;						//Ĭ�ϸߵ�ƽ
	GPIO_PinInit(GPIO1,5,&int_config); 				//��ʼ��GPIO1_05
    
    //KEY2
    int_config.direction=kGPIO_DigitalInput;		//����
	int_config.interruptMode=kGPIO_IntFallingEdge;	//�½��ش����ж�
	int_config.outputLogic=1;						//Ĭ�ϸߵ�ƽ
	GPIO_PinInit(GPIO3,26,&int_config);             //��ʼ��GPIO3_26
    
	//ʹ��WKUP(GPIO5_00)��KEY1(GPIO5_01)�ж�
	GPIO_PortEnableInterrupts(GPIO5,1<<0);			//GPIO5_00�ж�ʹ��
    GPIO_PortEnableInterrupts(GPIO5,1<<1);			//GPIO5_01�ж�ʹ��
	RT1052_NVIC_SetPriority(GPIO5_Combined_0_15_IRQn,3,0);//��ռ���ȼ�λ3��0λ�����ȼ�
	EnableIRQ(GPIO5_Combined_0_15_IRQn);			//ʹ��GPIO5_0~15IO���ж�
    
    //ʹ��KEY0(GPIO1_05)�ж�
	GPIO_PortEnableInterrupts(GPIO1,1<<5);			//GPIO1_05�ж�ʹ��
	RT1052_NVIC_SetPriority(GPIO1_Combined_0_15_IRQn,4,0);//��ռ���ȼ�λ3��0λ�����ȼ�
	EnableIRQ(GPIO1_Combined_0_15_IRQn);			//ʹ��GPIO1_0~15IO���ж�

    //ʹ��KEY2(GPIO3_26)�ж�
	GPIO_PortEnableInterrupts(GPIO3,1<<26);			//GPIO3_26�ж�ʹ��
	RT1052_NVIC_SetPriority(GPIO3_Combined_16_31_IRQn,5,0);//��ռ���ȼ�λ5��0λ�����ȼ�
	EnableIRQ(GPIO3_Combined_16_31_IRQn);			//ʹ��GPIO3_16~31 IO���ж�
}

//GPIO1_0~15���õ��жϷ�����
void GPIO1_Combined_0_15_IRQHandler(void)
{
    static u8 led0sta=1,led1sta=1;
    
    if(GPIO_GetPinsInterruptFlags(GPIO1)&(1<<5))  //�ж��Ƿ�ΪGPIO1_5�ж�
    {  
        //������ʱ������Ϊ�˷�����ʾ�����������ж���ʹ������ʱ��������������
        //��ʵ�ʵ���Ŀ�о��Խ�ֹ���ж���ʹ����ʱ������������������
        delay_ms(10);         
        if(GPIO_PinRead(GPIO1,5)==0) //KEY0(GPIO1_5)�Ƿ񱣳ְ���״̬
        {
            led1sta=!led1sta;
            led0sta=!led0sta;
            LED1(led1sta);
            LED0(led0sta); 
        }
    }
    GPIO_PortClearInterruptFlags(GPIO1,1<<5);		//����жϱ�־λ
	__DSB();				//����ͬ������ָ�� 
}


//GPIO3_16~31���õ��жϷ�����
void GPIO3_Combined_16_31_IRQHandler(void)
{
    if(GPIO_GetPinsInterruptFlags(GPIO3)&(1<<26))    //�ж��Ƿ�ΪGPIO3_26�ж�
	{
        //������ʱ������Ϊ�˷�����ʾ�����������ж���ʹ������ʱ��������������
        //��ʵ�ʵ���Ŀ�о��Խ�ֹ���ж���ʹ����ʱ������������������
        delay_ms(10); 
        if(GPIO_PinRead(GPIO3,26)==0)   //KEY2(GPIO3_25)�Ƿ񱣳ְ���״̬
        {
            LED0_Toggle;
        }
	}
    GPIO_PortClearInterruptFlags(GPIO3,1<<26);		//����жϱ�־λ
	__DSB();				//����ͬ������ָ��  
}

//GPIO5_0~15���õ��жϷ�����
void GPIO5_Combined_0_15_IRQHandler(void)
{
     static u8 led0sta=1,led1sta=1;
	
    if(GPIO_GetPinsInterruptFlags(GPIO5)&(1<<0))    //�ж��Ƿ�ΪGPIO5_0�ж�
	{
        delay_ms(10); 
        if(GPIO_PinRead(GPIO5,0)==0) //KEYUP(GPIO5_00)�Ƿ񱣳ְ���״̬
        {
            led1sta=!led1sta;
            led0sta=!led1sta;
            LED1(led1sta);
            LED0(led0sta);
        }
	}
    if(GPIO_GetPinsInterruptFlags(GPIO5)&(1<<1))   //�ж��Ƿ�ΪGPIO5_1�ж�  
    {
        delay_ms(10); 
        if(GPIO_PinRead(GPIO5,1)==0)  //(KEY1)GPIO5_01�Ƿ񱣳ְ���״̬ 
        {
            LED1_Toggle;
        }
    }
     GPIO_PortClearInterruptFlags(GPIO5,1<<0);		//����жϱ�־λ
     GPIO_PortClearInterruptFlags(GPIO5,1<<1);		//����жϱ�־λ
	__DSB();				//����ͬ������ָ��
}


#include "gptimer.h"
#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//GPT��ʱ����������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2012/12/23
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

gpt_config_t gpt1_onfig;

//��ʼ��GPTIMER1��ʱ��ԴΪperclk_clk_root=75MHz
//pre:��Ƶֵ��0~4096
//comcount:�Ƚϼ���ֵ��0~0xFFFFFFFF
//��ocrx==CNTʱ,�����ж�.
//��ʱʱ��=ocrx*(psc+1)/PERCLK_CLK_ROOT
void GPT1_Int_Init(u16 psc,u32 ocrx)
{
	GPT_GetDefaultConfig(&gpt1_onfig);	//�ȳ�ʼ��GPT1ΪĬ��ֵ
	gpt1_onfig.clockSource=kGPT_ClockSource_Periph;	//��ʼ��ʱ��Դperclk_clk_root
	gpt1_onfig.divider=psc;	        //���÷�Ƶֵ
	GPT_Init(GPT1,&gpt1_onfig);
	
	GPT_SetOutputCompareValue(GPT1,kGPT_OutputCompare_Channel1,ocrx);	    //���ñȽϼ���ֵ
	GPT_EnableInterrupts(GPT1,kGPT_OutputCompare1InterruptEnable);			//ʹ��GPT�Ƚ�ͨ��1�ж�
	RT1052_NVIC_SetPriority(GPT1_IRQn,5,0);									//��ռ���ȼ�6�������ȼ�0
	EnableIRQ(GPT1_IRQn);	//ʹ��GPT1�ж�
	GPT_StartTimer(GPT1);	//��ʼ��ʱ��	
}


//��ʼ��GPT2 CAP2���벶��
//psc : Ԥ��Ƶ��,0~4095
//������ʱ��=(2^32)*(psc+1)/PERCLK_CLK_ROOT=234562.5��,�����ϲ��������
void GPT2_CAP2_Init(u16 psc)
{
    gpt_config_t gpt2_onfig;
    
    //����GPT2_CAP2���IO(GPIO_EMC_40)�Ĺ���
	//��ת���ٶ�,����R0/6,�ٶ�Ϊ200Mhz���رտ�·���ܣ�ʹ��pull/keepr
	//ѡ��pull���ܣ�����22K Ohm 
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_40_GPT2_CAPTURE2,0);	  
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_40_GPT2_CAPTURE2,0xF0F1);
    
    GPT_GetDefaultConfig(&gpt2_onfig);	//�ȳ�ʼ��GPT2ΪĬ��ֵ
    gpt2_onfig.divider=psc;	            //���÷�Ƶֵ
	gpt2_onfig.clockSource=kGPT_ClockSource_Periph;	//����ʱ��Դ
	GPT_Init(GPT2,&gpt2_onfig);         //��ʼ��GPT2
  
    //����CAP2Ϊ�½��ز���
    GPT_SetInputOperationMode(GPT2,kGPT_InputCapture_Channel2,kGPT_InputOperation_FallEdge);
    GPT_EnableInterrupts(GPT2,kGPT_InputCapture2InterruptEnable);   //ʹ��GPT2�����ж�
    GPT_EnableInterrupts(GPT2,kGPT_RollOverFlagInterruptEnable);    //ʹ��GPT2����ж�
    GPT_StartTimer(GPT2);	            //��ʼ��ʱ��
    
    //�ж�����
    RT1052_NVIC_SetPriority(GPT2_IRQn,7,0);//����GPT2�ж����ȼ�
    EnableIRQ(GPT2_IRQn);	            //ʹ��GPT2�ж�
}


//GPT1�жϷ�����
void GPT1_IRQHandler(void)
{
    //OCR1�ж�
    if(GPT_GetStatusFlags(GPT1,kGPT_OutputCompare1Flag))
    {
        LED1_Toggle;			//LED1�Ʒ�ת
        GPT_ClearStatusFlags(GPT1,kGPT_OutputCompare1Flag);//����жϱ�־λ
    }
	__DSB();				//����ͬ������ָ��
}


//����״̬
//[7]:0,û�гɹ��Ĳ���;1,�ɹ�����һ��.
//[6]:0,��û�����½���;1,�Ѿ������½�����.
//[5:0]:����͵�ƽ������Ĵ���(����32λ��ʱ����˵,1us��������1,���ʱ��:42949.67��)
u8  GPT2CAP2_CAPTURE_STA=0;	//���벶��״̬		    				
u32	GPT2CAP2_CAPTURE_VAL;	//���벶��ֵ(GPT2��32λ)

//GPT2�жϷ�����
void GPT2_IRQHandler(void)
{
 	if((GPT2CAP2_CAPTURE_STA&0X80)==0)//��δ�ɹ�����	
	{
        if(GPT_GetStatusFlags(GPT2,kGPT_RollOverFlag))  //����ж�
		{	     
			if(GPT2CAP2_CAPTURE_STA&0X40)               //�Ѿ����񵽸ߵ�ƽ��
			{
				if((GPT2CAP2_CAPTURE_STA&0X3F)==0X3F)   //�ߵ�ƽ̫����
				{
					GPT2CAP2_CAPTURE_STA|=0X80;		    //��ǳɹ�������һ��
					GPT2CAP2_CAPTURE_VAL=0XFFFFFFFF;
				}else GPT2CAP2_CAPTURE_STA++;
			}	 
		}
        if(GPT_GetStatusFlags(GPT2,kGPT_InputCapture2Flag))  //����2���������¼�
		{	
			if(GPT2CAP2_CAPTURE_STA&0X40)			//����һ���½��� 		
			{	  			
				GPT2CAP2_CAPTURE_STA|=0X80;			//��ǲ���һ�ε͵�ƽ����
                //��ȡ��ǰ�Ĳ���ֵ.
                GPT2CAP2_CAPTURE_VAL=GPT_GetInputCaptureValue(GPT2,kGPT_InputCapture_Channel2);		
                //���ò����½��� 
                GPT_SetInputOperationMode(GPT2,kGPT_InputCapture_Channel2,kGPT_InputOperation_FallEdge);
			}else  									//��δ��ʼ,��һ�β���������
			{
				GPT2CAP2_CAPTURE_STA=0;				//���
				GPT2CAP2_CAPTURE_VAL=0;				//����ֵ����
				GPT2CAP2_CAPTURE_STA|=0X40;			//��ǲ����½���
                GPT_StopTimer(GPT2);                //�ر�GPT2    
                //���ò��������� 
                GPT_SetInputOperationMode(GPT2,kGPT_InputCapture_Channel2,kGPT_InputOperation_RiseEdge);
                GPT_StartTimer(GPT2);               //ʹ��GPT2
			}		    
		}			     	    					   
 	}
    GPT_ClearStatusFlags(GPT2,kGPT_RollOverFlag);       //�������жϱ�־λ 
    GPT_ClearStatusFlags(GPT2,kGPT_InputCapture2Flag);  //�������2�жϱ�־λ    
	__DSB();			//����ͬ������ָ�� 
}

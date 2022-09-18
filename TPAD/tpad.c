#include "tpad.h"
#include "delay.h"		    
#include "lpuart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//TPAD ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/3/26
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 
 
#define TPAD_CAP_MAX_VAL 0XFFFF			//�������ֵ
vu16 tpad_default_val=0;				//���ص�ʱ��(û���ְ���),��������Ҫ��ʱ��

//��ʼ����������
//��ÿ��ص�ʱ����������ȡֵ.
//psc:��Ƶϵ��,ԽС,������Խ��.
//����ֵ:0,��ʼ���ɹ�;1,��ʼ��ʧ��
u8 TPAD_Init(u8 psc)
{
	u16 buf[10];
	u16 temp;
	u8 j,i;
	QTMR3_CH2_CAP_Init(psc);	//���÷�Ƶϵ��
	for(i=0;i<10;i++)			//������ȡ10��
	{				 
		buf[i]=TPAD_Get_Val();
		delay_ms(10);	    
	}				    
	for(i=0;i<9;i++)//����
	{
		for(j=i+1;j<10;j++)
		{
			if(buf[i]>buf[j])//��������
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}
	temp=0;
	for(i=2;i<8;i++)temp+=buf[i];//ȡ�м��6�����ݽ���ƽ��
	tpad_default_val=temp/6;
	printf("tpad_default_val:%d\r\n",tpad_default_val);	
	if(tpad_default_val>TPAD_CAP_MAX_VAL/2)return 1;//��ʼ����������TPAD_ARR_MAX_VAL/2����ֵ,������!
	return 0;		     	    					   
}

//��λһ��
//�ͷŵ��ݵ������������ʱ���ļ���ֵ
void TPAD_Reset(void)
{
    gpio_pin_config_t tpad_config;	
    
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_02_GPIO1_IO18,0);	  
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_02_GPIO1_IO18,0x10B0);
    
	tpad_config.direction=kGPIO_DigitalOutput;	//���
	tpad_config.interruptMode=kGPIO_NoIntmode;	//��ʹ���жϹ���
	tpad_config.outputLogic=1;					//Ĭ�ϸߵ�ƽ
	GPIO_PinInit(GPIO1,18,&tpad_config); 	    //��ʼ��GPIO1_3
    GPIO_PinWrite(GPIO1,18,0);                  //GPIO1_IO18���0,�ŵ�
    
	delay_ms(5);
    QTMR_ClearStatusFlags(TMR3,kQTMR_Channel_2,kQTMR_EdgeFlag); //������ز�����
	TMR3->CHANNEL[kQTMR_Channel_2].CNTR=0;			            //����    
    
    //����QTIMER_TIMER3���IO(GPIO_AD_B1_02)�Ĺ���
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_02_QTIMER3_TIMER2,0);	  
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_02_QTIMER3_TIMER2,0x10C1);
}

//�õ���ʱ������ֵ
//�����ʱ,��ֱ�ӷ��ض�ʱ���ļ���ֵ.
//����ֵ������ֵ/����ֵ����ʱ������·��أ�
u16 TPAD_Get_Val(void)
{				   
	TPAD_Reset();
    while((QTMR_GetStatus(TMR3,kQTMR_Channel_2)&(kQTMR_EdgeFlag))!=kQTMR_EdgeFlag)//�ȴ�����������
	{
		if(TMR3->CHANNEL[kQTMR_Channel_2].CNTR>TPAD_CAP_MAX_VAL-500)
            return TMR3->CHANNEL[kQTMR_Channel_2].CNTR;//��ʱ��,ֱ�ӷ���CNT��ֵ
	};	
	return TMR3->CHANNEL[kQTMR_Channel_2].CAPT;	  
} 	

//��ȡn��,ȡ���ֵ
//n��������ȡ�Ĵ���
//����ֵ��n�ζ������������������ֵ
u16 TPAD_Get_MaxVal(u8 n)
{ 
	u16 temp=0; 
	u16 res=0; 
	u8 lcntnum=n*2/3;//����2/3*n����Ч������,������Ч
	u8 okcnt=0;
	while(n--)
	{
		temp=TPAD_Get_Val();//�õ�һ��ֵ
		if(temp>(tpad_default_val*5/4))okcnt++;//���ٴ���Ĭ��ֵ��5/4������Ч
		if(temp>res)res=temp;
	}
	if(okcnt>=lcntnum)return res;//����2/3�ĸ���,Ҫ����Ĭ��ֵ��5/4������Ч
	else return 0;
}  
//ɨ�败������
//mode:0,��֧����������(����һ�α����ɿ����ܰ���һ��);1,֧����������(����һֱ����)
//����ֵ:0,û�а���;1,�а���;										  
u8 TPAD_Scan(u8 mode)
{
	static u8 keyen=0;	//0,���Կ�ʼ���;>0,�����ܿ�ʼ���	 
	u8 res=0;
	u8 sample=3;	//Ĭ�ϲ�������Ϊ3��	 
	u16 rval;
	if(mode)
	{
		sample=6;	//֧��������ʱ�����ò�������Ϊ6��
		keyen=0;	//֧������	  
	}
	rval=TPAD_Get_MaxVal(sample); 
	if(rval>(tpad_default_val*4/3)&&rval<(10*tpad_default_val))//����tpad_default_val+(1/3)*����tpad_default_val,��С��10��tpad_default_val,����Ч
	{							 
		if(keyen==0)res=1;	//keyen==0,��Ч 
		//printf("r:%d\r\n",rval);		     	    					   
		keyen=3;				//����Ҫ�ٹ�3��֮����ܰ�����Ч   
	} 
	if(keyen)keyen--;		   							   		     	    					   
	return res;
}	 

//��ʼ��QTMR3��ʱ��CH2 ���벶�� 
//prisrc : ��һʱ��Դѡ��
//         0000~0011,ͨ��0~3����������.
//         0100~0111,ͨ��0~3�����.�����ڼ���.
//         1000~1111,IPG_CLK_ROOTʱ�ӵ�:1,2,4,8,16,32,64,128��Ƶ.
//�����ʱƵ��=QTMR3_CLK=IPG_CLK_ROOT/2^(prisrc-8); 
//����prisrc=1011,��QTMR3_CLK=IPG_CLK_ROOT/8=18.75MHz. 
void QTMR3_CH2_CAP_Init(u8 prisrc)
{
    qtmr_config_t qtimer3_config;
    qtmr_primary_count_source_t qtimer3_source;
    
    u32 ipgclk=CLOCK_GetFreq(kCLOCK_IpgClk);     //��ȡIPG_CLK=150Mhz
    qtimer3_source=(qtmr_primary_count_source_t)prisrc;
    
    //IO����    
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_02_QTIMER3_TIMER2,0);	  
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_02_QTIMER3_TIMER2,0x10C1);
	//IOMUXC->SELECT_INPUT[kIOMUXC_QTIMER3_TIMER2_SELECT_INPUT]=1;	//ѡ��TMR3_CH2������ӵ�GPIO_AD_B1_02(GPIO1_IO18,ALT1)
    
    QTMR_GetDefaultConfig(&qtimer3_config);             //������ΪĬ�����ã������ڸ���ʵ���������
    qtimer3_config.primarySource=qtimer3_source;        //���õ�һʱ��Դ
    QTMR_Init(TMR3,kQTMR_Channel_2,&qtimer3_config);    //��ʼ��QTIMER
    
    //�������벶��
    QTMR_SetupInputCapture(TMR3,kQTMR_Channel_2,kQTMR_Counter2InputPin,0,0,kQTMR_RisingEdge);
        
    //��ʼͨ��0
    QTMR_StartTimer(TMR3, kQTMR_Channel_2,kQTMR_PriSrcRiseEdge);    //ͨ��0��primaryʱ��Դ�������ؼ��� 
}
  













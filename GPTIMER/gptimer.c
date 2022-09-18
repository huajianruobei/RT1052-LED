#include "gptimer.h"
#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK I.MX RT1052开发板
//GPT定时器驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2012/12/23
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

gpt_config_t gpt1_onfig;

//初始化GPTIMER1，时钟源为perclk_clk_root=75MHz
//pre:分频值，0~4096
//comcount:比较计数值，0~0xFFFFFFFF
//当ocrx==CNT时,产生中断.
//定时时间=ocrx*(psc+1)/PERCLK_CLK_ROOT
void GPT1_Int_Init(u16 psc,u32 ocrx)
{
	GPT_GetDefaultConfig(&gpt1_onfig);	//先初始化GPT1为默认值
	gpt1_onfig.clockSource=kGPT_ClockSource_Periph;	//初始化时钟源perclk_clk_root
	gpt1_onfig.divider=psc;	        //设置分频值
	GPT_Init(GPT1,&gpt1_onfig);
	
	GPT_SetOutputCompareValue(GPT1,kGPT_OutputCompare_Channel1,ocrx);	    //设置比较计数值
	GPT_EnableInterrupts(GPT1,kGPT_OutputCompare1InterruptEnable);			//使能GPT比较通道1中断
	RT1052_NVIC_SetPriority(GPT1_IRQn,5,0);									//抢占优先级6，子优先级0
	EnableIRQ(GPT1_IRQn);	//使能GPT1中断
	GPT_StartTimer(GPT1);	//开始定时器	
}


//初始化GPT2 CAP2输入捕获
//psc : 预分频器,0~4095
//最大计数时间=(2^32)*(psc+1)/PERCLK_CLK_ROOT=234562.5秒,基本上不可能溢出
void GPT2_CAP2_Init(u16 psc)
{
    gpt_config_t gpt2_onfig;
    
    //配置GPT2_CAP2相关IO(GPIO_EMC_40)的功能
	//高转换速度,驱动R0/6,速度为200Mhz，关闭开路功能，使能pull/keepr
	//选择pull功能，上拉22K Ohm 
    IOMUXC_SetPinMux(IOMUXC_GPIO_EMC_40_GPT2_CAPTURE2,0);	  
    IOMUXC_SetPinConfig(IOMUXC_GPIO_EMC_40_GPT2_CAPTURE2,0xF0F1);
    
    GPT_GetDefaultConfig(&gpt2_onfig);	//先初始化GPT2为默认值
    gpt2_onfig.divider=psc;	            //设置分频值
	gpt2_onfig.clockSource=kGPT_ClockSource_Periph;	//设置时钟源
	GPT_Init(GPT2,&gpt2_onfig);         //初始化GPT2
  
    //设置CAP2为下降沿捕获
    GPT_SetInputOperationMode(GPT2,kGPT_InputCapture_Channel2,kGPT_InputOperation_FallEdge);
    GPT_EnableInterrupts(GPT2,kGPT_InputCapture2InterruptEnable);   //使能GPT2捕获中断
    GPT_EnableInterrupts(GPT2,kGPT_RollOverFlagInterruptEnable);    //使能GPT2溢出中断
    GPT_StartTimer(GPT2);	            //开始定时器
    
    //中断设置
    RT1052_NVIC_SetPriority(GPT2_IRQn,7,0);//设置GPT2中断优先级
    EnableIRQ(GPT2_IRQn);	            //使能GPT2中断
}


//GPT1中断服务函数
void GPT1_IRQHandler(void)
{
    //OCR1中断
    if(GPT_GetStatusFlags(GPT1,kGPT_OutputCompare1Flag))
    {
        LED1_Toggle;			//LED1灯翻转
        GPT_ClearStatusFlags(GPT1,kGPT_OutputCompare1Flag);//清除中断标志位
    }
	__DSB();				//数据同步屏蔽指令
}


//捕获状态
//[7]:0,没有成功的捕获;1,成功捕获到一次.
//[6]:0,还没捕获到下降沿;1,已经捕获到下降沿了.
//[5:0]:捕获低电平后溢出的次数(对于32位定时器来说,1us计数器加1,溢出时间:42949.67秒)
u8  GPT2CAP2_CAPTURE_STA=0;	//输入捕获状态		    				
u32	GPT2CAP2_CAPTURE_VAL;	//输入捕获值(GPT2是32位)

//GPT2中断服务函数
void GPT2_IRQHandler(void)
{
 	if((GPT2CAP2_CAPTURE_STA&0X80)==0)//还未成功捕获	
	{
        if(GPT_GetStatusFlags(GPT2,kGPT_RollOverFlag))  //溢出中断
		{	     
			if(GPT2CAP2_CAPTURE_STA&0X40)               //已经捕获到高电平了
			{
				if((GPT2CAP2_CAPTURE_STA&0X3F)==0X3F)   //高电平太长了
				{
					GPT2CAP2_CAPTURE_STA|=0X80;		    //标记成功捕获了一次
					GPT2CAP2_CAPTURE_VAL=0XFFFFFFFF;
				}else GPT2CAP2_CAPTURE_STA++;
			}	 
		}
        if(GPT_GetStatusFlags(GPT2,kGPT_InputCapture2Flag))  //捕获2发生捕获事件
		{	
			if(GPT2CAP2_CAPTURE_STA&0X40)			//捕获到一个下降沿 		
			{	  			
				GPT2CAP2_CAPTURE_STA|=0X80;			//标记捕获到一次低电平脉宽
                //获取当前的捕获值.
                GPT2CAP2_CAPTURE_VAL=GPT_GetInputCaptureValue(GPT2,kGPT_InputCapture_Channel2);		
                //设置捕获下降沿 
                GPT_SetInputOperationMode(GPT2,kGPT_InputCapture_Channel2,kGPT_InputOperation_FallEdge);
			}else  									//还未开始,第一次捕获上升沿
			{
				GPT2CAP2_CAPTURE_STA=0;				//清空
				GPT2CAP2_CAPTURE_VAL=0;				//捕获值清零
				GPT2CAP2_CAPTURE_STA|=0X40;			//标记捕获到下降沿
                GPT_StopTimer(GPT2);                //关闭GPT2    
                //设置捕获上升沿 
                GPT_SetInputOperationMode(GPT2,kGPT_InputCapture_Channel2,kGPT_InputOperation_RiseEdge);
                GPT_StartTimer(GPT2);               //使能GPT2
			}		    
		}			     	    					   
 	}
    GPT_ClearStatusFlags(GPT2,kGPT_RollOverFlag);       //清除溢出中断标志位 
    GPT_ClearStatusFlags(GPT2,kGPT_InputCapture2Flag);  //清除捕获2中断标志位    
	__DSB();			//数据同步屏蔽指令 
}

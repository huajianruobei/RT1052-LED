#include "tpad.h"
#include "delay.h"		    
#include "lpuart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK I.MX RT1052开发板
//TPAD 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/3/26
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 
 
#define TPAD_CAP_MAX_VAL 0XFFFF			//捕获最大值
vu16 tpad_default_val=0;				//空载的时候(没有手按下),计数器需要的时间

//初始化触摸按键
//获得空载的时候触摸按键的取值.
//psc:分频系数,越小,灵敏度越高.
//返回值:0,初始化成功;1,初始化失败
u8 TPAD_Init(u8 psc)
{
	u16 buf[10];
	u16 temp;
	u8 j,i;
	QTMR3_CH2_CAP_Init(psc);	//设置分频系数
	for(i=0;i<10;i++)			//连续读取10次
	{				 
		buf[i]=TPAD_Get_Val();
		delay_ms(10);	    
	}				    
	for(i=0;i<9;i++)//排序
	{
		for(j=i+1;j<10;j++)
		{
			if(buf[i]>buf[j])//升序排列
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}
	temp=0;
	for(i=2;i<8;i++)temp+=buf[i];//取中间的6个数据进行平均
	tpad_default_val=temp/6;
	printf("tpad_default_val:%d\r\n",tpad_default_val);	
	if(tpad_default_val>TPAD_CAP_MAX_VAL/2)return 1;//初始化遇到超过TPAD_ARR_MAX_VAL/2的数值,不正常!
	return 0;		     	    					   
}

//复位一次
//释放电容电量，并清除定时器的计数值
void TPAD_Reset(void)
{
    gpio_pin_config_t tpad_config;	
    
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_02_GPIO1_IO18,0);	  
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_02_GPIO1_IO18,0x10B0);
    
	tpad_config.direction=kGPIO_DigitalOutput;	//输出
	tpad_config.interruptMode=kGPIO_NoIntmode;	//不使用中断功能
	tpad_config.outputLogic=1;					//默认高电平
	GPIO_PinInit(GPIO1,18,&tpad_config); 	    //初始化GPIO1_3
    GPIO_PinWrite(GPIO1,18,0);                  //GPIO1_IO18输出0,放电
    
	delay_ms(5);
    QTMR_ClearStatusFlags(TMR3,kQTMR_Channel_2,kQTMR_EdgeFlag); //清除边沿捕获标记
	TMR3->CHANNEL[kQTMR_Channel_2].CNTR=0;			            //归零    
    
    //配置QTIMER_TIMER3相关IO(GPIO_AD_B1_02)的功能
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_02_QTIMER3_TIMER2,0);	  
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_02_QTIMER3_TIMER2,0x10C1);
}

//得到定时器捕获值
//如果超时,则直接返回定时器的计数值.
//返回值：捕获值/计数值（超时的情况下返回）
u16 TPAD_Get_Val(void)
{				   
	TPAD_Reset();
    while((QTMR_GetStatus(TMR3,kQTMR_Channel_2)&(kQTMR_EdgeFlag))!=kQTMR_EdgeFlag)//等待捕获上升沿
	{
		if(TMR3->CHANNEL[kQTMR_Channel_2].CNTR>TPAD_CAP_MAX_VAL-500)
            return TMR3->CHANNEL[kQTMR_Channel_2].CNTR;//超时了,直接返回CNT的值
	};	
	return TMR3->CHANNEL[kQTMR_Channel_2].CAPT;	  
} 	

//读取n次,取最大值
//n：连续获取的次数
//返回值：n次读数里面读到的最大读数值
u16 TPAD_Get_MaxVal(u8 n)
{ 
	u16 temp=0; 
	u16 res=0; 
	u8 lcntnum=n*2/3;//至少2/3*n的有效个触摸,才算有效
	u8 okcnt=0;
	while(n--)
	{
		temp=TPAD_Get_Val();//得到一次值
		if(temp>(tpad_default_val*5/4))okcnt++;//至少大于默认值的5/4才算有效
		if(temp>res)res=temp;
	}
	if(okcnt>=lcntnum)return res;//至少2/3的概率,要大于默认值的5/4才算有效
	else return 0;
}  
//扫描触摸按键
//mode:0,不支持连续触发(按下一次必须松开才能按下一次);1,支持连续触发(可以一直按下)
//返回值:0,没有按下;1,有按下;										  
u8 TPAD_Scan(u8 mode)
{
	static u8 keyen=0;	//0,可以开始检测;>0,还不能开始检测	 
	u8 res=0;
	u8 sample=3;	//默认采样次数为3次	 
	u16 rval;
	if(mode)
	{
		sample=6;	//支持连按的时候，设置采样次数为6次
		keyen=0;	//支持连按	  
	}
	rval=TPAD_Get_MaxVal(sample); 
	if(rval>(tpad_default_val*4/3)&&rval<(10*tpad_default_val))//大于tpad_default_val+(1/3)*大于tpad_default_val,且小于10倍tpad_default_val,则有效
	{							 
		if(keyen==0)res=1;	//keyen==0,有效 
		//printf("r:%d\r\n",rval);		     	    					   
		keyen=3;				//至少要再过3次之后才能按键有效   
	} 
	if(keyen)keyen--;		   							   		     	    					   
	return res;
}	 

//初始化QTMR3定时器CH2 输入捕获 
//prisrc : 第一时钟源选择
//         0000~0011,通道0~3的输入引脚.
//         0100~0111,通道0~3的输出.可用于级联.
//         1000~1111,IPG_CLK_ROOT时钟的:1,2,4,8,16,32,64,128分频.
//捕获计时频率=QTMR3_CLK=IPG_CLK_ROOT/2^(prisrc-8); 
//假设prisrc=1011,则QTMR3_CLK=IPG_CLK_ROOT/8=18.75MHz. 
void QTMR3_CH2_CAP_Init(u8 prisrc)
{
    qtmr_config_t qtimer3_config;
    qtmr_primary_count_source_t qtimer3_source;
    
    u32 ipgclk=CLOCK_GetFreq(kCLOCK_IpgClk);     //获取IPG_CLK=150Mhz
    qtimer3_source=(qtmr_primary_count_source_t)prisrc;
    
    //IO设置    
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_02_QTIMER3_TIMER2,0);	  
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_02_QTIMER3_TIMER2,0x10C1);
	//IOMUXC->SELECT_INPUT[kIOMUXC_QTIMER3_TIMER2_SELECT_INPUT]=1;	//选择TMR3_CH2的输入接到GPIO_AD_B1_02(GPIO1_IO18,ALT1)
    
    QTMR_GetDefaultConfig(&qtimer3_config);             //先设置为默认配置，后面在根据实际情况配置
    qtimer3_config.primarySource=qtimer3_source;        //设置第一时钟源
    QTMR_Init(TMR3,kQTMR_Channel_2,&qtimer3_config);    //初始化QTIMER
    
    //设置输入捕获
    QTMR_SetupInputCapture(TMR3,kQTMR_Channel_2,kQTMR_Counter2InputPin,0,0,kQTMR_RisingEdge);
        
    //开始通道0
    QTMR_StartTimer(TMR3, kQTMR_Channel_2,kQTMR_PriSrcRiseEdge);    //通道0在primary时钟源的上升沿计数 
}
  













#include "xbar.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK I.MX RT1052开发板
//XBAR驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/1/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//设置XBARA1的信号连接关系
//input: XBARA_INn选择,低8位有小,范围:0~87,参见xbar_input_signal_t的枚举值.
//output: XBARA_OUTx编号,低8位有效,范围:0~130, 参见xbar_output_signal_t的枚举值.
//该函数设置XBARA1_OUTx(output)对应的输入信号是哪个XBARA1_INn(input).
//详细的对应表,见:<<RT1052英文参考手册>>Table 3-5 和 Table 3-6
void XBARA1_Signal_Set(xbar_input_signal_t input, xbar_output_signal_t output)
{ 
	u8 outx,inx;
  
	outx=output&0XFF;									//得到真正的XBARA_OUT编号.
	inx=input&0XFF;										//得到真正的XBARA_IN编号. 
	if(outx>3&&outx<20)IOMUXC_GPR->GPR6|=1<<(12+outx);	//GPIO做输出,则设置对应的I0为输出模式
	if(inx>3&&inx<20)IOMUXC_GPR->GPR6&=~(1<<(12+inx));	//GPIO做输入,则设置对应的IO位输入模式 
    
    XBARA_Init(XBARA1);                                 //初始化XBARA1
    XBARA_SetSignalsConnection(XBARA1,input,output);    //设置输入和输出连接
}

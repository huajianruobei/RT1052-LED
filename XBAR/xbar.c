#include "xbar.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK I.MX RT1052������
//XBAR��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/1/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//����XBARA1���ź����ӹ�ϵ
//input: XBARA_INnѡ��,��8λ��С,��Χ:0~87,�μ�xbar_input_signal_t��ö��ֵ.
//output: XBARA_OUTx���,��8λ��Ч,��Χ:0~130, �μ�xbar_output_signal_t��ö��ֵ.
//�ú�������XBARA1_OUTx(output)��Ӧ�������ź����ĸ�XBARA1_INn(input).
//��ϸ�Ķ�Ӧ��,��:<<RT1052Ӣ�Ĳο��ֲ�>>Table 3-5 �� Table 3-6
void XBARA1_Signal_Set(xbar_input_signal_t input, xbar_output_signal_t output)
{ 
	u8 outx,inx;
  
	outx=output&0XFF;									//�õ�������XBARA_OUT���.
	inx=input&0XFF;										//�õ�������XBARA_IN���. 
	if(outx>3&&outx<20)IOMUXC_GPR->GPR6|=1<<(12+outx);	//GPIO�����,�����ö�Ӧ��I0Ϊ���ģʽ
	if(inx>3&&inx<20)IOMUXC_GPR->GPR6&=~(1<<(12+inx));	//GPIO������,�����ö�Ӧ��IOλ����ģʽ 
    
    XBARA_Init(XBARA1);                                 //��ʼ��XBARA1
    XBARA_SetSignalsConnection(XBARA1,input,output);    //����������������
}

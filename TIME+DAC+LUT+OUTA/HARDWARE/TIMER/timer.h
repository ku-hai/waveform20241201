#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//��ʱ�� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/4
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//********************************************************************************

void V_aOUTPUT(int Vl_a);
void TIM3_Int_Init(u16 arr,u16 psc);
void TIM2_Int_Init(u16 arr_down,u16 psc_down);
void VL_aOUTPUT(int Vl_a);
void VH_aOUTPUT(int Vh_a);
#endif

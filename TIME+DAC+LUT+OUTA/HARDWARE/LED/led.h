#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
#define LED0 PCout(4)// PC4
#define LED1 PCout(5)// PC5	

void LED_Init(void);//��ʼ��
void SetBite_1_0(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, char a);
void LED_TEST(void); 				    
#endif

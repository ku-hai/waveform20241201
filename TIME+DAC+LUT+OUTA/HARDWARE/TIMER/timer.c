#include "timer.h"
#include "led.h"
#include "usart.h"
#include "DAC8563.h"
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
//V1.1 20120904
//1,����TIM3_PWM_Init������
//2,����LED0_PWM_VAL�궨�壬����TIM3_CH2����									  
//////////////////////////////////////////////////////////////////////////////////  
   	  
//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!

extern int cnt;
extern int VT1, VT2,out_flag;//
//extern u16 F_1,F_2,T_1,T_2;
//extern int pwm_cnt,cnt_0;
extern int Waveform[];
extern int length;
extern int time;
extern int flag;
void TIM3_Int_Init(u16 arr_up,u16 psc_up)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr_up; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc_up; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
    
	//�Զ���װ
    TIM_ARRPreloadConfig(TIM3,ENABLE);
    //����жϱ�־
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update); 
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж� 

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx����
							 
}


//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)   //TIM3�ж�
{
	 if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) 
	{
		int a = Waveform[time];
		int b = Waveform[time-1];
		
		// ������ִ����PWM�����ش���ʱϣ��ִ�е�����
		
		if(time < length)
		{
        if(a!=b)
				{
					LED0=!LED0;
					LED1=!LED1;
				}
				V_aOUTPUT(Waveform[time]);
		    time++;
		}
		else
		{
			V_aOUTPUT(Waveform[length-1]);
			TIM_Cmd(TIM3, DISABLE);
			TIM_SetCounter(TIM3,0);
			GPIO_SetBits(GPIOC, GPIO_Pin_4);
			GPIO_SetBits(GPIOC, GPIO_Pin_5);
			flag=0;
		}
		TIM_Cmd(TIM3, DISABLE);
		TIM_SetCounter(TIM3,0);
		TIM_Cmd(TIM3, ENABLE);
	  TIM_ClearITPendingBit(TIM3, TIM_IT_Update);// ����жϱ�־λ
}
}
//void TIM2_Int_Init(u16 arr_down,u16 psc_down)
// {
//     TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
//     RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
//     
//     TIM_DeInit(TIM2);
//     
//     TIM_TimeBaseStructure.TIM_Period = arr_down-1;//2000 - 1;
//     TIM_TimeBaseStructure.TIM_Prescaler = psc_down-1;
//     TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
//     TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
//     
//     TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
//     
//     TIM_ClearFlag(TIM2, TIM_FLAG_Update);
//     TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
//     
//     TIM_Cmd(TIM2, ENABLE);
//	 
//	 NVIC_InitTypeDef NVIC_InitStructure;
//	 NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2�ж�
//	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
//	 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //�����ȼ�3��
//	 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
//	 NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
// }
 
// 
// void TIM2_IRQHandler(void)
// {
//     
//     if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
//     {              
//		
//		 
//		 if(cnt0_down>cnt_down||cnt_0==0)
//		{
//			TIM_Cmd(TIM2, DISABLE);
//			TIM_SetCounter(TIM3,0);
//			TIM_Cmd(TIM3, ENABLE);
//			
//		}
//		else
//		{
//			TIM_Cmd(TIM2, DISABLE);
////			TIM_Cmd(TIM3, DISABLE);
//			TIM_SetCounter(TIM2,0);
//			TIM_SetCounter(TIM3,0);
//		}
//		 VL_aOUTPUT(Vl_a);
//     }
//      TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update);
// }
// 
 void SET_count(TIM_TypeDef* TIMx,u8 T)
 {
	 	TIM_ARRPreloadConfig(TIMx,DISABLE);
		TIM_SetAutoreload(TIMx,T*50-1);//arr=5000 Ϊ1s,H_aΪ100�� H_aΪ��λ x.xxs
		TIM_ARRPreloadConfig(TIMx,ENABLE);
		TIM_SetCounter(TIMx,0);
 }
 
 void VH_aOUTPUT(int Vh_a)//Vh_a��0-40
 {
	if(Vh_a==0){DAC_OutA(0);}
	else
	{
		DAC_OutA(Vh_a*1638.4-1);
	}
	GPIO_ResetBits(GPIOC, GPIO_Pin_4);
	GPIO_SetBits(GPIOC, GPIO_Pin_5);
//	cnt_up++;	 
 }

 void VL_aOUTPUT(int Vl_a)
 {
	if(Vl_a==0){DAC_OutA(0);}
	else
	{
		DAC_OutA(Vl_a*1638.4-1);
	}
	GPIO_SetBits(GPIOC, GPIO_Pin_4);
	GPIO_ResetBits(GPIOC, GPIO_Pin_5);
//	cnt_down++;	 
 }

 void V_aOUTPUT(int Vl_a)
 {
	if(Vl_a==0){DAC_OutA(0);}
	else
	{
		DAC_OutA(Vl_a*1638.4-1);
	}
 }

#include "timer.h"
#include "led.h"
#include "usart.h"
#include "DAC8563.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/4
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//********************************************************************************
//V1.1 20120904
//1,增加TIM3_PWM_Init函数。
//2,增加LED0_PWM_VAL宏定义，控制TIM3_CH2脉宽									  
//////////////////////////////////////////////////////////////////////////////////  
   	  
//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!

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

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period = arr_up; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc_up; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
    
	//自动重装
    TIM_ARRPreloadConfig(TIM3,ENABLE);
    //清空中断标志
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update); 
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断 

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
							 
}


//定时器3中断服务程序
void TIM3_IRQHandler(void)   //TIM3中断
{
	 if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) 
	{
		int a = Waveform[time];
		int b = Waveform[time-1];
		
		// 在这里执行在PWM上升沿触发时希望执行的任务
		
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
	  TIM_ClearITPendingBit(TIM3, TIM_IT_Update);// 清除中断标志位
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
//	 NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2中断
//	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
//	 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //从优先级3级
//	 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
//	 NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
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
		TIM_SetAutoreload(TIMx,T*50-1);//arr=5000 为1s,H_a为100， H_a为三位 x.xxs
		TIM_ARRPreloadConfig(TIMx,ENABLE);
		TIM_SetCounter(TIMx,0);
 }
 
 void VH_aOUTPUT(int Vh_a)//Vh_a在0-40
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

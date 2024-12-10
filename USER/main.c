#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "oled.h"
#include "usart.h"	 	 
#include "timer.h"
#include "DAC8563.h"
#include "timer.h"





void set_out(void);
void OLED_Show(void);
void OLED_Show_model(void);
void OLED_Show_Num(void);
void Clear_all(void);
void CNT_PWM(void);

// PB5输出PWM
// PA4输出DAC（通过判断PB5引脚高低输出PA4，由DAC控制PA4的输出电压）
// ADC由PA1接受
// 由PE4 按键KEY0
// PA0 按键KEY2控制
// SCL PA_6
// SDA PA_7
// TIM3中断 输出高电平  TIM2中断输出低电平

u8 key;
u8 mode_0=1;
int  Vh_a=20;//Vh_a :A通道输出高电压，单位为V		Vh_a :B通道输出高电压，单位为V
int  V_b= 20, Vl_a=20;//V_b :A通道输出负电压，单位为V		
u32  H_a =100; //1.00s
u32  L_a= 100;

int cnt_0=0;//设置
u8 Ha[3], Hb[3], La[3], Lb[3], Cnt[3];

int cnt_up=0,cnt_down=0,pwm_cnt=0,cnt0_down,cnt0_up;//计数

int i=0,num=0,out_flag=1,flag_mode8=0;

char buf[] =  {" LA:   V Vb:   V"};
char buf1[] = {" HA:   V C:     "};
char buf2[] = {" H: .  s L: .  s"};	//H:为0.8-1000ms
char buf3[] = {" OUT:UP  OUT:OFF"};



char txt[16];	//需要在调用这个显示接口前声明txt这个变量
#define display(row, col, size, ...) {sprintf((char*)txt, __VA_ARGS__); OLED_ShowString(col*8, row*(size==16?2:1), txt, size);}
char str[30];	//显示缓存
extern u8 _return;



 int main(void)
   {		
	
	
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 //串口初始化为115200
	KEY_Init();			  //初始化按键程序
 	LED_Init();			     //LED端口初始化
	DAC8563_Init();
	delay_ms(100);
	DAC8563_WRITE(CMD_GAIN, DATA_GAIN_B2_A2);
	TIM3_Int_Init(500,14400);  //5000 1s 			不分频。clock频率=72000000Hz      time=(arr+1)*(psc+1)/72000000  psc=59999 arr=1119   Hz=72000000/(arr+1)*(psc+1)   Hz=1200/(arr+1)   
	TIM2_Int_Init(500,14400);
	TIM_Cmd(TIM2, DISABLE);
	TIM_Cmd(TIM3, DISABLE);
	OLED_Init();			//oled初始化
	OLED_Clear();           //清屏	   	
	delay_ms(100);
	DAC_OutAB(32767,32767);
	delay_ms(100);
	
	LED_TEST();

	OLED_ShowString(0,0,buf,16); //显示字符串
	OLED_ShowString(0,2,buf1,16); //显示字符串
	OLED_ShowString(0,4,buf2,16); //显示字符串
	OLED_ShowString(0,6,buf3,16);
	
	
	
	
	OLED_Show_Num();
	
	OLED_ShowChar(0,0,'*',16);
	OLED_ShowNum(104,0,00,2,16);//每个字占8个单元
	OLED_ShowNum(32,0,00,3,16);//每个字占8个单元
	OLED_ShowNum(32,2,00,3,16);//每个字占8个单元
	while(1)
	{	
		key=KEY_Scan(0);//按键扫描	
		
		// 	WKUP PB4, KEY0 PB6,  KEY1 PB7, KEY2  PB8
		if(key==KEY3_PRES)		//模式移动
		{
			mode_0++;
			if(mode_0>8)mode_0=1;
			i=1;num=0;cnt_up=0;cnt_down=0;pwm_cnt=0;flag_mode8=0;
			OLED_Show();
		}
		if(key==KEY2_PRES)//全部清除复位
		{
			Clear_all();

		}
		set_out();
		
	}	
 }

 /******************************************调用函数******************************************************************************/ 
 
 //输出模式

void Clear_all(void)
{
	Vh_a=20;//Vh_a :A通道输出高电压，单位为V		Vh_a :B通道输出高电压，单位为V
	V_b= 20, Vl_a=20;//V_b :A通道输出负电压，单位为V		
	H_a =100; //1.00s
	L_a= 100;
	cnt_0=0;
	pwm_cnt=0;
	cnt_up=0;
	cnt_down=0;
	TIM_Cmd(TIM2, DISABLE);
	TIM_Cmd(TIM3, DISABLE);
	DAC_OutB(20*1638.4-1);
	DAC_OutA(20*1638.4-1);
	mode_0=1;
	OLED_ShowChar(32,0,' ',16);
	OLED_ShowChar(96,0,' ',16);
	OLED_ShowChar(32,2,' ',16);
	OLED_ShowNum(40,0,Vh_a-20,2,16);
	OLED_ShowNum(104,0,V_b-20,2,16);
	OLED_ShowNum(40,2,20-Vh_a,2,16);
	OLED_ShowString(104,6,"OFF",16);
	OLED_Show_model();
	GPIO_SetBits(GPIOC, GPIO_Pin_4);
	GPIO_SetBits(GPIOC, GPIO_Pin_5);
}


void OLED_Show(void)
{
	OLED_Show_model();
	OLED_Show_Num();	
}
void OLED_Show_model(void)
{
	if(mode_0<9)
	{
		OLED_ShowChar(0,0,' ',16);
		OLED_ShowChar(0,2,' ',16);
		OLED_ShowChar(0,4,' ',16);
		OLED_ShowChar(0,6,' ',16);
		OLED_ShowChar(64,0,' ',16);
		OLED_ShowChar(64,2,' ',16);
		OLED_ShowChar(64,4,' ',16);
		OLED_ShowChar(64,6,' ',16);
		OLED_ShowChar((mode_0-1)%2*64,(mode_0-1)/2*2,'*',16);
	}
	else
	{
		OLED_ShowChar(0,0,' ',16);
		OLED_ShowChar(0,2,' ',16);
		OLED_ShowChar(0,4,' ',16);
		OLED_ShowChar(0,6,' ',16);
		OLED_ShowChar(64,0,' ',16);
		OLED_ShowChar(64,2,' ',16);
		OLED_ShowChar(64,4,' ',16);
		OLED_ShowChar(64,6,' ',16);
	}
}
void OLED_Show_Num(void)
{
    La[0] = L_a%1000/100;
	La[1] = L_a%100/10;
	La[2] = L_a%10;
	Ha[0] = H_a%1000/100;
	Ha[1] = H_a%100/10;
	Ha[2] = H_a%10;
	
	OLED_ShowNum(88,4,La[0],1,16);
	OLED_ShowNum(104,4,La[1],1,16);
	OLED_ShowNum(112,4,La[2],1,16);
	OLED_ShowNum(24,4,Ha[0],1,16);
	OLED_ShowNum(40,4,Ha[1],1,16);
	OLED_ShowNum(48,4,Ha[2],1,16);
	OLED_ShowNum(88,2,cnt_0,2,16);
	OLED_ShowNum(112,2,pwm_cnt,2,16);
}
void CNT_PWM(void)
{
		pwm_cnt=cnt_up;
		OLED_ShowNum(112,2,pwm_cnt,2,16);
}

void set_out(void)
{
	if(mode_0==1) //模式一：V_a电压调节
	{
		//调节V_a电压值
		if(key==KEY0_PRES)
		{
			Vh_a++;
			if(Vh_a>40)Vh_a=40;
			if(Vh_a>20)
			{
				OLED_ShowString(32,0,"+",16); //显示字符串
				OLED_ShowNum(40,0,Vh_a-20,2,16);//每个字占8个单元
			}
			else if(Vh_a==20)
			{	
				OLED_ShowString(32,0,"  0",16);
			} 
			else if(Vh_a<20)
			{
				OLED_ShowString(32,0,"-",16);
				OLED_ShowNum(40,0,20-Vh_a,2,16);//每个字占8个单元
			}
		}
		if(key==KEY1_PRES)
		{
			Vh_a--;
			if(Vh_a<0)Vh_a=0;
			if(Vh_a>20)
			{
				OLED_ShowString(32,0,"+",16); //显示字符串
				OLED_ShowNum(40,0,Vh_a-20,2,16);//每个字占8个单元
			}
			else if(Vh_a==20)
			{	
				OLED_ShowString(32,0,"  0",16);
			} 
			else if(Vh_a<20)
			{
				OLED_ShowString(32,0,"-",16);
				OLED_ShowNum(40,0,20-Vh_a,2,16);//每个字占8个单元
			}
		}
	}
	if(mode_0==2) //模式二：V_b电压调节
	{
		//调节V_a电压值
		if(key==KEY0_PRES)
		{
			V_b++;
			if(V_b>40)V_b=40;
			if(V_b>20)
			{
				OLED_ShowString(96,0,"+",16); //显示字符串
				OLED_ShowNum(104,0,V_b-20,2,16);//每个字占8个单元
			}
			else if(V_b==20)
			{	
				OLED_ShowString(96,0,"  0",16);
			} 
			else if(V_b<20)
			{
				OLED_ShowString(96,0,"-",16);
				OLED_ShowNum(104,0,20-V_b,2,16);//每个字占8个单元
			}
		}
		if(key==KEY1_PRES)
		{
			V_b--;
			if(V_b<0)V_b=0;
			if(V_b>20)
			{
				OLED_ShowString(96,0,"+",16); //显示字符串
				OLED_ShowNum(104,0,V_b-20,2,16);//每个字占8个单元
			}
			else if(V_b==20)
			{	
				OLED_ShowString(96,0,"  0",16);
			} 
			else if(V_b<20)
			{
				OLED_ShowString(96,0,"-",16);
				OLED_ShowNum(104,0,20-V_b,2,16);//每个字占8个单元
			}
		}	
	}
	if(mode_0 ==3) 
	{
		//调节V_a电压值
		if(key==KEY0_PRES)
		{
			Vl_a++;
			if(Vl_a>40)Vl_a=40;
			if(Vl_a>20)
			{
				OLED_ShowString(32,2,"+",16); //显示字符串
				OLED_ShowNum(40,2,Vl_a-20,2,16);//每个字占8个单元
			}
			else if(Vl_a==20)
			{	
				OLED_ShowString(32,2,"  0",16);
			} 
			else if(Vl_a<20)
			{
				OLED_ShowString(32,2,"-",16);
				OLED_ShowNum(40,2,20-Vl_a,2,16);//每个字占8个单元
			}
		}
		if(key==KEY1_PRES)
		{
			Vl_a--;
			if(Vl_a<0)Vl_a=0;
			if(Vl_a>20)
			{
				OLED_ShowString(32,2,"+",16); //显示字符串
				OLED_ShowNum(40,2,Vl_a-20,2,16);//每个字占8个单元
			}
			else if(Vl_a==20)
			{	
				OLED_ShowString(32,2,"  0",16);
			} 
			else if(Vl_a<20)
			{
				OLED_ShowString(32,2,"-",16);
				OLED_ShowNum(40,2,20-Vl_a,2,16);//每个字占8个单元
			}
		}
	}
	if(mode_0 ==4)
	{	
		Cnt[0] = cnt_0%100/10;
		Cnt[1] = cnt_0%10;
		
		i %=2;
		num++;
		if(key==KEY0_PRES)
		{
			i++;
		}
		if(key==KEY1_PRES)
		{
			Cnt[i]++;
			if(Cnt[i]>9){Cnt[i]=0;}
			cnt_0= Cnt[0]*10 + Cnt[1];
			cnt0_down=cnt_0;
			cnt0_up=cnt_0;
		}
		if(num==1)
		{
				OLED_ShowNum(88,2,cnt_0,2,16);
		}
		if(num==2)
		{
			OLED_ShowString(88+i*8,2," ",16);
			num=0;
		}
		
		
	}
	if(mode_0 ==5)// Ha
	{	
		Ha[0] = H_a%1000/100;
		Ha[1] = H_a%100/10;
		Ha[2] = H_a%10;
		
		i %=3;
		num++;
		if(key==KEY0_PRES)
		{
			i++;
		}
		if(key==KEY1_PRES)
		{
			Ha[i]++;
			if(Ha[i]>9){Ha[i]=0;}
			H_a= Ha[0]*100 + Ha[1]*10 + Ha[2];
			
		}
		if(num==1)
		{
				OLED_ShowNum(24,4,Ha[0],1,16);
				OLED_ShowNum(40,4,Ha[1],1,16);
				OLED_ShowNum(48,4,Ha[2],1,16);
		}
		if(num==2)
		{
			if(i==0)
			{OLED_ShowString(24,4," ",16);}
			else
			{OLED_ShowString(32+i*8,4," ",16);}
			
			OLED_ShowString(56,4,"s",16);
			num=0;
		}
		TIM_ARRPreloadConfig(TIM3,DISABLE);
		TIM_SetAutoreload(TIM3,H_a*50);//arr=5000 为1s,H_a为100， H_a为三位 x.xxs
		TIM_ARRPreloadConfig(TIM3,ENABLE);
		TIM_SetCounter(TIM3,0);
		
	}
	if(mode_0 ==6)//
	{
	    La[0] = L_a%1000/100;
		La[1] = L_a%100/10;
		La[2] = L_a%10;
		i %=3;
		num++;
		if(key==KEY0_PRES)
		{
			i++;
		}
		if(key==KEY1_PRES)
		{
			La[i]++;
			if(La[i]>9){La[i]=0;}
			L_a= La[0]*100 + La[1]*10 + La[2];
		}
		if(num==1)
		{
			OLED_ShowNum(88,4,La[0],1,16);
			OLED_ShowNum(104,4,La[1],1,16);
			OLED_ShowNum(112,4,La[2],1,16);
		}
		if(num==2)
		{
			if(i==0)
			{OLED_ShowString(88,4," ",16);}
			else
			{OLED_ShowString(96+i*8,4," ",16);}
			
			OLED_ShowString(120,4,"s",16);
			num=0;
		}
		TIM_ARRPreloadConfig(TIM2,DISABLE);
		TIM_SetAutoreload(TIM2,L_a*50);//arr=5000 为1s,H_a为100， H_a为三位 x.xxs
		TIM_ARRPreloadConfig(TIM2,ENABLE);
		TIM_SetCounter(TIM2,0);
	}
	
	
	if(mode_0 ==7)//  
	{
		i %=2;
		num++;
		
		if(key==KEY0_PRES)
		{
			i++;
			if(i==1)
			{
				OLED_ShowString(40,6,"   ",16);
				OLED_ShowString(40,6,"UP",16);
				out_flag=0;
			}
			else
			{
				OLED_ShowString(40,6,"   ",16);
				OLED_ShowString(40,6,"DWN",16);
				out_flag=1;
			}
		}
		if(key==KEY1_PRES)
		{
			La[i]++;
			if(La[i]>9){La[i]=0;}
			L_a= La[0]*100 + La[1]*10 + La[2];

		}
	
	}
	
	if(mode_0 ==8)//OUTPUT输出选择
	{
		
		num++;
		i %=2;
		CNT_PWM();
		if(key==KEY0_PRES)//暂停
		{
			i++;
			if(i==1)
			{
				OLED_ShowString(104,6,"   ",16);
				OLED_ShowString(104,6,"OFF",16);
				TIM_SetCounter(TIM2,0);
				TIM_SetCounter(TIM3,0);
				TIM_Cmd(TIM2, DISABLE);
				TIM_Cmd(TIM3, DISABLE);
				
			}
			else/////////////////////////////////////////////////输出模式
			{	
				DAC_OutB(V_b*1638.4-1);
				OLED_ShowString(104,6,"   ",16);
				OLED_ShowString(104,6,"ON",16);
				if(out_flag==1){cnt0_up--;out_flag=3;}
				if(out_flag==0){cnt0_down--;out_flag=3;}
				TIM_Cmd(TIM2, ENABLE);
				TIM_Cmd(TIM3, ENABLE);
//				if(flag_mode8==0)
//				{
//					VH_aOUTPUT();
//					flag_mode8=3;
//				}
			}
		}
		
	}

}

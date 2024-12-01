/**********************************************************
                       康威科技
更多电子需求，请到淘宝店，康威电子竭诚为您服务 ^_^
https://kvdz.taobao.com/
**********************************************************/


#include "task_manage.h"
#include "delay.h"
#include "DAC8563.h"
#include "key.h"
#include <stdio.h>
#include <ctype.h>
#include <cstring>

#define OUT_KEY  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)//读取按键0
#define FLASH_SAVE_ADDR  0x0801F000  				//设置FLASH 保存地址(必须为偶数)


u8 Firt_In = 1;
u8 Task_Index = 0;
u8 _return=0;



void Copybuf2dis(u8 *source, u8 dis[10], u8  dispoint)
{
	dis[0] = *source + 		 '0';
	dis[1] = *(source+1) + '0';
	dis[2] = *(source+2) + '0';
	dis[3] = *(source+3) + '0';
	dis[4] = *(source+4) + '0';
	dis[5] = 0;

	dis[dispoint] += 128;
}

void Copybuf2disbai(u8 *source, u8 dis[10])//无光标
{
		dis[0] = *source + 		 '0';
	dis[1] = *(source+1) + '0';
	dis[2] = *(source+2) + '0';
	dis[3] = *(source+3) + '0';
	dis[4] = *(source+4) + '0';
	dis[5] = 0;

}

void Set_PointFre(u32 Key_Value, u8* Task_ID)
{
	static u8 P_Index1 = 0,P_Index2 = 0, R_Index = 1,modAB=0;
	static u32 Fre1 = 65535,Fre2 = 65535;
	
	u8 fre_buf1[6],fre_buf2[6];
	u8 display[15];



	fre_buf1[0] = Fre1%100000/10000;
	fre_buf1[1] = Fre1%10000/1000;
	fre_buf1[2] = Fre1%1000/100;
	fre_buf1[3] = Fre1%100/10;
	fre_buf1[4] = Fre1%10;
	
	fre_buf2[0] = Fre2%100000/10000;
	fre_buf2[1] = Fre2%10000/1000;
	fre_buf2[2] = Fre2%1000/100;
	fre_buf2[3] = Fre2%100/10;
	fre_buf2[4] = Fre2%10;
	DAC_OutAB(Fre1,Fre2);
	switch(Key_Value)
	{
		case K_4_S: if(R_Index==0)
									fre_buf1[P_Index1]++;
								else if(R_Index==1)
									fre_buf2[P_Index2]++;break;
		case K_4_L: if(R_Index==0)
									fre_buf1[P_Index1]++;
								else if(R_Index==1)
									fre_buf2[P_Index2]++;break;	
								
		case K_5_L: if(R_Index==0)P_Index1++;
								else if(R_Index==1)P_Index2++;
								break;
		case K_5_S: if(R_Index==0)P_Index1++;
								else if(R_Index==1)P_Index2++;
								break;
		case K_1_L:if(R_Index==0)P_Index1--;
								else if(R_Index==1)P_Index2--;
								break;
		case K_1_S: if(R_Index==0)P_Index1--;
								else if(R_Index==1)P_Index2--;
								break;
								
		case K_3_S:  if(R_Index==0)
									fre_buf1[P_Index1]--;
								else if(R_Index==1)
									fre_buf2[P_Index2]--;break;	
		case K_3_L:  if(R_Index==0)
									fre_buf1[P_Index1]--;
								else if(R_Index==1)
									fre_buf2[P_Index2]--;break;	
		case K_2_L: modAB++;break;
		case K_2_S: R_Index^=1;break;
	}
	modAB %=3;
	P_Index1 %= 5;
	P_Index2 %= 5;
	if(R_Index==0)
	{
		if(fre_buf1[P_Index1] == 255) fre_buf1[P_Index1] = 9;
		if(fre_buf1[P_Index1] ==  10) fre_buf1[P_Index1] = 0;
	}
	else if(R_Index==1)
	{
		if(fre_buf2[P_Index2] == 255) fre_buf2[P_Index2] = 9;
		if(fre_buf2[P_Index2] ==  10) fre_buf2[P_Index2] = 0;
	}
	
	if(Key_Value != K_NO)
	{
		Fre1 = fre_buf1[0]*10000 + fre_buf1[1]*1000 + fre_buf1[2]*100+ fre_buf1[3]*10 + fre_buf1[4];
		Fre2 = fre_buf2[0]*10000 + fre_buf2[1]*1000 + fre_buf2[2]*100+ fre_buf2[3]*10 + fre_buf2[4];
		if(Fre1>65535) Fre1=65535;
		if(Fre2>65535) Fre2=65535;
		fre_buf1[0] = Fre1%100000/10000;
		fre_buf1[1] = Fre1%10000/1000;
		fre_buf1[2] = Fre1%1000/100;
		fre_buf1[3] = Fre1%100/10;
		fre_buf1[4] = Fre1%10;
	
		fre_buf2[0] = Fre2%100000/10000;
		fre_buf2[1] = Fre2%10000/1000;
		fre_buf2[2] = Fre2%1000/100;
		fre_buf2[3] = Fre2%100/10;
		fre_buf2[4] = Fre2%10;
		if(R_Index==0)
		{
			Copybuf2disbai(fre_buf2, display);
			OLED_ShowString(74, 4, display);
			Copybuf2dis(fre_buf1, display, P_Index1);
			OLED_ShowString(15, 4, display);
			//设置电压
			if(modAB==0)
			{
				DAC8563_WRITE(CMD_GAIN, DATA_GAIN_B2_A1);		// Set Gains A:+10 B:±10
				LCD_Show_CEStr(15,2,"+");
				LCD_Show_CEStr(66,2,"±");
			}			
			else if(modAB==1)
			{
				DAC8563_WRITE(CMD_GAIN, DATA_GAIN_B1_A1);		// Set Gains A:+10 B:+10
				LCD_Show_CEStr(15,2,"+");
				LCD_Show_CEStr(75,2,"+");
			}
			else if(modAB==2)
			{
				DAC8563_WRITE(CMD_GAIN, DATA_GAIN_B2_A2);		// Set Gains A:±10 B:±10
				LCD_Show_CEStr(6,2,"±");
				LCD_Show_CEStr(66,2,"±");
			}
				
			DAC_OutAB(Fre1,Fre2);
		}
		else if(R_Index==1)
		{
			Copybuf2disbai(fre_buf1, display);
			OLED_ShowString(15, 4, display);
			Copybuf2dis(fre_buf2, display, P_Index2);
			OLED_ShowString(74, 4, display);
			//设置电压
			if(modAB==0)
			{
				DAC8563_WRITE(CMD_GAIN, DATA_GAIN_B2_A1);		// Set Gains A:+10 B:±10
				LCD_Show_CEStr(15,2,"+");
				LCD_Show_CEStr(66,2,"±");
			}			
			else if(modAB==1)
			{
				DAC8563_WRITE(CMD_GAIN, DATA_GAIN_B1_A1);		// Set Gains A:+10 B:+10
				LCD_Show_CEStr(15,2,"+");
				LCD_Show_CEStr(75,2,"+");
			}
			else if(modAB==2)
			{
				DAC8563_WRITE(CMD_GAIN, DATA_GAIN_B2_A2);		// Set Gains A:±10 B:±10
				LCD_Show_CEStr(6,2,"±");
				LCD_Show_CEStr(66,2,"±");
			}
			
			DAC_OutAB(Fre1,Fre2);
		}
		
		_return=1;
	}
}



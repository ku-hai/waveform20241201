#ifndef __task_manage_H
#define __task_manage_H
#include "stm32f10x.h"
#include "oled.h"
#include "key.h"

#define Interface 2
#define StrMax    10//缓存数据大小

extern u32 SysTimer;

u8 Task_Delay(u32 delay_time, u8* delay_ID);
u8 TaskCycleDelay(u32 delay_time, u8* Last_delay_ID, u8* Self_delay_ID);
void welcome_KW(void);
void Main_Menu(u32 Key_Value, u8* Task_ID);
void LCD_Show_ModeCEInfo(u16 x0, u8 start_info, u8 current_deal_info);
void Set_PointFre(u32 Key_Value, u8* Task_ID);
void Sweep_Fre(u32 Key_Value, u8* Task_ID);
void Move_Pha(u32 Key_Value, u8* Task_ID);
void Jump_Fre(u32 Key_Value, u8* Task_ID);

void Copybuf2dis(u8 *source, u8 dis[10], u8  dispoint);

#endif

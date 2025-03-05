#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "oled.h"
#include "usart.h"	 	 
#include "timer.h"
#include "DAC8563.h"
#include "timer.h"

/*
�޸Ĵ��룬������Ҫ�����ڲ�����ʾ��Ȼ��������¼����������ͨ��������С���ں�����lut���޸���ʾ�Ĵ���

*/


//����ĺ�������
void set_out(void);
void OLED_Show(void);
void OLED_Show_model(void);
void OLED_Show_Num(void);
void Clear_all(void);
void CNT_PWM(void);
void start(void);
void para_display(void);
void end(void);
void select_display(void);
void generate_waveform(int input[][4], int num_inputs, int *Waveform, int *length) ;
// PB5���PWM
// PA4���DAC��ͨ���ж�PB5���Ÿߵ����PA4����DAC����PA4�������ѹ��
// ADC��PA1����
// ��PE4 ����KEY0
// PA0 ����KEY2����
// SCL PA_6
// SDA PA_7
// TIM3�ж� ����ߵ�ƽ  TIM2�ж�����͵�ƽ


int Period = 20; //��С����Ϊ20ms,һ֡�೤����λΪms��
int Waveform[5000];//�˴�����ʹ�ýű���������  �����Ϊ100s
int length;//������鳤��

int out_mode=0,flag,flag_start;
u8 key;
u8 mode_0=1;
int  V1_T1,V1_T2,V2_T1,V2_T2,V3_T1,V3_T2;//�趨��һ��������ڶ������������������ĵ�ѹ
int  R1,R2,R3,M1,M2,M3;//

int time = 0;
int cnt_0=0;//����
u8 V1_T1_display[3], V1_T2_display[3], V1_T2_display[3],V2_T1_display[3],V2_T2_display[3],V3_T1_display[3],V3_T2_display[3];
u8 R1_display[2], R2_display[2], R3_display[2], M1_display[2], M2_display[2], M3_display[2];

//int cnt_up=0,cnt_down=0,pwm_cnt=0,cnt0_down,cnt0_up;//����

int i=0,num_cnt=0,out_flag=1,flag_mode8=0;

char buf[] =  {"V1,V2,Re,Mu,  ms"};
char buf1[] = {"   V,   V,  ,   "};
char buf2[] = {"   V,   V,  ,   "};
char buf3[] = {"   V,   V,  ,   "};



char txt[16];	//��Ҫ�ڵ��������ʾ�ӿ�ǰ����txt�������
#define display(row, col, size, ...) {sprintf((char*)txt, __VA_ARGS__); OLED_ShowString(col*8, row*(size==16?2:1), txt, size);}
char str[30];	//��ʾ����
extern u8 _return;

//int input[][4] = {
//       {15, -15, 5,10},
//       {15, -15,25, 1},
//       { 0,  0, 1, 1}
//   };

int input[][4] = {
       {-15,	-15, 	1,	5},
       {0	,		0, 		1,	50},
       {15, 	15,		1, 	3},
       {-5,		-5,		1, 	2},
       {0,		0,		1,	50},
       {15, 	15,		1,	3},
       {3,		3,		1,  2},
       {0,		0, 		1,	50},
       {15,		15, 	1,	5},
       {0,  	0, 		1, 	50}
   };


int num_inputs;
 int main(void)
{		
	
/////////////////////��Ҫ�ĳ�ʼ��	
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 //���ڳ�ʼ��Ϊ115200
	KEY_Init();			  //��ʼ����������
 	LED_Init();			     //LED�˿ڳ�ʼ��
	DAC8563_Init();
	delay_ms(100);
	DAC8563_WRITE(CMD_GAIN, DATA_GAIN_B2_A2);
	TIM3_Int_Init(5*Period-1,14400-1);  //5000 1s 			����Ƶ��clockƵ��=72000000Hz      time=(arr+1)*(psc+1)/72000000  psc=14400 arr=500   Hz=72000000/(arr+1)*(psc+1)   Hz=1200/(arr+1)   
//	TIM2_Int_Init(500,14400-1);
//	TIM_Cmd(TIM2, DISABLE);
	TIM_Cmd(TIM3, DISABLE);
	OLED_Init();			//oled��ʼ��
	OLED_Clear();           //����	   	
	delay_ms(100);
	DAC_OutAB(32767,32767);
	delay_ms(100);
	
	LED_TEST();
//////////////////////OLED��ʼ������ʾ
	OLED_ShowString(0,0,buf,16); //��ʾ�ַ���
	OLED_ShowString(0,2,buf1,16); //��ʾ�ַ���
	OLED_ShowString(0,4,buf2,16); //��ʾ�ַ���
	OLED_ShowString(0,6,buf3,16);
		
	///////////////////////////////////////��ʼ�Ĳ��Σ������޸����������һ����ǰ50%�ĵ�ѹ���ڶ����Ǻ�50%�ĵ�ѹ����������ÿ����ѹ�ĳ���֡�������ĸ������ڵ��ظ�����

	V1_T1 = input[0][0]+20;
	V1_T2 = input[0][1]+20;
	V2_T1 = input[1][0]+20;
	V2_T2 = input[1][1]+20;
	V3_T1 = input[2][0]+20;
	V3_T2 = input[2][1]+20;
  R1    = input[0][2];
	R2    = input[1][2];
	R3    = input[2][2];
	M1    = input[0][3];
	M2    = input[1][3];
	M3    = input[2][3];
	para_display();
	num_inputs = sizeof(input) / sizeof(input[0]); // 
	generate_waveform(input, num_inputs, Waveform, &length);
  flag =0;
////////////////////////////////////////����ִ��
	while(1)
	{	
		key=KEY_Scan(0);//����ɨ��	
		
		// 	WKUP PB4, KEY0 PB6,  KEY1 PB7, KEY2  PB8
		if(key==KEY0_PRES)		//ģʽ�ƶ�
		{
			flag =0;
			mode_0++;
			if(mode_0>13)mode_0=1;
//			OLED_Show();
		}
		if(key==KEY3_PRES)//���ģʽ
		{
			flag =~flag;//�����ֵ(flag=1һֱ�ظ�ˢ�£�����ֹͣˢ��)
			if(flag)flag_start=1;
			//start();//�������
			para_display();//��ʾ���ݹ̶�
//			out_mode = ~out_mode;
//			if(out_mode){start();para_display();}
//			else{end();}
		}
    if(flag_start){flag_start=0;start();para_display();}//���ģʽ����ʾ���ݹ̶�
	  else{select_display();}//ѡ��ģʽ����ʾѡ��������˸
		set_out();//ģʽѡ�����������
		
	}	
}

 /******************************************���ú���******************************************************************************/ 


/*
int input[][4] = {
       {15, -15, 4, 3},
       {12, -12, 3, 2},
       {15,  15, 1, 1}
   };
num_inputs = sizeof(input) / sizeof(input[0]);
�����ֵwaveformΪ[15,15,15,15,-15,-15,-15,-15,15,15,15,15,-15,-15,-15,-15,12,12,12,-12,-12,-12,12,12,12,-12,-12,-12,15,15]
��һ�ε�{15, -15, 4, 3},Ϊ15������֡��-15������֡���������ظ����Σ��������waveform�����鳤�ȵ�length
*/

//�����������ת����LUT������waveform��
void generate_waveform(int input[][4], int num_inputs, int *Waveform, int *length) {
    int index = 0; // `Waveform` ��������

    for (int i = 0; i < num_inputs; i++) {
        int voltage1 = input[i][0]+20;   // ��ѹ1
        int voltage2 = input[i][1]+20;   // ��ѹ2
        int repeat = input[i][2];     // �ظ�����
        int multiplier = input[i][3]; // �ظ�����

        // ?? multiplier ���ظ�
        for (int m = 0; m < multiplier; m++) {
            // ? voltage1 �ظ� repeat ��
            for (int r = 0; r < repeat; r++) {
                Waveform[index++] = (char)voltage1;
            }
            // ? voltage2 �ظ� repeat ��
            for (int r = 0; r < repeat; r++) {
                Waveform[index++] = (char)voltage2;
            }
        }
    }

    *length = index; // ��Ʋ���ʵ�ʳ���
}


 //���ģʽ
//zheng fu de erweishu xianshide hanshu
// input (x,y,voltage,size),display on oled (x,y) output +/- voltage,
void pn_voltage_show(uint8_t x,uint8_t y, char voltage,uint8_t Size)
{
	if(voltage>20)
	{
		OLED_ShowString(x,y,"+",Size);
		OLED_ShowNum(x+8,y,voltage-20,2,Size);
	}
	else if(voltage==20)
		OLED_ShowString(x, y, "  0",Size);
	else if(voltage<20)
		{
			OLED_ShowString(x, y, "-",Size);
			OLED_ShowNum(x+8,y,20-voltage,2,Size);//ÿ����ռ8����Ԫ
		}
}

void start(void)//��ʼ
{
	time = 0; //count
	//���θı���lut����
	input[0][0] = V1_T1-20;
	input[0][1] = V1_T2-20;
	input[1][0] = V2_T1-20;
	input[1][1] = V2_T2-20;
	input[2][0] = V3_T1-20;
	input[2][1] = V3_T2-20;
  input[0][2] = R1;
	input[1][2] = R2;
	input[2][2] = R3;
	input[0][3] = M1;
	input[1][3] = M2;
	input[2][3] = M3;
	num_inputs = sizeof(input) / sizeof(input[0]); // 
	generate_waveform(input, num_inputs, Waveform, &length);

	VL_aOUTPUT(Waveform[time]);//output the first waveform
	
	//��ʱ����ز���
	TIM_Cmd(TIM3, DISABLE);  //timer3 disable
	TIM_ARRPreloadConfig(TIM3,DISABLE);
	TIM_SetAutoreload(TIM3,5*Period-1);
	TIM_ARRPreloadConfig(TIM3,ENABLE);
	TIM_SetCounter(TIM3,0);  //reset tim3 count
	TIM_Cmd(TIM3, ENABLE);//timer3 enable
}
void end(void)//����
{
	time=0;
	DAC_OutA(32767);//output 0 voltage
	TIM_Cmd(TIM3, DISABLE); //timer3 disable
}


//void OLED_Show(void)
//{
//	OLED_Show_model();
//	OLED_Show_Num();	
//}
//void OLED_Show_model(void)
//{
//	if(mode_0<9)
//	{
//		OLED_ShowChar(0,0,' ',16);
//		OLED_ShowChar(0,2,' ',16);
//		OLED_ShowChar(0,4,' ',16);
//		OLED_ShowChar(0,6,' ',16);
//		OLED_ShowChar(64,0,' ',16);
//		OLED_ShowChar(64,2,' ',16);
//		OLED_ShowChar(64,4,' ',16);
//		OLED_ShowChar(64,6,' ',16);
//		OLED_ShowChar((mode_0-1)%2*64,(mode_0-1)/2*2,'*',16);
//	}
//	else
//	{
//		OLED_ShowChar(0,0,' ',16);
//		OLED_ShowChar(0,2,' ',16);
//		OLED_ShowChar(0,4,' ',16);
//		OLED_ShowChar(0,6,' ',16);
//		OLED_ShowChar(64,0,' ',16);
//		OLED_ShowChar(64,2,' ',16);
//		OLED_ShowChar(64,4,' ',16);
//		OLED_ShowChar(64,6,' ',16);
//	}
//}
//void OLED_Show_Num(void)
//{
//  La[0] = L_a%1000/100;
//	La[1] = L_a%100/10;
//	La[2] = L_a%10;
//	Ha[0] = H_a%1000/100;
//	Ha[1] = H_a%100/10;
//	Ha[2] = H_a%10;
//	
//	OLED_ShowNum(88,4,La[0],1,16);
//	OLED_ShowNum(104,4,La[1],1,16);
//	OLED_ShowNum(112,4,La[2],1,16);
//	OLED_ShowNum(24,4,Ha[0],1,16);
//	OLED_ShowNum(40,4,Ha[1],1,16);
//	OLED_ShowNum(48,4,Ha[2],1,16);
//	OLED_ShowNum(88,2,cnt_0,2,16);
//	OLED_ShowNum(112,2,pwm_cnt,2,16);
//}
//void CNT_PWM(void)
//{
//		pwm_cnt=cnt_up;
//		OLED_ShowNum(112,2,pwm_cnt,2,16);
//}

void select_display(void)//ѡ��������˸
{
	if(mode_0==1)			 {OLED_ShowChar(104,0,' ',16);}
	else if(mode_0==2) {OLED_ShowChar( 16,2,' ',16);}
	else if(mode_0==3) {OLED_ShowChar( 56,2,' ',16);}
	else if(mode_0==4) {OLED_ShowChar( 88,2,' ',16);}
	else if(mode_0==5) {OLED_ShowChar(112,2,' ',16);}
	else if(mode_0==6) {OLED_ShowChar( 16,4,' ',16);}
	else if(mode_0==7) {OLED_ShowChar( 56,4,' ',16);}
	else if(mode_0==8) {OLED_ShowChar( 88,4,' ',16);}
	else if(mode_0==9) {OLED_ShowChar(112,4,' ',16);}
	else if(mode_0==10){OLED_ShowChar( 16,6,' ',16);}
	else if(mode_0==11){OLED_ShowChar( 56,6,' ',16);}
	else if(mode_0==12){OLED_ShowChar( 88,6,' ',16);}
	else if(mode_0==13){OLED_ShowChar(112,6,' ',16);}
}


void para_display(void)//��ʾ������ݹ̶�
{
	OLED_ShowNum(96,0,Period,2,16);
	pn_voltage_show(0,2, V1_T1,16);
	pn_voltage_show(40,2, V1_T2,16);
	OLED_ShowNum(80,2,R1,2,16);
	OLED_ShowNum(104,2,M1,2,16);
	pn_voltage_show(0,4, V2_T1,16);
	pn_voltage_show(40,4, V2_T2,16);	
	OLED_ShowNum(80,4,R2,2,16);
	OLED_ShowNum(104,4,M2,2,16);
	pn_voltage_show(0,6, V3_T1,16);
	pn_voltage_show(40,6, V3_T2,16);
	OLED_ShowNum(80,6,R3,2,16);
	OLED_ShowNum(104,6,M3,2,16);
}

void set_out(void)//ģʽѡ�����������
{
	if(mode_0==1) //ģʽһ��Period����
	{
		//������С֡����
		if(key==KEY1_PRES){Period++;}
		if(key==KEY2_PRES){Period--;}
		if(Period > 99)Period = 99;
		else if(Period < 0)Period = 0;
		OLED_ShowNum(96,0,Period,2,16);//ÿ����ռ8����Ԫ
	}
	if(mode_0==2) //
	{
		//���ڵ�ѹV1_T1
		if(key==KEY1_PRES){V1_T1++;}
		if(key==KEY2_PRES){V1_T1--;}
		if(V1_T1 > 40)V1_T1 = 40;
		else if(V1_T1 < 0)V1_T1 = 0;
		else V1_T1 =V1_T1;
		pn_voltage_show(0,2, V1_T1,16);
	}
	if(mode_0 ==3) 
	{
		//���ڵ�ѹV1_T2
		if(key==KEY1_PRES){V1_T2++;}
		if(key==KEY2_PRES){V1_T2--;}
		if(V1_T2 > 40)V1_T2 = 40;
		else if(V1_T2 < 0)V1_T2 = 0;
		else V1_T2 =V1_T2;
		pn_voltage_show(40,2, V1_T2,16);
	}
	if(mode_0 ==4)
	{	
		if(key==KEY1_PRES){R1++;}
		if(key==KEY2_PRES){R1--;}
		if(R1 > 99)R1 = 99;
		else if(R1 < 0)R1 = 0;
		else R1 =R1;
		OLED_ShowNum(80,2,R1,2,16);
	}
	if(mode_0 ==5)
	{	
		if(key==KEY1_PRES){M1++;}
		if(key==KEY2_PRES){M1--;}
		if(M1 > 99)M1 = 99;
		else if(M1 < 0)M1 = 0;
		else M1 =M1;
		OLED_ShowNum(104,2,M1,2,16);
	}
	if(mode_0==6) //
	{
		//���ڵ�ѹV1_T1
		if(key==KEY1_PRES){V2_T1++;}
		if(key==KEY2_PRES){V2_T1--;}
		if(V2_T1 > 40)V2_T1 = 40;
		else if(V2_T1 < 0)V2_T1 = 0;
		else V2_T1 =V2_T1;
		pn_voltage_show(0,4, V2_T1,16);
	}
	if(mode_0 ==7) 
	{
		//���ڵ�ѹV1_T2
		if(key==KEY1_PRES){V2_T2++;}
		if(key==KEY2_PRES){V2_T2--;}
		if(V2_T2 > 40)V2_T2 = 40;
		else if(V2_T2 < 0)V2_T2 = 0;
		else V2_T2 =V2_T2;
		pn_voltage_show(40,4, V2_T2,16);
	}
	if(mode_0 ==8)
	{	
		if(key==KEY1_PRES){R2++;}
		if(key==KEY2_PRES){R2--;}
		if(R2 > 99)R2 = 99;
		else if(R2 < 0)R2 = 0;
		else R2 =R2;
		OLED_ShowNum(80,4,R2,2,16);
	}
	if(mode_0 ==9)
	{	
		if(key==KEY1_PRES){M2++;}
		if(key==KEY2_PRES){M2--;}
		if(M2 > 99)M2 = 99;
		else if(M2 < 0)M2 = 0;
		else M2 =M2;
		OLED_ShowNum(104,4,M2,2,16);
	}
	if(mode_0==10) //
	{
		//���ڵ�ѹV1_T1
		if(key==KEY1_PRES){V3_T1++;}
		if(key==KEY2_PRES){V3_T1--;}
		if(V3_T1 > 40)V3_T1 = 40;
		else if(V3_T1 < 0)V3_T1 = 0;
		else V3_T1 =V3_T1;
		pn_voltage_show(0,6, V3_T1,16);
	}
	if(mode_0 ==11) 
	{
		//���ڵ�ѹV1_T2
		if(key==KEY1_PRES){V3_T2++;}
		if(key==KEY2_PRES){V3_T2--;}
		if(V3_T2 > 40)V3_T2 = 40;
		else if(V3_T2 < 0)V3_T2 = 0;
		else V3_T2 =V3_T2;
		pn_voltage_show(40,6, V3_T2,16);
	}
	if(mode_0 ==12)
	{	
		if(key==KEY1_PRES){R3++;}
		if(key==KEY2_PRES){R3--;}
		if(R3 > 99)R1 = 99;
		else if(R3 < 0)R3 = 0;
		else R3 =R3;
		OLED_ShowNum(80,6,R3,2,16);
	}
	if(mode_0 ==13)
	{	
		if(key==KEY1_PRES){M3++;}
		if(key==KEY2_PRES){M3--;}
		if(M3 > 99)M3 = 99;
		else if(M3 < 0)M3 = 0;
		else M3 =M3;
		OLED_ShowNum(104,6,M3,2,16);
	}
}

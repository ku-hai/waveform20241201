# waveform20241201
可用于ITO板的输出可控的电压波形（LUT）
使用stm32作为控制芯片
0.代码是在正点原子的例程基础上改的
1.实现功能为：有输出OUTA、输出OUTB和GND，三个输出端。输出OUTA方波电压，在-20V到20V可调，频率在1~200Hz内可调，占空比在0~100%可调。输出OUTB为直流电压，在-20V到20V可调。

2.使用到的外设与其对应功能
OLED：用于显示对应参数，显示按键控制的参数内容。
LED：对应显示波形的上半周期和下半周期，同时用于观察输出波形的情况。
DAC8563：用于数模转换，控制输出电压大小。
KEY0-3：用于参数调控
分别对应：
KEY0：参数切换
KEY1：加
KEY2：减
KEY3：波形输出

3.硬件设备对应的引脚接口
OLED（0.96寸）
	SCL	PA(6)
	SDA	PA(7）
LED×2
	LED1	PC(4)
	LED2	PC(5)
DAC8563:
	DIN		PC(8)
	SCLK	PC(12)
	SYNC	PC(11)
	CLR 	PC(10)
	LDAC	PC(9)

KEY0-3			
	KEY0	PB(6)
	KEY1	PB(7)
	KEY2	PB(8)
	KEY3	PB(4)

/*****************************************************************************
Copyright  : BGI
File name  : bsp_gpio.c
Description: GPIO引脚
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#include "bsp_gpio.h" 
#include "include.h"
#include "project_config.h"



//定义变量---------------------------------------------------------------------//


/*
 * @function: EXTI_NVIC_Config
 * @details : 配置外部中断
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
static void EXTI_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // 设置中断组为0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	// 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	// 设置主优先级为 1
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	// 设置抢占优先级为7
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * @function: ADCx_GPIO_Config
 * @details : ADC1引脚
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	
	EXTI_NVIC_Config();
	
	//打开GPIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	/* 其他，输入输出 */
	//检测光电传感器的输出信号(限位光耦?), PB0
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);		
	
	//MCU输出液面探测信号, PB1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* 拨码开关 */
	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	//PC13=SW1,PC14=SW2,PC15=SW3,
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//SW4=PA7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
				
	/* TMC2209 */	
	//SPREAD, PB3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB, GPIO_Pin_3);	
	//DIR, PB4
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB, GPIO_Pin_4);
	//EN, PA6
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_6);
	
	
	#if TMC2209_HARDWARE_TEST
		//STEP, TIM3_CH2,PA7
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_ResetBits(GPIOA, GPIO_Pin_7);
		
		//LED0
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_ResetBits(GPIOB, GPIO_Pin_5);
	#else
		//STEP, TIM3_CH2,PB5
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_ResetBits(GPIOB, GPIO_Pin_5);
	#endif
	
	//CLK, MCO, PA8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	GPIO_ResetBits(GPIOA, GPIO_Pin_8);

	//电机复位，检测光耦, PA15
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
//	//选择EXTI的信号源
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource15); 
//	EXTI_InitStructure.EXTI_Line = EXTI_Line15;
//	//EXTI为中断模式
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	//下降沿中断
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//	//使能中断
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);
	
	/* FDC2212 */
	//FDC2212的ADDR
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//FDC2212的INTB
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//选择EXTI的信号源
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource4); 
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	//EXTI为中断模式
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	//下降沿中断
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	//使能中断
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
		
	//FDC2212的SD
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
}


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
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);                       //PB3/PB4/PA15默认作为调试端口使用，要将其用作GPIO需要进行复用，因此要先开启复用时钟。
	
	
	//PB3、PB4作为普通引脚
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);                   //JTAG-DP Disabled and SW-DP Enabled 
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST, ENABLE);                       //Full SWJ Enabled (JTAG-DP + SW-DP) but without JTRST
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);                       //Full SWJ Disabled (JTAG-DP + SW-DP)，JTAG、SWD都无法仿真
	
	
	//系统运行指示灯
	GPIO_InitStructure.GPIO_Pin = SYS_LED_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SYS_LED_PORT, &GPIO_InitStructure);
	GPIO_SetBits(SYS_LED_PORT, SYS_LED_PIN);
	

	
	
	//检测吸头有无光耦信号, PB0
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
//	RCC_LSEConfig(RCC_LSE_OFF);    //关闭外部低速时钟,PC14,PC15用作普通IO
//	BKP_TamperPinCmd(DISABLE);     // 关闭入侵检测功能,PC13用作普通IO
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//	GPIO_Init(GPIOC, &GPIO_InitStructure);
//	//SW4=PA7
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
				
	/* TMC5130 */	
	//TMC CS, PB3
	GPIO_InitStructure.GPIO_Pin = TMC_SPI_CS_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(TMC_SPI_CS_PORT, &GPIO_InitStructure);
	GPIO_SetBits(TMC_SPI_CS_PORT, TMC_SPI_CS_PIN);
	
	//FLASH CS, PB12
	GPIO_InitStructure.GPIO_Pin = EEPROM_SPI_CS_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(EEPROM_SPI_CS_PORT, &GPIO_InitStructure);
	GPIO_SetBits(EEPROM_SPI_CS_PORT, EEPROM_SPI_CS_PIN);
//	
	//EN, PA6
	GPIO_InitStructure.GPIO_Pin = M0_EN_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(M0_EN_GPIO_Port, &GPIO_InitStructure);
	GPIO_SetBits(M0_EN_GPIO_Port, M0_EN_Pin);
	
	//CLK, MCO, PA8
#if 0
	//PA8输出低，使用TMC5130内部时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	GPIO_ResetBits(GPIOA, GPIO_Pin_8);

#else
	//使用通过PA8输出MCO,使用单片机晶振--HSE，16M
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	GPIO_ResetBits(GPIOA, GPIO_Pin_8);		
	//时钟选择
	RCC_MCOConfig(RCC_MCO_HSE);
		
#endif



//	//电机复位，检测光耦, PA15
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
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


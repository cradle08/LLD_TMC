/*****************************************************************************
Copyright  : BGI
File name  : bsp_adc.c
Description: ADC电压采样
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#include "bsp_adc.h" 
#include "include.h"



//定义变量---------------------------------------------------------------------//





/*
 * @function: ADCx_GPIO_Config
 * @details : ADC1引脚
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
static void ADCx_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// 打开 ADC IO端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	// 配置 ADC IO 引脚模式
	// 必须为模拟输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	
	//初始化 ADC IO
	GPIO_Init(GPIOA, &GPIO_InitStructure);				
}

/*
 * @function: ADCx_Mode_Config
 * @details : 配置ADC
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
static void ADCx_Mode_Config(void)
{
	ADC_InitTypeDef ADC_InitStructure;	
	
	
	// 打开ADC时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE );
	// 配置ADC时钟为PCLK2的8分频，即9MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	
	
	// ADC 模式配置
	// 只使用一个ADC，属于独立模式
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	
	// 禁止扫描模式，多通道才要，单通道不需要
	ADC_InitStructure.ADC_ScanConvMode = DISABLE ; 
	
	// 单次转换模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	
	// 不用外部触发转换，软件开启即可
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	
	// 转换结果右对齐
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	
	// 转换通道1个
	ADC_InitStructure.ADC_NbrOfChannel = 1;	
	
	// 初始化ADC
	ADC_Init(ADC1, &ADC_InitStructure);
	
	// 配置 ADC 通道转换顺序和采样时间
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1, ADC_SampleTime_239Cycles5);
	
	// ADC 转换结束产生中断，在中断服务程序中读取转换值
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
	
	// 开启ADC ，并开始转换
	ADC_Cmd(ADC1, ENABLE);
	
	
	
	// 初始化ADC 校准寄存器  
	ADC_ResetCalibration(ADC1);
	// 等待校准寄存器初始化完成
	while(ADC_GetResetCalibrationStatus(ADC1));
	
	// ADC开始校准
	ADC_StartCalibration(ADC1);
	// 等待校准完成
	while(ADC_GetCalibrationStatus(ADC1));
	
	// 由于没有采用外部触发，所以使用软件触发ADC转换 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}


/*
 * @function: ADC_NVIC_Config
 * @details : 配置ADC1/2中断
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
static void ADC_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	// 优先级分组
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	// 配置中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


/*
 * @function: ADCx_Init
 * @details : 初始化ADC
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void ADCx_Init(void)
{
	ADCx_GPIO_Config();
	ADCx_Mode_Config();
	ADC_NVIC_Config();
}

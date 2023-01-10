/*****************************************************************************
Copyright  : BGI
File name  : bsp_timer.c
Description: 定时器
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#include "bsp_timer.h" 
#include "include.h"



//定义变量---------------------------------------------------------------------//
struct tagSWSysTimeTick    SWSysTimeTick;




/*
 * @function: GENERAL_TIM_NVIC_Config
 * @details : 中断优先级配置
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
static void SWSysTimerNVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	// 设置中断组为0
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	// 设置中断来源
	NVIC_InitStructure.NVIC_IRQChannel = SW_SYS_TIMER_IRQ;
	// 设置主优先级为 0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	// 设置抢占优先级为3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


///*
// * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
// * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
// * 另外三个成员是通用定时器和高级定时器才有.
// *-----------------------------------------------------------------------------
// *typedef struct
// *{ TIM_Prescaler            都有
// *	TIM_CounterMode			     TIMx,x[6,7]没有，其他都有
// *  TIM_Period               都有
// *  TIM_ClockDivision        TIMx,x[6,7]没有，其他都有
// *  TIM_RepetitionCounter    TIMx,x[1,8,15,16,17]才有
// *}TIM_TimeBaseInitTypeDef; 
// *-----------------------------------------------------------------------------
// */
/*
 * @function: SWSysTiemrModeConfig
 * @details : 配置软件系统定时器，1ms定时。
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
static void SWSysTimerModeConfig(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;		
	// 开启定时器时钟,即内部时钟CK_INT=72M
	RCC_APB1PeriphClockCmd(SW_SYS_TIMER_CLK, ENABLE);	
	// 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
	TIM_TimeBaseStructure.TIM_Period = SW_SYS_TIMER_Period;
	// 时钟预分频数
	TIM_TimeBaseStructure.TIM_Prescaler = SW_SYS_TIMER_Prescaler;	
	// 时钟分频因子 ，没用到不用管
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		
	// 计数器计数模式，设置为向上计数
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 		
	// 重复计数器的值，没用到不用管
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	// 初始化定时器
	TIM_TimeBaseInit(SW_SYS_TIMER, &TIM_TimeBaseStructure);
	
	// 清除计数器中断标志位
	TIM_ClearFlag(SW_SYS_TIMER, TIM_FLAG_Update);
	  
	// 开启计数器中断
	TIM_ITConfig(SW_SYS_TIMER,TIM_IT_Update,ENABLE);
		
	// 使能计数器
	TIM_Cmd(SW_SYS_TIMER, ENABLE);
}


/*
 * @function: SWSysTimerInit
 * @details : 初始化软件系统定时器
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void SWSysTimerInit(void)
{
	SWSysTimerNVIC_Config();
	SWSysTimerModeConfig();	

	SWSysTimeTick.T1ms = 0;
	SWSysTimeTick.T5ms = 2;
	SWSysTimeTick.T10ms = 0;
	SWSysTimeTick.T20ms = 0;
	SWSysTimeTick.T100ms = 0;
	SWSysTimeTick.T1s = 0;	
}

/*
 * @function: SWSysTimerAdd
 * @details : 软件系统定时累加
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void SWSysTimerAdd(void)
{
	SWSysTimeTick.T1ms++;
	SWSysTimeTick.T5ms++;
	SWSysTimeTick.T10ms++;
	SWSysTimeTick.T20ms++;
	SWSysTimeTick.T1s++;
}

/*********************************************END OF FILE**********************/

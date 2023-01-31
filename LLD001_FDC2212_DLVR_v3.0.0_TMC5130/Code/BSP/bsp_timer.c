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
	
	//定时器2
	NVIC_InitStructure.NVIC_IRQChannel = SW_SYS_TIMER_IRQ;
	// 设置主优先级为 0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	// 设置抢占优先级为3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


/*
 * @function: MotorTimerNVIC_Config
 * @details : 中断优先级配置
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
static void MotorTimerNVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	// 设置中断组为0
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	
	//Enable the TIM1 Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
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
 * @function: TIM3_GPIO_Config
 * @details : 配置时器3
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void TIM3_GPIO_Config(void) 
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	//配置重映射模式为部分重映射
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,ENABLE);
	
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}


/*
 * @function: TIM3_Configuration
 * @details : 配置时器3
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void TIM3_Configuration(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	
	// 开启定时器时钟,即内部时钟CK_INT=72M
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	
	
	
	// 自动重装载寄存器的值，累计(n+1)个脉冲后产生一个更新或者中断
	TIM_TimeBaseStructure.TIM_Period = 49;
	// 时钟预分频数(n+1)
	TIM_TimeBaseStructure.TIM_Prescaler = 71;	
	// 时钟分频因子 ，没用到不用管
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		
	// 计数器计数模式，设置为向上计数
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 		
	// 重复计数器的值，没用到不用管
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;	
	// 初始化定时器
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	
	//PWM1 Mode configuration: Channel2
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
//	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
//	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
//	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;
	TIM_OCInitStructure.TIM_Pulse = 24;
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);	
	
	//自动加载的预装载寄存器使能
//	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Disable);
	
	// 清除计数器中断标志位
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	
	// 开启计数器中断
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
//	TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
	
	// 使能计数器
//	TIM_Cmd(TIM3, ENABLE);
	TIM_Cmd(TIM3, DISABLE);
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
 * @function: MotorTimerInit
 * @details : 电机定时器
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void MotorTimerInit(void)
{
	MotorTimerNVIC_Config();
	TIM3_GPIO_Config();	
	TIM3_Configuration();
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


/*
 * @function: MotorTimerPWMOutput
 * @details : MotorTimerPWMOutput
 * @input   : 1.fre：频率。
              2.dir：转向。
 * @output  : NULL
 * @return  : NULL
 */
uint8_t MotorTimerPWMOutput(uint16_t fre, uint8_t dir)
{
	uint8_t    ret = 0;
	uint8_t    pwm_en = DISABLE;
	
	uint32_t   tpsc = 0;
	uint32_t   tclk = 0;    //频率越高，输出PWM精度越好，但会频繁进入中断，负担重。
	uint32_t   tarr = 0;
	uint32_t   tccr = 0;
	
	
	if(0 == fre)
	{
		pwm_en = DISABLE;
	}
	else if(fre <= 50)
	{
		//72MHz系统时钟，分频系数(n+1)=1200，定时器时钟频率60KHz。
		tpsc = 1199;

		//重装载值，四舍五入
		//1Hz频率，arr = 60000；50Hz频率，arr = 1200。
	}
	else if(fre <= 500)
	{
		//时钟分频系数(n+1)=1200，定时器时钟频率3000KHz。
		tpsc = 23;
		
		//重装载值，四舍五入
		//51Hz频率，arr = 58824；500Hz频率，arr = 6000。
	}
	else if(fre <= 4000)
	{
		//时钟分频系数(n+1)=2，定时器时钟频率32MHz。
		tpsc = 1;
		
		//重装载值，四舍五入
		//501Hz频率，arr = 63872；4000Hz频率，arr = 8000；
	}
	else
	{
		//时钟分频系数(n+1)=1，定时器时钟频72MHz。
		tpsc = 0;
		
		//重装载值，四舍五入
		//4001Hz频率，arr = 17995；10kz频率，arr = 7200；
	}
	
	if((fre >= 1) && (fre <= 10000))
	{
		//定时器时钟分频
		tclk = SystemCoreClock / (tpsc + 1);
		
		//重装载值，四舍五入
		tarr = tclk * 10;
		tarr = (tarr / fre) + 5;
		tarr = tarr / 10;
		
		//占空比
		tccr = tarr / 2;
		
		//输出pwm使能
		pwm_en = ENABLE;
	}
	
	
	TIM3->PSC = tpsc;
	TIM3->ARR = tarr;
	TIM3->CCR2 = tccr;
  	TIM_CtrlPWMOutputs(TIM3, pwm_en);
	
	
	return (ret);
}

/*
 * @function: MotorTimerPWMStop
 * @details : 停止电机PWM
 * @output  : NULL
 * @return  : NULL
 */
uint8_t MotorTimerPWMStop(void)
{
//	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM3, DISABLE);
	
	TIM3->CNT = 0;
}

/*********************************************END OF FILE**********************/

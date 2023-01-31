/*****************************************************************************
Copyright  : BGI
File name  : bsp_timer.h
Description: 定时器
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H


#include "stm32f10x.h"


//宏定义----------------------------------------------------------------------//
//将通用定时器2作为软件系统SoftWareSystem定时
#define  SW_SYS_TIMER                  TIM2
#define  SW_SYS_TIMER_APBxClock_FUN    RCC_APB1PeriphClockCmd
#define  SW_SYS_TIMER_CLK              RCC_APB1Periph_TIM2
#define  SW_SYS_TIMER_Period           999
#define  SW_SYS_TIMER_Prescaler        71
#define  SW_SYS_TIMER_IRQ              TIM2_IRQn
#define  SW_SYS_TIMER_IRQHandler       TIM2_IRQHandler

//#define  MOTOR_TIMER                   TIM3
//#define  MOTOR_TIMER_APBxClock_FUN     RCC_APB1PeriphClockCmd
//#define  MOTOR_TIMER_CLK               RCC_APB1Periph_TIM3
//#define  MOTOR_TIMER_Period            999
//#define  MOTOR_TIMER_Prescaler         71
//#define  MOTOR_TIMER_IRQ               TIM3_IRQn
//#define  MOTOR_TIMER_IRQHandler        TIM3_IRQHandler





//定义结构体--------------------------------------------------------------------//
//软件系统定时器
struct tagSWSysTimeTick
{
	uint8_t   T1ms;
	uint8_t   T5ms;
	uint8_t   T10ms;
	uint8_t   T20ms;
//	uint8_t   T50ms;
	uint8_t   T100ms;
	uint8_t   T200ms;
	
	uint16_t  T500ms;
	uint16_t  T1s;
	
	uint16_t PowerOnS;//上电时间 x秒  
};


//声明变量----------------------------------------------------------------------//
extern struct tagSWSysTimeTick    SWSysTimeTick;


//声明函数----------------------------------------------------------------------//
void SWSysTimerInit(void);
void MotorTimerInit(void);
void SWSysTimerAdd(void);
uint8_t MotorTimerPWMOutput(uint16_t fre, uint8_t dir);
uint8_t MotorTimerPWMStop(void);


#endif



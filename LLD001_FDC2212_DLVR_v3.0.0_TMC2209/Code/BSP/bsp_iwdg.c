/*****************************************************************************
Copyright  : BGI
File name  : bsp_iwdg.c
Description: 看门狗
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/ 
#include "bsp_iwdg.h"   
#include "include.h"





//定义变量---------------------------------------------------------------------//




//定义函数---------------------------------------------------------------------//
/*
 * @function: IWDG_Config
 * @details : 设置 IWDG 的超时时间，Tout = prv/40 * rlv (s)，prv可以是[4,8,16,32,64,128,256]。
              举例：IWDG_Config(IWDG_Prescaler_64 ,625);  // IWDG 1s 超时溢出
 * @input   : prv:预分频器值，取值如下：
 *     @arg IWDG_Prescaler_4: IWDG prescaler set to 4
 *     @arg IWDG_Prescaler_8: IWDG prescaler set to 8
 *     @arg IWDG_Prescaler_16: IWDG prescaler set to 16
 *     @arg IWDG_Prescaler_32: IWDG prescaler set to 32
 *     @arg IWDG_Prescaler_64: IWDG prescaler set to 64
 *     @arg IWDG_Prescaler_128: IWDG prescaler set to 128
 *     @arg IWDG_Prescaler_256: IWDG prescaler set to 256
 
              rlv:预分频器值，取值范围为：0-0XFFF
 * @output  : NULL
 * @return  : NULL
 */
void IWDG_Config(uint8_t prv ,uint16_t rlv)
{	
	// 使能 预分频寄存器PR和重装载寄存器RLR可写
	IWDG_WriteAccessCmd( IWDG_WriteAccess_Enable );
	
	// 设置预分频器值
	IWDG_SetPrescaler( prv );
	
	// 设置重装载寄存器值
	IWDG_SetReload( rlv );
	
	// 把重装载寄存器的值放到计数器中
	IWDG_ReloadCounter();
	
	// 使能 IWDG
	IWDG_Enable();	
}

/*
 * @function: iwdg_Feed
 * @details : 喂狗
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void iwdg_Feed(void)
{
	// 把重装载寄存器的值放到计数器中，喂狗，防止IWDG复位
	// 当计数器的值减到0的时候会产生系统复位
	IWDG_ReloadCounter();
}


/*********************************************END OF FILE**********************/

/*****************************************************************************
Copyright  : BGI
File name  : bsp_sysclk.h
Description: 系统时钟配置
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#ifndef __BSP_SYSCLK_H
#define __BSP_SYSCLK_H


#include "stm32f10x.h"




//宏定义----------------------------------------------------------------------//



//定义结构体--------------------------------------------------------------------//


//声明变量----------------------------------------------------------------------//



//声明函数----------------------------------------------------------------------//
void HSI_SetSysClock(uint32_t pllmul);



#endif


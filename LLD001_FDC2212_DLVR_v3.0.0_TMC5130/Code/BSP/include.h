/*****************************************************************************
Copyright  : BGI
File name  : include.h
Description: 集中包含各个头文件
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#ifndef __INCLUDE_H
#define __INCLUDE_H


//driver
#include "stm32f10x.h"
#include "misc.h"
#include "stm32f10x_iwdg.h"
#include "stm32f10x_tim.h" 
#include "stm32f10x_rcc.h"
#include <string.h>
#include <stdio.h>


//bsp
#include "bsp_sysclk.h" 
#include "bsp_misc.h"
#include "bsp_lib.h" 
#include "bsp_iwdg.h" 
#include "bsp_timer.h" 
#include "bsp_adc.h" 
#include "bsp_can.h" 
#include "bsp_iic.h" 
#include "bsp_i2c_gpio.h"
#include "bsp_spi.h"
#include "bsp_usart.h"
#include "bsp_gpio.h" 
#include "bsp_flash.h" 

#include "m_usart.h"
#include "m_modbus.h" 
#include "m_storage.h" 


//rt-thread
#include "rtthread.h"


//User
#include "main.h" 
#include "board.h" 


//App
#include "app.h"
#include "app_misc.h" 
#include "monitor_can.h"
#include "monitor_usart.h" 
#include "liquid_level.h"
#include "storage.h" 
#include "gpio.h"



#endif

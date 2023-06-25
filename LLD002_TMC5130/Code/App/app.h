/*****************************************************************************
Copyright  : BGI
File name  : app.h
Description: 定义函数
Author     : lmj
Version    : 0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#ifndef __APP_H
#define __APP_H


#include "stm32f10x.h"
#include "rtthread.h"



//宏定义----------------------------------------------------------------------//  



//定义结构体--------------------------------------------------------------------//


//声明变量----------------------------------------------------------------------//



//声明函数----------------------------------------------------------------------//
void IWDG_Feed(void* parameter);
void SWSysTimer(void* parameter);
void SWTimer10ms(void* parameter);
void CommMonitor(void* parameter);
void MotorDrive(void* parameter);


#endif



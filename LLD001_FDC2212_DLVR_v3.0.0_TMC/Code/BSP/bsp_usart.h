/*****************************************************************************
Copyright  : BGI
File name  : bsp_usart.h
Description: 串口
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#ifndef __BSP_USART_H
#define	__BSP_USART_H


#include "stm32f10x.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>




//宏定义----------------------------------------------------------------------//
//缓冲区长度
#define BUFFER_SIZE 1024
#define INTR_PRI	5
#define UART_TIMEOUT_VALUE	10


//定义结构体--------------------------------------------------------------------//



//声明变量----------------------------------------------------------------------//



//声明函数----------------------------------------------------------------------//

void UART1_Init(void)
{
	USART_InitTypeDef UART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	USART_DeInit(USART2);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

}
























#endif




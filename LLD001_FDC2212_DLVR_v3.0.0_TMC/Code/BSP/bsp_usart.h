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
#define  UART_SEND_MAXNUM              100	//发送缓冲区长度
#define  UART_RECV_MAXNUM              100	//接收缓冲区长度

//调试错误信息
#define EEPROM_ERROR(fmt,arg...)       printf("<<-EEPROM-ERROR->> "fmt"\n",##arg)



//定义结构体--------------------------------------------------------------------//



//声明变量----------------------------------------------------------------------//



//声明函数----------------------------------------------------------------------//
void USART1_Config(void);
void USART2_Config(void);
void Usart_SendByte(USART_TypeDef *pUSARTx, uint8_t ch);
void Usart_SendArray(USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num);
void Usart_SendString(USART_TypeDef *pUSARTx, char *str);
void Usart_SendHalfWord(USART_TypeDef *pUSARTx, uint16_t ch);
void USART1_printf(char *fmt, ...);
void USART2_printf(char *fmt, ...);

#endif

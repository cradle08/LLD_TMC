/*****************************************************************************
Copyright  : BGI
File name  : bsp_usart.c
Description: 串口
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#include "bsp_usart.h"


#define __IO volatile

//定义变量---------------------------------------------------------------------//
static uint8_t UARTSendFlag;

static __IO uint8_t rxBuffer[BUFFER_SIZE] = {0};
static __IO uint8_t txBuffer[BUFFER_SIZE] = {0};
static __IO uint32_t available = 0;


/*
*
*  buffer struct
*/
typedef struct 
{
	uint32_t read;
	uint32_t write;
	__IO uint8_t buffer;
} Buffer_t;


/*
*
* 串口收发buffer
*/
typedef struct 
{
	Buffer_t tx;
	Buffer_t rx;
} RxTxBuffer_t;



void UART1_Init(void);




//定义函数---------------------------------------------------------------------//

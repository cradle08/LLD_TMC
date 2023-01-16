#ifndef __TMC_UART_H__
#define __TMC_UART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "public.h"



/*
*	buffer struct
*
*/
typedef struct
{
	uint16_t usRead;
	uint16_t usWrite;
	__IO uint8_t *pucaBuffer;
	
}Buffer_t;




/*
*	Rx Tx Buffer
*
*/
typedef struct
{
	Buffer_t tRx;
	Buffer_t tTx;
	
}TxRxBuffer_t;




//清除缓存
void Clear_TxRxBuffer(void);
static uint32_t bytesAvailable();
static void tx(uint8_t ch);
static void txN(uint8_t *str, unsigned char number);
static uint8_t rx(uint8_t *ch);
static uint8_t rxN(uint8_t *str, unsigned char number);
static uint32_t bytesAvailable(void);



//tmc通信串口初始化
void Uart_Init(void);

//UART1 中断函数
void USART1_IRQHandler(void);

//UART读写
int32_t UART_ReadWrite(uint8_t *pucaData, uint16_t usWriteLen, uint8_t ucReadLen);
void UART_ReadInt(uint8_t ucSlave, uint8_t ucRegisterAddress, int32_t *plValue);
void UART_WriteInt(uint8_t ucSlave, uint8_t ucRegisterAddress, int32_t lValue);


void Wait(uint32_t delay);
uint32_t TimeSince(uint32_t tick);
uint32_t TimeDiff(uint32_t newTick, uint32_t oldTick);










  
#ifdef __cplusplus
}
#endif

#endif //__TMC_UART_H__







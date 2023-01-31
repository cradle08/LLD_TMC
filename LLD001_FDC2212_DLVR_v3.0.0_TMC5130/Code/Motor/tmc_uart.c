#include "main.h"
#include "tmc_uart.h"
#include "CRC.h"
#include "TMC2209.h"

//

//
#define BUFFER_SIZE  16
#define INTR_PRI     5
#define UART_TIMEOUT_VALUE 10


static uint8_t UARTSendFlag;
static __IO uint8_t rxBuffer[BUFFER_SIZE] = {0};
static __IO uint8_t txBuffer[BUFFER_SIZE] = {0};
static __IO uint32_t available = 0;



/*
 * @function 
 * @brief	 UART 发送接受，buffer
 * @param
 * @retval
 */
static TxRxBuffer_t tTxRxBuffer = 
{
	.tRx = 
	{
		.usRead = 0,
		.usWrite = 0,
		.pucaBuffer = rxBuffer
	},
	
	.tTx = 
	{
		.usRead = 0,
		.usWrite = 0,
		.pucaBuffer = txBuffer
	}
};





/*
 * @function 
 * @brief	 清除缓存
 * @param
 * @retval
 */
void Clear_TxRxBuffer(void)
{
	__disable_irq();
	available         		 = 0;
	tTxRxBuffer.tRx.usRead   = 0;
	tTxRxBuffer.tRx.usWrite  = 0;

	tTxRxBuffer.tTx.usRead   = 0;
	tTxRxBuffer.tTx.usWrite  = 0;
	__enable_irq();
}


/*
 * @function 
 * @brief	 
 * @param
 * @retval
 */
static uint32_t bytesAvailable()
{
	return available;
}



/*
 * @function 
 * @brief	 
 * @param
 * @retval
 */
static void tx(uint8_t ch)
{
	tTxRxBuffer.tTx.pucaBuffer[tTxRxBuffer.tTx.usWrite] = ch;
	tTxRxBuffer.tTx.usWrite = (tTxRxBuffer.tTx.usWrite + 1)%BUFFER_SIZE;
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}


static void txN(uint8_t *str, unsigned char number)
{
	for(int32_t i = 0; i < number; i++)
		tx(str[i]);
}


/*
 * @function 
 * @brief	 
 * @param
 * @retval
 */
static uint8_t rx(uint8_t *ch)
{
	if(tTxRxBuffer.tRx.usRead == tTxRxBuffer.tRx.usWrite) return 0;
	*ch = tTxRxBuffer.tRx.pucaBuffer[tTxRxBuffer.tRx.usRead];
	tTxRxBuffer.tRx.usRead = (tTxRxBuffer.tRx.usRead + 1)%BUFFER_SIZE;
	available--;
	
	return 1;
}



/*
 * @function 
 * @brief	 
 * @param
 * @retval
 */
static uint8_t rxN(uint8_t *str, unsigned char number)
{
	if(bytesAvailable() < number)
		return 0;

	for(int32_t i = 0; i < number; i++)
		rx(&str[i]);

	return 1;
}



/*
 * @function 
 * @brief	 UART1 Init
 * @param
 * @retval
 */
void Uart_Init()
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	USART_DeInit(USART1);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//PA9 Tx
	GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//PA10 Rx
	GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* 配置串口的工作参数 */
	// 配置波特率
	USART_InitStructure.USART_BaudRate = 115200;
	// 配置 针数据字长
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	// 配置停止位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	// 配置校验位
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	// 配置硬件流控制
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	// 配置工作模式，收发一起
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// 完成串口的初始化配置
	USART_Init(USART1, &USART_InitStructure);
	
	
	/* 配置USART1中断源 */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	//抢断优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	//子优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
	//使能中断
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//初始化配置NVIC
	NVIC_Init(&NVIC_InitStructure);


	USART_ClearFlag(USART1, USART_FLAG_CTS | USART_FLAG_LBD  | USART_FLAG_TXE  |
	                        USART_FLAG_TC  | USART_FLAG_RXNE | USART_FLAG_IDLE |
	                        USART_FLAG_ORE | USART_FLAG_NE   | USART_FLAG_FE   |
	                        USART_FLAG_PE);
//	USART_ITConfig(USART1,USART_IT_PE, DISABLE);
//	USART_ITConfig(USART1,USART_IT_TXE, ENABLE);
//	USART_ITConfig(USART1,USART_IT_TC, ENABLE);
	USART_ITConfig(USART1,USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART1,USART_IT_IDLE, DISABLE);
	USART_ITConfig(USART1,USART_IT_LBD, DISABLE);
	USART_ITConfig(USART1,USART_IT_CTS, DISABLE);
	USART_ITConfig(USART1,USART_IT_ERR, DISABLE);

	USART_Cmd(USART1, ENABLE);
}




/*
 * @function 
 * @brief	 UART1 Init
 * @param
 * @retval
 */
void USART1_IRQHandler(void)
{
	uint8_t byte = 0;
	
	//接受中断
	if(USART1->SR & USART_FLAG_RXNE)
	{
		byte = USART1->DR;
		if(!UARTSendFlag)
		{
			tTxRxBuffer.tRx.pucaBuffer[tTxRxBuffer.tRx.usWrite] = byte;
			tTxRxBuffer.tRx.usWrite = (tTxRxBuffer.tRx.usWrite + 1)%BUFFER_SIZE;
			available++;
		}
	}
	
	//发送Data空,中断
	if(USART1->SR & USART_FLAG_TXE)
	{
		if(tTxRxBuffer.tTx.usRead != tTxRxBuffer.tTx.usWrite)
		{
			UARTSendFlag = 1;
			USART1->DR = tTxRxBuffer.tTx.pucaBuffer[tTxRxBuffer.tTx.usRead];
			tTxRxBuffer.tTx.usRead = (tTxRxBuffer.tTx.usRead + 1)%BUFFER_SIZE;
		}else{
			USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
		}
	}
	
	
	//发送完成，中断
	if(USART1->SR & USART_FLAG_TC)
	{
		if(tTxRxBuffer.tTx.usRead == tTxRxBuffer.tTx.usWrite)
		{
			byte = USART1->DR;
			UARTSendFlag = 0;
		}
		USART_ClearITPendingBit(USART1, USART_IT_TC);	
	}
}





/*
 * @function 
 * @brief	 UART读写
 * @param
 * @retval
 */
int32_t UART_ReadWrite(uint8_t *pucaData, uint16_t usWriteLen, uint8_t ucReadLen)
{
	Clear_TxRxBuffer();
	txN(pucaData, usWriteLen);
	
	Wait(2);
	
	//读取数据
	if(ucReadLen <= 0) return 0;
	
	//超时检测
	uint32_t ulTimeStamp = rt_tick_get();
	while(bytesAvailable() < ucReadLen)
	{
		if(TimeSince(ulTimeStamp) > UART_TIMEOUT_VALUE) return -1;
	}
	
	rxN(pucaData, ucReadLen);
	return 0;
}




/*
 * @function 
 * @brief	 UART读
 * @param
 * @retval
 */
void UART_ReadInt(uint8_t ucSlave, uint8_t ucRegisterAddress, int32_t *plValue)
{
	uint8_t ucaReadData[8] = {0}, ucaDataRequest[4] = {0};
	uint32_t ulTimeOut = 0;
	
	ucaDataRequest[0] = 0x05;
	ucaDataRequest[1] = ucSlave;
	ucaDataRequest[2] = ucRegisterAddress;
	ucaDataRequest[3] = tmc_CRC8(ucaDataRequest, 3, 1);
	
	//发送
	Clear_TxRxBuffer();
	txN(ucaDataRequest, sizeof(ucaDataRequest));
	
	//检测超时
	ulTimeOut = rt_tick_get();
	while(bytesAvailable() < sizeof(ucaReadData))
	{
		if(TimeSince(ulTimeOut) > UART_TIMEOUT_VALUE) return;
	}
	
	//接受数据
	rxN(ucaReadData, sizeof(ucaReadData));
	
	//
	if(ucaReadData[7] != tmc_CRC8(ucaReadData, 7, 1) || ucaReadData[0] != 0x05 || ucaReadData[1] != 0xFF || ucaReadData[2] != ucRegisterAddress)
		return;

	*plValue = ucaReadData[3] << 24 | ucaReadData[4] << 16 | ucaReadData[5] << 8 | ucaReadData[6];
	return;
}



/*
 * @function 
 * @brief	 UART写
 * @param
 * @retval
 */
void UART_WriteInt(uint8_t ucSlave, uint8_t ucRegisterAddress, int32_t lValue)
{
	uint8_t writeData[8];

	writeData[0] = 0x05;                         // Sync byte
	writeData[1] = ucSlave;                        // Slave address
	writeData[2] = ucRegisterAddress | TMC_WRITE_BIT;      // Register address with write bit set
	writeData[3] = lValue >> 24;                  // Register Data
	writeData[4] = lValue >> 16;                  // Register Data
	writeData[5] = lValue >> 8;                   // Register Data
	writeData[6] = lValue & 0xFF;                 // Register Data
	writeData[7] = tmc_CRC8(writeData, 7, 1);    // Cyclic redundancy check

	Clear_TxRxBuffer();
	for(uint32_t i = 0; i < ARRAY_SIZE(writeData); i++)
		tx(writeData[i]);

	/* Workaround: Give the UART time to send. Otherwise another write/readRegister can do clearBuffers()
	 * before we're done. This currently is an issue with the IDE when using the Register browser and the
	 * periodic refresh of values gets requested right after the write request.
	 */
	Wait(2);
}



/*
 * @function 
 * @brief	 UART读写
 * @param
 * @retval
 */
void Wait(uint32_t delay)	// wait for [delay] ms/systicks
{
	uint32_t startTick = rt_tick_get();
	while((rt_tick_get()-startTick) <= delay) {};
}



/*
 * @function 
 * @brief	 UART读写
 * @param
 * @retval
 */
uint32_t TimeSince(uint32_t tick)	// time difference since the [tick] timestamp in ms/systicks
{
	return TimeDiff(rt_tick_get(), tick);
}



/*
 * @function 
 * @brief	 UART读写
 * @param
 * @retval
 */
uint32_t TimeDiff(uint32_t newTick, uint32_t oldTick) // Time difference between newTick and oldTick timestamps
{
	uint32_t tickDiff = newTick - oldTick;

	// Prevent subtraction underflow - saturate to 0 instead
	if(tickDiff != 0)
		return tickDiff - 1;
	else
		return 0;
}










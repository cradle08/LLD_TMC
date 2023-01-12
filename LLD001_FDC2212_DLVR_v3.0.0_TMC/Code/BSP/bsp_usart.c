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




//定义变量---------------------------------------------------------------------//



//定义函数---------------------------------------------------------------------//
/*
 * @function: USART1_NVIC_Configuration
 * @details : USART1串口中断优先级配置
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
static void USART1_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	
	//配置USART1中断源
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	//抢断优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	//子优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
	//使能中断
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//初始化配置NVIC
	NVIC_Init(&NVIC_InitStructure);
}

/*
 * @function: USART1_Config
 * @details : 配置串口
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void USART1_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	
	// 打开串口外设的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	
	// 打开串口GPIO的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	
	// 将USART Tx的GPIO配置为推挽复用模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// 将USART Rx的GPIO配置为浮空输入模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// 配置串口的工作参数
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
	
	// 串口中断优先级配置
	USART1_NVIC_Configuration();
	
	// 使能串口接收中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	
	
	// 使能串口
	USART_Cmd(USART1, ENABLE);	    
}


/*
 * @function: USART2_NVIC_Configuration
 * @details : USART2串口中断优先级配置
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
static void USART2_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	
	//嵌套向量中断控制器组选择
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	//配置USART为中断源
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	//抢断优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	//子优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
	//使能中断
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//初始化配置NVIC
	NVIC_Init(&NVIC_InitStructure);
}


/*
 * @function: USART1_Config
 * @details : 配置串口
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void USART2_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	
	// 打开串口外设的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	
	
	// 打开串口GPIO的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	// 使能引脚重映射
//	GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
	
	
	// 将USART Tx的GPIO配置为推挽复用模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// 将USART Rx的GPIO配置为浮空输入模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// 配置串口的工作参数
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
	USART_Init(USART2, &USART_InitStructure);
	
	// 串口中断优先级配置
	USART2_NVIC_Configuration();
	
	// 使能串口接收中断
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	
	
	// 使能串口
	USART_Cmd(USART2, ENABLE);	    
}


/*
 * @function: Usart_SendByte
 * @details : 发送一个字节
 * @input   : 1.pUSARTx：串口指针。
              2.ch：待发送的数据。
 * @output  : NULL
 * @return  : NULL
 */
void Usart_SendByte(USART_TypeDef * pUSARTx, uint8_t ch)
{
	//发送一个字节数据到USART
	USART_SendData(pUSARTx, ch);
		
	//等待发送数据寄存器为空
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/*
 * @function: Usart_SendArray
 * @details : 发送数组数据
 * @input   : 1.pUSARTx：串口指针。
              2.array：装载数据的数组。
              3.num：发送字节数量。
 * @output  : NULL
 * @return  : NULL
 */
void Usart_SendArray(USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num)
{
	uint8_t i;

	for(i=0; i<num; i++)
	{
		//发送一个字节数据到USART
		Usart_SendByte(pUSARTx,array[i]);	

	}
	//等待发送完成
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET);
}

/*
 * @function: Usart_SendString
 * @details : 发送字符串
 * @input   : 1.pUSARTx：串口指针。
              2.str：字符串。
 * @output  : NULL
 * @return  : NULL
 */
void Usart_SendString(USART_TypeDef *pUSARTx, char *str)
{
	unsigned int k = 0;
	
	do 
	{
		Usart_SendByte(pUSARTx, *(str + k) );
		k++;
	} while(*(str + k)!='\0');

	//等待发送完成
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET)
	{}
}

/*
 * @function: Usart_SendHalfWord
 * @details : 发送16位变量
 * @input   : 1.pUSARTx：串口指针。
              2.ch：16位变量数据。
 * @output  : NULL
 * @return  : NULL
 */
void Usart_SendHalfWord( USART_TypeDef *pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;

	//取出高八位
	temp_h = (ch&0XFF00)>>8;
	//取出低八位
	temp_l = ch&0XFF;

	//发送高八位
	USART_SendData(pUSARTx, temp_h);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);

	//发送低八位
	USART_SendData(pUSARTx, temp_l);	
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/*
 * @function: fputc
 * @details : 重定向c库函数printf到串口，重定向后可使用printf函数
 * @input   : 1.ch：数据。
              2.f：流。
 * @output  : NULL
 * @return  : 发送的数据
 */
//int fputc(int ch, FILE *f)
//{
//	//发送一个字节数据到串口
//	USART_SendData(DEBUG_USARTx, (uint8_t) ch);

//	//等待发送完毕
//	while(USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);		

//	return (ch);
//}

/*
 * @function: fgetc
 * @details : 重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
 * @input   : 1.f：流。
 * @output  : NULL
 * @return  : 发送的数据
 */
//int fgetc(FILE *f)
//{
//	/* 等待串口输入数据 */
//	while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET);

//	return (int)USART_ReceiveData(DEBUG_USARTx);
//}


/*
 * @function: USART1_printf
 * @details : 多个串口使用printf函数
 
1、申明头文件#include<stdarg.h>
2、buffer的大小根据需要调整，相应的循环条件也要改，此处可以定义成宏，方便调节buffer大小。
3、定义va_list变量，该变量是一个字符指针，可以理解为指向当前参数的一个指针，取参必须通过这个指针进行。
4、va_start让arg_ptr指向printf函数可变参数里边的第一个参数；
5、vsnprintf()将按照fmt的格式将arg_ptr里的值依次转换成字符保存到buffer中，该函数有最大字符数限制，超过后会被截断，且该函数会自动在字符串末尾加‘\0’。
6、最后必须调用va_end()，由此确保堆栈的正确恢复。
 
 * @input   : 1.f：流。
 * @output  : NULL
 * @return  : 发送的数据
 */
void USART1_printf(char *fmt, ...)
{ 
	char       buffer[UART_RECV_MAXNUM + 1];     // 数据长度
	uint8_t    i = 0;
	
	
	va_list arg_ptr;
	va_start(arg_ptr, fmt); 
	vsnprintf(buffer, UART_RECV_MAXNUM + 1, fmt, arg_ptr);
	while((i < UART_RECV_MAXNUM) && (i < strlen(buffer)))
	{
        USART_SendData(USART1, (u8) buffer[i++]);
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET); 
	}
	va_end(arg_ptr);
}

/*
 * @function: USART2_printf
 * @details : 多个串口使用printf函数
 
1、申明头文件#include<stdarg.h>
2、buffer的大小根据需要调整，相应的循环条件也要改，此处可以定义成宏，方便调节buffer大小。
3、定义va_list变量，该变量是一个字符指针，可以理解为指向当前参数的一个指针，取参必须通过这个指针进行。
4、va_start让arg_ptr指向printf函数可变参数里边的第一个参数；
5、vsnprintf()将按照fmt的格式将arg_ptr里的值依次转换成字符保存到buffer中，该函数有最大字符数限制，超过后会被截断，且该函数会自动在字符串末尾加‘\0’。
6、最后必须调用va_end()，由此确保堆栈的正确恢复。
 
 * @input   : 1.f：流。
 * @output  : NULL
 * @return  : 发送的数据
 */
void USART2_printf(char *fmt, ...)
{ 
	char       buffer[UART_RECV_MAXNUM + 1];     // 数据长度
	uint8_t    i = 0;
	
	
	va_list arg_ptr;
	va_start(arg_ptr, fmt); 
	vsnprintf(buffer, UART_RECV_MAXNUM + 1, fmt, arg_ptr);
	while((i < UART_RECV_MAXNUM) && (i < strlen(buffer)))
	{
        USART_SendData(USART2, (u8) buffer[i++]);
        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET); 
	}
	va_end(arg_ptr);
}



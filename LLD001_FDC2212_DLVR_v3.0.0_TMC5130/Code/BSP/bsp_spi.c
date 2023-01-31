/*****************************************************************************
Copyright  : BGI
File name  : bsp_spi.c
Description: SPI通信，从野火移植而来。
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#include "bsp_spi.h"




//定义变量---------------------------------------------------------------------//




//定义函数---------------------------------------------------------------------//
/*
 * @function: SPI1_Configure
 * @details : 配置SPI1
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void SPI1_Configure(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	
	//使能SPI时钟
	RCC_APB2PeriphClockCmd(FLASH_SPI_CLK, ENABLE);

	//使能SPI引脚相关的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 
	
	//JLink引脚与SPI引脚重叠，需要禁止Jlink的引脚功能
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 
	GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE); 


	//配置SPI的CS引脚
	GPIO_InitStructure.GPIO_Pin = FLASH_SPI_CS_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//配置SPI的SCK、MISO、MOSI引脚
	GPIO_InitStructure.GPIO_Pin = FLASH_SPI_SCK_PIN | FLASH_SPI_MISO_PIN | FLASH_SPI_MOSI_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//停止信号 FLASH: CS引脚高电平
	FLASH_SPI_CS_HIGH();
	
	//SPI 模式配置
	//FLASH芯片 支持SPI模式0及模式3，据此设置CPOL CPHA
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(FLASH_SPIx, &SPI_InitStructure);
	
	//使能 SPI
	SPI_Cmd(FLASH_SPIx, ENABLE);
}









/*
 * @function: SPI_TIMEOUT_UserCallback
 * @details : 等待超时回调函数
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
static uint16_t SPI_TIMEOUT_UserCallback(uint16_t errorCode)
{
	//等待超时后的处理,输出错误信息
	FLASH_ERROR("SPI 等待超时!errorCode = %d", errorCode);
	
	return errorCode;
}


/*
 * @function: SPI_SendRecvByte
 * @details : 读写公共函数
 * @input   : 1.SPIx：SPI端口号。
              2.byte：要发送的数据。
 * @output  : NULL
 * @return  : 返回接收到的数据
 */
uint8_t SPI_SendRecvByte(SPI_TypeDef* spi, uint8_t byte)
{
	uint32_t   SPITimeout = SPIT_LONG_TIMEOUT;    
	
	
	//等待发送缓冲区为空，TXE事件
	while(SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_TXE) == RESET)
	{
		if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(1);
	}
	//写入数据寄存器，把要写入的数据写入发送缓冲区
	SPI_I2S_SendData(spi, byte);

	
	//等待接收缓冲区非空，RXNE事件
	SPITimeout = SPIT_FLAG_TIMEOUT;
	while(SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_RXNE) == RESET)
	{
		if((SPITimeout--) == 0) return SPI_TIMEOUT_UserCallback(1);
	}
	//读取数据寄存器，获取接收缓冲区数据
	return SPI_I2S_ReceiveData(spi);
}

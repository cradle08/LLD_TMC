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
#include "main.h"
#include "bsp_spi.h"
#include "TMC_Api.h"





//定义变量---------------------------------------------------------------------//




//定义函数---------------------------------------------------------------------//
/*
 * @function: SPI1_Configure
 * @details : 配置SPI1
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void SPI2_Configure(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//PORTB时钟使能 
	RCC_APB1PeriphClockCmd(	USE_SPI_CLK,  ENABLE );//SPI2时钟使能 	
 
	GPIO_InitStructure.GPIO_Pin = USE_SPI_SCK_PIN | USE_SPI_MISO_PIN | USE_SPI_MOSI_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //PB13/14/15复用推挽输出 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB

 	GPIO_SetBits(GPIOB, USE_SPI_SCK_PIN|USE_SPI_MISO_PIN|USE_SPI_MOSI_PIN);  //PB13/14/15上拉

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;		//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
	SPI_Init(USE_SPIx, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
 
	SPI_Cmd(USE_SPIx, ENABLE); //使能SPI外设
}




/***********************************************************
*
*	SPI TMC
*
************************************************************/
/*
 * TMC Chip，Spi CS High
 */
void TMC_Spi_CS_High(TMC_e eTMC)
{
	switch(eTMC)
	{
		case TMC_0:
		{
			TMC0_SPI_CS_HIGH;
		}
		break;
		case TMC_1:
		{
			TMC1_SPI_CS_HIGH;
		}
		break;
		case TMC_2:
		{
			TMC2_SPI_CS_HIGH;
		}
		break;
		case TMC_3:
		{
			
		}
		break;
		case TMC_4:
		{
			
		}
		break;
		case TMC_5:
		{
			
		}
		break;
		default:break;

	}
}


/*
 *	TMC Chip，Spi CS Low
 */
void TMC_Spi_CS_Low(TMC_e eTMC)
{
	switch(eTMC)
	{
		case TMC_0:
		{
			TMC0_SPI_CS_LOW;
		}
		break;
		case TMC_1:
		{
			TMC1_SPI_CS_LOW;
		}
		break;
		case TMC_2:
		{
			TMC2_SPI_CS_LOW;
		}
		break;
		case TMC_3:
		{
			
		}
		break;
		case TMC_4:
		{
			
		}
		break;
		case TMC_5:
		{
			
		}
		break;
		default:break;

	}
}



/*
 * SPI Read Write Data API
 */
uint8_t SPI_ReadWriteData(SPI_TypeDef* ptSpi, uint8_t ucData)
{
	uint32_t   SPITimeout = SPIT_LONG_TIMEOUT;    
		
	//等待发送缓冲区为空，TXE事件
	while(SPI_I2S_GetFlagStatus(ptSpi, SPI_I2S_FLAG_TXE) == RESET)
	{
		if((SPITimeout--) == 0) return 0;
	}
	//写入数据寄存器，把要写入的数据写入发送缓冲区
	SPI_I2S_SendData(ptSpi, ucData);

	
	//等待接收缓冲区非空，RXNE事件
	SPITimeout = SPIT_FLAG_TIMEOUT;
	while(SPI_I2S_GetFlagStatus(ptSpi, SPI_I2S_FLAG_RXNE) == RESET)
	{
		if((SPITimeout--) == 0) return 0;
	}
	//读取数据寄存器，获取接收缓冲区数据
	return SPI_I2S_ReceiveData(ptSpi);
}



/*
 * TMC SPI Read Write Data API
 */
uint8_t TMC_SPI_ReadWriteData(uint8_t ucData)
{	
	return SPI_ReadWriteData(USE_SPIx, ucData);
}




/*
 * SPI Read Int
 */
int32_t TMC_SPI_ReadInt(TMC_e eTMC, uint8_t ucAddr)
{
	int32_t lValue = 0;
	uint8_t ucTempAddr = 0;

	TMC_Spi_CS_Low(eTMC);

	ucTempAddr = TMC_ADDRESS(ucAddr);
	lValue = TMC_SPI_ReadWriteData(ucAddr);
	lValue <<= 8;
	lValue |= TMC_SPI_ReadWriteData(TMC_SPI_INVALID_VALUE);
	lValue <<= 8;
	lValue |= TMC_SPI_ReadWriteData(TMC_SPI_INVALID_VALUE);
	lValue <<= 8;
	lValue |= TMC_SPI_ReadWriteData(TMC_SPI_INVALID_VALUE);

	TMC_Spi_CS_High(eTMC);
	return lValue;
}

/*
 * SPI  Write Int
 */
void TMC_SPI_WriteInt(TMC_e eTMC, uint8_t ucAddr, uint32_t ulValue)
{
	TMC_Spi_CS_Low(eTMC);

	TMC_SPI_ReadWriteData(ucAddr | 0x80);
	TMC_SPI_ReadWriteData(0xFF & (ulValue >> 24));
	TMC_SPI_ReadWriteData(0xFF & (ulValue >> 16));
	TMC_SPI_ReadWriteData(0xFF & (ulValue >> 8));
	TMC_SPI_ReadWriteData(0xFF & (ulValue >> 0));

	TMC_Spi_CS_High(eTMC);
}



/*
 * SPI Read Write Array
 */
void TMC_SPI_ReadWriteArr(TMC_e eTMC, uint8_t *pucaData, uint16_t ucLen)
{
	uint16_t i = 0;

	TMC_Spi_CS_Low(eTMC);
	for(i = 0; i < ucLen; i++)
	{
		pucaData[i] = TMC_SPI_ReadWriteData(pucaData[i]);
	}
	TMC_Spi_CS_High(eTMC);
}




/***********************************************************
*
*	SPI2 EEPROM
*
************************************************************/

/*
 * SPI Read Write Data API
 */
uint8_t EEPROM_SPI_ReadWriteData(uint8_t ucData)
{
	return SPI_ReadWriteData(USE_SPIx, ucData);

}




/*
 * SPI Read Array
 */
void EEPROM_SPI_ReadArr(uint8_t *pucaData, uint16_t ucLen)
{
	uint16_t i = 0;

	for(i = 0; i < ucLen; i++)
	{
		 pucaData[i] = EEPROM_SPI_ReadWriteData(0);
	}
}



/*
 * SPI Write Array
 */
void EEPROM_SPI_WriteArr(uint8_t *pucaData, uint16_t ucLen)
{
	uint16_t i = 0;

	for(i = 0; i < ucLen; i++)
	{
		 EEPROM_SPI_ReadWriteData(pucaData[i]);
	}
}








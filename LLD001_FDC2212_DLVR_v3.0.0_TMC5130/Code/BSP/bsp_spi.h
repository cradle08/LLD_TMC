/*****************************************************************************
Copyright  : BGI
File name  : bsp_spi.h
Description: SPI通信接口配置
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#ifndef __BSP_SPI_H
#define __BSP_SPI_H

#include "stm32f10x.h"
#include <stdio.h>
#include "TMC_Api.h"
#include "bsp_gpio.h"



//宏定义----------------------------------------------------------------------//
#define  USE_SPIx                    	SPI2
#define  USE_SPI_CLK                 	RCC_APB1Periph_SPI2


//SCK引脚 
#define  USE_SPI_SCK_PIN             GPIO_Pin_13

//MISO引脚
#define  USE_SPI_MISO_PIN            GPIO_Pin_14

//MOSI引脚
#define  USE_SPI_MOSI_PIN            GPIO_Pin_15






//等待超时时间
//0x1000，72MHz时钟，示波器实测1.952ms（增加P电机控制后，疑似IIC通信时序被打断）
#define  SPIT_FLAG_TIMEOUT             ((uint32_t)0x1000)
//0x1000 * 10=0xA000，72MHz时钟，示波器实测19.44ms
#define  SPIT_LONG_TIMEOUT             ((uint32_t)(4 * SPIT_FLAG_TIMEOUT))


////信息输出
//#define  FLASH_DEBUG_ON                1
//#define  FLASH_INFO(fmt,arg...)        printf("<<-FLASH-INFO->> "fmt"\n",##arg)
//#define  FLASH_ERROR(fmt,arg...)       printf("<<-FLASH-ERROR->> "fmt"\n",##arg)
//#define  FLASH_DEBUG(fmt,arg...)       do{\
//                                           if(FLASH_DEBUG_ON)\
//                                           printf("<<-FLASH-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
//                                       }while(0)



										  

//定义结构体--------------------------------------------------------------------//



//声明变量----------------------------------------------------------------------//



//声明函数----------------------------------------------------------------------//
void SPI2_Configure(void);
//uint8_t SPI_SendRecvByte(SPI_TypeDef* SPIx, uint8_t byte);


									   
									   
void TMC_Spi_CS_High(TMC_e eTMC);
void TMC_Spi_CS_Low(TMC_e eTMC);




// SPI读写数据接口 TMC
uint8_t SPI_ReadWriteData(SPI_TypeDef* ptSpi, uint8_t ucData);
uint8_t TMC_SPI_ReadWriteData(uint8_t ucData);
int32_t TMC_SPI_ReadInt(TMC_e eTMC, uint8_t ucAddr);
void TMC_SPI_WriteInt(TMC_e eTMC, uint8_t ucAddr, uint32_t ulValue);

void TMC_SPI_ReadWriteArr(TMC_e eTMC, uint8_t *pucaData, uint16_t ucLen);




//SPI3 EEPROM
uint8_t EEPROM_SPI_ReadWriteData(uint8_t ucData);
void EEPROM_SPI_ReadArr(uint8_t *pucaData, uint16_t ucLen);
void EEPROM_SPI_WriteArr(uint8_t *pucaData, uint16_t ucLen);


#endif


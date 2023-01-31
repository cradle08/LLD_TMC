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



//宏定义----------------------------------------------------------------------//
#define  FLASH_SPIx                    SPI1
#define  FLASH_SPI_CLK                 RCC_APB2Periph_SPI1


//SCK引脚 
#define  FLASH_SPI_SCK_PIN             GPIO_Pin_3

//MISO引脚
#define  FLASH_SPI_MISO_PIN            GPIO_Pin_4

//MOSI引脚
#define  FLASH_SPI_MOSI_PIN            GPIO_Pin_5

//CS(NSS)引脚 片选选普通GPIO即可
#define  FLASH_SPI_CS_PIN              GPIO_Pin_6
#define  FLASH_SPI_CS_PORT             GPIOB

//片选信号控制
#define  FLASH_SPI_CS_LOW()            GPIO_ResetBits(FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN)
#define  FLASH_SPI_CS_HIGH()           GPIO_SetBits(FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN)

//片选信号
#define  FLASH_CS_LOW                  0
#define  FLASH_CS_HIGH                 1




//等待超时时间
#define  SPIT_FLAG_TIMEOUT             ((uint32_t)0x1000)
#define  SPIT_LONG_TIMEOUT             ((uint32_t)(10 * SPIT_FLAG_TIMEOUT))

//信息输出
#define  FLASH_DEBUG_ON                1
#define  FLASH_INFO(fmt,arg...)        printf("<<-FLASH-INFO->> "fmt"\n",##arg)
#define  FLASH_ERROR(fmt,arg...)       printf("<<-FLASH-ERROR->> "fmt"\n",##arg)
#define  FLASH_DEBUG(fmt,arg...)       do{\
                                           if(FLASH_DEBUG_ON)\
                                           printf("<<-FLASH-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                       }while(0)



										  

//定义结构体--------------------------------------------------------------------//



//声明变量----------------------------------------------------------------------//



//声明函数----------------------------------------------------------------------//
void SPI1_Configure(void);
uint8_t SPI_SendRecvByte(SPI_TypeDef* SPIx, uint8_t byte);



#endif


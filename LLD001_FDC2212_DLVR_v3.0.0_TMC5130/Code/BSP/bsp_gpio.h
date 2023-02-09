/*****************************************************************************
Copyright  : BGI
File name  : bsp_gpio.h
Description: GPIO引脚
Author     : lmj
Version    : 1.0.0.0
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#ifndef __BSP_GPIO_H
#define __BSP_GPIO_H


#include "stm32f10x.h"


//FLASH CS(NSS)引脚 片选选普通GPIO即可
#define  EEPROM_SPI_CS_PIN              GPIO_Pin_12
#define  EEPROM_SPI_CS_PORT             GPIOB

//TMC CS(NSS)引脚 片选选普通GPIO即可
#define  TMC_SPI_CS_PIN              	GPIO_Pin_5
#define  TMC_SPI_CS_PORT             	GPIOB


////系统运行指示灯
//#define  SYS_LED_PIN					GPIO_Pin_6
//#define  SYS_LED_PORT             		GPIOB





//宏定义----------------------------------------------------------------------//
//FLASH 片选信号控制
#define  EEPROM_CS_LOW            GPIO_ResetBits(EEPROM_SPI_CS_PORT, EEPROM_SPI_CS_PIN)
#define  EEPROM_CS_HIGH           GPIO_SetBits(EEPROM_SPI_CS_PORT, EEPROM_SPI_CS_PIN)

//TMC 片选信号控制
#define  TMC0_SPI_CS_LOW            GPIO_ResetBits(TMC_SPI_CS_PORT, TMC_SPI_CS_PIN)
#define  TMC0_SPI_CS_HIGH           GPIO_SetBits(TMC_SPI_CS_PORT, TMC_SPI_CS_PIN)

//
#define TMC1_SPI_CS_HIGH	//HAL_GPIO_WritePin(SPI1_M1_CS_GPIO_Port, SPI1_M1_CS_Pin, GPIO_PIN_SET)
#define TMC1_SPI_CS_LOW		//HAL_GPIO_WritePin(SPI1_M1_CS_GPIO_Port, SPI1_M1_CS_Pin, GPIO_PIN_RESET)
//
#define TMC2_SPI_CS_HIGH	//HAL_GPIO_WritePin(SPI1_M2_CS_GPIO_Port, SPI1_M2_CS_Pin, GPIO_PIN_SET)
#define TMC2_SPI_CS_LOW		//HAL_GPIO_WritePin(SPI1_M2_CS_GPIO_Port, SPI1_M2_CS_Pin, GPIO_PIN_RESET)



//片选信号
#define  FLASH_CS_LOW                  0
#define  FLASH_CS_HIGH                 1


//FLASH CS(NSS)引脚 片选选普通GPIO即可
#define  M0_EN_Pin              	GPIO_Pin_6
#define  M0_EN_GPIO_Port            GPIOA

//Motor En
#define TMC0_ENABLE			GPIO_ResetBits(M0_EN_GPIO_Port, M0_EN_Pin)
#define TMC0_DISABLE		GPIO_SetBits(M0_EN_GPIO_Port, M0_EN_Pin)
//
#define TMC1_ENABLE			//HAL_GPIO_WritePin(M1_EN_GPIO_Port, M1_EN_Pin, GPIO_PIN_RESET)
#define TMC1_DISABLE		//HAL_GPIO_WritePin(M1_EN_GPIO_Port, M1_EN_Pin, GPIO_PIN_SET)
//
#define TMC2_ENABLE			//HAL_GPIO_WritePin(M2_EN_GPIO_Port, M2_EN_Pin, GPIO_PIN_RESET)
#define TMC2_DISABLE		//HAL_GPIO_WritePin(M2_EN_GPIO_Port, M2_EN_Pin, GPIO_PIN_SET)
//
#define TMC3_ENABLE			
#define TMC3_DISABLE		
//
#define TMC4_ENABLE			
#define TMC4_DISABLE	
//
#define TMC5_ENABLE			
#define TMC5_DISABLE	



//系统指示灯
#define SYS_LED_HIGH	GPIO_SetBits(SYS_LED_PORT, SYS_LED_PIN)
#define SYS_LED_LOW		GPIO_ResetBits(SYS_LED_PORT, SYS_LED_PIN)
#define SYS_LED_TRIGGER	{uint32_t ulOut = 0; ulOut = SYS_LED_PORT->ODR, SYS_LED_PORT->BSRR = ((ulOut & SYS_LED_PIN) << 16) | (~ulOut & SYS_LED_PIN);}



//定义结构体--------------------------------------------------------------------//




//声明变量----------------------------------------------------------------------//





//声明函数----------------------------------------------------------------------//
void GPIO_Config(void);





#endif



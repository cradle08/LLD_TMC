/*****************************************************************************
Copyright  : BGI
File name  : bsp_i2c_gpio.h
Description: iic模拟通信
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#ifndef _BSP_I2C_GPIO_H
#define _BSP_I2C_GPIO_H


#include <inttypes.h>
#include "stm32f10x.h"




//宏定义-----------------------------------------------------------------------//
//IIC读写控制
#define  I2C_WR                        0                   //写控制bit
#define  I2C_RD                        1                   //读控制bit


//定义I2C总线连接的GPIO端口, 用户只需要修改下面4行代码即可任意改变SCL和SDA的引脚
#define  GPIO_PORT_I2C                 GPIOB
#define  RCC_I2C_PORT                  RCC_APB2Periph_GPIOB

//连接到SCL时钟线的GPIO
#define  I2C1_SCL_PIN                  GPIO_Pin_6

//连接到SDA数据线的GPIO
#define  I2C1_SDA_PIN                  GPIO_Pin_7


//连接到SCL时钟线的GPIO
#define  I2C2_SCL_PIN                  GPIO_Pin_10

//连接到SDA数据线的GPIO
#define  I2C2_SDA_PIN                  GPIO_Pin_11


//#if 1
//	//时钟信号
//	#define  I2C_SCL_1()               GPIO_SetBits(GPIO_PORT_I2C, I2C1_SCL_PIN)
//	#define  I2C_SCL_0()               GPIO_ResetBits(GPIO_PORT_I2C, I2C1_SCL_PIN)

//	//数据信号
//	#define  I2C_SDA_1()               GPIO_SetBits(GPIO_PORT_I2C, I2C1_SDA_PIN)
//	#define  I2C_SDA_0()               GPIO_ResetBits(GPIO_PORT_I2C, I2C1_SDA_PIN)

//	//读SDA口线状态
//	#define  I2C_SDA_READ()            GPIO_ReadInputDataBit(GPIO_PORT_I2C, I2C1_SDA_PIN)  	
//#else    //这个分支选择直接寄存器操作实现IO读写 
//    //注意：如下写法，在IAR最高级别优化时，会被编译器错误优化
//	#define  I2C_SCL_1()               GPIO_PORT_I2C->BSRR = I2C1_SCL_PIN
//	#define  I2C_SCL_0()               GPIO_PORT_I2C->BRR = I2C1_SCL_PIN
//	
//	#define  I2C_SDA_1()               GPIO_PORT_I2C->BSRR = I2C1_SDA_PIN
//	#define  I2C_SDA_0()               GPIO_PORT_I2C->BRR = I2C1_SDA_PIN
//	
//	#define  I2C_SDA_READ()            ((GPIO_PORT_I2C->IDR & I2C1_SDA_PIN) != 0)
//#endif


//IIC总线状态状态机
#define  IIC_RESET_DEV                 0u        //复位设备
#define  IIC_INIT                      1u        //初始化
#define  IIC_BUS_NORMAL                2u        //总线正常
#define  IIC_RELEASE                   3u        //释放IIC
#define  IIC_RESET                     4u        //复位IIC




//iic编号
enum tagIICIndex
{
	IIC_1 = 0,
	IIC_2 = 1,
	
	IIC_NUM
};



//定义结构体--------------------------------------------------------------------//
//模拟iic驱动
struct tagIICDriver
{
	void       (*GPIO_Cfg)(void);  
	uint8_t    (*SCL_Out)(uint8_t dat);  
	uint8_t    (*SDA_Write)(uint8_t dat);  
	uint8_t    (*SDA_Read)(void);

	uint8_t    State;	
	uint8_t    Dev1Err;
	uint8_t    Dev2Err;
	uint8_t    Dev1CommReset;
	uint8_t    Dev2CommReset;
};



//声明变量----------------------------------------------------------------------//

extern struct tagIICDriver    GPIOIIC[];





//声明函数----------------------------------------------------------------------//
void GPIO_IIC1_Init(void);
void GPIO_IIC2_Init(void);

uint8_t i2c_CheckDevice(struct tagIICDriver *iic, uint8_t dev_addr);
uint8_t i2c_CheckDevice2(struct tagIICDriver *iic, uint8_t dev_addr);
uint8_t i2c_ReadBytes(struct tagIICDriver *iic, uint8_t dev_addr, uint16_t reg_addr, uint8_t *buf, uint16_t size);
uint8_t i2c_ReadBytes2(struct tagIICDriver *iic, uint8_t dev_addr, uint16_t reg_addr, uint8_t *buf, uint16_t size);
uint8_t i2c_WriteBytes(struct tagIICDriver *iic, uint8_t dev_addr, uint16_t reg_addr, uint8_t *buf, uint16_t size);
uint8_t i2c_WriteBytes2(struct tagIICDriver *iic, uint8_t dev_addr, uint16_t reg_addr, uint8_t *buf, uint16_t size);
uint8_t ReleaseIIC(I2C_TypeDef* I2Cx, struct tagIICDriver* iic);


#endif


/*****************************************************************************
Copyright  : BGI
File name  : bsp_iic.h
Description: iic通信
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#ifndef __BSP_IIC_H
#define	__BSP_IIC_H


#include "stm32f10x.h"



//宏定义-----------------------------------------------------------------------//
//I2C标准传输速度，100kbit/s
//#define  IIC1_SPEED                    100000
//#define  IIC2_SPEED                    100000
//I2C快速传输速度，400kbit/s。上拉4.7K电阻；怀疑iic通信出现过中断。
#define  IIC1_SPEED                    400000
#define  IIC2_SPEED                    400000 


//这个地址只要与STM32外挂的I2C器件地址不一样即可
#define  I2Cx_OWN_ADDRESS7             0X0A   


//等待超时时间
//0x1000，72MHz时钟，示波器实测1.952ms（增加P电机控制后，疑似IIC通信时序被打断）
#define  I2CT_FLAG_TIMEOUT             ((uint32_t)0x1000)
//#define  I2CT_FLAG_TIMEOUT             ((uint32_t)0x4000)    //实测仍然失败
//#define  I2CT_FLAG_TIMEOUT             ((uint32_t)0x6000)    //实测仍然失败
//0x1000 * 10=0xA000，72MHz时钟，示波器实测19.44ms
#define  I2CT_LONG_TIMEOUT             ((uint32_t)(2 * I2CT_FLAG_TIMEOUT))


//IIC设备应答
typedef enum
{
    IIC_ACR_ERR = 0x00,
    IIC_ACR_NORMAL = 0x01,
}IIC_REPLY;



//结构体-----------------------------------------------------------------------//


//声明变量---------------------------------------------------------------------//


//声明函数---------------------------------------------------------------------//
void IIC1_Init(void);
void IIC2_Init(void);
void IIC2_Reset(void);

uint32_t I2C_Mem_Write(I2C_TypeDef* I2Cx, uint16_t DevAddress, uint8_t* pBuffer, uint8_t WriteAddr, uint8_t NumByteToWrite);
uint32_t I2C_Mem_Read(I2C_TypeDef* I2Cx, uint16_t DevAddress, uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);
uint32_t I2C_Mem_Read2(I2C_TypeDef* I2Cx, uint16_t DevAddress, uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);
uint32_t I2C_CheckDevice(I2C_TypeDef* I2Cx, uint16_t DevAddress, uint8_t rw);


#endif

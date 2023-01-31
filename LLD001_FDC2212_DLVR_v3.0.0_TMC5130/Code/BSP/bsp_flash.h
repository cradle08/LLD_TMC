/*****************************************************************************
Copyright  : BGI
File name  : bsp_flash.h
Description: 提供flash读写接口
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#ifndef __BSP_FLASH_H
#define __BSP_FLASH_H


#include "stm32f10x.h"




//宏定义----------------------------------------------------------------------//
//根据使用的芯片型号设置参数
#define  STM32_FLASH_SIZE              64                            //所选STM32的FLASH容量大小(单位为K)
#define  STM32_FLASH_BASE              0x08000000                    //STM32 FLASH的起始地址，FLASH解锁键值


//每个扇区的大小。STM32F103小容量和中等容量MCU，扇区是1KB，大容量则是2KB。
#if(STM32_FLASH_SIZE < 256)
#define  STM_SECTOR_SIZE               1024 				         //字节
#else 
#define  STM_SECTOR_SIZE               2048
#endif

	 

//扇区存储单元（每个存储单元占2字节）
//#define  STM_SECTOR_STORE_CELL         (STM_SECTOR_SIZE / 2)
#define  STM_SECTOR_STORE_CELL         512



//定义结构体--------------------------------------------------------------------//
//采集数据相关
struct tagSTMFlash
{
	uint16_t   SectorBuf[STM_SECTOR_STORE_CELL];		             //扇区缓存
//	uint16_t   w_test[16];
//	uint16_t   r_test[16];
//	
//	uint16_t   Cycle;
};



//声明变量----------------------------------------------------------------------//
extern struct tagSTMFlash    STMFlash;




//声明函数----------------------------------------------------------------------//
uint16_t STMFLASH_SeqRead(uint32_t sector, uint32_t addr, uint16_t *buff, uint16_t num);
uint16_t STMFLASH_SeqWrite(uint32_t sector, uint32_t addr, uint16_t *buff, uint16_t num);				   




#endif



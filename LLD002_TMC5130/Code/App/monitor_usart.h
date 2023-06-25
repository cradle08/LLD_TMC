/*****************************************************************************
Copyright  : BGI
File name  : monitor_usart.h
Description: 上位机监控
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#ifndef __MONITOR_USART_H
#define __MONITOR_USART_H


#include "stm32f10x.h"
#include "include.h"



//宏定义----------------------------------------------------------------------//


//定义结构体--------------------------------------------------------------------//
//Modbus功能码
struct tagModbusMon
{
	struct tagUartCom        Usart;            //usart发送管理结构体
	struct tagDataPacket     Modbus;           //Modbus发送管理结构体
	
	uint16_t   InputReg[20];                   //30001-30010
	uint16_t   HoldingReg[20];                 //40001-40010
};


//声明变量----------------------------------------------------------------------//
extern struct tagModbusMon    ModbusMon;	   


//声明函数----------------------------------------------------------------------//
void MonModbusCommInit(void);
uint8_t Usart2Updata(struct tagModbusMon *mon);
uint8_t Usart2FillBuff(struct tagModbusMon* mon);

void MonWriteHoldingReg(struct tagModbusMon* mon);
void MonReadHoldingReg(struct tagModbusMon* mon);
void MonReadInputReg(struct tagModbusMon* mon);



#endif



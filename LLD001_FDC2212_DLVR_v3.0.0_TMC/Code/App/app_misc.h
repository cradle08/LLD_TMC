/*****************************************************************************
Copyright  : BGI
File name  : app_misc.c
Description: 乱七八糟的东西
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#ifndef __APP_MISC_H
#define __APP_MISC_H


#include "stm32f10x.h"




//宏定义----------------------------------------------------------------------//  
//读取软件
#define  INS_RESET_NAME                0u
#define  INS_READ_SW_NAME              1u
#define  INS_READ_SW_OUTLEN            253u
#define  INS_READ_SW_CONTINUE          254u
#define  INS_READ_SW_FINISH            255u




//定义结构体--------------------------------------------------------------------//
//采集数据相关
struct tagSoftWare
{
	uint8_t    InsCtrl;                          //指令控制
	uint8_t    InsResult;                        //指令执行结果
	uint8_t    NameSegNo;
	
};

//采集数据相关
struct tagSoftSys
{
	uint8_t   DipSW[1];
	
	struct tagSoftWare   SoftWare;
};


//声明变量----------------------------------------------------------------------//
extern struct tagSoftSys    SoftSys;




//声明函数----------------------------------------------------------------------//
uint8_t ReadSoftWareNameIns(uint8_t *buf, uint8_t num);
uint8_t ReadSoftWareName(uint8_t *buf, uint8_t num);
uint8_t ReadSoftWareVer(uint8_t *buf, uint8_t num);




#endif

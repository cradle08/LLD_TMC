/*****************************************************************************
Copyright  : BGI
File name  : gpio.h
Description: gpio管脚
Author     : lmj
Version    : 1.0.0.0
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#ifndef __GPIO_H
#define __GPIO_H


#include "stm32f10x.h"





//宏定义----------------------------------------------------------------------//
#define  RT_TABLE_NUM                  91     //数组大小
#define  RT_TEMP                       -200   //RT表的第一个数组为-20℃
#define  DESC                          0      //降序
#define  ASC                           1      //升序
#define  ERR_TEMP_MIN                  -990   //温度故障最小值
#define  ERR_TEMP_MAX                  1990   //温度故障最大值

//电容、气压数据个数
#define  TEMP_BUFFNUM                  6

//拨码确定状态次数
#define  SW_PASS_TIME                  10




//宏定义----------------------------------------------------------------------//
//DI端口数量
enum tagDIIndex
{
	DI_LIGHT_ELE = 0,
	
	DI_NUM
};

//温度数量
enum tagTEMPIndex
{
	TEMP_PIPE_IN = 0,
	TEMP_PIPE_OUT,
	
	TEMP_NUM
};

//Dip数量
enum tagDIPIndex
{
	DIP1 = 0,
	DIP2,
	DIP3,
	DIP4,
	
	DIP_NUM
};




//定义结构体--------------------------------------------------------------------//
//存放数据
struct tagADValue
{
	uint16_t   OrigValue;	                                         //原始AD值
	uint16_t   FilterValue;                                          //经过滤波后的值
	
	int32_t    Value;                                                //经过各种转换后的值
//	int32_t    ValueFloat;
	uint8_t    ErrTime;                                              //传感器异常计时
	uint8_t    IsErr;                                                //是否故障，0:无故障；1:故障。
};

//温度传感器
struct tagTempSensor
{
	struct tagADValue    AirPump;                                    //气泵温度
	
	uint16_t   AppOrigBuff[TEMP_BUFFNUM];
	uint16_t   AppFilterBuff[TEMP_BUFFNUM];
	
	uint8_t    UpdateFlag;                                           //数据有更新
};

//检测DI信号
struct tagDIMan
{
	uint8_t    HighCnt;	                                             //检测高电平次数。
	uint8_t    LowCnt;                                               //检测低电平次数。
	uint8_t    Status;                                               //检测结果。
};




//声明变量----------------------------------------------------------------------//
extern struct tagDIMan         DIMan[];
extern struct tagTempSensor    TempSensor[];
extern struct tagDIMan         DipSW[];



//声明函数----------------------------------------------------------------------//
void CheckPhotosensor(void);
uint8_t LLDMonReadIO(uint8_t *buf, uint8_t num);
uint8_t LLD_OutPutPulse(uint8_t sig);
uint8_t FDC2212SetAddr(uint8_t sig);
uint8_t FDC2212ShutDownInput(uint8_t sig);

uint8_t DipSWInit(void);
uint8_t DipSWCheck(void);

uint8_t TempNTC(void);

uint8_t WriteTemp(uint8_t *buf, uint8_t num);
uint8_t ReadTemp(uint8_t *buf, uint8_t num);



#endif



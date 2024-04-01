/*****************************************************************************
Copyright  : BGI
File name  : storage.h
Description: 存储管理
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#ifndef __STORAGE_H
#define __STORAGE_H

#include "stm32f10x.h"




//宏定义----------------------------------------------------------------------//
//存储管理状态机
#define  STORAGE_NULL                  0u    //空闲
#define  STORAGE_CONFIG                1u    //配置
#define  STORAGE_RUN                   2u    //运行
#define  STORAGE_ERR                   3u    //故障



//用户参数编号
enum tagUserParaIndex
{
	//CAN设备号
	CAN_ID = 0,	
	
	//电容传感器寄存器
	CAP_RCOUNT_CH0,
	CAP_OFFSET_CH0,
	CAP_SETTLECOUNT_CH0,
	CAP_CLOCK_DIVIDERS_CH0,
	CAP_STATUS_CONFIG,
	CAP_CONFIG,
	CAP_MUX_CONFIG,
	CAP_DRIVE_CURRENT_CH0,
	
	
	LLD_SEN_FUN = 9,
	
	//电容探测液面参数
	CAP_CON_THRESHOLD_H,
	CAP_CON_THRESHOLD_L,
	CAP_LEV_THRESHOLD_H,
	CAP_LEV_THRESHOLD_L,
	
	CAP_SET_GEAR,  //预留
	CAP_SET_CONTIME,
	
	CAP_DETE_OBJ,
	
	
	//气压探测液面参数
	AIR_DETE_OBJ,
	
	AIR_ATMOS_NOISE,
	
	//气压斜率1
	AIR_RAKERATIO1_MIN = 19,
	AIR_RAKERATIO1_MAX,
	
	//气压斜率2
	AIR_RAKERATIO2_MIN,
	AIR_RAKERATIO2_MAX,
	
	//气压探测凝块
	AIR_ASP_LIQ_NOISE,
	AIR_RAKERATIO3_MIN,
	AIR_RAKERATIO3_MAX,
	
	AIR_ABS_WAIT_DLY,
	AIR_ABS_START_DLY,
	AIR_ABS_END_DLY,
	
	AIR_ABS_START_END = 29,
	AIR_DIS_START_END,
	
	
	USER_PARA_NUM
};


//定义结构体--------------------------------------------------------------------//
//存储
struct tagStorage
{
	uint16_t   MachineNo;                                                      //机器型号
	uint16_t   HardWareNo;                                                     //软件版本
	uint16_t   SoftWareNo;                                                     //硬件版本
	
	uint16_t   Reserved;                                                       //保留
	uint16_t   PageCRC8;                                                       //页CRC8
	
	uint8_t    Stage;                                                          //状态机阶段
	uint16_t   StageRunTime;                                                   //状态机阶段运行时间
	uint8_t    IsErr;                                                          //存储故障
	
	
	uint8_t    ParaNo;
};


//参数结构体
struct tagPara
{
	uint8_t    Index;                        //数据编号
	uint16_t   Value;                        //值
	uint16_t   Min;                          //最小值
	uint16_t   Max;                          //最大值
//	uint8_t    Step;                         //修改基数
//	uint8_t    DispType;                     //显示方式  0以正常方式显示中央区域2个数码管   1以温度方式显示中央区域4个数码管,包括小数点和摄氏度单位
//	uint8_t    ParaType  :1;                 //0用户参数   1工厂参数
//	uint8_t    ReadWrite :1;                 //0可读可写   1可读不可写
//	uint8_t    Changed   :1;                 //参数被改变
};




//声明变量----------------------------------------------------------------------//
//用户参数
extern struct tagStorage    Storage;
extern struct tagPara    UserPara[];



//声明函数----------------------------------------------------------------------//
uint8_t SetPara16(struct tagPara *data, uint16_t val);
uint8_t SetPara32(struct tagPara *data, uint32_t val);

void FLASH_Init(void);
void StorageManStage(void);



#endif


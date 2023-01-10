/*****************************************************************************
Copyright  : BGI
File name  : monitor_can.h
Description: 上位机监控
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#ifndef __MONITOR_CAN_H
#define __MONITOR_CAN_H


#include "stm32f10x.h"
#include "bsp_can.h" 
#include "bsp_lib.h" 




//宏定义----------------------------------------------------------------------//
//与AD1750通信状态机
#define  MON_CAN_NULL                  0u    //空闲
#define  MON_CAN_RECE                  1u    //接收
#define  MON_CAN_REPLY                 2u    //应答

//指令
#define  INS_IO                        4u    //IO读写
//#define  INS_LED                       5u    //LED读写
#define  INS_TEMP                      6u  //温度

#define  INS_RESERVE_201               201u  //预留
#define  INS_SELECT_SEN                202u  //选用传感器
#define  INS_READ_LLD                  203u  //读取液面探测数据
#define  INS_RESERVE_204               204u  //预留
#define  INS_RESERVE_205               205u  //预留
#define  INS_RESERVE_206               206u  //预留

#define  INS_CAPLLD_PARA1              207u  //设置电容探测参数1
#define  INS_RESERVE_208               208u  //设置电容探测参数2
#define  INS_CAPLLD_PARA3              209u  //设置电容探测参数3
#define  INS_CAP_READ_VAL              210u  //读电容映射值

#define  INS_AIRLLD_PARA1              211u  //设置气压探测参数1
#define  INS_AIRLLD_PARA2              212u  //设置气压探测参数2
#define  INS_AIRLLD_PARA3              213u  //设置气压探测参数3
#define  INS_AIR_READ_VAL              214u  //读气压映射值
#define  INS_AIRLLD_PARA4              215u  //设置气压探测参数4
#define  INS_AIRLLD_PARA5              216u  //设置气压探测参数5


#define  INS_R_SOFTWARE_NAME           252u  //读软件名称
#define  INS_R_SOFTWARE_VER            253u  //读软件版本

#define  INS_W_PARA                    254u  //写参数参数
#define  INS_R_PARA                    255u  //读参数参数


//操作码
#define  OPE_READ                      0u    //读
#define  OPE_WRITE                     1u    //写

//故障码
#define  ERR_CAN_NULL                  0u    //正常
#define  ERR_CAN_INS                   1u    //指令错误
#define  ERR_CAN_OPE                   2u    //操作错误
#define  ERR_CAN_DATA_FAILD            3u    //数据错误

#define  ERR_DEV1_FAILD                10u    //设备1故障
#define  ERR_DEV2_FAILD                11u    //设备2故障
#define  ERR_TEMP1_FAILD               20u    //温度传感器1故障
#define  ERR_TEMP2_FAILD               21u    //温度传感器2故障



//Can操作
#define  CAN_OPE_REPLY                 0u    //应答
#define  CAN_OPE_NO_REPLY              1u    //不应答
#define  CAN_OPE_WAIT                  5u    //等待

//Can阻塞应答最长时间
#define  CAN_ACK_BLOCK_WAIT            500u    //N个单位时间

//Can应答超时
#define  CAN_ACK_TIMEOUT               550u    //N个单位时间



//Can应用通信地址
#define  CAN_ID_MIM                    201u
#define  CAN_ID_MAX                    208u

//Can应用通信地址（电机部分）
#define  CAN_ID_MIM_MOTOR              101u
#define  CAN_ID_MAX_MOTOR              108u
#define  CAN_MSG_DATA_LENGTH		   8

//Can波特率模式
#define  CAN_BPS_MODE_1M               0u
#define  CAN_BPS_MODE_500K             1u


//Can二维缓存
#define  CAN_QUEUE_LOOP_LEN            8u
#define  CAN_QUEUE_LOOP_WIDTH          8u







//定义结构体--------------------------------------------------------------------//
//Can通信报文
struct tagCanMess
{
	uint8_t     MessID;
	uint8_t     Ins;                                                           //指令，命令号
	uint8_t     OpeCode;                                                       //操作码。
	uint8_t     DevNo;                                                         //操作设备的编号。
	
	uint32_t    ErrCode;
};


//Can广播报文
struct tagBroadcast
{
	uint8_t     DevAddr;
	uint8_t     FunCode;                                                       //功能码
	uint16_t    RegAddr;                                                       //寄存器地址
};


//Can通信管理
struct tagMonCan
{
	//报文相关
	struct tagCanConfig      Confg;
	CanTxMsg    TxMsg;                                                         //发送缓冲区
	CanRxMsg    RxMsg;                                                         //接收缓冲区
	
	//协议相关
	uint8_t     ComStage;                                                      //通信状态机
	uint8_t     ReceFinish;                                                    //接收报文结果
	uint8_t     ReceBroadcastFinish;                                           //接收广播
	uint8_t     AckBlock;                                                      //应答阻塞
	uint16_t    AckBlockTime;                                                  //应答阻塞计时
	
	struct tagCanMess        CanMess;
	struct tagBroadcast      Broadcast;	
	
	struct tagQueueLoop      RxQueue;
	uint8_t     RxBuf[CAN_QUEUE_LOOP_LEN][CAN_QUEUE_LOOP_WIDTH];
};









//声明变量----------------------------------------------------------------------//
extern struct tagMonCan    MonCan;


//声明函数----------------------------------------------------------------------//
uint8_t CAN_MonInit(void);
uint8_t CanIRQRecv(void);

uint8_t MonAnalyseCanMess(void);
uint8_t MonSendCanMess(void);
void CanMonComStage(void);


#endif



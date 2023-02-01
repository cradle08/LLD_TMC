#ifndef __PROCESS_H__
#define __PROCESS_H__

#ifdef __cplusplus
extern "C" {
#endif

//#include "bsp_can.h"
#include "main.h"
#include "public.h"
#include "project.h"
#include "TMC_Api.h"
#include "event.h"
#include "msg_handle.h"



/*
*	模块异常类型
*/
typedef enum {
	MODULE_ERROR_TYPE_MissStep	= 0,	//丢步

}ModuleErrorType_e;


/*
*	电机复位状态
*/
typedef enum {
	MOTOR_RESET_STATUS_NONE		= 0,	//未复位,上电后，未执行复位操作
	MOTOR_RESET_STATUS_ING,				//复位中
	MOTOR_RESET_STATUS_FINISH,			//复位完成
	MOTOR_RESET_STATUS_FAIL,			//复位失败

}MotorResetStatus_e;


/*
*	电机复位--执行状态
*/
typedef enum {
	MOTOR_RESET_EXEC_1		= 0,	
	MOTOR_RESET_EXEC_2,				
	MOTOR_RESET_EXEC_3,	
	MOTOR_RESET_EXEC_4,		
	MOTOR_RESET_EXEC_5,				
	MOTOR_RESET_EXEC_6,	
	MOTOR_RESET_EXEC_7,		
	MOTOR_RESET_EXEC_8,				
	MOTOR_RESET_EXEC_9,	
	MOTOR_RESET_EXEC_10,		
	MOTOR_RESET_EXEC_11,				
	MOTOR_RESET_EXEC_12,	
	MOTOR_RESET_EXEC_13,		
	
}MotorResetExec_e;



/*
*	电机复位信息，结构体
*/
typedef __packed struct {
	
	uint8_t				ucResetMode;	//左参考点 or 右参考点
	MotorResetStatus_e  eResetStatus;	//电机原点复位完成标志位，0：未复位，1：复位中， 2：复位已完成， 3：复位失败
	MotorResetExec_e    eResetExec;     //复位执行状态
	uint32_t 			ulStartTick;	//复位起始tick
	
} MotorReset_Info_t;



///*
//*	Can消息数据统计
//*/
//typedef __packed struct {
//	
//	uint32_t ulSendSuccessNum;	//发送消息次数
//	uint32_t ulSendFailNum;		//发送错误-消息次数
//	uint32_t ulRecvSuccessNum;	//接受消息次数
//	uint32_t ulRecvFailNum;		//接受失败-消息次数
//	uint32_t ulRecvErrorNum;	//接受错误-消息次数
//	uint32_t ulRecvOverNum;		//接受溢出次数
//	
//} CanMsgCount_Info_t;



/*
*	全局状态结构体
*/
typedef __packed struct {
	
	uint8_t  ucEnableFlag[TMC_MODULE_END];	  //TMC使能标志，0：失能，1：使能
//	uint8_t  ucMotorMoveFlag[TMC_MODULE_END]; //电机运动标志位，通过该标志，切换初始保持电流和运行保持电流	
	
	uint8_t  ucErrStatus[TMC_MODULE_END];	  //TMC模块状态
	uint32_t ulBoardStatus; 				  //板卡状态	
//	uint8_t  ucEEPRAM_Init_CRC_ErrFlag;	 	  //EPPROM状态： 0:正常，1：初始化读取参数异常		 
	uint8_t  ucExecProcessStatus;			  //执行流程状态
	
	uint8_t ucMotorResetStartFlag;			  //复位开启标志， 0：关闭，1：开启				
	MotorReset_Info_t  tMotorResetInfo[TMC_MODULE_END];	//复位状态信息

//	//CAN消息统计信息
//	CanMsgCount_Info_t tCanMsgCount_Info;
//	
//	//板卡识别码
//	uint16_t usSN;		//对MCU的UUID做crc16的校验，得到的值。
	
} TMCStatus_t;









//全局参数初始化
void Global_Status_Init(void);

//获取SN号
uint16_t Get_SN(uint8_t ucFlag);

//寄存器初始化
//void ShadowRegister_Init(AxisParamDefault_t *ptAxisParamDefault);

//MCU 重启
void MCU_Reset(void);

//出厂初始化
void Reset_Factory(void);

//获取软硬件版本号
void Get_Soft_HardWare_Version(uint8_t *pucaData);

//获取模块类型
uint32_t Get_Module_Type(void);

//异常检测
void Period_Error_Check(uint32_t ulTick);

//模块异常状态处理
ErrorType_e Module_Error_Handle(TMC_e eTMC, ModuleErrorType_e eType);

//丢步处理
ErrorType_e MissStep_Handle(TMC_e eTMC);


//接受消息处理
uint8_t Handle_RxMsg(MsgType_e eMsgType, RecvFrame_t *ptRecvFrame, SendFrame_t *ptSendFrame);


//事件处理
void Event_Process(void);




//LED 闪耀
void LED_Shine(uint16_t usCount, uint32_t ulTime);




/************************************************/
//调试功能
#if MGIM_DEBUG

void Param_WR_Test(void);

#endif



  
#ifdef __cplusplus
}
#endif

#endif //__PROCESS_H__








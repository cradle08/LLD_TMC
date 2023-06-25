#ifndef __MSG_HANDLE_H__
#define __MSG_HANDLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "event.h"


//
#define CAN_DATA_MAX_LEN 	8
#define USART_DATA_MAX_LEN 	8

/*
 * cmd define
 */
//
//#define CMD_ROTATE_RIGHT				0x10 	//rotate right
//#define CMD_ROTATE_LEFT				0x11 	//rotate left


#define CMD_ROTATE						0x10 	//rotate left
#define CMD_MOVE_POSITION_WITHOUT_ENC	0x11	//move position, without enc
#define CMD_MOVE_POSITION_WITH_ENC		0x12	//move position  with enc
#define CMD_STOP						0x13 	//stop
#define CMD_MOTOR_RESET					0x14	//motor reset
#define CMD_URGENT_STOP					0x15 	//urgent stop

//
#define CMD_MCU_REST					0x20	//mcu reset
#define CMD_QUERY_BOARD_TYPE			0x21 	//query board type
#define CMD_HARD_SOFT_VERSION			0x22	//query hard soft

//params
#define CMD_SET_AXIS_PARAM				0x30	//set axis param
#define CMD_GET_AXIS_PARAM				0x31	//get axis param
#define CMD_SET_DEFAULT_AXIS_PARAM		0x3A	//set default axis param
#define CMD_GET_DEFAULT_AXIS_PARAM		0x3B	//get default axis param

#define CMD_SET_GLOBAL_PARAM			0x33	//set module param
#define CMD_GET_GLOBAL_PARAM			0x34	//get module param
#define CMD_SET_IO_STATUS				0x35	//set io status
#define CMD_GET_INPUT_IO_STATUS			0x36	//get input io status
#define CMD_GET_OUTPUT_IO_STATUS		0x37	//get output io status

//
#define CMD_SET_EXEC_PROCESS			0x40	//set exec process
#define CMD_GET_EXEC_PROCESS			0x41	//get exec process
#define CMD_EXEC_PROCESS_CTRL			0x42	//control(run or stop) exec process
#define CMD_CLS_SAVE_EXEC_PROCESS		0x43	//clear or save exec process

//
#define CMD_QUERY_STATUS				0x50 	//query module status
#define CMD_ERROR_HANDLE				0x51	//error handle
//
#define CMD_CLEAR_EEPROM_PARAM			0x60	//clear default params

//update
#define CMD_UPGRADE_LANCH				0x70	//update lanch
#define CMD_UPGRADE_START				0x71	//update start
#define CMD_UPGRADE_APP_DATA			0x72	//update data
#define CMD_UPGRADE_FINISHED			0x73	//update finished
#define CMD_QUERY_RUNING_SOFT_TYPE		0x75	//查询当前正在运行的程序类型

//
#define CMD_GET_SN_CAN_ID				0x80	//获取SN号及CAN ID
#define CMD_SET_CAN_ID_WITH_SN			0x81	//根据系列号设置SN号及CAN ID
#define CMD_TYPE_WITH_SN				0x82	//根据系列号--获取板卡类型
#define CMD_SHINE_WITH_SN				0x83	//根据系列号--闪灯
#define CMD_SHAKE_WITH_SN				0x84	//根据系列号--控制电机抖动

// others
#define CMD_LABEL						0x90	//label for jump
#define CMD_WAIT						0x91	//wait condition change
#define CMD_DELAY						0x92	//wait condition change
#define CMD_CALC						0x93	//calc result of last subproces

#define CMD_JA							0x95	//jump 
#define CMD_JC							0x96	//jump and compare result of last subprocess
//
#define CMD_SET_AXIS_PARAM_AAP 			0x97	//设置轴参数--缓冲值
#define CMD_SET_GLOBAL_PARAM_AGP		0x98	//设置全局参数--缓冲值
#define CMD_TIME_COUNT					0x9A	//计时器触发 
#define CMD_TIME_COUNT_A				0x9B	//计时器触发—-缓冲值 
#define CMD_TIME_COUNT_CHECK			0x9C	//计时器触发检测
//
#define CMD_TEST						0xFE	 //move with v




/*
*	CAN Recv Msg Format，CAN 应答消息格式
*/
typedef struct {
	//CAN_RxHeaderTypeDef tCan_RxHeader;
	uint32_t ulRecvCanID;
	uint8_t  ucaRxData[CAN_DATA_MAX_LEN];

}Can_RxMsg_t;




/*
*	CAN Send Msg Format，CAN 发送消息格式
*/
typedef struct {
	//CAN_TxHeaderTypeDef tCan_TxHeader;
	uint8_t  ucaTxData[CAN_DATA_MAX_LEN];

}Can_TxMsg_t;




/*
*	接受报文格式
*/
typedef __packed struct {
	uint8_t  	ucMsgID;	//消息ID
	uint8_t  	ucDeviceID;	//设备ID
	uint8_t  	ucCmd;		//指令
	uint8_t  	ucType;		//类型
	Data4Byte_u	uData;		//数据

} RecvFrame_t;




/*
*	发送报文格式
*/
typedef __packed struct {
	uint8_t  	ucMsgID;	//消息ID
	uint8_t  	ucStatus;	//状态
	uint8_t  	ucCmd;		//指令
	uint8_t  	ucType;		//类型
	Data4Byte_u	uData;		//数据

} SendFrame_t;





/*
*	Can消息数据统计
*/
typedef __packed struct {
	
	uint32_t ulSendSuccessNum;	//发送消息次数
	uint32_t ulSendFailNum;		//发送错误-消息次数
	uint32_t ulRecvSuccessNum;	//接受消息次数
	uint32_t ulRecvFailNum;		//接受失败-消息次数
	uint32_t ulRecvErrorNum;	//接受错误-消息次数
	uint32_t ulRecvOverNum;		//接受溢出次数
	
} CanMsgCount_Info_t;



/*
*	全局状态结构体
*/
typedef __packed struct {
	

	uint8_t  ucEEPRAM_Init_CRC_ErrFlag;	 	  //EPPROM状态： 0:正常，1：初始化读取参数异常		 

	//CAN消息统计信息
	CanMsgCount_Info_t tCanMsgCount_Info;
	
	//板卡识别码
	uint16_t usSN;		//对MCU的UUID做crc16的校验，得到的值。
	
} BoardStatus_t;




//Can应答消息
uint8_t Can_Send_Msg(SendFrame_t *ptSendFrame);

//Can消息处理
uint8_t Handle_Can_RxMsg(Can_RxMsg_t *ptRxMsg);

//串口消息处理
uint8_t Handle_Usart_RxMsg(MsgUsart_t *ptMsgUsart);




//获取识别码
uint16_t Get_SN(uint8_t ucFlag);











  
#ifdef __cplusplus
}
#endif

#endif //__MSG_HANDLE_H__


#include "main.h"
#include "bsp_usart.h"
//#include "bsp_outin.h"
#include "bsp_can.h"
#include "msg_handle.h"
#include "public.h"
#include "log.h"
#include "event.h"
#include "crc.h"



#if (CURRENT_SOFT_TYPE==SOFT_TYPE_APP)
#include "TMC5130.h"
#include "TMC5130_Register.h"
#include "TMC5130_Constants.h"
#include "TMC_Api.h"
#include "TMC_Process.h"
#include "process.h"
#endif


#if (CURRENT_SOFT_TYPE==SOFT_TYPE_BOOT)
#include "boot.h"

#endif



//全局状态信息
__IO BoardStatus_t g_tBoardStatus = {0};






/*
*	获取SN号
* 	
*	SN号生成规则：对MCU的UUID的CRC16校验值
*	ucFlag：1=打印信息，0=不打印
*/

uint16_t Get_SN(uint8_t ucFlag)
{
	//UUID
	uint16_t usSN = 0;
	uint32_t ulaUUID[3] = {0};
	uint32_t ulUUID_Addr = MCU_UUID_FLASH_ADDR;

	//获取UUID值
	ulaUUID[0] = *(uint32_t*)(ulUUID_Addr);
	ulaUUID[1] = *(uint32_t*)(ulUUID_Addr+4);
	ulaUUID[2] = *(uint32_t*)(ulUUID_Addr+8);

	//模块1：MCU_UID 执行1次CRC16得到SN1
	usSN = CRC16((uint8_t*)ulaUUID, sizeof(ulaUUID));
	g_tBoardStatus.usSN = usSN;


	//模块2：SN1 执行1次CRC16得到SN2
	usSN = CRC16((uint8_t*)(&usSN), sizeof(usSN));
	g_tBoardStatus.usSN_LLD = usSN; 
	
  
	//打印信息
	if(0 != ucFlag)
	{
	//	LOG_Info("UUID=%X %X %X, SN=%X", ulaUUID[0], ulaUUID[1], ulaUUID[2], usSN);
	}

	//返回SN
	return usSN;
}





///* 
//*
//*	发送CAN应答消息
//*/
//uint8_t Can_Send_Msg(SendFrame_t *ptSendFrame)
//{
//	extern CAN_HandleTypeDef hcan;
//	extern __IO BoardStatus_t g_tBoardStatus;
//	uint32_t ulTxMailbox = 0;
//	Can_TxMsg_t tTxMsg = {0};
//	
//	//Tx Header
//	tTxMsg.tCan_TxHeader.StdId 				= Send_CanID();
//	tTxMsg.tCan_TxHeader.ExtId 				= 0;
//	tTxMsg.tCan_TxHeader.RTR	  			= CAN_RTR_DATA;
//	tTxMsg.tCan_TxHeader.IDE	  			= CAN_ID_STD;
//	tTxMsg.tCan_TxHeader.DLC   				= 8;
//	tTxMsg.tCan_TxHeader.TransmitGlobalTime	= 0;
//	//消息体
//	memmove((void*)tTxMsg.ucaTxData, (void*)ptSendFrame, CAN_DATA_MAX_LEN);
//	
//	//发送应答消息
//	if(0 == HAL_CAN_AddTxMessage(&hcan, &tTxMsg.tCan_TxHeader, tTxMsg.ucaTxData, &ulTxMailbox))
//	{
//		g_tBoardStatus.tCanMsgCount_Info.ulSendSuccessNum++;
//	}else{
//		g_tBoardStatus.tCanMsgCount_Info.ulSendFailNum++;
//		//LOG_Error("Can Send Msg Faill %d", g_tBoardStatus.tCanMsgCount_Info.ulSendFailNum);
//	}
//	
////	//发送失败, 检测TEERx标志
////	if(__HAL_CAN_GET_FLAG(&hcan, CAN_FLAG_ALST0) || __HAL_CAN_GET_FLAG(&hcan, CAN_FLAG_ALST2) || __HAL_CAN_GET_FLAG(&hcan, CAN_FLAG_ALST2))
////	{
////		g_tBoardStatus.tCanMsgCount_Info.ulSendFailNum++;
////	}
//	
//	return 0;
//}



/* 
*
*	发送CAN应答消息
*/
uint8_t Can_Send_Msg(SendFrame_t *ptSendFrame)
{
//	extern struct tagMonCan    MonCan;
	uint8_t    mailbox = 0;
	CanTxMsg    TxMsg;
	
	//填充报文
	TxMsg.StdId = Send_CanID();; 
	TxMsg.ExtId = 0x00;
	TxMsg.IDE = CAN_ID_STD;                 //标准模式
	TxMsg.RTR = CAN_RTR_DATA;               //发送的是数据
	TxMsg.DLC = 8;                          //数据长度为8字节	
	TxMsg.Data[0] = ptSendFrame->ucMsgID;
	
	memmove((void*)TxMsg.Data, (void*)ptSendFrame, 8);
	
//	TxMsg.Data[0] = ptSendFrame->ucMsgID;
//	TxMsg.Data[1] = ptSendFrame->ucStatus;
//	TxMsg.Data[2] = ptSendFrame->ucCmd;
//	TxMsg.Data[3] = ptSendFrame->ucType;
//	TxMsg.Data[4] = ptSendFrame->uData[0];
//	TxMsg.Data[5] = ptSendFrame->uData[1];
//	TxMsg.Data[6] = ptSendFrame->uData[2];
//	TxMsg.Data[7] = ptSendFrame->uData[3];
	
	
	//发送报文
	mailbox = CAN_Transmit(CAN1, &TxMsg);
	return(mailbox);

}






/*
*   *** 发送速度已大于30ms为佳 ***
*
*	解析、出来can收到的消息
*
*   Recv Msg Format:
*	Byte0:  报文编号，
*	Byte1： DeviceID
*	Byte2： CMD
*	Byte3： Type
*   Byte4:  数据,Data3
*   Byte5:  数据,Data2
*   Byte6:  数据,Data1
*   Byte7:  数据,Data0
*
*
*   Send Msg Format:
*	Byte0:  报文编号
*	Byte1： Status
*	Byte2： 保留，Reserve
*	Byte3： 保留，Reserve
*   Byte4:  数据,Data3
*   Byte5:  数据,Data2
*   Byte6:  数据,Data1
*   Byte7:  数据,Data0
*
*/
uint8_t Handle_Can_RxMsg(MsgCan_t *canRxMsg)
{
	uint8_t      reply = CAN_TxStatus_Ok;
	Can_TxMsg_t  tTxMsg = {0};
	
	
	
	MsgType_e eMsgType = MSG_TYPE_CAN;
	//获取接收的数据
	RecvFrame_t *ptRecvFrame = (RecvFrame_t*)canRxMsg->ucaBuffer;
	
	//打印接受消息
//	LOG_Info("Recv Msg: MsgID=%02X, DeviceID=%02X, Cmd=%02X, Type=%02X, Data=%02X %02X %02X %02X", ptRecvFrame->ucMsgID, \
			ptRecvFrame->ucDeviceID, ptRecvFrame->ucCmd, ptRecvFrame->ucType, ptRecvFrame->uData.ucData[0],\
			ptRecvFrame->uData.ucData[1],ptRecvFrame->uData.ucData[2],ptRecvFrame->uData.ucData[3]);
	
	//发送报文--头部分
	SendFrame_t *ptSendFrame = (SendFrame_t*)tTxMsg.ucaTxData;
	ptSendFrame->ucMsgID     = ptRecvFrame->ucMsgID;
	ptSendFrame->ucStatus    = 0;
	ptSendFrame->ucCmd  	 = ptRecvFrame->ucCmd;
	ptSendFrame->ucType  	 = ptRecvFrame->ucType;

	
	
	
//	//判断是否为广播消息，如何是广播消息，只支持“获取板卡识别码及CAN ID信息”协议
//	if(ptRxMsg->ulRecvCanID == CAN_BROADCAST_ID_MOTOR  && ptRxMsg->ucaRxData[2] != CMD_GET_SN_CAN_ID)
//	{
//		ptSendFrame->ucStatus    = ERROR_TYPE_EXEC_RIGH;
//		Can_Send_Msg(ptSendFrame);
//		return 0;
//	}
	
	//接受消息处理
	if(0 == Handle_RxMsg(eMsgType, canRxMsg, ptRecvFrame, ptSendFrame))
	{	
		//应答消息
		reply = Can_Send_Msg(ptSendFrame);
	}
	
	
	return reply;
}





/*
*	串口消息处理，（当前该功能仅用于调试！！！）
* 
*/
uint8_t Handle_Usart_RxMsg(MsgUsart_t *ptMsgUsart)
{
//	extern CAN_HandleTypeDef hcan;
	
	MsgType_e eMsgType = MSG_TYPE_USART;
	uint8_t ucaStData[USART_DATA_MAX_LEN+1] = {0};
	uint8_t ucaRxData[USART_DATA_MAX_LEN] = {0};

	
	//获取接收的数据
	if(ptMsgUsart->usLen > USART_DATA_MAX_LEN) ptMsgUsart->usLen = USART_DATA_MAX_LEN;
	
	memmove(ucaRxData, ptMsgUsart->ucaBuffer, ptMsgUsart->usLen);
	
	RecvFrame_t *ptRecvFrame = (RecvFrame_t*)ucaRxData;
	
	//打印接受消息
//	LOG_Info("Recv Msg: MsgID=%02X, DeviceID=%02X, Cmd=%02X, Type=%02X, Data=%02X %02X %02X %02X", ptRecvFrame->ucMsgID, \
			ptRecvFrame->ucDeviceID, ptRecvFrame->ucCmd, ptRecvFrame->ucType, ptRecvFrame->uData.ucData[0],\
			ptRecvFrame->uData.ucData[1],ptRecvFrame->uData.ucData[2],ptRecvFrame->uData.ucData[3]);
	
	//发送报文--头部分
	SendFrame_t *ptSendFrame = (SendFrame_t*)ucaStData;
	ptSendFrame->ucMsgID     = ptRecvFrame->ucMsgID;
	ptSendFrame->ucStatus    = 0;
	ptSendFrame->ucCmd  	 = ptRecvFrame->ucCmd;
	ptSendFrame->ucType  	 = ptRecvFrame->ucType;

	//接受消息处理
//	Handle_RxMsg(eMsgType, ptRxMsg, ptRecvFrame, ptSendFrame);
	
	//应答消息
//	LOG_Info("%s", ucaStData);
	return 0;
}







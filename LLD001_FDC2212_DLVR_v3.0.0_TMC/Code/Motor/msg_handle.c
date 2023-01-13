#include "main.h"
#include "bsp_usart.h"
//#include "bsp_outin.h"
#include "bsp_can.h"
#include "msg_handle.h"
//#include "public.h"
//#include "log.h"
#include "event.h"
#include "monitor_can.h"
//#include "crc.h"
//

//#if (CURRENT_SOFT_TYPE==SOFT_TYPE_APP)
//#include "TMC5160.h"
//#include "TMC5160_Register.h"
//#include "TMC5160_Constants.h"
//#include "TMC_Api.h"
//#include "TMC_Process.h"
//#include "process.h"
//#endif


//#if (CURRENT_SOFT_TYPE==SOFT_TYPE_BOOT)
//#include "boot.h"

//#endif



////全局状态信息
//__IO BoardStatus_t g_tBoardStatus = {0};






///*
//*	获取SN号
//* 	
//*	SN号生成规则：对MCU的UUID的CRC16校验值
//*	ucFlag：1=打印信息，0=不打印
//*/

//uint16_t Get_SN(uint8_t ucFlag)
//{
//	//UUID
//  uint16_t usSN = 0;
//  uint32_t ulaUUID[3] = {0};
//  uint32_t ulUUID_Addr = MCU_UUID_FLASH_ADDR;
//  
//  //获取UUID值
//  ulaUUID[0] = *(uint32_t*)(ulUUID_Addr);
//  ulaUUID[1] = *(uint32_t*)(ulUUID_Addr+4);
//  ulaUUID[2] = *(uint32_t*)(ulUUID_Addr+8);
//  
//  //CRC16
//  usSN = CRC16((uint8_t*)ulaUUID, sizeof(ulaUUID));
//  
//  //打印信息
//  if(0 != ucFlag)
//  {
//	LOG_Info("UUID=%X %X %X, SN=%X", ulaUUID[0], ulaUUID[1], ulaUUID[2], usSN);
//  }
//  
//  //返回SN
//  return usSN;
//}





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
//	if(HAL_OK == HAL_CAN_AddTxMessage(&hcan, &tTxMsg.tCan_TxHeader, tTxMsg.ucaTxData, &ulTxMailbox))
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
//	return HAL_OK;
//}





/* 
*
*	发送CAN应答消息
*/
uint8_t Can_Send_Msg(SendFrame_t *ptSendFrame)
{
	extern struct tagMonCan    MonCan;
	uint8_t    mailbox = 0;
	CanTxMsg    TxMsg;
	
	//填充报文
	TxMsg.StdId = MonCan.Confg.ModuleID_Motor; 
	TxMsg.ExtId = 0x00;
	TxMsg.IDE = CAN_ID_STD;                 //标准模式
	TxMsg.RTR = CAN_RTR_DATA;               //发送的是数据
	TxMsg.DLC = 8;                          //数据长度为8字节	
	TxMsg.Data[0] = ptSendFrame->ucMsgID;
	memmove((void*)TxMsg.Data, (void*)ptSendFrame, CAN_MSG_DATA_LENGTH);
	
	//发送报文
	mailbox = CAN_Transmit(CAN1, &TxMsg);
	return(mailbox);

}




/* 
*
*	处理Motor消息
*/
uint8_t Handle_Motor_Msg(RecvFrame_t *ptRecvFrame, SendFrame_t *ptSendFrame)
{
	uint8_t SendFlag = 0;

	
	switch(ptRecvFrame->ucCmd)
	{
		case CMD_ROTATE:  //0x10
		{
			
		}
		break;
		case CMD_MOVE_POSITION_WITHOUT_ENC:  //0x11
		{
			
		}
		break;
		case CMD_MOVE_POSITION_WITH_ENC: //0x12
		{		
		
		
		}
		break;
		case CMD_STOP:   //0x13
		{	
		
		}
		break;
		case CMD_MOTOR_RESET: //0x14
		{




		}
		break;
		case CMD_MCU_REST:  //0x20
		{


		}
		break;		
		case CMD_QUERY_BOARD_TYPE:  //0x21
		{


		}
		break;		
		case CMD_HARD_SOFT_VERSION:  //0x22
		{
			
			
			
		}
		break;
		case CMD_SET_AXIS_PARAM:  //0x30
		{			
			
			
		}
		break;
		case CMD_GET_AXIS_PARAM:  //0x31
		{


		}
		break;		
		case CMD_SET_DEFAULT_AXIS_PARAM: //0x3A
		{



		}
		break;
		case CMD_GET_DEFAULT_AXIS_PARAM: //0x3B
		{
			
			
			
			
		}
		break;		
		case CMD_SET_GLOBAL_PARAM:  //0x33
		{			
			
			
		}
		break;
		case CMD_GET_GLOBAL_PARAM:  //0x34
		{




		}
		break;			
		case CMD_SET_IO_STATUS:  //0x35
		{
			
			
		}
		break;
		case CMD_GET_INPUT_IO_STATUS:  //0x36
		{


		}
		break;	
		case CMD_GET_OUTPUT_IO_STATUS: //0x37
		{			
			
			
		}
		break;
		case CMD_SET_EXEC_PROCESS:  //0x40
		{
				
			
			
		}
		break;	
		case CMD_GET_EXEC_PROCESS:  //0x41
		{		
			
			
		}
		break;
		case CMD_EXEC_PROCESS_CTRL:  //0x42
		{		
			
			
		}
		break;
		case CMD_CLS_SAVE_EXEC_PROCESS:  //0x43
		{



		}
		break;
		case CMD_QUERY_STATUS:  //0x50
		{
			
			
		}
		break;
		case CMD_CLEAR_EEPROM_PARAM: //0x60
		{
			
			
		}
		break;
		case CMD_UPGRADE_LANCH: //0x70
		{


		}
		break;
		case CMD_QUERY_RUNING_SOFT_TYPE: //0x75
		{
			
			
		}
		break;
		case CMD_GET_SN_CAN_ID: //0x80
		{



		}
		break;
		case CMD_SET_CAN_ID_WITH_SN: //0x81
		{



		}
		break;
		case CMD_SHAKE_WITH_SN: //0x82
		{
			
			
			
		}
		break;
		case CMD_SHINE_WITH_SN: //0x83
		{


		}
		default:
		{
			ptSendFrame->ucStatus = ERROR_TYPE_CMD;
		}
		break;		
	}	
	return SendFlag;
}



/*
*   ******
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
uint8_t Handle_Can_RxMsg(Can_RxMsg_t *ptRecvCanMsg)
{


	Can_TxMsg_t tTxMsg = {0};
	
	
	//获取接收的数据
	RecvFrame_t *ptRecvFrame = (RecvFrame_t*)ptRecvCanMsg->ucaRxData;
	
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

	
	//判断是否为广播消息，如何是广播消息，只支持“获取板卡识别码及CAN ID信息”协议
	if(ptRecvCanMsg->ulRecvCanID == CAN_BROADCAST_ID_MOTOR  && ptRecvCanMsg->ucaRxData[2] != CMD_GET_SN_CAN_ID)
	{
		ptSendFrame->ucStatus    = ERROR_TYPE_EXEC_RIGH;
		Can_Send_Msg(ptSendFrame);
		return 0;
	}
	
	//接受消息处理
	if(0 == Handle_Motor_Msg(ptRecvFrame, ptSendFrame))
	{	
		//应答消息
		Can_Send_Msg(ptSendFrame);
	}
	
	return 0;
}






/*
*	消息事件处理
*/
void Event_Process(void)
{

    SysEvent_t *e;
	SysEvent_t tSysEvent = {0};
    
	__disable_irq();
    e = SysEventGet();
    if(e){
		memmove((void*)&tSysEvent, (void*)e, sizeof(SysEvent_t));
		SysEventFree(e);
		__enable_irq();
	}else{
		__enable_irq();
		return;
	}
    
	//
    switch(tSysEvent.eMsgType)
    {  
        case MSG_TYPE_CAN:
        {
            Can_RxMsg_t tRecvCan = {0};
			memmove((void*)&tRecvCan.ucaRxData, (void*)&tSysEvent.tMsg.tMsgCan.ucaBuffer, sizeof(MsgCan_t));
			tRecvCan.ulRecvCanID = tSysEvent.tMsg.tMsgCan.ulRecvCanID;
            Handle_Can_RxMsg(&tRecvCan);
        }
        break;
//        case MSG_TYPE_USART:
//        {
//			MsgUsart_t tRxMsg = {0};
//			memmove((void*)&tRxMsg, (void*)&tSysEvent.tMsg.tMsgUsart, sizeof(MsgUsart_t));
//            Handle_Usart_RxMsg(&tRxMsg);
//        }
//        break;
        default:break;
    }

}


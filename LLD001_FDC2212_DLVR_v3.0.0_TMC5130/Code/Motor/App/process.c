#include "main.h"
#include "bsp_usart.h"
//#include "bsp_outin.h"
#include "bsp_can.h"
#include "process.h"
#include "Event.h"
#include "bsp_gpio.h"
#include "msg_handle.h"
#include "process.h"
#include "eeprom.h"
#include "param.h"
#include "crc.h"
#include "public.h"
#include "log.h"

#include "TMC_Api.h"
#include "TMC5130.h"
#include "TMC_Process.h"

//
#include "lld_param.h"
#include "monitor_can.h" 

#include "liquid_level.h"





//TMC状态
__IO TMCStatus_t g_tTMCStatus = {0};



/*
*	全局值初始化
*/
void Global_Status_Init(void)
{
	extern __IO BoardStatus_t g_tBoardStatus;
	
	TMC_e eTMC = TMC_0;
	memset((void*)&g_tBoardStatus, 0, sizeof(BoardStatus_t));
	
	for(eTMC = TMC_0; eTMC < TMC_MODULE_END; eTMC++)
	{
		g_tTMCStatus.ulBoardStatus 	 = 0;
		g_tTMCStatus.tMotorResetInfo[eTMC].eResetStatus = MOTOR_RESET_STATUS_NONE; //电机复位状态。 0：未复位
	}
}









/*
*	MCU 重启
*/
void MCU_Reset(void)
{
	//MCU 复位, 单片机复位后，TMC状态（寄存器的值等信息状体）
	__set_PRIMASK(1);
	NVIC_SystemReset();
}






///*
//*	清除EEPROM保存的轴参数
//*/
//void ClearAndSave_Default_Axis_Params(void)
//{
//	Axis_Param_Fixed_SetDefault_Value(&g_tAxisParamDefault);
//	//Param_Write(EN_SAVE_PARAM_TYPE_AXIS, (uint8_t*)&g_tAxisParamDefault, sizeof(AxisParamDefault_t));
//	Save_Axis_Param_Default(&g_tAxisParamDefault);
//}



/*
*	清除所有保存在EEPROM的参数, 并设置为默认值
*/
void Reset_Factory(void)
{
	ClearAndSave_Default_Global_Params();
	ClearAndSave_Default_Axis_Params();
	ClearAndSave_Default_Process();
	
	//液面探测参数
	ClearAndSave_Default_LLDParams();
}





/*
 *  获取软硬件版本号
 */
void Get_Soft_HardWare_Version(uint8_t *pucaData)
{
	extern uint16_t g_usSoftVersion;		//软件版本号
	extern uint16_t g_usHardWareVesion;	//硬件版本号

	//软件版本号
	pucaData[0] = g_usSoftVersion & 0xFF;
	pucaData[1] = (g_usSoftVersion >> 8) & 0xFF;
	
	//硬件版本号
	pucaData[2] = g_usHardWareVesion & 0xFF;
	pucaData[3] = (g_usHardWareVesion >> 8) & 0xFF;

}



/*
 *  获取模块类型
 */
uint32_t Get_Module_Type(void)
{

	return CURRENT_MODULE_TYPE;

}




/*
* 状态检测, 1:有相关异常， 0：无相关异常
*/
void Period_Error_Check(uint32_t ulTick)
{
	extern __IO TMCStatus_t g_tTMCStatus;
	extern __IO BoardStatus_t g_tBoardStatus;
	
	static uint32_t s_ulTick = 0, s_ulTick2 = 0;
	TMC_e eTMC = TMC_0;
	int32_t lStatus = 0;
	uint8_t ucFlag = 0;
	
	//每隔200ms
	if(ulTick - s_ulTick >= 200)
	{
		s_ulTick = ulTick;
		//每个10ms，检测一次状态
		
		/* 检测、记录TMC状态信息 */
		for(eTMC = TMC_0; eTMC < TMC_MODULE_END; eTMC++)
		{
			lStatus = TMC_ReadInt(eTMC, TMC5160_DRVSTATUS);
			
			/* A相开路 */
			if((lStatus >> 30) == 1)
			{
				//设置位
				BIT_SET(g_tTMCStatus.ucErrStatus[eTMC], 0);
			}else{
				//清除位
				BIT_RESET(g_tTMCStatus.ucErrStatus[eTMC], 0);
			}
			

			/* B相开路 */
			if((lStatus >> 29) == 1)
			{
				//设置位
				BIT_SET(g_tTMCStatus.ucErrStatus[eTMC], 1);
			}else{
				//清除位
				BIT_RESET(g_tTMCStatus.ucErrStatus[eTMC], 1);
			}
			
			
			/* A相短路 */
			if((lStatus >> 28) == 1)
			{
				//设置位
				BIT_SET(g_tTMCStatus.ucErrStatus[eTMC], 2);
			}else{
				//清除位
				BIT_RESET(g_tTMCStatus.ucErrStatus[eTMC], 2);
			}
			
						
			/* B相短路 */
			if((lStatus >> 27) == 1)
			{
				//设置位
				BIT_SET(g_tTMCStatus.ucErrStatus[eTMC], 3);
			}else{
				//清除位
				BIT_RESET(g_tTMCStatus.ucErrStatus[eTMC], 3);
			}
			
			
			/* 过温 */
//			if((lStatus >> 26) == 1)
//			{
//				//设置位
//				BIT_SET(g_tBoardStatus.ucTMCStatus[eTMC], 5);
//			}else{
//				//清除位
//				BIT_RESET(g_tBoardStatus.ucTMCStatus[eTMC], 5);
//			}
			
			
			/* 过压检测  */
			
			
			/* 堵转检测  */			
		}
		
		
		/* 标记TMC状态标志，只要有一个发生，就会标记 */
		/* B相开路 */
		for(eTMC = TMC_0; eTMC < TMC_MODULE_END; eTMC++)
		{
			//A相开路
			if((g_tTMCStatus.ucErrStatus[eTMC] & 0x01) == 1) ucFlag = 1;
		}
		if(ucFlag == 1){			
				BIT_SET(g_tTMCStatus.ulBoardStatus, 0);
		}else{ 
				BIT_RESET(g_tTMCStatus.ulBoardStatus, 0);
		}
		
		/* B相开路 */
		for(eTMC = TMC_0; eTMC < TMC_MODULE_END; eTMC++)
		{
			//B相开路
			if((g_tTMCStatus.ucErrStatus[eTMC] & 0x02) == 1) ucFlag = 1;
		}
		if(ucFlag == 1)		
		{			
				BIT_SET(g_tTMCStatus.ulBoardStatus, 1);
		}else{ 
				BIT_RESET(g_tTMCStatus.ulBoardStatus, 1);
		}


		/* A相短路 */
		for(eTMC = TMC_0; eTMC < TMC_MODULE_END; eTMC++)
		{
			if((g_tTMCStatus.ucErrStatus[eTMC] & 0x04) == 1) ucFlag = 1;
		}
		if(ucFlag == 1)		
		{			
				BIT_SET(g_tTMCStatus.ulBoardStatus, 2);
		}else{ 
				BIT_RESET(g_tTMCStatus.ulBoardStatus, 2);
		}
		
		
		/* B相短路 */
		for(eTMC = TMC_0; eTMC < TMC_MODULE_END; eTMC++)
		{
			if((g_tTMCStatus.ucErrStatus[eTMC] & 0x08) == 1) ucFlag = 1;
		}
		if(ucFlag == 1)		
		{			
				BIT_SET(g_tTMCStatus.ulBoardStatus, 3);
		}else{ 
				BIT_RESET(g_tTMCStatus.ulBoardStatus, 3);
		}
		
		/* 初始化EEPROM 读写异常 */
		if(1 == g_tBoardStatus.ucEEPRAM_Init_CRC_ErrFlag)
		{
			BIT_SET(g_tTMCStatus.ulBoardStatus, 9);
		}else{
			BIT_RESET(g_tTMCStatus.ulBoardStatus, 9);
		}
		
		
		/* 执行流程异常 */
		if(1 == g_tTMCStatus.ucExecProcessStatus)
		{
			BIT_SET(g_tTMCStatus.ulBoardStatus, 9);
		}else{
			BIT_RESET(g_tTMCStatus.ulBoardStatus, 9);
		}	
	}
	
	
	//每隔10ms，检测一次
	if(ulTick - s_ulTick2 >= 10)
	{
		s_ulTick2 = ulTick;
		
		/* 失步检测  */
		for(eTMC = TMC_0; eTMC < TMC_MODULE_END; eTMC++)
		{
			/* 失步检测  */
			lStatus = TMC_ReadInt(eTMC, TMC5160_ENC_STATUS);
			if(lStatus & 0x02)
			{
				//设置位，失步，不会自动清除
				BIT_SET(g_tTMCStatus.ucErrStatus[eTMC], 4);
				//LOG_Error("Step Error");
			
			}else{
				//清除位
				BIT_RESET(g_tTMCStatus.ucErrStatus[eTMC], 4);
			}			
		}	
		//
		for(eTMC = TMC_0; eTMC < TMC_MODULE_END; eTMC++)
		{
			if((g_tTMCStatus.ucErrStatus[eTMC] & 0x10) == 1) ucFlag = 1;
		}
		if(ucFlag == 1)		
		{			
				BIT_SET(g_tTMCStatus.ulBoardStatus, 8);
		}else{ 
				BIT_RESET(g_tTMCStatus.ulBoardStatus, 8);
		}
		
		
		/* CAN通信应答异常 */
		if(g_tBoardStatus.tCanMsgCount_Info.ulSendFailNum >= 1)
		{
			BIT_SET(g_tTMCStatus.ulBoardStatus, 11);
		}
	}
}



/*
* 模块异常状态处理
*/
ErrorType_e Module_Error_Handle(TMC_e eTMC, ModuleErrorType_e eType)
{
	ErrorType_e eError = ERROR_TYPE_SUCCESS;
	switch(eType)
	{
		case MODULE_ERROR_TYPE_MissStep:
		{
			eError = MissStep_Handle(eTMC);
		
		}
		break;
		default:
		{
			eError = ERROR_TYPE_TYPE;
			LOG_Error("unkonwn Type=%d", eType);
		}
	
	}
	return eError;
}



/*
*	丢步处理	
*/
ErrorType_e MissStep_Handle(TMC_e eTMC)
{	
	ErrorType_e eError = ERROR_TYPE_SUCCESS;
	if(eTMC >= TMC_MODULE_END) return ERROR_TYPE_DEVICE_ID;
	   	
	//需处理丢步，是XENC和XVACTUAL小于失步阈值，再写1，清除该标志位。或关闭丢步检测功能，再执行运动，再清除该位。
	int32_t lAc = TMC5160_ReadInt(eTMC, TMC5160_XACTUAL);
	int32_t lEc = TMC5160_ReadInt(eTMC, TMC5160_XENC);
	int32_t lX = TMC5160_ReadInt(eTMC, TMC5160_XTARGET);

//	LOG_Info("lAc=%d, Enc=%d, lX=%d", lAc, lEc, lX);
	//TMC5160_WriteInt(eTMC, TMC5160_XENC, lAc);
	TMC5160_WriteInt(eTMC, TMC5160_XTARGET, lEc);
	TMC5160_WriteInt(eTMC, TMC5160_XACTUAL, lEc);


	lAc = TMC5160_ReadInt(eTMC, TMC5160_XACTUAL);
	lEc = TMC5160_ReadInt(eTMC, TMC5160_XENC);
	lX = TMC5160_ReadInt(eTMC, TMC5160_XTARGET);
	uint8_t flag = TMC5160_FIELD_READ(eTMC, TMC5160_ENC_STATUS, TMC5160_DEVIATION_WARN_MASK, TMC5160_DEVIATION_WARN_SHIFT);

	//清除丢步标志位
	TMC5160_FIELD_UPDATE(eTMC, TMC5160_ENC_STATUS, TMC5160_DEVIATION_WARN_MASK, TMC5160_DEVIATION_WARN_SHIFT, 1);

	return eError;
}




/*
*	急停处理
*/
ErrorType_e Urgent_Stop(TMC_e eTMC)
{
	ErrorType_e eErrorTyoe = ERROR_TYPE_SUCCESS;
	uint32_t ulValue = 0, ulStatus = 0, ulXActual = 0;
//	uint8_t ucValid = 0, ucPolarity = 0;

	//备份SWMODE的值
	ulValue = TMC_ReadInt(eTMC, TMC5160_SWMODE);

	/* 触发急停 */
//	ucValid = TMC5160_FIELD_READ(eTMC, TMC5160_RAMPSTAT, TMC5160_STATUS_STOP_L_MASK, TMC5160_STATUS_STOP_L_SHIFT);
//	ucPolarity = TMC5160_FIELD_READ(eTMC, TMC5160_SWMODE, TMC5160_POL_STOP_L_MASK, TMC5160_POL_STOP_L_SHIFT);
//	if(0 == ucValid && 0 == ucPolarity)
//	{
//		//高有效，此刻为无效状态 ==》即，左限位为低电平
//		ulStatus =  0x05; 
//	}else if(0 == ucValid && 1 == ucPolarity){
//		//高有效，此刻为有效状态 ==》即，左限位为高电平
//		ulStatus = 0x01;
//	}else if(1 == ucValid && 0 == ucPolarity){
//		//低有效，此刻为无效状态 ==》即，左限位为高电平
//		ulStatus = 0x01;
//	}else if(1 == ucValid && 1 == ucPolarity){
//		//低有效，此刻为有效状态 ==》即，左限位为低电平
//		ulStatus = 0x05;
//	}
	//TMC_WriteInt(eTMC, TMC5160_SWMODE, ulStatus);
	eErrorTyoe = TMC_WriteInt(eTMC, TMC5160_SWMODE, 0x03);

	//急停，后处理
	eErrorTyoe = TMC_WriteInt(eTMC, TMC5160_VMAX, 0);
	ulXActual = TMC_ReadInt(eTMC, TMC5160_XACTUAL);
	eErrorTyoe = TMC_WriteInt(eTMC, TMC5160_XTARGET, ulXActual);

	//恢复SWMODE的值
	eErrorTyoe = TMC_WriteInt(eTMC, TMC5160_SWMODE, ulValue);

	//复位状态下，急停
	if(g_tTMCStatus.ucMotorResetStartFlag != 0)
	{
		//
		g_tTMCStatus.tMotorResetInfo[eTMC].eResetStatus = MOTOR_RESET_STATUS_FAIL;
		
		//设置模式--位置模式
		TMC5160_WriteInt(eTMC, TMC5160_RAMPMODE, TMC_MODE_POSITION);
		
		//恢复速度设置
		TMC_SetPMode_V(eTMC, 2);
	}
	
	return eErrorTyoe;
}





/*
* 处理接受消息, 返回值，0：没有发送应答消息。1：已发送应答消息
* 
*/
uint8_t Handle_RxMsg(MsgType_e eMsgType, RecvFrame_t *ptRecvFrame, SendFrame_t *ptSendFrame)
{
//	extern CAN_HandleTypeDef hcan;
	extern Process_t g_tProcess;
	extern TMC5160_t g_taTMC5160[TMC_MODULE_END];
	extern __IO BoardStatus_t g_tBoardStatus;
	extern AxisParamDefault_t g_tAxisParamDefault;
	
	uint8_t ucSendFlag = 0; //应答消息，应答标志，0：无应答，1：已应答
	ErrorType_e eError = ERROR_TYPE_SUCCESS;
	TMC_e eTMC = TMC_0;
	
	
	/* device id参数检测 */
	if(ptRecvFrame->ucCmd == CMD_SET_GLOBAL_PARAM || ptRecvFrame->ucCmd == CMD_GET_GLOBAL_PARAM)
	{
		//全局参数bank检查
		if(ptRecvFrame->ucDeviceID >= MODULE_MAX_BANK_NUM)
		{
			ptSendFrame->ucStatus = ERROR_TYPE_DEVICE_ID;
			return ucSendFlag;
		}
	}else if(ptRecvFrame->ucCmd == CMD_SET_IO_STATUS || ptRecvFrame->ucCmd == CMD_GET_OUTPUT_IO_STATUS){
		//输出IO num检查
		if(ptRecvFrame->ucDeviceID >= MODULE_MAX_OUT_IO_NUM)
		{
			ptSendFrame->ucStatus = ERROR_TYPE_DEVICE_ID;
			return ucSendFlag;
		}
		
	}else if(ptRecvFrame->ucCmd == CMD_GET_INPUT_IO_STATUS){
		//输入IO num 检查
		if(ptRecvFrame->ucDeviceID >= MODULE_MAX_IN_IO_NUM)
		{
			ptSendFrame->ucStatus = ERROR_TYPE_DEVICE_ID;
			return ucSendFlag;
		}
	}else{
		//电机id检查
		eTMC = (TMC_e)ptRecvFrame->ucDeviceID;
		if(eTMC >= TMC_MODULE_END)
		{
			//LOG_Error("TMC DeviceID=%d Is Err", eTMC);
			ptSendFrame->ucStatus = ERROR_TYPE_DEVICE_ID;
			return ucSendFlag;
		}
	}
	
	
	//指令处理	
	switch(ptRecvFrame->ucCmd)
	{
		/**********************/
		case CMD_ROTATE:  //0x10
		{
			if(g_tTMCStatus.tMotorResetInfo[eTMC].eResetStatus == MOTOR_RESET_STATUS_ING)
			{
				ptSendFrame->ucStatus = ERROR_TYPE_EXEC_RIGH; //电机在复位中，不执行旋转指令
				return ucSendFlag;
			}
			
			TMC_WriteInt(eTMC, TMC5160_ENC_DEVIATION, 0); //关闭编码器失步检测功能
			
			//旋转
			ptSendFrame->ucStatus = TMC_Rotate(eTMC, ptRecvFrame->ucType, ptRecvFrame->uData.ulData);
			TMC_SetVMode_V(eTMC, 0);
			
			
		}
		break;
		case CMD_MOVE_POSITION_WITHOUT_ENC:  //0x11
		{
			if(g_tTMCStatus.tMotorResetInfo[eTMC].eResetStatus == MOTOR_RESET_STATUS_ING)
			{
				ptSendFrame->ucStatus = ERROR_TYPE_EXEC_RIGH; //电机在复位中，不执行移动指令
				return ucSendFlag;
			}	
				
			
			//关闭编码器失步检测功能
			TMC_WriteInt(eTMC, TMC5160_ENC_DEVIATION, 0);
			
			//移动
			if(0 == ptRecvFrame->ucType)
			{
				//绝对偏移
				ptSendFrame->ucStatus = TMC_MoveTo(eTMC, ptRecvFrame->uData.lData);
			}else{
				//相对偏移
				ptSendFrame->ucStatus = TMC_MoveBy(eTMC, ptRecvFrame->uData.lData);
			}
			TMC_SetPMode_V(eTMC, 0);
			
		}
		break;
		case CMD_MOVE_POSITION_WITH_ENC: //0x12
		{
			if(g_tTMCStatus.tMotorResetInfo[eTMC].eResetStatus == MOTOR_RESET_STATUS_ING)
			{
				ptSendFrame->ucStatus = ERROR_TYPE_EXEC_RIGH; //电机在复位中，不执行移动指令
				return ucSendFlag;
			}	
			
			//编码器失步阈值, 该值为零，则关闭该功能			
			ptSendFrame->ucStatus = TMC_WriteInt(eTMC, TMC5160_ENC_DEVIATION, g_tAxisParamDefault.lEncDiff_Threshold[eTMC]);
//			LOG_Info("ENC Deviation Start: Motor=%d, EncDiff=%d, Steps=%d, CurStep=%d", \
					eTMC, g_tAxisParamDefault.lEncDiff_Threshold[eTMC], ptRecvFrame->uData.lData, TMC5160_ReadInt(eTMC, TMC5160_XACTUAL));
			
			//移动
			if(0 == ptRecvFrame->ucType)
			{
				//绝对偏移
				ptSendFrame->ucStatus = TMC_MoveTo(eTMC, ptRecvFrame->uData.lData);
			}else{
				//相对偏移
				ptSendFrame->ucStatus = TMC_MoveBy(eTMC, ptRecvFrame->uData.lData);
			}
			TMC_SetPMode_V(eTMC, 0);
		}
		break;
		case CMD_STOP:   //0x13
		{
			//停止
			//LOG_Info("Before STOP XTarget=%d", TMC_ReadInt(eTMC, TMC5160_XACTUAL));
			ptSendFrame->ucStatus = TMC_Stop(eTMC);
			
			//复位状态下，停止
			if(g_tTMCStatus.ucMotorResetStartFlag != 0)
			{
				//
				g_tTMCStatus.tMotorResetInfo[eTMC].eResetStatus = MOTOR_RESET_STATUS_FAIL;
				
				//关闭做参考点复位
				TMC5160_FIELD_UPDATE(eTMC, TMC5160_SWMODE, TMC5160_STATUS_STOP_L_MASK, TMC5160_STATUS_STOP_L_SHIFT, 0);
				
				//设置模式--位置模式
				TMC5160_WriteInt(eTMC, TMC5160_RAMPMODE, TMC_MODE_POSITION);
				
				//恢复速度设置
				TMC_SetPMode_V(eTMC, 2);
			}
			
			//LOG_Info("End STOP XTarget=%d", TMC_ReadInt(eTMC, TMC5160_XACTUAL));
		}
		break;
		case CMD_MOTOR_RESET: //0x14
		{
			/* 复位，到原点（参考位置）*/
			
			//复位未完成
			if(g_tTMCStatus.tMotorResetInfo[eTMC].eResetStatus != MOTOR_RESET_STATUS_ING)
			{
				//修改复位控制信息，启动复位处理
				g_tTMCStatus.tMotorResetInfo[eTMC].eResetStatus = MOTOR_RESET_STATUS_ING;
				g_tTMCStatus.tMotorResetInfo[eTMC].eResetExec   = MOTOR_RESET_EXEC_1;
				g_tTMCStatus.ucMotorResetStartFlag  = 1; //执行复位
			}
//			g_tBoardStatus.eMotorResetStatus[eTMC] = 0;
//			TMC_Reset(eTMC, TMC_REF_LEFT);  //TMC_REF_RIGHT   TMC_REF_LEFT
//			//复位已完成
//			g_tBoardStatus.ucMotor_ResetStatus[eTMC] = 1;
		}
		break;
		case CMD_URGENT_STOP:   //0x15
		{
			/* 尽量避免在高速下，急停 */
			Urgent_Stop(eTMC);
			//LOG_Info("End STOP XTarget=%d", TMC_ReadInt(eTMC, TMC5160_XACTUAL));
		}
		break;
		
		/**********************/
		case CMD_MCU_REST:  //0x20
		{
			/* 重启mcu */
			
			//发送应答
			if(MSG_TYPE_CAN == eMsgType)
			{
				Can_Send_Msg(ptSendFrame);
				ucSendFlag = 1;
			}
			rt_thread_mdelay(3);//HAL_Delay(3);
			
			//重启
			MCU_Reset();
		}
		break;		
		case CMD_QUERY_BOARD_TYPE:  //0x21
		{
			//查询板卡类型
			ptSendFrame->uData.ulData = Get_Module_Type();
			//ptSendFrame->ucType = Recv_CanID();
			
		}
		break;		
		case CMD_HARD_SOFT_VERSION:  //0x22
		{
			//查询软硬件版本
			Get_Soft_HardWare_Version(ptSendFrame->uData.ucData);
		}
		break;
		
		
		/**********************/
		case CMD_SET_AXIS_PARAM:  //0x30
		{
			//设置轴参数
			ptSendFrame->ucStatus = TMC_AxisParam(eTMC, TMC_WRITE, ptRecvFrame->ucType, &ptRecvFrame->uData);
//			LOG_Info("Set Axis Parma,T=%d, V=%d", ptRecvFrame->ucType, ptRecvFrame->uData.lData)
		}
		break;
		case CMD_GET_AXIS_PARAM:  //0x31
		{
			//查询轴参数
			ptSendFrame->ucStatus = TMC_AxisParam(eTMC, TMC_READ, ptRecvFrame->ucType, &ptSendFrame->uData);
//			LOG_Info("Get Axis Parma,T=%d, V=%d", ptRecvFrame->ucType, ptSendFrame->uData.lData)
		}
		break;		
		case CMD_SET_DEFAULT_AXIS_PARAM: //0x3A
		{
			//设置默认轴参数
			ptSendFrame->ucStatus = TMC_AxisParam_Default(eTMC, TMC_WRITE, ptRecvFrame->ucType, &ptRecvFrame->uData);		
		
		}
		break;
		case CMD_GET_DEFAULT_AXIS_PARAM: //0x3B
		{
			//查询默认轴参数
			ptSendFrame->ucStatus = TMC_AxisParam_Default(eTMC, TMC_READ, ptRecvFrame->ucType, &ptSendFrame->uData);		
		}
		break;		
		
		case CMD_SET_GLOBAL_PARAM:  //0x33
		{
			/* 设置全局参数 */
			//应答
			if(MSG_TYPE_CAN == eMsgType)
			{
				Can_Send_Msg(ptSendFrame);
				ucSendFlag = 1;
			}
			
			//设置全局参数
			Bank_e eBank = (Bank_e)ptRecvFrame->ucDeviceID;
			ptSendFrame->ucStatus = TMC_Global_Param(eBank, TMC_WRITE, ptRecvFrame->ucType, &ptRecvFrame->uData);
//			LOG_Info("Set Module Parma,T=%d, V=%d", ptRecvFrame->ucType, lValue)
		}
		break;
		case CMD_GET_GLOBAL_PARAM:  //0x34
		{
			//查询全局参数
			Bank_e eBank = (Bank_e)ptRecvFrame->ucDeviceID;
			ptSendFrame->ucStatus = TMC_Global_Param(eBank, TMC_READ, ptRecvFrame->ucType, &ptSendFrame->uData);
//			LOG_Info("Get Module Parma,T=%d, V=%d", ptRecvFrame->ucType, ptSendFrame->uData.lData)
		}
		break;			
		case CMD_SET_IO_STATUS:  //0x35
		{
			//设置IO状态
//@todo			eError = Set_Out_IO(ptRecvFrame->ucType, ptRecvFrame->uData.ulData);
//@todo			ptSendFrame->ucStatus = (uint8_t)eError;
			ptSendFrame->ucStatus = ERROR_TYPE_DEVICE_ID;
		}
		break;
		case CMD_GET_INPUT_IO_STATUS:  //0x36
		{
			//查询IO状态
//			uint16_t usOutState = 0, usInState = 0;
			
//@todo			ptSendFrame->ucStatus = Get_In_IO_One(ptRecvFrame->ucType, &ptSendFrame->uData.ucData[0]);
			ptSendFrame->ucStatus = ERROR_TYPE_DEVICE_ID;
		}
		break;	
		case CMD_GET_OUTPUT_IO_STATUS: //0x37
		{
//@todo			ptSendFrame->ucStatus = Get_Out_IO_One(ptRecvFrame->ucType, &ptSendFrame->uData.ucData[0]);
			ptSendFrame->ucStatus = ERROR_TYPE_DEVICE_ID;
		}
		break;
		
		/**********************/
		case CMD_SET_EXEC_PROCESS:  //0x40
		{
			//设置执行流程
			ptSendFrame->ucStatus = Set_Process(ptRecvFrame);
		}
		break;	
		case CMD_GET_EXEC_PROCESS:  //0x41
		{
			//获取执行流程
			SubProcess_t tSubProcess = {0};
			//ucSendFlag = 1;
			
			for(uint8_t ucIndex = 0; ucIndex < SUB_PROCESS_MAX_CMD_NUM; ucIndex++)
			{
				memset((void*)&tSubProcess, 0, sizeof(SubProcess_t));
				Get_Process(ucIndex, &tSubProcess);
				
//				if(tSubProcess.ucParamNum >= SUB_PROCESS_MAX_PARAM_NUM)
//				{
//					//数据异常
//					LOG_Error("Param Num Error=%d", tSubProcess.ucParamNum);
//					ptSendFrame->ucStatus = ERROR_TYPE_CRC;
//					if(MSG_TYPE_CAN == eMsgType)
//					{
//						Can_Send_Msg(ptSendFrame);
//						break;
//					}
//				}
				
				/* send set "cmd"*/
				ptSendFrame->ucStatus = ERROR_TYPE_SUCCESS;
				ptSendFrame->uData.ucData[0] = tSubProcess.ucCmd;
				if(0 != tSubProcess.ucCmd)
				{
					ptSendFrame->ucType = ucIndex;
					Can_Send_Msg(ptSendFrame);
					rt_thread_mdelay(2);//HAL_Delay(2);
				}
		
				/* send set param */
				if(tSubProcess.ucParamNum != 0 && tSubProcess.ucParamNum <=  SUB_PROCESS_MAX_PARAM_NUM)
				{
					for(uint8_t i = 0; i < tSubProcess.ucParamNum; i++)
					{
						memset((void*)&ptSendFrame->uData.ulData, 0, sizeof(Data4Byte_u));
						ptSendFrame->ucType = ucIndex | 0x80;
						ptSendFrame->uData.lData = tSubProcess.uParam[i].lData;
						Can_Send_Msg(ptSendFrame);
						rt_thread_mdelay(2);//HAL_Delay(2);
					}
					rt_thread_mdelay(2);//HAL_Delay(2);
				}					
			}
			ptSendFrame->ucStatus = ERROR_TYPE_SUCCESS;		
			ptSendFrame->ucType = 0xFF;
			ptSendFrame->uData.ulData = 0;
		}
		break;
		case CMD_EXEC_PROCESS_CTRL:  //0x42
		{
			//执行流程--执行和停止
			Exec_Process_Ctrl(ptRecvFrame->ucType);
		}
		break;
		case CMD_CLS_SAVE_EXEC_PROCESS:  //0x43
		{
			//执行流程--清楚和保存
			Exec_Process_Clear_Or_Save(ptRecvFrame->ucType);
			//LOG_Info("Save Prcess");
		}
		break;

		
		
		/********************/
		case CMD_QUERY_STATUS:  //0x50
		{
			//查询状态
			ptSendFrame->uData.ulData = g_tTMCStatus.ulBoardStatus;
		}
		break;
//		case CMD_ERROR_HANDLE: //0x51
//		{
//			//异常状态处理
//			ptSendFrame->ucStatus = Module_Error_Handle(eTMC,  ptRecvFrame->ucType);
//		}
//		break;
		
		/********************/
		case CMD_CLEAR_EEPROM_PARAM: //0x60
		{
			//应答
			if(MSG_TYPE_CAN == eMsgType)
			{
				Can_Send_Msg(ptSendFrame);
				ucSendFlag = 1;
			}
			
			//处理
			if(0 == ptRecvFrame->ucType)
			{
				Reset_Factory();
			}else if(1 == ptRecvFrame->ucType){
				ClearAndSave_Default_Global_Params();
			}else if(2 == ptRecvFrame->ucType){
				ClearAndSave_Default_Axis_Params();
			}else if(3 == ptRecvFrame->ucType){
				ClearAndSave_Default_Process();
			}
			
			rt_thread_mdelay(3);//HAL_Delay(3);
			MCU_Reset();
		}
		break;
		case CMD_UPGRADE_LANCH: //0x70
		{
			/* 启动升级，跳转至Boot */
			//修改升级标志位，
			ptSendFrame->ucStatus = Set_UpdateFlag(1); //@todo, 改为跳转到boot的方式
			if(MSG_TYPE_CAN == eMsgType)
			{
				Can_Send_Msg(ptSendFrame);
				ucSendFlag = 1;
			}
			
			//重启
			rt_thread_mdelay(3);//HAL_Delay(3);
			MCU_Reset();
		}
		break;
		case CMD_QUERY_RUNING_SOFT_TYPE: //0x75
		{
			/* 查询当前正在执行的程序类型 */
			ptSendFrame->uData.ucData[0] = SOFT_TYPE_APP;
		}
		break;
		case CMD_GET_SN_CAN_ID: //0x80
		{
			/* 数据部分不等于0时，不应答消息，防止在使用该命令时，网络中存在冲突CanID，消息在网络中回环发送, 同时在使用该指令时，数据部分必须为0 */ 
			if(ptRecvFrame->uData.ulData != 0)
			{
				ucSendFlag = 1;
				return ucSendFlag;
			}
			
			/* 获取系列号及CAN ID */
			ptSendFrame->uData.ucData[0] = Recv_CanID();
			ptSendFrame->uData.ucData[1] = Send_CanID();
			
			ptSendFrame->uData.ucData[2] = g_tBoardStatus.usSN & 0xFF;
			ptSendFrame->uData.ucData[3] = (g_tBoardStatus.usSN>>8) & 0xFF;	
				
		}
		break;
		case CMD_SET_CAN_ID_WITH_SN: //0x81
		{
			/* 获取系列号及CAN ID */
			uint16_t usSN = (ptRecvFrame->uData.ucData[3]<<8) | ptRecvFrame->uData.ucData[2];
			
//			if(MSG_TYPE_CAN == eMsgType)
//			{
//				Can_Send_Msg(ptSendFrame);
//				ucSendFlag = 1;
//				HAL_Delay(3);
//			}

			//当识别码相等才去修改CanID
			if(g_tBoardStatus.usSN == usSN){			
				Can_Send_Msg(ptSendFrame);
				ucSendFlag = 1;
				rt_thread_mdelay(3);//HAL_Delay(3);
				ptSendFrame->ucStatus = GlobalParam_Set_CanID(ptRecvFrame->uData.ucData[0], ptRecvFrame->uData.ucData[1]);
			}else{
				//tSendFrame->ucStatus = ERROR_TYPE_DATA;
				ucSendFlag = 1;
				break;
			}
		}
		break;
		case CMD_SHAKE_WITH_SN: //抖动 0x82
		{
			uint8_t i = 0, ucFlag = 0;
			uint16_t usCount = 0;
			uint16_t usSN = (ptRecvFrame->uData.ucData[1]<<8) | ptRecvFrame->uData.ucData[0];
			uint16_t usShake = (ptRecvFrame->uData.ucData[3]<<8) | (ptRecvFrame->uData.ucData[2]);
						
			//判断SN是否一致
			if(usSN != g_tBoardStatus.usSN) 
			{
				//不一致，不执行，不应答
				//ptSendFrame->ucStatus = ERROR_TYPE_EXEC_RIGH;
				//Can_Send_Msg(ptSendFrame);
				ucSendFlag = 1;
				break;
			}else{
				//提前应答
				Can_Send_Msg(ptSendFrame);
				ucSendFlag = 1;
			}
			
			//默认抖动幅度，
			if(usShake == 0) usShake = g_tAxisParamDefault.usMicroStepResultion[eTMC]*3; //大概5度
			
			//抖动，3次
			for(i = 0; i < 3; i++)
			{
				//正向移动
				ptSendFrame->ucStatus = TMC_MoveBy(eTMC, usShake);
				while(usCount < 1500)
				{
					//最长等待2秒,检测是否到达位置
					ucFlag = TMC5160_FIELD_READ(eTMC, TMC5160_RAMPSTAT, TMC5160_RAMPSTAT_POS_REACH_MASK, TMC5160_RAMPSTAT_POS_REACH_SHIFT);
					
					//到达退出
					if(ucFlag == 1) break; 
					rt_thread_mdelay(50);//HAL_Delay(50);
					usCount += 50;
				}
				
				//延时
				rt_thread_mdelay(50);//HAL_Delay(50);				
				
				//回退
				ptSendFrame->ucStatus = TMC_MoveBy(eTMC, -usShake);
				while(usCount < 1500)
				{
					//最长等待2秒,检测是否到达位置
					ucFlag = TMC5160_FIELD_READ(eTMC, TMC5160_RAMPSTAT, TMC5160_RAMPSTAT_POS_REACH_MASK, TMC5160_RAMPSTAT_POS_REACH_SHIFT);
					
					//到达退出
					if(ucFlag == 1) break;
					rt_thread_mdelay(50);//HAL_Delay(50);
					usCount += 50;
				}				
			}
		}
		break;
		case CMD_SHINE_WITH_SN: //闪灯 0x83
		{
			uint16_t usSN = (ptRecvFrame->uData.ucData[1]<<8) | ptRecvFrame->uData.ucData[0];
						
			//判断SN是否一致
			if(usSN != g_tBoardStatus.usSN) 
			{
				//不一致，不执行，不应答
				//ptSendFrame->ucStatus = ERROR_TYPE_EXEC_RIGH;
				//Can_Send_Msg(ptSendFrame);
				ucSendFlag = 1;
				break;
			}
			//3次，间隔50ms
			LED_Shine(6, 50);
		}
		break;
		case CMD_TYPE_WITH_SN:  //0x84
		{
			uint16_t usSN = (ptRecvFrame->uData.ucData[1]<<8) | ptRecvFrame->uData.ucData[0];
			
			//判断SN是否一致
			if(usSN != g_tBoardStatus.usSN) 
			{
				//不一致，不执行，不应答
				//ptSendFrame->ucStatus = ERROR_TYPE_EXEC_RIGH;
				//Can_Send_Msg(ptSendFrame);
				ucSendFlag = 1;
				break;
			}
			
			//查询板卡类型
			ptSendFrame->uData.ulData = Get_Module_Type();
			//ptSendFrame->ucType = Recv_CanID();
		}
		break;
		case CMD_TEST: // 0xFE
		{
			if(0 == ptRecvFrame->ucType)
			{
				uint8_t ucAddr = ptRecvFrame->uData.lData;
			
				if(CheckRegister_Addr(ucAddr) != ERROR_TYPE_SUCCESS) return ERROR_TYPE_DATA;
				ptSendFrame->uData.lData = TMC5160_ReadInt(eTMC, ucAddr);
			}
		
//			LOG_Info("Start ...");
//			if(0 == ptRecvFrame->ucType)
//			{
//				//打印六点加速值
//				TMC5160_PrintSixPoint_V(eTMC);
//			}else if(1 == ptRecvFrame->ucType){
//				//打印所有寄存器值
//				Print_AllRegister_Value(eTMC);
//			}else if(2 == ptRecvFrame->ucType){

//				//打印CAN通信，收发统计信息
//				LOG_Debug("Recv: S=%d, E=%d, F=%d, O=%d", g_tBoardStatus.tCanMsgCount_Info.ulRecvSuccessNum, g_tBoardStatus.tCanMsgCount_Info.ulRecvErrorNum, \
//														  g_tBoardStatus.tCanMsgCount_Info.ulRecvFailNum, g_tBoardStatus.tCanMsgCount_Info.ulRecvOverNum);
//				LOG_Debug("Send: S=%d, F=%d", g_tBoardStatus.tCanMsgCount_Info.ulSendSuccessNum, g_tBoardStatus.tCanMsgCount_Info.ulSendFailNum);
//			}else if(3 == ptRecvFrame->ucType){
//				// TMC SPI通信测试
//				ErrorType_e eError = ERROR_TYPE_SUCCESS;
//				uint32_t i = 0, ulV = 0, ulNum = ptRecvFrame->uData.ulData;
//				
//				if(MSG_TYPE_CAN == eMsgType)
//				{
//					Can_Send_Msg(ptSendFrame);
//					ucSendFlag = 1;
//				}
//				//
//				eError = ERROR_TYPE_SUCCESS;
//				for(i = 0; i < ulNum; i++)
//				{
//					ulV = rand();
//					eError = TMC5160_WriteInt(eTMC, TMC5160_VSTOP, ulV);  
//					if(eError != ERROR_TYPE_SUCCESS)
//					{
//						ptSendFrame->ucStatus = eError;
//						if(MSG_TYPE_CAN == eMsgType)
//						{
//							Can_Send_Msg(ptSendFrame);
//						}
//						HAL_Delay(3);
//					}
//				}
//			}
		}
		break;
		default:
		{
			ptSendFrame->ucStatus = ERROR_TYPE_CMD;
		}
		break;
	}
	
	return ucSendFlag;
}





/*
 *
*   Event Process
*/
void Event_Process(void)
{
	uint8_t      reply = CAN_TxStatus_Ok;
    SysEvent_t   *e;
	
	
	
	//从报文队列中查询报文
    switch(MonCan.Motor.tSysEvent.eMsgType)
    {
        case MSG_TYPE_NULL:
        {
//			__disable_irq();
			e = SysEventGet();
			if(e)
			{
				
				memmove((void*)&MonCan.Motor.tSysEvent, (void*)e, sizeof(SysEvent_t));
				SysEventFree(e);
			}
			else
			{
				;
			}
//			__enable_irq();
		}
		break;
		
        default:
		{
		}
		break;
    }
	
	
    switch(MonCan.Motor.tSysEvent.eMsgType)
    {
        case MSG_TYPE_NULL:
        {
		}
		break;
		
        case MSG_TYPE_CAN:
        {
//            Can_RxMsg_t tRxMsg = {0};
//			memmove((void*)&tRxMsg, (void*)tSysEvent.tMsg.ucaDataBuf, sizeof(Can_RxMsg_t));
//            Handle_Can_RxMsg(&tRxMsg);
			Can_RxMsg_t *ptRxMsg = (Can_RxMsg_t*)MonCan.Motor.tSysEvent.tMsg.ucaDataBuf;
			reply = Handle_Can_RxMsg(ptRxMsg);
			
			
			//判断是否需要重发
			if(CAN_TxStatus_NoMailBox == reply)
			{
				MonCan.Motor.IsReSend = true;
				//发送邮箱满
				//CAN_Config(CAN1, &MonCan.Confg);
//				CAN_Config(CAN1);
			}
			else
			{
				MonCan.Motor.tSysEvent.eMsgType = MSG_TYPE_NULL;
				MonCan.Motor.IsReSend = false;
			}
        }
        break;
		
//        case MSG_TYPE_USART:
//        {
////			MsgUsart_t tRxMsg = {0};
////			memmove((void*)&tRxMsg, (void*)&tSysEvent.tMsg.tMsgUsart, sizeof(MsgUsart_t));
////            Handle_Usart_RxMsg(&tRxMsg);
//			
////			MsgUsart_t *ptRxMsg = (MsgUsart_t*)tSysEvent.tMsg.ucaDataBuf;
////			Handle_Usart_RxMsg(ptRxMsg);
//        }
//        break;
		
        default:
		{
			MonCan.Motor.tSysEvent.eMsgType = MSG_TYPE_NULL;
			MonCan.Motor.IsReSend = false;
		}
		break;
    }
}








//LED 闪耀
void LED_Shine(uint16_t usCount, uint32_t ulTime)
{
	for(uint16_t i = 0; i < usCount; i++)
	{
		//指示灯闪烁
		SYS_LED_TRIGGER;
		rt_thread_mdelay(ulTime);//HAL_Delay(ulTime);
	}
}


/************************************************/
//调试功能
#if MGIM_DEBUG


#endif













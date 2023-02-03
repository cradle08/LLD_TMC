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


//TMC״̬
__IO TMCStatus_t g_tTMCStatus = {0};



/*
*	ȫ��ֵ��ʼ��
*/
void Global_Status_Init(void)
{
	extern __IO BoardStatus_t g_tBoardStatus;
	
	TMC_e eTMC = TMC_0;
	memset((void*)&g_tBoardStatus, 0, sizeof(BoardStatus_t));
	
	for(eTMC = TMC_0; eTMC < TMC_MODULE_END; eTMC++)
	{
		g_tTMCStatus.ulBoardStatus 	 = 0;
		g_tTMCStatus.tMotorResetInfo[eTMC].eResetStatus = MOTOR_RESET_STATUS_NONE; //�����λ״̬�� 0��δ��λ
	}
}









/*
*	MCU ����
*/
void MCU_Reset(void)
{
	//MCU ��λ, ��Ƭ����λ��TMC״̬���Ĵ�����ֵ����Ϣ״�壩
	__set_PRIMASK(1);
	NVIC_SystemReset();
}






///*
//*	���EEPROM����������
//*/
//void ClearAndSave_Default_Axis_Params(void)
//{
//	Axis_Param_Fixed_SetDefault_Value(&g_tAxisParamDefault);
//	//Param_Write(EN_SAVE_PARAM_TYPE_AXIS, (uint8_t*)&g_tAxisParamDefault, sizeof(AxisParamDefault_t));
//	Save_Axis_Param_Default(&g_tAxisParamDefault);
//}



/*
*	������б�����EEPROM�Ĳ���, ������ΪĬ��ֵ
*/
void Reset_Factory(void)
{
	ClearAndSave_Default_Global_Params();
	ClearAndSave_Default_Axis_Params();
	ClearAndSave_Default_Process();
	
	//Һ��̽�����
	ClearAndSave_Default_LLDParams();
}





/*
 *  ��ȡ��Ӳ���汾��
 */
void Get_Soft_HardWare_Version(uint8_t *pucaData)
{
	extern uint16_t g_usSoftVersion;		//����汾��
	extern uint16_t g_usHardWareVesion;	//Ӳ���汾��

	//����汾��
	pucaData[0] = g_usSoftVersion & 0xFF;
	pucaData[1] = (g_usSoftVersion >> 8) & 0xFF;
	
	//Ӳ���汾��
	pucaData[2] = g_usHardWareVesion & 0xFF;
	pucaData[3] = (g_usHardWareVesion >> 8) & 0xFF;

}



/*
 *  ��ȡģ������
 */
uint32_t Get_Module_Type(void)
{

	return CURRENT_MODULE_TYPE;

}




/*
* ״̬���, 1:������쳣�� 0��������쳣
*/
void Period_Error_Check(uint32_t ulTick)
{
	extern __IO TMCStatus_t g_tTMCStatus;
	extern __IO BoardStatus_t g_tBoardStatus;
	
	static uint32_t s_ulTick = 0, s_ulTick2 = 0;
	TMC_e eTMC = TMC_0;
	int32_t lStatus = 0;
	uint8_t ucFlag = 0;
	
	//ÿ��200ms
	if(ulTick - s_ulTick >= 200)
	{
		s_ulTick = ulTick;
		//ÿ��10ms�����һ��״̬
		
		/* ��⡢��¼TMC״̬��Ϣ */
		for(eTMC = TMC_0; eTMC < TMC_MODULE_END; eTMC++)
		{
			lStatus = TMC_ReadInt(eTMC, TMC5160_DRVSTATUS);
			
			/* A�࿪· */
			if((lStatus >> 30) == 1)
			{
				//����λ
				BIT_SET(g_tTMCStatus.ucErrStatus[eTMC], 0);
			}else{
				//���λ
				BIT_RESET(g_tTMCStatus.ucErrStatus[eTMC], 0);
			}
			

			/* B�࿪· */
			if((lStatus >> 29) == 1)
			{
				//����λ
				BIT_SET(g_tTMCStatus.ucErrStatus[eTMC], 1);
			}else{
				//���λ
				BIT_RESET(g_tTMCStatus.ucErrStatus[eTMC], 1);
			}
			
			
			/* A���· */
			if((lStatus >> 28) == 1)
			{
				//����λ
				BIT_SET(g_tTMCStatus.ucErrStatus[eTMC], 2);
			}else{
				//���λ
				BIT_RESET(g_tTMCStatus.ucErrStatus[eTMC], 2);
			}
			
						
			/* B���· */
			if((lStatus >> 27) == 1)
			{
				//����λ
				BIT_SET(g_tTMCStatus.ucErrStatus[eTMC], 3);
			}else{
				//���λ
				BIT_RESET(g_tTMCStatus.ucErrStatus[eTMC], 3);
			}
			
			
			/* ���� */
//			if((lStatus >> 26) == 1)
//			{
//				//����λ
//				BIT_SET(g_tBoardStatus.ucTMCStatus[eTMC], 5);
//			}else{
//				//���λ
//				BIT_RESET(g_tBoardStatus.ucTMCStatus[eTMC], 5);
//			}
			
			
			/* ��ѹ���  */
			
			
			/* ��ת���  */			
		}
		
		
		/* ���TMC״̬��־��ֻҪ��һ���������ͻ��� */
		/* B�࿪· */
		for(eTMC = TMC_0; eTMC < TMC_MODULE_END; eTMC++)
		{
			//A�࿪·
			if((g_tTMCStatus.ucErrStatus[eTMC] & 0x01) == 1) ucFlag = 1;
		}
		if(ucFlag == 1){			
				BIT_SET(g_tTMCStatus.ulBoardStatus, 0);
		}else{ 
				BIT_RESET(g_tTMCStatus.ulBoardStatus, 0);
		}
		
		/* B�࿪· */
		for(eTMC = TMC_0; eTMC < TMC_MODULE_END; eTMC++)
		{
			//B�࿪·
			if((g_tTMCStatus.ucErrStatus[eTMC] & 0x02) == 1) ucFlag = 1;
		}
		if(ucFlag == 1)		
		{			
				BIT_SET(g_tTMCStatus.ulBoardStatus, 1);
		}else{ 
				BIT_RESET(g_tTMCStatus.ulBoardStatus, 1);
		}


		/* A���· */
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
		
		
		/* B���· */
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
		
		/* ��ʼ��EEPROM ��д�쳣 */
		if(1 == g_tBoardStatus.ucEEPRAM_Init_CRC_ErrFlag)
		{
			BIT_SET(g_tTMCStatus.ulBoardStatus, 9);
		}else{
			BIT_RESET(g_tTMCStatus.ulBoardStatus, 9);
		}
		
		
		/* ִ�������쳣 */
		if(1 == g_tTMCStatus.ucExecProcessStatus)
		{
			BIT_SET(g_tTMCStatus.ulBoardStatus, 9);
		}else{
			BIT_RESET(g_tTMCStatus.ulBoardStatus, 9);
		}	
	}
	
	
	//ÿ��10ms�����һ��
	if(ulTick - s_ulTick2 >= 10)
	{
		s_ulTick2 = ulTick;
		
		/* ʧ�����  */
		for(eTMC = TMC_0; eTMC < TMC_MODULE_END; eTMC++)
		{
			/* ʧ�����  */
			lStatus = TMC_ReadInt(eTMC, TMC5160_ENC_STATUS);
			if(lStatus & 0x02)
			{
				//����λ��ʧ���������Զ����
				BIT_SET(g_tTMCStatus.ucErrStatus[eTMC], 4);
				//LOG_Error("Step Error");
			
			}else{
				//���λ
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
		
		
		/* CANͨ��Ӧ���쳣 */
		if(g_tBoardStatus.tCanMsgCount_Info.ulSendFailNum >= 1)
		{
			BIT_SET(g_tTMCStatus.ulBoardStatus, 11);
		}
	}
}



/*
* ģ���쳣״̬����
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
*	��������	
*/
ErrorType_e MissStep_Handle(TMC_e eTMC)
{	
	ErrorType_e eError = ERROR_TYPE_SUCCESS;
	if(eTMC >= TMC_MODULE_END) return ERROR_TYPE_DEVICE_ID;
	   	
	//�账��������XENC��XVACTUALС��ʧ����ֵ����д1������ñ�־λ����رն�����⹦�ܣ���ִ���˶����������λ��
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

	//���������־λ
	TMC5160_FIELD_UPDATE(eTMC, TMC5160_ENC_STATUS, TMC5160_DEVIATION_WARN_MASK, TMC5160_DEVIATION_WARN_SHIFT, 1);

	return eError;
}






/*
* ���������Ϣ, ����ֵ��0��û�з���Ӧ����Ϣ��1���ѷ���Ӧ����Ϣ
* 
*/
uint8_t Handle_RxMsg(MsgType_e eMsgType, RecvFrame_t *ptRecvFrame, SendFrame_t *ptSendFrame)
{
//	extern CAN_HandleTypeDef hcan;
	extern Process_t g_tProcess;
	extern TMC5160_t g_taTMC5160[TMC_MODULE_END];
	extern __IO BoardStatus_t g_tBoardStatus;
	extern AxisParamDefault_t g_tAxisParamDefault;
	
	uint8_t ucSendFlag = 0; //Ӧ����Ϣ��Ӧ���־��0����Ӧ��1����Ӧ��
	ErrorType_e eError = ERROR_TYPE_SUCCESS;
	TMC_e eTMC = TMC_0;
	
	//
	eTMC = (TMC_e)ptRecvFrame->ucDeviceID;
	//�������
	if(eTMC >= TMC_MODULE_END)
	{
		//LOG_Error("TMC DeviceID=%d Is Err", eTMC);
		ptSendFrame->ucStatus = ERROR_TYPE_DEVICE_ID;
		return ucSendFlag;
	}
	
	//ָ���	
	switch(ptRecvFrame->ucCmd)
	{
		/**********************/
		case CMD_ROTATE:  //0x10
		{
			if(g_tTMCStatus.tMotorResetInfo[eTMC].eResetStatus == MOTOR_RESET_STATUS_ING)
			{
				ptSendFrame->ucStatus = ERROR_TYPE_EXEC_RIGH; //����ڸ�λ�У���ִ����תָ��
				return ucSendFlag;
			}
			
			TMC_WriteInt(eTMC, TMC5160_ENC_DEVIATION, 0); //�رձ�����ʧ����⹦��
			
			//��ת
			ptSendFrame->ucStatus = TMC_Rotate(eTMC, ptRecvFrame->ucType, ptRecvFrame->uData.ulData);
			TMC_SetVMode_V(eTMC, 0);
			
			
		}
		break;
		case CMD_MOVE_POSITION_WITHOUT_ENC:  //0x11
		{
			if(g_tTMCStatus.tMotorResetInfo[eTMC].eResetStatus == MOTOR_RESET_STATUS_ING)
			{
				ptSendFrame->ucStatus = ERROR_TYPE_EXEC_RIGH; //����ڸ�λ�У���ִ���ƶ�ָ��
				return ucSendFlag;
			}	
				
			
			//�رձ�����ʧ����⹦��
			TMC_WriteInt(eTMC, TMC5160_ENC_DEVIATION, 0);
			
			//�ƶ�
			if(0 == ptRecvFrame->ucType)
			{
				//����ƫ��
				ptSendFrame->ucStatus = TMC_MoveTo(eTMC, ptRecvFrame->uData.lData);
			}else{
				//���ƫ��
				ptSendFrame->ucStatus = TMC_MoveBy(eTMC, ptRecvFrame->uData.lData);
			}
			TMC_SetPMode_V(eTMC, 0);
			
		}
		break;
		case CMD_MOVE_POSITION_WITH_ENC: //0x12
		{
			if(g_tTMCStatus.tMotorResetInfo[eTMC].eResetStatus == MOTOR_RESET_STATUS_ING)
			{
				ptSendFrame->ucStatus = ERROR_TYPE_EXEC_RIGH; //����ڸ�λ�У���ִ���ƶ�ָ��
				return ucSendFlag;
			}	
			
			//������ʧ����ֵ, ��ֵΪ�㣬��رոù���			
			ptSendFrame->ucStatus = TMC_WriteInt(eTMC, TMC5160_ENC_DEVIATION, g_tAxisParamDefault.lEncDiff_Threshold[eTMC]);
//			LOG_Info("ENC Deviation Start: Motor=%d, EncDiff=%d, Steps=%d, CurStep=%d", \
					eTMC, g_tAxisParamDefault.lEncDiff_Threshold[eTMC], ptRecvFrame->uData.lData, TMC5160_ReadInt(eTMC, TMC5160_XACTUAL));
			
			//�ƶ�
			if(0 == ptRecvFrame->ucType)
			{
				//����ƫ��
				ptSendFrame->ucStatus = TMC_MoveTo(eTMC, ptRecvFrame->uData.lData);
			}else{
				//���ƫ��
				ptSendFrame->ucStatus = TMC_MoveBy(eTMC, ptRecvFrame->uData.lData);
			}
			TMC_SetPMode_V(eTMC, 0);
		}
		break;
		case CMD_STOP:   //0x13
		{
			//ֹͣ
			//LOG_Info("Before STOP XTarget=%d", TMC_ReadInt(eTMC, TMC5160_XACTUAL));
			ptSendFrame->ucStatus = TMC_Stop(eTMC);
			
			//��λ״̬�£�ֹͣ
			if(g_tTMCStatus.ucMotorResetStartFlag != 0)
			{
				//
				g_tTMCStatus.tMotorResetInfo[eTMC].eResetStatus = MOTOR_RESET_STATUS_FAIL;
				
				//�ر����ο��㸴λ
				TMC5160_FIELD_UPDATE(eTMC, TMC5160_SWMODE, TMC5160_STATUS_STOP_L_MASK, TMC5160_STATUS_STOP_L_SHIFT, 0);
				
				//����ģʽ--λ��ģʽ
				TMC5160_WriteInt(eTMC, TMC5160_RAMPMODE, TMC_MODE_POSITION);
				
				//�ָ��ٶ�����
				TMC_SetPMode_V(eTMC, 2);
			}
			
			//LOG_Info("End STOP XTarget=%d", TMC_ReadInt(eTMC, TMC5160_XACTUAL));
		}
		break;
		case CMD_MOTOR_RESET: //0x14
		{
			/* ��λ����ԭ�㣨�ο�λ�ã�*/
			
			//��λδ���
			if(g_tTMCStatus.tMotorResetInfo[eTMC].eResetStatus != MOTOR_RESET_STATUS_ING)
			{
				//�޸ĸ�λ������Ϣ��������λ����
				g_tTMCStatus.tMotorResetInfo[eTMC].eResetStatus = MOTOR_RESET_STATUS_ING;
				g_tTMCStatus.tMotorResetInfo[eTMC].eResetExec   = MOTOR_RESET_EXEC_1;
				g_tTMCStatus.ucMotorResetStartFlag  = 1; //ִ�и�λ
			}
//			g_tBoardStatus.eMotorResetStatus[eTMC] = 0;
//			TMC_Reset(eTMC, TMC_REF_LEFT);  //TMC_REF_RIGHT   TMC_REF_LEFT
//			//��λ�����
//			g_tBoardStatus.ucMotor_ResetStatus[eTMC] = 1;
		}
		break;
		
		
		/**********************/
		case CMD_MCU_REST:  //0x20
		{
			/* ����mcu */
			
			//����Ӧ��
			if(MSG_TYPE_CAN == eMsgType)
			{
				Can_Send_Msg(ptSendFrame);
				ucSendFlag = 1;
			}
			rt_thread_delay(3);//HAL_Delay(3);
			
			//����
			MCU_Reset();
		}
		break;		
		case CMD_QUERY_BOARD_TYPE:  //0x21
		{
			//��ѯ�忨����
			ptSendFrame->uData.ulData = Get_Module_Type();
			//ptSendFrame->ucType = Recv_CanID();
			
		}
		break;		
		case CMD_HARD_SOFT_VERSION:  //0x22
		{
			//��ѯ��Ӳ���汾
			Get_Soft_HardWare_Version(ptSendFrame->uData.ucData);
		}
		break;
		
		
		/**********************/
		case CMD_SET_AXIS_PARAM:  //0x30
		{
			//���������
			ptSendFrame->ucStatus = TMC_AxisParam(eTMC, TMC_WRITE, ptRecvFrame->ucType, &ptRecvFrame->uData);
//			LOG_Info("Set Axis Parma,T=%d, V=%d", ptRecvFrame->ucType, ptRecvFrame->uData.lData)
		}
		break;
		case CMD_GET_AXIS_PARAM:  //0x31
		{
			//��ѯ�����
			ptSendFrame->ucStatus = TMC_AxisParam(eTMC, TMC_READ, ptRecvFrame->ucType, &ptSendFrame->uData);
//			LOG_Info("Get Axis Parma,T=%d, V=%d", ptRecvFrame->ucType, ptSendFrame->uData.lData)
		}
		break;		
		case CMD_SET_DEFAULT_AXIS_PARAM: //0x3A
		{
			//����Ĭ�������
			ptSendFrame->ucStatus = TMC_AxisParam_Default(eTMC, TMC_WRITE, ptRecvFrame->ucType, &ptRecvFrame->uData);		
		
		}
		break;
		case CMD_GET_DEFAULT_AXIS_PARAM: //0x3B
		{
			//��ѯĬ�������
			ptSendFrame->ucStatus = TMC_AxisParam_Default(eTMC, TMC_READ, ptRecvFrame->ucType, &ptSendFrame->uData);		
		}
		break;		
		
		case CMD_SET_GLOBAL_PARAM:  //0x33
		{
			/* ����ȫ�ֲ��� */
			//Ӧ��
			if(MSG_TYPE_CAN == eMsgType)
			{
				Can_Send_Msg(ptSendFrame);
				ucSendFlag = 1;
			}
			
			//����ȫ�ֲ���
			Bank_e eBank = (Bank_e)ptRecvFrame->ucDeviceID;
			ptSendFrame->ucStatus = TMC_Global_Param(eBank, TMC_WRITE, ptRecvFrame->ucType, &ptRecvFrame->uData);
//			LOG_Info("Set Module Parma,T=%d, V=%d", ptRecvFrame->ucType, lValue)
		}
		break;
		case CMD_GET_GLOBAL_PARAM:  //0x34
		{
			//��ѯȫ�ֲ���
			Bank_e eBank = (Bank_e)ptRecvFrame->ucDeviceID;
			ptSendFrame->ucStatus = TMC_Global_Param(eBank, TMC_READ, ptRecvFrame->ucType, &ptSendFrame->uData);
//			LOG_Info("Get Module Parma,T=%d, V=%d", ptRecvFrame->ucType, ptSendFrame->uData.lData)
		}
		break;			
		case CMD_SET_IO_STATUS:  //0x35
		{
			//����IO״̬
//@todo			eError = Set_Out_IO(ptRecvFrame->ucType, ptRecvFrame->uData.ulData);
//@todo			ptSendFrame->ucStatus = (uint8_t)eError;
			ptSendFrame->ucStatus = ERROR_TYPE_DEVICE_ID;
		}
		break;
		case CMD_GET_INPUT_IO_STATUS:  //0x36
		{
			//��ѯIO״̬
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
			//����ִ������
			ptSendFrame->ucStatus = Set_Process(ptRecvFrame);
		}
		break;	
		case CMD_GET_EXEC_PROCESS:  //0x41
		{
			//��ȡִ������
			SubProcess_t tSubProcess = {0};
			ucSendFlag = 1;
			
			for(uint8_t ucIndex = 0; ucIndex < SUB_PROCESS_MAX_CMD_NUM; ucIndex++)
			{
				memset((void*)&tSubProcess, 0, sizeof(SubProcess_t));
				Get_Process(ucIndex, &tSubProcess);
				
				if(tSubProcess.ucParamNum >= SUB_PROCESS_MAX_PARAM_NUM)
				{
					//�����쳣
					LOG_Error("Param Num Error=%d", tSubProcess.ucParamNum);
					ptSendFrame->ucStatus = ERROR_TYPE_CRC;
					if(MSG_TYPE_CAN == eMsgType)
					{
						Can_Send_Msg(ptSendFrame);
					}
				}
				
				/* send set "cmd"*/
				ptSendFrame->ucStatus = ERROR_TYPE_SUCCESS;
				ptSendFrame->uData.ucData[0] = tSubProcess.ucCmd;
				if(MSG_TYPE_CAN == eMsgType)
				{
					Can_Send_Msg(ptSendFrame);
				}
		
				/* send set param */
				for(uint8_t i = 0; i < tSubProcess.ucParamNum; i++)
				{
					memset((void*)&ptSendFrame->uData.ulData, 0, sizeof(Data4Byte_u));
					ptSendFrame->uData.lData = tSubProcess.uParam[i].lData;
					
					if(MSG_TYPE_CAN == eMsgType)
					{
						Can_Send_Msg(ptSendFrame);
					}
					rt_thread_delay(2);//HAL_Delay(2);
				}		
			}	
		}
		break;
		case CMD_EXEC_PROCESS_CTRL:  //0x42
		{
			//ִ������--ִ�к�ֹͣ
			Exec_Process_Ctrl(ptRecvFrame->ucType);
		}
		break;
		case CMD_CLS_SAVE_EXEC_PROCESS:  //0x43
		{
			//ִ������--����ͱ���
			Exec_Process_Clear_Or_Save(ptRecvFrame->ucType);
			//LOG_Info("Save Prcess");
		}
		break;

		
		
		/********************/
		case CMD_QUERY_STATUS:  //0x50
		{
			//��ѯ״̬
			ptSendFrame->uData.ulData = g_tTMCStatus.ulBoardStatus;
		}
		break;
//		case CMD_ERROR_HANDLE: //0x51
//		{
//			//�쳣״̬����
//			ptSendFrame->ucStatus = Module_Error_Handle(eTMC,  ptRecvFrame->ucType);
//		}
//		break;
		
		/********************/
		case CMD_CLEAR_EEPROM_PARAM: //0x60
		{
			//Ӧ��
			if(MSG_TYPE_CAN == eMsgType)
			{
				Can_Send_Msg(ptSendFrame);
				ucSendFlag = 1;
			}
			
			//����
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
			
			rt_thread_delay(3);//HAL_Delay(3);
			MCU_Reset();
		}
		break;
		case CMD_UPGRADE_LANCH: //0x70
		{
			/* ������������ת��Boot */
			//�޸�������־λ��
			ptSendFrame->ucStatus = Set_UpdateFlag(1); //@todo, ��Ϊ��ת��boot�ķ�ʽ
			if(MSG_TYPE_CAN == eMsgType)
			{
				Can_Send_Msg(ptSendFrame);
				ucSendFlag = 1;
			}
			
			//����
			rt_thread_delay(3);//HAL_Delay(3);
			MCU_Reset();
		}
		break;
		case CMD_QUERY_RUNING_SOFT_TYPE: //0x75
		{
			/* ��ѯ��ǰ����ִ�еĳ������� */
			ptSendFrame->uData.ucData[0] = SOFT_TYPE_APP;
		}
		break;
		case CMD_GET_SN_CAN_ID: //0x80
		{
			/* ���ݲ��ֲ�����0ʱ����Ӧ����Ϣ����ֹ��ʹ�ø�����ʱ�������д��ڳ�ͻCanID����Ϣ�������лػ�����, ͬʱ��ʹ�ø�ָ��ʱ�����ݲ��ֱ���Ϊ0 */ 
			if(ptRecvFrame->uData.ulData != 0)
			{
				ucSendFlag = 1;
				return ucSendFlag;
			}
			
			/* ��ȡϵ�кż�CAN ID */
			ptSendFrame->uData.ucData[0] = Recv_CanID();
			ptSendFrame->uData.ucData[1] = Send_CanID();
			
			ptSendFrame->uData.ucData[2] = g_tBoardStatus.usSN & 0xFF;
			ptSendFrame->uData.ucData[3] = (g_tBoardStatus.usSN>>8) & 0xFF;	
				
		}
		break;
		case CMD_SET_CAN_ID_WITH_SN: //0x81
		{
			/* ��ȡϵ�кż�CAN ID */
			uint16_t usSN = (ptRecvFrame->uData.ucData[3]<<8) | ptRecvFrame->uData.ucData[2];
			
			if(MSG_TYPE_CAN == eMsgType)
			{
				Can_Send_Msg(ptSendFrame);
				ucSendFlag = 1;
				rt_thread_delay(2);//HAL_Delay(3);
			}

			//��ʶ������Ȳ�ȥ�޸�CanID
			if(g_tBoardStatus.usSN == usSN)
			{
				ptSendFrame->ucStatus = GlobalParam_Set_CanID(ptRecvFrame->uData.ucData[0], ptRecvFrame->uData.ucData[1]);
			}else{
				ptSendFrame->ucStatus = ERROR_TYPE_DATA;
			}
			
//			HAL_Delay(100);
//			//

			
//			/* ϵ�кż�CAN ID */
//			ptSendFrame->uData.ucData[0] = Recv_CanID();
//			ptSendFrame->uData.ucData[1] = Send_CanID();
//			
//			ptSendFrame->uData.ucData[2] = g_tBoardStatus.usSN & 0xFF;
//			ptSendFrame->uData.ucData[3] = (g_tBoardStatus.usSN>>8) & 0xFF;	
							
//			//����
//			Can_Send_Msg(ptSendFrame);
//			//�����Ӧ��
//			ucSendFlag = 1;
		}
		break;
		case CMD_SHAKE_WITH_SN: //0x82
		{
			uint8_t i = 0, ucFlag = 0;
			uint16_t usTick = 0;
			uint16_t usSN = (ptRecvFrame->uData.ucData[1]<<8) | ptRecvFrame->uData.ucData[0];
			uint16_t usShake = (ptRecvFrame->uData.ucData[3]<<8) | (ptRecvFrame->uData.ucData[2]);
						
			//�ж�SN�Ƿ�һ��
			if(usSN != g_tBoardStatus.usSN) 
			{
				//��һ�£���ִ�У�Ӧ��
				ptSendFrame->ucStatus = ERROR_TYPE_EXEC_RIGH;
				Can_Send_Msg(ptSendFrame);
				ucSendFlag = 1;
				break;
			}else{
				Can_Send_Msg(ptSendFrame);
				ucSendFlag = 1;
			}
			
			//Ĭ�϶�������
			if(usShake == 0) usShake = 500;
			
			//������3��
			for(i = 0; i < 3; i++)
			{
				//�����ƶ�
				ptSendFrame->ucStatus = TMC_MoveBy(eTMC, usShake);
				while(usTick < 200)
				{
					//��ȴ�2��,����Ƿ񵽴�λ��
					ucFlag = TMC5160_FIELD_READ(eTMC, TMC5160_RAMPSTAT, TMC5160_RAMPSTAT_POS_REACH_MASK, TMC5160_RAMPSTAT_POS_REACH_SHIFT);
					
					//�����˳�
					if(ucFlag == 1) break; 
					rt_thread_delay(10);//HAL_Delay(10);
				}
				
				//��ʱ
				rt_thread_delay(50);//HAL_Delay(50);				
				
				//����
				ptSendFrame->ucStatus = TMC_MoveBy(eTMC, -usShake);
				while(usTick < 200)
				{
					//��ȴ�2��,����Ƿ񵽴�λ��
					ucFlag = TMC5160_FIELD_READ(eTMC, TMC5160_RAMPSTAT, TMC5160_RAMPSTAT_POS_REACH_MASK, TMC5160_RAMPSTAT_POS_REACH_SHIFT);
					
					//�����˳�
					if(ucFlag == 1) break;
					rt_thread_delay(10);//HAL_Delay(10);
				}				
			}
		}
		break;
		case CMD_SHINE_WITH_SN: //0x83
		{
			uint16_t usSN = (ptRecvFrame->uData.ucData[1]<<8) | ptRecvFrame->uData.ucData[0];
						
			//�ж�SN�Ƿ�һ��
			if(usSN != g_tBoardStatus.usSN) 
			{
				//��һ�£���ִ�У�Ӧ��
				ptSendFrame->ucStatus = ERROR_TYPE_EXEC_RIGH;
				Can_Send_Msg(ptSendFrame);
				ucSendFlag = 1;
				break;
			}
			//3�Σ����50ms
			LED_Shine(6, 50);
		}
		break;
		case 0xF0://Һ��̽����� д
		{
			int32_t lValue = ptRecvFrame->uData.lData;
			ptSendFrame->ucStatus = LLD_Param(TMC_WRITE, ptRecvFrame->ucType, &lValue);
		}
		break;
		case 0xF1://Һ��̽����� ��
		{
			int32_t lValue = 0;
			ptSendFrame->ucStatus = LLD_Param(TMC_READ, ptRecvFrame->ucType, &lValue);
			ptSendFrame->uData.lData = lValue;
		}
		break;
		case CMD_TEST: // 0xFE
		{		
			
//			LOG_Info("Start ...");
			if(0 == ptRecvFrame->ucType)
			{
				//��ӡ�������ֵ
				TMC5160_PrintSixPoint_V(eTMC);
			}else if(1 == ptRecvFrame->ucType){
				//��ӡ���мĴ���ֵ
				Print_AllRegister_Value(eTMC);
			}else if(2 == ptRecvFrame->ucType){

				//��ӡCANͨ�ţ��շ�ͳ����Ϣ
//				LOG_Debug("Recv: S=%d, E=%d, F=%d, O=%d", g_tBoardStatus.tCanMsgCount_Info.ulRecvSuccessNum, g_tBoardStatus.tCanMsgCount_Info.ulRecvErrorNum, \
														  g_tBoardStatus.tCanMsgCount_Info.ulRecvFailNum, g_tBoardStatus.tCanMsgCount_Info.ulRecvOverNum);
//				LOG_Debug("Send: S=%d, F=%d", g_tBoardStatus.tCanMsgCount_Info.ulSendSuccessNum, g_tBoardStatus.tCanMsgCount_Info.ulSendFailNum);
			}else if(3 == ptRecvFrame->ucType){
				// TMC SPIͨ�Ų���
				ErrorType_e eError = ERROR_TYPE_SUCCESS;
				uint32_t i = 0, ulV = 0, ulNum = ptRecvFrame->uData.ulData;
				
				if(MSG_TYPE_CAN == eMsgType)
				{
					Can_Send_Msg(ptSendFrame);
					ucSendFlag = 1;
				}
				//
				eError = ERROR_TYPE_SUCCESS;
				for(i = 0; i < ulNum; i++)
				{
					ulV = rand();
					eError = TMC5160_WriteInt(eTMC, TMC5160_VSTOP, ulV);  
					if(eError != ERROR_TYPE_SUCCESS)
					{
						ptSendFrame->ucStatus = eError;
						if(MSG_TYPE_CAN == eMsgType)
						{
							Can_Send_Msg(ptSendFrame);
						}
						rt_thread_delay(3);//HAL_Delay(3);
					}
				}
			}
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
//            Can_RxMsg_t tRxMsg = {0};
//			memmove((void*)&tRxMsg, (void*)tSysEvent.tMsg.ucaDataBuf, sizeof(Can_RxMsg_t));
//            Handle_Can_RxMsg(&tRxMsg);
			Can_RxMsg_t *ptRxMsg = (Can_RxMsg_t*)tSysEvent.tMsg.ucaDataBuf;
			Handle_Can_RxMsg(ptRxMsg);
        }
        break;
        case MSG_TYPE_USART:
        {
//			MsgUsart_t tRxMsg = {0};
//			memmove((void*)&tRxMsg, (void*)&tSysEvent.tMsg.tMsgUsart, sizeof(MsgUsart_t));
//            Handle_Usart_RxMsg(&tRxMsg);
			
//			MsgUsart_t *ptRxMsg = (MsgUsart_t*)tSysEvent.tMsg.ucaDataBuf;
//			Handle_Usart_RxMsg(ptRxMsg);
        }
        break;
        default:break;
    }

}








//LED ��ҫ
void LED_Shine(uint16_t usCount, uint32_t ulTime)
{
	for(uint16_t i = 0; i < usCount; i++)
	{
		//ָʾ����˸
//@todo		SYS_LED_TRIGGER;
		rt_thread_delay(ulTime);//HAL_Delay(ulTime);
	}
}


/************************************************/
//���Թ���
#if MGIM_DEBUG


#endif













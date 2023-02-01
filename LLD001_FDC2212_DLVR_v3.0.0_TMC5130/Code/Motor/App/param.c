#include "param.h"
#include "main.h"
#include "log.h"
#include "crc.h"
#include "eeprom.h"
#include "bsp_can.h"
//#include "TMC_Process.h"

//ģ�������
__IO GlobalParam_t 	 g_tGlobalParam = {0};



/*
*	ȫ�ֲ�����ʼ��
*/
ErrorType_e Global_Param_Init(void)
{
	return Read_Global_Param(&g_tGlobalParam);
	
}



/*
*	ģ����� ���
*/
void Global_Param_SetDefault_Value(__IO GlobalParam_t *ptGlobalParam)
{
	//��һ���ϵ�, ��ʼ��������
	memset((void*)ptGlobalParam, 0, sizeof(GlobalParam_t));
	
	/* Bank 0 */
	ptGlobalParam->ulInitFlag  	  = PARAM_INIT_FLAG;
	ptGlobalParam->eCanBaud    	  = EN_CAN_BAUD_1000; 	   //������
	ptGlobalParam->ulRecvCanID    = CAN_DEFAULT_RECV_ID;   //Ĭ�Ͻ���CanID
	ptGlobalParam->ulSendCanID    = CAN_DEFAULT_SEND_ID;   //Ĭ�Ϸ���CanID
	ptGlobalParam->ucProcessAutoExecMode = 0; //�Զ������̣��ϵ��Զ�ִ�б�־, 0:�·�ָ���ִ�У�1���ϵ��Զ�ִ��
	
	/* Bank 1 ���� */
	
	/* Bank 2  �û�����2*/
	
	/* Bank 3 �ж� */
	
	/* Bank 2  �û�����1*/
	
	//crc
	ptGlobalParam->usCrc       	  = CRC16((uint8_t*)ptGlobalParam, GLOBAL_PARAM_SAVE_TO_EEPROM_LEN-2);
}




/*
*	���EEPROM�����ģ�����
*/
void ClearAndSave_Default_Global_Params(void)
{
	Global_Param_SetDefault_Value(&g_tGlobalParam);
	//Param_Write(EN_SAVE_PARAM_TYPE_GLOBAL, (uint8_t*)&g_tGlobalParam, GLOBAL_PARAM_SAVE_TO_EEPROM_LEN);
	Save_Global_Param(&g_tGlobalParam);
	
}






/*
*	����������־λ
*/
ErrorType_e Set_UpdateFlag(uint8_t ucFlag)
{
	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
	
	if(g_tGlobalParam.ucUpdateFlag != ucFlag)
	{
		g_tGlobalParam.ucUpdateFlag = ucFlag;
		g_tGlobalParam.usCrc = CRC16((uint8_t*)&g_tGlobalParam, GLOBAL_PARAM_SAVE_TO_EEPROM_LEN-2);
		eErrorType = Save_Global_Param(&g_tGlobalParam);	
	}	
	
	return eErrorType;
}







/*
* ����/��ȡģ����� Bank_0
*/
ErrorType_e GlobalParam_Bank_0(ReadWrite_e eReadWrite, uint8_t ucType, Data4Byte_u *puData)
{
	extern __IO GlobalParam_t g_tGlobalParam;
	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
	uint8_t ucSaveFlag = 0;
	
	//
	switch(ucType)
	{
		case EN_MODULE_PARAM_TYPE_CanBaud:
		{
			if(eReadWrite == TMC_READ)
			{
				puData->lData = g_tGlobalParam.eCanBaud;
			}else if(eReadWrite == TMC_WRITE) {
				//�������
				if(puData->lData >= EN_CAN_BAUD_END) return ERROR_TYPE_DATA;
				if(g_tGlobalParam.eCanBaud != puData->lData)
				{
					g_tGlobalParam.eCanBaud = (CanBaud_e)puData->lData;
					ucSaveFlag = 2;
				}
			}
		}
		break;
		case EN_MODULE_PARAM_TYPE_RECV_CanID:
		{
			if(eReadWrite == TMC_READ)
			{
				puData->ulData = g_tGlobalParam.ulRecvCanID;
			}else if(eReadWrite == TMC_WRITE) {
				//�������		
				if(g_tGlobalParam.ulRecvCanID != puData->ulData)
				{
					g_tGlobalParam.ulRecvCanID = puData->ulData;
					ucSaveFlag = 2;
				}
			}
		}
		break;
		case EN_MODULE_PARAM_TYPE_SEND_CanID:
		{
			if(eReadWrite == TMC_READ)
			{
				puData->ulData = g_tGlobalParam.ulSendCanID;
			}else if(eReadWrite == TMC_WRITE) {
				//�������			
				if(g_tGlobalParam.ulSendCanID != puData->ulData)
				{
					g_tGlobalParam.ulSendCanID = puData->ulData;
					ucSaveFlag = 1;
				}
			}
		}
		break;
		case EN_MODULE_PARAM_TYPE_ProcessAutoExecMode:
		{
			if(eReadWrite == TMC_READ)
			{
				puData->lData = g_tGlobalParam.ucProcessAutoExecMode;
			}else if(eReadWrite == TMC_WRITE) {
				//�������
				if(g_tGlobalParam.ucProcessAutoExecMode != puData->lData)
				{
					g_tGlobalParam.ucProcessAutoExecMode = puData->lData;
					ucSaveFlag = 1;
				}
			}
		}
		break;
		default:
		{
			//Type Error
			return ERROR_TYPE_TYPE;
		}
	}

	//save modified param
	if(1 == ucSaveFlag)
	{
		//param had modified
		g_tGlobalParam.usCrc = CRC16((uint8_t*)&g_tGlobalParam, GLOBAL_PARAM_SAVE_TO_EEPROM_LEN-2);
		eErrorType = Save_Global_Param(&g_tGlobalParam);
	}
	
	//Re Init Can
	if(ucSaveFlag == 2)
	{			
		//param had modified
		g_tGlobalParam.usCrc = CRC16((uint8_t*)&g_tGlobalParam, GLOBAL_PARAM_SAVE_TO_EEPROM_LEN-2);
		eErrorType = Save_Global_Param(&g_tGlobalParam);
		
		//MCU_Reset();
//@todo		Can_ReInit(g_tGlobalParam.eCanBaud);
	}
	
	return eErrorType;
}




/*
* ����CanID
*/
ErrorType_e GlobalParam_Set_CanID(uint8_t ucRecvCanID, uint8_t SendCanID)
{
	extern void MCU_Reset(void);
	extern __IO GlobalParam_t g_tGlobalParam;
	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
	uint8_t ucSaveFlag = 0;
	
				
	//�������			
	if(g_tGlobalParam.ulSendCanID != SendCanID)
	{
		g_tGlobalParam.ulSendCanID = SendCanID;
		ucSaveFlag = 1;
	}
	
	//�������		
	if(g_tGlobalParam.ulRecvCanID != ucRecvCanID)
	{
		g_tGlobalParam.ulRecvCanID = ucRecvCanID;
		ucSaveFlag = 2;
	}
				
				
	//save modified param
	if(1 == ucSaveFlag)
	{
		//param had modified
		g_tGlobalParam.usCrc = CRC16((uint8_t*)&g_tGlobalParam, GLOBAL_PARAM_SAVE_TO_EEPROM_LEN-2);
		eErrorType = Save_Global_Param(&g_tGlobalParam);
	}
	
	//Re Init Can
	if(ucSaveFlag == 2)
	{			
		//param had modified
		g_tGlobalParam.usCrc = CRC16((uint8_t*)&g_tGlobalParam, GLOBAL_PARAM_SAVE_TO_EEPROM_LEN-2);
		eErrorType = Save_Global_Param(&g_tGlobalParam);
		
		MCU_Reset(); //@todo
		//Can_ReInit(g_tGlobalParam.eCanBaud);
	}
	
	return eErrorType;
}




/*
* ����/��ȡģ����� Bank_1
*/
ErrorType_e GlobalParam_Bank_1(ReadWrite_e eReadWrite, uint8_t ucType, Data4Byte_u *puData)
{
	extern __IO  GlobalParam_t g_tGlobalParam;
//	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
	uint8_t ucSaveFlag = 0;
	
	//
	if(ucType >= BANK1_USER_VAR_LEN)
	{
		return ERROR_TYPE_TYPE;
	}else{
		if(eReadWrite == TMC_READ)
		{
			puData->lData = g_tGlobalParam.laBank1_UserVar[ucType];
		}else if(eReadWrite == TMC_WRITE) {
			if(g_tGlobalParam.laBank1_UserVar[ucType] != puData->lData)
			{
				g_tGlobalParam.laBank1_UserVar[ucType] = puData->lData;
				ucSaveFlag = 1;
			}
		}
	}
	
	
	//save to EEPROM
	if(ucSaveFlag == 1)
	{
		//param had modified
		g_tGlobalParam.usCrc = CRC16((uint8_t*)&g_tGlobalParam, GLOBAL_PARAM_SAVE_TO_EEPROM_LEN-2);
		Save_Global_Param(&g_tGlobalParam);
	}
	
	
	return ERROR_TYPE_SUCCESS;
}


/*
* ����/��ȡģ����� Bank_2 �û�����
*/
ErrorType_e GlobalParam_Bank_2(ReadWrite_e eReadWrite, uint8_t ucType, Data4Byte_u *puData)
{

	extern __IO  GlobalParam_t g_tGlobalParam;
//	uint8_t ucSaveFlag = 0;
	
	//
	if(ucType >= BANK2_USER_VAR_LEN)
	{
		return ERROR_TYPE_TYPE;
	}else{
		if(eReadWrite == TMC_READ)
		{
			puData->lData = g_tGlobalParam.laBank2_UserVar[ucType];
		}else if(eReadWrite == TMC_WRITE) {

			g_tGlobalParam.laBank2_UserVar[ucType] = puData->lData;
		}
	}
//	
//	if(ucType >= 0 && ucType <= 0x3F) /* �û�����1 , RAM��*/ 
//	{
//		if(eReadWrite == TMC_READ)
//		{
//			puData->lData = g_tGlobalParam.laBank2_UserVar[ucType];
//		}else if(eReadWrite == TMC_WRITE) {

//			g_tGlobalParam.laBank2_UserVar[ucType] = puData->lData;
//		}	
//	}else if(ucType >= 40 && ucType <= 0x7F){  /* �û�����2 RAM�У������µ�EEPROM��*/
//		
//		if(eReadWrite == TMC_READ)
//		{
//			puData->lData = g_tGlobalParam.laBank2_UserVar[ucType];
//		}else if(eReadWrite == TMC_WRITE) {
//			if(g_tGlobalParam.laBank2_UserVar[ucType] != puData->lData)
//			{
//				g_tGlobalParam.laBank2_UserVar[ucType] = puData->lData;
//				ucSaveFlag = 1;
//			}
//			
//		}
//	}else{
//		return ERROR_TYPE_TYPE;
//	}
//	
//	
//	//save to EEPROM
//	if(ucSaveFlag == 1)
//	{
//		//param had modified
//		g_tGlobalParam.usCrc = CRC16((uint8_t*)&g_tGlobalParam, GLOBAL_PARAM_SAVE_TO_EEPROM_LEN-2);
//		Save_Global_Param(&g_tGlobalParam);
//	}

	return ERROR_TYPE_SUCCESS;
}



/*
* ����/��ȡģ����� Bank_3 
*/
ErrorType_e GlobalParam_Bank_3(ReadWrite_e eReadWrite, uint8_t ucType, Data4Byte_u *puData)
{
//	extern GlobalParam_t g_tGlobalParam;
//	extern ProtcessTimeCount_t g_tProtcessTimeCount;
	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
////	uint8_t ucSaveFlag = 0;
//	
//	//
//	if(ucType >= BANK3_USER_VAR_LEN)
//	{
//		return ERROR_TYPE_TYPE;
//	}else{
//		if(eReadWrite == TMC_READ)
//		{
//			puData->lData = g_tGlobalParam.laBank3_UserVar[ucType];
//		}else if(eReadWrite == TMC_WRITE) {
//			if(puData->lData != 0)
//			{
//				g_tProtcessTimeCount.ulStartFlag[ucType] = 1;
//				g_tProtcessTimeCount.ulTriggerFlag[ucType] = 0;
//				g_tProtcessTimeCount.ulStartTick[ucType] = HAL_GetTick();
//				g_tProtcessTimeCount.ulThreshTick[ucType] = puData->lData;
//			}else{
//				g_tProtcessTimeCount.ulStartFlag[ucType] = 0;
//				g_tProtcessTimeCount.ulTriggerFlag[ucType] = 0;
//				g_tProtcessTimeCount.ulStartTick[ucType] = 0;
//				g_tProtcessTimeCount.ulThreshTick[ucType] = 0;			
//			}
//		}
//	}
	
	return eErrorType;
}



/*
*  ����/��ȡģ�����
*/
ErrorType_e TMC_Global_Param(Bank_e eBank, ReadWrite_e eReadWrite, uint8_t ucType, Data4Byte_u *puData)
{	
	ErrorType_e eError = ERROR_TYPE_SUCCESS;
	
	//�������
	if(eBank >= BANK_END) return ERROR_TYPE_DEVICE_ID;
	
	switch(eBank) 
	{
		case BANK_0:
		{
			eError = GlobalParam_Bank_0(eReadWrite, ucType, puData);
		}
		break;
		case BANK_1:
		{
			eError = GlobalParam_Bank_1(eReadWrite, ucType, puData);
		}
		break;
		case BANK_2:
		{
			eError = GlobalParam_Bank_2(eReadWrite, ucType, puData);
		}
		break;
		case BANK_3:
		{
			eError = GlobalParam_Bank_3(eReadWrite, ucType, puData);
		}
		break;
		default:
		{
			LOG_Error("Bank ID=%d Is Err", eBank);
			eError = ERROR_TYPE_DEVICE_ID;
		}
		break;
	}
	
	return eError;
	
	
}




/* 
* 
*  ��ȡȫ�ֲ��� 
*
*/
ErrorType_e Read_Global_Param(__IO GlobalParam_t *ptGlobalParam)
{
	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
	uint8_t  ucNum = 0;
	uint16_t usCrc = 0;
	
	//��ȡ����	
	do{
		//����
		ucNum++;
		
		//��ȡ
		memset((void*)ptGlobalParam, 0, sizeof(GlobalParam_t));
		Param_Read(EN_SAVE_PARAM_TYPE_GLOBAL, (uint8_t*)ptGlobalParam, GLOBAL_PARAM_SAVE_TO_EEPROM_LEN);
		usCrc = CRC16((uint8_t*)ptGlobalParam, GLOBAL_PARAM_SAVE_TO_EEPROM_LEN-2);
		
		//У��
		if(usCrc == ptGlobalParam->usCrc)
		{
			return ERROR_TYPE_SUCCESS;
		}else{
			//HAL_Delay(2);
			rt_thread_delay(2);
		}
		
	}while(ucNum < 3);

	
	//��һ���ϵ�
	if(ptGlobalParam->usCrc != usCrc && ptGlobalParam->ulInitFlag != PARAM_INIT_FLAG)
	{
		//��һ���ϵ�, ��ʼ��������
		Global_Param_SetDefault_Value(ptGlobalParam);
		//����		
		return Save_Global_Param(ptGlobalParam);
	}

	//����У����
	if(ucNum >= 3)
	{
//		LOG_Warn("Read Global Param CRC Error");
		return ERROR_TYPE_EEPROM;
	}
	
	return eErrorType;
}


/* 
* 
*  ����ȫ�ֲ��� 
*
*/
ErrorType_e Save_Global_Param(__IO GlobalParam_t *ptGlobalParam)
{
	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
	uint8_t  ucNum = 0;
	uint16_t usWriteCrc = 0, usReadCrc = 0;
	GlobalParam_t  tGlobalParam = {0};
	
	//����ԭʼ����CRC
	usWriteCrc = ptGlobalParam->usCrc;

	
	//��ȡ����	
	do{
		//����
		ucNum++;
		
		//д��
		memmove((void*)&tGlobalParam, (void*)ptGlobalParam, sizeof(GlobalParam_t));
		Param_Write(EN_SAVE_PARAM_TYPE_GLOBAL, (uint8_t*)&tGlobalParam, GLOBAL_PARAM_SAVE_TO_EEPROM_LEN);
			
		//��ȡ
		memset((void*)&tGlobalParam, 0, sizeof(GlobalParam_t));
		Param_Read(EN_SAVE_PARAM_TYPE_GLOBAL, (uint8_t*)&tGlobalParam, GLOBAL_PARAM_SAVE_TO_EEPROM_LEN);
		usReadCrc = CRC16((uint8_t*)&tGlobalParam, GLOBAL_PARAM_SAVE_TO_EEPROM_LEN-2);
		
		//�Ա�ǰ��crc
		if(usWriteCrc == usReadCrc && usReadCrc == tGlobalParam.usCrc)
		{
			return ERROR_TYPE_SUCCESS;
		}else{
			//HAL_Delay(2);
			rt_thread_delay(2);
		}
		
	}while(ucNum < 3);  //����ظ�3��

	
	//����ʧ��
	if(ucNum >= 3)
	{
		LOG_Error("Save Global Param Fail");
		return ERROR_TYPE_EEPROM;
	}

	return eErrorType;
}


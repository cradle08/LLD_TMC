#include "main.h"
#include "eeprom.h"
#include "lld_param.h"
#include "crc.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdarg.h"
#include "string.h"

//ģ�������
__IO LLDParam_t g_tLLDParam = {0};



/*
*	ȫ�ֲ�����ʼ��
*/
ErrorType_e LLDParam_Init(void)
{
	return Read_LLDParam(&g_tLLDParam);
}



/*
*	ģ����� ���
*/
void LLDParam_SetDefault_Value(__IO LLDParam_t *ptLLDParam)
{
	//��һ���ϵ�, ��ʼ��������
	memset((void*)ptLLDParam, 0, sizeof(LLDParam_t));
	
	/* Bank 0 */
	ptLLDParam->ulInitFlag  	   = PARAM_INIT_FLAG;
	ptLLDParam->CanConfig.eCanBaud = EN_CAN_BAUD_1000; 	   //������
	ptLLDParam->CanConfig.ModuleID = LLD_CAN_DEFAULT_RECV_ID;   //Ĭ�Ͻ���CanID
	ptLLDParam->CanConfig.ReplyID  = LLD_CAN_DEFAULT_SEND_ID;   //Ĭ�Ϸ���CanID

	//crc
	ptLLDParam->usCrc       	   = CRC16((uint8_t*)ptLLDParam, LLD_PARAM_LEN-2);
}




/*
*	���EEPROM�����ģ�����
*/
void ClearAndSave_Default_LLDParams(void)
{
	LLDParam_SetDefault_Value(&g_tLLDParam);
	//Param_Write(EN_SAVE_PARAM_TYPE_GLOBAL, (uint8_t*)&g_tGlobalParam, GLOBAL_PARAM_SAVE_TO_EEPROM_LEN);
	Save_LLDParam(&g_tLLDParam);
}






///*
//*	����������־λ
//*/
//ErrorType_e Set_UpdateFlag(uint8_t ucFlag)
//{
//	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
//	
//	if(g_tLLDParam.ucUpdateFlag != ucFlag)
//	{
//		g_tLLDParam.ucUpdateFlag = ucFlag;
//		g_tLLDParam.usCrc = CRC16((uint8_t*)&LLDParam_t, sizeof(LLDParam_t)-2);
//		eErrorType = Save_Global_Param(&g_tLLDParam);	
//	}	
//	
//	return eErrorType;
//}




/* 
* 
*  ��ȡȫ�ֲ��� 
*
*/
ErrorType_e Read_LLDParam(__IO LLDParam_t *ptLLDParam)
{
	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
	uint8_t  ucNum = 0;
	uint16_t usCrc = 0;
	
	//��ȡ����	
	do{
		//����
		ucNum++;
		
		//��ȡ
		memset((void*)ptLLDParam, 0, sizeof(LLDParam_t));
		Param_Read(EN_SAVE_PARAM_TYPE_LLD_PARAM, (uint8_t*)ptLLDParam, LLD_PARAM_LEN);
		usCrc = CRC16((uint8_t*)ptLLDParam, LLD_PARAM_LEN-2);
		
		//У��
		if(usCrc == ptLLDParam->usCrc)
		{
			return ERROR_TYPE_SUCCESS;
		}else{
			//HAL_Delay(2);
			rt_thread_mdelay(2);
		}
		
	}while(ucNum < 3);

	
	//��һ���ϵ�
	if(ptLLDParam->usCrc != usCrc && ptLLDParam->ulInitFlag != PARAM_INIT_FLAG)
	{
		//��һ���ϵ�, ��ʼ��������
		LLDParam_SetDefault_Value(ptLLDParam);
		//����		
		return Save_LLDParam(ptLLDParam);
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
ErrorType_e Save_LLDParam(__IO LLDParam_t *ptLLDParam)
{
	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
	uint8_t  ucNum = 0;
	uint16_t usWriteCrc = 0, usReadCrc = 0;
	LLDParam_t  tLLDParam = {0};
	
	//����ԭʼ����CRC
	usWriteCrc = ptLLDParam->usCrc;
	
	//��ȡ����	
	do{
		//����
		ucNum++;
		
		//д��
		memmove((void*)&tLLDParam, (void*)ptLLDParam, sizeof(LLDParam_t));
		Param_Write(EN_SAVE_PARAM_TYPE_LLD_PARAM, (uint8_t*)&tLLDParam, LLD_PARAM_LEN);
			
		//��ȡ
		memset((void*)&tLLDParam, 0, sizeof(LLDParam_t));
		Param_Read(EN_SAVE_PARAM_TYPE_LLD_PARAM, (uint8_t*)&tLLDParam, LLD_PARAM_LEN);
		usReadCrc = CRC16((uint8_t*)&tLLDParam, LLD_PARAM_LEN-2);
		
		//�Ա�ǰ��crc
		if(usWriteCrc == usReadCrc && usReadCrc == tLLDParam.usCrc)
		{
			return ERROR_TYPE_SUCCESS;
		}else{
			//HAL_Delay(2);
			rt_thread_mdelay(2);
		}
		
	}while(ucNum < 3);  //����ظ�3��

	
	//����ʧ��
	if(ucNum >= 3)
	{
//		LOG_Error("Save Global Param Fail");
		return ERROR_TYPE_EEPROM;
	}

	return eErrorType;
}





//�޸�Һ��̽��can����
ErrorType_e LLD_Param(ReadWrite_e eReadWrite, uint8_t ucType, int32_t *plValue)
{
	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
	uint8_t ucSaveFlag = 0;
	
	//
	switch(ucType)
	{
		case 0://������
		{
			if(eReadWrite == TMC_READ)
			{
				*plValue = g_tLLDParam.CanConfig.eCanBaud;
			}else if(eReadWrite == TMC_WRITE) {
				//�������
				if(*plValue >= EN_CAN_BAUD_END) return ERROR_TYPE_DATA;
				if(g_tLLDParam.CanConfig.eCanBaud != *plValue)
				{
					g_tLLDParam.CanConfig.eCanBaud = (CanBaud_e)*plValue;
					ucSaveFlag = 2;
				}
			}
		}
		break;
		case 1: //ģ��Can ID������
		{
			if(eReadWrite == TMC_READ)
			{
				*plValue = g_tLLDParam.CanConfig.ModuleID;
			}else if(eReadWrite == TMC_WRITE) {
				//�������		
				if(g_tLLDParam.CanConfig.ModuleID != *plValue)
				{
					g_tLLDParam.CanConfig.ModuleID = *plValue;
					ucSaveFlag = 2;
				}
			}
		}
		break;
		case 2://ģ��Can ID������
		{
			if(eReadWrite == TMC_READ)
			{
				*plValue = g_tLLDParam.CanConfig.ReplyID;
			}else if(eReadWrite == TMC_WRITE) {
				//�������			
				if(g_tLLDParam.CanConfig.ReplyID != *plValue)
				{
					g_tLLDParam.CanConfig.ReplyID = *plValue;
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
		g_tLLDParam.usCrc = CRC16((uint8_t*)&g_tLLDParam, LLD_PARAM_LEN-2);
		eErrorType = Save_LLDParam(&g_tLLDParam);
	}
	
	//Re Init Can
	if(ucSaveFlag == 2)
	{			
		//param had modified
		g_tLLDParam.usCrc = CRC16((uint8_t*)&g_tLLDParam, LLD_PARAM_LEN-2);
		eErrorType = Save_LLDParam(&g_tLLDParam);

//		Can_ReInit(g_tGlobalParam.eCanBaud);		
		CAN_Config(CAN1);
		CAN_NVIC_Config();
	}
	
	return eErrorType;

}








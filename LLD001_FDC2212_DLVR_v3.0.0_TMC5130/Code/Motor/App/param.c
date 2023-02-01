#include "param.h"
#include "main.h"
#include "log.h"
#include "crc.h"
#include "eeprom.h"
#include "bsp_can.h"
//#include "TMC_Process.h"

//模块参数表
__IO GlobalParam_t 	 g_tGlobalParam = {0};



/*
*	全局参数初始化
*/
ErrorType_e Global_Param_Init(void)
{
	return Read_Global_Param(&g_tGlobalParam);
	
}



/*
*	模块参数 清除
*/
void Global_Param_SetDefault_Value(__IO GlobalParam_t *ptGlobalParam)
{
	//第一次上电, 初始化，保存
	memset((void*)ptGlobalParam, 0, sizeof(GlobalParam_t));
	
	/* Bank 0 */
	ptGlobalParam->ulInitFlag  	  = PARAM_INIT_FLAG;
	ptGlobalParam->eCanBaud    	  = EN_CAN_BAUD_1000; 	   //波特率
	ptGlobalParam->ulRecvCanID    = CAN_DEFAULT_RECV_ID;   //默认接受CanID
	ptGlobalParam->ulSendCanID    = CAN_DEFAULT_SEND_ID;   //默认发送CanID
	ptGlobalParam->ucProcessAutoExecMode = 0; //自定义流程，上电自动执行标志, 0:下发指令触发执行，1：上电自动执行
	
	/* Bank 1 保留 */
	
	/* Bank 2  用户变量2*/
	
	/* Bank 3 中断 */
	
	/* Bank 2  用户变量1*/
	
	//crc
	ptGlobalParam->usCrc       	  = CRC16((uint8_t*)ptGlobalParam, GLOBAL_PARAM_SAVE_TO_EEPROM_LEN-2);
}




/*
*	清除EEPROM保存的模块参数
*/
void ClearAndSave_Default_Global_Params(void)
{
	Global_Param_SetDefault_Value(&g_tGlobalParam);
	//Param_Write(EN_SAVE_PARAM_TYPE_GLOBAL, (uint8_t*)&g_tGlobalParam, GLOBAL_PARAM_SAVE_TO_EEPROM_LEN);
	Save_Global_Param(&g_tGlobalParam);
	
}






/*
*	设置升级标志位
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
* 设置/获取模块参数 Bank_0
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
				//参数检查
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
				//参数检查		
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
				//参数检查			
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
				//参数检查
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
* 设置CanID
*/
ErrorType_e GlobalParam_Set_CanID(uint8_t ucRecvCanID, uint8_t SendCanID)
{
	extern void MCU_Reset(void);
	extern __IO GlobalParam_t g_tGlobalParam;
	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
	uint8_t ucSaveFlag = 0;
	
				
	//参数检查			
	if(g_tGlobalParam.ulSendCanID != SendCanID)
	{
		g_tGlobalParam.ulSendCanID = SendCanID;
		ucSaveFlag = 1;
	}
	
	//参数检查		
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
* 设置/获取模块参数 Bank_1
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
* 设置/获取模块参数 Bank_2 用户变量
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
//	if(ucType >= 0 && ucType <= 0x3F) /* 用户变量1 , RAM中*/ 
//	{
//		if(eReadWrite == TMC_READ)
//		{
//			puData->lData = g_tGlobalParam.laBank2_UserVar[ucType];
//		}else if(eReadWrite == TMC_WRITE) {

//			g_tGlobalParam.laBank2_UserVar[ucType] = puData->lData;
//		}	
//	}else if(ucType >= 40 && ucType <= 0x7F){  /* 用户变量2 RAM中，并更新到EEPROM中*/
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
* 设置/获取模块参数 Bank_3 
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
*  设置/获取模块参数
*/
ErrorType_e TMC_Global_Param(Bank_e eBank, ReadWrite_e eReadWrite, uint8_t ucType, Data4Byte_u *puData)
{	
	ErrorType_e eError = ERROR_TYPE_SUCCESS;
	
	//参数检擦
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
*  读取全局参数 
*
*/
ErrorType_e Read_Global_Param(__IO GlobalParam_t *ptGlobalParam)
{
	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
	uint8_t  ucNum = 0;
	uint16_t usCrc = 0;
	
	//读取参数	
	do{
		//计数
		ucNum++;
		
		//读取
		memset((void*)ptGlobalParam, 0, sizeof(GlobalParam_t));
		Param_Read(EN_SAVE_PARAM_TYPE_GLOBAL, (uint8_t*)ptGlobalParam, GLOBAL_PARAM_SAVE_TO_EEPROM_LEN);
		usCrc = CRC16((uint8_t*)ptGlobalParam, GLOBAL_PARAM_SAVE_TO_EEPROM_LEN-2);
		
		//校验
		if(usCrc == ptGlobalParam->usCrc)
		{
			return ERROR_TYPE_SUCCESS;
		}else{
			//HAL_Delay(2);
			rt_thread_delay(2);
		}
		
	}while(ucNum < 3);

	
	//第一次上电
	if(ptGlobalParam->usCrc != usCrc && ptGlobalParam->ulInitFlag != PARAM_INIT_FLAG)
	{
		//第一次上电, 初始化，保存
		Global_Param_SetDefault_Value(ptGlobalParam);
		//保存		
		return Save_Global_Param(ptGlobalParam);
	}

	//数据校验检测
	if(ucNum >= 3)
	{
//		LOG_Warn("Read Global Param CRC Error");
		return ERROR_TYPE_EEPROM;
	}
	
	return eErrorType;
}


/* 
* 
*  保存全局参数 
*
*/
ErrorType_e Save_Global_Param(__IO GlobalParam_t *ptGlobalParam)
{
	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
	uint8_t  ucNum = 0;
	uint16_t usWriteCrc = 0, usReadCrc = 0;
	GlobalParam_t  tGlobalParam = {0};
	
	//计数原始数据CRC
	usWriteCrc = ptGlobalParam->usCrc;

	
	//读取参数	
	do{
		//计数
		ucNum++;
		
		//写入
		memmove((void*)&tGlobalParam, (void*)ptGlobalParam, sizeof(GlobalParam_t));
		Param_Write(EN_SAVE_PARAM_TYPE_GLOBAL, (uint8_t*)&tGlobalParam, GLOBAL_PARAM_SAVE_TO_EEPROM_LEN);
			
		//读取
		memset((void*)&tGlobalParam, 0, sizeof(GlobalParam_t));
		Param_Read(EN_SAVE_PARAM_TYPE_GLOBAL, (uint8_t*)&tGlobalParam, GLOBAL_PARAM_SAVE_TO_EEPROM_LEN);
		usReadCrc = CRC16((uint8_t*)&tGlobalParam, GLOBAL_PARAM_SAVE_TO_EEPROM_LEN-2);
		
		//对比前后crc
		if(usWriteCrc == usReadCrc && usReadCrc == tGlobalParam.usCrc)
		{
			return ERROR_TYPE_SUCCESS;
		}else{
			//HAL_Delay(2);
			rt_thread_delay(2);
		}
		
	}while(ucNum < 3);  //最多重复3次

	
	//保存失败
	if(ucNum >= 3)
	{
		LOG_Error("Save Global Param Fail");
		return ERROR_TYPE_EEPROM;
	}

	return eErrorType;
}


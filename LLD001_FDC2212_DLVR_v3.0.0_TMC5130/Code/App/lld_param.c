#include "main.h"
#include "eeprom.h"
#include "lld_param.h"
#include "crc.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdarg.h"
#include "string.h"

//模块参数表
__IO LLDParam_t g_tLLDParam = {0};



/*
*	全局参数初始化
*/
ErrorType_e LLDParam_Init(void)
{
	return Read_LLDParam(&g_tLLDParam);
}



/*
*	模块参数 清除
*/
void LLDParam_SetDefault_Value(__IO LLDParam_t *ptLLDParam)
{
	//第一次上电, 初始化，保存
	memset((void*)ptLLDParam, 0, sizeof(LLDParam_t));
	
	/* Bank 0 */
	ptLLDParam->ulInitFlag  	   = PARAM_INIT_FLAG;
	ptLLDParam->CanConfig.eCanBaud = EN_CAN_BAUD_1000; 	   //波特率
	ptLLDParam->CanConfig.ModuleID = LLD_CAN_DEFAULT_RECV_ID;   //默认接受CanID
	ptLLDParam->CanConfig.ReplyID  = LLD_CAN_DEFAULT_SEND_ID;   //默认发送CanID

	//crc
	ptLLDParam->usCrc       	   = CRC16((uint8_t*)ptLLDParam, LLD_PARAM_LEN-2);
}




/*
*	清除EEPROM保存的模块参数
*/
void ClearAndSave_Default_LLDParams(void)
{
	LLDParam_SetDefault_Value(&g_tLLDParam);
	//Param_Write(EN_SAVE_PARAM_TYPE_GLOBAL, (uint8_t*)&g_tGlobalParam, GLOBAL_PARAM_SAVE_TO_EEPROM_LEN);
	Save_LLDParam(&g_tLLDParam);
}






///*
//*	设置升级标志位
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
*  读取全局参数 
*
*/
ErrorType_e Read_LLDParam(__IO LLDParam_t *ptLLDParam)
{
	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
	uint8_t  ucNum = 0;
	uint16_t usCrc = 0;
	
	//读取参数	
	do{
		//计数
		ucNum++;
		
		//读取
		memset((void*)ptLLDParam, 0, sizeof(LLDParam_t));
		Param_Read(EN_SAVE_PARAM_TYPE_LLD_PARAM, (uint8_t*)ptLLDParam, LLD_PARAM_LEN);
		usCrc = CRC16((uint8_t*)ptLLDParam, LLD_PARAM_LEN-2);
		
		//校验
		if(usCrc == ptLLDParam->usCrc)
		{
			return ERROR_TYPE_SUCCESS;
		}else{
			//HAL_Delay(2);
			rt_thread_mdelay(2);
		}
		
	}while(ucNum < 3);

	
	//第一次上电
	if(ptLLDParam->usCrc != usCrc && ptLLDParam->ulInitFlag != PARAM_INIT_FLAG)
	{
		//第一次上电, 初始化，保存
		LLDParam_SetDefault_Value(ptLLDParam);
		//保存		
		return Save_LLDParam(ptLLDParam);
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
ErrorType_e Save_LLDParam(__IO LLDParam_t *ptLLDParam)
{
	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
	uint8_t  ucNum = 0;
	uint16_t usWriteCrc = 0, usReadCrc = 0;
	LLDParam_t  tLLDParam = {0};
	
	//计数原始数据CRC
	usWriteCrc = ptLLDParam->usCrc;
	
	//读取参数	
	do{
		//计数
		ucNum++;
		
		//写入
		memmove((void*)&tLLDParam, (void*)ptLLDParam, sizeof(LLDParam_t));
		Param_Write(EN_SAVE_PARAM_TYPE_LLD_PARAM, (uint8_t*)&tLLDParam, LLD_PARAM_LEN);
			
		//读取
		memset((void*)&tLLDParam, 0, sizeof(LLDParam_t));
		Param_Read(EN_SAVE_PARAM_TYPE_LLD_PARAM, (uint8_t*)&tLLDParam, LLD_PARAM_LEN);
		usReadCrc = CRC16((uint8_t*)&tLLDParam, LLD_PARAM_LEN-2);
		
		//对比前后crc
		if(usWriteCrc == usReadCrc && usReadCrc == tLLDParam.usCrc)
		{
			return ERROR_TYPE_SUCCESS;
		}else{
			//HAL_Delay(2);
			rt_thread_mdelay(2);
		}
		
	}while(ucNum < 3);  //最多重复3次

	
	//保存失败
	if(ucNum >= 3)
	{
//		LOG_Error("Save Global Param Fail");
		return ERROR_TYPE_EEPROM;
	}

	return eErrorType;
}





//修改液面探测can参数
ErrorType_e LLD_Param(ReadWrite_e eReadWrite, uint8_t ucType, int32_t *plValue)
{
	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
	uint8_t ucSaveFlag = 0;
	
	//
	switch(ucType)
	{
		case 0://波特率
		{
			if(eReadWrite == TMC_READ)
			{
				*plValue = g_tLLDParam.CanConfig.eCanBaud;
			}else if(eReadWrite == TMC_WRITE) {
				//参数检查
				if(*plValue >= EN_CAN_BAUD_END) return ERROR_TYPE_DATA;
				if(g_tLLDParam.CanConfig.eCanBaud != *plValue)
				{
					g_tLLDParam.CanConfig.eCanBaud = (CanBaud_e)*plValue;
					ucSaveFlag = 2;
				}
			}
		}
		break;
		case 1: //模块Can ID，接收
		{
			if(eReadWrite == TMC_READ)
			{
				*plValue = g_tLLDParam.CanConfig.ModuleID;
			}else if(eReadWrite == TMC_WRITE) {
				//参数检查		
				if(g_tLLDParam.CanConfig.ModuleID != *plValue)
				{
					g_tLLDParam.CanConfig.ModuleID = *plValue;
					ucSaveFlag = 2;
				}
			}
		}
		break;
		case 2://模块Can ID，发送
		{
			if(eReadWrite == TMC_READ)
			{
				*plValue = g_tLLDParam.CanConfig.ReplyID;
			}else if(eReadWrite == TMC_WRITE) {
				//参数检查			
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








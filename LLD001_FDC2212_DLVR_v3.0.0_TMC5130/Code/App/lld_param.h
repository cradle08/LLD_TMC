#ifndef __LLD_PARAM_H__
#define __LLD_PARAM_H__

#include "main.h"





/*
*	模块参数表， 整个全局参数中，当前ucaUserVar1部分不需要保存到EEPROM中，其他部分都需要保存到EEPROM中
*/
typedef __packed struct {
	uint32_t    ulInitFlag;			//初始化标志
	/* Bank 0 */
//	uint8_t 	ucUpdateFlag;				//0:不需要升级，1：需要升级
	CanBaud_e	eCanBaud;					//CAN 波特率
	uint32_t 	ulRecvCanID;				//接受CanID
	uint32_t	ulSendCanID;				//发送CanID

	//crc
	uint16_t 	usCrc;				//CRC16 校验码
		
} LLDParam_t;
#define LLD_PARAM_LEN	sizeof(LLDParam_t)



//全局参数初始化
ErrorType_e LLDParam_Init(void);

//清除EEPROM保存的模块参数
void ClearAndSave_Default_LLDParams(void);

//清除 模块参数
void LLDParam_SetDefault_Value(__IO LLDParam_t *ptLLDParam);

//升级标志位
//ErrorType_e Set_UpdateFlag(uint8_t ucFlag);


/* 参数保存 */
ErrorType_e Read_LLDParam(__IO LLDParam_t *ptLLDParam);
ErrorType_e Save_LLDParam(__IO LLDParam_t *ptLLDParam);






/********* 液面探测 ***/


#endif // __LLD_PARAM_H__











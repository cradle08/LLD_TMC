#ifndef __PARAM_H__
#define __PARAM_H__

#include "main.h"



//参数初始化标志
#define PARAM_INIT_FLAG				0xea05



/*
 * 全局变量，区域ID
 */
typedef enum {
	BANK_0 = 0,	//
	BANK_1,		//
	BANK_2,		//
	BANK_3,		//
	BANK_END,

}Bank_e;



/*
*	模块参数类型
*/
typedef enum {
	EN_MODULE_PARAM_TYPE_CanBaud		= 0,	//波特率
	EN_MODULE_PARAM_TYPE_RECV_CanID,
	EN_MODULE_PARAM_TYPE_SEND_CanID,
	EN_MODULE_PARAM_TYPE_ProcessAutoExecMode,
	
	
//	EN_MODULE_PARAM_TYPE_ResetHighSpeed,		//复位，高速
//	EN_MODULE_PARAM_TYPE_ResetLowSpeed,			//复位，低速
//	EN_MODULE_PARAM_TYPE_ResetOff,				//复位，偏移值

} ModuleParamType_e;





/*
*	模块参数表， 整个全局参数中，当前ucaUserVar1部分不需要保存到EEPROM中，其他部分都需要保存到EEPROM中
*/
#define BANK1_USER_VAR_LEN	32
#define BANK2_USER_VAR_LEN	128
#define BANK3_USER_VAR_LEN	0
typedef __packed struct {
	uint32_t    ulInitFlag;			//初始化标志
	/* Bank 0 */
	uint8_t 	ucUpdateFlag;				//0:不需要升级，1：需要升级
	CanBaud_e	eCanBaud;					//CAN 波特率
	uint32_t 	ulRecvCanID;				//接受CanID
	uint32_t	ulSendCanID;				//发送CanID
	uint8_t     ucProcessAutoExecMode;		//自定义流程，上电自动执行标志, 0:下发指令触发执行，1：上电自动执行
	
	/* Bank 1 保留 */
	int32_t		laBank1_UserVar[BANK1_USER_VAR_LEN]; //Bank1 用户变量 (Type:0-31)，数据修改数据后，会默认同步到EEPROM中，上电启动后，从EEPROM中加载到RAM中
	
	//crc
	uint16_t 	usCrc;				//CRC16 校验码
	
	/* Bank 2  用户变量2*/
	int32_t		laBank2_UserVar[BANK2_USER_VAR_LEN]; //用户变量2 (Type:0-127)，仅保存到RAM中
	
	/* Bank 3  */
	//int32_t		laBank3_UserVar[BANK3_USER_VAR_LEN]; //用户变量2 (Type: 0)，仅保存到RAM中
	

		
} GlobalParam_t;
//#define GLOBAL_PARAM_SAVE_TO_EEPROM_LEN	 (sizeof(GlobalParam_t) -  BANK2_USER_VAR_LEN*sizeof(int32_t) - BANK3_USER_VAR_LEN*sizeof(int32_t))
#define GLOBAL_PARAM_SAVE_TO_EEPROM_LEN	 (sizeof(GlobalParam_t) -  BANK2_USER_VAR_LEN*sizeof(int32_t))



/* 全局参数 */

//全局参数初始化
ErrorType_e Global_Param_Init(void);
//清除EEPROM保存的模块参数
void ClearAndSave_Default_Global_Params(void);
//清除 模块参数
void Global_Param_SetDefault_Value(__IO GlobalParam_t *ptGlobalParam);
//清除EEPROM保存的模块参数
void ClearAndSave_Default_Global_Params(void);
//升级标志位
ErrorType_e Set_UpdateFlag(uint8_t ucFlag);

//全局参数块
ErrorType_e GlobalParam_Bank_0(ReadWrite_e eReadWrite, uint8_t ucType, Data4Byte_u *puData);
ErrorType_e GlobalParam_Bank_1(ReadWrite_e eReadWrite, uint8_t ucType, Data4Byte_u *puData);
ErrorType_e GlobalParam_Bank_2(ReadWrite_e eReadWrite, uint8_t ucType, Data4Byte_u *puData);
ErrorType_e GlobalParam_Bank_3(ReadWrite_e eReadWrite, uint8_t ucType, Data4Byte_u *puData);
ErrorType_e TMC_Global_Param(Bank_e eBank, ReadWrite_e eReadWrite, uint8_t ucType, Data4Byte_u *puData);

//
ErrorType_e GlobalParam_Set_CanID(uint8_t ucRecvCanID, uint8_t SendCanID);

/* 参数保存 */
ErrorType_e Read_Global_Param(__IO GlobalParam_t *ptGlobalParam);
ErrorType_e Save_Global_Param(__IO GlobalParam_t *ptGlobalParam);






#endif // __PARAM_H__











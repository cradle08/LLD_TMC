#ifndef __BSP_EEPROM_H__
#define __BSP_EEPROM_H__

#include "main.h"
#include "public.h"


//参数初始化标志
#define PARAM_INIT_FLAG				0xea05

/*
* 
*	当前EPPROM容量64Kb=8KB
*/
#define GLOBAL_PARAM_SAVE_ADDR		320		//全局参数保存起始地址
#define GLOBAL_PARAM_SAVE_MAX_LEN	1024	//全局数最大保持长度

//模块参数保存起始地址
#define AXIS_PARAM_SAVE_ADDR		(GLOBAL_PARAM_SAVE_ADDR+GLOBAL_PARAM_SAVE_MAX_LEN)
#define AXIS_ARAM_SAVE_MAX_LEN		1024    //模块参数最大保持长度

//流程保存起始地址
#define PROCESS_SAVE_ADDR			(AXIS_PARAM_SAVE_ADDR+AXIS_ARAM_SAVE_MAX_LEN)	
#define PROCESS_SAVE_MAX_LEN		3072	//流程最大保持长度


//液面探测参数
#define LLD_PARAM_SAVE_ADDR			6144	//从第6k处开始	
#define LLD_PARAM_SAVE_MAX_LEN		2048	//最大保持长度





/*
*	EEPROM 保存参数类型
*/
typedef enum {
	EN_SAVE_PARAM_TYPE_AXIS,			//轴参数(电机)
	EN_SAVE_PARAM_TYPE_GLOBAL,			//模块参数(电机)
	EN_SAVE_PARAM_TYPE_PROCESS,			//流程参数（电机）
	EN_SAVE_PARAM_TYPE_LLD_PARAM,		//液面探测参数
	EN_SAVE_PARAM_TYPE_END,
	
} SaveParamType_e;



/*
*	EEPROM结构体
*/
typedef struct {
	uint8_t 	 ucStatus;				//状态信息
	uint8_t		 ucEepromType;			//EEPROM 类型
	uint16_t	 usPageSize;			//页大小
	uint16_t	 usVolume;				//EEPROM 容量
	void 		(*pfWrite)(uint16_t usAddr, __IO uint8_t *pucData, uint16_t usLen);  //写
	void 		(*pfRead)(uint16_t usAddr, __IO uint8_t *pucData, uint16_t usLen);	//读
	void 		(*pfDelay)(uint32_t ulTick);									//延时ms
	void 		(*pfCS)(Bool_e eBool);						//片选
	void 		(*pfWriteProtect)(void);
	void 		(*pfHold)(void);
	uint8_t 	(*pfStatusRegister)(void);
	
} EEPROM_t;



//EEPROM结构体
void EEPROM_Init(void);
void EEPROM_Write(uint16_t usAddr, __IO uint8_t *pucData, uint16_t usLen);
void EEPROM_Read(uint16_t usAddr, __IO uint8_t *pucData, uint16_t usLen);




//参数
void Param_Read(SaveParamType_e eType, __IO uint8_t *pucData, uint16_t usLen);
void Param_Write(SaveParamType_e eType, __IO uint8_t *pucData, uint16_t usLen);	





#endif 









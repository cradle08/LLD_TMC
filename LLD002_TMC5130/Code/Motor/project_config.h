#ifndef __PROJECT_CONFIG_H__
#define __PROJECT_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "main.h"


#include "project.h"

	
//调试功能
#define MGIM_DEBUG		0

/*
*	当前程序类型为 APP
*/
#define CURRENT_SOFT_TYPE	SOFT_TYPE_APP

	

/* 
*  当前程序编译时，指定具体的模块类型
*/
#define CURRENT_MODULE_TYPE		 	MODULE_TYPE_TMC_STEP_MOTOR_1301_Common	 
#define CURRENT_USE_EEPROM_TYPE		EEPROM_TYPE_M95xxx



//TMC数量，输入输出IO
#define TMC_MODULE_END 			 	1		//该模块使用芯片的数量：1片TMC芯片
#define MODULE_MAX_OUT_IO_NUM	 	1		//最大支持1个输出IO
#define MODULE_MAX_IN_IO_NUM	 	0		//最大支持1个出入IO
#define MODULE_MAX_BANK_NUM	 	 	4		//全局参数块，个数







/*
*	App 起始地址定义
*/
#if (CURRENT_MODULE_TYPE == MODULE_TYPE_TMC_STEP_MOTOR_1161) //当前使用的MCU型号：STM32F103RCT6: 64k_SRAM + 256k_Flash
	
	//Bootloader 起始地址和长度定义
	#define BOOT_LOADER_START_ADDR		0x8000000	//bootloader
	#define BOOT_LOADER_MAX_LEN			0x8000		//32K
	
	//APP，起始地址和长度定义
	#define APP_START_ADDR				0x800A000	//40KB  起始点
	#define APP_MAX_LEN					0x36000		//216KB(256-40) 长度
	
	//MCU内部flash一页大小
	#define MCU_FLASH_ONE_PAGE_LEN		2048		//页大小	
	
	
	
#elif (CURRENT_MODULE_TYPE == MODULE_TYPE_TMC_STEP_MOTOR_3301_Pipette) //当前使用的MCU型号：STM32F103RCT6: 64k_SRAM + 256k_Flash
	
	//Bootloader 起始地址和长度定义
	#define BOOT_LOADER_START_ADDR		0x8000000	//bootloader
	#define BOOT_LOADER_MAX_LEN			0x8000		//32K
	
	//APP，起始地址和长度定义
	#define APP_START_ADDR				0x800A000	//40KB  起始点
	#define APP_MAX_LEN					0x36000		//216KB(256-40) 长度
	
	//MCU内部flash一页大小
	#define MCU_FLASH_ONE_PAGE_LEN		2048		//页大小	
	
	
	
#elif (CURRENT_MODULE_TYPE == MODULE_TYPE_TMC_STEP_MOTOR_3311_Common) //当前使用的MCU型号：STM32F103RCT6: 64k_SRAM + 256k_Flash
	//Bootloader 起始地址和长度定义
	#define BOOT_LOADER_START_ADDR		0x8000000	//bootloader
	#define BOOT_LOADER_MAX_LEN			0x8000		//32K
	
	//APP，起始地址和长度定义
	#define APP_START_ADDR				0x800A000	//40KB  起始点
	#define APP_MAX_LEN					0x36000		//216KB(256-40) 长度
	
	//MCU内部flash一页大小
	#define MCU_FLASH_ONE_PAGE_LEN		2048		//页大小	

#elif (CURRENT_MODULE_TYPE == MODULE_TYPE_TMC_STEP_MOTOR_1301_Common) //当前使用的MCU型号：STM32F103C8T6: 20k_SRAM + 64k_Flash
	//Bootloader 起始地址和长度定义
	#define BOOT_LOADER_START_ADDR		0x8000000	//bootloader
	#define BOOT_LOADER_MAX_LEN			0x5800		//22K
	
	//APP，起始地址和长度定义
	#define APP_START_ADDR				0x8005C00	//23KB  起始点
	#define APP_MAX_LEN					0xA400		//41k 长度
	
	//MCU内部flash一页大小
	#define MCU_FLASH_ONE_PAGE_LEN		1024		//页大小	


#elif (CURRENT_MODULE_TYPE == MODULE_TYPE_TMC_STEP_MOTOR_6201) //当前使用的MCU型号：STM32G431VBT6: 32k_SRAM + 128k_Flash
	//Bootloader 起始地址和长度定义
	#define BOOT_LOADER_START_ADDR		0x8000000	//bootloader
	#define BOOT_LOADER_MAX_LEN			0x5400		//21K
	
	//APP，起始地址和长度定义
	#define APP_START_ADDR				0x8005800	//22KB  起始点
	#define APP_MAX_LEN					0xA800		//216KB(256-40) 长度
	
	//MCU内部flash一页大小
	#define MCU_FLASH_ONE_PAGE_LEN		1024		//页大小	

#endif








#ifdef __cplusplus
}
#endif

#endif //__PROJECT_CONFIG_H__








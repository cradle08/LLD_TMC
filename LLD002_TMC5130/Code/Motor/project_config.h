#ifndef __PROJECT_CONFIG_H__
#define __PROJECT_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "main.h"


#include "project.h"

	
//���Թ���
#define MGIM_DEBUG		0

/*
*	��ǰ��������Ϊ APP
*/
#define CURRENT_SOFT_TYPE	SOFT_TYPE_APP

	

/* 
*  ��ǰ�������ʱ��ָ�������ģ������
*/
#define CURRENT_MODULE_TYPE		 	MODULE_TYPE_TMC_STEP_MOTOR_1301_Common	 
#define CURRENT_USE_EEPROM_TYPE		EEPROM_TYPE_M95xxx



//TMC�������������IO
#define TMC_MODULE_END 			 	1		//��ģ��ʹ��оƬ��������1ƬTMCоƬ
#define MODULE_MAX_OUT_IO_NUM	 	1		//���֧��1�����IO
#define MODULE_MAX_IN_IO_NUM	 	0		//���֧��1������IO
#define MODULE_MAX_BANK_NUM	 	 	4		//ȫ�ֲ����飬����







/*
*	App ��ʼ��ַ����
*/
#if (CURRENT_MODULE_TYPE == MODULE_TYPE_TMC_STEP_MOTOR_1161) //��ǰʹ�õ�MCU�ͺţ�STM32F103RCT6: 64k_SRAM + 256k_Flash
	
	//Bootloader ��ʼ��ַ�ͳ��ȶ���
	#define BOOT_LOADER_START_ADDR		0x8000000	//bootloader
	#define BOOT_LOADER_MAX_LEN			0x8000		//32K
	
	//APP����ʼ��ַ�ͳ��ȶ���
	#define APP_START_ADDR				0x800A000	//40KB  ��ʼ��
	#define APP_MAX_LEN					0x36000		//216KB(256-40) ����
	
	//MCU�ڲ�flashһҳ��С
	#define MCU_FLASH_ONE_PAGE_LEN		2048		//ҳ��С	
	
	
	
#elif (CURRENT_MODULE_TYPE == MODULE_TYPE_TMC_STEP_MOTOR_3301_Pipette) //��ǰʹ�õ�MCU�ͺţ�STM32F103RCT6: 64k_SRAM + 256k_Flash
	
	//Bootloader ��ʼ��ַ�ͳ��ȶ���
	#define BOOT_LOADER_START_ADDR		0x8000000	//bootloader
	#define BOOT_LOADER_MAX_LEN			0x8000		//32K
	
	//APP����ʼ��ַ�ͳ��ȶ���
	#define APP_START_ADDR				0x800A000	//40KB  ��ʼ��
	#define APP_MAX_LEN					0x36000		//216KB(256-40) ����
	
	//MCU�ڲ�flashһҳ��С
	#define MCU_FLASH_ONE_PAGE_LEN		2048		//ҳ��С	
	
	
	
#elif (CURRENT_MODULE_TYPE == MODULE_TYPE_TMC_STEP_MOTOR_3311_Common) //��ǰʹ�õ�MCU�ͺţ�STM32F103RCT6: 64k_SRAM + 256k_Flash
	//Bootloader ��ʼ��ַ�ͳ��ȶ���
	#define BOOT_LOADER_START_ADDR		0x8000000	//bootloader
	#define BOOT_LOADER_MAX_LEN			0x8000		//32K
	
	//APP����ʼ��ַ�ͳ��ȶ���
	#define APP_START_ADDR				0x800A000	//40KB  ��ʼ��
	#define APP_MAX_LEN					0x36000		//216KB(256-40) ����
	
	//MCU�ڲ�flashһҳ��С
	#define MCU_FLASH_ONE_PAGE_LEN		2048		//ҳ��С	

#elif (CURRENT_MODULE_TYPE == MODULE_TYPE_TMC_STEP_MOTOR_1301_Common) //��ǰʹ�õ�MCU�ͺţ�STM32F103C8T6: 20k_SRAM + 64k_Flash
	//Bootloader ��ʼ��ַ�ͳ��ȶ���
	#define BOOT_LOADER_START_ADDR		0x8000000	//bootloader
	#define BOOT_LOADER_MAX_LEN			0x5800		//22K
	
	//APP����ʼ��ַ�ͳ��ȶ���
	#define APP_START_ADDR				0x8005C00	//23KB  ��ʼ��
	#define APP_MAX_LEN					0xA400		//41k ����
	
	//MCU�ڲ�flashһҳ��С
	#define MCU_FLASH_ONE_PAGE_LEN		1024		//ҳ��С	


#elif (CURRENT_MODULE_TYPE == MODULE_TYPE_TMC_STEP_MOTOR_6201) //��ǰʹ�õ�MCU�ͺţ�STM32G431VBT6: 32k_SRAM + 128k_Flash
	//Bootloader ��ʼ��ַ�ͳ��ȶ���
	#define BOOT_LOADER_START_ADDR		0x8000000	//bootloader
	#define BOOT_LOADER_MAX_LEN			0x5400		//21K
	
	//APP����ʼ��ַ�ͳ��ȶ���
	#define APP_START_ADDR				0x8005800	//22KB  ��ʼ��
	#define APP_MAX_LEN					0xA800		//216KB(256-40) ����
	
	//MCU�ڲ�flashһҳ��С
	#define MCU_FLASH_ONE_PAGE_LEN		1024		//ҳ��С	

#endif








#ifdef __cplusplus
}
#endif

#endif //__PROJECT_CONFIG_H__








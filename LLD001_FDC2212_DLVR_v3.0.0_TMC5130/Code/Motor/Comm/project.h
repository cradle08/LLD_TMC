#ifndef __PROJECT_H__
#define __PROJECT_H__

#ifdef __cplusplus
extern "C" {
#endif



/* 
*  程序类型，BOOT  or APP
*/
#define SOFT_TYPE_BOOT	1
#define SOFT_TYPE_APP	2


/* 
*  程序支持的，TMC步进电机模块类型
*/
#define MODULE_TYPE_TMC_STEP_MOTOR_1301_Common		0x00001301		//TMC步进电机模块, 单轴板,3A,01号板 --通用单轴
#define MODULE_TYPE_TMC_STEP_MOTOR_1601				0x00001601		//TMC步进电机模块, 单轴板,6A,01号板
#define MODULE_TYPE_TMC_STEP_MOTOR_3301_Pipette		0x00003301		//TMC步进电机模块, 三轴板,3A,01号板 --移液器三轴
#define MODULE_TYPE_TMC_STEP_MOTOR_3311_Common		0x00003311		//TMC步进电机模块, 三轴板,3A,02号板 --通用三轴
#define MODULE_TYPE_TMC_STEP_MOTOR_6201				0x00006201		//TMC步进电机模块，六轴板,2A,01号板


//MCU的UUID地址
#define MCU_UUID_FLASH_ADDR							0x1FFFF7E8    //0x1FFFF7E0	//0x1FFFF7E8

/*
*
*	当前程序支持的EEPROM型号
*/
#define	EEPROM_TYPE_M95xxx 							0x01	//ST M85640 
#define	EEPROM_TYPE_AT25640 						0x02	//ST AT25640



/*
*
*	支持MCU型号
*/
#define MCU_MODEL_STM32F103C8						0x01
#define MCU_MODEL_STM32F103RC						0x02
#define MCU_MODEL_STM32G431VB						0x03




//广播CAN ID
#define CAN_BROADCAST_ID							0x00





#define PROJECT_TAG									"1576eb9"


  
#ifdef __cplusplus
}
#endif

#endif //__PROJECT_H__









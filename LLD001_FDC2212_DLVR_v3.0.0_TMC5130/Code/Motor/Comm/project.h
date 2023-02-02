#ifndef __PROJECT_H__
#define __PROJECT_H__

#ifdef __cplusplus
extern "C" {
#endif



/* 
*  �������ͣ�BOOT  or APP
*/
#define SOFT_TYPE_BOOT	1
#define SOFT_TYPE_APP	2


/* 
*  ����֧�ֵģ�TMC�������ģ������
*/
#define MODULE_TYPE_TMC_STEP_MOTOR_1301_Common		0x00001301		//TMC�������ģ��, �����,3A,01�Ű� --ͨ�õ���
#define MODULE_TYPE_TMC_STEP_MOTOR_1601				0x00001601		//TMC�������ģ��, �����,6A,01�Ű�
#define MODULE_TYPE_TMC_STEP_MOTOR_3301_Pipette		0x00003301		//TMC�������ģ��, �����,3A,01�Ű� --��Һ������
#define MODULE_TYPE_TMC_STEP_MOTOR_3311_Common		0x00003311		//TMC�������ģ��, �����,3A,02�Ű� --ͨ������
#define MODULE_TYPE_TMC_STEP_MOTOR_6201				0x00006201		//TMC�������ģ�飬�����,2A,01�Ű�


//MCU��UUID��ַ
#define MCU_UUID_FLASH_ADDR							0x1FFFF7E8    //0x1FFFF7E0	//0x1FFFF7E8

/*
*
*	��ǰ����֧�ֵ�EEPROM�ͺ�
*/
#define	EEPROM_TYPE_M95xxx 							0x01	//ST M85640 
#define	EEPROM_TYPE_AT25640 						0x02	//ST AT25640



/*
*
*	֧��MCU�ͺ�
*/
#define MCU_MODEL_STM32F103C8						0x01
#define MCU_MODEL_STM32F103RC						0x02
#define MCU_MODEL_STM32G431VB						0x03




//�㲥CAN ID
#define CAN_BROADCAST_ID							0x00





#define PROJECT_TAG									"1576eb9"


  
#ifdef __cplusplus
}
#endif

#endif //__PROJECT_H__









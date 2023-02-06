#ifndef __BSP_EEPROM_H__
#define __BSP_EEPROM_H__

#include "main.h"
#include "public.h"


//������ʼ����־
#define PARAM_INIT_FLAG				0xea05

/*
* 
*	��ǰEPPROM����64Kb=8KB
*/
#define GLOBAL_PARAM_SAVE_ADDR		320		//ȫ�ֲ���������ʼ��ַ
#define GLOBAL_PARAM_SAVE_MAX_LEN	1024	//ȫ������󱣳ֳ���

//ģ�����������ʼ��ַ
#define AXIS_PARAM_SAVE_ADDR		(GLOBAL_PARAM_SAVE_ADDR+GLOBAL_PARAM_SAVE_MAX_LEN)
#define AXIS_ARAM_SAVE_MAX_LEN		1024    //ģ�������󱣳ֳ���

//���̱�����ʼ��ַ
#define PROCESS_SAVE_ADDR			(AXIS_PARAM_SAVE_ADDR+AXIS_ARAM_SAVE_MAX_LEN)	
#define PROCESS_SAVE_MAX_LEN		3072	//������󱣳ֳ���


//Һ��̽�����
#define LLD_PARAM_SAVE_ADDR			6144	//�ӵ�6k����ʼ	
#define LLD_PARAM_SAVE_MAX_LEN		2048	//��󱣳ֳ���





/*
*	EEPROM �����������
*/
typedef enum {
	EN_SAVE_PARAM_TYPE_AXIS,			//�����(���)
	EN_SAVE_PARAM_TYPE_GLOBAL,			//ģ�����(���)
	EN_SAVE_PARAM_TYPE_PROCESS,			//���̲����������
	EN_SAVE_PARAM_TYPE_LLD_PARAM,		//Һ��̽�����
	EN_SAVE_PARAM_TYPE_END,
	
} SaveParamType_e;



/*
*	EEPROM�ṹ��
*/
typedef struct {
	uint8_t 	 ucStatus;				//״̬��Ϣ
	uint8_t		 ucEepromType;			//EEPROM ����
	uint16_t	 usPageSize;			//ҳ��С
	uint16_t	 usVolume;				//EEPROM ����
	void 		(*pfWrite)(uint16_t usAddr, __IO uint8_t *pucData, uint16_t usLen);  //д
	void 		(*pfRead)(uint16_t usAddr, __IO uint8_t *pucData, uint16_t usLen);	//��
	void 		(*pfDelay)(uint32_t ulTick);									//��ʱms
	void 		(*pfCS)(Bool_e eBool);						//Ƭѡ
	void 		(*pfWriteProtect)(void);
	void 		(*pfHold)(void);
	uint8_t 	(*pfStatusRegister)(void);
	
} EEPROM_t;



//EEPROM�ṹ��
void EEPROM_Init(void);
void EEPROM_Write(uint16_t usAddr, __IO uint8_t *pucData, uint16_t usLen);
void EEPROM_Read(uint16_t usAddr, __IO uint8_t *pucData, uint16_t usLen);




//����
void Param_Read(SaveParamType_e eType, __IO uint8_t *pucData, uint16_t usLen);
void Param_Write(SaveParamType_e eType, __IO uint8_t *pucData, uint16_t usLen);	





#endif 









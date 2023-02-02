#ifndef __LLD_PARAM_H__
#define __LLD_PARAM_H__

#include "main.h"





/*
*	ģ������� ����ȫ�ֲ����У���ǰucaUserVar1���ֲ���Ҫ���浽EEPROM�У��������ֶ���Ҫ���浽EEPROM��
*/
typedef __packed struct {
	uint32_t    ulInitFlag;			//��ʼ����־
	/* Bank 0 */
//	uint8_t 	ucUpdateFlag;				//0:����Ҫ������1����Ҫ����
	CanBaud_e	eCanBaud;					//CAN ������
	uint32_t 	ulRecvCanID;				//����CanID
	uint32_t	ulSendCanID;				//����CanID

	//crc
	uint16_t 	usCrc;				//CRC16 У����
		
} LLDParam_t;
#define LLD_PARAM_LEN	sizeof(LLDParam_t)



//ȫ�ֲ�����ʼ��
ErrorType_e LLDParam_Init(void);

//���EEPROM�����ģ�����
void ClearAndSave_Default_LLDParams(void);

//��� ģ�����
void LLDParam_SetDefault_Value(__IO LLDParam_t *ptLLDParam);

//������־λ
//ErrorType_e Set_UpdateFlag(uint8_t ucFlag);


/* �������� */
ErrorType_e Read_LLDParam(__IO LLDParam_t *ptLLDParam);
ErrorType_e Save_LLDParam(__IO LLDParam_t *ptLLDParam);






/********* Һ��̽�� ***/


#endif // __LLD_PARAM_H__











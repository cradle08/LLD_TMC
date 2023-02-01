#ifndef __PARAM_H__
#define __PARAM_H__

#include "main.h"



//������ʼ����־
#define PARAM_INIT_FLAG				0xea05



/*
 * ȫ�ֱ���������ID
 */
typedef enum {
	BANK_0 = 0,	//
	BANK_1,		//
	BANK_2,		//
	BANK_3,		//
	BANK_END,

}Bank_e;



/*
*	ģ���������
*/
typedef enum {
	EN_MODULE_PARAM_TYPE_CanBaud		= 0,	//������
	EN_MODULE_PARAM_TYPE_RECV_CanID,
	EN_MODULE_PARAM_TYPE_SEND_CanID,
	EN_MODULE_PARAM_TYPE_ProcessAutoExecMode,
	
	
//	EN_MODULE_PARAM_TYPE_ResetHighSpeed,		//��λ������
//	EN_MODULE_PARAM_TYPE_ResetLowSpeed,			//��λ������
//	EN_MODULE_PARAM_TYPE_ResetOff,				//��λ��ƫ��ֵ

} ModuleParamType_e;





/*
*	ģ������� ����ȫ�ֲ����У���ǰucaUserVar1���ֲ���Ҫ���浽EEPROM�У��������ֶ���Ҫ���浽EEPROM��
*/
#define BANK1_USER_VAR_LEN	32
#define BANK2_USER_VAR_LEN	128
#define BANK3_USER_VAR_LEN	0
typedef __packed struct {
	uint32_t    ulInitFlag;			//��ʼ����־
	/* Bank 0 */
	uint8_t 	ucUpdateFlag;				//0:����Ҫ������1����Ҫ����
	CanBaud_e	eCanBaud;					//CAN ������
	uint32_t 	ulRecvCanID;				//����CanID
	uint32_t	ulSendCanID;				//����CanID
	uint8_t     ucProcessAutoExecMode;		//�Զ������̣��ϵ��Զ�ִ�б�־, 0:�·�ָ���ִ�У�1���ϵ��Զ�ִ��
	
	/* Bank 1 ���� */
	int32_t		laBank1_UserVar[BANK1_USER_VAR_LEN]; //Bank1 �û����� (Type:0-31)�������޸����ݺ󣬻�Ĭ��ͬ����EEPROM�У��ϵ������󣬴�EEPROM�м��ص�RAM��
	
	//crc
	uint16_t 	usCrc;				//CRC16 У����
	
	/* Bank 2  �û�����2*/
	int32_t		laBank2_UserVar[BANK2_USER_VAR_LEN]; //�û�����2 (Type:0-127)�������浽RAM��
	
	/* Bank 3  */
	//int32_t		laBank3_UserVar[BANK3_USER_VAR_LEN]; //�û�����2 (Type: 0)�������浽RAM��
	

		
} GlobalParam_t;
//#define GLOBAL_PARAM_SAVE_TO_EEPROM_LEN	 (sizeof(GlobalParam_t) -  BANK2_USER_VAR_LEN*sizeof(int32_t) - BANK3_USER_VAR_LEN*sizeof(int32_t))
#define GLOBAL_PARAM_SAVE_TO_EEPROM_LEN	 (sizeof(GlobalParam_t) -  BANK2_USER_VAR_LEN*sizeof(int32_t))



/* ȫ�ֲ��� */

//ȫ�ֲ�����ʼ��
ErrorType_e Global_Param_Init(void);
//���EEPROM�����ģ�����
void ClearAndSave_Default_Global_Params(void);
//��� ģ�����
void Global_Param_SetDefault_Value(__IO GlobalParam_t *ptGlobalParam);
//���EEPROM�����ģ�����
void ClearAndSave_Default_Global_Params(void);
//������־λ
ErrorType_e Set_UpdateFlag(uint8_t ucFlag);

//ȫ�ֲ�����
ErrorType_e GlobalParam_Bank_0(ReadWrite_e eReadWrite, uint8_t ucType, Data4Byte_u *puData);
ErrorType_e GlobalParam_Bank_1(ReadWrite_e eReadWrite, uint8_t ucType, Data4Byte_u *puData);
ErrorType_e GlobalParam_Bank_2(ReadWrite_e eReadWrite, uint8_t ucType, Data4Byte_u *puData);
ErrorType_e GlobalParam_Bank_3(ReadWrite_e eReadWrite, uint8_t ucType, Data4Byte_u *puData);
ErrorType_e TMC_Global_Param(Bank_e eBank, ReadWrite_e eReadWrite, uint8_t ucType, Data4Byte_u *puData);

//
ErrorType_e GlobalParam_Set_CanID(uint8_t ucRecvCanID, uint8_t SendCanID);

/* �������� */
ErrorType_e Read_Global_Param(__IO GlobalParam_t *ptGlobalParam);
ErrorType_e Save_Global_Param(__IO GlobalParam_t *ptGlobalParam);






#endif // __PARAM_H__











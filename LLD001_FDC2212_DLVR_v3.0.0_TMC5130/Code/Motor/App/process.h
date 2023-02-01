#ifndef __PROCESS_H__
#define __PROCESS_H__

#ifdef __cplusplus
extern "C" {
#endif

//#include "bsp_can.h"
#include "main.h"
#include "public.h"
#include "project.h"
#include "TMC_Api.h"
#include "event.h"
#include "msg_handle.h"



/*
*	ģ���쳣����
*/
typedef enum {
	MODULE_ERROR_TYPE_MissStep	= 0,	//����

}ModuleErrorType_e;


/*
*	�����λ״̬
*/
typedef enum {
	MOTOR_RESET_STATUS_NONE		= 0,	//δ��λ,�ϵ��δִ�и�λ����
	MOTOR_RESET_STATUS_ING,				//��λ��
	MOTOR_RESET_STATUS_FINISH,			//��λ���
	MOTOR_RESET_STATUS_FAIL,			//��λʧ��

}MotorResetStatus_e;


/*
*	�����λ--ִ��״̬
*/
typedef enum {
	MOTOR_RESET_EXEC_1		= 0,	
	MOTOR_RESET_EXEC_2,				
	MOTOR_RESET_EXEC_3,	
	MOTOR_RESET_EXEC_4,		
	MOTOR_RESET_EXEC_5,				
	MOTOR_RESET_EXEC_6,	
	MOTOR_RESET_EXEC_7,		
	MOTOR_RESET_EXEC_8,				
	MOTOR_RESET_EXEC_9,	
	MOTOR_RESET_EXEC_10,		
	MOTOR_RESET_EXEC_11,				
	MOTOR_RESET_EXEC_12,	
	MOTOR_RESET_EXEC_13,		
	
}MotorResetExec_e;



/*
*	�����λ��Ϣ���ṹ��
*/
typedef __packed struct {
	
	uint8_t				ucResetMode;	//��ο��� or �Ҳο���
	MotorResetStatus_e  eResetStatus;	//���ԭ�㸴λ��ɱ�־λ��0��δ��λ��1����λ�У� 2����λ����ɣ� 3����λʧ��
	MotorResetExec_e    eResetExec;     //��λִ��״̬
	uint32_t 			ulStartTick;	//��λ��ʼtick
	
} MotorReset_Info_t;



///*
//*	Can��Ϣ����ͳ��
//*/
//typedef __packed struct {
//	
//	uint32_t ulSendSuccessNum;	//������Ϣ����
//	uint32_t ulSendFailNum;		//���ʹ���-��Ϣ����
//	uint32_t ulRecvSuccessNum;	//������Ϣ����
//	uint32_t ulRecvFailNum;		//����ʧ��-��Ϣ����
//	uint32_t ulRecvErrorNum;	//���ܴ���-��Ϣ����
//	uint32_t ulRecvOverNum;		//�����������
//	
//} CanMsgCount_Info_t;



/*
*	ȫ��״̬�ṹ��
*/
typedef __packed struct {
	
	uint8_t  ucEnableFlag[TMC_MODULE_END];	  //TMCʹ�ܱ�־��0��ʧ�ܣ�1��ʹ��
//	uint8_t  ucMotorMoveFlag[TMC_MODULE_END]; //����˶���־λ��ͨ���ñ�־���л���ʼ���ֵ��������б��ֵ���	
	
	uint8_t  ucErrStatus[TMC_MODULE_END];	  //TMCģ��״̬
	uint32_t ulBoardStatus; 				  //�忨״̬	
//	uint8_t  ucEEPRAM_Init_CRC_ErrFlag;	 	  //EPPROM״̬�� 0:������1����ʼ����ȡ�����쳣		 
	uint8_t  ucExecProcessStatus;			  //ִ������״̬
	
	uint8_t ucMotorResetStartFlag;			  //��λ������־�� 0���رգ�1������				
	MotorReset_Info_t  tMotorResetInfo[TMC_MODULE_END];	//��λ״̬��Ϣ

//	//CAN��Ϣͳ����Ϣ
//	CanMsgCount_Info_t tCanMsgCount_Info;
//	
//	//�忨ʶ����
//	uint16_t usSN;		//��MCU��UUID��crc16��У�飬�õ���ֵ��
	
} TMCStatus_t;









//ȫ�ֲ�����ʼ��
void Global_Status_Init(void);

//��ȡSN��
uint16_t Get_SN(uint8_t ucFlag);

//�Ĵ�����ʼ��
//void ShadowRegister_Init(AxisParamDefault_t *ptAxisParamDefault);

//MCU ����
void MCU_Reset(void);

//������ʼ��
void Reset_Factory(void);

//��ȡ��Ӳ���汾��
void Get_Soft_HardWare_Version(uint8_t *pucaData);

//��ȡģ������
uint32_t Get_Module_Type(void);

//�쳣���
void Period_Error_Check(uint32_t ulTick);

//ģ���쳣״̬����
ErrorType_e Module_Error_Handle(TMC_e eTMC, ModuleErrorType_e eType);

//��������
ErrorType_e MissStep_Handle(TMC_e eTMC);


//������Ϣ����
uint8_t Handle_RxMsg(MsgType_e eMsgType, RecvFrame_t *ptRecvFrame, SendFrame_t *ptSendFrame);


//�¼�����
void Event_Process(void);




//LED ��ҫ
void LED_Shine(uint16_t usCount, uint32_t ulTime);




/************************************************/
//���Թ���
#if MGIM_DEBUG

void Param_WR_Test(void);

#endif



  
#ifdef __cplusplus
}
#endif

#endif //__PROCESS_H__








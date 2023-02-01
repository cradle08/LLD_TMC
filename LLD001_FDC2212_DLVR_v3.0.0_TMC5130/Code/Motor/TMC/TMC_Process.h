#ifndef __TMC_PROCESS_H__
#define __TMC_PROCESS_H__

#include "main.h"
#include "project.h"
#include "public.h"
#include "TMC_Api.h"
#include "TMC5130.h"
//
#include "eeprom.h"
#include "process.h"
#include "bsp_can.h"
#include "msg_handle.h"
#include "param.h"
#include "TMC5130_Constants.h"

//Ĭ��Can ID
#define CAN_DEFAULT_RECV_ID		200		//Ĭ�Ͻ���CanID
#define CAN_DEFAULT_SEND_ID		200		//Ĭ�Ϸ���CanID

//����������������
#define SUB_PROCESS_MAX_PARAM_NUM	4

//һ���������֧���������
#define SUB_PROCESS_MAX_CMD_NUM		127


//
#define TMC_MODE_POSITION	TMC5160_MODE_POSITION
#define TMC_MODE_VELPOS		TMC5160_MODE_VELPOS   
#define TMC_MODE_VELNEG		TMC5160_MODE_VELNEG   
#define TMC_MODE_HOLD		TMC5160_MODE_HOLD     






/*
*	�豣��������--Ĭ��ֵ
*/
typedef __packed struct {	
	
	uint32_t    ulInitFlag;	 //��ʼ����־
	/*
	*  lEncConstValue = lFullSteppPerRound*usMicroStepResultion/usEncResultion/4 ==> lEncConstValue
	*/
	//������
	uint16_t usEncResultion[TMC_MODULE_END];	 //�������ֱ���
	int32_t	 lEncDiff_Threshold[TMC_MODULE_END]; //���������ʧ������ֵ
	uint8_t  ucEncCountDirect[TMC_MODULE_END];	 //��������������
	int32_t  lEncConstValue[TMC_MODULE_END];	 //����������--�����ȡ
	
	
	//�ο���-��λ����
	int32_t	lResetSpeedHigh[TMC_MODULE_END];	//��λ����	
	int32_t	lResetSpeedLow[TMC_MODULE_END];		//��λ����	
	int32_t lResetAcc[TMC_MODULE_END];			//��λ���ٶ�
	int32_t	lResetOff[TMC_MODULE_END];			//��λƫ�ƾ���
	
	//����
	uint8_t ucIRun[TMC_MODULE_END];		  //���е���
	uint8_t ucIHold[TMC_MODULE_END];	  //���ֵ���
	uint8_t ucIHoldDelay[TMC_MODULE_END]; //��������IHold����ʱ��

	//����ϸ�֡�ûתȫ����
	uint16_t   usMicroStepResultion[TMC_MODULE_END];	//΢��ϸ��
	uint16_t   usFullStepPerRound[TMC_MODULE_END];		//ȫ��ÿת�� ͨ�ò������Ϊ200 	


	//�����ٶȲ���������λ��ģʽ
	int32_t lVStart[TMC_MODULE_END];	//�����ٶ�
	int32_t lA1[TMC_MODULE_END];		//��һ�μ��ٶ�
	int32_t lV1[TMC_MODULE_END];		//�����ٶ�
	int32_t lAMax[TMC_MODULE_END];		//�ڶ��μ��ٶ�
	int32_t lVMax[TMC_MODULE_END];		//����ٶ�
	int32_t lDMax[TMC_MODULE_END];		//��һ�μ��ٶ�
	int32_t lD1[TMC_MODULE_END];		//�ڶ��μ��ٶ�
	int32_t lVStop[TMC_MODULE_END];		//ֹͣ�ٶ�
	

	//����������ģʽ
	uint8_t	ucMode[TMC_MODULE_END];		//0:λ��ģʽ��1��2�ٶ�ģʽ��3������ģʽ
	
	//�ٶȲ����������ٶ�ģʽ
	int32_t lAMax_VMode[TMC_MODULE_END];	//�ڶ��μ��ٶ�
	int32_t lVMax_VMode[TMC_MODULE_END];	//����ٶ�

	//�ο��㸴λ
	uint8_t ucRighLimitPolarity[TMC_MODULE_END]; //����λ���ԣ�����Ч
	uint8_t ucLeftLimitPolarity[TMC_MODULE_END]; //����λ���ԣ�����Ч
	uint8_t ucRotateDirect[TMC_MODULE_END];		 //��ת����
	
	//У��ֵ
	uint16_t 	usCrc;							//CRC16 У����

} AxisParamDefault_t;





/*
*	�Ƚϻ���ת  ����
*/
typedef enum 
{
	EN_COMPARE_JC_EQ = 0, 			//equal
	EN_COMPARE_JC_NE, 			//not equal
	EN_COMPARE_JC_GT, 			//greater
	EN_COMPARE_JC_GE,			//greater/equal
	EN_COMPARE_JC_LT,			//lower
	EN_COMPARE_JC_LE,			//lower/equal
//	EN_COMPARE_JC_ETO,			//time out error
//	EN_COMPARE_JC_EAL,			//external alarm
//	EN_COMPARE_JC_EDV,			//deviation error
//	EN_COMPARE_JC_EPO,			//position error
	
} CompareJC_e;




/*
*	��������  
*/
typedef enum 
{
	EN_CALC_ADD    	= 0,	//add
	EN_CALC_SUB, 			//subtract
	EN_CALC_MUL, 			//multiply
	EN_CALC_DIV, 			//divide
	EN_CALC_MOD, 			//modulo
	EN_CALC_AND,			//logical and
	EN_CALC_OR,				//logical or
	EN_CALC_XOR,			//logical exor
	EN_CALC_NOT,			//tlogical invert
	EN_CALC_RIGHT_LIFT,		//��λ��
	EN_CALC_LEFT_LIFT,		//��λ��
	
} Calc_e;





/*
*	����������ö��
*/
typedef enum 
{
	EN_PROCESS_INDEX_0    = 0,
	EN_PROCESS_INDEX_1, 
	EN_PROCESS_INDEX_2, 
	EN_PROCESS_INDEX_3, 	
	EN_PROCESS_INDEX_END, 
	
} ProcessIndex_e;



/*
*	����������ö��
*/
typedef enum 
{
	EN_SUB_PROCESS_TYPE_CMD    = 0,
	EN_SUB_PROCESS_TYPE_PARAM, 

} SubProcessType_e;





/*
*	��������״̬ö��
*/
typedef enum 
{
	EN_PROCESS_STATUS_READY    = 0,		//����
	EN_PROCESS_STATUS_EXEC, 			//����ִ��
	EN_PROCESS_STATUS_STOP, 			//ֹͣ
	EN_PROCESS_STATUS_CLEAR, 			//���
	EN_PROCESS_STATUS_END
	
} ProcessStatus_e;



/*
*	�����̶���, ָ������Ϊ�����ӿڣ�API���������������Ϊ��������������ĸ�������
*/
typedef __packed struct 
{
	uint8_t	ucCmd;									//ָ��
	uint8_t ucParamNum;								//��������
	Data4Byte_u uParam[SUB_PROCESS_MAX_PARAM_NUM];	//����
	
} SubProcess_t;



/*
*	���̶���
*/
typedef __packed struct 
{
	uint32_t 		ulInitFlag;								//��ʼ����־
	uint32_t		ulExecNum;								//�������������̣�ִ�еĴ����� 0����ʾ�����Σ�
	ProcessStatus_e eProcessStatus;							//����״̬
	uint8_t 		ucSubProcessNum;					    //��������Ч����
	SubProcess_t 	taSubProcess[SUB_PROCESS_MAX_CMD_NUM];  //����������
	uint16_t 		usCrc;									//CRC16 У����
	
} Process_t;



/*
*	���̿��ƣ���������Ϣ
*/
#define PROCESS_TIME_COUNT_NUM	4
typedef __packed struct 
{
	uint8_t 		ulStartFlag[PROCESS_TIME_COUNT_NUM];		//������־
	uint8_t 		ulTriggerFlag[PROCESS_TIME_COUNT_NUM];		//������־
	uint32_t		ulStartTick[PROCESS_TIME_COUNT_NUM];		//����ʱ��Tick
	uint32_t		ulThreshTick[PROCESS_TIME_COUNT_NUM];		//��ֵTick
	
} ProtcessTimeCount_t;





//�Ƕ�����λ
uint8_t Motor_Reset_Handle(uint32_t ulTick);

//���±���������(������)
ErrorType_e UpdateAndSave_Enc_ConstValue(TMC_e eTMC, __IO AxisParamDefault_t *ptAxisParamDefault);

//���±������������
ErrorType_e Update_Enc_ConstValue(TMC_e eTMC, __IO AxisParamDefault_t *ptAxisParamDefault, uint8_t ucValidFlag);

//���EEPROM����������
void ClearAndSave_Default_Axis_Params(void);

//���EEPROM���������
void ClearAndSave_Default_Process(void);

//�������ʼ��
ErrorType_e Axis_Param_Default_Init(void);
void Axis_Param_Fixed_SetDefault_Value(__IO AxisParamDefault_t *ptAxisParamDefault);

//�Ĵ�����ʼ��
void ShadowRegister_Init(__IO AxisParamDefault_t *ptAxisParamDefault);

/* ����� */
//����/��ȡ�����
ErrorType_e TMC_AxisParam(TMC_e eTMC, ReadWrite_e eReadWrite, uint8_t ucType, Data4Byte_u *puData);
ErrorType_e TMC_AxisParam_Default(TMC_e eTMC, ReadWrite_e eReadWrite, uint8_t ucType, Data4Byte_u *puData);
//���EEPROM����������
void ClearAndSave_Default_Axis_Params(void);
//
ErrorType_e Read_Axis_Param_Default(__IO AxisParamDefault_t *ptAxisParamDefault);
ErrorType_e Save_Axis_Param_Default(__IO AxisParamDefault_t *ptAxisParamDefault);



/* �������� */
ErrorType_e Read_Global_Param(__IO GlobalParam_t *ptGlobalParam);
ErrorType_e Save_Global_Param(__IO GlobalParam_t *ptGlobalParam);



/* �Զ������� */
//���̲�����ʼ��
void Process_Param_SetDefault_Value(__IO Process_t *ptProcess);
//���EEPROM���������
void ClearAndSave_Default_Process(void);
//���̳�ʼ��  
ErrorType_e Process_Init(void);

//��������
ErrorType_e Set_Process(RecvFrame_t *ptRecvFrame);
//��ȡ����
ErrorType_e Get_Process(uint8_t ucIndex, SubProcess_t *ptSendFrame);
//����--����
ErrorType_e Exec_Process_Ctrl(uint8_t ucType);
//���̶�д
ErrorType_e Read_Process(__IO Process_t *ptProcess);
ErrorType_e Save_Process(__IO Process_t *ptProccess);
void Clear_Process(void);
void Del_Process(void);
void Exec_Process(void);
void Stop_Process(void );

//ִ������--����Ϳ���
ErrorType_e Exec_Process_Clear_Or_Save(uint8_t ucType);
//ִ������--����
ErrorType_e Process_Handle(uint32_t ulTicks);






#endif //__TMC_PROCESS_H__















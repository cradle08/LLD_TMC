#include "rtthread.h"
#include "TMC_Process.h"
#include "TMC5130.h"
#include "TMC_Api.h"
#include "log.h"
#include "crc.h"

#include "msg_handle.h"
#include "TMC5130_Constants.h"
#include "bsp_gpio.h"
#include "bsp_can.h"


//�豣��������
__IO AxisParamDefault_t g_tAxisParamDefault = {0};
//����
__IO Process_t 		 g_tProcess = {0};
//����-��ʱ����Ϣ
__IO ProtcessTimeCount_t g_tProtcessTimeCount = {0};


/*
 * TMC Reset�� �Ƕ���ִ��
 */
#include "process.h"
#define MOTOR_RESET_TIMEOUT  90000 //90s
uint8_t Motor_Reset_Handle(uint32_t ulTick)
{
	extern __IO AxisParamDefault_t g_tAxisParamDefault;
	extern __IO TMCStatus_t g_tTMCStatus;
	static uint32_t s_ulTick = 0;
	static int32_t  s_lTrigglePos = 0;
	uint8_t  ucFlag = 0, ucOC_Status = 0;
	int32_t lSpeed = 0;
	
	//����Ҫִ�и�λ����
	if(g_tTMCStatus.ucMotorResetStartFlag == 0) return 0;
	
	//ÿ��1ms����һ��
	if(ulTick - s_ulTick >= 1)
	{
		s_ulTick = ulTick;
		for(TMC_e eTMC = TMC_0; eTMC < TMC_MODULE_END; eTMC++)
		{
			if(g_tTMCStatus.tMotorResetInfo[eTMC].eResetStatus == MOTOR_RESET_STATUS_ING)
			{
				/* ���ڸ�λ */
				ucFlag = 1;
				switch(g_tTMCStatus.tMotorResetInfo[eTMC].eResetExec)
				{
					//
					case MOTOR_RESET_EXEC_1:
					{
						//��λ��ʼtick
						g_tTMCStatus.tMotorResetInfo[eTMC].ulStartTick = ulTick;
						//�ٶ�Ϊ0
						TMC5160_WriteInt(eTMC, TMC5160_VMAX, 0);
						//���㸴λģʽ
						//TMC5160_WriteInt(eTMC, TMC5160_SWMODE, 0);
						
						//������״��
						ucOC_Status = TMC5160_FIELD_READ(eTMC, TMC5160_RAMPSTAT, TMC5160_RAMPSTAT_LEFT_SW_TRIGGER_MASK, TMC5160_RAMPSTAT_LEFT_SW_TRIGGER_SHIFT);

#ifdef P_AXIS_REF_POLITY						
						if(eTMC == TMC_0 || eTMC == TMC_1)
						{							
							if(ucOC_Status == 0)
							{
						         //�����޴���
								 TMC5160_WriteInt(eTMC, TMC5160_SWMODE, 0x0821); //Ϊ��ο��㣬��λ������Ч
								 g_tBoardStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_2;
								
							}else{
								//�����������λ,�л��������ٻ��ˡ�								
								g_tBoardStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_4;
							}
						}else{
							//TMC_2
							if(ucOC_Status == 1)
							{
						         //�����޴���
								TMC5160_WriteInt(eTMC, TMC5160_SWMODE, 0x0825); //ģ������Ϊ��ο��㣬��λ������Ч�� ģ��1��2������ʱΪ1������Ч��
								 g_tBoardStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_2;
								
							}else{
								//�����������λ���л��������ٻ��ˡ�
								g_tBoardStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_4;
							}
						}
#else
						if(ucOC_Status == 0)
						{
							 //�����޴���
							 //TMC5160_WriteInt(eTMC, TMC5160_SWMODE, 0x0821); //Ϊ��ο��㣬��λ������Ч
							
							//ʹ�����ο��㸴λ
							TMC5160_FIELD_UPDATE(eTMC, TMC5160_SWMODE, TMC5160_STATUS_STOP_L_MASK, TMC5160_STATUS_STOP_L_SHIFT, 1);
							g_tTMCStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_2;
							
						}else{
							//�����������λ,�л��������ٻ��ˡ�								
							g_tTMCStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_4;
						}		
						
#endif
					}
					break;
					/* ���ٻ��� */
					case MOTOR_RESET_EXEC_2:
					{
						TMC5160_WriteInt(eTMC, TMC5160_VMAX, 0); 
						TMC5160_WriteInt(eTMC, TMC5160_VMAX, g_tAxisParamDefault.lResetSpeedHigh[eTMC]*V_CHANGE_CONST);   // RESET_LOW_SPEED  RESET_HIGH_SPEED
						TMC5160_WriteInt(eTMC, TMC5160_AMAX ,g_tAxisParamDefault.lResetAcc[eTMC]); 		 //change AMax, so than  stop quickly
						TMC5160_WriteInt(eTMC, TMC5160_RAMPMODE, TMC5160_MODE_VELNEG); 					 //TMC5160_MODE_VELPOS   TMC5160_MODE_VELNEG
					
						//�л���3���ٶ��Ƿ�Ϊ0
						g_tTMCStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_3;
					}
					break;
					/* �ٶ��Ƿ�Ϊ0 */
					case MOTOR_RESET_EXEC_3:
					{					
						lSpeed = TMC5160_ReadInt(eTMC, TMC5160_VACTUAL);
						if(lSpeed == 0)
						{
							//�л���
							g_tTMCStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_4;
						}
					}
					break;				
					/* ������ת���� */
					case MOTOR_RESET_EXEC_4:
					{
						TMC5160_WriteInt(eTMC, TMC5160_VMAX, g_tAxisParamDefault.lResetSpeedLow[eTMC]*V_CHANGE_CONST);  		// RESET_LOW_SPEED  RESET_HIGH_SPEED
						//TMC5160_WriteInt(eTMC, TMC5160_AMAX ,RESET_LOW_AMAX); //
						TMC5160_WriteInt(eTMC, TMC5160_RAMPMODE, TMC5160_MODE_VELPOS);  //TMC5160_MODE_VELPOS   TMC5160_MODE_VELNEG
											
						//�л���
						g_tTMCStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_5;
					
					}
					break;
					/* ���ȴ�������� */
					case MOTOR_RESET_EXEC_5:
					{					
						//������״��
						ucOC_Status = TMC5160_FIELD_READ(eTMC, TMC5160_RAMPSTAT, TMC5160_RAMPSTAT_LEFT_SW_TRIGGER_MASK, TMC5160_RAMPSTAT_LEFT_SW_TRIGGER_SHIFT);
#ifdef P_AXIS_REF_POLITY					
						if(eTMC == TMC_0 || eTMC == TMC_1)
						{							
							if(ucOC_Status == 0)
							{
								//��¼��ǰλ��
								s_lTrigglePos = TMC5160_ReadInt(eTMC, TMC5160_XACTUAL);
						        //�����޴���(���������)�� 
								g_tBoardStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_6;
							}
						}else{
							//TMC_2
							if(ucOC_Status == 1)
							{
								//��¼��ǰλ��
								s_lTrigglePos = TMC5160_ReadInt(eTMC, TMC5160_XACTUAL);
						        //�����޴�������������
								g_tBoardStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_6;
							}
						}
#else
						if(ucOC_Status == 0)
						{
							#if 0  //ȥ���̶�λ�û��ˣ����裡������
								//��¼��ǰλ��
								s_lTrigglePos = TMC5160_ReadInt(eTMC, TMC5160_XACTUAL);
								//�����޴���(���������)�� 
								g_tBoardStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_6;
							#else
								TMC5160_WriteInt(eTMC, TMC5160_VMAX, 0);
								g_tTMCStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_7;
							#endif
						}
#endif
					}
					break;	
					/* ִ�й̶����� */
					case MOTOR_RESET_EXEC_6:
					{
						//
						int32_t lCurPos = TMC5160_ReadInt(eTMC, TMC5160_XACTUAL);
						if(lCurPos >= s_lTrigglePos + g_tAxisParamDefault.lResetOff[eTMC])
						{
							//ֹͣ
							TMC5160_WriteInt(eTMC, TMC5160_VMAX, 0); 
							//�л���
							g_tTMCStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_7;
						}
					}
					break;
					/* �ٶ��Ƿ�Ϊ0 */
					case MOTOR_RESET_EXEC_7:
					{
						lSpeed = TMC5160_ReadInt(eTMC, TMC5160_VACTUAL);
						if(lSpeed == 0)
						{
							//�л���
							g_tTMCStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_8;
						}
					}
					break;	
					/* ���ø�λģʽ */
					case MOTOR_RESET_EXEC_8:
					{	
#ifdef P_AXIS_REF_POLITY						
						if(eTMC == TMC_0 || eTMC == TMC_1)
						{				
							TMC5160_WriteInt(eTMC, TMC5160_SWMODE, 0x0821); //Ϊ��ο��㣬��λ������Ч							
						}else{
							//TMC_2
							TMC5160_WriteInt(eTMC, TMC5160_SWMODE, 0x0825); //ģ�����������ʱ0������Ч�� ģ��1��2������ʱΪ1������Ч��

						}
#else
						
//						uint32_t ulSWMode = 0;
//						ulSWMode = (0x0800 | (g_tAxisParamDefault.ucRighLimitPolarity[eTMC] << 3) | \
//									(g_tAxisParamDefault.ucLeftLimitPolarity[eTMC] << 2) | 1);
						
						//ʹ�����ο��㸴λ
						TMC5160_FIELD_UPDATE(eTMC, TMC5160_SWMODE, TMC5160_STATUS_STOP_L_MASK, TMC5160_STATUS_STOP_L_SHIFT, 1);					
						//TMC5160_WriteInt(eTMC, TMC5160_SWMODE, ulSWMode); //��λ��ʹ�ܺ�������ο���ֹͣ, �����棬�������ԡ� �����Ҳο��㼫�ԣ�����ʹ��	
						
#endif
						//�л���
						g_tTMCStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_9;
					}
					break;
					/* ���ٸ�λ */
					case MOTOR_RESET_EXEC_9:
					{
						TMC5160_WriteInt(eTMC, TMC5160_VMAX, g_tAxisParamDefault.lResetSpeedLow[eTMC]*V_CHANGE_CONST);  		// RESET_LOW_SPEED  RESET_HIGH_SPEED
						TMC5160_WriteInt(eTMC, TMC5160_RAMPMODE, TMC5160_MODE_VELNEG);  //TMC5160_MODE_VELPOS   TMC5160_MODE_VELNEG
					
						//�л���
						g_tTMCStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_10;
					}
					break;		
					/* �ٶ��Ƿ�Ϊ0 */					
					case MOTOR_RESET_EXEC_10:
					{					
						lSpeed = TMC5160_ReadInt(eTMC, TMC5160_VACTUAL);
						if(lSpeed == 0)
						{
							//�л���
							g_tTMCStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_11;
						}
					}
					break;
					/* �������� */
					case MOTOR_RESET_EXEC_11:
					{
						//ʵ��λ��
						int32_t lXActual = TMC5160_ReadInt(eTMC, TMC5160_XACTUAL);
						//get XLatch�� �����λ��
						int32_t lXLatch = TMC5160_ReadInt(eTMC, TMC5160_XLATCH);
						//ƫ��
						int32_t	lDiff = lXActual - lXLatch;
						
						//λ��ģʽ����������
						TMC5160_WriteInt(eTMC, TMC5160_XACTUAL, lDiff);
						TMC5160_WriteInt(eTMC, TMC5160_XTARGET, 0);
						TMC5160_WriteInt(eTMC, TMC5160_RAMPMODE, TMC_MODE_POSITION);
						TMC_SetPMode_V(eTMC, 1);
					
						//�л���
						g_tTMCStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_12;
					}
					break;		
					/* ������ɣ��ٶ�=0 */
					case MOTOR_RESET_EXEC_12:
					{
					
						lSpeed = TMC5160_ReadInt(eTMC, TMC5160_VACTUAL);
						if(lSpeed == 0)
						{
							//�л���
							g_tTMCStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_13;
						}
					}
					/* ��λ��ɣ����� */
					case MOTOR_RESET_EXEC_13:
					{
						//�ر����ο��㸴λ
						TMC5160_FIELD_UPDATE(eTMC, TMC5160_SWMODE, TMC5160_STATUS_STOP_L_MASK, TMC5160_STATUS_STOP_L_SHIFT, 0);
						
						//����Ϊ����ģʽ
						//TMC5160_WriteInt(eTMC, TMC5160_RAMPMODE, TMC_MODE_HOLD);
						
						//����
						TMC5160_WriteInt(eTMC, TMC5160_XTARGET, 0);
						TMC5160_WriteInt(eTMC, TMC5160_XACTUAL, 0);						
						TMC5160_WriteInt(eTMC, TMC5160_XENC, 0);
						//�ָ��ٶ�����
						TMC_SetPMode_V(eTMC, 1);
						g_tTMCStatus.tMotorResetInfo[eTMC].eResetStatus = MOTOR_RESET_STATUS_FINISH;
						
						//����ģʽ--λ��ģʽ
						TMC5160_WriteInt(eTMC, TMC5160_RAMPMODE, TMC_MODE_POSITION); //
						//�ٴν�����������
						TMC5160_WriteInt(eTMC, TMC5160_XENC, 0);
					}
					break;						
				}		
				
				//��λ��ʱ���
				if(ulTick - g_tTMCStatus.tMotorResetInfo[eTMC].ulStartTick >= MOTOR_RESET_TIMEOUT) 
				{
					//��ʱ
					g_tTMCStatus.tMotorResetInfo[eTMC].eResetStatus = MOTOR_RESET_STATUS_FAIL;
					LOG_Error("Motor=%d Reset Timeout", eTMC);
					return 0;
				}
			}
		}
		
		//���и�λ��ִ�����
		if(ucFlag == 0)
		{
			g_tTMCStatus.ucMotorResetStartFlag = 0;
		}
	}
	return 0;;
}





/*
*	���±���������, ucValidFlag:1=д��������Ĵ���������������Ч�� 0=��д�룬����Ч
*/
ErrorType_e Update_Enc_ConstValue(TMC_e eTMC, __IO AxisParamDefault_t *ptAxisParamDefault, uint8_t ucValidFlag)
{	
	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
	
	//����������
	float fMid = (float)((ptAxisParamDefault->usFullStepPerRound[eTMC]) * ptAxisParamDefault->usMicroStepResultion[eTMC])/(float)(ptAxisParamDefault->usEncResultion[eTMC] * (int32_t)4); 
	//����������--��������
	int32_t lInt = (int32_t)fMid;
	//����������--С������
	float fDecimal = fMid - lInt;

	//
	//LOG_Info("Update Enc Const: eTMC=%d. FullStepPerRound=%d, MicroStepResultion=%d, EncResultion=%d, EncCountDirect=%d", \
			 eTMC, ptAxisParamDefault->usFullStepPerRound[eTMC], ptAxisParamDefault->usMicroStepResultion[eTMC], \
			 ptAxisParamDefault->usEncResultion[eTMC], ptAxisParamDefault->ucEncCountDirect[eTMC]);
	
	//�������������
	if(0 == g_tAxisParamDefault.ucEncCountDirect[eTMC])
	{
		//����
		ptAxisParamDefault->lEncConstValue[eTMC] = lInt*65536+ (int32_t)(fDecimal*10000);
		
	}else if(1 == g_tAxisParamDefault.ucEncCountDirect[eTMC]){
		//����		
		ptAxisParamDefault->lEncConstValue[eTMC] = (-(lInt+1))*65536+ (int32_t)(10000-(fDecimal*10000));
	}
	
	//
	//LOG_Info("EncConst %d", ptAxisParamDefault->lEncConstValue[eTMC]);
	if(1 == ucValidFlag)
	{
		eErrorType = TMC_WriteInt(eTMC, TMC5160_ENC_CONST, g_tAxisParamDefault.lEncConstValue[eTMC]);
	}
	return eErrorType;
}






/*
*	���²��������������
*/
ErrorType_e UpdateAndSave_Enc_ConstValue(TMC_e eTMC, __IO AxisParamDefault_t *ptAxisParamDefault)
{
	//���±���������
	Update_Enc_ConstValue(eTMC, ptAxisParamDefault, 0);
	
	//���¼���CRC
	ptAxisParamDefault->usCrc = CRC16((uint8_t*)ptAxisParamDefault, sizeof(AxisParamDefault_t)-2);
	
	//���������
	return Save_Axis_Param_Default(ptAxisParamDefault);
	
}






/*
*	�������ʼ��
*/
void Axis_Param_Fixed_SetDefault_Value(__IO AxisParamDefault_t *ptAxisParamDefault)
{
	TMC_e eTMC = TMC_0;
	
	//��һ���ϵ磬��ʼ��������
	ptAxisParamDefault->ulInitFlag = PARAM_INIT_FLAG;
	
#if (CURRENT_MODULE_TYPE==MODULE_TYPE_TMC_STEP_MOTOR_1301_Common) || (CURRENT_MODULE_TYPE==MODULE_TYPE_TMC_STEP_MOTOR_1161) 
	/* ���� */		
	
	//���������
	ptAxisParamDefault->usEncResultion[eTMC]	  = DEFAULT_ENC_RESULTION_1000;   	//�������ֱ���
	ptAxisParamDefault->lEncDiff_Threshold[eTMC]  = 100;					    //���������ʧ������ֵ
	ptAxisParamDefault->ucEncCountDirect[eTMC]    = 0;					   		//��������������
	ptAxisParamDefault->lEncConstValue[eTMC] 	  = DEFAULT_ENC_CONST_VALUE_TMC_0;  //����������--�����ȡ


	//�ο���-��λ����
	ptAxisParamDefault->lResetSpeedHigh[eTMC] 	= 3500;  //��λ�ٶ�
	ptAxisParamDefault->lResetSpeedLow[eTMC]  	= 1500;	 //��λ�ٶ�  
	ptAxisParamDefault->lResetAcc[eTMC]      	= 500;   //��λ���ٶ�
	ptAxisParamDefault->lResetOff[eTMC]  		= 200;	 //��λƫ�ƾ���

	//����
	ptAxisParamDefault->ucIRun[eTMC]	   = 65; //���е���
	ptAxisParamDefault->ucIHold[eTMC]	   = 8;  //���ֵ���
	ptAxisParamDefault->ucIHoldDelay[eTMC] = 6;   //��������IHold����ʱ��	

	//����ϸ�֡�ûתȫ����
	ptAxisParamDefault->usMicroStepResultion[eTMC] = 64;//DEFAULT_MICRO_STEP_RESULTION; 
	ptAxisParamDefault->usFullStepPerRound[eTMC]   = DEFAULT_FULL_STEP_PER_ROUND; //ȫ��ÿת


	//�����ٶȲ���������λ��ģʽ
	ptAxisParamDefault->lVStart[eTMC] = 0;
	ptAxisParamDefault->lA1[eTMC] 	  = 500000; 
	ptAxisParamDefault->lV1[eTMC] 	  = 40000;
	ptAxisParamDefault->lAMax[eTMC]   = 300000; 
	ptAxisParamDefault->lVMax[eTMC]   = 65000;
	ptAxisParamDefault->lDMax[eTMC]   = 300000; 
	ptAxisParamDefault->lD1[eTMC] 	  = 500000; 
	ptAxisParamDefault->lVStop[eTMC]  = 10;
	//����������ģʽ������λ��ģʽ
	ptAxisParamDefault->ucMode[eTMC] = TMC_MODE_POSITION;
	//�ٶȲ����������ٶ�ģʽ
	ptAxisParamDefault->lAMax_VMode[eTMC] = ptAxisParamDefault->lAMax[eTMC];	//�ڶ��μ��ٶ�
	ptAxisParamDefault->lVMax_VMode[eTMC] = ptAxisParamDefault->lVMax[eTMC];	//����ٶ�

	//�ο��㸴λ
	ptAxisParamDefault->ucRighLimitPolarity[eTMC] = 0; //����λ���ԣ�����Ч
	ptAxisParamDefault->ucLeftLimitPolarity[eTMC] = 0; //����λ���ԣ�����Ч
	ptAxisParamDefault->ucRotateDirect[eTMC]	  = 0; //��ת����		
			
#elif (CURRENT_MODULE_TYPE==MODULE_TYPE_TMC_STEP_MOTOR_3301_Pipette)
	/* ���� ��Һ�� */	
	for(eTMC = TMC_0; eTMC < TMC_MODULE_END; eTMC++)
	{			
		if(TMC_0 == eTMC)
		{
			/* ���� M0, (Z��)*/
			
			//���������
			ptAxisParamDefault->usEncResultion[eTMC]	 = DEFAULT_ENC_RESULTION_720;     //�������ֱ���
			ptAxisParamDefault->lEncDiff_Threshold[eTMC] = 100;					   	  //���������ʧ������ֵ
			ptAxisParamDefault->ucEncCountDirect[eTMC]   = 0;					   	  //��������������
			ptAxisParamDefault->lEncConstValue[eTMC] 	 = DEFAULT_ENC_CONST_VALUE_Z; //����������--�����ȡ			
			
			//�ο���-��λ����
			ptAxisParamDefault->lResetSpeedHigh[eTMC] = 15000; //��λ�ٶ�
			ptAxisParamDefault->lResetSpeedLow[eTMC]  = 1000;  //��λ�ٶ�  
			ptAxisParamDefault->lResetAcc[eTMC]       = 500;   //��λ���ٶ�
			ptAxisParamDefault->lResetOff[eTMC]  	  = 800;   //��λƫ�ƾ���

			//����
			ptAxisParamDefault->ucIRun[eTMC]	   = 128; //���е���
			ptAxisParamDefault->ucIHold[eTMC]	   = 8; //���ֵ���
			ptAxisParamDefault->ucIHoldDelay[eTMC] = 6;  //��������IHold����ʱ��	
			
			//����ϸ�֡�ûתȫ����
			ptAxisParamDefault->usMicroStepResultion[eTMC] = 64;//DEFAULT_MICRO_STEP_RESULTION; 
			ptAxisParamDefault->usFullStepPerRound[eTMC]   = DEFAULT_FULL_STEP_PER_ROUND; //ȫ��ÿת
			
			//�����ٶȲ���������λ��ģʽ
			ptAxisParamDefault->lVStart[eTMC] = 0;
			ptAxisParamDefault->lA1[eTMC] 	 = 900000; 
			ptAxisParamDefault->lV1[eTMC] 	 = 150000;
			ptAxisParamDefault->lAMax[eTMC]  = 1200000; 
			ptAxisParamDefault->lVMax[eTMC]  = 243200;
			ptAxisParamDefault->lDMax[eTMC]  = 1200000; 
			ptAxisParamDefault->lD1[eTMC] 	 = 900000; 
			ptAxisParamDefault->lVStop[eTMC] = 10;
			//����������ģʽ
			ptAxisParamDefault->ucMode[eTMC] = TMC_MODE_POSITION;
			//�ٶȲ����������ٶ�ģʽ
			ptAxisParamDefault->lAMax_VMode[eTMC] = ptAxisParamDefault->lAMax[eTMC];	//�ڶ��μ��ٶ�
			ptAxisParamDefault->lVMax_VMode[eTMC] = ptAxisParamDefault->lVMax[eTMC];	//����ٶ�
	
			//�ο��㸴λ
			ptAxisParamDefault->ucRighLimitPolarity[eTMC] = 0; //����λ���ԣ�����Ч
			ptAxisParamDefault->ucLeftLimitPolarity[eTMC] = 0; //����λ���ԣ�����Ч
			ptAxisParamDefault->ucRotateDirect[eTMC]	  = 0; //��ת����
			
		}else if(TMC_1 == eTMC){
			/* ���� M1, (Y��)*/		
			//���������
			ptAxisParamDefault->usEncResultion[eTMC]	 = DEFAULT_ENC_RESULTION_1000;   //�������ֱ���
			ptAxisParamDefault->lEncDiff_Threshold[eTMC] = 100;					   //���������ʧ������ֵ
			ptAxisParamDefault->ucEncCountDirect[eTMC]   = 0;					   //��������������
			ptAxisParamDefault->lEncConstValue[eTMC] 	 = DEFAULT_ENC_CONST_VALUE_Y; //����������--�����ȡ
		
			//�ο���-��λ����
			ptAxisParamDefault->lResetSpeedHigh[eTMC] = 5000; //��λ�ٶ�
			ptAxisParamDefault->lResetSpeedLow[eTMC]  = 1000; //��λ�ٶ� 
			ptAxisParamDefault->lResetAcc[eTMC]       = 500;  //��λ���ٶ�
			ptAxisParamDefault->lResetOff[eTMC]  	  = 400;  //��λƫ�ƾ���
			ptAxisParamDefault->lEncDiff_Threshold[eTMC] = 100;	//���������ʧ������ֵ
			
			//����
			ptAxisParamDefault->ucIRun[eTMC]	   = 128; //���е���
			ptAxisParamDefault->ucIHold[eTMC]	   = 8;  //���ֵ���
			ptAxisParamDefault->ucIHoldDelay[eTMC] = 6;   //��������IHold����ʱ��
			
			//����ϸ�֡�ûתȫ����
			ptAxisParamDefault->usMicroStepResultion[eTMC] = 64; //Ĭ��ϸ��
			ptAxisParamDefault->usFullStepPerRound[eTMC]   = DEFAULT_FULL_STEP_PER_ROUND; //ȫ��ÿת			
			
			//�����ٶȲ���������λ��ģʽ
			ptAxisParamDefault->lVStart[eTMC] = 0;
			ptAxisParamDefault->lA1[eTMC] 	 = 900000; 
			ptAxisParamDefault->lV1[eTMC] 	 = 80000;
			ptAxisParamDefault->lAMax[eTMC]  = 600000;
			ptAxisParamDefault->lVMax[eTMC]  = 128000;
			ptAxisParamDefault->lDMax[eTMC]  = 600000;
			ptAxisParamDefault->lD1[eTMC] 	 = 800000;
			ptAxisParamDefault->lVStop[eTMC] = 10;
			//����������ģʽ
			ptAxisParamDefault->ucMode[eTMC] = TMC_MODE_POSITION;
			//�ٶȲ����������ٶ�ģʽ
			ptAxisParamDefault->lAMax_VMode[eTMC] = ptAxisParamDefault->lAMax[eTMC];	//�ڶ��μ��ٶ�
			ptAxisParamDefault->lVMax_VMode[eTMC] = ptAxisParamDefault->lVMax[eTMC];	//����ٶ�
			
			//�ο��㸴λ
			ptAxisParamDefault->ucRighLimitPolarity[eTMC] = 0; //����λ���ԣ�����Ч
			ptAxisParamDefault->ucLeftLimitPolarity[eTMC] = 0; //����λ���ԣ�����Ч
			ptAxisParamDefault->ucRotateDirect[eTMC]	  = 0; //��ת����
				
		}else if(TMC_2 == eTMC){	
			/* ���� M1, (P��) */
			//���������
			ptAxisParamDefault->usEncResultion[eTMC]	 = DEFAULT_ENC_RESULTION_1000;     //�������ֱ���
			ptAxisParamDefault->lEncDiff_Threshold[eTMC] = 100;					      //���������ʧ������ֵ
			ptAxisParamDefault->ucEncCountDirect[eTMC]   = 0;					      //��������������
			ptAxisParamDefault->lEncConstValue[eTMC] 	 = DEFAULT_ENC_CONST_VALUE_P; //����������--�����ȡ
		
			//�ο���-��λ����
			ptAxisParamDefault->lResetSpeedHigh[eTMC] 	 = 3500; //��λ�ٶ�
			ptAxisParamDefault->lResetSpeedLow[eTMC]  	 = 1500; //��λ�ٶ�  
			ptAxisParamDefault->lResetAcc[eTMC]      	 = 800;	 //��λ���ٶ�
			ptAxisParamDefault->lResetOff[eTMC]  		 = 3200; //��λƫ�ƾ���			
			ptAxisParamDefault->lEncDiff_Threshold[eTMC] = 0;	 //���������ʧ������ֵ	
			
			//����
			ptAxisParamDefault->ucIRun[eTMC]	   = 128; //���е���
			ptAxisParamDefault->ucIHold[eTMC]	   = 8; //���ֵ���
			ptAxisParamDefault->ucIHoldDelay[eTMC] = 6;  //��������IHold����ʱ��
						
			//�����ٶȲ���,����λ��ģʽ
			ptAxisParamDefault->lVStart[eTMC] = 0;
			ptAxisParamDefault->lA1[eTMC] 	  = 500000; 
			ptAxisParamDefault->lV1[eTMC] 	  = 40000;
			ptAxisParamDefault->lAMax[eTMC]   = 300000;
			ptAxisParamDefault->lVMax[eTMC]   = 65000;
			ptAxisParamDefault->lDMax[eTMC]   = 300000;
			ptAxisParamDefault->lD1[eTMC] 	  = 500000;
			ptAxisParamDefault->lVStop[eTMC]  = 10;
			//����������ģʽ
			ptAxisParamDefault->ucMode[eTMC] = TMC_MODE_POSITION;
			//�ٶȲ����������ٶ�ģʽ
			ptAxisParamDefault->lAMax_VMode[eTMC] = ptAxisParamDefault->lAMax[eTMC];	//�ڶ��μ��ٶ�
			ptAxisParamDefault->lVMax_VMode[eTMC] = ptAxisParamDefault->lVMax[eTMC];	//����ٶ�
			
			//����ϸ�֡�ûתȫ����
			ptAxisParamDefault->usMicroStepResultion[eTMC] = 64; //Ĭ��ϸ��
			ptAxisParamDefault->usFullStepPerRound[eTMC]   = DEFAULT_FULL_STEP_PER_ROUND;  //ȫ��ÿת
			
			//�ο��㸴λ
			ptAxisParamDefault->ucRighLimitPolarity[eTMC] = 0; //����λ���ԣ�����Ч
			ptAxisParamDefault->ucLeftLimitPolarity[eTMC] = 0; //����λ���ԣ�����Ч
			ptAxisParamDefault->ucRotateDirect[eTMC]	  = 0; //��ת����
		}
	}
	
#elif (CURRENT_MODULE_TYPE==MODULE_TYPE_TMC_STEP_MOTOR_3311_Common) 	
	/* ���� ͨ�ð� */	
	for(eTMC = TMC_0; eTMC < TMC_MODULE_END; eTMC++)
	{			
		if(TMC_0 == eTMC)
		{
			/* ���� M0 */
			
			//���������
			ptAxisParamDefault->usEncResultion[eTMC]	 = DEFAULT_ENC_RESULTION_1000;     //�������ֱ���
			ptAxisParamDefault->lEncDiff_Threshold[eTMC] = 100;					   	  //���������ʧ������ֵ
			ptAxisParamDefault->ucEncCountDirect[eTMC]   = 0;					   	  //��������������
			ptAxisParamDefault->lEncConstValue[eTMC] 	 = DEFAULT_ENC_CONST_VALUE_TMC_0; //����������--�����ȡ			
			
			//�ο���-��λ����
			ptAxisParamDefault->lResetSpeedHigh[eTMC] = 3500; //��λ�ٶ�
			ptAxisParamDefault->lResetSpeedLow[eTMC]  = 100;  //��λ�ٶ�  
			ptAxisParamDefault->lResetAcc[eTMC]       = 500;   //��λ���ٶ�
			ptAxisParamDefault->lResetOff[eTMC]  	  = 200;   //��λƫ�ƾ���

			//����
			ptAxisParamDefault->ucIRun[eTMC]	   = 128; //���е���
			ptAxisParamDefault->ucIHold[eTMC]	   = 8; //���ֵ���
			ptAxisParamDefault->ucIHoldDelay[eTMC] = 6;  //��������IHold����ʱ��	
			
			//����ϸ�֡�ûתȫ����
			ptAxisParamDefault->usMicroStepResultion[eTMC] = 64;//DEFAULT_MICRO_STEP_RESULTION; 
			ptAxisParamDefault->usFullStepPerRound[eTMC]   = DEFAULT_FULL_STEP_PER_ROUND; //ȫ��ÿת
			
			
			//�����ٶȲ���������λ��ģʽ
			ptAxisParamDefault->lVStart[eTMC] = 0;
			ptAxisParamDefault->lA1[eTMC] 	 = 200000; 
			ptAxisParamDefault->lV1[eTMC] 	 = 35000;
			ptAxisParamDefault->lAMax[eTMC]  = 95000; 
			ptAxisParamDefault->lVMax[eTMC]  = 61000;
			ptAxisParamDefault->lDMax[eTMC]  = 95000; 
			ptAxisParamDefault->lD1[eTMC] 	 = 200000; 
			ptAxisParamDefault->lVStop[eTMC] = 10;
			//����������ģʽ
			ptAxisParamDefault->ucMode[eTMC] = TMC_MODE_POSITION;
			//�ٶȲ����������ٶ�ģʽ
			ptAxisParamDefault->lAMax_VMode[eTMC] = ptAxisParamDefault->lAMax[eTMC];	//�ڶ��μ��ٶ�
			ptAxisParamDefault->lVMax_VMode[eTMC] = ptAxisParamDefault->lVMax[eTMC];	//����ٶ�
			
			//�ο��㸴λ
			ptAxisParamDefault->ucRighLimitPolarity[eTMC] = 0; //����λ���ԣ�����Ч
			ptAxisParamDefault->ucLeftLimitPolarity[eTMC] = 0; //����λ���ԣ�����Ч
			ptAxisParamDefault->ucRotateDirect[eTMC]	  = 0; //��ת����
			
		}else if(TMC_1 == eTMC){
			/* ���� M1, (Y��)*/		
			//���������
			ptAxisParamDefault->usEncResultion[eTMC]	 = DEFAULT_ENC_RESULTION_1000;   //�������ֱ���
			ptAxisParamDefault->lEncDiff_Threshold[eTMC] = 100;					   //���������ʧ������ֵ
			ptAxisParamDefault->ucEncCountDirect[eTMC]   = 0;					   //��������������
			ptAxisParamDefault->lEncConstValue[eTMC] 	 = DEFAULT_ENC_CONST_VALUE_TMC_1; //����������--�����ȡ
		
			//�ο���-��λ����
			ptAxisParamDefault->lResetSpeedHigh[eTMC] = 35000; //��λ�ٶ�
			ptAxisParamDefault->lResetSpeedLow[eTMC]  = 1000; //��λ�ٶ� 
			ptAxisParamDefault->lResetAcc[eTMC]       = 500;  //��λ���ٶ�
			ptAxisParamDefault->lResetOff[eTMC]  	  = 400;  //��λƫ�ƾ���
			ptAxisParamDefault->lEncDiff_Threshold[eTMC] = 100;	//���������ʧ������ֵ
			
			//����
			ptAxisParamDefault->ucIRun[eTMC]	   = 128; //���е���
			ptAxisParamDefault->ucIHold[eTMC]	   = 8;  //���ֵ���
			ptAxisParamDefault->ucIHoldDelay[eTMC] = 6;   //��������IHold����ʱ��
			
			//����ϸ�֡�ûתȫ����
			ptAxisParamDefault->usMicroStepResultion[eTMC] = 64; //Ĭ��ϸ��
			ptAxisParamDefault->usFullStepPerRound[eTMC]   = DEFAULT_FULL_STEP_PER_ROUND; //ȫ��ÿת			
			
			//�����ٶȲ���������λ��ģʽ
			ptAxisParamDefault->lVStart[eTMC] = 0;
			ptAxisParamDefault->lA1[eTMC] 	 = 900000; 
			ptAxisParamDefault->lV1[eTMC] 	 = 80000;
			ptAxisParamDefault->lAMax[eTMC]  = 600000;
			ptAxisParamDefault->lVMax[eTMC]  = 128000;
			ptAxisParamDefault->lDMax[eTMC]  = 600000;
			ptAxisParamDefault->lD1[eTMC] 	 = 800000;
			ptAxisParamDefault->lVStop[eTMC] = 10;
			//����������ģʽ
			ptAxisParamDefault->ucMode[eTMC] = TMC_MODE_POSITION;
			//�ٶȲ����������ٶ�ģʽ
			ptAxisParamDefault->lAMax_VMode[eTMC] = ptAxisParamDefault->lAMax[eTMC];	//�ڶ��μ��ٶ�
			ptAxisParamDefault->lVMax_VMode[eTMC] = ptAxisParamDefault->lVMax[eTMC];	//����ٶ�
			
			//�ο��㸴λ
			ptAxisParamDefault->ucRighLimitPolarity[eTMC] = 0; //����λ���ԣ�����Ч
			ptAxisParamDefault->ucLeftLimitPolarity[eTMC] = 0; //����λ���ԣ�����Ч
			ptAxisParamDefault->ucRotateDirect[eTMC]	  = 0; //��ת����
				
		}else if(TMC_2 == eTMC){	
			/* ���� M1, (P��) */
			//���������
			ptAxisParamDefault->usEncResultion[eTMC]	 = DEFAULT_ENC_RESULTION_1000;     //�������ֱ���
			ptAxisParamDefault->lEncDiff_Threshold[eTMC] = 100;					      //���������ʧ������ֵ
			ptAxisParamDefault->ucEncCountDirect[eTMC]   = 0;					      //��������������
			ptAxisParamDefault->lEncConstValue[eTMC] 	 = DEFAULT_ENC_CONST_VALUE_TMC_2; //����������--�����ȡ
		
			//�ο���-��λ����
			ptAxisParamDefault->lResetSpeedHigh[eTMC] 	 = 3500; //��λ�ٶ�
			ptAxisParamDefault->lResetSpeedLow[eTMC]  	 = 1500; //��λ�ٶ�  
			ptAxisParamDefault->lResetAcc[eTMC]      	 = 800;	 //��λ���ٶ�
			ptAxisParamDefault->lResetOff[eTMC]  		 = 3200; //��λƫ�ƾ���			
			ptAxisParamDefault->lEncDiff_Threshold[eTMC] = 0;	 //���������ʧ������ֵ	
			
			//����
			ptAxisParamDefault->ucIRun[eTMC]	   = 128; //���е���
			ptAxisParamDefault->ucIHold[eTMC]	   = 8; //���ֵ���
			ptAxisParamDefault->ucIHoldDelay[eTMC] = 6;  //��������IHold����ʱ��
						
			//�����ٶȲ���������λ��ģʽ
			ptAxisParamDefault->lVStart[eTMC] = 0;
			ptAxisParamDefault->lA1[eTMC] 	  = 500000; 
			ptAxisParamDefault->lV1[eTMC] 	  = 40000;
			ptAxisParamDefault->lAMax[eTMC]   = 300000;
			ptAxisParamDefault->lVMax[eTMC]   = 65000;
			ptAxisParamDefault->lDMax[eTMC]   = 300000;
			ptAxisParamDefault->lD1[eTMC] 	  = 500000;
			ptAxisParamDefault->lVStop[eTMC]  = 10;
			//����������ģʽ
			ptAxisParamDefault->ucMode[eTMC] = TMC_MODE_POSITION;
			//�ٶȲ����������ٶ�ģʽ
			ptAxisParamDefault->lAMax_VMode[eTMC] = ptAxisParamDefault->lAMax[eTMC];	//�ڶ��μ��ٶ�
			ptAxisParamDefault->lVMax_VMode[eTMC] = ptAxisParamDefault->lVMax[eTMC];	//����ٶ�
			
			//����ϸ�֡�ûתȫ����
			ptAxisParamDefault->usMicroStepResultion[eTMC] = 64; //Ĭ��ϸ��
			ptAxisParamDefault->usFullStepPerRound[eTMC]   = DEFAULT_FULL_STEP_PER_ROUND;  //ȫ��ÿת
			
			//�ο��㸴λ
			ptAxisParamDefault->ucRighLimitPolarity[eTMC] = 0; //����λ���ԣ�����Ч
			ptAxisParamDefault->ucLeftLimitPolarity[eTMC] = 0; //����λ���ԣ�����Ч
			ptAxisParamDefault->ucRotateDirect[eTMC]	  = 0; //��ת����
		}
	}

#endif
	
	ptAxisParamDefault->usCrc = CRC16((uint8_t*)ptAxisParamDefault, sizeof(AxisParamDefault_t)-2);
}





/*
*	�������ʼ��
*/
ErrorType_e Axis_Param_Default_Init(void)
{
	return Read_Axis_Param_Default(&g_tAxisParamDefault);
}




//�Ĵ�����ʼ��
void ShadowRegister_Init(__IO AxisParamDefault_t *ptAxisParamDefault)
{
	extern TMC5160_t g_taTMC5160[TMC_MODULE_END];
	TMC_e eTMC = TMC_0;
	
	//
	for(eTMC = TMC_0; eTMC < TMC_MODULE_END; eTMC++)
	{
		g_taTMC5160[eTMC].laShadowRegister[TMC5160_ENC_CONST] = ptAxisParamDefault->lEncConstValue[eTMC];
		g_taTMC5160[eTMC].laShadowRegister[TMC5160_ENC_DEVIATION] = ptAxisParamDefault->lEncDiff_Threshold[eTMC];
		//@todo other param need copy
	}
}








///*
//*	�����
//*/
//ErrorType_e TMC_AxisParam(TMC_e eTMC, ReadWrite_e eReadWrite, uint8_t ucType, Data4Byte_u *puData)
//{
//	ErrorType_e eError = ERROR_TYPE_SUCCESS;
//	
//	//�������
//	if(eTMC >= TMC_END)
//	{
//		LOG_Error("TMC DeviceID=%d Is Error", eTMC);
//		return ERROR_TYPE_DEVICE_ID;
//	}
//	
//	switch(eTMC)
//	{
//		case TMC_0:
//		case TMC_1:
//		case TMC_2:
//		case TMC_3:
//		case TMC_4:
//		case TMC_5:
//		{
//			eError = TMC5160_AxisParam(eTMC, eReadWrite, ucType, puData);
//		}
//		break;
//		default:break;
//	}
//	return eError;
//}









/*
*	���������
*/
ErrorType_e TMC_AxisParam(TMC_e eTMC, ReadWrite_e eReadWrite, uint8_t ucType, Data4Byte_u *puData)
//ErrorType_e TMC5160_AxisParam(TMC_e eTMC, ReadWrite_e eReadWrite, uint8_t ucType, Data4Byte_u *puData)
{
//	extern Process_t g_tProcess;
	extern TMC5160_t g_taTMC5160[TMC_MODULE_END];
	extern __IO TMCStatus_t g_tTMCStatus;
	extern __IO  AxisParamDefault_t g_tAxisParamDefault;
	
//	uint32_t buffer;
	ErrorType_e eError = ERROR_TYPE_SUCCESS;

	
	//�������
	if(eTMC >= TMC_END)
	{
		LOG_Error("TMC DeviceID=%d Is Error", eTMC);
		return ERROR_TYPE_DEVICE_ID;
	}
	

	switch(ucType)
	{
	/*** λ�����  ***/
	case 0x00:
		// Ŀ��λ�ã�Target position
		if(eReadWrite == TMC_READ) {
			puData->lData = TMC5160_ReadInt(eTMC, TMC5160_XTARGET);
		} else if(eReadWrite == TMC_WRITE) {
			eError = TMC5160_WriteInt(eTMC, TMC5160_XTARGET, puData->lData);
		}
		break;
	case 0x01:
		// ʵ��λ�ã�Actual position
		if(eReadWrite == TMC_READ) {
			puData->lData = TMC5160_ReadInt(eTMC, TMC5160_XACTUAL);
		} else if(eReadWrite == TMC_WRITE) {
			eError = TMC5160_WriteInt(eTMC, TMC5160_XACTUAL, puData->lData);
		}
		break;
		
	case 0x02:
		// �������λ�ñ�־��RO
		if(eReadWrite == TMC_READ) {
			puData->lData = TMC5160_FIELD_READ(eTMC, TMC5160_RAMPSTAT, TMC5160_RAMPSTAT_POS_REACH_MASK, TMC5160_RAMPSTAT_POS_REACH_SHIFT);
			//LOG_Info("RAMP_STAT=%X", TMC_ReadInt(eTMC, TMC5160_RAMPSTAT));
		} else if(eReadWrite == TMC_WRITE){
			LOG_Error("Read Max Postion Do not Be Write");
			return ERROR_TYPE_RW_RIGHT;
		}
		break;
			
		
	/*** ������ ***/
	case 0x03:
		//������ֵ�� Encoder position
		if(eReadWrite == TMC_READ) {
			puData->lData = TMC5160_ReadInt(eTMC, TMC5160_XENC);
		} else if(eReadWrite == TMC_WRITE) {
			eError = TMC5160_WriteInt(eTMC, TMC5160_XENC, puData->lData);
		}
		break;
	case 0x04:
		// �������ֱ��� Encoder Resolution
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.usEncResultion[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			if(g_tAxisParamDefault.usEncResultion[eTMC] != puData->lData)
			{
				//ֵ��ͬ�������
				g_tAxisParamDefault.usEncResultion[eTMC] = puData->lData;
				//������µĲ���
				Update_Enc_ConstValue(eTMC, &g_tAxisParamDefault, 1);
			}
		}
		break;	
		
	case 0x05:		
		// ����������
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.ucEncCountDirect[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			if(puData->lData == 0 || puData->lData == 1)
			{
				g_tAxisParamDefault.ucEncCountDirect[eTMC] = puData->lData;
				//���µĲ���
				Update_Enc_ConstValue(eTMC, &g_tAxisParamDefault, 1);
			}else{
				
				LOG_Error("Param=%d Is Error", puData->lData);
				return ERROR_TYPE_DATA;
			}
		}
		break;	
	case 0x06:
		// ������ֵ 
		if(eReadWrite == TMC_READ) {
			
			puData->lData = g_tAxisParamDefault.lEncDiff_Threshold[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			eError = TMC5160_WriteInt(eTMC, TMC5160_ENC_DEVIATION, puData->lData);
			g_tAxisParamDefault.lEncDiff_Threshold[eTMC] = puData->lData;
		}
		break;		
		
	case 0x07:
		// ����ֵ����С�� 
		if(eReadWrite == TMC_READ) {
			int32_t lXEnc = 0, lXActual = 0, lDiff = 0;
			
			lXEnc = TMC5160_ReadInt(eTMC, TMC5160_XENC);
			lXActual = TMC5160_ReadInt(eTMC, TMC5160_XACTUAL);
			lDiff = lXActual - lXEnc;
			
			//
			puData->lData = lDiff;
			LOG_Info("Actual=%d, XEnc=%d, Diff=%d", lXActual, lXEnc, lDiff);
		} else if(eReadWrite == TMC_WRITE) {
			LOG_Error("Enc Diff Do not Be Write");
			return ERROR_TYPE_RW_RIGHT;
		}
		break;	
	case 0x08:
		//������־λ
		if(eReadWrite == TMC_READ) {
			puData->lData = TMC5160_FIELD_READ(eTMC, TMC5160_ENC_STATUS, TMC5160_DEVIATION_WARN_MASK, TMC5160_DEVIATION_WARN_SHIFT);
		}else if(eReadWrite == TMC_WRITE) {
			
			//�账��������XENC��XVACTUALС��ʧ����ֵ����д1������ñ�־λ����رն�����⹦�ܣ���ִ���˶����������λ��			
			
			//���������־λ
			//TMC5160_FIELD_UPDATE(eTMC, TMC5160_ENC_STATUS, TMC5160_DEVIATION_WARN_MASK, TMC5160_DEVIATION_WARN_SHIFT, puData->lData);
			LOG_Error("Enc Diff Do not Be Write");
			return ERROR_TYPE_RW_RIGHT;
		}
		break;
		
		
		
	/*** ��λ��� ***/
	case 0x10:
		//��λ�ٶ�  ��
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.lResetSpeedLow[eTMC];
		}else if(eReadWrite == TMC_WRITE) {
			g_tAxisParamDefault.lResetSpeedLow[eTMC] = puData->lData;
		}
		break;	
	case 0x11:
		//��λ�ٶ�  ��
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.lResetSpeedHigh[eTMC];
		}else if(eReadWrite == TMC_WRITE) {
			g_tAxisParamDefault.lResetSpeedHigh[eTMC] = puData->lData;
		}
		break;		
	case 0x12:
		//��λƫ�ƾ���
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.lResetOff[eTMC];
		}else if(eReadWrite == TMC_WRITE) {
			g_tAxisParamDefault.lResetOff[eTMC] = puData->lData;
		}
		break;		
	case 0x13:
		//��λ���ٶ�
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.lResetAcc[eTMC];
		}else if(eReadWrite == TMC_WRITE) {
			g_tAxisParamDefault.lResetAcc[eTMC] = puData->lData;
		}
		break;				
	case 0x14:
		//��λ״̬����־λ��
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tTMCStatus.tMotorResetInfo[eTMC].eResetStatus;
		}else if(eReadWrite == TMC_WRITE) {
			LOG_Error("Read Only, Don't Be Writed");
			return ERROR_TYPE_RW_RIGHT;
		}
		break;			

		
		
	/*** ������� ****/	
	case 0x20:
		//���е���
		if(eReadWrite == TMC_READ) {
			//puData->lData = TMC5160_FIELD_READ(eTMC, TMC5160_IHOLD_IRUN, TMC5160_IRUN_MASK, TMC5160_IRUN_SHIFT);
			puData->lData = g_tAxisParamDefault.ucIRun[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			g_tAxisParamDefault.ucIRun[eTMC] = puData->lData;
			uint8_t ucIRun = g_tAxisParamDefault.ucIRun[eTMC]/8;
			eError = TMC5160_FIELD_UPDATE(eTMC, TMC5160_IHOLD_IRUN, TMC5160_IRUN_MASK, TMC5160_IRUN_SHIFT, ucIRun);
		}
		break;
	case 0x21:
		//���ֵ���
		if(eReadWrite == TMC_READ) {
			//puData->lData = TMC5160_FIELD_READ(eTMC, TMC5160_IHOLD_IRUN, TMC5160_IHOLD_MASK, TMC5160_IHOLD_SHIFT);
			puData->lData = g_tAxisParamDefault.ucIHold[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			g_tAxisParamDefault.ucIHold[eTMC] = puData->lData;
			uint8_t ucIHold = g_tAxisParamDefault.ucIHold[eTMC]/8;
			eError = TMC5160_FIELD_UPDATE(eTMC, TMC5160_IHOLD_IRUN, TMC5160_IHOLD_MASK, TMC5160_IHOLD_SHIFT, ucIHold);
		}
		break;
	case 0x22:
		//�����л�ʱ��
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.ucIHoldDelay[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			g_tAxisParamDefault.ucIHoldDelay[eTMC] = puData->lData;
			eError = TMC5160_FIELD_UPDATE(eTMC, TMC5160_IHOLD_IRUN, TMC5160_IHOLDDELAY_MASK, TMC5160_IHOLDDELAY_SHIFT, g_tAxisParamDefault.ucIHoldDelay[eTMC]);
		}
		break;		
//	case 0x22:
//		//����΢�� Global Current Scaler
//		if(eReadWrite == TMC_READ) {
//			puData->lData = g_taTMC5160[eTMC].laShadowRegister[TMC5160_GLOBAL_SCALER];
//		}else if(eReadWrite == TMC_WRITE) {
//			eError = TMC5160_WriteInt(eTMC, TMC5160_GLOBAL_SCALER, puData->lData);
//		}
//		break;		
	case 0x23:
		// ����ϸ�� Microstep Resolution
		if(eReadWrite == TMC_READ) {
			uint8_t ucMicroStep_Rg = 0;
			ucMicroStep_Rg = TMC5160_FIELD_READ(eTMC, TMC5160_CHOPCONF, TMC5160_MRES_MASK, TMC5160_MRES_SHIFT);
			puData->lData  = MicroStep_Register2SetValue((TMC_MicroStep_Resolution_e)ucMicroStep_Rg);
		} else if(eReadWrite == TMC_WRITE) {
			uint16_t usMicroStep = puData->lData;
			uint8_t ucMicroStep_Rg = 0;
			
			//���ø���
//			if(usMicroStep != g_tAxisParamDefault.usMicroStepResultion[eTMC])
//			{
				//���ֱ�������ֵ  ת���� �Ĵ����ֱ���ֵ
				eError = MicroStep_SetValue2Register(usMicroStep, &ucMicroStep_Rg);
				if(eError != ERROR_TYPE_SUCCESS)
				{
					LOG_Error("Data Micro Step=%d Error", usMicroStep);
					return ERROR_TYPE_DATA;
				}
			
				//д��Ĵ���
				eError = TMC5160_FIELD_UPDATE(eTMC, TMC5160_CHOPCONF, TMC5160_MRES_MASK, TMC5160_MRES_SHIFT, ucMicroStep_Rg);
				g_tAxisParamDefault.usMicroStepResultion[eTMC] = usMicroStep;
				//������µĲ���
				Update_Enc_ConstValue(eTMC, &g_tAxisParamDefault, 1);
				//LOG_Info("Micro Step Resolution=%d", usMicroStep);
			//}		
		}
		break;		
	case 0x24:
	{
		//���ûתȫ����
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.usFullStepPerRound[eTMC];
		}else if(eReadWrite == TMC_WRITE) {
//			if(puData->lData != g_tAxisParamDefault.usFullStepPerRound[eTMC])
//			{
				g_tAxisParamDefault.usFullStepPerRound[eTMC] = puData->lData;
				//������µĲ���
				Update_Enc_ConstValue(eTMC, &g_tAxisParamDefault,1);
//				LOG_Info("Full Step Per Round=%d", puData->lData);
//			}
		}
	}
	break;
		
		
	/*** ���������� ***/		
	case 0x30:
		// ��ʼ�ٶ� Velocity VSTART
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.lVStart[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			eError = TMC5160_WriteInt(eTMC, TMC5160_VSTART, (puData->lData)*V_CHANGE_CONST);
			g_tAxisParamDefault.lVStart[eTMC] = puData->lData;
		}
		break;
	case 0x31:
		// ��һ�׶μ��ٶ�A1  Acceleration A1
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.lA1[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			eError = TMC5160_WriteInt(eTMC, TMC5160_A1, (puData->lData)*A_CHANGE_CONST);
			g_tAxisParamDefault.lA1[eTMC] = puData->lData;
		}
		break;
	case 0x32:
		// �����ٶ� Velocity V1
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.lV1[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			eError = TMC5160_WriteInt(eTMC, TMC5160_V1, (puData->lData)*V_CHANGE_CONST);
			g_tAxisParamDefault.lV1[eTMC] = puData->lData;
		}
		break;		
	case 0x33:
		// �ڶ��׶μ��ٶ� AMAX  Maximum acceleration��  ����λ��ģʽ
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.lAMax[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			if(g_tAxisParamDefault.ucMode[eTMC] == TMC_MODE_POSITION)
			{
				//�����ǰ����λ��ģʽ����������Ч
				eError = TMC5160_WriteInt(eTMC, TMC5160_AMAX, (puData->lData)*A_CHANGE_CONST);
			}
			//ʹ��ʱ����Ч
			g_tAxisParamDefault.lAMax[eTMC] = puData->lData;
		}
		break;
	case 0x34:
		// Ŀ�꣨����ٶ� VMAX  Target speed, ����λ��ģʽ
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.lVMax[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			if(g_tAxisParamDefault.ucMode[eTMC] == TMC_MODE_POSITION)
			{
				//�����ǰ����λ��ģʽ����������Ч
				eError = TMC5160_WriteInt(eTMC, TMC5160_VMAX, abs(puData->lData)*V_CHANGE_CONST);
			}
			//ʹ��ʱ����Ч
			g_tAxisParamDefault.lVMax[eTMC] = abs(puData->lData);
		}
		break;
	case 0x35:
		//�ڶ��׶μ��ٶ� DMAX, Maximum Deceleration
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.lDMax[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			eError = TMC5160_WriteInt(eTMC, TMC5160_DMAX, (puData->lData)*A_CHANGE_CONST);
			g_tAxisParamDefault.lDMax[eTMC] = puData->lData;
		}
		break;		
	case 0x36:
		//��һ�׶μ��ٶ� Deceleration D1
		if(eReadWrite == TMC_READ) {			
			puData->lData = g_tAxisParamDefault.lD1[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			eError = TMC5160_WriteInt(eTMC, TMC5160_D1, (puData->lData)*A_CHANGE_CONST);
			g_tAxisParamDefault.lD1[eTMC] = puData->lData;
		}
		break;			
	case 0x37:
		// ֹͣ�ٶ� Velocity VSTOP
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.lVStop[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			eError = TMC5160_WriteInt(eTMC, TMC5160_VSTOP, (puData->lData)*V_CHANGE_CONST);
			g_tAxisParamDefault.lVStop[eTMC] = puData->lData;
		}
		break;
	case 0x38:
		// Waiting time after ramp down
		if(eReadWrite == TMC_READ) {			
			puData->lData = TMC5160_ReadInt(eTMC, TMC5160_TZEROWAIT);
		} else if(eReadWrite == TMC_WRITE) {
			eError = TMC5160_WriteInt(eTMC, TMC5160_TZEROWAIT, puData->lData);
		}
		break;		

	case 0x39:
		// ��ǰʵ���ٶ� Actual speed
		if(eReadWrite == TMC_READ) {
			puData->lData = TMC5160_ReadInt(eTMC, TMC5160_VACTUAL)/V_CHANGE_CONST;
		} else if(eReadWrite == TMC_WRITE) {
			LOG_Error("VACTUAL Do Not Be Write");
			return ERROR_TYPE_RW_RIGHT;
		}
		break;
	case 0x3A:
		// reach max speed
		if(eReadWrite == TMC_READ) {
			puData->lData = TMC5160_FIELD_READ(eTMC, TMC5160_RAMPSTAT, TMC5160_RAMPSTAT_VELOCITY_REACH_MASK, TMC5160_RAMPSTAT_VELOCITY_REACH_SHIFT);
		} else if(eReadWrite == TMC_WRITE) {
			LOG_Error("Max Speed Flag Do Not Be Write");
			return ERROR_TYPE_RW_RIGHT;
		}
		break;
	case 0x3B:
		// speed is zero
		if(eReadWrite == TMC_READ) {
			puData->lData = TMC5160_FIELD_READ(eTMC, TMC5160_RAMPSTAT, TMC5160_RAMPSTAT_VELOCITY_IS_ZERO_MASK, TMC5160_RAMPSTAT_VELOCITY_IS_ZERO_SHIFT);
		} else if(eReadWrite == TMC_WRITE) {
			LOG_Error("Speed is zero Flag Do Not Be Write");
			return ERROR_TYPE_RW_RIGHT;
		}
		break;

	case 0x40:
		// �ٶ�ģʽ�£����ٶ�ֵ
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.lAMax_VMode[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			if(g_tAxisParamDefault.ucMode[eTMC] == TMC_MODE_VELNEG || g_tAxisParamDefault.ucMode[eTMC] == TMC_MODE_VELPOS)
			{
				//�����ǰΪ�ٶ�ģʽ��������Ч
				eError = TMC5160_WriteInt(eTMC, TMC5160_AMAX, (puData->lData)*A_CHANGE_CONST);
			}
			//ʹ��ʱ����Ч
			g_tAxisParamDefault.lAMax_VMode[eTMC] = puData->lData;
		}
		break;
	case 0x41:
		// �ٶ�ģʽ�£�Ŀ�꣨����ٶ� VMAX  Target speed
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.lVMax[eTMC];
		} else if(eReadWrite == TMC_WRITE){
			if(g_tAxisParamDefault.ucMode[eTMC] == TMC_MODE_VELNEG || g_tAxisParamDefault.ucMode[eTMC] == TMC_MODE_VELPOS)
			{
				//�����ǰΪ�ٶ�ģʽ��������Ч
				eError = TMC5160_WriteInt(eTMC, TMC5160_VMAX, abs(puData->lData)*V_CHANGE_CONST);
			}
			//ʹ��ʱ����Ч
			g_tAxisParamDefault.lVMax_VMode[eTMC] = abs(puData->lData);
		}
		break;
		
//	case 4:
//		// Maximum speed
//		if(eReadWrite == TMC_READ) {
//			*plValue = g_taTMC5160[eTMC].laShadowRegister[TMC5160_VMAX];
//		} else if(eReadWrite == TMC_WRITE) {
//			g_taTMC5160[eTMC].laShadowRegister[TMC5160_VMAX] = abs(*plValue);
//			if(TMC5160_ReadInt(eTMC, TMC5160_RAMPMODE) == TMC_MODE_POSITION)
//				TMC5160_WriteInt(eTMC, TMC5160_VMAX, abs(*plValue));
//		}
//		break;



		
	/***  �ο�������� ***/	
//	case 0x50:
//		// SW_MODE Register
//		if(eReadWrite == TMC_READ) {
//			*plValue = TMC5160_ReadInt(eTMC, TMC5160_SWMODE);
//		} else if(eReadWrite == TMC_WRITE) {
//			TMC5160_WriteInt(eTMC, TMC5160_SWMODE, *plValue);
//		}
//		break;
//		
//	case 0x51:
//		// Right endstop
//		if(eReadWrite == TMC_READ) {
//			*plValue = !TMC5160_FIELD_READ(eTMC, TMC5160_RAMPSTAT, TMC5160_STATUS_STOP_R_MASK, TMC5160_STATUS_STOP_R_SHIFT);
//		} else if(eReadWrite == TMC_WRITE) {
//////			errors |= TMC_ERROR_TYPE;
//		}
//		break;
//	case 0x52:
//		// Left endstop
//		if(eReadWrite == TMC_READ) {
//			*plValue = !TMC5160_FIELD_READ(eTMC, TMC5160_RAMPSTAT, TMC5160_STATUS_STOP_L_MASK, TMC5160_STATUS_STOP_L_SHIFT);
//		} else if(eReadWrite == TMC_WRITE) {
//////			errors |= TMC_ERROR_TYPE;
//		}
//		break;
//		
//	case 0x53:
//		//@todo swap right/left switch 
//		break;
//		
	case 0x54:
		// ����λ���� right limit switch polarity
		if(eReadWrite == TMC_READ) {
			puData->lData = TMC5160_FIELD_READ(eTMC, TMC5160_SWMODE, TMC5160_POL_STOP_R_MASK, TMC5160_POL_STOP_R_SHIFT);
		} else if(eReadWrite == TMC_WRITE) {
			if(puData->lData == 0 || puData->lData == 1)
			{
				eError = TMC5160_FIELD_UPDATE(eTMC, TMC5160_SWMODE, TMC5160_POL_STOP_R_MASK, TMC5160_POL_STOP_R_SHIFT, puData->lData);
				g_tAxisParamDefault.ucRighLimitPolarity[eTMC] = puData->lData;
			}else{
				LOG_Error("Data=%d Is Error", puData->lData);
				return ERROR_TYPE_DATA;
			}
		}
		break;
	case 0x55:
		// ����λ���� left limit switch polarity
		if(eReadWrite == TMC_READ) {
			puData->lData = TMC5160_FIELD_READ(eTMC, TMC5160_SWMODE, TMC5160_POL_STOP_L_MASK, TMC5160_POL_STOP_L_SHIFT);
		} else if(eReadWrite == TMC_WRITE) {
			if(puData->lData == 0 || puData->lData == 1)
			{
				eError = TMC5160_FIELD_UPDATE(eTMC, TMC5160_SWMODE, TMC5160_POL_STOP_L_MASK, TMC5160_POL_STOP_L_SHIFT, puData->lData);
				g_tAxisParamDefault.ucLeftLimitPolarity[eTMC] = puData->lData;
			}else{
				LOG_Error("Data=%d Is Error", puData->lData);
				return ERROR_TYPE_DATA;
			}
		}
		break;

	case 0x56:
		// ����λ������־ right limit triggle flag
		if(eReadWrite == TMC_READ) {
			puData->lData = TMC5160_FIELD_READ(eTMC, TMC5160_RAMPSTAT, TMC5160_STATUS_STOP_R_MASK, TMC5160_STATUS_STOP_R_SHIFT);
		} else if(eReadWrite == TMC_WRITE) {
			LOG_Error("Left Limit Flag Do Not Be Write");
			eError = ERROR_TYPE_RW_RIGHT;
		}
		break;

	case 0x57:
		// ����λ������־
		if(eReadWrite == TMC_READ) {
			puData->lData = TMC5160_FIELD_READ(eTMC, TMC5160_RAMPSTAT, TMC5160_STATUS_STOP_L_MASK, TMC5160_STATUS_STOP_L_SHIFT);
		} else if(eReadWrite == TMC_WRITE) {
			LOG_Error("Left Limit Flag Do Not Be Write");
			eError = ERROR_TYPE_RW_RIGHT;
		}
		break;
	case 0x58:
		// ���ʹ��
		if(eReadWrite == TMC_READ) {	
			puData->lData = g_tTMCStatus.ucEnableFlag[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			if(puData->lData == 0)
			{
				TMC_Disable(eTMC);
				g_tTMCStatus.ucEnableFlag[eTMC] = 0;
			}else if(puData->lData == 1){
				TMC_Enable(eTMC);
				g_tTMCStatus.ucEnableFlag[eTMC] = 1;
			}else{
				LOG_Error("Param=%d Is Error", puData->lData);
				return ERROR_TYPE_DATA;
			}
		}
		break;

	case 0x59:
		// �������
		if(eReadWrite == TMC_READ) {
			puData->lData = TMC5160_FIELD_READ(eTMC, TMC5160_GCONF, TMC5160_SHAFT_MASK, TMC5160_SHAFT_SHIFT);
		} else if(eReadWrite == TMC_WRITE) {
			if(puData->lData == 0 || puData->lData == 1)
			{
				eError = TMC5160_FIELD_UPDATE(eTMC, TMC5160_GCONF, TMC5160_SHAFT_MASK, TMC5160_SHAFT_SHIFT, puData->lData);
			}else{
				
				LOG_Error("Param=%d Is Error", puData->lData);
				return ERROR_TYPE_DATA;
			}
		}
		break;

	case 0x5A:
		// ��ȡTMC5160�ļĴ�����ֵ
		if(eReadWrite == TMC_READ) {
			uint8_t ucAddr = puData->lData;
			
			if(CheckRegister_Addr(ucAddr) != ERROR_TYPE_SUCCESS) return ERROR_TYPE_DATA;
			puData->lData = TMC5160_ReadInt(eTMC, puData->lData);
			
		} else if(eReadWrite == TMC_WRITE) {
			return ERROR_TYPE_DATA;
		}
		break;
		
	/***  ն����� ***/	
/*
	case 60:
		// Speed threshold for high speed mode
		if(eReadWrite == TMC_READ) {
			buffer = TMC5160_ReadInt(eTMC, TMC5160_THIGH);
			*plValue = MIN(0xFFFFF, (1 << 24) / ((buffer)? buffer : 1));
		} else if(eReadWrite == TMC_WRITE) {
			*plValue = MIN(0xFFFFF, (1 << 24) / ((*plValue)? *plValue:1));
			TMC5160_WriteInt(eTMC, TMC5160_THIGH, *plValue);
		}
		break;
	case 61:
		// Minimum speed for switching to dcStep
		if(eReadWrite == TMC_READ) {
			*plValue = TMC5160_ReadInt(eTMC, TMC5160_VDCMIN);
		} else if(eReadWrite == TMC_WRITE) {
			TMC5160_WriteInt(eTMC, TMC5160_VDCMIN, *plValue);
		}
		break;
	case 62:
		// High speed chopper mode
		if(eReadWrite == TMC_READ) {
			*plValue = TMC5160_FIELD_READ(eTMC, TMC5160_CHOPCONF, TMC5160_VHIGHCHM_MASK, TMC5160_VHIGHCHM_SHIFT);
		} else if(eReadWrite == TMC_WRITE) {
			TMC5160_FIELD_UPDATE(eTMC, TMC5160_CHOPCONF, TMC5160_VHIGHCHM_MASK, TMC5160_VHIGHCHM_SHIFT, *plValue);
		}
		break;
	case 63:
		// High speed fullstep mode
		if(eReadWrite == TMC_READ) {
			*plValue = TMC5160_FIELD_READ(eTMC, TMC5160_CHOPCONF, TMC5160_VHIGHFS_MASK, TMC5160_VHIGHFS_SHIFT);
		} else if(eReadWrite == TMC_WRITE) {
			TMC5160_FIELD_UPDATE(eTMC, TMC5160_CHOPCONF, TMC5160_VHIGHFS_MASK, TMC5160_VHIGHFS_SHIFT, *plValue);
		}
		break;
	case 64:
		// Measured Speed
		if(eReadWrite == TMC_READ) {
////			*plValue = TMC5160.velocity;
		} else if(eReadWrite == TMC_WRITE) {
////			errors |= TMC_ERROR_TYPE;
		}
		break;
	case 65:
		// Analog I Scale
		if(eReadWrite == TMC_READ) {
			*plValue = TMC5160_FIELD_READ(eTMC, TMC5160_GCONF, TMC5160_RECALIBRATE_MASK, TMC5160_RECALIBRATE_SHIFT);
		} else if(eReadWrite == TMC_WRITE) {
			TMC5160_FIELD_UPDATE(eTMC, TMC5160_GCONF, TMC5160_RECALIBRATE_MASK, TMC5160_RECALIBRATE_SHIFT, *plValue);
		}
		break;
	case 66:
		// Internal RSense
		if(eReadWrite == TMC_READ) {
			*plValue = TMC5160_FIELD_READ(eTMC, TMC5160_GCONF, TMC5160_REFR_DIR_MASK, TMC5160_REFR_DIR_SHIFT);
		} else if(eReadWrite == TMC_WRITE) {
			TMC5160_FIELD_UPDATE(eTMC, TMC5160_GCONF, TMC5160_REFR_DIR_MASK, TMC5160_REFR_DIR_SHIFT, *plValue);
		}
		break;

	case 67:
		// Chopper blank time
		if(eReadWrite == TMC_READ) {
			*plValue = TMC5160_FIELD_READ(eTMC, TMC5160_CHOPCONF, TMC5160_TBL_MASK, TMC5160_TBL_SHIFT);
		} else if(eReadWrite == TMC_WRITE) {
			TMC5160_FIELD_UPDATE(eTMC, TMC5160_CHOPCONF, TMC5160_TBL_MASK, TMC5160_TBL_SHIFT, *plValue);
		}
		break;
	case 68:
		// Constant TOff Mode
		if(eReadWrite == TMC_READ) {
			*plValue = TMC5160_FIELD_READ(eTMC, TMC5160_CHOPCONF, TMC5160_CHM_MASK, TMC5160_CHM_SHIFT);
		} else if(eReadWrite == TMC_WRITE) {
			TMC5160_FIELD_UPDATE(eTMC, TMC5160_CHOPCONF, TMC5160_CHM_MASK, TMC5160_CHM_SHIFT, *plValue);
		}
		break;
	case 69:
		// Disable fast decay comparator
		if(eReadWrite == TMC_READ) {
			*plValue = TMC5160_FIELD_READ(eTMC, TMC5160_CHOPCONF, TMC5160_DISFDCC_MASK, TMC5160_DISFDCC_SHIFT);
		} else if(eReadWrite == TMC_WRITE) {
			TMC5160_FIELD_UPDATE(eTMC, TMC5160_CHOPCONF, TMC5160_DISFDCC_MASK, TMC5160_DISFDCC_SHIFT, *plValue);
		}
		break;
	case 6A:
		// Chopper hysteresis end / fast decay time
		buffer = TMC5160_ReadInt(eTMC, TMC5160_CHOPCONF);
		if(eReadWrite == TMC_READ) {
			if(buffer & (1 << TMC5160_CHM_SHIFT))
			{
				*plValue = (buffer >> TMC5160_HEND_SHIFT) & TMC5160_HEND_MASK;
			}
			else
			{
				*plValue = (TMC5160_ReadInt(eTMC, TMC5160_CHOPCONF) >> TMC5160_TFD_ALL_SHIFT) & TMC5160_TFD_ALL_MASK;
				if(buffer & TMC5160_TFD_3_SHIFT)
					*plValue |= 1<<3; // MSB wird zu plValue dazugef��gt
			}
		} else if(eReadWrite == TMC_WRITE) {
			if(TMC5160_ReadInt(eTMC, TMC5160_CHOPCONF) & (1<<14))
			{
				TMC5160_FIELD_UPDATE(eTMC, TMC5160_CHOPCONF, TMC5160_HEND_MASK, TMC5160_HEND_SHIFT, *plValue);
			}
			else
			{
				TMC5160_FIELD_UPDATE(eTMC, TMC5160_CHOPCONF, TMC5160_TFD_3_MASK, TMC5160_TFD_3_SHIFT, (*plValue & (1<<3))); // MSB wird zu plValue dazugef��gt
				TMC5160_FIELD_UPDATE(eTMC, TMC5160_CHOPCONF, TMC5160_TFD_ALL_MASK, TMC5160_TFD_ALL_SHIFT, *plValue);
			}
		}
		break;
	case 6B:
		// Chopper hysteresis start / sine wave offset
		buffer = TMC5160_ReadInt(eTMC, TMC5160_CHOPCONF);
		if(eReadWrite == TMC_READ) {
			if(buffer & (1 << TMC5160_CHM_SHIFT))
			{
				*plValue = (buffer >> TMC5160_HSTRT_SHIFT) & TMC5160_HSTRT_MASK;
			}
			else
			{
				*plValue = (buffer >> TMC5160_OFFSET_SHIFT) & TMC5160_OFFSET_MASK;
				if(buffer & (1 << TMC5160_TFD_3_SHIFT))
					*plValue |= 1<<3; // MSB wird zu plValue dazugef��gt
			}
		} else if(eReadWrite == TMC_WRITE) {
			if(buffer & (1 << TMC5160_CHM_SHIFT))
			{
				TMC5160_FIELD_UPDATE(eTMC, TMC5160_CHOPCONF, TMC5160_HSTRT_MASK, TMC5160_HSTRT_SHIFT, *plValue);
			}
			else
			{
				TMC5160_FIELD_UPDATE(eTMC, TMC5160_CHOPCONF, TMC5160_OFFSET_MASK, TMC5160_OFFSET_SHIFT, *plValue);
			}
		}
		break;
	case 6C:
		// Chopper off time
		if(eReadWrite == TMC_READ) {
			*plValue = TMC5160_FIELD_READ(eTMC, TMC5160_CHOPCONF, TMC5160_TOFF_MASK, TMC5160_TOFF_SHIFT);
		} else if(eReadWrite == TMC_WRITE) {
			TMC5160_FIELD_UPDATE(eTMC, TMC5160_CHOPCONF, TMC5160_TOFF_MASK, TMC5160_TOFF_SHIFT, *plValue);
		}
		break;
	case 168:
		// smartEnergy current minimum (SEIMIN)
		if(eReadWrite == TMC_READ) {
			*plValue = TMC5160_FIELD_READ(eTMC, TMC5160_COOLCONF, TMC5160_SEIMIN_MASK, TMC5160_SEIMIN_SHIFT);
		} else if(eReadWrite == TMC_WRITE) {
			TMC5160_FIELD_UPDATE(eTMC, TMC5160_COOLCONF, TMC5160_SEIMIN_MASK, TMC5160_SEIMIN_SHIFT, *plValue);
		}
		break;
	case 169:
		// smartEnergy current down step
		if(eReadWrite == TMC_READ) {
			*plValue = TMC5160_FIELD_READ(eTMC, TMC5160_COOLCONF, TMC5160_SEDN_MASK, TMC5160_SEDN_SHIFT);
		} else if(eReadWrite == TMC_WRITE) {
			TMC5160_FIELD_UPDATE(eTMC, TMC5160_COOLCONF, TMC5160_SEDN_MASK, TMC5160_SEDN_SHIFT, *plValue);
		}
		break;
	case 170:
		// smartEnergy hysteresis
		if(eReadWrite == TMC_READ) {
			*plValue = TMC5160_FIELD_READ(eTMC, TMC5160_COOLCONF, TMC5160_SEMAX_MASK, TMC5160_SEMAX_SHIFT);
		} else if(eReadWrite == TMC_WRITE) {
			TMC5160_FIELD_UPDATE(eTMC, TMC5160_COOLCONF, TMC5160_SEMAX_MASK, TMC5160_SEMAX_SHIFT, *plValue);
		}
		break;
	case 171:
		// smartEnergy current up step
		if(eReadWrite == TMC_READ) {
			*plValue = TMC5160_FIELD_READ(eTMC, TMC5160_COOLCONF, TMC5160_SEUP_MASK, TMC5160_SEUP_SHIFT);
		} else if(eReadWrite == TMC_WRITE) {
			TMC5160_FIELD_UPDATE(eTMC, TMC5160_COOLCONF, TMC5160_SEUP_MASK, TMC5160_SEUP_SHIFT, *plValue);
		}
		break;
	case 172:
		// smartEnergy hysteresis start
		if(eReadWrite == TMC_READ) {
			*plValue = TMC5160_FIELD_READ(eTMC, TMC5160_COOLCONF, TMC5160_SEMIN_MASK, TMC5160_SEMIN_SHIFT);
		} else if(eReadWrite == TMC_WRITE) {
			TMC5160_FIELD_UPDATE(eTMC, TMC5160_COOLCONF, TMC5160_SEMIN_MASK, TMC5160_SEMIN_SHIFT, *plValue);
		}
		break;
	case 173:
		// stallGuard2 filter enable
		if(eReadWrite == TMC_READ) {
			*plValue = TMC5160_FIELD_READ(eTMC, TMC5160_COOLCONF, TMC5160_SFILT_MASK, TMC5160_SFILT_SHIFT);
		} else if(eReadWrite == TMC_WRITE) {
			TMC5160_FIELD_UPDATE(eTMC, TMC5160_COOLCONF, TMC5160_SFILT_MASK, TMC5160_SFILT_SHIFT, *plValue);
		}
		break;
	case 174:
		// stallGuard2 threshold
		if(eReadWrite == TMC_READ) {
			*plValue = TMC5160_FIELD_READ(eTMC, TMC5160_COOLCONF, TMC5160_SGT_MASK, TMC5160_SGT_SHIFT);
			*plValue = CAST_Sn_TO_S32(*plValue, 7);
		} else if(eReadWrite == TMC_WRITE) {
			TMC5160_FIELD_UPDATE(eTMC, TMC5160_COOLCONF, TMC5160_SGT_MASK, TMC5160_SGT_SHIFT, *plValue);
		}
		break;
	case 180:
		// smartEnergy actual current
		if(eReadWrite == TMC_READ) {
			*plValue = TMC5160_FIELD_READ(eTMC, TMC5160_DRVSTATUS, TMC5160_CS_ACTUAL_MASK, TMC5160_CS_ACTUAL_SHIFT);
		} else if(eReadWrite == TMC_WRITE) {
////			errors |= TMC_ERROR_TYPE;
		}
		break;
	case 181:
		// smartEnergy stall velocity
		//this function sort of doubles with 182 but is necessary to allow cross chip compliance
		if(eReadWrite == TMC_READ) {
			if(TMC5160_FIELD_READ(eTMC, TMC5160_SWMODE, TMC5160_SG_STOP_MASK, TMC5160_SG_STOP_SHIFT))
			{
				buffer = TMC5160_ReadInt(eTMC, TMC5160_TCOOLTHRS);
				*plValue = MIN(0xFFFFF, (1<<24) / ((buffer)? buffer:1));
			}
			else
			{
				*plValue = 0;
			}
		} else if(eReadWrite == TMC_WRITE) {
			TMC5160_FIELD_UPDATE(eTMC, TMC5160_SWMODE, TMC5160_SG_STOP_MASK, TMC5160_SG_STOP_SHIFT, (*plValue)? 1:0);

			*plValue = MIN(0xFFFFF, (1<<24) / ((*plValue)? *plValue:1));
			TMC5160_WriteInt(eTMC, TMC5160_TCOOLTHRS, *plValue);
		}
		break;
	case 182:
		// smartEnergy threshold speed
		if(eReadWrite == TMC_READ) {
			buffer = TMC5160_ReadInt(eTMC, TMC5160_TCOOLTHRS);
			*plValue = MIN(0xFFFFF, (1<<24) / ((buffer)? buffer:1));
		} else if(eReadWrite == TMC_WRITE) {
			*plValue = MIN(0xFFFFF, (1<<24) / ((*plValue)? *plValue:1));
			TMC5160_WriteInt(eTMC, TMC5160_TCOOLTHRS, *plValue);
		}
		break;
	case 184:
		// Random TOff mode
		if(eReadWrite == TMC_READ) {
			*plValue = TMC5160_FIELD_READ(eTMC, TMC5160_CHOPCONF, TMC5160_RNDTF_MASK, TMC5160_RNDTF_SHIFT);
		} else if(eReadWrite == TMC_WRITE) {
			TMC5160_FIELD_UPDATE(eTMC, TMC5160_CHOPCONF, TMC5160_RNDTF_MASK, TMC5160_RNDTF_SHIFT, *plValue);
		}
		break;
*/


		
	/***  PWM��� ***/	
/* 
	case 185:
		// Chopper synchronization
		if(eReadWrite == TMC_READ) {
			*plValue = (TMC5160_ReadInt(eTMC, TMC5160_CHOPCONF) >> 20) & 0x0F;
		} else if(eReadWrite == TMC_WRITE) {
			buffer = TMC5160_ReadInt(eTMC, TMC5160_CHOPCONF);
			buffer &= ~(0x0F<<20);
			buffer |= (*plValue & 0x0F) << 20;
			TMC5160_WriteInt(eTMC, TMC5160_CHOPCONF,buffer);
		}
		break;
	case 186:
		// PWM threshold speed
		if(eReadWrite == TMC_READ) {
			buffer = TMC5160_ReadInt(eTMC, TMC5160_TPWMTHRS);
			*plValue = MIN(0xFFFFF, (1<<24) / ((buffer)? buffer:1));
		} else if(eReadWrite == TMC_WRITE) {
			*plValue = MIN(0xFFFFF, (1<<24) / ((*plValue)? *plValue:1));
			TMC5160_WriteInt(eTMC, TMC5160_TPWMTHRS, *plValue);
		}
		break;
	case 187:
		// PWM gradient
		if(eReadWrite == TMC_READ) {
			*plValue = TMC5160_FIELD_READ(eTMC, TMC5160_PWMCONF, TMC5160_PWM_GRAD_MASK, TMC5160_PWM_GRAD_SHIFT);
		} else if(eReadWrite == TMC_WRITE) {
			// Set gradient
			TMC5160_FIELD_UPDATE(eTMC, TMC5160_PWMCONF, TMC5160_PWM_GRAD_MASK, TMC5160_PWM_GRAD_SHIFT, *plValue);
			// Enable/disable stealthChop accordingly
			TMC5160_FIELD_UPDATE(eTMC, TMC5160_GCONF, TMC5160_EN_PWM_MODE_MASK, TMC5160_EN_PWM_MODE_SHIFT, (*plValue) ? 1 : 0);
		}
		break;
	case 188:
		// PWM amplitude
		if(eReadWrite == TMC_READ) {
			*plValue = TMC5160_FIELD_READ(eTMC, TMC5160_PWMCONF, TMC5160_PWM_OFS_MASK, TMC5160_PWM_OFS_SHIFT);
		} else if(eReadWrite == TMC_WRITE) {
			TMC5160_FIELD_UPDATE(eTMC, TMC5160_PWMCONF, TMC5160_GLOBAL_SCALER_MASK, TMC5160_GLOBAL_SCALER_SHIFT, *plValue);
		}
		break;
	case 191:
		// PWM frequency
		if(eReadWrite == TMC_READ) {
			*plValue = TMC5160_FIELD_READ(eTMC, TMC5160_PWMCONF, TMC5160_PWM_FREQ_MASK, TMC5160_PWM_FREQ_SHIFT);
		} else if(eReadWrite == TMC_WRITE) {
			if(*plValue >= 0 && *plValue < 4)
			{
				TMC5160_FIELD_UPDATE(eTMC, TMC5160_PWMCONF, TMC5160_PWM_FREQ_MASK, TMC5160_PWM_FREQ_SHIFT, *plValue);
			}
			else
			{
				errors |= TMC_ERROR_VALUE;
			}
		}
		break;
	case 192:
		// PWM autoscale
		if(eReadWrite == TMC_READ) {
			*plValue = TMC5160_FIELD_READ(eTMC, TMC5160_PWMCONF, TMC5160_PWM_AUTOSCALE_MASK, TMC5160_PWM_AUTOSCALE_SHIFT);
		} else if(eReadWrite == TMC_WRITE) {
			if(*plValue >= 0 && *plValue < 2)
			{
				TMC5160_FIELD_UPDATE(eTMC, TMC5160_PWMCONF, TMC5160_PWM_AUTOSCALE_MASK, TMC5160_PWM_AUTOSCALE_SHIFT, *plValue);
			}
			else
			{
				errors |= TMC_ERROR_VALUE;
			}
		}
		break;
	case 204:
		// Freewheeling mode
		if(eReadWrite == TMC_READ) {
			*plValue = TMC5160_FIELD_READ(eTMC, TMC5160_PWMCONF, TMC5160_FREEWHEEL_MASK, TMC5160_FREEWHEEL_SHIFT);
		} else if(eReadWrite == TMC_WRITE) {
			TMC5160_FIELD_UPDATE(eTMC, TMC5160_PWMCONF, TMC5160_FREEWHEEL_MASK, TMC5160_FREEWHEEL_SHIFT, *plValue);
		}
		break;
	case 206:
		// Load plValue
		if(eReadWrite == TMC_READ) {
			*plValue = TMC5160_FIELD_READ(eTMC, TMC5160_DRVSTATUS, TMC5160_SG_RESULT_MASK, TMC5160_SG_RESULT_SHIFT);
		} else if(eReadWrite == TMC_WRITE) {
////			errors |= TMC_ERROR_TYPE;
		}
		break;
			
*/			


		
//		
//	case 8:
//		// Position reached flag
//		if(eReadWrite == TMC_READ) {
//			*plValue = TMC5160_FIELD_READ(eTMC, TMC5160_RAMPSTAT, TMC5160_POSITION_REACHED_MASK, TMC5160_POSITION_REACHED_SHIFT);
//		} else if(eReadWrite == TMC_WRITE) {
//////			errors |= TMC_ERROR_TYPE;
//		}
//		break;
		

		default:
		{
			LOG_Error("unKnown Type");
			eError = ERROR_TYPE_TYPE;
		}
		break;
	}
	return eError;
}







///*
//*	����Ĭ�������
//*/
//ErrorType_e TMC_AxisParam_Default(TMC_e eTMC, ReadWrite_e eReadWrite, uint8_t ucType, Data4Byte_u *puData)
//{
//	ErrorType_e eError = ERROR_TYPE_SUCCESS;
//	
//	//�������
//	if(eTMC >= TMC_END)
//	{
//		LOG_Error("TMC DeviceID=%d Is Error", eTMC);
//		return ERROR_TYPE_DEVICE_ID;
//	}
//	
//	switch(eTMC)
//	{
//		case TMC_0:
//		case TMC_1:
//		case TMC_2:
//		case TMC_3:
//		case TMC_4:
//		case TMC_5:
//		{
//			eError = TMC5160_AxisParam_Default(eTMC, eReadWrite, ucType, puData);
//		}
//		break;
//		default:break;
//	}
//	return eError;
//}






/*
*	����Ĭ�������
*/
ErrorType_e TMC_AxisParam_Default(TMC_e eTMC, ReadWrite_e eReadWrite, uint8_t ucType, Data4Byte_u *puData)
//ErrorType_e TMC5160_AxisParam_Default(TMC_e eTMC, ReadWrite_e eReadWrite, uint8_t ucType, Data4Byte_u *puData)
{
//	extern Process_t g_tProcess;
	extern TMC5160_t g_taTMC5160[TMC_MODULE_END];
	//extern AxisParamDefault_t g_tAxisParamDefault;
	AxisParamDefault_t tAxisParamDefault = {0};
	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
	uint8_t ucSaveFlag = 0;
	
	//�������
	if(eTMC >= TMC_END)
	{
		LOG_Error("TMC DeviceID=%d Is Error", eTMC);
		return ERROR_TYPE_DEVICE_ID;
	}
	
	//��ȡĬ�������
	if(ERROR_TYPE_SUCCESS != Read_Axis_Param_Default(&tAxisParamDefault))
	{
		return ERROR_TYPE_CRC;
	}
	
	
	//
	switch(ucType)
	{

	/*** ���������� ***/
	case 0x04:
		// �������ֱ��� Encoder Resolution
		if(eReadWrite == TMC_READ) {
			puData->lData = tAxisParamDefault.usEncResultion[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			if(tAxisParamDefault.usEncResultion[eTMC] != puData->lData)
			{
				//ֵ��ͬ�������
				tAxisParamDefault.usEncResultion[eTMC] = puData->lData;
				//���µĲ���
				Update_Enc_ConstValue(eTMC, &tAxisParamDefault, 0);
				ucSaveFlag = 1;
			}
		}
		break;	
		
	case 0x05:		
		// ����������
		if(eReadWrite == TMC_READ) {
			puData->lData = tAxisParamDefault.ucEncCountDirect[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			if(puData->lData == 0 || puData->lData == 1)
			{ 
				if(tAxisParamDefault.ucEncCountDirect[eTMC] != puData->lData)
				{
					tAxisParamDefault.ucEncCountDirect[eTMC] = puData->lData;
					//���µĲ���
					Update_Enc_ConstValue(eTMC, &tAxisParamDefault, 0);
					ucSaveFlag = 1;
				}
			}else{
				LOG_Error("Param=%d Is Error", puData->lData);
				return ERROR_TYPE_DATA;
			}
		}
		break;	
	case 0x06:
		// ������ֵ 
		if(eReadWrite == TMC_READ) {
			puData->lData = tAxisParamDefault.lEncDiff_Threshold[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			if(tAxisParamDefault.lEncDiff_Threshold[eTMC] != puData->lData)
			{
				tAxisParamDefault.lEncDiff_Threshold[eTMC] = puData->lData;
				ucSaveFlag = 1;
			}
		}
		break;			

		
		
		
	/*** ��λ��� ***/
	case 0x10:
		//��λ�ٶ�  ��
		if(eReadWrite == TMC_READ) {
			puData->lData = tAxisParamDefault.lResetSpeedLow[eTMC];
		}else if(eReadWrite == TMC_WRITE) {
			if(tAxisParamDefault.lResetSpeedLow[eTMC] != puData->lData)
			{
				tAxisParamDefault.lResetSpeedLow[eTMC] = puData->lData;
				ucSaveFlag = 1;
			}
		}
		break;	
	case 0x11:
		//��λ�ٶ�  ��
		if(eReadWrite == TMC_READ) {
			puData->lData = tAxisParamDefault.lResetSpeedHigh[eTMC];
		}else if(eReadWrite == TMC_WRITE) {
			if(tAxisParamDefault.lResetSpeedHigh[eTMC] != puData->lData)
			{
				tAxisParamDefault.lResetSpeedHigh[eTMC] = puData->lData;
				ucSaveFlag = 1;
			}
		}
		break;		
	case 0x12:
		//��λƫ�ƾ���
		if(eReadWrite == TMC_READ) {
			puData->lData = tAxisParamDefault.lResetOff[eTMC];
		}else if(eReadWrite == TMC_WRITE) {
			if(tAxisParamDefault.lResetOff[eTMC] != puData->lData)
			{
				tAxisParamDefault.lResetOff[eTMC] = puData->lData;
				ucSaveFlag = 1;
			}
		}
		break;		
	case 0x13:
		//��λ���ٶ�
		if(eReadWrite == TMC_READ) {
			puData->lData = tAxisParamDefault.lResetAcc[eTMC];
		}else if(eReadWrite == TMC_WRITE) {
			if(tAxisParamDefault.lResetAcc[eTMC] != puData->lData)
			{
				tAxisParamDefault.lResetAcc[eTMC] = puData->lData;
				ucSaveFlag = 1;
			}
		}
		break;				
		
		
	/*** ������� ****/	
	case 0x19:
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.ucIHoldDelay[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			if(g_tAxisParamDefault.ucIHoldDelay[eTMC] != puData->lData)
			{
				g_tAxisParamDefault.ucIHoldDelay[eTMC] = puData->lData;
				ucSaveFlag = 1;
			}
		}
		break;
	case 0x20:
		//���е���
		if(eReadWrite == TMC_READ) {
			puData->lData = tAxisParamDefault.ucIRun[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			
			//�������
			if(puData->lData > CURRENT_MAX_VALUE)
			{
				LOG_Error("Current Set Value=%d More than %d", puData->lData, CURRENT_CHANGE_CONST);
				return ERROR_TYPE_DATA;
			}
			
			//���ò���
			if(tAxisParamDefault.ucIRun[eTMC] != puData->lData)
			{				
				tAxisParamDefault.ucIRun[eTMC] = puData->lData;
				ucSaveFlag = 1;
			}
		}
		break;
	case 0x21:
		//���ֵ���
		if(eReadWrite == TMC_READ) {
			puData->lData = tAxisParamDefault.ucIHold[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			//�������
			if(puData->lData > CURRENT_MAX_VALUE)
			{
				LOG_Error("Current Set Value=%d More than %d", puData->lData, CURRENT_CHANGE_CONST);
				return ERROR_TYPE_DATA;
			}
			
			//���ò���
			if(tAxisParamDefault.ucIHold[eTMC] != puData->lData)
			{
				tAxisParamDefault.ucIHold[eTMC] = puData->lData;
				ucSaveFlag = 1;
			}			
		}
		break;		
		
	case 0x23:
		// ����ϸ�� Microstep Resolution
		if(eReadWrite == TMC_READ) {		
			puData->lData = tAxisParamDefault.usMicroStepResultion[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			uint16_t usMicroStep = puData->lData;
			uint8_t ucMicroStep_Rg = 0;
			
			//���ø���
			if(usMicroStep != tAxisParamDefault.usMicroStepResultion[eTMC])
			{
				//���ֱ�������ֵ  ת���� �Ĵ����ֱ���ֵ
				eErrorType = MicroStep_SetValue2Register(usMicroStep, &ucMicroStep_Rg);
				if(eErrorType != ERROR_TYPE_SUCCESS)
				{
					LOG_Error("Data Micro Step=%d Error", usMicroStep);
					return ERROR_TYPE_DATA;
				}
			
				//д��Ĵ���
				tAxisParamDefault.usMicroStepResultion[eTMC] = usMicroStep;
				//������µĲ���
				Update_Enc_ConstValue(eTMC, &tAxisParamDefault, 0);
				ucSaveFlag = 1;
//				LOG_Info("Micro Step Resolution=%d", usMicroStep);
			}		
		}
		break;		
	case 0x24:
	{
		//���ûתȫ����
		if(eReadWrite == TMC_READ) {
			puData->lData = tAxisParamDefault.usFullStepPerRound[eTMC];
		}else if(eReadWrite == TMC_WRITE) {
			if(puData->lData != tAxisParamDefault.usFullStepPerRound[eTMC])
			{
				tAxisParamDefault.usFullStepPerRound[eTMC] = puData->lData;
				//������µĲ���
				Update_Enc_ConstValue(eTMC, &tAxisParamDefault, 0);
				ucSaveFlag = 1;
//				LOG_Info("Full Step Per Round=%d", puData->lData);
			}
		}
	}
	break;
		
		
	/*** ���������� ***/		
	case 0x30:
		// ��ʼ�ٶ� Velocity VSTART
		if(eReadWrite == TMC_READ) {
			puData->lData = tAxisParamDefault.lVStart[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			if(tAxisParamDefault.lVStart[eTMC] != puData->lData)
			{
				tAxisParamDefault.lVStart[eTMC] = puData->lData;
				ucSaveFlag = 1;
			}
		}
		break;
	case 0x31:
		// ��һ�׶μ��ٶ�A1  Acceleration A1
		if(eReadWrite == TMC_READ) {
			puData->lData = tAxisParamDefault.lA1[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			if(tAxisParamDefault.lA1[eTMC] != puData->lData)
			{
				tAxisParamDefault.lA1[eTMC] = puData->lData;
				ucSaveFlag = 1;
			}
		}
		break;
	case 0x32:
		// �����ٶ� Velocity V1
		if(eReadWrite == TMC_READ) {
			puData->lData = tAxisParamDefault.lV1[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			if(tAxisParamDefault.lV1[eTMC] != puData->lData)
			{
				tAxisParamDefault.lV1[eTMC] = puData->lData;
				ucSaveFlag = 1;
			}
		}
		break;		
	case 0x33:
		// �ڶ��׶μ��ٶ� AMAX  Maximum acceleration������λ��ģʽ
		if(eReadWrite == TMC_READ) {
			puData->lData = tAxisParamDefault.lAMax[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			if(tAxisParamDefault.lAMax[eTMC] != puData->lData)
			{
				tAxisParamDefault.lAMax[eTMC] = puData->lData;
				ucSaveFlag = 1;
			}
		}
		break;
	case 0x34:
		// Ŀ�꣨����ٶ� VMAX  Target speed������λ��ģʽ
		if(eReadWrite == TMC_READ) {
			puData->lData = tAxisParamDefault.lVMax[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			if(tAxisParamDefault.lVMax[eTMC] != abs(puData->lData))
			{
				tAxisParamDefault.lVMax[eTMC] = abs(puData->lData);
				ucSaveFlag = 1;
			}
		}
		break;
	case 0x35:
		//�ڶ��׶μ��ٶ� DMAX, Maximum Deceleration
		if(eReadWrite == TMC_READ) {
			puData->lData = tAxisParamDefault.lDMax[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			if(tAxisParamDefault.lDMax[eTMC] != puData->lData)
			{
				tAxisParamDefault.lDMax[eTMC] = puData->lData;
				ucSaveFlag = 1;
			}
		}
		break;		
	case 0x36:
		//��һ�׶μ��ٶ� Deceleration D1
		if(eReadWrite == TMC_READ) {			
			puData->lData = tAxisParamDefault.lD1[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			if(tAxisParamDefault.lD1[eTMC] != puData->lData)
			{
				tAxisParamDefault.lD1[eTMC] = puData->lData;
				ucSaveFlag = 1;
			}
		}
		break;			
	case 0x37:
		// ֹͣ�ٶ� Velocity VSTOP
		if(eReadWrite == TMC_READ) {
			puData->lData = tAxisParamDefault.lVStop[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			if(tAxisParamDefault.lVStop[eTMC] != puData->lData)
			{
				tAxisParamDefault.lVStop[eTMC] = puData->lData;
				ucSaveFlag = 1;
			}
		}
		break;
	case 0x38:
		// Waiting time after ramp down
		if(eReadWrite == TMC_READ) {			
			//puData->lData = g_taTMC5160[eTMC].laShadowRegister[TMC5160_TZEROWAIT];
		} else if(eReadWrite == TMC_WRITE) {
			
			//g_taTMC5160[eTMC].laShadowRegister[TMC5160_TZEROWAIT] = puData->lData;
			//ucSaveFlag = 1;
		}
		break;		
	case 0x40:
		// �ٶ�ģʽ�£����ٶ�ֵ
		if(eReadWrite == TMC_READ) {
			puData->lData = tAxisParamDefault.lAMax_VMode[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			if(tAxisParamDefault.lAMax_VMode[eTMC] != puData->lData)
			{
				tAxisParamDefault.lAMax_VMode[eTMC] = puData->lData;
				ucSaveFlag = 1;
			}
		}
		break;
	case 0x41:
		// �ٶ�ģʽ�£�Ŀ�꣨����ٶ� VMAX  Target speed
		if(eReadWrite == TMC_READ) {
			puData->lData = tAxisParamDefault.lVMax_VMode[eTMC];
		} else if(eReadWrite == TMC_WRITE){
			if(tAxisParamDefault.lVMax_VMode[eTMC] != abs(puData->lData))
			{
				tAxisParamDefault.lVMax_VMode[eTMC] = abs(puData->lData);
				ucSaveFlag = 1;
			}
		}
		break;
	

	/***  ������λ  ***/
	case 0x54:
		// ����λ���� right limit switch polarity
		if(eReadWrite == TMC_READ) {
			puData->lData = tAxisParamDefault.ucRighLimitPolarity[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			if(puData->lData == 0 || puData->lData == 1)
			{
				if(tAxisParamDefault.ucRighLimitPolarity[eTMC] != puData->lData)
				{
					tAxisParamDefault.ucRighLimitPolarity[eTMC] = puData->lData;
					ucSaveFlag = 1;
				}
			}else{
				LOG_Error("Data=%d Is Error", puData->lData);
				return ERROR_TYPE_DATA;
			}
		}
		break;
	case 0x55:
		// ����λ���� left limit switch polarity
		if(eReadWrite == TMC_READ) {
			puData->lData = tAxisParamDefault.ucLeftLimitPolarity[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			if(puData->lData == 0 || puData->lData == 1)
			{
				if(tAxisParamDefault.ucLeftLimitPolarity[eTMC] != puData->lData)
				{
					tAxisParamDefault.ucLeftLimitPolarity[eTMC] = puData->lData;
					ucSaveFlag = 1;
				}
			}else{
				LOG_Error("Data=%d Is Error", puData->lData);
				return ERROR_TYPE_DATA;
			}
		}
		break;


	case 0x59:
		// �������
		if(eReadWrite == TMC_READ) {
			puData->lData = tAxisParamDefault.ucRotateDirect[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			if(puData->lData == 0 || puData->lData == 1)
			{
				if(tAxisParamDefault.ucRotateDirect[eTMC] != puData->lData)
				{
					tAxisParamDefault.ucRotateDirect[eTMC] = puData->lData;
					ucSaveFlag = 1;
				}
				
			}else{
				
				LOG_Error("unKnow Motor Rotate Direct %d", puData->lData);
				return ERROR_TYPE_DATA;
			}
		}
		break;
		default:
		{
			LOG_Error("unKnow Type");
			eErrorType = ERROR_TYPE_TYPE;
		}
		break;
	}
	

	/* ���� */
	if(ucSaveFlag == 1)
	{
		//���¼���CRC
		tAxisParamDefault.usCrc = CRC16((uint8_t*)&tAxisParamDefault, sizeof(AxisParamDefault_t)-2);
	
		//���������
		//Param_Write(EN_SAVE_PARAM_TYPE_AXIS, (uint8_t*)&g_tAxisParamDefault, sizeof(AxisParamDefault_t));
		Save_Axis_Param_Default(&tAxisParamDefault);
	}
	return eErrorType;
}





/*
*	���EEPROM����������
*/
void ClearAndSave_Default_Axis_Params(void)
{
	Axis_Param_Fixed_SetDefault_Value(&g_tAxisParamDefault);
	//Param_Write(EN_SAVE_PARAM_TYPE_AXIS, (uint8_t*)&g_tAxisParamDefault, sizeof(AxisParamDefault_t));
	Save_Axis_Param_Default(&g_tAxisParamDefault);
}








/* 
* 
*  ��ȡĬ������� 
*
*/
ErrorType_e Read_Axis_Param_Default(__IO AxisParamDefault_t *ptAxisParamDefault)
{
	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
	uint8_t ucNum = 0;
	uint16_t usCrc = 0;
	
	//��ȡ����	
	do{
		//����
		ucNum++;
		
		memset((void*)ptAxisParamDefault, 0, sizeof(AxisParamDefault_t));
		Param_Read(EN_SAVE_PARAM_TYPE_AXIS, (uint8_t*)ptAxisParamDefault, sizeof(AxisParamDefault_t));
		usCrc = CRC16((uint8_t*)ptAxisParamDefault, sizeof(AxisParamDefault_t)-2);
		
		//����У��
		if(usCrc == ptAxisParamDefault->usCrc)
		{
			return ERROR_TYPE_SUCCESS;			
		}else{
			rt_thread_delay(2);//HAL_Delay(2);
		}
		
	}while(ucNum < 3);

	
	//��һ���ϵ�
	if(usCrc != ptAxisParamDefault->usCrc && ptAxisParamDefault->ulInitFlag != PARAM_INIT_FLAG)
	{
		Axis_Param_Fixed_SetDefault_Value(ptAxisParamDefault);
		
		//���������
		return Save_Axis_Param_Default(ptAxisParamDefault);
	}

	
	//����У����
	if(ucNum >= 3)
	{
		//����У��ʧ��
//		LOG_Warn("EEPROM Axis Default Param CRC Error");
		return ERROR_TYPE_EEPROM;
	}
	
	return eErrorType;
}


/* 
* 
*  ����Ĭ������� 
*
*/
ErrorType_e Save_Axis_Param_Default(__IO AxisParamDefault_t *ptAxisParamDefault)
{
	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
	uint8_t  ucNum = 0;
	uint16_t usWriteCrc = 0, usReadCrc = 0;
	AxisParamDefault_t  tAxisParamDefault = {0};
	
	//����ԭʼ����CRC
	usWriteCrc = ptAxisParamDefault->usCrc;
	
	//��ȡ����	
	do{
		//����
		ucNum++;
		
		//д��
		memmove((void*)&tAxisParamDefault, (void*)ptAxisParamDefault, sizeof(AxisParamDefault_t));
		Param_Write(EN_SAVE_PARAM_TYPE_AXIS, (uint8_t*)&tAxisParamDefault, sizeof(AxisParamDefault_t));
			
		//��ȡ
		memset((void*)&tAxisParamDefault, 0, sizeof(AxisParamDefault_t));
		Param_Read(EN_SAVE_PARAM_TYPE_AXIS, (uint8_t*)&tAxisParamDefault, sizeof(AxisParamDefault_t));
		usReadCrc = CRC16((uint8_t*)&tAxisParamDefault, sizeof(AxisParamDefault_t)-2);
		
		//�Ա�ǰ��crc
		if(usWriteCrc == usReadCrc || usReadCrc == tAxisParamDefault.usCrc)
		{
			return ERROR_TYPE_SUCCESS;
		}else{
			rt_thread_delay(2);//HAL_Delay(2);
		}
		
	}while(ucNum < 3);  //����ظ�3��

	//����ʧ��
	if(ucNum >= 3)
	{
		LOG_Error("Save Process Fail");
		return ERROR_TYPE_EEPROM;
	}

	return eErrorType;
}





/*
*	���̲��� ���
*/
void Process_Param_SetDefault_Value(__IO Process_t *ptProcess)
{
	//
	memset((void*)ptProcess, 0, sizeof(Process_t));
	//
	ptProcess->ulInitFlag = PARAM_INIT_FLAG;
	ptProcess->ulExecNum  = 0;
	ptProcess->eProcessStatus = EN_PROCESS_STATUS_CLEAR;
	ptProcess->usCrc = CRC16((uint8_t*)ptProcess, sizeof(Process_t)-2);
}




/*
*	���EEPROM���������
*/
void ClearAndSave_Default_Process(void)
{
	Process_Param_SetDefault_Value(&g_tProcess);
	Param_Write(EN_SAVE_PARAM_TYPE_PROCESS, (uint8_t*)&g_tProcess, sizeof(Process_t));
}




/*
*	���̳�ʼ��
*/
ErrorType_e Process_Init(void)
{
	return Read_Process(&g_tProcess);
}




  
/*
*	����RAM�����̣�����Index������������̣�������˳����밴ָ���˳��
*/
ErrorType_e Set_Process(RecvFrame_t *ptRecvFrame)
{
	ErrorType_e eError = ERROR_TYPE_SUCCESS;
	
	uint8_t ucIndex = ptRecvFrame->ucType & 0x7F;
	if(ucIndex > SUB_PROCESS_MAX_CMD_NUM)
	{
		//���֧��128ָ��
		LOG_Error("SubProcess Index=%d Error", ucIndex);
		return ERROR_TYPE_DATA;
	}
			
	if(ptRecvFrame->ucType & 0x80)
	{
		//����
		//if(g_tProcess.taSubProcess[ucIndex].ucParamNum != 0xFF && g_tProcess.taSubProcess[ucIndex].ucParamNum >= SUB_PROCESS_MAX_PARAM_NUM)
		if( g_tProcess.taSubProcess[ucIndex].ucParamNum >= SUB_PROCESS_MAX_PARAM_NUM)
		{
			//�������������3��
			LOG_Error("SubProcess Parma Index=%d Error", g_tProcess.taSubProcess[ucIndex].ucParamNum);
			return ERROR_TYPE_EXEC;
		}
		
		//��ȡ����
		g_tProcess.taSubProcess[ucIndex].uParam[g_tProcess.taSubProcess[ucIndex].ucParamNum++].lData = ptRecvFrame->uData.lData;
	
	}else{
		//ָ��
		memset((void*)&g_tProcess.taSubProcess[ucIndex], 0, sizeof(SubProcess_t));
		g_tProcess.taSubProcess[ucIndex].ucCmd = ptRecvFrame->uData.ucData[0];
		//�޸Ļ�ֵָ��ʱ����������������
		//g_tProcess.taSubProcess[ucIndex].ucParamNum = 0xFF;
	
	}
	
	return eError;
}







/*
*	��ȡ RAM����
*/
ErrorType_e Get_Process(uint8_t ucIndex, SubProcess_t *ptSubProcess)
{
	//
	if(ucIndex > SUB_PROCESS_MAX_CMD_NUM)
	{
		//���֧��128ָ��
		LOG_Error("SubProcess Index=%d Error", ucIndex);
		return ERROR_TYPE_DATA;
	}
		
	//��ȡָ��
	memmove((void*)ptSubProcess, (void*)&g_tProcess.taSubProcess[ucIndex], sizeof(SubProcess_t));
	return ERROR_TYPE_SUCCESS;
	
}
	




/*
*	ִ������--����
*/

ErrorType_e Exec_Process_Ctrl(uint8_t ucType)
{
	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
	
	//
	switch(ucType)
	{
		case 0x00:
		{
			//ֹͣ
			g_tProcess.eProcessStatus = EN_PROCESS_STATUS_STOP;
		}
		break;
		case 0x01:
		{
			//����
			g_tProcess.eProcessStatus = EN_PROCESS_STATUS_EXEC;
		}
		break;		
		default:
		{
			LOG_Error("unKnow Type=%d", ucType);
			return ERROR_TYPE_TYPE;
		}
		//break;
	}
	
	return eErrorType;
}





/*
*	��ȡ����
*/
ErrorType_e Read_Process(__IO Process_t *ptProcess)
{
	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
	uint8_t ucNum = 0;
	uint16_t usCrc = 0;

	//��ȡ����	
	do{
		//����
		ucNum++;
		
		memset((void*)ptProcess, 0, sizeof(Process_t));
		Param_Read(EN_SAVE_PARAM_TYPE_PROCESS, (uint8_t*)ptProcess, sizeof(Process_t));
		usCrc = CRC16((uint8_t*)ptProcess, sizeof(Process_t)-2);
		
		
		//����У��
		if(usCrc == ptProcess->usCrc)
		{
			return ERROR_TYPE_SUCCESS;			
		}else{
			rt_thread_delay(2);//HAL_Delay(2);
		}
	}while(ucNum < 3);
	
	
	//��һ���ϵ�
	if(usCrc != ptProcess->usCrc && ptProcess->ulInitFlag != PARAM_INIT_FLAG)
	{
		Process_Param_SetDefault_Value(ptProcess);
		//���������
		return Save_Process(ptProcess);
	}
	

	//����У����
	if(ucNum >= 3)
	{
//		LOG_Warn("EEPROM Process Data CRC Error");
		return ERROR_TYPE_EEPROM;
	}
	
	return eErrorType;
}


/*
*	�������̵�EEPROM��
*/
ErrorType_e Save_Process(__IO Process_t *ptProccess)
{
	//
	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
	uint8_t ucNum = 0;
	uint16_t usWriteCrc = 0, usReadCrc = 0;
	Process_t tProcess = {0};
	
	usWriteCrc = ptProccess->usCrc;
	//����
	do{
		//����
		ucNum++;
		
		//д��
		memmove((void*)&tProcess, (void*)ptProccess, sizeof(Process_t));
		Param_Write(EN_SAVE_PARAM_TYPE_PROCESS, (uint8_t*)&tProcess, sizeof(Process_t));
		
		//��ȡ
		memset((void*)&tProcess, 0, sizeof(Process_t));
		Param_Read(EN_SAVE_PARAM_TYPE_PROCESS, (uint8_t*)&tProcess, sizeof(Process_t));
		usReadCrc = CRC16((uint8_t*)&tProcess, sizeof(Process_t)-2);
		
		//�Ա�ǰ��crc
		if(usWriteCrc == usReadCrc || usReadCrc == tProcess.usCrc)
		{
			return ERROR_TYPE_SUCCESS;
		}else{
			rt_thread_delay(2);//HAL_Delay(2);
		}
		
	}while(ucNum < 3);  //����ظ�3��
	
	
	//����ʧ��
	if(ucNum >= 3)
	{
		LOG_Error("Save Process Fail");
		return ERROR_TYPE_EEPROM;
	}
		
	return eErrorType;
}




/*
*	ɾ��EEPROM����������
*/
void Del_Process(void)
{
	//ɾ��
	memset((void*)&g_tProcess, 0, sizeof(Process_t));
	g_tProcess.ulInitFlag	  = PARAM_INIT_FLAG;
	g_tProcess.eProcessStatus = EN_PROCESS_STATUS_CLEAR;
	g_tProcess.usCrc          = CRC16((uint8_t*)&g_tProcess, sizeof(Process_t)-2);
	
	//
	Param_Write(EN_SAVE_PARAM_TYPE_PROCESS, (uint8_t*)&g_tProcess, sizeof(Process_t));
}



/*
*	���RAM�е���������
*/
void Clear_Process(void)
{
	//
	memset((void*)&g_tProcess, 0, sizeof(Process_t));
	g_tProcess.eProcessStatus = EN_PROCESS_STATUS_CLEAR;
	
}	



/*
*	ִ������
*/
void Exec_Process(void)
{
	g_tProcess.eProcessStatus = EN_PROCESS_STATUS_EXEC;
}


  
/*
*	ֹͣ����
*/
void Stop_Process(void)
{
	//
	g_tProcess.eProcessStatus = EN_PROCESS_STATUS_STOP;
	
}





/*
*	ִ������--����
*/

ErrorType_e Exec_Process_Clear_Or_Save(uint8_t ucType)
{
	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
	
	//
	switch(ucType)
	{	
		case 0x00:
		{
			//���RAM�е�ִ������
			Clear_Process();
		}
		break;
		case 0x01:
		{
			//���RAM��EEPROM�е�ִ������
			Del_Process();
		
		}
		break;		
		case 0x02:
		{
			//��RAMִ�����̱��浽EEPROM��
			g_tProcess.ulInitFlag = PARAM_INIT_FLAG;
			g_tProcess.eProcessStatus = EN_PROCESS_STATUS_READY;
			g_tProcess.usCrc = CRC16((uint8_t*)&g_tProcess, sizeof(Process_t)-2);
			//
			Save_Process(&g_tProcess);
		}
		break;
		default:
		{
			LOG_Error("unKnow Type=%d", ucType);
			return ERROR_TYPE_TYPE;
		}
		//break;
	}
	
	return eErrorType;
}






/*
*	���̴���
*/
ErrorType_e Process_Handle(uint32_t ulTicks)
{
	extern __IO  AxisParamDefault_t g_tAxisParamDefault;
	extern BoardStatus_t g_tBoardStatus;
	extern GlobalParam_t g_tGlobalParam;
	extern __IO TMCStatus_t g_tTMCStatus;
	static ProcessStatus_e s_eProcessStatus = EN_PROCESS_STATUS_READY;
	static uint8_t s_ucIndex = 0;	//����ת�ı�ǩ
	static uint8_t s_ucFlag = 0;	//ִ������ָ���־
	static uint32_t s_ulTick = 0;	//Tick�δ��ʱ
	static Data4Byte_u s_uResult = {0};
	
	uint8_t ucType = 0;
	ErrorType_e eError = ERROR_TYPE_SUCCESS;
	SubProcess_t tSubProcess = {0};
	TMC_e eTMC = TMC_0;
	int32_t  lValue  = 0;
	uint32_t ulValue = 0;
	//float 	 fValue  = 0;
	
	
	
//	if((s_eProcessStatus == EN_PROCESS_STATUS_READY && g_tProcess.eProcessStatus == EN_PROCESS_STATUS_EXEC) || \
//	   (s_eProcessStatus == EN_PROCESS_STATUS_IN_EXEC && g_tProcess.eProcessStatus == EN_PROCESS_STATUS_STOP) || \
//		)
	
	
	if(EN_PROCESS_STATUS_EXEC == g_tProcess.eProcessStatus)
	{
		//ִ������
		
		//��������Ƿ�Ϊ��
//		if(0 == g_tProcess.ucSubProcessNum)
//		{
//			LOG_Warn("SubProcess is Empty, Num=%d", g_tProcess.ucSubProcessNum);
//			return eError;
//		}
		
		//ִ��������
//		SubProcess_Handle(ulTicks, &g_tProcess.taSubProcess[s_ucIndex]);
			
		//
		memmove((void*)&tSubProcess, (void*)&g_tProcess.taSubProcess[s_ucIndex], sizeof(SubProcess_t));
		//if(tSubProcess.ucDeviceID > ) LOG_Warn("DeviceID=%d is Error", tSubProcess.ucDeviceID);
		
//		/* ������� */
//		if(tSubProcess.ucDeviceID >= TMC_MODULE_END && (tSubProcess.ucCmd != CMD_GET_IO_STATUS || \
//			tSubProcess.ucCmd != CMD_SET_IO_STATUS || tSubProcess.ucCmd != CMD_SET_GLOBAL_PARAM ||  \
//			tSubProcess.ucCmd != CMD_GET_GLOBAL_PARAM)) 
//		{
//			LOG_Error("TMC Device ID=%d is Error", eTMC);
//			// return error msg
//		}else{
//			eTMC = (TMC_e)tSubProcess.ucDeviceID;	
//		}
		 
		
		//
		switch(tSubProcess.ucCmd)
		{
			/**********************/
			case CMD_ROTATE:  //0x10
			{
//				//��һ�����У����������õ�������
//				if(g_tBoardStatus.ucMotorMoveFlag[eTMC] == 0)
//				{
//					TMC5160_I_Set(eTMC);
//					g_tBoardStatus.ucMotorMoveFlag[eTMC] = 1;				
//				}
				//��ת 
				eTMC   = (TMC_e)tSubProcess.uParam[0].ucData[0];
				ucType = tSubProcess.uParam[1].ucData[0];
				lValue = tSubProcess.uParam[2].lData;
				TMC_Rotate(eTMC, (RotateDirect_e)ucType, lValue);
				
				TMC_SetVMode_V(eTMC, 0);
			}
			break;
			case CMD_MOVE_POSITION_WITHOUT_ENC:  //0x11
			{
				//�رձ�����ʧ����⹦��
				TMC_WriteInt(eTMC, TMC5160_ENC_DEVIATION, 0);
				
				//��һ�����У����������õ�������
//				if(g_tBoardStatus.ucMotorMoveFlag[eTMC] == 0)
//				{
//					TMC5160_I_Set(eTMC);
//					g_tBoardStatus.ucMotorMoveFlag[eTMC] = 1;				
//				}
			
				//�ƶ�	
				eTMC   = (TMC_e)tSubProcess.uParam[0].ucData[0];
				ucType = tSubProcess.uParam[1].ucData[0];
				lValue = tSubProcess.uParam[2].lData;
				
				TMC_WriteInt(eTMC, TMC5160_ENC_DEVIATION, 0); //�رձ�����ʧ����⹦��
				if(0 == ucType)
				{
					//����ƫ��
					TMC_MoveTo(eTMC, lValue);
				}else if(1 == ucType){
					//���ƫ��
					TMC_MoveBy(eTMC, lValue);
				}else if(2 == ucType){
					//����ֵ���ƶ�
					TMC_MoveTo(eTMC, s_uResult.lData);
					
				}
				TMC_SetPMode_V(eTMC, 0);			
			}
			break;
			case CMD_MOVE_POSITION_WITH_ENC:  //0x12
			{
				//������ʧ����ֵ, ��ֵΪ�㣬��رոù���			
				TMC_WriteInt(eTMC, TMC5160_ENC_DEVIATION, g_tAxisParamDefault.lEncDiff_Threshold[eTMC]);
	//			LOG_Info("ENC Deviation Start: Motor=%d, EncDiff=%d, Steps=%d, CurStep=%d", \
						eTMC, g_tAxisParamDefault.lEncDiff_Threshold[eTMC], ptRecvFrame->uData.lData, TMC5160_ReadInt(eTMC, TMC5160_XACTUAL));
		
				//��һ�����У����������õ�������
//				if(g_tBoardStatus.ucMotorMoveFlag[eTMC] == 0)
//				{
//					TMC5160_I_Set(eTMC);
//					g_tBoardStatus.ucMotorMoveFlag[eTMC] = 1;				
//				}
			
				//�ƶ�		
				eTMC   = (TMC_e)tSubProcess.uParam[0].ucData[0];
				ucType = tSubProcess.uParam[1].ucData[0];
				lValue = tSubProcess.uParam[2].lData;
				
				TMC_WriteInt(eTMC, TMC5160_ENC_DEVIATION, g_tAxisParamDefault.lEncDiff_Threshold[eTMC]); //���ñ�����ʧ����⹦��
				if(0 == ucType)
				{
					//����ƫ��
					TMC_MoveTo(eTMC, lValue);
				}else if(1 == ucType){
					//���ƫ��
					TMC_MoveBy(eTMC, lValue);
				}else if(2 == ucType){
					//����ֵ���ƶ�
					TMC_MoveTo(eTMC, s_uResult.lData);
					
				}
				TMC_SetPMode_V(eTMC, 0);
			}
			break;
			case CMD_STOP:   //0x13
			{
				//ֹͣ
				eTMC   = (TMC_e)tSubProcess.uParam[0].ucData[0];
				TMC_Stop(eTMC);
			}
			break;
			case CMD_MOTOR_RESET: //0x14
			{
				//��λ����ԭ�㣨�ο�λ�ã�
				eTMC   = (TMC_e)tSubProcess.uParam[0].ucData[0];
				TMC_Reset(eTMC, TMC_REF_LEFT);  //TMC_REF_RIGHT   TMC_REF_LEFT
			}
			break;
			
			
			/**********************/
			case CMD_QUERY_BOARD_TYPE:  //0x21
			{
				//��ѯ�忨����
				s_uResult.ulData = Get_Module_Type();				
			}
			break;		
			case CMD_HARD_SOFT_VERSION:  //0x22
			{
				//��ѯ��Ӳ���汾
				Get_Soft_HardWare_Version(s_uResult.ucData);				
			}
			break;
			
			
			/**********************/
			case CMD_SET_AXIS_PARAM:  //0x30
			{
				//���������
				eTMC   = (TMC_e)tSubProcess.uParam[0].ucData[0];
				ucType = tSubProcess.uParam[1].ucData[0];
				
				TMC_AxisParam(eTMC, TMC_WRITE, ucType, &tSubProcess.uParam[2]);  
				//LOG_Debug("Axis Param Add(%X)  %d", ucType, tSubProcess.uParam[2].lData);

			}
			break;
			case CMD_GET_AXIS_PARAM:  //0x31
			{
				//��ȡ�����
				eTMC   = (TMC_e)tSubProcess.uParam[0].ucData[0];
				ucType = tSubProcess.uParam[1].ucData[0];
				
				TMC_AxisParam(eTMC, TMC_READ, ucType, &s_uResult);  
			}
			break;
			case CMD_SET_GLOBAL_PARAM:  //0x33
			{
				//����ȫ�ֲ���
				Bank_e eBank   = (Bank_e)tSubProcess.uParam[0].ucData[0];
				ucType = tSubProcess.uParam[1].ucData[0];
				lValue = tSubProcess.uParam[2].lData;
				
				TMC_Global_Param(eBank, TMC_WRITE, ucType, &tSubProcess.uParam[2]);
			}
			break;
			case CMD_GET_GLOBAL_PARAM:  //0x34
			{
				//��ѯģ�����
				Bank_e eBank   = (Bank_e)tSubProcess.uParam[0].ucData[0];
				ucType = tSubProcess.uParam[1].ucData[0];
				//lValue = tSubProcess.uParam[2].lData;
				
				TMC_Global_Param(eBank, TMC_READ, ucType, &s_uResult);
			}
			break;			
			case CMD_SET_IO_STATUS:  //0x34
			{
				//����IO״̬
				uint8_t ucIO_ID = tSubProcess.uParam[0].ucData[0];
				lValue = tSubProcess.uParam[1].ucData[0];
				
//@todo				Set_Out_IO(ucIO_ID, (GPIO_PinState)lValue);
			}
			break;
			case CMD_GET_INPUT_IO_STATUS:
			{
//@todo				Get_In_IO_One(tSubProcess.uParam[0].ucData[0], &s_uResult.ucData[0]);
			
			}
			break;
			case CMD_GET_OUTPUT_IO_STATUS:
			{
//@todo				Get_Out_IO_One(tSubProcess.uParam[0].ucData[0], &s_uResult.ucData[0]);
			}
			break;
			
//			case CMD_GET_IO_STATUS:  //0x35
//			{
//				//��ѯIO״̬
//				uint16_t usOutState = 0, usInState = 0;
//			
//				//��ȡ���IO״̬
//				Get_Out_IO(&usOutState);
//				//��ȡ����IO״̬
//				Get_In_IO(&usInState);
//				
//				//IO״̬
//				s_uResult.ucData[0] = usOutState;
//				s_uResult.ucData[1] = usOutState >> 8;;
//				s_uResult.ucData[2] = usInState;
//				s_uResult.ucData[3] = usInState >> 8;;
//				
//			}
//			break;	
			
			
		/**********************/
		case CMD_EXEC_PROCESS_CTRL:  //0x42
		{
			ucType = tSubProcess.uParam[0].ucData[0];
			Exec_Process_Ctrl(ucType);
		}
		break;

			
			/**********************/
//			case CMD_ERROR_HANDLE:
//			{
//				
//				Module_Error_Handle(tSubProcess.ucDeviceID, s_tResult.uResult.ulValue);
//			}
//			break;
			
			
			/**********************/
			case CMD_LABEL:
			{
				//��ת��ǩ, ��ǩ���Ĳ���:label_index���л�����subprocess
				//s_ucIndex++;
				s_ucFlag = 0;
			
			}
			break;
			case CMD_WAIT:
			{
				eTMC   = (TMC_e)tSubProcess.uParam[0].ucData[0];
				ucType = tSubProcess.uParam[1].ucData[0];
				lValue = tSubProcess.uParam[2].lData;
				
				//
			    if(0 == ucType){ //wait reset finished			
					if(1 == g_tTMCStatus.tMotorResetInfo[eTMC].eResetStatus)
					{
						//��λ���
						//LOG_Info("Motor Reset Finished");
						s_ucFlag = 0;
						
					}else{
						//δ���
						s_ucFlag = 1;					
					}	
				}else if(1 == ucType){ //wait move finished						
					if(1 == TMC5160_FIELD_READ(eTMC, TMC5160_RAMPSTAT, TMC5160_RAMPSTAT_POS_REACH_MASK, TMC5160_RAMPSTAT_POS_REACH_SHIFT))
					{
						//move �������
						//LOG_Info("Move Step Finished");
						s_ucFlag = 0;
						
					}else{
						//δ���
						s_ucFlag = 1;					
					}
				}else if(2 == ucType){ //wait speed reach max value
					if(1 == TMC5160_FIELD_READ(eTMC, TMC5160_RAMPSTAT, TMC5160_RAMPSTAT_VELOCITY_IS_ZERO_MASK, TMC5160_RAMPSTAT_VELOCITY_IS_ZERO_SHIFT))
					{
						//��������ٶ�
						//LOG_Info("Reach Max Speed");
						s_ucFlag = 0;
						
					}else{
						//δ���
						s_ucFlag = 1;					
					}
				}else if(3 == ucType){ //wait speed = 0
					if(0 == TMC5160_ReadInt(eTMC, TMC5160_VACTUAL))
					{
						//�ٶ�Ϊ��
						//LOG_Info("Speed Is ZERO");
						s_ucFlag = 0;
						
					}else{
						//δ���
						s_ucFlag = 1;					
					}
				}
			}
			break;
			case CMD_DELAY:
			{
				if(0 == s_ucFlag)
				{
					//��ʱδ�ﵽ
					ulValue = tSubProcess.uParam[0].ulData;
					s_ulTick = ulTicks + ulValue;
					s_ucFlag = 1;
					
				}else{
					if(ulTicks >= s_ulTick)
					{
						//��ʱ����
						s_ucFlag = 0;
						s_ulTick = ulTicks;
						//s_ucIndex++;
					}
				}
			}
			break;
			case CMD_CALC:
			{
				//ֵ����
				ucType = tSubProcess.uParam[0].ucData[0];
				lValue = tSubProcess.uParam[1].lData;
				
				//��һ�ν�� tResult.uResult.ulValue ;
				switch(ucType)
				{
					case EN_CALC_ADD: //
					{
						s_uResult.ulData += lValue;
					}
					break;
					case EN_CALC_SUB:
					{
						s_uResult.ulData -= lValue;
					}
					break;
					case EN_CALC_MUL:
					{
						s_uResult.ulData *= lValue;
					}
					break;
					case EN_CALC_DIV:
					{
						if(lValue != 0)
							s_uResult.ulData /= lValue;
					}
					break;
					case EN_CALC_MOD:
					{
						if(lValue < 0)
							s_uResult.ulData = -lValue;
						else 
							s_uResult.ulData = lValue;
					}
					break;
					case EN_CALC_AND:
					{
						s_uResult.ulData &= lValue;
					}
					break;
					case EN_CALC_OR:
					{
						s_uResult.ulData |= lValue;
					}
					break;
					case EN_CALC_XOR:
					{
						s_uResult.ulData ^= lValue;
					}
					break;
					case EN_CALC_NOT:
					{
						s_uResult.ulData = !lValue;
					}
					break;		
					case EN_CALC_RIGHT_LIFT:
					{
						//��λ��
						s_uResult.ulData >>= lValue;
					}
					break;
					case EN_CALC_LEFT_LIFT:
					{
						//��λ��
						s_uResult.ulData <<= lValue;
					
					}
					break;
					default:
					{
						g_tTMCStatus.ucExecProcessStatus = 1;
						LOG_Error("CMD_CALC unkonwn Type=%d", ucType);
					}
				}
			}
			break;
			case CMD_JA:
			{
				//��������ת
				uint8_t ucLabelIndex = tSubProcess.uParam[0].ucData[0];
			    s_ucIndex = ucLabelIndex;
			}
			break;
			case CMD_JC:
			{
				//������ת
				//ֵ�Ƚ�
				ucType = tSubProcess.uParam[0].ucData[0]; //�ȽϷ�ʽ
				lValue = tSubProcess.uParam[1].lData; 	  //�Ƚ�ֵ
				uint8_t ucLabelIndex = tSubProcess.uParam[2].ucData[0];	//��ǩindex
				//LOG_Debug("Type=%d, CompareV=%d, Label=%d, Result=%d",ucType, lValue, ucLabelIndex, s_uResult.lData);
				switch(ucType)
				{
					case EN_COMPARE_JC_EQ: //����
					{
						if(s_uResult.ulData == lValue)
						{
							s_ucIndex = ucLabelIndex;
						}
					}
					break;
					case EN_COMPARE_JC_NE: //������
					{
						if(s_uResult.ulData != lValue)
						{
							s_ucIndex = ucLabelIndex;
						}
					}
					break;
					case EN_COMPARE_JC_GT: //����
					{
						if(s_uResult.ulData > lValue)
						{
							s_ucIndex = ucLabelIndex;
						}
					}
					break;
					case EN_COMPARE_JC_GE: //���ڵ���
					{
						if(s_uResult.ulData >= lValue)
						{
							s_ucIndex = ucLabelIndex;
						}
					}
					break;
					case EN_COMPARE_JC_LT: //С��
					{
						if(s_uResult.ulData < lValue)
						{
							s_ucIndex = ucLabelIndex;
						}
					}
					break;
					case EN_COMPARE_JC_LE: //С�ڵ���
					{
						if(s_uResult.ulData <= lValue)
						{
							s_ucIndex = ucLabelIndex;
						}
					}
					break;
//					case EN_COMPARE_JC_ETO:
//					{
//					
//					}
//					break;
//					case EN_COMPARE_JC_EAL:
//					{
//					
//					}
//					break;
//					case EN_COMPARE_JC_EDV:
//					{
//					
//					}
//					break;
//					case EN_COMPARE_JC_EPO:
//					{
//					
//					}
//					break;
					default:
					{
						g_tTMCStatus.ucExecProcessStatus = 2;
						LOG_Error("unKonwn Type=%d", ucType);
					}
					break;
				}
			}
			break;
			case CMD_SET_AXIS_PARAM_AAP:  //0x97
			{
				//���������
				eTMC   = (TMC_e)tSubProcess.uParam[0].ucData[0];
				TMC_AxisParam(eTMC, TMC_WRITE, ucType, &s_uResult);  
				//LOG_Debug("Axis Param Add(%X)  %d", ucType, tSubProcess.uParam[2].lData);

			}
			break;
			case CMD_SET_GLOBAL_PARAM_AGP:  //0x98
			{
				//����ȫ�ֲ���
				Bank_e eBank   = (Bank_e)tSubProcess.uParam[0].ucData[0];
				ucType = tSubProcess.uParam[1].ucData[0];
				
				
				TMC_Global_Param(eBank, TMC_WRITE, ucType, &s_uResult);
			}
			break;
			case CMD_TIME_COUNT:
			{
				//������ʱ��
				ucType = tSubProcess.uParam[0].ucData[0]; //���
				ulValue = tSubProcess.uParam[1].ulData; //��ʱʱ��
				
				//��ʱ��Ϣ
				g_tProtcessTimeCount.ulStartFlag[ucType] = 1;
				g_tProtcessTimeCount.ulTriggerFlag[ucType] = 0;
				g_tProtcessTimeCount.ulStartTick[ucType] = rt_tick_get();//HAL_GetTick();
				g_tProtcessTimeCount.ulThreshTick[ucType] = ulValue;
						
			}
			break;
			case CMD_TIME_COUNT_A:
			{
				//������ʱ��--����ֵ��ʽ
				ucType = tSubProcess.uParam[0].ucData[0]; //���
				
				//��ʱ��Ϣ
				g_tProtcessTimeCount.ulStartFlag[ucType] = 1;
				g_tProtcessTimeCount.ulTriggerFlag[ucType] = 0;
				g_tProtcessTimeCount.ulStartTick[ucType] = rt_tick_get();//HAL_GetTick();
				g_tProtcessTimeCount.ulThreshTick[ucType] = s_uResult.ulData;									
			}
			break;
			case CMD_TIME_COUNT_CHECK:
			{
				//��ʱ����ʱ���
				ucType = tSubProcess.uParam[0].ucData[0]; //���
				if(g_tProtcessTimeCount.ulTriggerFlag[ucType] == 1)
				{
					//����
					s_uResult.ulData = 1;		
				}else{
					//δ����
					s_uResult.ulData = 0;					
				}
			}
			break;
			default:
			{
				g_tTMCStatus.ucExecProcessStatus = 3;
//				LOG_Warn("nuKnown CMD=%d", tSubProcess.ucCmd);
			}
			break;
			
		} //switch cmd
			
		//ִ����һ��ָ��
	    if(0 == s_ucFlag) s_ucIndex++; 
		
	}else if(EN_PROCESS_STATUS_STOP == g_tProcess.eProcessStatus){
		if(EN_PROCESS_STATUS_EXEC == s_eProcessStatus)
		{
			//��ִ�е�ֹͣ��ִ���ƺ���
			
		}
		s_ucIndex = 0;
		
	}else{
		//����״̬������Ҫִ��
		s_ucIndex = 0;
	}
	
	//ִ����һ�ֺ󣬴��¿�ʼִ��
//	if(s_ucIndex >= g_tProcess.ucSubProcessNum) s_ucIndex = 0;
	s_eProcessStatus = g_tProcess.eProcessStatus;
	
	//
	for(uint8_t i = 0; i < PROCESS_TIME_COUNT_NUM; i++)
	{
		if(1 == g_tProtcessTimeCount.ulStartFlag[i])
		{
			 //if(HAL_GetTick() > (g_tProtcessTimeCount.ulStartTick[i] + g_tProtcessTimeCount.ulThreshTick[i]))
			if(rt_tick_get() > (g_tProtcessTimeCount.ulStartTick[i] + g_tProtcessTimeCount.ulThreshTick[i]))
			 {
				g_tProtcessTimeCount.ulTriggerFlag[i] = 1;
				g_tProtcessTimeCount.ulStartFlag[i] = 0;
				g_tProtcessTimeCount.ulStartTick[ucType] = 0;
				g_tProtcessTimeCount.ulThreshTick[ucType] = 0;
			 }
		}
	}
	
	return eError;
}













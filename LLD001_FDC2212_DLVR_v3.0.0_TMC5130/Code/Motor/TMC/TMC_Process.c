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


//需保存的轴参数
__IO AxisParamDefault_t g_tAxisParamDefault = {0};
//流程
__IO Process_t 		 g_tProcess = {0};
//流程-计时器信息
__IO ProtcessTimeCount_t g_tProtcessTimeCount = {0};


/*
 * TMC Reset， 非堵塞执行
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
	
	//不需要执行复位操作
	if(g_tTMCStatus.ucMotorResetStartFlag == 0) return 0;
	
	//每隔1ms处理一次
	if(ulTick - s_ulTick >= 1)
	{
		s_ulTick = ulTick;
		for(TMC_e eTMC = TMC_0; eTMC < TMC_MODULE_END; eTMC++)
		{
			if(g_tTMCStatus.tMotorResetInfo[eTMC].eResetStatus == MOTOR_RESET_STATUS_ING)
			{
				/* 正在复位 */
				ucFlag = 1;
				switch(g_tTMCStatus.tMotorResetInfo[eTMC].eResetExec)
				{
					//
					case MOTOR_RESET_EXEC_1:
					{
						//复位起始tick
						g_tTMCStatus.tMotorResetInfo[eTMC].ulStartTick = ulTick;
						//速度为0
						TMC5160_WriteInt(eTMC, TMC5160_VMAX, 0);
						//清零复位模式
						//TMC5160_WriteInt(eTMC, TMC5160_SWMODE, 0);
						
						//检查光耦状体
						ucOC_Status = TMC5160_FIELD_READ(eTMC, TMC5160_RAMPSTAT, TMC5160_RAMPSTAT_LEFT_SW_TRIGGER_MASK, TMC5160_RAMPSTAT_LEFT_SW_TRIGGER_SHIFT);

#ifdef P_AXIS_REF_POLITY						
						if(eTMC == TMC_0 || eTMC == TMC_1)
						{							
							if(ucOC_Status == 0)
							{
						         //光耦无触发
								 TMC5160_WriteInt(eTMC, TMC5160_SWMODE, 0x0821); //为左参考点，软复位，高有效
								 g_tBoardStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_2;
								
							}else{
								//光耦触发已在零位,切换到“低速回退”								
								g_tBoardStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_4;
							}
						}else{
							//TMC_2
							if(ucOC_Status == 1)
							{
						         //光耦无触发
								TMC5160_WriteInt(eTMC, TMC5160_SWMODE, 0x0825); //模块三：为左参考点，软复位，低有效。 模块1、2：触发时为1，高有效。
								 g_tBoardStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_2;
								
							}else{
								//光耦触发已在零位，切换到“低速回退”
								g_tBoardStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_4;
							}
						}
#else
						if(ucOC_Status == 0)
						{
							 //光耦无触发
							 //TMC5160_WriteInt(eTMC, TMC5160_SWMODE, 0x0821); //为左参考点，软复位，高有效
							
							//使能做参考点复位
							TMC5160_FIELD_UPDATE(eTMC, TMC5160_SWMODE, TMC5160_STATUS_STOP_L_MASK, TMC5160_STATUS_STOP_L_SHIFT, 1);
							g_tTMCStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_2;
							
						}else{
							//光耦触发已在零位,切换到“低速回退”								
							g_tTMCStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_4;
						}		
						
#endif
					}
					break;
					/* 高速回零 */
					case MOTOR_RESET_EXEC_2:
					{
						TMC5160_WriteInt(eTMC, TMC5160_VMAX, 0); 
						TMC5160_WriteInt(eTMC, TMC5160_VMAX, g_tAxisParamDefault.lResetSpeedHigh[eTMC]*V_CHANGE_CONST);   // RESET_LOW_SPEED  RESET_HIGH_SPEED
						TMC5160_WriteInt(eTMC, TMC5160_AMAX ,g_tAxisParamDefault.lResetAcc[eTMC]); 		 //change AMax, so than  stop quickly
						TMC5160_WriteInt(eTMC, TMC5160_RAMPMODE, TMC5160_MODE_VELNEG); 					 //TMC5160_MODE_VELPOS   TMC5160_MODE_VELNEG
					
						//切换到3，速度是否为0
						g_tTMCStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_3;
					}
					break;
					/* 速度是否为0 */
					case MOTOR_RESET_EXEC_3:
					{					
						lSpeed = TMC5160_ReadInt(eTMC, TMC5160_VACTUAL);
						if(lSpeed == 0)
						{
							//切换到
							g_tTMCStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_4;
						}
					}
					break;				
					/* 低速旋转回退 */
					case MOTOR_RESET_EXEC_4:
					{
						TMC5160_WriteInt(eTMC, TMC5160_VMAX, g_tAxisParamDefault.lResetSpeedLow[eTMC]*V_CHANGE_CONST);  		// RESET_LOW_SPEED  RESET_HIGH_SPEED
						//TMC5160_WriteInt(eTMC, TMC5160_AMAX ,RESET_LOW_AMAX); //
						TMC5160_WriteInt(eTMC, TMC5160_RAMPMODE, TMC5160_MODE_VELPOS);  //TMC5160_MODE_VELPOS   TMC5160_MODE_VELNEG
											
						//切换到
						g_tTMCStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_5;
					
					}
					break;
					/* 检测等待脱离光耦 */
					case MOTOR_RESET_EXEC_5:
					{					
						//检查光耦状体
						ucOC_Status = TMC5160_FIELD_READ(eTMC, TMC5160_RAMPSTAT, TMC5160_RAMPSTAT_LEFT_SW_TRIGGER_MASK, TMC5160_RAMPSTAT_LEFT_SW_TRIGGER_SHIFT);
#ifdef P_AXIS_REF_POLITY					
						if(eTMC == TMC_0 || eTMC == TMC_1)
						{							
							if(ucOC_Status == 0)
							{
								//记录当前位置
								s_lTrigglePos = TMC5160_ReadInt(eTMC, TMC5160_XACTUAL);
						        //光耦无触发(已脱离光耦)， 
								g_tBoardStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_6;
							}
						}else{
							//TMC_2
							if(ucOC_Status == 1)
							{
								//记录当前位置
								s_lTrigglePos = TMC5160_ReadInt(eTMC, TMC5160_XACTUAL);
						        //光耦无触发（已脱离光耦）
								g_tBoardStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_6;
							}
						}
#else
						if(ucOC_Status == 0)
						{
							#if 0  //去掉固定位置回退，步骤！！！！
								//记录当前位置
								s_lTrigglePos = TMC5160_ReadInt(eTMC, TMC5160_XACTUAL);
								//光耦无触发(已脱离光耦)， 
								g_tBoardStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_6;
							#else
								TMC5160_WriteInt(eTMC, TMC5160_VMAX, 0);
								g_tTMCStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_7;
							#endif
						}
#endif
					}
					break;	
					/* 执行固定距离 */
					case MOTOR_RESET_EXEC_6:
					{
						//
						int32_t lCurPos = TMC5160_ReadInt(eTMC, TMC5160_XACTUAL);
						if(lCurPos >= s_lTrigglePos + g_tAxisParamDefault.lResetOff[eTMC])
						{
							//停止
							TMC5160_WriteInt(eTMC, TMC5160_VMAX, 0); 
							//切换到
							g_tTMCStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_7;
						}
					}
					break;
					/* 速度是否为0 */
					case MOTOR_RESET_EXEC_7:
					{
						lSpeed = TMC5160_ReadInt(eTMC, TMC5160_VACTUAL);
						if(lSpeed == 0)
						{
							//切换到
							g_tTMCStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_8;
						}
					}
					break;	
					/* 设置复位模式 */
					case MOTOR_RESET_EXEC_8:
					{	
#ifdef P_AXIS_REF_POLITY						
						if(eTMC == TMC_0 || eTMC == TMC_1)
						{				
							TMC5160_WriteInt(eTMC, TMC5160_SWMODE, 0x0821); //为左参考点，软复位，高有效							
						}else{
							//TMC_2
							TMC5160_WriteInt(eTMC, TMC5160_SWMODE, 0x0825); //模块三：光耦触发时0，低有效。 模块1、2：触发时为1，高有效。

						}
#else
						
//						uint32_t ulSWMode = 0;
//						ulSWMode = (0x0800 | (g_tAxisParamDefault.ucRighLimitPolarity[eTMC] << 3) | \
//									(g_tAxisParamDefault.ucLeftLimitPolarity[eTMC] << 2) | 1);
						
						//使能做参考点复位
						TMC5160_FIELD_UPDATE(eTMC, TMC5160_SWMODE, TMC5160_STATUS_STOP_L_MASK, TMC5160_STATUS_STOP_L_SHIFT, 1);					
						//TMC5160_WriteInt(eTMC, TMC5160_SWMODE, ulSWMode); //软复位，使能和设置左参考点停止, 左锁存，触发极性。 设置右参考点极性，但不使能	
						
#endif
						//切换到
						g_tTMCStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_9;
					}
					break;
					/* 低速复位 */
					case MOTOR_RESET_EXEC_9:
					{
						TMC5160_WriteInt(eTMC, TMC5160_VMAX, g_tAxisParamDefault.lResetSpeedLow[eTMC]*V_CHANGE_CONST);  		// RESET_LOW_SPEED  RESET_HIGH_SPEED
						TMC5160_WriteInt(eTMC, TMC5160_RAMPMODE, TMC5160_MODE_VELNEG);  //TMC5160_MODE_VELPOS   TMC5160_MODE_VELNEG
					
						//切换到
						g_tTMCStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_10;
					}
					break;		
					/* 速度是否为0 */					
					case MOTOR_RESET_EXEC_10:
					{					
						lSpeed = TMC5160_ReadInt(eTMC, TMC5160_VACTUAL);
						if(lSpeed == 0)
						{
							//切换到
							g_tTMCStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_11;
						}
					}
					break;
					/* 修正便宜 */
					case MOTOR_RESET_EXEC_11:
					{
						//实际位置
						int32_t lXActual = TMC5160_ReadInt(eTMC, TMC5160_XACTUAL);
						//get XLatch， 光耦触发位置
						int32_t lXLatch = TMC5160_ReadInt(eTMC, TMC5160_XLATCH);
						//偏差
						int32_t	lDiff = lXActual - lXLatch;
						
						//位置模式，修正便宜
						TMC5160_WriteInt(eTMC, TMC5160_XACTUAL, lDiff);
						TMC5160_WriteInt(eTMC, TMC5160_XTARGET, 0);
						TMC5160_WriteInt(eTMC, TMC5160_RAMPMODE, TMC_MODE_POSITION);
						TMC_SetPMode_V(eTMC, 1);
					
						//切换到
						g_tTMCStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_12;
					}
					break;		
					/* 修正完成，速度=0 */
					case MOTOR_RESET_EXEC_12:
					{
					
						lSpeed = TMC5160_ReadInt(eTMC, TMC5160_VACTUAL);
						if(lSpeed == 0)
						{
							//切换到
							g_tTMCStatus.tMotorResetInfo[eTMC].eResetExec = MOTOR_RESET_EXEC_13;
						}
					}
					/* 复位完成，处理 */
					case MOTOR_RESET_EXEC_13:
					{
						//关闭做参考点复位
						TMC5160_FIELD_UPDATE(eTMC, TMC5160_SWMODE, TMC5160_STATUS_STOP_L_MASK, TMC5160_STATUS_STOP_L_SHIFT, 0);
						
						//设置为保持模式
						//TMC5160_WriteInt(eTMC, TMC5160_RAMPMODE, TMC_MODE_HOLD);
						
						//清零
						TMC5160_WriteInt(eTMC, TMC5160_XTARGET, 0);
						TMC5160_WriteInt(eTMC, TMC5160_XACTUAL, 0);						
						TMC5160_WriteInt(eTMC, TMC5160_XENC, 0);
						//恢复速度设置
						TMC_SetPMode_V(eTMC, 1);
						g_tTMCStatus.tMotorResetInfo[eTMC].eResetStatus = MOTOR_RESET_STATUS_FINISH;
						
						//设置模式--位置模式
						TMC5160_WriteInt(eTMC, TMC5160_RAMPMODE, TMC_MODE_POSITION); //
						//再次将编码器清零
						TMC5160_WriteInt(eTMC, TMC5160_XENC, 0);
					}
					break;						
				}		
				
				//复位超时检查
				if(ulTick - g_tTMCStatus.tMotorResetInfo[eTMC].ulStartTick >= MOTOR_RESET_TIMEOUT) 
				{
					//超时
					g_tTMCStatus.tMotorResetInfo[eTMC].eResetStatus = MOTOR_RESET_STATUS_FAIL;
					LOG_Error("Motor=%d Reset Timeout", eTMC);
					return 0;
				}
			}
		}
		
		//所有复位已执行完成
		if(ucFlag == 0)
		{
			g_tTMCStatus.ucMotorResetStartFlag = 0;
		}
	}
	return 0;;
}





/*
*	更新编码器常数, ucValidFlag:1=写入控制器寄存器，即：立即生效， 0=不写入，不生效
*/
ErrorType_e Update_Enc_ConstValue(TMC_e eTMC, __IO AxisParamDefault_t *ptAxisParamDefault, uint8_t ucValidFlag)
{	
	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
	
	//编码器因子
	float fMid = (float)((ptAxisParamDefault->usFullStepPerRound[eTMC]) * ptAxisParamDefault->usMicroStepResultion[eTMC])/(float)(ptAxisParamDefault->usEncResultion[eTMC] * (int32_t)4); 
	//编码器因子--整数部分
	int32_t lInt = (int32_t)fMid;
	//编码器因子--小数部分
	float fDecimal = fMid - lInt;

	//
	//LOG_Info("Update Enc Const: eTMC=%d. FullStepPerRound=%d, MicroStepResultion=%d, EncResultion=%d, EncCountDirect=%d", \
			 eTMC, ptAxisParamDefault->usFullStepPerRound[eTMC], ptAxisParamDefault->usMicroStepResultion[eTMC], \
			 ptAxisParamDefault->usEncResultion[eTMC], ptAxisParamDefault->ucEncCountDirect[eTMC]);
	
	//计算编码器常数
	if(0 == g_tAxisParamDefault.ucEncCountDirect[eTMC])
	{
		//正向
		ptAxisParamDefault->lEncConstValue[eTMC] = lInt*65536+ (int32_t)(fDecimal*10000);
		
	}else if(1 == g_tAxisParamDefault.ucEncCountDirect[eTMC]){
		//反向		
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
*	更新并保存编码器常数
*/
ErrorType_e UpdateAndSave_Enc_ConstValue(TMC_e eTMC, __IO AxisParamDefault_t *ptAxisParamDefault)
{
	//更新编码器常数
	Update_Enc_ConstValue(eTMC, ptAxisParamDefault, 0);
	
	//重新计算CRC
	ptAxisParamDefault->usCrc = CRC16((uint8_t*)ptAxisParamDefault, sizeof(AxisParamDefault_t)-2);
	
	//保存轴参数
	return Save_Axis_Param_Default(ptAxisParamDefault);
	
}






/*
*	轴参数初始化
*/
void Axis_Param_Fixed_SetDefault_Value(__IO AxisParamDefault_t *ptAxisParamDefault)
{
	TMC_e eTMC = TMC_0;
	
	//第一次上电，初始化，保存
	ptAxisParamDefault->ulInitFlag = PARAM_INIT_FLAG;
	
#if (CURRENT_MODULE_TYPE==MODULE_TYPE_TMC_STEP_MOTOR_1301_Common) || (CURRENT_MODULE_TYPE==MODULE_TYPE_TMC_STEP_MOTOR_1161) 
	/* 单轴 */		
	
	//编码器相关
	ptAxisParamDefault->usEncResultion[eTMC]	  = DEFAULT_ENC_RESULTION_1000;   	//编码器分辨率
	ptAxisParamDefault->lEncDiff_Threshold[eTMC]  = 100;					    //编码器检测失步，阈值
	ptAxisParamDefault->ucEncCountDirect[eTMC]    = 0;					   		//编码器计数方向
	ptAxisParamDefault->lEncConstValue[eTMC] 	  = DEFAULT_ENC_CONST_VALUE_TMC_0;  //编码器常数--计算获取


	//参考点-复位参数
	ptAxisParamDefault->lResetSpeedHigh[eTMC] 	= 3500;  //复位速度
	ptAxisParamDefault->lResetSpeedLow[eTMC]  	= 1500;	 //复位速度  
	ptAxisParamDefault->lResetAcc[eTMC]      	= 500;   //复位加速度
	ptAxisParamDefault->lResetOff[eTMC]  		= 200;	 //复位偏移距离

	//电流
	ptAxisParamDefault->ucIRun[eTMC]	   = 65; //运行电流
	ptAxisParamDefault->ucIHold[eTMC]	   = 8;  //保持电流
	ptAxisParamDefault->ucIHoldDelay[eTMC] = 6;   //电流降到IHold所需时间	

	//步进细分、没转全步数
	ptAxisParamDefault->usMicroStepResultion[eTMC] = 64;//DEFAULT_MICRO_STEP_RESULTION; 
	ptAxisParamDefault->usFullStepPerRound[eTMC]   = DEFAULT_FULL_STEP_PER_ROUND; //全步每转


	//六点速度参数，用于位置模式
	ptAxisParamDefault->lVStart[eTMC] = 0;
	ptAxisParamDefault->lA1[eTMC] 	  = 500000; 
	ptAxisParamDefault->lV1[eTMC] 	  = 40000;
	ptAxisParamDefault->lAMax[eTMC]   = 300000; 
	ptAxisParamDefault->lVMax[eTMC]   = 65000;
	ptAxisParamDefault->lDMax[eTMC]   = 300000; 
	ptAxisParamDefault->lD1[eTMC] 	  = 500000; 
	ptAxisParamDefault->lVStop[eTMC]  = 10;
	//驱动器运行模式，用于位置模式
	ptAxisParamDefault->ucMode[eTMC] = TMC_MODE_POSITION;
	//速度参数、用于速度模式
	ptAxisParamDefault->lAMax_VMode[eTMC] = ptAxisParamDefault->lAMax[eTMC];	//第二段加速度
	ptAxisParamDefault->lVMax_VMode[eTMC] = ptAxisParamDefault->lVMax[eTMC];	//最大速度

	//参考点复位
	ptAxisParamDefault->ucRighLimitPolarity[eTMC] = 0; //右限位极性，高有效
	ptAxisParamDefault->ucLeftLimitPolarity[eTMC] = 0; //左限位极性，高有效
	ptAxisParamDefault->ucRotateDirect[eTMC]	  = 0; //旋转方向		
			
#elif (CURRENT_MODULE_TYPE==MODULE_TYPE_TMC_STEP_MOTOR_3301_Pipette)
	/* 三轴 移液器 */	
	for(eTMC = TMC_0; eTMC < TMC_MODULE_END; eTMC++)
	{			
		if(TMC_0 == eTMC)
		{
			/* 三轴 M0, (Z轴)*/
			
			//编码器相关
			ptAxisParamDefault->usEncResultion[eTMC]	 = DEFAULT_ENC_RESULTION_720;     //编码器分辨率
			ptAxisParamDefault->lEncDiff_Threshold[eTMC] = 100;					   	  //编码器检测失步，阈值
			ptAxisParamDefault->ucEncCountDirect[eTMC]   = 0;					   	  //编码器计数方向
			ptAxisParamDefault->lEncConstValue[eTMC] 	 = DEFAULT_ENC_CONST_VALUE_Z; //编码器常数--计算获取			
			
			//参考点-复位参数
			ptAxisParamDefault->lResetSpeedHigh[eTMC] = 15000; //复位速度
			ptAxisParamDefault->lResetSpeedLow[eTMC]  = 1000;  //复位速度  
			ptAxisParamDefault->lResetAcc[eTMC]       = 500;   //复位加速度
			ptAxisParamDefault->lResetOff[eTMC]  	  = 800;   //复位偏移距离

			//电流
			ptAxisParamDefault->ucIRun[eTMC]	   = 128; //运行电流
			ptAxisParamDefault->ucIHold[eTMC]	   = 8; //保持电流
			ptAxisParamDefault->ucIHoldDelay[eTMC] = 6;  //电流降到IHold所需时间	
			
			//步进细分、没转全步数
			ptAxisParamDefault->usMicroStepResultion[eTMC] = 64;//DEFAULT_MICRO_STEP_RESULTION; 
			ptAxisParamDefault->usFullStepPerRound[eTMC]   = DEFAULT_FULL_STEP_PER_ROUND; //全步每转
			
			//六点速度参数，用于位置模式
			ptAxisParamDefault->lVStart[eTMC] = 0;
			ptAxisParamDefault->lA1[eTMC] 	 = 900000; 
			ptAxisParamDefault->lV1[eTMC] 	 = 150000;
			ptAxisParamDefault->lAMax[eTMC]  = 1200000; 
			ptAxisParamDefault->lVMax[eTMC]  = 243200;
			ptAxisParamDefault->lDMax[eTMC]  = 1200000; 
			ptAxisParamDefault->lD1[eTMC] 	 = 900000; 
			ptAxisParamDefault->lVStop[eTMC] = 10;
			//驱动器运行模式
			ptAxisParamDefault->ucMode[eTMC] = TMC_MODE_POSITION;
			//速度参数、用于速度模式
			ptAxisParamDefault->lAMax_VMode[eTMC] = ptAxisParamDefault->lAMax[eTMC];	//第二段加速度
			ptAxisParamDefault->lVMax_VMode[eTMC] = ptAxisParamDefault->lVMax[eTMC];	//最大速度
	
			//参考点复位
			ptAxisParamDefault->ucRighLimitPolarity[eTMC] = 0; //右限位极性，高有效
			ptAxisParamDefault->ucLeftLimitPolarity[eTMC] = 0; //左限位极性，高有效
			ptAxisParamDefault->ucRotateDirect[eTMC]	  = 0; //旋转方向
			
		}else if(TMC_1 == eTMC){
			/* 三轴 M1, (Y轴)*/		
			//编码器相关
			ptAxisParamDefault->usEncResultion[eTMC]	 = DEFAULT_ENC_RESULTION_1000;   //编码器分辨率
			ptAxisParamDefault->lEncDiff_Threshold[eTMC] = 100;					   //编码器检测失步，阈值
			ptAxisParamDefault->ucEncCountDirect[eTMC]   = 0;					   //编码器计数方向
			ptAxisParamDefault->lEncConstValue[eTMC] 	 = DEFAULT_ENC_CONST_VALUE_Y; //编码器常数--计算获取
		
			//参考点-复位参数
			ptAxisParamDefault->lResetSpeedHigh[eTMC] = 5000; //复位速度
			ptAxisParamDefault->lResetSpeedLow[eTMC]  = 1000; //复位速度 
			ptAxisParamDefault->lResetAcc[eTMC]       = 500;  //复位加速度
			ptAxisParamDefault->lResetOff[eTMC]  	  = 400;  //复位偏移距离
			ptAxisParamDefault->lEncDiff_Threshold[eTMC] = 100;	//编码器检测失步，阈值
			
			//电流
			ptAxisParamDefault->ucIRun[eTMC]	   = 128; //运行电流
			ptAxisParamDefault->ucIHold[eTMC]	   = 8;  //保持电流
			ptAxisParamDefault->ucIHoldDelay[eTMC] = 6;   //电流降到IHold所需时间
			
			//步进细分、没转全步数
			ptAxisParamDefault->usMicroStepResultion[eTMC] = 64; //默认细分
			ptAxisParamDefault->usFullStepPerRound[eTMC]   = DEFAULT_FULL_STEP_PER_ROUND; //全步每转			
			
			//六点速度参数，用于位置模式
			ptAxisParamDefault->lVStart[eTMC] = 0;
			ptAxisParamDefault->lA1[eTMC] 	 = 900000; 
			ptAxisParamDefault->lV1[eTMC] 	 = 80000;
			ptAxisParamDefault->lAMax[eTMC]  = 600000;
			ptAxisParamDefault->lVMax[eTMC]  = 128000;
			ptAxisParamDefault->lDMax[eTMC]  = 600000;
			ptAxisParamDefault->lD1[eTMC] 	 = 800000;
			ptAxisParamDefault->lVStop[eTMC] = 10;
			//驱动器运行模式
			ptAxisParamDefault->ucMode[eTMC] = TMC_MODE_POSITION;
			//速度参数、用于速度模式
			ptAxisParamDefault->lAMax_VMode[eTMC] = ptAxisParamDefault->lAMax[eTMC];	//第二段加速度
			ptAxisParamDefault->lVMax_VMode[eTMC] = ptAxisParamDefault->lVMax[eTMC];	//最大速度
			
			//参考点复位
			ptAxisParamDefault->ucRighLimitPolarity[eTMC] = 0; //右限位极性，高有效
			ptAxisParamDefault->ucLeftLimitPolarity[eTMC] = 0; //左限位极性，高有效
			ptAxisParamDefault->ucRotateDirect[eTMC]	  = 0; //旋转方向
				
		}else if(TMC_2 == eTMC){	
			/* 三轴 M1, (P轴) */
			//编码器相关
			ptAxisParamDefault->usEncResultion[eTMC]	 = DEFAULT_ENC_RESULTION_1000;     //编码器分辨率
			ptAxisParamDefault->lEncDiff_Threshold[eTMC] = 100;					      //编码器检测失步，阈值
			ptAxisParamDefault->ucEncCountDirect[eTMC]   = 0;					      //编码器计数方向
			ptAxisParamDefault->lEncConstValue[eTMC] 	 = DEFAULT_ENC_CONST_VALUE_P; //编码器常数--计算获取
		
			//参考点-复位参数
			ptAxisParamDefault->lResetSpeedHigh[eTMC] 	 = 3500; //复位速度
			ptAxisParamDefault->lResetSpeedLow[eTMC]  	 = 1500; //复位速度  
			ptAxisParamDefault->lResetAcc[eTMC]      	 = 800;	 //复位加速度
			ptAxisParamDefault->lResetOff[eTMC]  		 = 3200; //复位偏移距离			
			ptAxisParamDefault->lEncDiff_Threshold[eTMC] = 0;	 //编码器检测失步，阈值	
			
			//电流
			ptAxisParamDefault->ucIRun[eTMC]	   = 128; //运行电流
			ptAxisParamDefault->ucIHold[eTMC]	   = 8; //保持电流
			ptAxisParamDefault->ucIHoldDelay[eTMC] = 6;  //电流降到IHold所需时间
						
			//六点速度参数,用于位置模式
			ptAxisParamDefault->lVStart[eTMC] = 0;
			ptAxisParamDefault->lA1[eTMC] 	  = 500000; 
			ptAxisParamDefault->lV1[eTMC] 	  = 40000;
			ptAxisParamDefault->lAMax[eTMC]   = 300000;
			ptAxisParamDefault->lVMax[eTMC]   = 65000;
			ptAxisParamDefault->lDMax[eTMC]   = 300000;
			ptAxisParamDefault->lD1[eTMC] 	  = 500000;
			ptAxisParamDefault->lVStop[eTMC]  = 10;
			//驱动器运行模式
			ptAxisParamDefault->ucMode[eTMC] = TMC_MODE_POSITION;
			//速度参数、用于速度模式
			ptAxisParamDefault->lAMax_VMode[eTMC] = ptAxisParamDefault->lAMax[eTMC];	//第二段加速度
			ptAxisParamDefault->lVMax_VMode[eTMC] = ptAxisParamDefault->lVMax[eTMC];	//最大速度
			
			//步进细分、没转全步数
			ptAxisParamDefault->usMicroStepResultion[eTMC] = 64; //默认细分
			ptAxisParamDefault->usFullStepPerRound[eTMC]   = DEFAULT_FULL_STEP_PER_ROUND;  //全步每转
			
			//参考点复位
			ptAxisParamDefault->ucRighLimitPolarity[eTMC] = 0; //右限位极性，高有效
			ptAxisParamDefault->ucLeftLimitPolarity[eTMC] = 0; //左限位极性，高有效
			ptAxisParamDefault->ucRotateDirect[eTMC]	  = 0; //旋转方向
		}
	}
	
#elif (CURRENT_MODULE_TYPE==MODULE_TYPE_TMC_STEP_MOTOR_3311_Common) 	
	/* 三轴 通用版 */	
	for(eTMC = TMC_0; eTMC < TMC_MODULE_END; eTMC++)
	{			
		if(TMC_0 == eTMC)
		{
			/* 三轴 M0 */
			
			//编码器相关
			ptAxisParamDefault->usEncResultion[eTMC]	 = DEFAULT_ENC_RESULTION_1000;     //编码器分辨率
			ptAxisParamDefault->lEncDiff_Threshold[eTMC] = 100;					   	  //编码器检测失步，阈值
			ptAxisParamDefault->ucEncCountDirect[eTMC]   = 0;					   	  //编码器计数方向
			ptAxisParamDefault->lEncConstValue[eTMC] 	 = DEFAULT_ENC_CONST_VALUE_TMC_0; //编码器常数--计算获取			
			
			//参考点-复位参数
			ptAxisParamDefault->lResetSpeedHigh[eTMC] = 3500; //复位速度
			ptAxisParamDefault->lResetSpeedLow[eTMC]  = 100;  //复位速度  
			ptAxisParamDefault->lResetAcc[eTMC]       = 500;   //复位加速度
			ptAxisParamDefault->lResetOff[eTMC]  	  = 200;   //复位偏移距离

			//电流
			ptAxisParamDefault->ucIRun[eTMC]	   = 128; //运行电流
			ptAxisParamDefault->ucIHold[eTMC]	   = 8; //保持电流
			ptAxisParamDefault->ucIHoldDelay[eTMC] = 6;  //电流降到IHold所需时间	
			
			//步进细分、没转全步数
			ptAxisParamDefault->usMicroStepResultion[eTMC] = 64;//DEFAULT_MICRO_STEP_RESULTION; 
			ptAxisParamDefault->usFullStepPerRound[eTMC]   = DEFAULT_FULL_STEP_PER_ROUND; //全步每转
			
			
			//六点速度参数，用于位置模式
			ptAxisParamDefault->lVStart[eTMC] = 0;
			ptAxisParamDefault->lA1[eTMC] 	 = 200000; 
			ptAxisParamDefault->lV1[eTMC] 	 = 35000;
			ptAxisParamDefault->lAMax[eTMC]  = 95000; 
			ptAxisParamDefault->lVMax[eTMC]  = 61000;
			ptAxisParamDefault->lDMax[eTMC]  = 95000; 
			ptAxisParamDefault->lD1[eTMC] 	 = 200000; 
			ptAxisParamDefault->lVStop[eTMC] = 10;
			//驱动器运行模式
			ptAxisParamDefault->ucMode[eTMC] = TMC_MODE_POSITION;
			//速度参数、用于速度模式
			ptAxisParamDefault->lAMax_VMode[eTMC] = ptAxisParamDefault->lAMax[eTMC];	//第二段加速度
			ptAxisParamDefault->lVMax_VMode[eTMC] = ptAxisParamDefault->lVMax[eTMC];	//最大速度
			
			//参考点复位
			ptAxisParamDefault->ucRighLimitPolarity[eTMC] = 0; //右限位极性，高有效
			ptAxisParamDefault->ucLeftLimitPolarity[eTMC] = 0; //左限位极性，高有效
			ptAxisParamDefault->ucRotateDirect[eTMC]	  = 0; //旋转方向
			
		}else if(TMC_1 == eTMC){
			/* 三轴 M1, (Y轴)*/		
			//编码器相关
			ptAxisParamDefault->usEncResultion[eTMC]	 = DEFAULT_ENC_RESULTION_1000;   //编码器分辨率
			ptAxisParamDefault->lEncDiff_Threshold[eTMC] = 100;					   //编码器检测失步，阈值
			ptAxisParamDefault->ucEncCountDirect[eTMC]   = 0;					   //编码器计数方向
			ptAxisParamDefault->lEncConstValue[eTMC] 	 = DEFAULT_ENC_CONST_VALUE_TMC_1; //编码器常数--计算获取
		
			//参考点-复位参数
			ptAxisParamDefault->lResetSpeedHigh[eTMC] = 35000; //复位速度
			ptAxisParamDefault->lResetSpeedLow[eTMC]  = 1000; //复位速度 
			ptAxisParamDefault->lResetAcc[eTMC]       = 500;  //复位加速度
			ptAxisParamDefault->lResetOff[eTMC]  	  = 400;  //复位偏移距离
			ptAxisParamDefault->lEncDiff_Threshold[eTMC] = 100;	//编码器检测失步，阈值
			
			//电流
			ptAxisParamDefault->ucIRun[eTMC]	   = 128; //运行电流
			ptAxisParamDefault->ucIHold[eTMC]	   = 8;  //保持电流
			ptAxisParamDefault->ucIHoldDelay[eTMC] = 6;   //电流降到IHold所需时间
			
			//步进细分、没转全步数
			ptAxisParamDefault->usMicroStepResultion[eTMC] = 64; //默认细分
			ptAxisParamDefault->usFullStepPerRound[eTMC]   = DEFAULT_FULL_STEP_PER_ROUND; //全步每转			
			
			//六点速度参数，用于位置模式
			ptAxisParamDefault->lVStart[eTMC] = 0;
			ptAxisParamDefault->lA1[eTMC] 	 = 900000; 
			ptAxisParamDefault->lV1[eTMC] 	 = 80000;
			ptAxisParamDefault->lAMax[eTMC]  = 600000;
			ptAxisParamDefault->lVMax[eTMC]  = 128000;
			ptAxisParamDefault->lDMax[eTMC]  = 600000;
			ptAxisParamDefault->lD1[eTMC] 	 = 800000;
			ptAxisParamDefault->lVStop[eTMC] = 10;
			//驱动器运行模式
			ptAxisParamDefault->ucMode[eTMC] = TMC_MODE_POSITION;
			//速度参数、用于速度模式
			ptAxisParamDefault->lAMax_VMode[eTMC] = ptAxisParamDefault->lAMax[eTMC];	//第二段加速度
			ptAxisParamDefault->lVMax_VMode[eTMC] = ptAxisParamDefault->lVMax[eTMC];	//最大速度
			
			//参考点复位
			ptAxisParamDefault->ucRighLimitPolarity[eTMC] = 0; //右限位极性，高有效
			ptAxisParamDefault->ucLeftLimitPolarity[eTMC] = 0; //左限位极性，高有效
			ptAxisParamDefault->ucRotateDirect[eTMC]	  = 0; //旋转方向
				
		}else if(TMC_2 == eTMC){	
			/* 三轴 M1, (P轴) */
			//编码器相关
			ptAxisParamDefault->usEncResultion[eTMC]	 = DEFAULT_ENC_RESULTION_1000;     //编码器分辨率
			ptAxisParamDefault->lEncDiff_Threshold[eTMC] = 100;					      //编码器检测失步，阈值
			ptAxisParamDefault->ucEncCountDirect[eTMC]   = 0;					      //编码器计数方向
			ptAxisParamDefault->lEncConstValue[eTMC] 	 = DEFAULT_ENC_CONST_VALUE_TMC_2; //编码器常数--计算获取
		
			//参考点-复位参数
			ptAxisParamDefault->lResetSpeedHigh[eTMC] 	 = 3500; //复位速度
			ptAxisParamDefault->lResetSpeedLow[eTMC]  	 = 1500; //复位速度  
			ptAxisParamDefault->lResetAcc[eTMC]      	 = 800;	 //复位加速度
			ptAxisParamDefault->lResetOff[eTMC]  		 = 3200; //复位偏移距离			
			ptAxisParamDefault->lEncDiff_Threshold[eTMC] = 0;	 //编码器检测失步，阈值	
			
			//电流
			ptAxisParamDefault->ucIRun[eTMC]	   = 128; //运行电流
			ptAxisParamDefault->ucIHold[eTMC]	   = 8; //保持电流
			ptAxisParamDefault->ucIHoldDelay[eTMC] = 6;  //电流降到IHold所需时间
						
			//六点速度参数，用于位置模式
			ptAxisParamDefault->lVStart[eTMC] = 0;
			ptAxisParamDefault->lA1[eTMC] 	  = 500000; 
			ptAxisParamDefault->lV1[eTMC] 	  = 40000;
			ptAxisParamDefault->lAMax[eTMC]   = 300000;
			ptAxisParamDefault->lVMax[eTMC]   = 65000;
			ptAxisParamDefault->lDMax[eTMC]   = 300000;
			ptAxisParamDefault->lD1[eTMC] 	  = 500000;
			ptAxisParamDefault->lVStop[eTMC]  = 10;
			//驱动器运行模式
			ptAxisParamDefault->ucMode[eTMC] = TMC_MODE_POSITION;
			//速度参数、用于速度模式
			ptAxisParamDefault->lAMax_VMode[eTMC] = ptAxisParamDefault->lAMax[eTMC];	//第二段加速度
			ptAxisParamDefault->lVMax_VMode[eTMC] = ptAxisParamDefault->lVMax[eTMC];	//最大速度
			
			//步进细分、没转全步数
			ptAxisParamDefault->usMicroStepResultion[eTMC] = 64; //默认细分
			ptAxisParamDefault->usFullStepPerRound[eTMC]   = DEFAULT_FULL_STEP_PER_ROUND;  //全步每转
			
			//参考点复位
			ptAxisParamDefault->ucRighLimitPolarity[eTMC] = 0; //右限位极性，高有效
			ptAxisParamDefault->ucLeftLimitPolarity[eTMC] = 0; //左限位极性，高有效
			ptAxisParamDefault->ucRotateDirect[eTMC]	  = 0; //旋转方向
		}
	}

#endif
	
	ptAxisParamDefault->usCrc = CRC16((uint8_t*)ptAxisParamDefault, sizeof(AxisParamDefault_t)-2);
}





/*
*	轴参数初始化
*/
ErrorType_e Axis_Param_Default_Init(void)
{
	return Read_Axis_Param_Default(&g_tAxisParamDefault);
}




//寄存器初始化
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
//*	轴参数
//*/
//ErrorType_e TMC_AxisParam(TMC_e eTMC, ReadWrite_e eReadWrite, uint8_t ucType, Data4Byte_u *puData)
//{
//	ErrorType_e eError = ERROR_TYPE_SUCCESS;
//	
//	//参数检测
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
*	抽参数设置
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

	
	//参数检测
	if(eTMC >= TMC_END)
	{
		LOG_Error("TMC DeviceID=%d Is Error", eTMC);
		return ERROR_TYPE_DEVICE_ID;
	}
	

	switch(ucType)
	{
	/*** 位置相关  ***/
	case 0x00:
		// 目标位置，Target position
		if(eReadWrite == TMC_READ) {
			puData->lData = TMC5160_ReadInt(eTMC, TMC5160_XTARGET);
		} else if(eReadWrite == TMC_WRITE) {
			eError = TMC5160_WriteInt(eTMC, TMC5160_XTARGET, puData->lData);
		}
		break;
	case 0x01:
		// 实际位置，Actual position
		if(eReadWrite == TMC_READ) {
			puData->lData = TMC5160_ReadInt(eTMC, TMC5160_XACTUAL);
		} else if(eReadWrite == TMC_WRITE) {
			eError = TMC5160_WriteInt(eTMC, TMC5160_XACTUAL, puData->lData);
		}
		break;
		
	case 0x02:
		// 到达最大位置标志，RO
		if(eReadWrite == TMC_READ) {
			puData->lData = TMC5160_FIELD_READ(eTMC, TMC5160_RAMPSTAT, TMC5160_RAMPSTAT_POS_REACH_MASK, TMC5160_RAMPSTAT_POS_REACH_SHIFT);
			//LOG_Info("RAMP_STAT=%X", TMC_ReadInt(eTMC, TMC5160_RAMPSTAT));
		} else if(eReadWrite == TMC_WRITE){
			LOG_Error("Read Max Postion Do not Be Write");
			return ERROR_TYPE_RW_RIGHT;
		}
		break;
			
		
	/*** 编码器 ***/
	case 0x03:
		//编码器值， Encoder position
		if(eReadWrite == TMC_READ) {
			puData->lData = TMC5160_ReadInt(eTMC, TMC5160_XENC);
		} else if(eReadWrite == TMC_WRITE) {
			eError = TMC5160_WriteInt(eTMC, TMC5160_XENC, puData->lData);
		}
		break;
	case 0x04:
		// 编码器分辨率 Encoder Resolution
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.usEncResultion[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			if(g_tAxisParamDefault.usEncResultion[eTMC] != puData->lData)
			{
				//值不同，需更新
				g_tAxisParamDefault.usEncResultion[eTMC] = puData->lData;
				//保存更新的参数
				Update_Enc_ConstValue(eTMC, &g_tAxisParamDefault, 1);
			}
		}
		break;	
		
	case 0x05:		
		// 编码器反向
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.ucEncCountDirect[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			if(puData->lData == 0 || puData->lData == 1)
			{
				g_tAxisParamDefault.ucEncCountDirect[eTMC] = puData->lData;
				//更新的参数
				Update_Enc_ConstValue(eTMC, &g_tAxisParamDefault, 1);
			}else{
				
				LOG_Error("Param=%d Is Error", puData->lData);
				return ERROR_TYPE_DATA;
			}
		}
		break;	
	case 0x06:
		// 丢步阈值 
		if(eReadWrite == TMC_READ) {
			
			puData->lData = g_tAxisParamDefault.lEncDiff_Threshold[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			eError = TMC5160_WriteInt(eTMC, TMC5160_ENC_DEVIATION, puData->lData);
			g_tAxisParamDefault.lEncDiff_Threshold[eTMC] = puData->lData;
		}
		break;		
		
	case 0x07:
		// 丢步值（大小） 
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
		//丢步标志位
		if(eReadWrite == TMC_READ) {
			puData->lData = TMC5160_FIELD_READ(eTMC, TMC5160_ENC_STATUS, TMC5160_DEVIATION_WARN_MASK, TMC5160_DEVIATION_WARN_SHIFT);
		}else if(eReadWrite == TMC_WRITE) {
			
			//需处理丢步，是XENC和XVACTUAL小于失步阈值，再写1，清除该标志位。或关闭丢步检测功能，再执行运动，再清除该位。			
			
			//清除丢步标志位
			//TMC5160_FIELD_UPDATE(eTMC, TMC5160_ENC_STATUS, TMC5160_DEVIATION_WARN_MASK, TMC5160_DEVIATION_WARN_SHIFT, puData->lData);
			LOG_Error("Enc Diff Do not Be Write");
			return ERROR_TYPE_RW_RIGHT;
		}
		break;
		
		
		
	/*** 复位相关 ***/
	case 0x10:
		//复位速度  低
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.lResetSpeedLow[eTMC];
		}else if(eReadWrite == TMC_WRITE) {
			g_tAxisParamDefault.lResetSpeedLow[eTMC] = puData->lData;
		}
		break;	
	case 0x11:
		//复位速度  高
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.lResetSpeedHigh[eTMC];
		}else if(eReadWrite == TMC_WRITE) {
			g_tAxisParamDefault.lResetSpeedHigh[eTMC] = puData->lData;
		}
		break;		
	case 0x12:
		//复位偏移距离
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.lResetOff[eTMC];
		}else if(eReadWrite == TMC_WRITE) {
			g_tAxisParamDefault.lResetOff[eTMC] = puData->lData;
		}
		break;		
	case 0x13:
		//复位加速度
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.lResetAcc[eTMC];
		}else if(eReadWrite == TMC_WRITE) {
			g_tAxisParamDefault.lResetAcc[eTMC] = puData->lData;
		}
		break;				
	case 0x14:
		//复位状态（标志位）
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tTMCStatus.tMotorResetInfo[eTMC].eResetStatus;
		}else if(eReadWrite == TMC_WRITE) {
			LOG_Error("Read Only, Don't Be Writed");
			return ERROR_TYPE_RW_RIGHT;
		}
		break;			

		
		
	/*** 电流相关 ****/	
	case 0x20:
		//运行电流
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
		//保持电流
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
		//电流切换时间
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.ucIHoldDelay[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			g_tAxisParamDefault.ucIHoldDelay[eTMC] = puData->lData;
			eError = TMC5160_FIELD_UPDATE(eTMC, TMC5160_IHOLD_IRUN, TMC5160_IHOLDDELAY_MASK, TMC5160_IHOLDDELAY_SHIFT, g_tAxisParamDefault.ucIHoldDelay[eTMC]);
		}
		break;		
//	case 0x22:
//		//电流微调 Global Current Scaler
//		if(eReadWrite == TMC_READ) {
//			puData->lData = g_taTMC5160[eTMC].laShadowRegister[TMC5160_GLOBAL_SCALER];
//		}else if(eReadWrite == TMC_WRITE) {
//			eError = TMC5160_WriteInt(eTMC, TMC5160_GLOBAL_SCALER, puData->lData);
//		}
//		break;		
	case 0x23:
		// 步进细分 Microstep Resolution
		if(eReadWrite == TMC_READ) {
			uint8_t ucMicroStep_Rg = 0;
			ucMicroStep_Rg = TMC5160_FIELD_READ(eTMC, TMC5160_CHOPCONF, TMC5160_MRES_MASK, TMC5160_MRES_SHIFT);
			puData->lData  = MicroStep_Register2SetValue((TMC_MicroStep_Resolution_e)ucMicroStep_Rg);
		} else if(eReadWrite == TMC_WRITE) {
			uint16_t usMicroStep = puData->lData;
			uint8_t ucMicroStep_Rg = 0;
			
			//设置更新
//			if(usMicroStep != g_tAxisParamDefault.usMicroStepResultion[eTMC])
//			{
				//将分辨率设置值  转换成 寄存器分辨率值
				eError = MicroStep_SetValue2Register(usMicroStep, &ucMicroStep_Rg);
				if(eError != ERROR_TYPE_SUCCESS)
				{
					LOG_Error("Data Micro Step=%d Error", usMicroStep);
					return ERROR_TYPE_DATA;
				}
			
				//写入寄存器
				eError = TMC5160_FIELD_UPDATE(eTMC, TMC5160_CHOPCONF, TMC5160_MRES_MASK, TMC5160_MRES_SHIFT, ucMicroStep_Rg);
				g_tAxisParamDefault.usMicroStepResultion[eTMC] = usMicroStep;
				//保存更新的参数
				Update_Enc_ConstValue(eTMC, &g_tAxisParamDefault, 1);
				//LOG_Info("Micro Step Resolution=%d", usMicroStep);
			//}		
		}
		break;		
	case 0x24:
	{
		//电机没转全步数
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.usFullStepPerRound[eTMC];
		}else if(eReadWrite == TMC_WRITE) {
//			if(puData->lData != g_tAxisParamDefault.usFullStepPerRound[eTMC])
//			{
				g_tAxisParamDefault.usFullStepPerRound[eTMC] = puData->lData;
				//保存更新的参数
				Update_Enc_ConstValue(eTMC, &g_tAxisParamDefault,1);
//				LOG_Info("Full Step Per Round=%d", puData->lData);
//			}
		}
	}
	break;
		
		
	/*** 六点加速相关 ***/		
	case 0x30:
		// 开始速度 Velocity VSTART
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.lVStart[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			eError = TMC5160_WriteInt(eTMC, TMC5160_VSTART, (puData->lData)*V_CHANGE_CONST);
			g_tAxisParamDefault.lVStart[eTMC] = puData->lData;
		}
		break;
	case 0x31:
		// 第一阶段加速度A1  Acceleration A1
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.lA1[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			eError = TMC5160_WriteInt(eTMC, TMC5160_A1, (puData->lData)*A_CHANGE_CONST);
			g_tAxisParamDefault.lA1[eTMC] = puData->lData;
		}
		break;
	case 0x32:
		// 换挡速度 Velocity V1
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.lV1[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			eError = TMC5160_WriteInt(eTMC, TMC5160_V1, (puData->lData)*V_CHANGE_CONST);
			g_tAxisParamDefault.lV1[eTMC] = puData->lData;
		}
		break;		
	case 0x33:
		// 第二阶段加速度 AMAX  Maximum acceleration，  用于位置模式
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.lAMax[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			if(g_tAxisParamDefault.ucMode[eTMC] == TMC_MODE_POSITION)
			{
				//如果当前处于位置模式，则立即生效
				eError = TMC5160_WriteInt(eTMC, TMC5160_AMAX, (puData->lData)*A_CHANGE_CONST);
			}
			//使用时，生效
			g_tAxisParamDefault.lAMax[eTMC] = puData->lData;
		}
		break;
	case 0x34:
		// 目标（最大）速度 VMAX  Target speed, 用于位置模式
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.lVMax[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			if(g_tAxisParamDefault.ucMode[eTMC] == TMC_MODE_POSITION)
			{
				//如果当前处于位置模式，则立即生效
				eError = TMC5160_WriteInt(eTMC, TMC5160_VMAX, abs(puData->lData)*V_CHANGE_CONST);
			}
			//使用时，生效
			g_tAxisParamDefault.lVMax[eTMC] = abs(puData->lData);
		}
		break;
	case 0x35:
		//第二阶段减速度 DMAX, Maximum Deceleration
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.lDMax[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			eError = TMC5160_WriteInt(eTMC, TMC5160_DMAX, (puData->lData)*A_CHANGE_CONST);
			g_tAxisParamDefault.lDMax[eTMC] = puData->lData;
		}
		break;		
	case 0x36:
		//第一阶段减速度 Deceleration D1
		if(eReadWrite == TMC_READ) {			
			puData->lData = g_tAxisParamDefault.lD1[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			eError = TMC5160_WriteInt(eTMC, TMC5160_D1, (puData->lData)*A_CHANGE_CONST);
			g_tAxisParamDefault.lD1[eTMC] = puData->lData;
		}
		break;			
	case 0x37:
		// 停止速度 Velocity VSTOP
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
		// 当前实际速度 Actual speed
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
		// 速度模式下，加速度值
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.lAMax_VMode[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			if(g_tAxisParamDefault.ucMode[eTMC] == TMC_MODE_VELNEG || g_tAxisParamDefault.ucMode[eTMC] == TMC_MODE_VELPOS)
			{
				//如果当前为速度模式，立即生效
				eError = TMC5160_WriteInt(eTMC, TMC5160_AMAX, (puData->lData)*A_CHANGE_CONST);
			}
			//使用时，生效
			g_tAxisParamDefault.lAMax_VMode[eTMC] = puData->lData;
		}
		break;
	case 0x41:
		// 速度模式下，目标（最大）速度 VMAX  Target speed
		if(eReadWrite == TMC_READ) {
			puData->lData = g_tAxisParamDefault.lVMax[eTMC];
		} else if(eReadWrite == TMC_WRITE){
			if(g_tAxisParamDefault.ucMode[eTMC] == TMC_MODE_VELNEG || g_tAxisParamDefault.ucMode[eTMC] == TMC_MODE_VELPOS)
			{
				//如果当前为速度模式，立即生效
				eError = TMC5160_WriteInt(eTMC, TMC5160_VMAX, abs(puData->lData)*V_CHANGE_CONST);
			}
			//使用时，生效
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



		
	/***  参考开关相关 ***/	
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
		// 右限位极性 right limit switch polarity
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
		// 左限位极性 left limit switch polarity
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
		// 右限位触发标志 right limit triggle flag
		if(eReadWrite == TMC_READ) {
			puData->lData = TMC5160_FIELD_READ(eTMC, TMC5160_RAMPSTAT, TMC5160_STATUS_STOP_R_MASK, TMC5160_STATUS_STOP_R_SHIFT);
		} else if(eReadWrite == TMC_WRITE) {
			LOG_Error("Left Limit Flag Do Not Be Write");
			eError = ERROR_TYPE_RW_RIGHT;
		}
		break;

	case 0x57:
		// 左限位触发标志
		if(eReadWrite == TMC_READ) {
			puData->lData = TMC5160_FIELD_READ(eTMC, TMC5160_RAMPSTAT, TMC5160_STATUS_STOP_L_MASK, TMC5160_STATUS_STOP_L_SHIFT);
		} else if(eReadWrite == TMC_WRITE) {
			LOG_Error("Left Limit Flag Do Not Be Write");
			eError = ERROR_TYPE_RW_RIGHT;
		}
		break;
	case 0x58:
		// 电机使能
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
		// 电机反向
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
		// 获取TMC5160的寄存器的值
		if(eReadWrite == TMC_READ) {
			uint8_t ucAddr = puData->lData;
			
			if(CheckRegister_Addr(ucAddr) != ERROR_TYPE_SUCCESS) return ERROR_TYPE_DATA;
			puData->lData = TMC5160_ReadInt(eTMC, puData->lData);
			
		} else if(eReadWrite == TMC_WRITE) {
			return ERROR_TYPE_DATA;
		}
		break;
		
	/***  斩波相关 ***/	
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
					*plValue |= 1<<3; // MSB wird zu plValue dazugefügt
			}
		} else if(eReadWrite == TMC_WRITE) {
			if(TMC5160_ReadInt(eTMC, TMC5160_CHOPCONF) & (1<<14))
			{
				TMC5160_FIELD_UPDATE(eTMC, TMC5160_CHOPCONF, TMC5160_HEND_MASK, TMC5160_HEND_SHIFT, *plValue);
			}
			else
			{
				TMC5160_FIELD_UPDATE(eTMC, TMC5160_CHOPCONF, TMC5160_TFD_3_MASK, TMC5160_TFD_3_SHIFT, (*plValue & (1<<3))); // MSB wird zu plValue dazugefügt
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
					*plValue |= 1<<3; // MSB wird zu plValue dazugefügt
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


		
	/***  PWM相关 ***/	
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
//*	设置默认轴参数
//*/
//ErrorType_e TMC_AxisParam_Default(TMC_e eTMC, ReadWrite_e eReadWrite, uint8_t ucType, Data4Byte_u *puData)
//{
//	ErrorType_e eError = ERROR_TYPE_SUCCESS;
//	
//	//参数检测
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
*	设置默认轴参数
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
	
	//参数检测
	if(eTMC >= TMC_END)
	{
		LOG_Error("TMC DeviceID=%d Is Error", eTMC);
		return ERROR_TYPE_DEVICE_ID;
	}
	
	//读取默认轴参数
	if(ERROR_TYPE_SUCCESS != Read_Axis_Param_Default(&tAxisParamDefault))
	{
		return ERROR_TYPE_CRC;
	}
	
	
	//
	switch(ucType)
	{

	/*** 编码器参数 ***/
	case 0x04:
		// 编码器分辨率 Encoder Resolution
		if(eReadWrite == TMC_READ) {
			puData->lData = tAxisParamDefault.usEncResultion[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			if(tAxisParamDefault.usEncResultion[eTMC] != puData->lData)
			{
				//值不同，需更新
				tAxisParamDefault.usEncResultion[eTMC] = puData->lData;
				//更新的参数
				Update_Enc_ConstValue(eTMC, &tAxisParamDefault, 0);
				ucSaveFlag = 1;
			}
		}
		break;	
		
	case 0x05:		
		// 编码器反向
		if(eReadWrite == TMC_READ) {
			puData->lData = tAxisParamDefault.ucEncCountDirect[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			if(puData->lData == 0 || puData->lData == 1)
			{ 
				if(tAxisParamDefault.ucEncCountDirect[eTMC] != puData->lData)
				{
					tAxisParamDefault.ucEncCountDirect[eTMC] = puData->lData;
					//更新的参数
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
		// 丢步阈值 
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

		
		
		
	/*** 复位相关 ***/
	case 0x10:
		//复位速度  低
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
		//复位速度  高
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
		//复位偏移距离
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
		//复位加速度
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
		
		
	/*** 电流相关 ****/	
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
		//运行电流
		if(eReadWrite == TMC_READ) {
			puData->lData = tAxisParamDefault.ucIRun[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			
			//参数检擦
			if(puData->lData > CURRENT_MAX_VALUE)
			{
				LOG_Error("Current Set Value=%d More than %d", puData->lData, CURRENT_CHANGE_CONST);
				return ERROR_TYPE_DATA;
			}
			
			//设置参数
			if(tAxisParamDefault.ucIRun[eTMC] != puData->lData)
			{				
				tAxisParamDefault.ucIRun[eTMC] = puData->lData;
				ucSaveFlag = 1;
			}
		}
		break;
	case 0x21:
		//保持电流
		if(eReadWrite == TMC_READ) {
			puData->lData = tAxisParamDefault.ucIHold[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			//参数检擦
			if(puData->lData > CURRENT_MAX_VALUE)
			{
				LOG_Error("Current Set Value=%d More than %d", puData->lData, CURRENT_CHANGE_CONST);
				return ERROR_TYPE_DATA;
			}
			
			//设置参数
			if(tAxisParamDefault.ucIHold[eTMC] != puData->lData)
			{
				tAxisParamDefault.ucIHold[eTMC] = puData->lData;
				ucSaveFlag = 1;
			}			
		}
		break;		
		
	case 0x23:
		// 步进细分 Microstep Resolution
		if(eReadWrite == TMC_READ) {		
			puData->lData = tAxisParamDefault.usMicroStepResultion[eTMC];
		} else if(eReadWrite == TMC_WRITE) {
			uint16_t usMicroStep = puData->lData;
			uint8_t ucMicroStep_Rg = 0;
			
			//设置更新
			if(usMicroStep != tAxisParamDefault.usMicroStepResultion[eTMC])
			{
				//将分辨率设置值  转换成 寄存器分辨率值
				eErrorType = MicroStep_SetValue2Register(usMicroStep, &ucMicroStep_Rg);
				if(eErrorType != ERROR_TYPE_SUCCESS)
				{
					LOG_Error("Data Micro Step=%d Error", usMicroStep);
					return ERROR_TYPE_DATA;
				}
			
				//写入寄存器
				tAxisParamDefault.usMicroStepResultion[eTMC] = usMicroStep;
				//保存更新的参数
				Update_Enc_ConstValue(eTMC, &tAxisParamDefault, 0);
				ucSaveFlag = 1;
//				LOG_Info("Micro Step Resolution=%d", usMicroStep);
			}		
		}
		break;		
	case 0x24:
	{
		//电机没转全步数
		if(eReadWrite == TMC_READ) {
			puData->lData = tAxisParamDefault.usFullStepPerRound[eTMC];
		}else if(eReadWrite == TMC_WRITE) {
			if(puData->lData != tAxisParamDefault.usFullStepPerRound[eTMC])
			{
				tAxisParamDefault.usFullStepPerRound[eTMC] = puData->lData;
				//保存更新的参数
				Update_Enc_ConstValue(eTMC, &tAxisParamDefault, 0);
				ucSaveFlag = 1;
//				LOG_Info("Full Step Per Round=%d", puData->lData);
			}
		}
	}
	break;
		
		
	/*** 六点加速相关 ***/		
	case 0x30:
		// 开始速度 Velocity VSTART
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
		// 第一阶段加速度A1  Acceleration A1
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
		// 换挡速度 Velocity V1
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
		// 第二阶段加速度 AMAX  Maximum acceleration，用于位置模式
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
		// 目标（最大）速度 VMAX  Target speed，用于位置模式
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
		//第二阶段减速度 DMAX, Maximum Deceleration
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
		//第一阶段减速度 Deceleration D1
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
		// 停止速度 Velocity VSTOP
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
		// 速度模式下，加速度值
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
		// 速度模式下，目标（最大）速度 VMAX  Target speed
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
	

	/***  左右限位  ***/
	case 0x54:
		// 右限位极性 right limit switch polarity
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
		// 左限位极性 left limit switch polarity
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
		// 电机反向
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
	

	/* 保存 */
	if(ucSaveFlag == 1)
	{
		//重新计算CRC
		tAxisParamDefault.usCrc = CRC16((uint8_t*)&tAxisParamDefault, sizeof(AxisParamDefault_t)-2);
	
		//保存轴参数
		//Param_Write(EN_SAVE_PARAM_TYPE_AXIS, (uint8_t*)&g_tAxisParamDefault, sizeof(AxisParamDefault_t));
		Save_Axis_Param_Default(&tAxisParamDefault);
	}
	return eErrorType;
}





/*
*	清除EEPROM保存的轴参数
*/
void ClearAndSave_Default_Axis_Params(void)
{
	Axis_Param_Fixed_SetDefault_Value(&g_tAxisParamDefault);
	//Param_Write(EN_SAVE_PARAM_TYPE_AXIS, (uint8_t*)&g_tAxisParamDefault, sizeof(AxisParamDefault_t));
	Save_Axis_Param_Default(&g_tAxisParamDefault);
}








/* 
* 
*  读取默认轴参数 
*
*/
ErrorType_e Read_Axis_Param_Default(__IO AxisParamDefault_t *ptAxisParamDefault)
{
	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
	uint8_t ucNum = 0;
	uint16_t usCrc = 0;
	
	//读取参数	
	do{
		//计数
		ucNum++;
		
		memset((void*)ptAxisParamDefault, 0, sizeof(AxisParamDefault_t));
		Param_Read(EN_SAVE_PARAM_TYPE_AXIS, (uint8_t*)ptAxisParamDefault, sizeof(AxisParamDefault_t));
		usCrc = CRC16((uint8_t*)ptAxisParamDefault, sizeof(AxisParamDefault_t)-2);
		
		//数据校验
		if(usCrc == ptAxisParamDefault->usCrc)
		{
			return ERROR_TYPE_SUCCESS;			
		}else{
			rt_thread_delay(2);//HAL_Delay(2);
		}
		
	}while(ucNum < 3);

	
	//第一次上电
	if(usCrc != ptAxisParamDefault->usCrc && ptAxisParamDefault->ulInitFlag != PARAM_INIT_FLAG)
	{
		Axis_Param_Fixed_SetDefault_Value(ptAxisParamDefault);
		
		//保存轴参数
		return Save_Axis_Param_Default(ptAxisParamDefault);
	}

	
	//数据校验检测
	if(ucNum >= 3)
	{
		//数据校验失败
//		LOG_Warn("EEPROM Axis Default Param CRC Error");
		return ERROR_TYPE_EEPROM;
	}
	
	return eErrorType;
}


/* 
* 
*  保存默认轴参数 
*
*/
ErrorType_e Save_Axis_Param_Default(__IO AxisParamDefault_t *ptAxisParamDefault)
{
	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
	uint8_t  ucNum = 0;
	uint16_t usWriteCrc = 0, usReadCrc = 0;
	AxisParamDefault_t  tAxisParamDefault = {0};
	
	//计数原始数据CRC
	usWriteCrc = ptAxisParamDefault->usCrc;
	
	//读取参数	
	do{
		//计数
		ucNum++;
		
		//写入
		memmove((void*)&tAxisParamDefault, (void*)ptAxisParamDefault, sizeof(AxisParamDefault_t));
		Param_Write(EN_SAVE_PARAM_TYPE_AXIS, (uint8_t*)&tAxisParamDefault, sizeof(AxisParamDefault_t));
			
		//读取
		memset((void*)&tAxisParamDefault, 0, sizeof(AxisParamDefault_t));
		Param_Read(EN_SAVE_PARAM_TYPE_AXIS, (uint8_t*)&tAxisParamDefault, sizeof(AxisParamDefault_t));
		usReadCrc = CRC16((uint8_t*)&tAxisParamDefault, sizeof(AxisParamDefault_t)-2);
		
		//对比前后crc
		if(usWriteCrc == usReadCrc || usReadCrc == tAxisParamDefault.usCrc)
		{
			return ERROR_TYPE_SUCCESS;
		}else{
			rt_thread_delay(2);//HAL_Delay(2);
		}
		
	}while(ucNum < 3);  //最多重复3次

	//保存失败
	if(ucNum >= 3)
	{
		LOG_Error("Save Process Fail");
		return ERROR_TYPE_EEPROM;
	}

	return eErrorType;
}





/*
*	流程参数 清除
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
*	清除EEPROM保存的流程
*/
void ClearAndSave_Default_Process(void)
{
	Process_Param_SetDefault_Value(&g_tProcess);
	Param_Write(EN_SAVE_PARAM_TYPE_PROCESS, (uint8_t*)&g_tProcess, sizeof(Process_t));
}




/*
*	流程初始化
*/
ErrorType_e Process_Init(void)
{
	return Read_Process(&g_tProcess);
}




  
/*
*	设置RAM中流程，根据Index任意插入子流程，参数的顺序必须按指令传参顺序
*/
ErrorType_e Set_Process(RecvFrame_t *ptRecvFrame)
{
	ErrorType_e eError = ERROR_TYPE_SUCCESS;
	
	uint8_t ucIndex = ptRecvFrame->ucType & 0x7F;
	if(ucIndex > SUB_PROCESS_MAX_CMD_NUM)
	{
		//最大支持128指令
		LOG_Error("SubProcess Index=%d Error", ucIndex);
		return ERROR_TYPE_DATA;
	}
			
	if(ptRecvFrame->ucType & 0x80)
	{
		//参数
		//if(g_tProcess.taSubProcess[ucIndex].ucParamNum != 0xFF && g_tProcess.taSubProcess[ucIndex].ucParamNum >= SUB_PROCESS_MAX_PARAM_NUM)
		if( g_tProcess.taSubProcess[ucIndex].ucParamNum >= SUB_PROCESS_MAX_PARAM_NUM)
		{
			//参数个数，最多3个
			LOG_Error("SubProcess Parma Index=%d Error", g_tProcess.taSubProcess[ucIndex].ucParamNum);
			return ERROR_TYPE_EXEC;
		}
		
		//获取参数
		g_tProcess.taSubProcess[ucIndex].uParam[g_tProcess.taSubProcess[ucIndex].ucParamNum++].lData = ptRecvFrame->uData.lData;
	
	}else{
		//指令
		memset((void*)&g_tProcess.taSubProcess[ucIndex], 0, sizeof(SubProcess_t));
		g_tProcess.taSubProcess[ucIndex].ucCmd = ptRecvFrame->uData.ucData[0];
		//修改或赋值指令时，参数个数需置零
		//g_tProcess.taSubProcess[ucIndex].ucParamNum = 0xFF;
	
	}
	
	return eError;
}







/*
*	读取 RAM流程
*/
ErrorType_e Get_Process(uint8_t ucIndex, SubProcess_t *ptSubProcess)
{
	//
	if(ucIndex > SUB_PROCESS_MAX_CMD_NUM)
	{
		//最大支持128指令
		LOG_Error("SubProcess Index=%d Error", ucIndex);
		return ERROR_TYPE_DATA;
	}
		
	//获取指令
	memmove((void*)ptSubProcess, (void*)&g_tProcess.taSubProcess[ucIndex], sizeof(SubProcess_t));
	return ERROR_TYPE_SUCCESS;
	
}
	




/*
*	执行流程--控制
*/

ErrorType_e Exec_Process_Ctrl(uint8_t ucType)
{
	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
	
	//
	switch(ucType)
	{
		case 0x00:
		{
			//停止
			g_tProcess.eProcessStatus = EN_PROCESS_STATUS_STOP;
		}
		break;
		case 0x01:
		{
			//运行
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
*	读取流程
*/
ErrorType_e Read_Process(__IO Process_t *ptProcess)
{
	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
	uint8_t ucNum = 0;
	uint16_t usCrc = 0;

	//读取流程	
	do{
		//计数
		ucNum++;
		
		memset((void*)ptProcess, 0, sizeof(Process_t));
		Param_Read(EN_SAVE_PARAM_TYPE_PROCESS, (uint8_t*)ptProcess, sizeof(Process_t));
		usCrc = CRC16((uint8_t*)ptProcess, sizeof(Process_t)-2);
		
		
		//数据校验
		if(usCrc == ptProcess->usCrc)
		{
			return ERROR_TYPE_SUCCESS;			
		}else{
			rt_thread_delay(2);//HAL_Delay(2);
		}
	}while(ucNum < 3);
	
	
	//第一次上电
	if(usCrc != ptProcess->usCrc && ptProcess->ulInitFlag != PARAM_INIT_FLAG)
	{
		Process_Param_SetDefault_Value(ptProcess);
		//保存轴参数
		return Save_Process(ptProcess);
	}
	

	//数据校验检测
	if(ucNum >= 3)
	{
//		LOG_Warn("EEPROM Process Data CRC Error");
		return ERROR_TYPE_EEPROM;
	}
	
	return eErrorType;
}


/*
*	保存流程到EEPROM中
*/
ErrorType_e Save_Process(__IO Process_t *ptProccess)
{
	//
	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
	uint8_t ucNum = 0;
	uint16_t usWriteCrc = 0, usReadCrc = 0;
	Process_t tProcess = {0};
	
	usWriteCrc = ptProccess->usCrc;
	//保存
	do{
		//计数
		ucNum++;
		
		//写入
		memmove((void*)&tProcess, (void*)ptProccess, sizeof(Process_t));
		Param_Write(EN_SAVE_PARAM_TYPE_PROCESS, (uint8_t*)&tProcess, sizeof(Process_t));
		
		//读取
		memset((void*)&tProcess, 0, sizeof(Process_t));
		Param_Read(EN_SAVE_PARAM_TYPE_PROCESS, (uint8_t*)&tProcess, sizeof(Process_t));
		usReadCrc = CRC16((uint8_t*)&tProcess, sizeof(Process_t)-2);
		
		//对比前后crc
		if(usWriteCrc == usReadCrc || usReadCrc == tProcess.usCrc)
		{
			return ERROR_TYPE_SUCCESS;
		}else{
			rt_thread_delay(2);//HAL_Delay(2);
		}
		
	}while(ucNum < 3);  //最多重复3次
	
	
	//保存失败
	if(ucNum >= 3)
	{
		LOG_Error("Save Process Fail");
		return ERROR_TYPE_EEPROM;
	}
		
	return eErrorType;
}




/*
*	删除EEPROM中流程数据
*/
void Del_Process(void)
{
	//删除
	memset((void*)&g_tProcess, 0, sizeof(Process_t));
	g_tProcess.ulInitFlag	  = PARAM_INIT_FLAG;
	g_tProcess.eProcessStatus = EN_PROCESS_STATUS_CLEAR;
	g_tProcess.usCrc          = CRC16((uint8_t*)&g_tProcess, sizeof(Process_t)-2);
	
	//
	Param_Write(EN_SAVE_PARAM_TYPE_PROCESS, (uint8_t*)&g_tProcess, sizeof(Process_t));
}



/*
*	清除RAM中的流程数据
*/
void Clear_Process(void)
{
	//
	memset((void*)&g_tProcess, 0, sizeof(Process_t));
	g_tProcess.eProcessStatus = EN_PROCESS_STATUS_CLEAR;
	
}	



/*
*	执行流程
*/
void Exec_Process(void)
{
	g_tProcess.eProcessStatus = EN_PROCESS_STATUS_EXEC;
}


  
/*
*	停止流程
*/
void Stop_Process(void)
{
	//
	g_tProcess.eProcessStatus = EN_PROCESS_STATUS_STOP;
	
}





/*
*	执行流程--控制
*/

ErrorType_e Exec_Process_Clear_Or_Save(uint8_t ucType)
{
	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
	
	//
	switch(ucType)
	{	
		case 0x00:
		{
			//清空RAM中的执行流程
			Clear_Process();
		}
		break;
		case 0x01:
		{
			//清空RAM和EEPROM中的执行流程
			Del_Process();
		
		}
		break;		
		case 0x02:
		{
			//将RAM执行流程保存到EEPROM中
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
*	流程处理
*/
ErrorType_e Process_Handle(uint32_t ulTicks)
{
	extern __IO  AxisParamDefault_t g_tAxisParamDefault;
	extern BoardStatus_t g_tBoardStatus;
	extern GlobalParam_t g_tGlobalParam;
	extern __IO TMCStatus_t g_tTMCStatus;
	static ProcessStatus_e s_eProcessStatus = EN_PROCESS_STATUS_READY;
	static uint8_t s_ucIndex = 0;	//需跳转的标签
	static uint8_t s_ucFlag = 0;	//执行下条指令标志
	static uint32_t s_ulTick = 0;	//Tick滴答计时
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
		//执行流程
		
		//检测流程是否为空
//		if(0 == g_tProcess.ucSubProcessNum)
//		{
//			LOG_Warn("SubProcess is Empty, Num=%d", g_tProcess.ucSubProcessNum);
//			return eError;
//		}
		
		//执行子流程
//		SubProcess_Handle(ulTicks, &g_tProcess.taSubProcess[s_ucIndex]);
			
		//
		memmove((void*)&tSubProcess, (void*)&g_tProcess.taSubProcess[s_ucIndex], sizeof(SubProcess_t));
		//if(tSubProcess.ucDeviceID > ) LOG_Warn("DeviceID=%d is Error", tSubProcess.ucDeviceID);
		
//		/* 参数检测 */
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
//				//第一次运行，需重新设置电流参数
//				if(g_tBoardStatus.ucMotorMoveFlag[eTMC] == 0)
//				{
//					TMC5160_I_Set(eTMC);
//					g_tBoardStatus.ucMotorMoveFlag[eTMC] = 1;				
//				}
				//旋转 
				eTMC   = (TMC_e)tSubProcess.uParam[0].ucData[0];
				ucType = tSubProcess.uParam[1].ucData[0];
				lValue = tSubProcess.uParam[2].lData;
				TMC_Rotate(eTMC, (RotateDirect_e)ucType, lValue);
				
				TMC_SetVMode_V(eTMC, 0);
			}
			break;
			case CMD_MOVE_POSITION_WITHOUT_ENC:  //0x11
			{
				//关闭编码器失步检测功能
				TMC_WriteInt(eTMC, TMC5160_ENC_DEVIATION, 0);
				
				//第一次运行，需重新设置电流参数
//				if(g_tBoardStatus.ucMotorMoveFlag[eTMC] == 0)
//				{
//					TMC5160_I_Set(eTMC);
//					g_tBoardStatus.ucMotorMoveFlag[eTMC] = 1;				
//				}
			
				//移动	
				eTMC   = (TMC_e)tSubProcess.uParam[0].ucData[0];
				ucType = tSubProcess.uParam[1].ucData[0];
				lValue = tSubProcess.uParam[2].lData;
				
				TMC_WriteInt(eTMC, TMC5160_ENC_DEVIATION, 0); //关闭编码器失步检测功能
				if(0 == ucType)
				{
					//绝对偏移
					TMC_MoveTo(eTMC, lValue);
				}else if(1 == ucType){
					//相对偏移
					TMC_MoveBy(eTMC, lValue);
				}else if(2 == ucType){
					//缓冲值，移动
					TMC_MoveTo(eTMC, s_uResult.lData);
					
				}
				TMC_SetPMode_V(eTMC, 0);			
			}
			break;
			case CMD_MOVE_POSITION_WITH_ENC:  //0x12
			{
				//编码器失步阈值, 该值为零，则关闭该功能			
				TMC_WriteInt(eTMC, TMC5160_ENC_DEVIATION, g_tAxisParamDefault.lEncDiff_Threshold[eTMC]);
	//			LOG_Info("ENC Deviation Start: Motor=%d, EncDiff=%d, Steps=%d, CurStep=%d", \
						eTMC, g_tAxisParamDefault.lEncDiff_Threshold[eTMC], ptRecvFrame->uData.lData, TMC5160_ReadInt(eTMC, TMC5160_XACTUAL));
		
				//第一次运行，需重新设置电流参数
//				if(g_tBoardStatus.ucMotorMoveFlag[eTMC] == 0)
//				{
//					TMC5160_I_Set(eTMC);
//					g_tBoardStatus.ucMotorMoveFlag[eTMC] = 1;				
//				}
			
				//移动		
				eTMC   = (TMC_e)tSubProcess.uParam[0].ucData[0];
				ucType = tSubProcess.uParam[1].ucData[0];
				lValue = tSubProcess.uParam[2].lData;
				
				TMC_WriteInt(eTMC, TMC5160_ENC_DEVIATION, g_tAxisParamDefault.lEncDiff_Threshold[eTMC]); //设置编码器失步检测功能
				if(0 == ucType)
				{
					//绝对偏移
					TMC_MoveTo(eTMC, lValue);
				}else if(1 == ucType){
					//相对偏移
					TMC_MoveBy(eTMC, lValue);
				}else if(2 == ucType){
					//缓冲值，移动
					TMC_MoveTo(eTMC, s_uResult.lData);
					
				}
				TMC_SetPMode_V(eTMC, 0);
			}
			break;
			case CMD_STOP:   //0x13
			{
				//停止
				eTMC   = (TMC_e)tSubProcess.uParam[0].ucData[0];
				TMC_Stop(eTMC);
			}
			break;
			case CMD_MOTOR_RESET: //0x14
			{
				//复位，到原点（参考位置）
				eTMC   = (TMC_e)tSubProcess.uParam[0].ucData[0];
				TMC_Reset(eTMC, TMC_REF_LEFT);  //TMC_REF_RIGHT   TMC_REF_LEFT
			}
			break;
			
			
			/**********************/
			case CMD_QUERY_BOARD_TYPE:  //0x21
			{
				//查询板卡类型
				s_uResult.ulData = Get_Module_Type();				
			}
			break;		
			case CMD_HARD_SOFT_VERSION:  //0x22
			{
				//查询软硬件版本
				Get_Soft_HardWare_Version(s_uResult.ucData);				
			}
			break;
			
			
			/**********************/
			case CMD_SET_AXIS_PARAM:  //0x30
			{
				//设置轴参数
				eTMC   = (TMC_e)tSubProcess.uParam[0].ucData[0];
				ucType = tSubProcess.uParam[1].ucData[0];
				
				TMC_AxisParam(eTMC, TMC_WRITE, ucType, &tSubProcess.uParam[2]);  
				//LOG_Debug("Axis Param Add(%X)  %d", ucType, tSubProcess.uParam[2].lData);

			}
			break;
			case CMD_GET_AXIS_PARAM:  //0x31
			{
				//获取轴参数
				eTMC   = (TMC_e)tSubProcess.uParam[0].ucData[0];
				ucType = tSubProcess.uParam[1].ucData[0];
				
				TMC_AxisParam(eTMC, TMC_READ, ucType, &s_uResult);  
			}
			break;
			case CMD_SET_GLOBAL_PARAM:  //0x33
			{
				//设置全局参数
				Bank_e eBank   = (Bank_e)tSubProcess.uParam[0].ucData[0];
				ucType = tSubProcess.uParam[1].ucData[0];
				lValue = tSubProcess.uParam[2].lData;
				
				TMC_Global_Param(eBank, TMC_WRITE, ucType, &tSubProcess.uParam[2]);
			}
			break;
			case CMD_GET_GLOBAL_PARAM:  //0x34
			{
				//查询模块参数
				Bank_e eBank   = (Bank_e)tSubProcess.uParam[0].ucData[0];
				ucType = tSubProcess.uParam[1].ucData[0];
				//lValue = tSubProcess.uParam[2].lData;
				
				TMC_Global_Param(eBank, TMC_READ, ucType, &s_uResult);
			}
			break;			
			case CMD_SET_IO_STATUS:  //0x34
			{
				//设置IO状态
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
//				//查询IO状态
//				uint16_t usOutState = 0, usInState = 0;
//			
//				//获取输出IO状态
//				Get_Out_IO(&usOutState);
//				//获取输入IO状态
//				Get_In_IO(&usInState);
//				
//				//IO状态
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
				//跳转标签, 标签带的参数:label_index，切换到该subprocess
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
						//复位完成
						//LOG_Info("Motor Reset Finished");
						s_ucFlag = 0;
						
					}else{
						//未完成
						s_ucFlag = 1;					
					}	
				}else if(1 == ucType){ //wait move finished						
					if(1 == TMC5160_FIELD_READ(eTMC, TMC5160_RAMPSTAT, TMC5160_RAMPSTAT_POS_REACH_MASK, TMC5160_RAMPSTAT_POS_REACH_SHIFT))
					{
						//move 步数完成
						//LOG_Info("Move Step Finished");
						s_ucFlag = 0;
						
					}else{
						//未完成
						s_ucFlag = 1;					
					}
				}else if(2 == ucType){ //wait speed reach max value
					if(1 == TMC5160_FIELD_READ(eTMC, TMC5160_RAMPSTAT, TMC5160_RAMPSTAT_VELOCITY_IS_ZERO_MASK, TMC5160_RAMPSTAT_VELOCITY_IS_ZERO_SHIFT))
					{
						//到达最大速度
						//LOG_Info("Reach Max Speed");
						s_ucFlag = 0;
						
					}else{
						//未完成
						s_ucFlag = 1;					
					}
				}else if(3 == ucType){ //wait speed = 0
					if(0 == TMC5160_ReadInt(eTMC, TMC5160_VACTUAL))
					{
						//速度为零
						//LOG_Info("Speed Is ZERO");
						s_ucFlag = 0;
						
					}else{
						//未完成
						s_ucFlag = 1;					
					}
				}
			}
			break;
			case CMD_DELAY:
			{
				if(0 == s_ucFlag)
				{
					//延时未达到
					ulValue = tSubProcess.uParam[0].ulData;
					s_ulTick = ulTicks + ulValue;
					s_ucFlag = 1;
					
				}else{
					if(ulTicks >= s_ulTick)
					{
						//延时到达
						s_ucFlag = 0;
						s_ulTick = ulTicks;
						//s_ucIndex++;
					}
				}
			}
			break;
			case CMD_CALC:
			{
				//值计算
				ucType = tSubProcess.uParam[0].ucData[0];
				lValue = tSubProcess.uParam[1].lData;
				
				//上一次结果 tResult.uResult.ulValue ;
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
						//右位移
						s_uResult.ulData >>= lValue;
					}
					break;
					case EN_CALC_LEFT_LIFT:
					{
						//左位移
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
				//无条件跳转
				uint8_t ucLabelIndex = tSubProcess.uParam[0].ucData[0];
			    s_ucIndex = ucLabelIndex;
			}
			break;
			case CMD_JC:
			{
				//条件跳转
				//值比较
				ucType = tSubProcess.uParam[0].ucData[0]; //比较方式
				lValue = tSubProcess.uParam[1].lData; 	  //比较值
				uint8_t ucLabelIndex = tSubProcess.uParam[2].ucData[0];	//标签index
				//LOG_Debug("Type=%d, CompareV=%d, Label=%d, Result=%d",ucType, lValue, ucLabelIndex, s_uResult.lData);
				switch(ucType)
				{
					case EN_COMPARE_JC_EQ: //等于
					{
						if(s_uResult.ulData == lValue)
						{
							s_ucIndex = ucLabelIndex;
						}
					}
					break;
					case EN_COMPARE_JC_NE: //不等于
					{
						if(s_uResult.ulData != lValue)
						{
							s_ucIndex = ucLabelIndex;
						}
					}
					break;
					case EN_COMPARE_JC_GT: //大于
					{
						if(s_uResult.ulData > lValue)
						{
							s_ucIndex = ucLabelIndex;
						}
					}
					break;
					case EN_COMPARE_JC_GE: //大于等于
					{
						if(s_uResult.ulData >= lValue)
						{
							s_ucIndex = ucLabelIndex;
						}
					}
					break;
					case EN_COMPARE_JC_LT: //小于
					{
						if(s_uResult.ulData < lValue)
						{
							s_ucIndex = ucLabelIndex;
						}
					}
					break;
					case EN_COMPARE_JC_LE: //小于等于
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
				//设置轴参数
				eTMC   = (TMC_e)tSubProcess.uParam[0].ucData[0];
				TMC_AxisParam(eTMC, TMC_WRITE, ucType, &s_uResult);  
				//LOG_Debug("Axis Param Add(%X)  %d", ucType, tSubProcess.uParam[2].lData);

			}
			break;
			case CMD_SET_GLOBAL_PARAM_AGP:  //0x98
			{
				//设置全局参数
				Bank_e eBank   = (Bank_e)tSubProcess.uParam[0].ucData[0];
				ucType = tSubProcess.uParam[1].ucData[0];
				
				
				TMC_Global_Param(eBank, TMC_WRITE, ucType, &s_uResult);
			}
			break;
			case CMD_TIME_COUNT:
			{
				//启动计时器
				ucType = tSubProcess.uParam[0].ucData[0]; //编号
				ulValue = tSubProcess.uParam[1].ulData; //计时时间
				
				//计时信息
				g_tProtcessTimeCount.ulStartFlag[ucType] = 1;
				g_tProtcessTimeCount.ulTriggerFlag[ucType] = 0;
				g_tProtcessTimeCount.ulStartTick[ucType] = rt_tick_get();//HAL_GetTick();
				g_tProtcessTimeCount.ulThreshTick[ucType] = ulValue;
						
			}
			break;
			case CMD_TIME_COUNT_A:
			{
				//启动计时器--缓存值方式
				ucType = tSubProcess.uParam[0].ucData[0]; //编号
				
				//计时信息
				g_tProtcessTimeCount.ulStartFlag[ucType] = 1;
				g_tProtcessTimeCount.ulTriggerFlag[ucType] = 0;
				g_tProtcessTimeCount.ulStartTick[ucType] = rt_tick_get();//HAL_GetTick();
				g_tProtcessTimeCount.ulThreshTick[ucType] = s_uResult.ulData;									
			}
			break;
			case CMD_TIME_COUNT_CHECK:
			{
				//计时器到时检测
				ucType = tSubProcess.uParam[0].ucData[0]; //编号
				if(g_tProtcessTimeCount.ulTriggerFlag[ucType] == 1)
				{
					//触发
					s_uResult.ulData = 1;		
				}else{
					//未触发
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
			
		//执行下一条指令
	    if(0 == s_ucFlag) s_ucIndex++; 
		
	}else if(EN_PROCESS_STATUS_STOP == g_tProcess.eProcessStatus){
		if(EN_PROCESS_STATUS_EXEC == s_eProcessStatus)
		{
			//由执行到停止，执行善后工作
			
		}
		s_ucIndex = 0;
		
	}else{
		//其他状态，不需要执行
		s_ucIndex = 0;
	}
	
	//执行完一轮后，从新开始执行
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













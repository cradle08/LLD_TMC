#include "main.h"
#include "bsp_spi.h"
#include "TMC5160.h"
#include "TMC_Api.h"
#include "bsp_outin.h"
#include "public.h"
#include "log.h"
#include "TMC5160_Constants.h"
#include "process.h"
#include "TMC_Process.h"

/*
 * set TMC mode work mode
 */
ErrorType_e TMC_Mode_Set(TMC_e eIndex, TMC_Work_Mode_e eMode)
{
	ErrorType_e eError =  ERROR_TYPE_SUCCESS;
	switch(eIndex)
	{
		case TMC_0:
		case TMC_1:
		case TMC_2:
		case TMC_3:
		case TMC_4:
		case TMC_5:
		{
			if(TMC_WORK_MODE_1 == eMode)
			{
				//SD_MODE=0,SPI_MODE=1, only use spi
//				TMC1_SPI_MODE_HIGH;
//				TMC1_SD_MODE_LOW;

			}else if(TMC_WORK_MODE_2 == eMode){
				//SD_MODE=1,SPI_MODE=1, use spi set parameter, and than use step/dir to driver
//				TMC1_SPI_MODE_HIGH;
//				TMC1_SD_MODE_HIGH;
			}else if(TMC_WORK_MODE_3 == eMode){
				//SD_MODE=1,SPI_MODE=0, only use step/dir to driver
//				TMC1_SPI_MODE_LOW;
//				TMC1_SD_MODE_HIGH;
			}
		}
		break;
		default:
		{
			return ERROR_TYPE_DEVICE_ID;
		}
//		break;
	}
	
	return eError;
}




/*
 *  打印寄存器的值
 */
void Print_Register_Value(TMC_e eTMC,  uint8_t ucAddr)
{
	int32_t lValue = TMC_ReadInt(eTMC, ucAddr);
	LOG_Info("Addr(%X): %X=%d", ucAddr, lValue, lValue);

}



/*
 *  打印所有寄存器的值
 */
void Print_AllRegister_Value(TMC_e eTMC)
{
	for(uint8_t i = 0; i < TMC5160_REGISTER_COUNT; i++)
	{
		Print_Register_Value(eTMC, i);
	}
	LOG_Info("-------------------------");

}



/*
*	设置/更新速度模式下，加减速值
*/
void TMC_SetVMode_V(TMC_e eTMC, uint8_t ucOpt)
{
	extern AxisParamDefault_t g_tAxisParamDefault;
	
	if(eTMC >= TMC_MODULE_END)
	{
		LOG_Error("unKonwn TMC NUM=%d", eTMC);
		return;
	}
	
	if(ucOpt == 0)
	{
		TMC_WriteInt(eTMC, TMC5160_AMAX, g_tAxisParamDefault.lAMax_VMode[eTMC]*A_CHANGE_CONST);  
	}else{
		TMC_WriteInt(eTMC, TMC5160_AMAX, g_tAxisParamDefault.lAMax_VMode[eTMC]*A_CHANGE_CONST);  
		TMC_WriteInt(eTMC, TMC5160_VMAX, g_tAxisParamDefault.lVMax_VMode[eTMC]*V_CHANGE_CONST);
	}
}




/*
*	设置/更新位置模式下，加减速值
*/
void TMC_SetPMode_V(TMC_e eTMC, uint8_t ucOpt)
{
	extern AxisParamDefault_t g_tAxisParamDefault;
	
	if(eTMC >= TMC_MODULE_END)
	{
		LOG_Error("unKonwn TMC NUM=%d", eTMC);
		return;
	}
	
   if(ucOpt == 0)
   {
	    TMC_WriteInt(eTMC, TMC5160_AMAX, g_tAxisParamDefault.lAMax[eTMC]*A_CHANGE_CONST);  
	    TMC_WriteInt(eTMC, TMC5160_VMAX, g_tAxisParamDefault.lVMax[eTMC]*V_CHANGE_CONST);
   }else{
		TMC_WriteInt(eTMC, TMC5160_VSTART, g_tAxisParamDefault.lVStart[eTMC]*V_CHANGE_CONST);
		TMC_WriteInt(eTMC, TMC5160_A1, g_tAxisParamDefault.lA1[eTMC]*A_CHANGE_CONST);      			
		TMC_WriteInt(eTMC, TMC5160_V1, g_tAxisParamDefault.lV1[eTMC]*V_CHANGE_CONST);     			
		TMC_WriteInt(eTMC, TMC5160_AMAX, g_tAxisParamDefault.lAMax[eTMC]*A_CHANGE_CONST);       			
		if(ucOpt == 1)
		{
			TMC_WriteInt(eTMC, TMC5160_VMAX, g_tAxisParamDefault.lVMax[eTMC]*V_CHANGE_CONST);   			
		}else{
			//用在复位中，停止
			TMC_WriteInt(eTMC, TMC5160_VMAX, 0);   			
		}
		TMC_WriteInt(eTMC, TMC5160_DMAX, g_tAxisParamDefault.lDMax[eTMC]*A_CHANGE_CONST);       			
		TMC_WriteInt(eTMC, TMC5160_D1, g_tAxisParamDefault.lD1[eTMC]*A_CHANGE_CONST);     				
		TMC_WriteInt(eTMC, TMC5160_VSTOP, g_tAxisParamDefault.lVStop[eTMC]*V_CHANGE_CONST);    
   }
	
//	/*  ?????? */
//	//TMC5160_WriteInt(eTMC, TMC5160_VSTART ,0);       			
//	TMC5160_WriteInt(eTMC, TMC5160_VSTART, g_tAxisParamDefault.lVStart[eTMC]);
//	TMC5160_WriteInt(eTMC, TMC5160_A1,   g_tAxisParamDefault.lA1[eTMC]);      			//PAGE35:A1=1000???????
//	TMC5160_WriteInt(eTMC, TMC5160_V1,   g_tAxisParamDefault.lV1[eTMC]);     			//PAGE35:V1=50000???????V1
//	TMC5160_WriteInt(eTMC, TMC5160_AMAX, g_tAxisParamDefault.lAMax[eTMC]);       			//PAGE35:AMAX=500??V1????
//	TMC5160_WriteInt(eTMC, TMC5160_VMAX, g_tAxisParamDefault.lVMax[eTMC]);   			//PAGE35:VMAX=200000
//	TMC5160_WriteInt(eTMC, TMC5160_DMAX, g_tAxisParamDefault.lDMax[eTMC]);       			//PAGE35:DMAX=700??V1????
//	TMC5160_WriteInt(eTMC, TMC5160_D1,   g_tAxisParamDefault.lD1[eTMC]);     				//PAGE35:D1=1400??V1????
//	TMC5160_WriteInt(eTMC, TMC5160_VSTOP,g_tAxisParamDefault.lVStop[eTMC]);       			//PAGE35:VSTOP=10????,???0

	
//		TMC5160_WriteInt(eTMC, TMC5160_VSTART ,g_taTMC5160[eTMC].laShadowRegister[TMC5160_VSTART]);      			//
//		TMC5160_WriteInt(eTMC, TMC5160_A1 ,g_taTMC5160[eTMC].laShadowRegister[TMC5160_A1]);      			//PAGE35:A1=1000???????
//		TMC5160_WriteInt(eTMC, TMC5160_V1 ,g_taTMC5160[eTMC].laShadowRegister[TMC5160_V1]);     			//PAGE35:V1=50000???????V1
//		TMC5160_WriteInt(eTMC, TMC5160_AMAX ,g_taTMC5160[eTMC].laShadowRegister[TMC5160_AMAX]);       			//PAGE35:AMAX=500??V1????
//		TMC5160_WriteInt(eTMC, TMC5160_VMAX ,g_taTMC5160[eTMC].laShadowRegister[TMC5160_VMAX]);   			//PAGE35:VMAX=200000
//		TMC5160_WriteInt(eTMC, TMC5160_DMAX ,g_taTMC5160[eTMC].laShadowRegister[TMC5160_DMAX]);       			//PAGE35:DMAX=700??V1????
//		TMC5160_WriteInt(eTMC, TMC5160_D1 ,g_taTMC5160[eTMC].laShadowRegister[TMC5160_D1]);     				//PAGE35:D1=1400??V1????
//		TMC5160_WriteInt(eTMC, TMC5160_VSTOP ,g_taTMC5160[eTMC].laShadowRegister[TMC5160_VSTOP]);       			//PAGE35:VSTOP=10????,???0


//		TMC5160_WriteInt(eTMC, TMC5160_VSTART ,	0);      			//PAGE35:A1=1000???????
//		TMC5160_WriteInt(eTMC, TMC5160_A1 ,		10000);      		//PAGE35:A1=1000???????
//		TMC5160_WriteInt(eTMC, TMC5160_V1 ,		500000);     		//PAGE35:V1=50000???????V1
//		TMC5160_WriteInt(eTMC, TMC5160_AMAX ,	50000);       		//PAGE35:AMAX=500??V1????
//		TMC5160_WriteInt(eTMC, TMC5160_VMAX ,	5000000);   		//PAGE35:VMAX=200000
//		TMC5160_WriteInt(eTMC, TMC5160_DMAX ,	70000);       		//PAGE35:DMAX=700??V1????
//		TMC5160_WriteInt(eTMC, TMC5160_D1 ,		14000);     		//PAGE35:D1=1400??V1????
//		TMC5160_WriteInt(eTMC, TMC5160_VSTOP ,	10);       			//PAGE35:VSTOP=10????,???0
		
		
//		TMC5160_WriteInt(eTMC, TMC5160_VSTART ,0);      			//PAGE35:A1=1000???????
//		TMC5160_WriteInt(eTMC, TMC5160_A1 ,10000);      			//PAGE35:A1=1000???????
//		TMC5160_WriteInt(eTMC, TMC5160_V1 ,500000);     			//PAGE35:V1=50000???????V1
//		TMC5160_WriteInt(eTMC, TMC5160_AMAX ,5000);       			//PAGE35:AMAX=500??V1????
//		TMC5160_WriteInt(eTMC, TMC5160_VMAX ,5000000);   			//PAGE35:VMAX=200000
//		TMC5160_WriteInt(eTMC, TMC5160_DMAX ,7000);       			//PAGE35:DMAX=700??V1????
//		TMC5160_WriteInt(eTMC, TMC5160_D1 ,14000);     				//PAGE35:D1=1400??V1????
//		TMC5160_WriteInt(eTMC, TMC5160_VSTOP ,0);       			//PAGE35:VSTOP=10????,???0
//	}

}




/*
 *  TMC Module Init
 */
void TMC_Init(void)
{
	extern TMCStatus_t g_tTMCStatus;
	TMC_e eTMC;
	
	//
	for(eTMC = TMC_0; eTMC < TMC_MODULE_END; eTMC++)
	{
		//TMC_Mode_Set(eTMC, TMC_WORK_MODE_1);
		//HAL_Delay(5);
		TMC5160_Init(eTMC);
		TMC_Enable(eTMC);
		//使能
		g_tTMCStatus.ucEnableFlag[eTMC] = 1;
		
		//Print_6PV_Register(eTMC);
		HAL_Delay(100);
	}	
	
//	HAL_Delay(1000);
//	for(eTMC = TMC_0; eTMC < TMC_MODULE_END; eTMC++)
//	{
//		
//		TMC5160_I_Init(eTMC);
//		HAL_Delay(5);
//	}
	
}




/*
 *  TMC Module Enable
 */
void TMC_Enable(TMC_e eTMC)
{
	switch(eTMC)
	{
		case TMC_0:
		{
			TMC0_ENABLE;
		}
		break;
		case TMC_1:
		{
			TMC1_ENABLE;
		}
		break;
		case TMC_2:
		{
			TMC2_ENABLE; 
		}
		break;
		case TMC_3:
		{
			TMC3_ENABLE;
		}
		break;
		case TMC_4:
		{
			TMC4_ENABLE
		}
		break;
		case TMC_5:
		{
			TMC5_ENABLE
		}
		break;
		default:break;
	}
}

/*
 *  TMC Module Disable
 */
void TMC_Disable(TMC_e eTMC)
{
	switch(eTMC)
	{
		case TMC_0:
		{
			TMC0_DISABLE;
		}
		break;
		case TMC_1:
		{
			TMC1_DISABLE;
		}
		break;
		case TMC_2:
		{
			TMC2_DISABLE; 
		}
		break;
		case TMC_3:
		{
			TMC3_DISABLE;
		}
		break;
		case TMC_4:
		{
			TMC4_DISABLE;
		}
		break;
		case TMC_5:
		{
			TMC5_DISABLE;
		}
		break;
		default:break;
	}

//	return;
}


/*
 * TMC Reset， 堵塞执行
 */
uint8_t TMC_Reset(TMC_e eTMC, TMC_Ref_e eRef)
{
	switch(eTMC)
	{
		case TMC_0:
		case TMC_1:
		case TMC_2:
		case TMC_3:
		case TMC_4:
		case TMC_5:
		{
			TMC5160_Reset(eTMC, eRef);
		}
		break;
		default:break;
	}

	return 0;
}





/*
 * 	TMC Write Register(int-4Byte)
 */
uint8_t TMC_WriteInt(TMC_e eTMC, uint8_t ucAddr, uint32_t ulValue)
{
	uint8_t ucStatus = 0;

	switch(eTMC)
	{
		case TMC_0:
		case TMC_1:
		case TMC_2:
		case TMC_3:
		case TMC_4:
		case TMC_5:
		{
			ucStatus = TMC5160_WriteInt(eTMC, ucAddr, ulValue);
		}
		break;
		default:break;
	}
	return ucStatus;
}


/*
 *  TMC Read Register(int-4Byte)
 */
int32_t TMC_ReadInt(TMC_e eTMC, uint8_t ucAddr)
{
	int32_t lValue = 0;
	switch(eTMC)
	{
		case TMC_0:
		case TMC_1:
		case TMC_2:
		case TMC_3:
		case TMC_4:
		case TMC_5:
		{
			lValue = TMC5160_ReadInt(eTMC, ucAddr);
		}
		break;
		default:break;
	}
	return lValue;
}


/*
 *
 */
uint8_t TMC_WriteDataGram(TMC_e eTMC, uint8_t ucAddr, uint8_t Byte1, uint8_t Byte2, uint8_t Byte3, uint8_t Byte4)
{
	uint8_t ucStatus = 0;
	switch(eTMC)
	{
		case TMC_0:
		case TMC_1:
		case TMC_2:
		case TMC_3:
		case TMC_4:
		case TMC_5:
		{
			ucStatus = TMC5160_WriteDataGram(eTMC, ucAddr, Byte1, Byte2, Byte3, Byte4);
		}
		break;
		default:break;
	}

	return ucStatus;
}





/*
 *	电机旋转，
 */
ErrorType_e TMC_Rotate(TMC_e eTMC, RotateDirect_e eDirect, uint32_t lVelocity)
{
	ErrorType_e eError = ERROR_TYPE_SUCCESS;
	
	//参数检测--旋转方向
	if(eDirect != ROTATE_DIRECT_CLOCKWISE && eDirect != ROTATE_DIRECT_ANTI_CLOCKWISE)
	{
		LOG_Error("Rotate Direct=%d Is Error", eDirect);
		return ERROR_TYPE_TYPE;
	}
	
	switch(eTMC)
	{
		case TMC_0:
		case TMC_1:
		case TMC_2:
		case TMC_3:
		case TMC_4:
		case TMC_5:
		{
			eError = TMC5160_Rotate(eTMC, eDirect, lVelocity);
		}
		break;
		default:
		{
			LOG_Error("TMC DeviceID=%d Is Error", eTMC);
			return ERROR_TYPE_DEVICE_ID;
		}
//		break;
	}
	return eError;
}




/*
 *
 */
ErrorType_e TMC_Stop(TMC_e eTMC)
{
	ErrorType_e eError = ERROR_TYPE_SUCCESS;
	
	switch(eTMC)
	{		
		case TMC_0:
		case TMC_1:
		case TMC_2:
		case TMC_3:
		case TMC_4:
		case TMC_5:
		{
			eError = TMC5160_Stop(eTMC);
		}
		break;
		default:
		{
			LOG_Error("TMC DeviceID=%d Is Error", eTMC);
			return ERROR_TYPE_DEVICE_ID;
		}
		//break;
	}
	
	return eError;
}



ErrorType_e TMC_Stop_Urgent(TMC_e eTMC)
{
	ErrorType_e eError = ERROR_TYPE_SUCCESS;

	switch(eTMC)
	{		
		case TMC_0:
		case TMC_1:
		case TMC_2:
		case TMC_3:
		case TMC_4:
		case TMC_5:
		{					
			TMC_Disable(eTMC);
			LOG_Info("Urgent XTarget=%d", TMC_ReadInt(eTMC, TMC5160_XACTUAL));
			HAL_Delay(1);
			TMC_WriteInt(eTMC, TMC5160_VMAX, 0);
			TMC_Enable(eTMC);
			LOG_Info("Urgent XTarget=%d", TMC_ReadInt(eTMC, TMC5160_XACTUAL));
		}
		break;
		default:
		{
			LOG_Error("TMC DeviceID=%d Is Error", eTMC);
			return ERROR_TYPE_DEVICE_ID;
		}
		//break;
	}
	return eError;
}


/*
 * 绝对移动
 */
ErrorType_e TMC_MoveTo(TMC_e eTMC, int32_t lPosition)
{
	ErrorType_e eError = ERROR_TYPE_SUCCESS;
	
	switch(eTMC)
	{
		case TMC_0:
		case TMC_1:
		case TMC_2:
		case TMC_3:
		case TMC_4:
		case TMC_5:
		{
			eError = TMC5160_MoveTo(eTMC, lPosition);
		}
		break;
		default:
		{
			LOG_Error("TMC DeviceID=%d Is Error", eTMC);
			return ERROR_TYPE_DEVICE_ID;
		}
		//break;
	}
	return eError;
}


/*
 *	相对移动
 */
ErrorType_e TMC_MoveBy(TMC_e eTMC, int32_t lTicks)
{
	ErrorType_e eError = ERROR_TYPE_SUCCESS;
	
	switch(eTMC)
	{
		case TMC_0:
		case TMC_1:
		case TMC_2:
		case TMC_3:
		case TMC_4:
		case TMC_5:
		{
			eError = TMC5160_MoveBy(eTMC, lTicks);
		}
		break;
		default:
		{
			LOG_Error("TMC DeviceID=%d Is Error", eTMC);
			return ERROR_TYPE_DEVICE_ID;
		}
		//break;
	}
	return eError;
}






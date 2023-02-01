
#include "TMC5160.h"
#include "TMC5160_Register.h"
#include "TMC5160_Constants.h"
#include "TMC5160_Fields.h"
//
#include "TMC_Api.h"
#include "bsp_spi.h"
#include "log.h"
//
#include "TMC_Process.h"


//
TMC5160_t g_taTMC5160[TMC_MODULE_END] = {0};




// todo ADD API 3: Some Registers have default hardware configuration from OTP, we shouldnt overwrite those - add the weak write register permission and use it here (LH)
// Default Register Values
#define R00 0x00000008  // GCONF
#define R09 0x00010606  // SHORT_CONF
#define R0A 0x00080400  // DRV_CONF
#define R10 0x00070A03  // IHOLD_IRUN
#define R11 0x0000000A  // TPOWERDOWN
#define R2B 0x00000001  // VSTOP
#define R3A 0x00010000  // ENC_CONST
#define R60 0xAAAAB554  // MSLUT[0]
#define R61 0x4A9554AA  // MSLUT[1]
#define R62 0x24492929  // MSLUT[2]
#define R63 0x10104222  // MSLUT[3]
#define R64 0xFBFFFFFF  // MSLUT[4]
#define R65 0xB5BB777D  // MSLUT[5]
#define R66 0x49295556  // MSLUT[6]
#define R67 0x00404222  // MSLUT[7]
#define R68 0xFFFF8056  // MSLUTSEL
#define R69 0x00F70000  // MSLUTSTART
#define R6C 0x00410153  // CHOPCONF
#define R70 0xC40C001E  // PWMCONF



/* Register access permissions:
 * 0: none (reserved)
 * 1: read
 * 2: write
 * 3: read/write
 * 7: read^write (seperate functions/values)
 */
const uint8_t tmc5160_defaultRegisterAccess[TMC5160_REGISTER_COUNT] =
{
//	0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
	3, 7, 1, 2, 7, 2, 2, 1, 1, 2, 2, 2, 1, 0, 0, 0, // 0x00 - 0x0F
	2, 2, 1, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x10 - 0x1F
	3, 3, 1, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 3, 0, 0, // 0x20 - 0x2F
	0, 0, 0, 2, 3, 7, 1, 0, 3, 3, 2, 7, 1, 2, 0, 0, // 0x30 - 0x3F
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x40 - 0x4F
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x50 - 0x5F
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 3, 2, 2, 1, // 0x60 - 0x6F
	2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  // 0x70 - 0x7F
};



const int32_t tmc5160_defaultRegisterResetState[TMC5160_REGISTER_COUNT] =
{
//	0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
	R00, 0,   0,   0,   0,   0,   0,   0,   0,   R09, R0A, 0,   0,   0,   0,   0, // 0x00 - 0x0F
	R10, R11, 0,   0,   0,   0,   0,   0,   0,   0,   0,   R2B, 0,   0,   0,   0, // 0x10 - 0x1F
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 0x20 - 0x2F
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   R3A, 0,   0,   0,   0,   0, // 0x30 - 0x3F
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 0x40 - 0x4F
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 0x50 - 0x5F
	R60, R61, R62, R63, R64, R65, R66, R67, R68, R69, 0,   0,   R6C, 0,   0,   0, // 0x60 - 0x6F
	R70, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0  // 0x70 - 0x7F
};









/*
*	Print TMC5160 six point v
*/
void TMC5160_PrintSixPoint_V(TMC_e eTMC)
{
	if(eTMC >= TMC_MODULE_END)
	{
		LOG_Error("unKonwn TMC NUM=%d", eTMC);
		return;
	}
	
	LOG_Info("M=%d, VSTART=%d, A1=%d, V1=%d, AMAX=%d, VMAX=%d, DMAX=%d, D1=%d, VSTOP=%d",
		TMC5160_ReadInt(eTMC, TMC5160_RAMPMODE), \
		TMC5160_ReadInt(eTMC, TMC5160_VSTART), \
		TMC5160_ReadInt(eTMC, TMC5160_A1), \
		TMC5160_ReadInt(eTMC, TMC5160_V1), \
		TMC5160_ReadInt(eTMC, TMC5160_AMAX), \
		TMC5160_ReadInt(eTMC, TMC5160_VMAX), \
		TMC5160_ReadInt(eTMC, TMC5160_DMAX), \
		TMC5160_ReadInt(eTMC, TMC5160_D1), \
		TMC5160_ReadInt(eTMC, TMC5160_VSTOP));     
	
	LOG_Info("XTarget=%d, XActual=%d, XEnc=%d", \
		TMC5160_ReadInt(eTMC, TMC5160_XTARGET),\
		TMC5160_ReadInt(eTMC, TMC5160_XACTUAL),\
		TMC5160_ReadInt(eTMC, TMC5160_XENC));
	
}



///*
//*	写配置
//*/
//void tmc5160_writeConfiguration(TMC_e eTMC)
//{

//	uint8_t *ptr = &TMC5160_config->configIndex;
//	const int32_t *settings = (TMC5160_config->state == CONFIG_RESTORE) ? g_taTMC5160[ptRecvFrame->ucDeviceID].laShadowRegister : tmc5160->registerResetState;

//	while((*ptr < TMC5160_REGISTER_COUNT) && !TMC_IS_WRITABLE(tmc5160->registerAccess[*ptr]))
//		(*ptr)++;

//	if(*ptr < TMC5160_REGISTER_COUNT)
//	{
//		TMC5160_WriteInt(eTMC, *ptr, settings[*ptr]);
//		(*ptr)++;
//	}
//	else
//	{
//		TMC5160_config->state = CONFIG_READY;
//	}
//}




//void tmc5160_periodicJob(uint8_t eTMC, uint32_t tick, TMC5160TypeDef *tmc5160, ConfigurationTypeDef *TMC5160_config)
//{
//	if(TMC5160_config->state != CONFIG_READY)
//	{
//		tmc5160_writeConfiguration(eTMC, tmc5160, TMC5160_config);
//		return;
//	}

//	int XActual;
//	uint32_t tickDiff;

//	if((tickDiff = tick-tmc5160->oldTick) >= 5) // measured speed dx/dt
//	{
//		XActual = TMC5160_ReadInt(eTMC, TMC5160_XACTUAL);
//		g_taTMC5160[ptRecvFrame->ucDeviceID].laShadowRegister[TMC5160_XACTUAL] = XActual;
//		tmc5160->velocity = (int) ((float) ((XActual-tmc5160->oldX) / (float) tickDiff) * (float) 1048.576);

//		tmc5160->oldX     = XActual;
//		tmc5160->oldTick  = tick;
//	}
//}





/*
*	TMC5160 Init
*/
#define  DEFAULT_INIT_HOLD_I_VALUE		8	//上电默认保持电流
void TMC5160_Init(TMC_e eTMC)
{
	extern AxisParamDefault_t g_tAxisParamDefault;
	uint8_t ucIRun = 0, ucIHold = 0, ucMicoStep = 0;
	int32_t lCurrent = 0;
	
	switch(eTMC)
	{
		case TMC_0:
		case TMC_1:
		case TMC_2:
		case TMC_3:
		case TMC_4:
		case TMC_5:
		{	
			//初始化
			g_taTMC5160[eTMC].lOldX = 0;
			g_taTMC5160[eTMC].lVelocity = 0;
			g_taTMC5160[eTMC].ulOldTick = 0;
			for(uint8_t i = 0; i < TMC5160_REGISTER_COUNT; i++)
			{
				g_taTMC5160[eTMC].ucaRegisterAccess[i] = tmc5160_defaultRegisterAccess[i];
				g_taTMC5160[eTMC].laShadowRegister[i] = 0;//tmc5160_defaultRegisterResetState[i];
				
			}
			//寄存器初始化
			//ShadowRegister_Init(&g_tAxisParamDefault);

#if 1
			/* 控制器 */
			//TMC5160_WriteInt(eTMC, TMC5160_CHOPCONF, 0x000100C3); 		//PAGE46:CHOPCONF: TOFF=3（TOFF=0电机关闭扭矩）, HSTRT=4, HEND=1,TBL=2, CHM=0 (spreadcycle)	   
			//TMC5160_WriteInt(eTMC, TMC5160_GCONF ,0x00000014);  		//PAGE27:EN_PWM_MODE=1，使能, shaft电极反相，
			TMC5160_WriteInt(eTMC, TMC5160_GCONF, 0x00000000); //0x00000004; EN_PWM_MODE=1，
			//0x000C0000  0   0xC40C001E
			TMC5160_WriteInt(eTMC, TMC5160_PWMCONF, 0xC40C001E);  //0xC400001E;//pwm_autoscale=0, pwm_autograd=0 =>速度前馈控制 //默认值：0xC40C001E;
			TMC5160_WriteInt(eTMC, TMC5160_CHOPCONF, 0x02410253);
			TMC5160_WriteInt(eTMC, TMC5160_SHORT_CONF, 0x00000306);			
			TMC5160_WriteInt(eTMC, TMC5160_DRV_CONF, 0x0001000A);
#else
			
			/* 控制器 */
			//TMC5160_WriteInt(eTMC, TMC5160_CHOPCONF, 0x000100C3); 		//PAGE46:CHOPCONF: TOFF=3（TOFF=0电机关闭扭矩）, HSTRT=4, HEND=1,TBL=2, CHM=0 (spreadcycle)	   
			//TMC5160_WriteInt(eTMC, TMC5160_GCONF ,0x00000014);  		//PAGE27:EN_PWM_MODE=1，使能, shaft电极反相，
			
			TMC5160_WriteInt(eTMC, TMC5160_GCONF, 0x00000004); //0x00000004; EN_PWM_MODE=1，
			TMC5160_WriteInt(eTMC, TMC5160_CHOPCONF, 0x00410253);
			TMC5160_WriteInt(eTMC, TMC5160_SHORT_CONF, 0x00000306);	
			TMC5160_WriteInt(eTMC, TMC5160_DRV_CONF, 0x0001000A);
			TMC5160_WriteInt(eTMC, TMC5160_RAMPMODE, 0x00000000);
			//0x000C0000  0   0xC40C001E
			TMC5160_WriteInt(eTMC, TMC5160_PWMCONF, 0xC40C001E);  //0xC400001E;//pwm_autoscale=0, pwm_autograd=0 =>速度前馈控制 //默认值：0xC40C001E;
			TMC5160_WriteInt(eTMC, TMC5160_TCOOLTHRS, 0x000FFFFF);
			TMC5160_WriteInt(eTMC, TMC5160_DRV_CONF, 0x0001000A);
			
#endif
			
			/* 编码器 */
			//编码器模式, 十进制模式，AB极性高有效，失能N
			TMC_WriteInt(eTMC, TMC5160_ENCMODE, DEFAULT_ENC_MODE_VALUE); 	//ENCMODE 	
			//编码器计数方向/编码器常数		
			Update_Enc_ConstValue(eTMC, &g_tAxisParamDefault, 1);		
			
			/* 电流 */
			ucIRun = g_tAxisParamDefault.ucIRun[eTMC]/CURRENT_CHANGE_CONST;
			ucIHold = g_tAxisParamDefault.ucIHold[eTMC]/CURRENT_CHANGE_CONST;
			//ucIHold = DEFAULT_INIT_HOLD_I_VALUE/CURRENT_CHANGE_CONST;
			lCurrent = 	(((g_tAxisParamDefault.ucIHoldDelay[eTMC] & 0x1F)<<16) |((ucIRun & 0x1F)<<8)|(ucIHold & 0x1F));
			TMC5160_WriteInt(eTMC, TMC5160_IHOLD_IRUN, lCurrent);    //PAGE33:IHOLD_IRUN: IHOLD=10, IRUN=31(这里的IHOLD和IRUN数值请根据实际电流修改), IHOLDDELAY=6
			//
			TMC5160_WriteInt(eTMC, TMC5160_TPOWERDOWN , 0x0000000A); 	//PAGE33:TPOWERDOWN=10:电机静止到电流减小之间的延时


			/* 步进细分 */
			MicroStep_SetValue2Register(g_tAxisParamDefault.usMicroStepResultion[eTMC], &ucMicoStep);
			TMC5160_FIELD_UPDATE(eTMC, TMC5160_CHOPCONF, TMC5160_MRES_MASK, TMC5160_MRES_SHIFT, ucMicoStep);
			
			/* 六点速度参数 */
			TMC5160_WriteInt(eTMC, TMC5160_VSTART, g_tAxisParamDefault.lVStart[eTMC]*V_CHANGE_CONST);
			TMC5160_WriteInt(eTMC, TMC5160_A1, g_tAxisParamDefault.lA1[eTMC]*A_CHANGE_CONST);      			
			TMC5160_WriteInt(eTMC, TMC5160_V1, g_tAxisParamDefault.lV1[eTMC]*V_CHANGE_CONST);     			
			TMC5160_WriteInt(eTMC, TMC5160_AMAX, g_tAxisParamDefault.lAMax[eTMC]*A_CHANGE_CONST);       			
			TMC5160_WriteInt(eTMC, TMC5160_VMAX, g_tAxisParamDefault.lVMax[eTMC]*V_CHANGE_CONST);   			
			TMC5160_WriteInt(eTMC, TMC5160_DMAX, g_tAxisParamDefault.lDMax[eTMC]*A_CHANGE_CONST);       			
			TMC5160_WriteInt(eTMC, TMC5160_D1, g_tAxisParamDefault.lD1[eTMC]*A_CHANGE_CONST);     				
			TMC5160_WriteInt(eTMC, TMC5160_VSTOP, g_tAxisParamDefault.lVStop[eTMC]*V_CHANGE_CONST);       			  
			
			/* 参考点复位 */
			uint32_t ulSWMode = 0;
			//软复位，设置左右参考点，左触发锁存，极性（0:高有效，1：低有效）。	
			ulSWMode = (0x0800 | 0x0020 | (g_tAxisParamDefault.ucRighLimitPolarity[eTMC] << 3) | (g_tAxisParamDefault.ucLeftLimitPolarity[eTMC] << 2));
			TMC5160_WriteInt(eTMC, TMC5160_SWMODE, ulSWMode); 				
			//电机旋转方向
			TMC5160_FIELD_UPDATE(eTMC, TMC5160_GCONF, TMC5160_SHAFT_MASK, TMC5160_SHAFT_SHIFT, g_tAxisParamDefault.ucRotateDirect[eTMC]);
			
			/* clear XTarget，XActual，XECN */
			TMC5160_WriteInt(eTMC, TMC5160_XTARGET ,0);
			TMC5160_WriteInt(eTMC, TMC5160_XACTUAL ,0);
			TMC5160_WriteInt(eTMC, TMC5160_XENC ,0); 
			//默认位置模式
			TMC5160_WriteInt(eTMC, TMC5160_RAMPMODE, TMC5160_MODE_POSITION); 	
			g_tAxisParamDefault.ucMode[eTMC] = TMC5160_MODE_POSITION;
	
	
/*
			TMC5160_WriteInt(eTMC, TMC5160_CHOPCONF,0x000100C3); 	//PAGE46:CHOPCONF: TOFF=3（TOFF=0电机关闭扭矩）, HSTRT=4, HEND=1,TBL=2, CHM=0 (spreadcycle)
			TMC5160_WriteInt(eTMC, TMC5160_IHOLD_IRUN ,0x00061F0A);  //PAGE33:IHOLD_IRUN: IHOLD=10, IRUN=31(这里的IHOLD和IRUN数值请根据实际电流修改), IHOLDDELAY=6
			//TMC5160_WriteInt(eTMC, TMC5160_IHOLD_IRUN,0x00061403);  //PAGE33:IHOLD_IRUN: IHOLD=10, IRUN=31(这里的IHOLD和IRUN数值请根据实际电流修改), IHOLDDELAY=6
			TMC5160_WriteInt(eTMC, TMC5160_TPOWERDOWN ,0x0000000A);  //PAGE33:TPOWERDOWN=10:电机静止到电流减小之间的延时
			TMC5160_WriteInt(eTMC, TMC5160_GCONF ,0x00000004);  		//PAGE27:EN_PWM_MODE=1，使能
			TMC5160_WriteInt(eTMC, TMC5160_PWMCONF ,0x000C0000);  	//PAGE43:PWMCONF
			TMC5160_WriteInt(eTMC, TMC5160_TPWMTHRS ,0x000001F4);  	//PAGE33:TPWM_THRS=500,对应切换速度35000=ca.30RPM
			TMC5160_WriteInt(eTMC, TMC5160_A1 ,10000);      			//PAGE35:A1=1000第一阶段加速度
			TMC5160_WriteInt(eTMC, TMC5160_V1 ,500000);     			//PAGE35:V1=50000加速度阀值速度V1
			TMC5160_WriteInt(eTMC, TMC5160_AMAX ,5000);       		//PAGE35:AMAX=500大于V1的加速度
			TMC5160_WriteInt(eTMC, TMC5160_VMAX ,20000000);   		//PAGE35:VMAX=200000
		    TMC5160_WriteInt(eTMC, TMC5160_DMAX ,7000);       		//PAGE35:DMAX=700大于V1的减速度
		    TMC5160_WriteInt(eTMC, TMC5160_D1 ,14000);     			//PAGE35:D1=1400小于V1的减速度
		    TMC5160_WriteInt(eTMC, TMC5160_VSTOP ,100);       		//PAGE35:VSTOP=10停止速度，接近于0
		    TMC5160_WriteInt(eTMC, TMC5160_RAMPMODE ,0);         	//PAGE35:RAMPMODE=0位置模式，使用所有A、V、D参数

*/

		}
		break;
		default:break;
	}
}




/*
*	电流初始化
*/
void TMC5160_I_Set(TMC_e eTMC)
{
	extern AxisParamDefault_t g_tAxisParamDefault;
	uint8_t ucIRun = 0, ucIHold = 0;
	uint32_t lCurrent = 0;
	
	/* 电流 */
	ucIRun = g_tAxisParamDefault.ucIRun[eTMC]/CURRENT_CHANGE_CONST;
	ucIHold = g_tAxisParamDefault.ucIHold[eTMC]/CURRENT_CHANGE_CONST;
	lCurrent = 	(((g_tAxisParamDefault.ucIHoldDelay[eTMC] & 0x1F)<<16) |((ucIRun & 0x1F)<<8)|(ucIHold & 0x1F));
	TMC5160_WriteInt(eTMC, TMC5160_IHOLD_IRUN, lCurrent);    //PAGE33:IHOLD_IRUN: IHOLD=10, IRUN=31(这里的IHOLD和IRUN数值请根据实际电流修改), IHOLDDELAY=6
	//
	TMC5160_WriteInt(eTMC, TMC5160_TPOWERDOWN , 0x0000000A); 	//PAGE33:TPOWERDOWN=10:电机静止到电流减小之间的延时

}



/*
*	检查输入的寄存器地址，是否存在
*/
ErrorType_e CheckRegister_Addr(uint8_t ucAddr)
{
	switch(ucAddr)
	{
		case TMC5160_GCONF:
		case TMC5160_GSTAT:
		case TMC5160_IFCNT:
		case TMC5160_SLAVECONF:
		case TMC5160_INP_OUT:			
		case TMC5160_X_COMPARE:
		case TMC5160_OTP_PROG:
		case TMC5160_OTP_READ:			
		case TMC5160_FACTORY_CONF:
		case TMC5160_SHORT_CONF:
		case TMC5160_DRV_CONF:			
		case TMC5160_GLOBAL_SCALER:
		case TMC5160_OFFSET_READ:
		case TMC5160_IHOLD_IRUN:			
		case TMC5160_TPOWERDOWN:
		case TMC5160_TSTEP:
		case TMC5160_TPWMTHRS:			
		case TMC5160_TCOOLTHRS:
		case TMC5160_THIGH:
		case TMC5160_RAMPMODE:			
		case TMC5160_XACTUAL:
		case TMC5160_VACTUAL:
		case TMC5160_VSTART:			
		case TMC5160_A1:
		case TMC5160_V1:
		case TMC5160_AMAX:			
		case TMC5160_VMAX:
		case TMC5160_DMAX:
		case TMC5160_D1:			
		case TMC5160_VSTOP:
		case TMC5160_TZEROWAIT:
		case TMC5160_XTARGET:			
		case TMC5160_VDCMIN:
		case TMC5160_SWMODE:
		case TMC5160_RAMPSTAT:			
		case TMC5160_XLATCH:
		case TMC5160_ENCMODE:
		case TMC5160_XENC:			
		case TMC5160_ENC_CONST:
		case TMC5160_ENC_STATUS:
		case TMC5160_ENC_LATCH:			
		case TMC5160_ENC_DEVIATION:
		case TMC5160_MSLUT0:
		case TMC5160_MSLUT1:			
		case TMC5160_MSLUT2:
		case TMC5160_MSLUT3:
		case TMC5160_MSLUT4:			
		case TMC5160_MSLUT5:
		case TMC5160_MSLUT6:
		case TMC5160_MSLUT7:			
		case TMC5160_MSLUTSEL:
		case TMC5160_MSLUTSTART:
		case TMC5160_MSCNT:			
		case TMC5160_MSCURACT:
		case TMC5160_CHOPCONF:
		case TMC5160_COOLCONF:			
		case TMC5160_DCCTRL:
		case TMC5160_DRVSTATUS:
		case TMC5160_PWMCONF:			
		case TMC5160_PWMSCALE:
		case TMC5160_PWM_AUTO:
		case TMC5160_LOST_STEPS:	
		{
			return ERROR_TYPE_SUCCESS;
		}
//		break;
		default:
		{
			return ERROR_TYPE_DATA;
		}
//		break;
		
	}


}


//#define RESET_HIGH_SPEED 		51200*5 //10	//复位高速，10圈每秒
//#define RESET_LOW_SPEED 		51200*2			//复位低速，2圈每秒
//#define RESET_FIXED_DISTANCE	25600			//复位后退，固定距离
//
#define RESET_HIGH_AMAX			51200*2			//复位，加速度（高）
#define RESET_LOW_AMAX			51200			//复位，加速度（低）



/*
*	1、高速回零,到速度为0,(光耦触发后，急停-需选择适当的方式)
*	2、低速反向回退，离开光耦后，走固定距离
*	3、再低速回零，等待速度将为0时，触发光耦记录位置P0，记录位置P1，计算D=P1-P0;
*	4、根据D-偏差，校准零位
*
*/

/*
 *  Reset the TMC5160.
 */
 #define MOTOR_RESET_TIMTOUT	600000
ErrorType_e TMC5160_Reset(TMC_e eTMC, TMC_Ref_e eRef)
{

//	extern GlobalParam_t g_tGlobalParam;
//	extern AxisParamDefault_t g_tAxisParamDefault;
//	//extern BoardStatus_t g_tBoardStatus;
//	
	ErrorType_e eError = ERROR_TYPE_SUCCESS;
//	uint16_t i = 0, j = 0, ucFlag = 0;
//	int32_t lSpeed = 0, lXActual = 0, ulCurPos = 0, lXLatch = 0, lDiff = 0, lStatus = 0;
////	int32_t lEnc = 0;
//	
//	uint32_t ulTD = 0, ulT1 = 0, ulT2 = 0, ulT3 = 0;
//	uint8_t ucOC_Status = 0;
//	
//	int32_t lVstart = 0, lA1 = 0, lV1 = 0, lAMax = 0, lVMax = 0, lDMax = 0, lD1 = 0, lVStop = 0;
//	//备份六点速度值
//	lVstart = g_taTMC5160[eTMC].laShadowRegister[TMC5160_VSTART];
//	lA1 = g_taTMC5160[eTMC].laShadowRegister[TMC5160_A1];
//	lV1 = g_taTMC5160[eTMC].laShadowRegister[TMC5160_V1];
//	lAMax = g_taTMC5160[eTMC].laShadowRegister[TMC5160_AMAX];
//	lVMax = g_taTMC5160[eTMC].laShadowRegister[TMC5160_VMAX];
//	lDMax = g_taTMC5160[eTMC].laShadowRegister[TMC5160_DMAX];
//	lD1 = g_taTMC5160[eTMC].laShadowRegister[TMC5160_D1];
//	lVStop = g_taTMC5160[eTMC].laShadowRegister[TMC5160_VSTOP];

//	//速度为0
//	TMC5160_WriteInt(eTMC, TMC5160_VMAX, 0);
//	//清零复位模式
//	TMC5160_WriteInt(eTMC, TMC5160_SWMODE, 0);

//		
//	ulT1 = HAL_GetTick();
//	//状态检测
//	ucFlag = 0;
//	///lStatus = TMC5160_ReadInt(eTMC, TMC5160_RAMPSTAT);
//	ucOC_Status = TMC5160_FIELD_READ(eTMC, TMC5160_RAMPSTAT, TMC5160_RAMPSTAT_LEFT_SW_TRIGGER_MASK, TMC5160_RAMPSTAT_LEFT_SW_TRIGGER_SHIFT);
//	if(eTMC == TMC_2)
//	{
//		if(ucOC_Status == 1)
//		{
//		   //光耦无触发
//			
//			/*  1、高速回零,光耦触发后，停止 */
//			//高速回零
//			// Set VMAX and direction, let's Rotate， 
//			TMC5160_WriteInt(eTMC, TMC5160_VMAX, 0); 
//			TMC5160_WriteInt(eTMC, TMC5160_VMAX, g_tAxisParamDefault.lResetSpeedHigh[eTMC]);  		// RESET_LOW_SPEED  RESET_HIGH_SPEED
//			TMC5160_WriteInt(eTMC, TMC5160_AMAX ,RESET_LOW_AMAX); 					//change AMax, so than  stop quickly
//			TMC5160_WriteInt(eTMC, TMC5160_RAMPMODE, TMC5160_MODE_VELNEG);  //TMC5160_MODE_VELPOS   TMC5160_MODE_VELNEG
//			//HAL_Delay(800);
//			LOG_Info("1, High Speed To ZERO");
//					
//			for(i = 0; i < MOTOR_RESET_TIMTOUT; i++)
//			{
//				//状态检测
//				//lStatus = TMC5160_ReadInt(eTMC, TMC5160_RAMPSTAT);
//				ucOC_Status = TMC5160_FIELD_READ(eTMC, TMC5160_RAMPSTAT, TMC5160_RAMPSTAT_LEFT_SW_TRIGGER_MASK, TMC5160_RAMPSTAT_LEFT_SW_TRIGGER_SHIFT);
//				if(ucOC_Status == 0)
//				{
//					//光耦触发
//					ulT2 = HAL_GetTick();
//					ulTD = ulT2 - ulT1;
//					LOG_Info("OC Triggle DT12=%d", ulTD);
//					//快速减速到零
//					TMC5160_WriteInt(eTMC, TMC5160_AMAX ,RESET_HIGH_AMAX); /////////change AMax, so than  stop quickly
//					TMC5160_WriteInt(eTMC, TMC5160_VMAX, 0); 
//					//wait Speed to 0
//					for(i = 0; i < 15000; i++)
//					{						
//						lSpeed = TMC5160_ReadInt(eTMC, TMC5160_VACTUAL);
//						if(0 == lSpeed){
//							ulT2 = HAL_GetTick();
//							ulTD = ulT2 - ulT1;
//							
//							ucFlag = 1;
//							LOG_Info("VMAX Reach 0 DT12=%d", ulTD);
//							LOG_Info("1, High Speed To ZERO Finished");
//							//stop, set to hold mode(velocity is not change)
//							//TMC5160_WriteInt(TMC_0, TMC5160_RAMPMODE, TMC5160_MODE_HOLD);
//							break;
//						}
//						TMC_Delay(1);
//					 }
//				  }
//				  TMC_Delay(1);
//				  if(ucFlag == 1) break;					
//			 }
//		 }
//	 }else{
//	 	if(ucOC_Status == 0)
//		{
//		   //光耦无触发
//			
//			/*  1、高速回零,光耦触发后，停止 */
//			//高速回零
//			// Set VMAX and direction, let's Rotate， 
//			TMC5160_WriteInt(eTMC, TMC5160_VMAX, 0); 
//			TMC5160_WriteInt(eTMC, TMC5160_VMAX, g_tAxisParamDefault.lResetSpeedHigh[eTMC]);  		// RESET_LOW_SPEED  RESET_HIGH_SPEED
//			TMC5160_WriteInt(eTMC, TMC5160_AMAX ,RESET_LOW_AMAX); 					//change AMax, so than  stop quickly
//			TMC5160_WriteInt(eTMC, TMC5160_RAMPMODE, TMC5160_MODE_VELNEG);  //TMC5160_MODE_VELPOS   TMC5160_MODE_VELNEG
//			//HAL_Delay(800);
//			LOG_Info("1, High Speed To ZERO");
//					
//			for(i = 0; i < MOTOR_RESET_TIMTOUT; i++)
//			{
//				//状态检测
//				lStatus = TMC5160_ReadInt(eTMC, TMC5160_RAMPSTAT);
//				if((lStatus & 0x00000001) == 1)
//				{
//					//光耦触发
//					ulT2 = HAL_GetTick();
//					ulTD = ulT2 - ulT1;
//					LOG_Info("OC Triggle DT12=%d", ulTD);
//					//快速减速到零
//					TMC5160_WriteInt(eTMC, TMC5160_AMAX ,RESET_HIGH_AMAX); /////////change AMax, so than  stop quickly
//					TMC5160_WriteInt(eTMC, TMC5160_VMAX, 0); 
//					//wait Speed to 0
//					for(i = 0; i < 15000; i++)
//					{						
//						lSpeed = TMC5160_ReadInt(eTMC, TMC5160_VACTUAL);
//						if(0 == lSpeed){
//							ulT2 = HAL_GetTick();
//							ulTD = ulT2 - ulT1;
//							
//							ucFlag = 1;
//							LOG_Info("VMAX Reach 0 DT12=%d", ulTD);
//							LOG_Info("1, High Speed To ZERO Finished");
//							//stop, set to hold mode(velocity is not change)
//							//TMC5160_WriteInt(TMC_0, TMC5160_RAMPMODE, TMC5160_MODE_HOLD);
//							break;
//						}
//						TMC_Delay(1);
//					 }
//				  }
//				  TMC_Delay(1);
//				  if(ucFlag == 1) break;					
//			 }
//		 }
//	 }
//	ulT3 = HAL_GetTick();
//	ulTD = ulT3 - ulT1;
//	LOG_Info("DT23=%d", ulTD);
//	
//	
//	//set SW_MODE, soft stop, left reference, high valid,
////	TMC5160_WriteInt(eTMC, TMC5160_SWMODE, 0x0821);
//	//2、低速反向回退，检测到离开光耦后，走固定距离	//检测光耦触发（有效）
//	//低速反转，离开光耦
//	HAL_Delay(50); ////
//	TMC5160_WriteInt(eTMC, TMC5160_VMAX, g_tAxisParamDefault.lResetSpeedLow[eTMC]);  		// RESET_LOW_SPEED  RESET_HIGH_SPEED
//	TMC5160_WriteInt(eTMC, TMC5160_AMAX ,RESET_LOW_AMAX); //
//	TMC5160_WriteInt(eTMC, TMC5160_RAMPMODE, TMC5160_MODE_VELPOS);  //TMC5160_MODE_VELPOS   TMC5160_MODE_VELNEG
//	ucFlag = 0;
//	LOG_Info("2, Low Speed To Fixed Postions, Start");
//	for(i = 0; i < 15000; i++)
//	{
//		if(eTMC == TMC_2)
//		{
//			lStatus = TMC5160_ReadInt(eTMC, TMC5160_RAMPSTAT);
//			if((lStatus & 0x00000001) == 1)
//			{ //光耦未触发
//				
//				//走固定位置
//				ulCurPos = TMC5160_ReadInt(eTMC, TMC5160_XACTUAL);
//				for(j = 0; j < 15000; j++)
//				{
//					lXActual = TMC5160_ReadInt(eTMC, TMC5160_XACTUAL);
//					if(ulCurPos + g_tAxisParamDefault.lResetOff[eTMC] <= lXActual) 
//					{
//						//位置到达
//						ucFlag = 1;
//						TMC5160_WriteInt(eTMC, TMC5160_VMAX, 0); //停止
//						//wait Speed to 0
//						for(uint32_t n = 0; n < 15000; n++)
//						{
//							
//							lSpeed = TMC5160_ReadInt(eTMC, TMC5160_VACTUAL);
//							if(0 == lSpeed)
//							{
//								LOG_Info("2, Low Speed To Fixed Pos Finish");
//								//stop, set to hold mode(velocity is not change)
//								//TMC5160_WriteInt(TMC_0, TMC5160_RAMPMODE, TMC5160_MODE_HOLD);
//								TMC_Delay(50);///////
//								break;
//							}
//							TMC_Delay(1);
//						}
//					}
//					TMC_Delay(1);
//					if(ucFlag == 1) break;	
//				}
//			}
//			if(ucFlag == 1) break;
//			
//		}else{
//	
//			lStatus = TMC5160_ReadInt(eTMC, TMC5160_RAMPSTAT);
//			if((lStatus & 0x00000001) == 0)
//			{ //光耦未触发
//				
//				//走固定位置
//				ulCurPos = TMC5160_ReadInt(eTMC, TMC5160_XACTUAL);
//				for(j = 0; j < 15000; j++)
//				{
//					lXActual = TMC5160_ReadInt(eTMC, TMC5160_XACTUAL);
//					if(ulCurPos + g_tAxisParamDefault.lResetOff[eTMC] <= lXActual) 
//					{
//						//位置到达
//						ucFlag = 1;
// 						TMC5160_WriteInt(eTMC, TMC5160_VMAX, 0); //停止
//						//wait Speed to 0
//						for(uint32_t n = 0; n < 15000; n++)
//						{							
//							lSpeed = TMC5160_ReadInt(eTMC, TMC5160_VACTUAL);
//							if(0 == lSpeed)
//							{
//								LOG_Info("2, Low Speed To Fixed Pos Finish");
//								//stop, set to hold mode(velocity is not change)
//								//TMC5160_WriteInt(TMC_0, TMC5160_RAMPMODE, TMC5160_MODE_HOLD);
//								TMC_Delay(50);///////
//								break;
//							}
//							TMC_Delay(1);
//						}
//					}
//					TMC_Delay(1);
//					if(ucFlag == 1) break;	
//				}
//			}
//			if(ucFlag == 1) break;
//		}
//	}
//	if(i >= 15000) return 1; //复位最大时间15S, fail
//	//TMC_Delay(1000);
//	LOG_Info("2, Low Speed To Fixed Postions, End");
//	
//	
//	
//	/* 设置复位模式，为左参考点，软复位，高有效  */
//	lXActual = TMC5160_ReadInt(eTMC, TMC5160_XACTUAL);
//	if(TMC_REF_LEFT == eRef)
//	{
//		if(eTMC == TMC_2)
//		{
//			//set SW_MODE, soft stop, left reference, low valid,
//			TMC5160_WriteInt(eTMC, TMC5160_SWMODE, 0x0825); //模块三：光耦触发时0，低有效。 模块1、2：触发时为1，高有效。
//		}else{
//			TMC5160_WriteInt(eTMC, TMC5160_SWMODE, 0x0821);
//		}
//		
//		//set SW_MODE, hard stop, left reference, high valid,
//		//TMC5160_WriteInt(eTMC, TMC5160_SWMODE, 0x0021);		
//		
//	}else if(TMC_REF_RIGHT == eRef){
//		//set SW_MODE, soft stop, right reference, high valid,
//		//TMC5160_WriteInt(eTMC, TMC5160_SWMODE, 0x0882);
//	}
//	
//	
//	/* 3、 再低速回零，等待速度将为0时，触发光耦记录位置P0，记录位置P1，计算D=P1-P0; */
//	//低速回零
//	// Set VMAX and direction, let's Rotate， 
//	HAL_Delay(50); ///////
//	TMC5160_WriteInt(eTMC, TMC5160_VMAX, g_tAxisParamDefault.lResetSpeedLow[eTMC]);  		// RESET_LOW_SPEED  RESET_HIGH_SPEED
//	TMC5160_WriteInt(eTMC, TMC5160_RAMPMODE, TMC5160_MODE_VELNEG);  //TMC5160_MODE_VELPOS   TMC5160_MODE_VELNEG
//	LOG_Info("3, Low Speed To ZERO");
//	//wait Speed to 0
//	for(i = 0; i < 15000; i++)
//	{		
//		lSpeed = TMC5160_ReadInt(eTMC, TMC5160_VACTUAL);
//		if(0 == lSpeed){
//			//stop, set to hold mode(velocity is not change)
//			TMC5160_WriteInt(eTMC, TMC5160_RAMPMODE, TMC5160_MODE_HOLD);
//			LOG_Info("3, Low Speed To ZERO Finished");
//			break;
//		}
//		TMC_Delay(1);
//	}
//	if(i >= 15000) return 1; //复位最大时间15S, fail
//	TMC_Delay(50);/////////
//	

//	//4
//	//触发光耦记录位置P0, 记录位置P1
//	//get XACTUAL plValue
//	lXActual = TMC5160_ReadInt(eTMC, TMC5160_XACTUAL);
//	lXActual = TMC5160_ReadInt(eTMC, TMC5160_XACTUAL);
//	lXActual = TMC5160_ReadInt(eTMC, TMC5160_XACTUAL);
//	//get XLatch
//	lXLatch = TMC5160_ReadInt(eTMC, TMC5160_XLATCH);
//	
//	
//	//set XACTUAL
//	lDiff = lXActual - lXLatch;
////	TMC5160_WriteInt(eTMC, TMC5160_XACTUAL, lXActual);
////	TMC5160_ReadInt(eTMC, TMC5160_XTARGET, &lDiff);
////	TMC5160_ReadInt(eTMC, TMC5160_XTARGET, &lDiff);
////	TMC5160_ReadInt(eTMC, TMC5160_XTARGET, &lDiff);

//	
//	/*  根据D-偏差，校准零位  */
//	//clear to 0
//	HAL_Delay(50);  ////////
//	TMC5160_WriteInt(eTMC, TMC5160_XACTUAL, lDiff);
//	TMC5160_WriteInt(eTMC, TMC5160_XTARGET, 0);
//	LOG_Info("4, To ZERO");
//	//set to position mode
//	
//	//base on 0, move Diff step
//	ucFlag = 0;
//	TMC5160_WriteInt(eTMC, TMC5160_RAMPMODE, TMC5160_MODE_POSITION);
//	TMC_SetPMode_V(eTMC, 1);
//	for(j = 0; j < 15000; j++)
//	{
////		if(eTMC == TMC_2)
////		{
////			lStatus = TMC5160_ReadInt(eTMC, TMC5160_RAMPSTAT);
////			if(((lStatus >> 9) & 0x00000001) == 0)
////			{ //位置到达
////				ucFlag = 1;
////				LOG_Info("4, To ZERO Finished");
////				break;
////			}
////			
////		}else{
//			lStatus = TMC5160_ReadInt(eTMC, TMC5160_RAMPSTAT);
//			if(((lStatus >> 9) & 0x00000001) == 1)
//			{ //位置到达
//				ucFlag = 1;
//				LOG_Info("4, To ZERO Finished");
//				break;
//			}
////		}
//		TMC_Delay(1);
//	}
//	
//	//clear to 0, again
//	TMC5160_WriteInt(eTMC, TMC5160_XENC, 0);
//	//
//	lDiff = TMC5160_ReadInt(eTMC, TMC5160_XACTUAL);
//	lDiff = TMC5160_ReadInt(eTMC, TMC5160_XTARGET);
//	lDiff = TMC5160_ReadInt(eTMC, TMC5160_XENC);
//	
////	TMC5160_WriteInt(eTMC, TMC5160_XTARGET, 0);
////	TMC5160_ReadInt(eTMC, TMC5160_XACTUAL, &lXActual);
////	TMC_SetPMode_V(eTMC, 1);
////	TMC_SetPMode_V(eTMC, 1);

//	//恢复六点速度值
//	TMC5160_WriteInt(eTMC, TMC5160_VSTART , lVstart);      			
//	TMC5160_WriteInt(eTMC, TMC5160_A1 , lA1);      			
//	TMC5160_WriteInt(eTMC, TMC5160_V1 , lV1);     			
//	TMC5160_WriteInt(eTMC, TMC5160_AMAX , lAMax);       		
//	TMC5160_WriteInt(eTMC, TMC5160_VMAX , lVMax);   
//	TMC5160_WriteInt(eTMC, TMC5160_DMAX , lDMax);         
//	TMC5160_WriteInt(eTMC, TMC5160_D1 , lD1);
//	TMC5160_WriteInt(eTMC, TMC5160_VSTOP , lVStop);       


	return eError;
}






/*
uint8_t tmc5160_reset(TMC5160TypeDef *tmc5160)
{
	if(tmc5160->config->state != CONFIG_READY)
		return false;

	// Reset the dirty bits and wipe the shadow registers
	size_t i;
	for(i = 0; i < TMC5160_REGISTER_COUNT; i++)
	{
		tmc5160->registerAccess[i] &= ~TMC_ACCESS_DIRTY;
		tmc5160->config->shadowRegister[i] = 0;
	}

	tmc5160->config->state        = CONFIG_RESET;
	tmc5160->config->configIndex  = 0;

	return true;
}
*/

// Restore the TMC5160 to the state stored in the shadow registers.
// This can be used to recover the IC configuration after a VM power loss.
/*
uint8_t tmc5160_restore(TMC5160TypeDef *tmc5160)
{
	if(tmc5160->config->state != CONFIG_READY)
		return false;

	tmc5160->config->state        = CONFIG_RESTORE;
	tmc5160->config->configIndex  = 0;

	return true;
}
*/



// Writes (x1 << 24) | (x2 << 16) | (x3 << 8) | x4 to the given address
//void TMC5160_WriteDataGram(TMC5160TypeDef *tmc5160, uint8_t address, uint8_t x1, uint8_t x2, uint8_t x3, uint8_t x4)
uint8_t TMC5160_WriteDataGram(TMC_e eTMC, uint8_t ucAddr, uint8_t Byte1, uint8_t Byte2, uint8_t Byte3, uint8_t Byte4)
{
	uint8_t ucaData[5] = { ucAddr | TMC5160_WRITE_BIT, Byte1, Byte2, Byte3, Byte4 };
	TMC_SPI_ReadWriteArr(eTMC, ucaData, 5);

	//status
	return ucaData[0];

/*
	uint8_t data[5] = { address | TMC5160_WRITE_BIT, x1, x2, x3, x4 };
	tmc5160_readWriteArray(tmc5160->config->channel, &data[0], 5);


	int32_t plValue = ((uint32_t)x1 << 24) | ((uint32_t)x2 << 16) | (x3 << 8) | x4;

	// Write to the shadow register and mark the register dirty
	address = TMC_ADDRESS(address);
	tmc5160->config->shadowRegister[address] = plValue;
	tmc5160->registerAccess[address] |= TMC_ACCESS_DIRTY;
*/
}


// Write an integer to the given address
//void TMC5160_writeInt(TMC5160TypeDef *tmc5160, uint8_t address, int32_t plValue)
ErrorType_e TMC5160_WriteInt(TMC_e eTMC, uint8_t ucAddr, int32_t lValue)
{
	//tmc5160_writeDatagram(tmc5160, address, BYTE(plValue, 3), BYTE(plValue, 2), BYTE(plValue, 1), BYTE(plValue, 0));
	ErrorType_e eErrorType = ERROR_TYPE_SUCCESS;
	int32_t lTempValue = 0;
	uint8_t ucNum = 0;
	
	//check write right
	if(!TMC_IS_WRITABLE(g_taTMC5160[eTMC].ucaRegisterAccess[ucAddr]))
	{
		LOG_Warn("Register=%d Have Not Wright Right", ucAddr);
		return ERROR_TYPE_SUCCESS;
	}
	g_taTMC5160[eTMC].laShadowRegister[ucAddr] = lValue;
	
	//写及校验
	do{
		TMC5160_WriteDataGram(eTMC, ucAddr, BYTE(lValue, 3), BYTE(lValue, 2), BYTE(lValue, 1), BYTE(lValue, 0));
		lTempValue = TMC5160_ReadInt(eTMC, ucAddr);
		if(lTempValue == lValue)
		{
			break;
		}else{
			Delay_US(100);
			ucNum++;
		}
	
	}while(ucNum < 3);
	
	//
	if(ucNum != 0)
	{
		LOG_Warn("TMC5160_Write Failure %d Time", ucNum);
		if(ucNum == 3)
		{
			LOG_Error("TMC5160_Write Failure %d Time", ucNum);
			return ERROR_TYPE_TMC_RW;
		}
	}
	
	return eErrorType;
}



// Read an integer from the given address
//int32_t TMC5160_ReadInt(TMC5160TypeDef *tmc5160, uint8_t address)
int32_t TMC5160_ReadInt(TMC_e eTMC, uint8_t ucAddr)
{
	uint8_t data[5] = { 0, 0, 0, 0, 0 };
	ucAddr = TMC_ADDRESS(ucAddr);

	//check read right
	if(!TMC_IS_READABLE(g_taTMC5160[eTMC].ucaRegisterAccess[ucAddr]))
	{
		return g_taTMC5160[eTMC].laShadowRegister[ucAddr];
		
	}
			
	//read twice
	data[0] = ucAddr;
	TMC_SPI_ReadWriteArr(eTMC, &data[0], 5);

	data[0] = ucAddr;
	TMC_SPI_ReadWriteArr(eTMC, &data[0], 5);

	return (((uint32_t)data[1] << 24) | ((uint32_t)data[2] << 16) | (data[3] << 8) | data[4]);

	 //status
	 ///return data[0];
}

// Initialize a TMC5160 IC.
// This function requires:
//     - tmc5160: The pointer to a TMC5160TypeDef struct, which represents one IC
//     - channel: The channel index, which will be sent back in the SPI callback
//     - config: A ConfigurationTypeDef struct, which will be used by the IC
//     - registerResetState: An int32_t array with 128 elements. This holds the values to be used for a reset.
/*
void tmc5160_init(TMC5160TypeDef *tmc5160, uint8_t channel, ConfigurationTypeDef *config, const int32_t *registerResetState)
{
	tmc5160->velocity  = 0;
	tmc5160->oldTick   = 0;
	tmc5160->oldX      = 0;

	tmc5160->config               = config;
	tmc5160->config->callback     = NULL;
	tmc5160->config->channel      = channel;
	tmc5160->config->configIndex  = 0;
	tmc5160->config->state        = CONFIG_READY;

	size_t i;
	for(i = 0; i < TMC5160_REGISTER_COUNT; i++)
	{
		tmc5160->registerAccess[i]      = tmc5160_defaultRegisterAccess[i];
		tmc5160->registerResetState[i]  = registerResetState[i];
	}
}
*/

// Fill the shadow registers of hardware preset non-readable registers
// Only needed if you want to 'read' those registers e.g to display the plValue
// in the TMCL IDE register browser
/*
void tmc5160_fillShadowRegisters(TMC5160TypeDef *tmc5160)
{
	// Check if we have constants defined
	if(ARRAY_SIZE(tmc5160_RegisterConstants) == 0)
		return;

	size_t i, j;
	for(i = 0, j = 0; i < TMC5160_REGISTER_COUNT; i++)
	{
		// We only need to worry about hardware preset, write-only registers
		// that have not yet been written (no dirty bit) here.
		if(tmc5160->registerAccess[i] != TMC_ACCESS_W_PRESET)
			continue;

		// Search the constant list for the current address. With the constant
		// list being sorted in ascended order, we can walk through the list
		// until the entry with an address equal or greater than i
		while(j < ARRAY_SIZE(tmc5160_RegisterConstants) && (tmc5160_RegisterConstants[j].address < i))
			j++;

		// Abort when we reach the end of the constant list
		if (j == ARRAY_SIZE(tmc5160_RegisterConstants))
			break;

		// If we have an entry for our current address, write the constant
		if(tmc5160_RegisterConstants[j].address == i)
		{
			tmc5160->config->shadowRegister[i] = tmc5160_RegisterConstants[j].plValue;
		}
	}
}
*/



// Change the values the IC will be configured with when performing a reset.
/*
void tmc5160_setRegisterResetState(TMC5160TypeDef *tmc5160, const int32_t *resetState)
{
	size_t i;
	for(i = 0; i < TMC5160_REGISTER_COUNT; i++)
	{
		tmc5160->registerResetState[i] = resetState[i];
	}
}
*/

/*
// Register a function to be called after completion of the configuration mechanism
void tmc5160_setCallback(TMC5160TypeDef *tmc5160, tmc5160_callback callback)
{
	tmc5160->config->callback = (tmc_callback_config) callback;
}
*/


// Helper function: Configure the next register.
/*
static void writeConfiguration(TMC5160TypeDef *tmc5160)
{
	uint8_t *ptr = &tmc5160->config->configIndex;
	const int32_t *settings;

	if(tmc5160->config->state == CONFIG_RESTORE)
	{
		settings = tmc5160->config->shadowRegister;
		// Find the next restorable register
		while((*ptr < TMC5160_REGISTER_COUNT) && !TMC_IS_RESTORABLE(tmc5160->registerAccess[*ptr]))
		{
			(*ptr)++;
		}
	}
	else
	{
		settings = tmc5160->registerResetState;
		// Find the next resettable register
		while((*ptr < TMC5160_REGISTER_COUNT) && !TMC_IS_RESETTABLE(tmc5160->registerAccess[*ptr]))
		{
			(*ptr)++;
		}
	}

	if(*ptr < TMC5160_REGISTER_COUNT)
	{
		TMC5160_WriteInt(tmc5160, *ptr, settings[*ptr]);
		(*ptr)++;
	}
	else // Finished configuration
	{
		if(tmc5160->config->callback)
		{
			((tmc5160_callback)tmc5160->config->callback)(tmc5160, tmc5160->config->state);
		}

		tmc5160->config->state = CONFIG_READY;
	}
}
*/


// Call this periodically
/*
void tmc5160_periodicJob(TMC5160TypeDef *tmc5160, uint32_t tick)
{
	if(tmc5160->config->state != CONFIG_READY)
	{
		writeConfiguration(tmc5160);
		return;
	}

	int32_t XActual;
	uint32_t tickDiff;

	// Calculate velocity v = dx/dt
	if((tickDiff = tick - tmc5160->oldTick) >= 5)
	{
		XActual = TMC5160_ReadInt(tmc5160, TMC5160_XACTUAL);
		// ToDo CHECK 2: API Compatibility - write alternative algorithm w/o floating point? (LH)
		tmc5160->velocity = (uint32_t) ((float32_t) ((XActual - tmc5160->oldX) / (float32_t) tickDiff) * (float32_t) 1048.576);

		tmc5160->oldX     = XActual;
		tmc5160->oldTick  = tick;
	}
}
*/

// Rotate with a given velocity (to the right)
//void tmc5160_rotate(TMC_e eTMC, int32_t velocity)
ErrorType_e TMC5160_Rotate(TMC_e eTMC, RotateDirect_e eDrect, int32_t lVelocity)
{
	extern AxisParamDefault_t g_tAxisParamDefault;
	ErrorType_e eError = ERROR_TYPE_SUCCESS;
	
	if(eDrect == ROTATE_DIRECT_CLOCKWISE)
	{
		eError = TMC5160_WriteInt(eTMC, TMC5160_VMAX, abs(lVelocity)*V_CHANGE_CONST);
		g_tAxisParamDefault.lVMax_VMode[eTMC] = abs(lVelocity);
		// Set direction
		eError = TMC5160_WriteInt(eTMC, TMC5160_RAMPMODE, TMC5160_MODE_VELPOS);
		g_tAxisParamDefault.ucMode[eTMC] = TMC5160_MODE_VELPOS;
	
	}else{
		eError = TMC5160_WriteInt(eTMC, TMC5160_VMAX, abs(lVelocity)*V_CHANGE_CONST);
		g_tAxisParamDefault.lVMax_VMode[eTMC] = abs(lVelocity);
		// Set direction
		eError = TMC5160_WriteInt(eTMC, TMC5160_RAMPMODE, TMC5160_MODE_VELNEG);
		g_tAxisParamDefault.ucMode[eTMC] = TMC5160_MODE_VELNEG;
	
	}
	return eError;
}



// Stop moving
//void tmc5160_stop(TMC5160TypeDef *tmc5160)
ErrorType_e TMC5160_Stop(TMC_e eTMC)
{
	uint32_t ulXActual = 0;
	ErrorType_e eError = ERROR_TYPE_SUCCESS;
	uint32_t ulWaitMaxTick = 0;
	
	//V=0
	eError = TMC5160_WriteInt(eTMC, TMC5160_VMAX, 0);

	 //wait speed to 0, max wait time 1000ms
	 while(ulWaitMaxTick < 1000)
	 {
		if(0 == TMC5160_ReadInt(eTMC, TMC5160_VACTUAL)) break;
		TMC_Delay(1);
		ulWaitMaxTick++;
	 }
	 
	//XActual
	ulXActual = TMC5160_ReadInt(eTMC, TMC5160_XACTUAL);
	//XTarget
	eError = TMC5160_WriteInt(eTMC, TMC5160_XTARGET, ulXActual);

	return eError;
}



// Move to a specified position with a given velocity
//void tmc5160_moveTo(TMC5160TypeDef *tmc5160, int32_t position, uint32_t velocityMax)
//void TMC5160_MoveTo(TMC_e eTMC, int32_t lPosition, uint32_t ulVelocityMax)
ErrorType_e TMC5160_MoveTo(TMC_e eTMC, int32_t lPosition)
{
	extern AxisParamDefault_t g_tAxisParamDefault;
	ErrorType_e eError = ERROR_TYPE_SUCCESS;

	TMC5160_WriteInt(eTMC, TMC5160_RAMPMODE, TMC5160_MODE_POSITION);
	g_tAxisParamDefault.ucMode[eTMC] = TMC5160_MODE_POSITION;

	// VMAX also holds the target velocity in velocity mode.
	// Re-write the position mode maximum velocity here.
//	TMC5160_WriteInt(eTMC, TMC5160_VMAX, ulVelocityMax);

	eError = TMC5160_WriteInt(eTMC, TMC5160_XTARGET, lPosition);
	return eError;
}

// Move by a given amount with a given velocity
// This function will write the absolute target position to *ticks
//void tmc5160_moveBy(TMC5160TypeDef *tmc5160, int32_t *ticks, uint32_t velocityMax)
ErrorType_e TMC5160_MoveBy(TMC_e eTMC, int32_t lTicks)
{
//	uint8_t ucStatus = 0;
	ErrorType_e eError = ERROR_TYPE_SUCCESS;
	int32_t lPos = 0;

	// determine actual position and add numbers of ticks to move
	lPos = TMC5160_ReadInt(eTMC, TMC5160_XACTUAL);
	lPos += lTicks;

	eError = TMC5160_MoveTo(eTMC, lPos);
	
	return eError;
}



/*
*	获取微步值, 从寄存器值转换成设置值
*/
uint16_t MicroStep_Register2SetValue(TMC_MicroStep_Resolution_e eMicroStep)
{
	uint16_t usMicroStep = 0;
	switch(eMicroStep)
	{
		case TMC_MICRO_STEP_RESOLUTION_256:
		{
			usMicroStep = 256;
		}
		break;
		case TMC_MICRO_STEP_RESOLUTION_128:
		{
			usMicroStep = 128;
		}
		break;
		case TMC_MICRO_STEP_RESOLUTION_64:
		{
			usMicroStep = 64;
		}
		break;
		case TMC_MICRO_STEP_RESOLUTION_32:
		{
			usMicroStep = 32;
		}
		break;
		case TMC_MICRO_STEP_RESOLUTION_16:
		{
			usMicroStep = 16;
		}
		break;
		case TMC_MICRO_STEP_RESOLUTION_8:
		{
			usMicroStep = 8;
		}
		break;
		case TMC_MICRO_STEP_RESOLUTION_4:
		{
			usMicroStep = 4;
		}
		break;
		case TMC_MICRO_STEP_RESOLUTION_2:
		{
			usMicroStep = 2;
		}
		break;
		case TMC_MICRO_STEP_RESOLUTION_1:
		{
			usMicroStep = 1;
		}
		break;
		default:break;
	}
	
	return usMicroStep;
}



/*
*	获取微步值，
*/
ErrorType_e MicroStep_SetValue2Register(uint16_t usMicroStep, uint8_t *pucMicroStep_Rg)
{
	ErrorType_e eError = ERROR_TYPE_SUCCESS;
	TMC_MicroStep_Resolution_e eMicroStep = TMC_MICRO_STEP_RESOLUTION_64; //默认64
	
	switch(usMicroStep)
	{
		case 256:
		{
			eMicroStep = TMC_MICRO_STEP_RESOLUTION_256;
		}
		break;
		case 128:
		{
			eMicroStep = TMC_MICRO_STEP_RESOLUTION_128;
		}
		break;
		case 64:
		{
			eMicroStep = TMC_MICRO_STEP_RESOLUTION_64;
		}
		break;
		case 32:
		{
			eMicroStep = TMC_MICRO_STEP_RESOLUTION_32;
		}
		break;
		case 16:
		{
			eMicroStep = TMC_MICRO_STEP_RESOLUTION_16;
		}
		break;
		case 8:
		{
			eMicroStep = TMC_MICRO_STEP_RESOLUTION_8;
		}
		break;
		case 4:
		{
			eMicroStep = TMC_MICRO_STEP_RESOLUTION_4;
		}
		break;
		case 2:
		{
			eMicroStep = TMC_MICRO_STEP_RESOLUTION_2;
		}
		break;
		case 1:
		{
			eMicroStep = TMC_MICRO_STEP_RESOLUTION_1;
		}
		break;
		default:
		{
			LOG_Error("unKnow Micro Step SetValue=%d", usMicroStep);
			return ERROR_TYPE_DATA;
		}
		//break;
	}
	
	*pucMicroStep_Rg = eMicroStep;
	
	return eError;
}





/*
 * TMC MS delay
 */
void TMC_Delay(uint32_t ulMS)
{
	HAL_Delay(ulMS);
}





//打印六点速度值
void Print_6PV_Register(TMC_e eTMC)
{

	LOG_Info("VSTART=%d, A1=%d, V1=%d, AMAX=%d, VMAX=%d, DMAX=%d, D1=%d, VSTOP=%d, TZEROWAIT=%d", \
		g_taTMC5160[eTMC].laShadowRegister[TMC5160_VSTART],g_taTMC5160[eTMC].laShadowRegister[TMC5160_A1], \
		g_taTMC5160[eTMC].laShadowRegister[TMC5160_V1],g_taTMC5160[eTMC].laShadowRegister[TMC5160_AMAX], \
		g_taTMC5160[eTMC].laShadowRegister[TMC5160_VMAX],g_taTMC5160[eTMC].laShadowRegister[TMC5160_DMAX], \
		g_taTMC5160[eTMC].laShadowRegister[TMC5160_D1],g_taTMC5160[eTMC].laShadowRegister[TMC5160_VSTOP], \
		g_taTMC5160[eTMC].laShadowRegister[TMC5160_TZEROWAIT]);
}




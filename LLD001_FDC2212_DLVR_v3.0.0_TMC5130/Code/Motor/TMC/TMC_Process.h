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

//默认Can ID
#define CAN_DEFAULT_RECV_ID		200		//默认接受CanID
#define CAN_DEFAULT_SEND_ID		200		//默认发送CanID

//子流程最大参数个数
#define SUB_PROCESS_MAX_PARAM_NUM	4

//一个流程最多支持命令个数
#define SUB_PROCESS_MAX_CMD_NUM		127


//
#define TMC_MODE_POSITION	TMC5160_MODE_POSITION
#define TMC_MODE_VELPOS		TMC5160_MODE_VELPOS   
#define TMC_MODE_VELNEG		TMC5160_MODE_VELNEG   
#define TMC_MODE_HOLD		TMC5160_MODE_HOLD     






/*
*	需保存的轴参数--默认值
*/
typedef __packed struct {	
	
	uint32_t    ulInitFlag;	 //初始化标志
	/*
	*  lEncConstValue = lFullSteppPerRound*usMicroStepResultion/usEncResultion/4 ==> lEncConstValue
	*/
	//编码器
	uint16_t usEncResultion[TMC_MODULE_END];	 //编码器分辨率
	int32_t	 lEncDiff_Threshold[TMC_MODULE_END]; //编码器检测失步，阈值
	uint8_t  ucEncCountDirect[TMC_MODULE_END];	 //编码器计数方向
	int32_t  lEncConstValue[TMC_MODULE_END];	 //编码器常数--计算获取
	
	
	//参考点-复位参数
	int32_t	lResetSpeedHigh[TMC_MODULE_END];	//复位高速	
	int32_t	lResetSpeedLow[TMC_MODULE_END];		//复位低速	
	int32_t lResetAcc[TMC_MODULE_END];			//复位加速度
	int32_t	lResetOff[TMC_MODULE_END];			//复位偏移距离
	
	//电流
	uint8_t ucIRun[TMC_MODULE_END];		  //运行电流
	uint8_t ucIHold[TMC_MODULE_END];	  //保持电流
	uint8_t ucIHoldDelay[TMC_MODULE_END]; //电流降到IHold所需时间

	//步进细分、没转全步数
	uint16_t   usMicroStepResultion[TMC_MODULE_END];	//微步细分
	uint16_t   usFullStepPerRound[TMC_MODULE_END];		//全步每转， 通用步进电机为200 	


	//六点速度参数，用于位置模式
	int32_t lVStart[TMC_MODULE_END];	//启动速度
	int32_t lA1[TMC_MODULE_END];		//第一段加速度
	int32_t lV1[TMC_MODULE_END];		//换挡速度
	int32_t lAMax[TMC_MODULE_END];		//第二段加速度
	int32_t lVMax[TMC_MODULE_END];		//最大速度
	int32_t lDMax[TMC_MODULE_END];		//第一段减速度
	int32_t lD1[TMC_MODULE_END];		//第二段减速度
	int32_t lVStop[TMC_MODULE_END];		//停止速度
	

	//驱动器运行模式
	uint8_t	ucMode[TMC_MODULE_END];		//0:位置模式。1、2速度模式，3、保持模式
	
	//速度参数、用于速度模式
	int32_t lAMax_VMode[TMC_MODULE_END];	//第二段加速度
	int32_t lVMax_VMode[TMC_MODULE_END];	//最大速度

	//参考点复位
	uint8_t ucRighLimitPolarity[TMC_MODULE_END]; //右限位极性，高有效
	uint8_t ucLeftLimitPolarity[TMC_MODULE_END]; //左限位极性，高有效
	uint8_t ucRotateDirect[TMC_MODULE_END];		 //旋转方向
	
	//校验值
	uint16_t 	usCrc;							//CRC16 校验码

} AxisParamDefault_t;





/*
*	比较或跳转  类型
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
*	计算类型  
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
	EN_CALC_RIGHT_LIFT,		//右位移
	EN_CALC_LEFT_LIFT,		//左位移
	
} Calc_e;





/*
*	子流程类型枚举
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
*	子流程类型枚举
*/
typedef enum 
{
	EN_SUB_PROCESS_TYPE_CMD    = 0,
	EN_SUB_PROCESS_TYPE_PARAM, 

} SubProcessType_e;





/*
*	流程运行状态枚举
*/
typedef enum 
{
	EN_PROCESS_STATUS_READY    = 0,		//就绪
	EN_PROCESS_STATUS_EXEC, 			//正在执行
	EN_PROCESS_STATUS_STOP, 			//停止
	EN_PROCESS_STATUS_CLEAR, 			//清除
	EN_PROCESS_STATUS_END
	
} ProcessStatus_e;



/*
*	子流程定义, 指令：可理解为函数接口（API），参数：可理解为函数参数（最多四个参数）
*/
typedef __packed struct 
{
	uint8_t	ucCmd;									//指令
	uint8_t ucParamNum;								//参数个数
	Data4Byte_u uParam[SUB_PROCESS_MAX_PARAM_NUM];	//数据
	
} SubProcess_t;



/*
*	流程定义
*/
typedef __packed struct 
{
	uint32_t 		ulInitFlag;								//初始化标志
	uint32_t		ulExecNum;								//整个大流程流程，执行的次数， 0：表示无数次，
	ProcessStatus_e eProcessStatus;							//流程状态
	uint8_t 		ucSubProcessNum;					    //子流程有效个数
	SubProcess_t 	taSubProcess[SUB_PROCESS_MAX_CMD_NUM];  //子流程数组
	uint16_t 		usCrc;									//CRC16 校验码
	
} Process_t;



/*
*	流程控制，计数器信息
*/
#define PROCESS_TIME_COUNT_NUM	4
typedef __packed struct 
{
	uint8_t 		ulStartFlag[PROCESS_TIME_COUNT_NUM];		//启动标志
	uint8_t 		ulTriggerFlag[PROCESS_TIME_COUNT_NUM];		//触发标志
	uint32_t		ulStartTick[PROCESS_TIME_COUNT_NUM];		//启动时刻Tick
	uint32_t		ulThreshTick[PROCESS_TIME_COUNT_NUM];		//阈值Tick
	
} ProtcessTimeCount_t;





//非堵塞复位
uint8_t Motor_Reset_Handle(uint32_t ulTick);

//更新编码器常数(不保存)
ErrorType_e UpdateAndSave_Enc_ConstValue(TMC_e eTMC, __IO AxisParamDefault_t *ptAxisParamDefault);

//更新保存编码器常数
ErrorType_e Update_Enc_ConstValue(TMC_e eTMC, __IO AxisParamDefault_t *ptAxisParamDefault, uint8_t ucValidFlag);

//清除EEPROM保存的轴参数
void ClearAndSave_Default_Axis_Params(void);

//清除EEPROM保存的流程
void ClearAndSave_Default_Process(void);

//轴参数初始化
ErrorType_e Axis_Param_Default_Init(void);
void Axis_Param_Fixed_SetDefault_Value(__IO AxisParamDefault_t *ptAxisParamDefault);

//寄存器初始化
void ShadowRegister_Init(__IO AxisParamDefault_t *ptAxisParamDefault);

/* 轴参数 */
//设置/获取轴参数
ErrorType_e TMC_AxisParam(TMC_e eTMC, ReadWrite_e eReadWrite, uint8_t ucType, Data4Byte_u *puData);
ErrorType_e TMC_AxisParam_Default(TMC_e eTMC, ReadWrite_e eReadWrite, uint8_t ucType, Data4Byte_u *puData);
//清除EEPROM保存的轴参数
void ClearAndSave_Default_Axis_Params(void);
//
ErrorType_e Read_Axis_Param_Default(__IO AxisParamDefault_t *ptAxisParamDefault);
ErrorType_e Save_Axis_Param_Default(__IO AxisParamDefault_t *ptAxisParamDefault);



/* 参数保存 */
ErrorType_e Read_Global_Param(__IO GlobalParam_t *ptGlobalParam);
ErrorType_e Save_Global_Param(__IO GlobalParam_t *ptGlobalParam);



/* 自定义流程 */
//过程参数初始化
void Process_Param_SetDefault_Value(__IO Process_t *ptProcess);
//清除EEPROM保存的流程
void ClearAndSave_Default_Process(void);
//流程初始化  
ErrorType_e Process_Init(void);

//设置流程
ErrorType_e Set_Process(RecvFrame_t *ptRecvFrame);
//读取流程
ErrorType_e Get_Process(uint8_t ucIndex, SubProcess_t *ptSendFrame);
//流程--控制
ErrorType_e Exec_Process_Ctrl(uint8_t ucType);
//流程读写
ErrorType_e Read_Process(__IO Process_t *ptProcess);
ErrorType_e Save_Process(__IO Process_t *ptProccess);
void Clear_Process(void);
void Del_Process(void);
void Exec_Process(void);
void Stop_Process(void );

//执行流程--清除和控制
ErrorType_e Exec_Process_Clear_Or_Save(uint8_t ucType);
//执行流程--控制
ErrorType_e Process_Handle(uint32_t ulTicks);






#endif //__TMC_PROCESS_H__















/*****************************************************************************
Copyright  : BGI
File name  : app.c
Description: 定义函数。
             注意线程中调用的子函数不能带有返回值！！！
Author     : lmj
Version    : 0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#include "stm32f10x.h"
#include "include.h"


#include "event.h"
#include "eeprom.h"
#include "bsp_gpio.h"
#include "bsp_can.h"
#include "TMC_Api.h"
#include "process.h"
#include "log.h"
#include "project.h"
#include "TMC_Process.h"
#include "param.h"
#include "lld_param.h"


//定义变量---------------------------------------------------------------------//
/*
 * @function: IWDG_Feed
 * @details : 喂狗
 * @input   : parameter:参数。未使用。
 * @output  : NULL
 * @return  : NULL
 */
void IWDG_Feed(void* parameter)
{
	// 把重装载寄存器的值放到计数器中，喂狗，防止IWDG复位
	// 当计数器的值减到0的时候会产生系统复位
	while(1)
	{
		IWDG_ReloadCounter();
		rt_thread_delay(200); 
	}
}

/*
 * @function: SWSysTimer
 * @details : 软件1s计时
 * @input   : parameter:参数。未使用。
 * @output  : NULL
 * @return  : NULL
 */
void SWSysTimer(void* parameter)
{
	while(1)
	{
		Accumulation16(&SWSysTimeTick.PowerOnS);
		Accumulation8(&CapSenPara.RdyTime);
		
//		CAN_MonInit();
		
//		StorageManStage();
//		MemManStage(&StorageMan);
		
//		MotorMoveUpAndDown();
		
		
		rt_thread_delay(1000);
	}
}


/*
 * @function: SWTimer10ms
 * @details : 软件10ms定时器
 * @input   : parameter:参数，未使用。
 * @output  : NULL
 * @return  : NULL
 */
void SWTimer10ms(void* parameter)
{
	while(1)
	{
		//同时使用IIC1和IIC2接口进行通信，发现有相互干扰现象。
		CapSensor();
//		AirSensor();
		LLDReslut();
		
		CheckPhotosensor();
		DipSWCheck();
		TempNTC();
		
		rt_thread_delay(10);
	}
}



/*** 电机部分版本号 ***/
uint16_t g_usSoftVersion 	= 0x0102;	//软件版本号
uint16_t g_usHardWareVesion	= 0x0010;	//硬件版本号


void EPPROM_Data_Reset(void)
{
	extern __IO GlobalParam_t g_tGlobalParam;
	extern __IO Process_t g_tProcess;
	extern __IO AxisParamDefault_t g_tAxisParamDefault;
	
	
	//第一次上电, 初始化，保存
	Global_Param_SetDefault_Value(&g_tGlobalParam);
	//保存		
	Save_Global_Param(&g_tGlobalParam);


	Axis_Param_Fixed_SetDefault_Value(&g_tAxisParamDefault);
	//保存轴参数
	Save_Axis_Param_Default(&g_tAxisParamDefault);

	Process_Param_SetDefault_Value(&g_tProcess);
	//保存轴参数
	Save_Process(&g_tProcess);
	
	//
	ClearAndSave_Default_LLDParams();
}



uint32_t ulTemp = 0;
//电机运用初始化
void Motor_App_Init(void)
{
//	uint32_t ulTemp = 0;
	extern __IO GlobalParam_t g_tGlobalParam;
	extern __IO Process_t g_tProcess;
	extern __IO BoardStatus_t g_tBoardStatus;
	
	LED_Shine(4, 60);
	
	ulTemp = sizeof(LLDParam_t);	//0x0B=11
	ulTemp = sizeof(GlobalParam_t); //0x291=657
	ulTemp = sizeof(AxisParamDefault_t); //0x54=84
	ulTemp = sizeof(Process_t); //0x8FA=2298
	

	//全局值初始化
	Global_Status_Init();

	//获取、打印识别码信息
	g_tBoardStatus.usSN = Get_SN(1);

	//参数初始化
	EEPROM_Init();
		
	//液面探测参数
	if(ERROR_TYPE_EEPROM == LLDParam_Init())
	{  
		//读取数据异常
		g_tBoardStatus.ucEEPRAM_Init_CRC_ErrFlag = 1;
	}
	
	//全局参数
	if(ERROR_TYPE_EEPROM == Global_Param_Init())
	{  
		//读取数据异常
		g_tBoardStatus.ucEEPRAM_Init_CRC_ErrFlag = 1;
	}
	//Read_Global_Param2(&g_tGlobalParam);
//	LOG_Info("Can: Baud=%d, RecvID=%d, SendID=%d", g_tGlobalParam.eCanBaud, g_tGlobalParam.ulRecvCanID, g_tGlobalParam.ulSendCanID);

	//默认轴参数
	if(ERROR_TYPE_EEPROM == Axis_Param_Default_Init())
	{
		//读取数据异常
		g_tBoardStatus.ucEEPRAM_Init_CRC_ErrFlag = 1;	
	}

	//EPPROM_Data_Reset();
	//Can初始化,ID
	//Bsp_Can_Init(g_tGlobalParam.eCanBaud);
	CAN_MonInit();

	//TMC
	TMC_Init();

	//流程初始化
	if(ERROR_TYPE_EEPROM == Process_Init())
	{
		//读取数据异常
		g_tBoardStatus.ucEEPRAM_Init_CRC_ErrFlag = 1;
	}

	//流程自动执行
	if(g_tGlobalParam.ucProcessAutoExecMode == 1)
	{
		//自动模式，自动执行
		g_tProcess.eProcessStatus = EN_PROCESS_STATUS_EXEC;
	}
}


/*
 * @function: CanMonitor
 * @details : 上位机Can监控
 * @input   : parameter:参数。未使用。
 * @output  : NULL
 * @return  : NULL
 */
//uint32_t lT = 0;
void CommMonitor(void* parameter)
{
		extern struct rt_messagequeue can_msg_queue;
	extern __IO LLDParam_t g_tLLDParam;
	extern __IO GlobalParam_t 	 g_tGlobalParam;
	CanRxMsg    rx_msg;

	Motor_App_Init();
	
#if USE_OS_QUEUE
	while(1)
	{
		//BSP_UartCommStage(&ModbusMon.Usart);
		memset(&rx_msg, 0, sizeof(CanRxMsg));
		if(rt_mq_recv(&can_msg_queue, &rx_msg, sizeof(CanRxMsg), RT_WAITING_FOREVER) == RT_EOK)
		{
			//比较ID
			if(((rx_msg.StdId == g_tLLDParam.CanConfig.ModuleID) || ((rx_msg.StdId == LLD_CAN_BROADCAST_ID_MOTOR))) && (CAN_ID_STD == rx_msg.IDE) && (8 == rx_msg.DLC))
			{			
				CanMonComStage();
			}
			else if(((rx_msg.StdId == g_tGlobalParam.ulRecvCanID) || (rx_msg.StdId == CAN_BROADCAST_ID_MOTOR)) && (CAN_ID_STD == rx_msg.IDE) && (8 == rx_msg.DLC))
			{
				Can_RxMsg_t *ptRxMsg = (Can_RxMsg_t*)rx_msg.Data;
				Handle_Can_RxMsg(ptRxMsg);
			}
		}
		//CanMonComStage();
		//电机协议处理
		//Event_Process();
		rt_thread_delay(2);
	}
	
#else	
	while(1)
	{
//		BSP_UartCommStage(&ModbusMon.Usart);
		CanMonComStage();
		//电机协议处理
		Event_Process();

		rt_thread_delay(2);
	}
#endif
	
}

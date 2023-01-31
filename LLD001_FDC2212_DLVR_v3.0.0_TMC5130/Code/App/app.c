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
#include "msg_handle.h"
#include "TMC2209.h"
#include "public.h"


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
		
		CAN_MonInit();
		
//		StorageManStage();
//		MemManStage(&StorageMan);
		
		MotorMoveUpAndDown();
		
		
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


/*
 * @function: CanMonitor
 * @details : 上位机Can监控
 * @input   : parameter:参数。未使用。
 * @output  : NULL
 * @return  : NULL
 */
void CommMonitor(void* parameter)
{
	uint32_t i = 0, Tick = 0;
	
	
    TMC2209_WriteInt(0x00,0x000000C0);			//gcong
    TMC2209_WriteInt(0x10,0x00070C03);          //IHOLD_IRUN: IHOLD=3=205mA, IRUN=0C=600MA电流, IHOLDDELAY=7。退吸头电流。
	
//	 TMC2209_WriteInt(0x10,0x00070903);          //IHOLD_IRUN: IHOLD=3, IRUN=09电流, IHOLDDELAY=7
	
    TMC2209_WriteInt(0x6C,0x10010053);	        //TMC2209_CHOPCONF
    TMC2209_WriteInt(0x70,0xC10D0024);			// pwmconf

	
	while(1)
	{
//		BSP_UartCommStage(&ModbusMon.Usart);
		
		CanMonComStage();
		
		//电机协议处理
//		Event_Process();
		
		
		
		
		
//		TMC2209_WriteInt(0x10,0x00071403); //IHOLD_IRUN: IHOLD=3, IRUN=0C=600MA电流, IHOLDDELAY=7
//		Tick = rt_tick_get();
//		while(i < Tick+5000)
//		{
//			TMC2209_STEP_HIGH;
//			Delay_US(10);
//			TMC2209_STEP_LOW;
//			Delay_US(10);
//			
//			i = rt_tick_get();
//		}
//		rt_thread_delay(5000);
		
		
//		//手动产生方波
//		//TMC2209_WriteInt(0x10,0x00070903); //IHOLD_IRUN: IHOLD=3, IRUN=0C=600MA电流, IHOLDDELAY=7
//		//TMC2209_WriteInt(0x10,0x00071403);
//		TMC2209_WriteInt(0x10,0x00070903); //0-31
//		Tick = rt_tick_get();
//		while(i < Tick+5000)
//		{
//			TMC2209_STEP_HIGH;
//			Delay_US(10);
//			TMC2209_STEP_LOW;
//			Delay_US(10);
//			
//			i = rt_tick_get();
//		}
////		rt_thread_delay(5000);
		
		
		
		rt_thread_delay(2);
	}
}

/*****************************************************************************
Copyright  : BGI
File name  : main.c
Description: 用于详细说明此程序文件完成的主要功能，与其他模块或函数的接口，输出
值、取值范围、含义及参数间的控制、顺序、独立或依赖等关系
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#include "stm32f10x.h"
#include "include.h"




//定义变量---------------------------------------------------------------------//
//定义线程控制块
struct rt_thread wdog_tcb;
struct rt_thread SWSysTimer_tcb;
struct rt_thread SWTimer10ms_tcb;
struct rt_thread comm_monitor_tcb;



//从SRAM动态内存分配空间
//static rt_thread_t SWSysTimer_thread = RT_NULL;


//定义线程栈时要求按RT_ALIGN_SIZE个字节对齐
ALIGN(RT_ALIGN_SIZE)
rt_uint8_t rt_wdog_stk[128];
rt_uint8_t rt_SWSysTimer_stk[128];
rt_uint8_t rt_SWTimer10ms_stk[512];
rt_uint8_t rt_comm_monitor_stk[512];



//定义函数---------------------------------------------------------------------//
/*
 * @function: 
 * @details : 
 * @input   :
 * @output  :
 * @return  :
 */
int main(void)
{
    /* 
	 * 开发板硬件初始化，RTT系统初始化已经在main函数之前完成，
	 * 即在component.c文件中的rtthread_startup()函数中完成了。
	 * 所以在main函数中，只需要创建线程和启动线程即可。
	 */
	//线程优先级设置为0~6时，实时性很好，优先级>=7时，实时性特别差，原因未知。
	//在rtconfig.h配置文件中，线程最大优先级设置为8级（默认值为8，即#define RT_THREAD_PRIORITY_MAX 8），
	//并且系统空闲线程被设置为最低优先级（默认值7），当有用户线程也设置为7时，实时性就会变差。
	
	
//	//看门狗
//	rt_thread_init(&wdog_tcb,                                        // 线程控制块
//	               "wdog",                                           // 线程名字
//	               IWDG_Feed,                                        // 线程入口函数
//	               RT_NULL,                                          // 线程入口函数参数
//	               &rt_wdog_stk[0],                                  // 线程栈起始地址
//	               sizeof(rt_wdog_stk),                              // 线程栈大小
//	               0,                                                // 线程的优先级
//	               5);                                               // 线程时间片(心跳时间tick)
//	rt_thread_startup(&wdog_tcb);                                    // 启动线程，开启调度
//	
	
	//软件计时
//	rt_thread_init(&SWSysTimer_tcb,                                  // 线程控制块
//	               "SWSysT",                                         // 线程名字
//	               SWSysTimer,                                       // 线程入口函数
//	               RT_NULL,                                          // 线程入口函数参数
//	               &rt_SWSysTimer_stk[0],                            // 线程栈起始地址
//	               sizeof(rt_SWSysTimer_stk),                        // 线程栈大小
//	               4,                                                // 线程的优先级
//	               5);                                               // 线程时间片(心跳时间tick)
//	rt_thread_startup(&SWSysTimer_tcb);                              // 启动线程，开启调度
//	
	
	//10ms软件定时
	rt_thread_init(&SWTimer10ms_tcb,                                 // 线程控制块
	               "SW10ms",                                         // 线程名字
	               SWTimer10ms,                                      // 线程入口函数
	               RT_NULL,                                          // 线程入口函数参数
	               &rt_SWTimer10ms_stk[0],                           // 线程栈起始地址
	               sizeof(rt_SWTimer10ms_stk),                       // 线程栈大小
	               1,                                                // 线程的优先级
	               5);                                               // 线程时间片(心跳时间tick)
	rt_thread_startup(&SWTimer10ms_tcb);                             // 启动线程，开启调度
	
	
	//通信
	rt_thread_init(&comm_monitor_tcb,                                // 线程控制块
	               "CommMon",                                        // 线程名字
	               CommMonitor,                                      // 线程入口函数
	               RT_NULL,                                          // 线程入口函数参数
	               &rt_comm_monitor_stk[0],                          // 线程栈起始地址
	               sizeof(rt_comm_monitor_stk),                      // 线程栈大小
	               5,                                                // 线程的优先级
	               5);                                               // 线程时间片(心跳时间tick)
	rt_thread_startup(&comm_monitor_tcb);                            // 启动线程，开启调度
}
/*********************************************END OF FILE**********************/

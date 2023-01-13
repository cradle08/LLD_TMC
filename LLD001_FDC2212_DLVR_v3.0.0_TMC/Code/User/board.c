/*****************************************************************************
Copyright  : BGI
File name  : board.c
Description: rt系统用于配置板级设备
Author     : lmj
Version    : 1.0.0
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#include "board.h" 

//RT-Thread相关头文件
#include <rthw.h>
#include <rtthread.h>
#include "include.h"
#include "TMC2209.h"



//定义变量---------------------------------------------------------------------//

//从内部SRAM里面分配一部分静态内存来作为rtt的堆空间，这里配置为4KB
#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
#define RT_HEAP_SIZE 1024
static uint32_t rt_heap[RT_HEAP_SIZE];
/*
 * @function: rt_heap_begin_get
 * @details : 获取rt堆起始地址
 * @input   : NULL
 * @output  : NULL
 * @return  : rt堆起始地址指针
 */
RT_WEAK void *rt_heap_begin_get(void)
{
    return rt_heap;
}

/*
 * @function: rt_heap_end_get
 * @details : 获取rt堆结束地址
 * @input   : NULL
 * @output  : NULL
 * @return  : rt堆结束地址指针
 */
RT_WEAK void *rt_heap_end_get(void)
{
    return rt_heap + RT_HEAP_SIZE;
}
#endif


/*
 * @function: rt_hw_board_init
 * @details : 开发板硬件初始化函数。RTT把开发板相关的初始化函数统一放到board.c文件中实现，
              当然，你想把这些函数统一放到main.c文件也是可以的。
 * @input   : NULL
 * @output  : NULL
 * @return  : rt堆结束地址指针
 */
void rt_hw_board_init()
{
	//初始化SysTick
	SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);	
	rt_thread_delay(300);
	
#if 0
	//硬件BSP初始化统统放在这里，比如LED，串口，LCD等
	//外设初始化------------------------------------//
	IWDG_Config(IWDG_Prescaler_64 ,625);         //IWDG 1s 超时溢出
//	SWSysTimerInit();                            //软件系统定时
	
	
//	IIC1_Init(); 
	IIC2_Init();                                 //IIC通信
//	GPIO_IIC1_Init();
	GPIO_IIC2_Init();
	
//	USART1_Config();                             //串口配置
//	USART2_Config();
	
	CAN_MonInit();
	
	ADCx_Init();
	
	GPIO_Config();
	
	
	GenDelay_nop(72000);
	//用户初始化------------------------------------//
	DipSWInit();
	DipSWCheck();
	
	//外设二次初始化----------------------------------//
//	//在此初始化Can设备，通信偶尔故障，原因未知。
//	CAN_MonInit();
	
		
	//用户二次初始化----------------------------------//
//	MonModbusCommInit();
	LLDInit();
	FLASH_Init();
	
#else
	//初始化SysTick
	SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);	

	//硬件BSP初始化统统放在这里，比如LED，串口，LCD等
	//外设初始化------------------------------------//
//	IWDG_Config(IWDG_Prescaler_64 ,625);         //IWDG 1s 超时溢出
//	SWSysTimerInit();                            //软件系统定时
	
////	IIC1_Init(); 
//	IIC2_Init();                                 //IIC通信
////	GPIO_IIC1_Init();
//	GPIO_IIC2_Init();
	
	USART1_Config();                             //串口配置
//	USART2_Config();
	
//	SysEventInit();
//	CAN_MonInit();
	
//	ADCx_Init();
	
	GPIO_Config();
	TMC2209_Init();
	
	GenDelay_nop(72000);
	//用户初始化------------------------------------//
//	DipSWInit();
//	DipSWCheck();
	
	//外设二次初始化----------------------------------//
//	//在此初始化Can设备，通信偶尔故障，原因未知。
//	CAN_MonInit();
	
		
	//用户二次初始化----------------------------------//
////	MonModbusCommInit();
//	LLDInit();
//	FLASH_Init();

#endif
	
	
	
	
//调用组件初始化函数 (use INIT_BOARD_EXPORT())
#ifdef RT_USING_COMPONENTS_INIT
	rt_components_board_init();
#endif
    
#if defined(RT_USING_CONSOLE) && defined(RT_USING_DEVICE)
	rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif
    
#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
	rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());
#endif
}


/*
 * @function: SysTick_Handler
 * @details : SysTick中断服务函数。
               SysTick中断服务函数在固件库文件stm32f10x_it.c中也定义了，而现在
			   在board.c中又定义一次，那么编译的时候会出现重复定义的错误，解决
			   方法是可以把stm32f10x_it.c中的注释或者删除即可。
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
//void SysTick_Handler(void)
//{

//}



/**
  * @brief  重映射串口DEBUG_USARTx到rt_kprintf()函数
  *   Note：DEBUG_USARTx是在bsp_usart.h中定义的宏，默认使用串口1
  * @param  str：要输出到串口的字符串
  * @retval 无
  *
  * @attention
  * 
  */
/*
 * @function: rt_hw_console_output
 * @details : 重映射串口DEBUG_USARTx到rt_kprintf()函数
              DEBUG_USARTx是在bsp_usart.h中定义的宏，默认使用串口1
 * @input   : str：要输出到串口的字符串。
 * @output  : NULL
 * @return  : NULL
 */
//void rt_hw_console_output(const char *str)
//{	
//	/* 进入临界段 */
//    rt_enter_critical();

//	/* 直到字符串结束 */
//    while (*str!='\0')
//	{
//		/* 换行 */
//        if (*str=='\n')
//		{
////			USART_SendData(DEBUG_USARTx, '\r'); 
////			while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);
//		}

////		USART_SendData(DEBUG_USARTx, *str++); 				
////		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);	
//	}	

//	/* 退出临界段 */
//    rt_exit_critical();
//}

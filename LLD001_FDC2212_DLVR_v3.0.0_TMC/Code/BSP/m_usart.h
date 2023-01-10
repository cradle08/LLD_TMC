/*****************************************************************************
Copyright  : BGI
File name  : m_usart.h
Description: 串口
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#ifndef __M_USART_H
#define	__M_USART_H


#include "stm32f10x.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "bsp_misc.h"
#include "bsp_usart.h"



//宏定义----------------------------------------------------------------------//
//主从模式
#define  UART_MODE_SLAVE               0    // 从模式设备（包含从模式转发器）
#define  UART_MODE_MASTER              1    // 主模式设备

//串口输入输出状态
#define  UART_REC_MODE                 0    //接收模式
#define  UART_SEND_MODE                1    //发送模式
#define  UART_CLOSE_MODE               2    //禁用模式


//通信阶段状态
#define  COMM_NULL                     0    //空状态
#define  COMM_INIT                     1    //初始化
#define  COMM_WAIT                     2    //空闲状态
#define  COMM_UPDATA                   3    //解析状态
#define  COMM_SNED_READY               4    //发送准备阶段	
#define  COMM_SEND_WAIT                5    //发送等待阶段
#define  COMM_SEND_ING                 6    //发送中状态




//定义结构体--------------------------------------------------------------------//
//Uart通信
struct tagUartCom 
{
	uint8_t    CommMode;                         //串口工作模式，发送或接收
	uint8_t    Stage;                            //串口通信运行阶段
	uint8_t    SendNum;                          //待发送字节数
	uint16_t   RecvCnt;                          //计数当前接收数据帧字节数
	uint16_t   SendCnt;                          //计数当前发送数据帧字节数
	uint8_t    SendBuf[UART_SEND_MAXNUM];        //发送接收缓冲区
	uint8_t    RecvBuf[UART_RECV_MAXNUM];
	
	uint8_t    Receiving;                        //正在接收标志
	uint8_t    RepeatInitFlag;                   //再初始化标志 （初始化后自动清零）
	
	uint8_t    RecvTimeCnt;                      //接收到字节时开始计时，用于判断字节接收是否超时。
	uint8_t    RecvEndTime;                      //接收完一帧数据的设定时间（用户设定），与波特率有关。
	
	uint16_t   SendDlyCnt;                       //发送前延时计时               
	uint16_t   SendDlyTime;                      //发送前设置延时

	uint16_t   ForceJumpDlyCnt;                  //强制跳转完成判断时间计时
	uint16_t   ForceJumpDlyTime;                 //在发送准备阶段等待太久，强制跳走
	
	uint16_t   NetworkIdleTime;                  //网络空闲持续时间（累加变量） 
	uint8_t    SendEndTime;                      //发送结束判断时间T2（用户设定）（串口发送中断中使用）
	uint16_t   SendIntervalTime;                 //发送间隔判断时间T4（主模式使用）（用户设定）
	
	//=================== ===========================================
 	uint8_t   (*CommUpdate)(void *);             // 解析函数指针
	uint8_t   (*CommSend)(void *);               // 发送函数指针
	void      (*SetRecvOrSend)(uint8_t recv_send_flag, struct tagUartCom *);      // 设置接收
	void      (*Set485OnOff)(uint8_t set_value);              //设置485开关
	//=================== ============================================
	
	uint16_t   OutTime;                          //通讯中断时间
	uint8_t    IsAnotherPackFlag;                //是否还有数据帧需要连续发送，0：没有；1：有

};





//声明变量----------------------------------------------------------------------//



//声明函数----------------------------------------------------------------------//
void UsartIRQReceiveHandler(struct tagUartCom *pUart, uint8_t ch);

extern void BSP_UartCommTimeHandler(void* com);
extern void BSP_UartCommStage(void* com);
extern uint8_t BSP_CommUartInit(void* com,
                                uint8_t (*recv_fun)(void *),
                                uint8_t (*send_fun)(void *),
                                void ( *set_recv_send )(uint8_t recv_send_flag, struct tagUartCom *),
                                void ( *set_485_on_off)(uint8_t set_value),
                                uint8_t comm_mode,								
								uint8_t recv_end_time,								
                                uint8_t send_dly,
                                uint8_t send_end_time,
                                uint16_t send_cycle,                              
                                uint16_t force_jump_time);
								
#endif

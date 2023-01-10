/*
**  事件管理模块 :
**
**      事件管理模块是系统核心的分模块，任务是将系统中所发生的各种事件
**  提交给用户界面处理模块。系统事件有多种类型，每种类型包含一类相关事
**  件。为了简化事件接口的程序设计，对各类系统事件建立统一的事件数据结
**  构，并由事件管理模块进行管理。
**
**  本文件定义以下内容 :
**
**      * 系统事件类型
**      * 故障事件出错码
**      * 事件数据结构
**      * 事件管理API函数原型
*/

#ifndef __EVENT_H__
#define __EVENT_H__

#include "main.h"
#include "stdlib.h"




//串口buffer长度
#define DATA_BUF_LEN	  8
#define USART_BUFFER_LEN  10



//开关，全局中断
#define EVENT_USE_OS	0
#if EVENT_USE_OS
#define ENABLE_EVENT_IRQ	__enable_irq()
#define DISABLE_EVENT_IRQ	__disable_irq()
#else
#define ENABLE_EVENT_IRQ
#define DISABLE_EVENT_IRQ
#endif

/*
*	消息队列，类型
*/
typedef enum {
	MSG_TYPE_NULL	= 0,
	MSG_TYPE_KEY,
	MSG_TYPE_TIMER,
	MSG_TYPE_USART,
	MSG_TYPE_CAN,
	MSG_TYPE_ADC,
	MSG_TYPE_DEBUG,
	MSG_TYPE_END

} MsgType_e;



//按键事件 消息结构体
typedef struct KEY_ET
{
	uint8_t ucflag;		//是否长按键
	uint8_t ucKeyValue;
	
}MsgKey_t;



/* 
*  定时事件,消息结构体
*/
typedef struct {
	uint8_t  ucIndex;	//定时器类型
	uint16_t usCount;
	
}MsgTimer_t;



/*
*	串口事件,消息结构体
*/
typedef struct {
	uint8_t 	ucIndex;		//串口号
	uint8_t 	ucaBuffer[USART_BUFFER_LEN]; //消息buffer
	uint16_t	usLen; //消息长度
	
}MsgUsart_t;



/*
*	老化事件，信息结构体
*/
typedef struct {
	uint8_t  ucIndex;		//老化选项
	
}Aging_t;




// 系统事件数据结构定义：
typedef struct SysEvent{
    MsgType_e			eMsgType;       //事件类型
    union
    {
        MsgKey_t		tMsgKey;  		//按键
		MsgTimer_t		tMsgTimer;		//定时器事件
		MsgUsart_t		tMsgUsart;		//串口消息
		Aging_t			tAging;			//老化事件
		uint8_t			ucaDataBuf[DATA_BUF_LEN];	//数据buffer
    }tMsg;
    struct SysEvent   *ptNext;  // 下一个事件
	
} SysEvent_t;



// 事件管理模块API函数原型：
void SysEventInit(void);                 // 事件管理模块初始化
SysEvent_t * SysEventAlloc(void) ;         // 分配一个事件缓存
void newSysEventAlloc(SysEvent_t **e);     // 分配一个事件缓存
void SysEventFree(SysEvent_t *e);          // 释放一个事件缓存
SysEvent_t * SysEventGet(void);            // 取下一个系统事件
void SysEventPut(SysEvent_t *e, int pri);  // reentrant; // 系统事件压入队列






#endif //__EVENT_H__





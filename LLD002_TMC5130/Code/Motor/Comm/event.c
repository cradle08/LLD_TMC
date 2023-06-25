/*
**  Program   : event.c
**  Purpose   :
**      事件管理模块的 API函数实现。事件管理模块是系统核心的分模块，任
**  务是将系统中所发生的各种事件提交给用户界面处理模块。系统事件有多种
**  类型，每种类型包含一类相关事件。
**
**  Functions :
**
**      SysEventInit()     - 事件管理模块初始化
**      SysEventAlloc()    - 分配一个事件缓存
**      SysEventFree()     - 释放一个事件缓存
**      SysEventGet()      - 取下一个系统事件
**      SysEventPut()      - 系统事件压入队列
*/
//#include <string.h>                   // string and memory functions          

//#include "GlobalInfo.h"
#include "event.h"
//#include "task.h"

#define MAXSYSEVENTS 16   // 最大待处理系统事件数

// 系统事件队列的首、尾指针数组：
SysEvent_t *SysEventHead[2];          		// 队首指针
SysEvent_t *SysEventTail[2];          		// 队尾指针
SysEvent_t *SysEventFreeList;         		// 空闲链指针
SysEvent_t SysEventBuf[MAXSYSEVENTS] = {0}; // 系统事件缓存
uint8_t SysEventCount = 0;    				// 事件计数


/*
**     SysEventHead[2]和SysEventTail[2]分别是当前不同优先级系统事件
** 队列的队首和队尾指针。新到系统事件从队尾处加入到系统事件队列，下
** 一个被取出处理的系统事件从队首处取出(按优先级)。
**     SysEventFreeList 是当前空闲的系统事件缓存链。新增系统事件从
** SysEventFreeList 链首取一个空闲的系统事件缓存，用过的系统事件缓
** 存再放回此链。
**     SysEventBuf[MAXSYSEVENTS]是系统事件缓存区。
**     SysEventCount 是当前事件计数。当系统事件的数目超过64时，按键
** 事件将忽略。
*/


/*
** Function : SysEventInit
** Purpose  :
**     事件模块初始化.
*/
void SysEventInit(void)
{
    int i;

	DISABLE_EVENT_IRQ;
	// 各优先级队列置为空：无系统事件
	for ( i = 0; i < 2; i++ )
	{
        SysEventHead[i] = SysEventTail[i] = (SysEvent_t *)NULL;
	}

    // 将所有的系统事件缓存串联起来，放入自由链
    SysEventFreeList = &SysEventBuf[0];

    for ( i = 0; i < MAXSYSEVENTS - 1; i++ )
	{
        SysEventBuf[i].ptNext = &SysEventBuf[i+1];
	}

    SysEventBuf[i].ptNext = (SysEvent_t *)NULL;

    // 系统事件计数清0
    SysEventCount = 0;
	
	//
	ENABLE_EVENT_IRQ;
}


/*
**  Function : SysEventAlloc
**  Purpose  :
**      从SysEventFreeList链首处分配一个空闲事件缓存指针
**  Return   :
**      分配到的事件缓存指针
*/
SysEvent_t *SysEventAlloc(void) 
{
    SysEvent_t * e;

	//cli();
	DISABLE_EVENT_IRQ;
    // 取一个事件缓存。如果自由链不空，SysEventFreeList下移
    e = SysEventFreeList;
    if (  e != (SysEvent_t *) NULL ){
        SysEventFreeList = SysEventFreeList->ptNext;
	}
    //sei();
	ENABLE_EVENT_IRQ;

    // 返回该事件缓存，或为空
    return e;
}



/*
**  Function : SysEventFree
**  Purpose  :
**      释放一块已用过的事件缓存到SysEventFreeList上
**  Params   :
**      e : 要释放的事件指针.
*/
void SysEventFree(SysEvent_t *e)
{
    
    // 插到自由链首
    if ( e != (SysEvent_t *)NULL )
    {
		//
//		DISABLE_EVENT_IRQ;
		rt_enter_critical();
		
		memset((void*)e, 0, sizeof(SysEvent_t));
        e->ptNext = SysEventFreeList;
        SysEventFreeList = e;
		
		//
//		ENABLE_EVENT_IRQ;
		rt_exit_critical();
	}	
}



/*
**  Function : SysEventGet
**  Purpose  :
**      从当前有事件的优先级最高的系统事件队列中取出一个系统事件，
**      将其从队列中摘下，并返回给调用者。如所有队列为NULL，则返回
**      (SysEvent *)NULL。
**  Return  :
**      返回获得的事件指针或空.
**  Calls   :
**      TaskSched : 任务管理模块函数, task.c;
*/
SysEvent_t * SysEventGet(void)
{
    int i;
    SysEvent_t *e;
	
	
//	DISABLE_EVENT_IRQ;
	rt_enter_critical();

	// 从最高优先级开始查找
    for ( i = 0; i < 2; i++ )
    {

        e = SysEventHead[i];

        if ( e != (SysEvent_t *)NULL )
        {
            // 如找到，从队列中摘下
            SysEventHead[i] = SysEventHead[i]->ptNext; 
            if ( SysEventHead[i] == NULL)
                SysEventTail[i] = (SysEvent_t *)NULL;

            // 事件计数减1
            SysEventCount--;
            break;
        }
    }

//	ENABLE_EVENT_IRQ;
	rt_exit_critical();
    return e;
}
	



/*
**  Function : SysEventPut
**  Purpose  :
**      将系统其他部分提交的指定优先级的新事件挂到该优先级系统事件队
**      列队尾处。
**  Params  :
**      e   : 事件指针;
**      pri : 事件优先级.
*/
void SysEventPut( SysEvent_t *e, int pri ) //reentrant
{

	DISABLE_EVENT_IRQ;
    // e 不为空
    if ( e != (SysEvent_t *)NULL )
    {
        // 若事件计数小于16
        if ( SysEventCount < MAXSYSEVENTS   )
        {

            // 事件计数加1
            SysEventCount++;

            // 将 e 加到事件队列尾
            e->ptNext = (SysEvent_t *)NULL;

            if ( SysEventTail[pri] )
                SysEventTail[pri]->ptNext = e;
            else
                SysEventHead[pri] = e;

            SysEventTail[pri] = e;
        }									       
        else
        {
            // 若事件计数已满16, 放弃事件
            e->ptNext = SysEventFreeList;
            SysEventFreeList = e;
        }
    }
	//sei();
	ENABLE_EVENT_IRQ;
}







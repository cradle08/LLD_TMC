/*
**  Program   : event.c
**  Purpose   :
**      �¼�����ģ��� API����ʵ�֡��¼�����ģ����ϵͳ���ĵķ�ģ�飬��
**  ���ǽ�ϵͳ���������ĸ����¼��ύ���û����洦��ģ�顣ϵͳ�¼��ж���
**  ���ͣ�ÿ�����Ͱ���һ������¼���
**
**  Functions :
**
**      SysEventInit()     - �¼�����ģ���ʼ��
**      SysEventAlloc()    - ����һ���¼�����
**      SysEventFree()     - �ͷ�һ���¼�����
**      SysEventGet()      - ȡ��һ��ϵͳ�¼�
**      SysEventPut()      - ϵͳ�¼�ѹ�����
*/
//#include <string.h>                   // string and memory functions          

//#include "GlobalInfo.h"
#include "event.h"
//#include "task.h"

#define MAXSYSEVENTS 16   // ��������ϵͳ�¼���

// ϵͳ�¼����е��ס�βָ�����飺
SysEvent_t *SysEventHead[2];          		// ����ָ��
SysEvent_t *SysEventTail[2];          		// ��βָ��
SysEvent_t *SysEventFreeList;         		// ������ָ��
SysEvent_t SysEventBuf[MAXSYSEVENTS] = {0}; // ϵͳ�¼�����
uint8_t SysEventCount = 0;    				// �¼�����


/*
**     SysEventHead[2]��SysEventTail[2]�ֱ��ǵ�ǰ��ͬ���ȼ�ϵͳ�¼�
** ���еĶ��׺Ͷ�βָ�롣�µ�ϵͳ�¼��Ӷ�β�����뵽ϵͳ�¼����У���
** һ����ȡ�������ϵͳ�¼��Ӷ��״�ȡ��(�����ȼ�)��
**     SysEventFreeList �ǵ�ǰ���е�ϵͳ�¼�������������ϵͳ�¼���
** SysEventFreeList ����ȡһ�����е�ϵͳ�¼����棬�ù���ϵͳ�¼���
** ���ٷŻش�����
**     SysEventBuf[MAXSYSEVENTS]��ϵͳ�¼���������
**     SysEventCount �ǵ�ǰ�¼���������ϵͳ�¼�����Ŀ����64ʱ������
** �¼������ԡ�
*/


/*
** Function : SysEventInit
** Purpose  :
**     �¼�ģ���ʼ��.
*/
void SysEventInit(void)
{
    int i;

	DISABLE_EVENT_IRQ;
	// �����ȼ�������Ϊ�գ���ϵͳ�¼�
	for ( i = 0; i < 2; i++ )
	{
        SysEventHead[i] = SysEventTail[i] = (SysEvent_t *)NULL;
	}

    // �����е�ϵͳ�¼����洮������������������
    SysEventFreeList = &SysEventBuf[0];

    for ( i = 0; i < MAXSYSEVENTS - 1; i++ )
	{
        SysEventBuf[i].ptNext = &SysEventBuf[i+1];
	}

    SysEventBuf[i].ptNext = (SysEvent_t *)NULL;

    // ϵͳ�¼�������0
    SysEventCount = 0;
	
	//
	ENABLE_EVENT_IRQ;
}


/*
**  Function : SysEventAlloc
**  Purpose  :
**      ��SysEventFreeList���״�����һ�������¼�����ָ��
**  Return   :
**      ���䵽���¼�����ָ��
*/
SysEvent_t *SysEventAlloc(void) 
{
    SysEvent_t * e;

	//cli();
	DISABLE_EVENT_IRQ;
    // ȡһ���¼����档������������գ�SysEventFreeList����
    e = SysEventFreeList;
    if (  e != (SysEvent_t *) NULL ){
        SysEventFreeList = SysEventFreeList->ptNext;
	}
    //sei();
	ENABLE_EVENT_IRQ;

    // ���ظ��¼����棬��Ϊ��
    return e;
}



/*
**  Function : SysEventFree
**  Purpose  :
**      �ͷ�һ�����ù����¼����浽SysEventFreeList��
**  Params   :
**      e : Ҫ�ͷŵ��¼�ָ��.
*/
void SysEventFree(SysEvent_t *e)
{
    
    // �嵽��������
    if ( e != (SysEvent_t *)NULL )
    {
		//
		DISABLE_EVENT_IRQ;
		
		memset((void*)e, 0, sizeof(SysEvent_t));
        e->ptNext = SysEventFreeList;
        SysEventFreeList = e;
		
		//
		ENABLE_EVENT_IRQ;
	}	
}



/*
**  Function : SysEventGet
**  Purpose  :
**      �ӵ�ǰ���¼������ȼ���ߵ�ϵͳ�¼�������ȡ��һ��ϵͳ�¼���
**      ����Ӷ�����ժ�£������ظ������ߡ������ж���ΪNULL���򷵻�
**      (SysEvent *)NULL��
**  Return  :
**      ���ػ�õ��¼�ָ����.
**  Calls   :
**      TaskSched : �������ģ�麯��, task.c;
*/
SysEvent_t * SysEventGet(void)
{
    int i;
    SysEvent_t *e;

	DISABLE_EVENT_IRQ;

	// ��������ȼ���ʼ����
    for ( i = 0; i < 2; i++ )
    {

        e = SysEventHead[i];

        if ( e != (SysEvent_t *)NULL )
        {
            // ���ҵ����Ӷ�����ժ��
            SysEventHead[i] = SysEventHead[i]->ptNext; 
            if ( SysEventHead[i] == NULL)
                SysEventTail[i] = (SysEvent_t *)NULL;

            // �¼�������1
            SysEventCount--;
            break;
        }
    }

	ENABLE_EVENT_IRQ;
    return e;
}
	



/*
**  Function : SysEventPut
**  Purpose  :
**      ��ϵͳ���������ύ��ָ�����ȼ������¼��ҵ������ȼ�ϵͳ�¼���
**      �ж�β����
**  Params  :
**      e   : �¼�ָ��;
**      pri : �¼����ȼ�.
*/
void SysEventPut( SysEvent_t *e, int pri ) //reentrant
{

	DISABLE_EVENT_IRQ;
    // e ��Ϊ��
    if ( e != (SysEvent_t *)NULL )
    {
        // ���¼�����С��16
        if ( SysEventCount < MAXSYSEVENTS   )
        {

            // �¼�������1
            SysEventCount++;

            // �� e �ӵ��¼�����β
            e->ptNext = (SysEvent_t *)NULL;

            if ( SysEventTail[pri] )
                SysEventTail[pri]->ptNext = e;
            else
                SysEventHead[pri] = e;

            SysEventTail[pri] = e;
        }									       
        else
        {
            // ���¼���������16, �����¼�
            e->ptNext = SysEventFreeList;
            SysEventFreeList = e;
        }
    }
	//sei();
	ENABLE_EVENT_IRQ;
}







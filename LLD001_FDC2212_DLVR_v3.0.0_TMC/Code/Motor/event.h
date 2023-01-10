/*
**  �¼�����ģ�� :
**
**      �¼�����ģ����ϵͳ���ĵķ�ģ�飬�����ǽ�ϵͳ���������ĸ����¼�
**  �ύ���û����洦��ģ�顣ϵͳ�¼��ж������ͣ�ÿ�����Ͱ���һ�������
**  ����Ϊ�˼��¼��ӿڵĳ�����ƣ��Ը���ϵͳ�¼�����ͳһ���¼����ݽ�
**  ���������¼�����ģ����й���
**
**  ���ļ������������� :
**
**      * ϵͳ�¼�����
**      * �����¼�������
**      * �¼����ݽṹ
**      * �¼�����API����ԭ��
*/

#ifndef __EVENT_H__
#define __EVENT_H__

#include "main.h"
#include "stdlib.h"




//����buffer����
#define DATA_BUF_LEN	  8
#define USART_BUFFER_LEN  10



//���أ�ȫ���ж�
#define EVENT_USE_OS	0
#if EVENT_USE_OS
#define ENABLE_EVENT_IRQ	__enable_irq()
#define DISABLE_EVENT_IRQ	__disable_irq()
#else
#define ENABLE_EVENT_IRQ
#define DISABLE_EVENT_IRQ
#endif

/*
*	��Ϣ���У�����
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



//�����¼� ��Ϣ�ṹ��
typedef struct KEY_ET
{
	uint8_t ucflag;		//�Ƿ񳤰���
	uint8_t ucKeyValue;
	
}MsgKey_t;



/* 
*  ��ʱ�¼�,��Ϣ�ṹ��
*/
typedef struct {
	uint8_t  ucIndex;	//��ʱ������
	uint16_t usCount;
	
}MsgTimer_t;



/*
*	�����¼�,��Ϣ�ṹ��
*/
typedef struct {
	uint8_t 	ucIndex;		//���ں�
	uint8_t 	ucaBuffer[USART_BUFFER_LEN]; //��Ϣbuffer
	uint16_t	usLen; //��Ϣ����
	
}MsgUsart_t;



/*
*	�ϻ��¼�����Ϣ�ṹ��
*/
typedef struct {
	uint8_t  ucIndex;		//�ϻ�ѡ��
	
}Aging_t;




// ϵͳ�¼����ݽṹ���壺
typedef struct SysEvent{
    MsgType_e			eMsgType;       //�¼�����
    union
    {
        MsgKey_t		tMsgKey;  		//����
		MsgTimer_t		tMsgTimer;		//��ʱ���¼�
		MsgUsart_t		tMsgUsart;		//������Ϣ
		Aging_t			tAging;			//�ϻ��¼�
		uint8_t			ucaDataBuf[DATA_BUF_LEN];	//����buffer
    }tMsg;
    struct SysEvent   *ptNext;  // ��һ���¼�
	
} SysEvent_t;



// �¼�����ģ��API����ԭ�ͣ�
void SysEventInit(void);                 // �¼�����ģ���ʼ��
SysEvent_t * SysEventAlloc(void) ;         // ����һ���¼�����
void newSysEventAlloc(SysEvent_t **e);     // ����һ���¼�����
void SysEventFree(SysEvent_t *e);          // �ͷ�һ���¼�����
SysEvent_t * SysEventGet(void);            // ȡ��һ��ϵͳ�¼�
void SysEventPut(SysEvent_t *e, int pri);  // reentrant; // ϵͳ�¼�ѹ�����






#endif //__EVENT_H__





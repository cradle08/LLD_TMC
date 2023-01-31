/*****************************************************************************
Copyright  : BGI
File name  : bsp_can.h
Description: can通信
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#ifndef __BSP_CAN_H
#define __BSP_CAN_H


#include "stm32f10x.h"




//宏定义----------------------------------------------------------------------//
// Log define
//#define  CAN_INFO(fmt,arg...)           printf("<<-CAN-INFO->> "fmt"\n",##arg)
//#define  CAN_ERROR(fmt,arg...)          printf("<<-CAN-ERROR->> "fmt"\n",##arg)
//#define  CAN_DEBUG(fmt,arg...)          do{\
//                                            if(CAN_DEBUG_ON)\
//                                            printf("<<-CAN-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
//                                        }while(0)

//#define CAN_DEBUG_ARRAY(array, num)     do{\
//                                            int32_t i;\
//                                            uint8_t* a = array;\
//                                            if(CAN_DEBUG_ARRAY_ON)\
//                                            {\
//                                                printf("<<-CAN-DEBUG-ARRAY->>\n");\
//                                                for(i = 0; i < (num); i++)\
//                                                {\
//                                                    printf("%02x   ", (a)[i]);\
//                                                    if((i + 1 ) %10 == 0)\
//                                                    {\
//                                                        printf("\n");\
//                                                    }\
//                                                }\
//                                                printf("\n");\
//                                            }\
//                                        }while(0)

//#define CAN_DEBUG_FUNC()                do{\
//                                            if(CAN_DEBUG_FUNC_ON)\
//                                            printf("<<-CAN-FUNC->> Func:%s@Line:%d\n",__func__,__LINE__);\
//                                        }while(0)


//定义结构体--------------------------------------------------------------------//
struct tagCanConfig
{
	uint16_t    ModuleID;                                                      //本模块ID
	uint16_t    ReplyID;                                                       //应答ID	
	uint16_t    BroadcastID;                                                   //广播ID	
	
	uint16_t    ModuleID_Motor;                                                  //本模块ID  电机部分
	uint16_t    ReplyID_Motor;                                                   //应答ID	   电机部分
	uint16_t    BroadcastID_Motor;                                               //广播ID	   电机部分

	//	uint16_t    Bps;                                                           //波特率
	uint16_t    BpsMode;                                                       //波特率模式
};



//声明变量----------------------------------------------------------------------//




//声明函数----------------------------------------------------------------------//
void CAN_GPIO_Config(void);
void CAN_NVIC_Config(void);
void CAN_Config(CAN_TypeDef* can, struct tagCanConfig *confg);

void Init_RxMes(CanRxMsg *rx_msg);



#endif



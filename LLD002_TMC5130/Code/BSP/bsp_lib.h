/*****************************************************************************
Copyright  : BGI
File name  : bsp_lib.h
Description: 公用函数库
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#ifndef __BSP_LIB_H
#define __BSP_LIB_H


#include "stm32f10x.h"




//宏定义----------------------------------------------------------------------//
//测量数组元素长度
#define  ARRAY_SIZE(x)                 ((sizeof(x)/sizeof(x[0])))
#define  MERGE_UINT16(H, L)            ((uint16_t)((H<<8) | (L)))

//对变量的位数据置位或清零
#define  BITSET(x,y)                   (x |= (1<<y))
#define  BITCLR(x,y)                   (x &= ~(1<<y))

//读取变量的位数据
#define  GETBIT(Obj, StartBit)         (((Obj) & (0x01<<StartBit))>>StartBit)





//定义结构体--------------------------------------------------------------------//
//循环队列
struct tagQueueLoop
{
	int16_t     Front;                                                        //队列前排数据位置
	int16_t     Rear;                                                         //队列末尾数据位置
	int16_t     Len;                                                          //队列长度
	int16_t     DataNum;                                                      //队列数据个数
	
	uint8_t     *Buf;
};


//声明变量----------------------------------------------------------------------//



//声明函数----------------------------------------------------------------------//
uint8_t BSP_Crc8(uint8_t *pframe, uint8_t len);
uint16_t BSP_Crc16(uint8_t* frame, uint32_t len);
uint16_t Smoothfilter(uint16_t *buf, uint16_t data, uint8_t len);
uint32_t Smoothfilter32(uint32_t *buf, uint32_t data, uint8_t len);
uint32_t LPF1(uint32_t data, uint32_t pre_data, float ratio);
uint32_t LPF2(int32_t data, int32_t pre_data, float ratio);
uint8_t SmoothPipeline16(uint16_t *buf, uint16_t data, uint8_t len);
uint8_t SmoothPipeline32(uint32_t *buf, uint32_t data, uint8_t len);

void Accumulation8(uint8_t* var);
void DecreaseUint8(uint8_t* var);
void Accumulation16(uint16_t* var);
void DecreaseUint16(uint16_t* var);
void Accumulation32(uint32_t* var);

void GenDelay_nop(uint32_t ntimer);
uint8_t CheckUint16ParaValid(uint16_t *check_val, uint16_t min, uint16_t max, uint16_t def_val);
uint8_t MyMemSet16(uint16_t *buf, uint16_t data, uint8_t len);
uint8_t MyMemSet32(uint32_t *buf, uint32_t data, uint8_t len);

uint8_t QueueLoopInit(struct tagQueueLoop *queue, void *buf, uint16_t len);
uint8_t QueueLoopClear(struct tagQueueLoop *queue);
uint16_t QueueLoopDataNum(struct tagQueueLoop *queue);
uint16_t QueueLoopIsEmpty(struct tagQueueLoop *queue);
uint16_t QueueLoopIsFull(struct tagQueueLoop *queue);



#endif



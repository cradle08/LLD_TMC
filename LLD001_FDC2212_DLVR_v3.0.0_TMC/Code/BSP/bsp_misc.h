/*****************************************************************************
Copyright  : BGI
File name  : bsp_misc.c
Description: 乱七八糟的东西
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#ifndef __BSP_MISC_H
#define __BSP_MISC_H

#ifdef __cplusplus
 extern "C" {
#endif 

//Includes ------------------------------------------------------------------//
#include "stm32f10x.h"





	 
	 
//宏定义----------------------------------------------------------------------//
//程序名称
#define  SOFTWARE_NAME_LEN             24u 
#define  SOFTWARE_VER_LEN              4u 


//逻辑真假
#define  FALSE                         0    
#define  TRUE                          1 


//LED
#define  LED_OFF                       0    
#define  LED_ON                        1 


//IO
#define  IO_LOW                        0    
#define  IO_HIGH                       1 


//滑动滤波数据个数
#define  AVERAGE_NUM                   16


//定义结构体--------------------------------------------------------------------//
//采集数据相关
struct tagCollectData
{
	uint32_t   OrigData[AVERAGE_NUM];            //原始AD值
	uint32_t   FilterData[AVERAGE_NUM];          //经过滤波后的值
	uint32_t   OrigValue;	                     //原始AD值
	uint32_t   FilterValue;                      //经过滤波后的值
	
	int32_t    Value;                            //经过各种转换后的值
	uint8_t    ErrTime;                          //传感器异常计时
	uint8_t    IsErr;                            //是否故障  0:无故障   1:故障	
};


//声明变量---------------------------------------------------------------------//
typedef unsigned int    BOOL;

extern const char SoftWareName[];
extern const uint16_t SoftWareVer[];



//声明函数----------------------------------------------------------------------//





#ifdef __cplusplus
}
#endif
  
#endif


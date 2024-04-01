/*****************************************************************************
Copyright  : BGI
File name  : storage.c
Description: 存储管理
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#include "storage.h"
#include "include.h"




//定义变量---------------------------------------------------------------------//
//用户参数
struct tagPara    UserPara[USER_PARA_NUM];
struct tagStorage    Storage;






//定义函数---------------------------------------------------------------------//
/*
 * @function: SetPara16
 * @details : 参数赋值。
 * @input   : 1.data：数据指针。
              2.val：数据值。
 * @output  : NULL
 * @return  : 0：成功；其他：故障指令。
 */
uint8_t SetPara16(struct tagPara *data, uint16_t val)
{
	uint8_t  ret = FALSE;
	
	
	if((val < data->Min) || (val > data->Max))
	{
		ret = TRUE;
	}
	else
	{
		data->Value = val;
	}
	
	
	return ( ret );
}

/*
 * @function: SetPara32
 * @details : 参数赋值。
 * @input   : 1.data：数据指针。
              2.val：数据值。
 * @output  : NULL
 * @return  : 0：成功；其他：故障指令。
 */
uint8_t SetPara32(struct tagPara *data, uint32_t val)
{
	uint8_t  ret = FALSE;
	
	
	if((val < data->Min) || (val > data->Max))
	{
		ret = TRUE;
	}
	else
	{
		data->Value = val;
	}
	
	
	return ( ret );
}



/*
 * @function: ParaConfig
 * @details : 参数赋值。
 * @input   : 1.para：数据指针。
              2.num：数据索引。
			  3.min：参数最小值。
			  4.max：参数最大值。
 * @output  : NULL
 * @return  : NULL
 */
void ParaConfig(struct tagPara *para, uint8_t num, uint16_t min, uint16_t max)
{
	para->Index = num;
	para->Min = min;
	para->Max = max;
}






/*
 * @function: ParaInit
 * @details : 参数初始化。
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void ParaInit(void)
{
	ParaConfig(&UserPara[CAN_ID], 0, 0xC9, 0xD0);
	
	ParaConfig(&UserPara[CAP_RCOUNT_CH0], 1, 0x06, 0xFF);
	ParaConfig(&UserPara[CAP_OFFSET_CH0], 2, 0x0000, 0xFFFF);
	ParaConfig(&UserPara[CAP_SETTLECOUNT_CH0], 3, 0x00, 0xFFFF);
	ParaConfig(&UserPara[CAP_CLOCK_DIVIDERS_CH0], 4, 0x00, 0xFFFF);
	ParaConfig(&UserPara[CAP_STATUS_CONFIG], 4, 0x00, 0xFFFF);
	ParaConfig(&UserPara[CAP_CONFIG], 6, 0x00, 0xFFFF);
	ParaConfig(&UserPara[CAP_MUX_CONFIG], 7, 0x00, 0xFFFF);
	ParaConfig(&UserPara[CAP_DRIVE_CURRENT_CH0], 8, 0x00, 0xFFFF);
	
	
	ParaConfig(&UserPara[LLD_SEN_FUN], 9, 0x00, 0x2F);
	
	
	ParaConfig(&UserPara[CAP_CON_THRESHOLD_H], 10, 0x00, 0xFFFF);
	ParaConfig(&UserPara[CAP_CON_THRESHOLD_L], 11, 0x00, 0xFFFF);
	ParaConfig(&UserPara[CAP_LEV_THRESHOLD_H], 12, 0x00, 0xFFFF);
	ParaConfig(&UserPara[CAP_LEV_THRESHOLD_L], 13, 0x00, 0xFFFF);
	ParaConfig(&UserPara[CAP_SET_GEAR], 14, 0x00, 0xFF);
	ParaConfig(&UserPara[CAP_SET_CONTIME], 15, 0x01, 0xFF);
	
	
	ParaConfig(&UserPara[CAP_DETE_OBJ], 16, 0x00, 6);
	ParaConfig(&UserPara[AIR_DETE_OBJ], 17, 0x00, 6);
	
	ParaConfig(&UserPara[AIR_ATMOS_NOISE], 18, 0x00, 0xFFFF);
	ParaConfig(&UserPara[AIR_RAKERATIO1_MIN], 19, 0x00, 0xFFFF);
	ParaConfig(&UserPara[AIR_RAKERATIO1_MAX], 20, 0x00, 0xFFFF);
	ParaConfig(&UserPara[AIR_RAKERATIO2_MIN], 21, 0x00, 0xFFFF);
	ParaConfig(&UserPara[AIR_RAKERATIO2_MAX], 22, 0x00, 0xFFFF);
	ParaConfig(&UserPara[AIR_ASP_LIQ_NOISE], 23, 0x00, 0xFFFF);
	ParaConfig(&UserPara[AIR_RAKERATIO3_MIN], 24, 0x00, 0xFFFF);
	ParaConfig(&UserPara[AIR_RAKERATIO3_MAX], 25, 0x00, 0xFFFF);
	
	ParaConfig(&UserPara[AIR_ABS_WAIT_DLY], 26, 0x00, 0xFFFF);
	ParaConfig(&UserPara[AIR_ABS_START_DLY], 27, 0x00, 0xFFFF);
	ParaConfig(&UserPara[AIR_ABS_END_DLY], 28, 0x00, 0xFFFF);
	
	ParaConfig(&UserPara[AIR_ABS_START_END], 29, 0x00, 0xFFFF);
	ParaConfig(&UserPara[AIR_DIS_START_END], 30, 0x00, 0xFFFF);
	
	
	Storage.Reserved = 0xFFFF;
}





/*
 * @function: FLASH_Init
 * @details : 初始化芯片
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void FLASH_Init(void)
{
	uint8_t     ret = 0;
	
	
	//设置参数上下限
	ParaInit();
	
//	//从Flash读取数据
//	ret = MemManInit(&StorageMan, StorageParaTab, 20, 10, ParaCallBackFun, FLASH_PageRead, FLASH_PageWrite, BSP_Crc8);	
//	if(0 == ret)
//	{
//		LimitPara();		
//	}
}



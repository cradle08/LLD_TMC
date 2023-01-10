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


//参数管理表格
const struct tagStorageTab StorageParaTab[] = {
//  默认值     变量名称                          变量类型
	//------------ Page0 --------------------//
	{0xFFFF,    &Storage.MachineNo,                        TYPE_UINT16},
	{0xFFFF,    &Storage.HardWareNo,                       TYPE_UINT16},
	{0xFFFF,    &Storage.SoftWareNo,                       TYPE_UINT16},
	{0xFFFF,    &Storage.PageCRC8,                         TYPE_UINT16},	
	
	
	//------------ Page1 --------------------//
	{0xFFFF,    &Storage.Reserved,                         TYPE_UINT16},
	{0xFFFF,    &Storage.Reserved,                         TYPE_UINT16},
	{0xFFFF,    &Storage.Reserved,                         TYPE_UINT16},
	{0xFFFF,    &Storage.PageCRC8,                         TYPE_UINT16},
	
	
	//必须保留。
	{NULL,      NULL,                                      TYPE_UINT16},
};





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
 * @function: ParaCallBackFun
 * @details : 回调函数
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void ParaCallBackFun( void )
{
}

/*
 * @function: FLASH_PageRead
 * @details : 读取以页作为单位大小的数据。
 * @input   : 1.buffer：存储读出数据的指针。
              2.start_addr：读取地址。
			  3.byte_num：读取数据长度。
 * @output  : NULL
 * @return  : 读取结果，0：成功；1：失败。
 */
uint8_t FLASH_PageRead(uint8_t *buffer, uint16_t start_addr, uint16_t byte_num)
{
	uint8_t    ret = 0;
	uint8_t    len = 0;
	uint16_t   *page_buff = NULL;
	
	
	len = byte_num / 2;
	page_buff = (uint16_t *)buffer;
	if((start_addr % 8) != 0)
	{
		return 1;
	}
	ret = STMFLASH_SeqRead(63, start_addr, page_buff, len);
	
	return (ret);
}


/*
 * @function: FLASH_PageWrite
 * @details : 写以页作为单位大小的数据。
 * @input   : 1.buffer：要写入数据的指针。
              2.start_addr：写入地址。
			  3.byte_num：写入数据长度，必须小于等于SPI_FLASH_PerWritePageSize。
 * @output  : NULL
 * @return  : 写结果，0：成功；1：失败。
 */
uint8_t FLASH_PageWrite(uint8_t *buffer, uint16_t start_addr, uint16_t byte_num)
{
	uint8_t    ret = 0;
	uint8_t    len = 0;
	uint16_t   *page_buff = NULL;
	
	
	len = byte_num / 2;
	page_buff = (uint16_t *)buffer;
	if((start_addr % 8) != 0)
	{
		return 1;
	}
	ret = STMFLASH_SeqWrite(63, start_addr, page_buff, len);
	
	
	return (ret);	
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
	ParaConfig(&UserPara[CAP_SET_CONTIME], 15, 0x06, 0xFF);
	
	
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
 * @function: LimitPara
 * @details : 限制参数范围
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void LimitPara( void )
{
	//Page0
	CheckUint16ParaValid(&Storage.MachineNo, 0, 0xFF, 0);
	CheckUint16ParaValid(&Storage.HardWareNo, 0, 0xFF, 0);
	CheckUint16ParaValid(&Storage.SoftWareNo, 0, 0xFF, 0);
	
	//Page1
	CheckUint16ParaValid(&UserPara[CAN_ID].Value, UserPara[CAN_ID].Min, UserPara[CAN_ID].Max, UserPara[CAN_ID].Min);
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
	
	//从Flash读取数据
	ret = MemManInit(&StorageMan, StorageParaTab, 20, 10, ParaCallBackFun, FLASH_PageRead, FLASH_PageWrite, BSP_Crc8);	
	if(0 == ret)
	{
		LimitPara();		
	}
}


/*
 * @function: GetStorageErr
 * @details : 获取存储设备是否有页故障
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
uint8_t GetStorageErr(void)
{
	uint8_t    ret = FALSE;
	uint8_t    page_index = 0;
	
	
	for(page_index = 0; page_index < StorageMan.UsePageNum; page_index++)
	{
		if(TRUE == GetMemPageIsErr(&StorageMan, page_index))
		{
			ret = TRUE;
			break;
		}
	}
	
	Storage.IsErr = ret;
	
	
	return (ret);
}



/*
 * @function: StorageManStage
 * @details : 参数管理
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void StorageManStage(void)
{
	GetStorageErr();
	
	
	switch(Storage.Stage)
	{
		case STORAGE_NULL:
		{
			Storage.Stage = STORAGE_CONFIG;
			Storage.StageRunTime = 0;
		}
		break;			
		
		case STORAGE_CONFIG:
		{
			Storage.Stage = STORAGE_RUN;
			Storage.StageRunTime = 0;		
		}
		break;

		case STORAGE_RUN:
		{
			//发现存储芯片有页故障
			if(TRUE == Storage.IsErr)
			{
				Storage.Stage = STORAGE_ERR;
				Storage.StageRunTime = 0;
			}
		}
		break;

		case STORAGE_ERR:
		{
			//页故障消失，回到正常阶段
			if(FALSE == Storage.IsErr)
			{
				Storage.Stage = STORAGE_RUN;
				Storage.StageRunTime = 0;
			}		
			
			//页故障持续10s，回到配置阶段
			if(Storage.StageRunTime > 10)
			{
				Storage.Stage = STORAGE_CONFIG;
				Storage.StageRunTime = 0;
			}			
		}
		break;
		
		default:
		{
		}
		break;
	}
	
	
	Accumulation16(&Storage.StageRunTime);
}


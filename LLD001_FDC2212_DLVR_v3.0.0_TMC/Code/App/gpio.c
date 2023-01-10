/*****************************************************************************
Copyright  : BGI
File name  : gpio.c
Description: gpio管脚
Author     : lmj
Version    : 1.0.0.0
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#include "gpio.h"
#include "include.h"



//定义变量---------------------------------------------------------------------//
struct tagDIMan         DIMan[DI_NUM];
struct tagTempSensor    TempSensor[TEMP_NUM];
struct tagDIMan         DipSW[DIP_NUM];


const static uint16_t TemperTab[] = {
	380,    //-20
	400,    //-19
	422,    //-18
	444,    //-17
	467,    //-16
	491,    //-15
	516,    //-14
	542,    //-13
	568,    //-12
	596,    //-11
	
	625,    //-10
	654,    //-9
	684,    //-8
	715,    //-7
	748,    //-6
	781,    //-5
	814,    //-4
	849,    //-3
	885,    //-2
	921,    //-1
	
	958,    //0
	996,    //1
	1035,   //2
	1075,   //3
	1115,   //4
	1156,   //5
	1197,   //6
	1239,   //7
	1282,   //8
	1325,   //9
	
	1369,   //10
	1413,   //11
	1457,   //12
	1502,   //13
	1547,   //14
	1592,   //15
	1638,   //16
	1683,   //17
	1729,   //18
	1775,   //19
	
	1821,   //20
	1866,   //21
	1912,   //22
	1958,   //23
	2003,   //24
	2048,   //25
	2093,   //26
	2137,   //27
	2181,   //28
	2225,   //29
	
	2269,   //30
	2311,   //31
	2354,   //32
	2396,   //33
	2437,   //34
	2478,   //35
	2518,   //36
	2558,   //37
	2597,   //38
	2635,   //39
	
	2673,   //40
	2710,   //41
	2746,   //42
	2782,   //43
	2816,   //44
	2851,   //45
	2884,   //46
	2917,   //47
	2949,   //48
	2980,   //49
	
	3011,   //50
	3041,   //51
	3070,   //52
	3099,   //53
	3126,   //54
	3154,   //55
	3180,   //56
	3206,   //57
	3231,   //58
	3255,   //59
	3279,   //60
	
	3302,   //61
	3325,   //62
	3347,   //63
	3368,   //64
	3389,   //65
	3409,   //66
	3429,   //67
	3448,   //68
	3466,   //69
	3484    //70
};




//定义函数---------------------------------------------------------------------//
/*
 * @function: CheckPhotosensor
 * @details : 读取TIP头复位信号
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void CheckPhotosensor(void)
{
	DIMan[DI_LIGHT_ELE].Status = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0);
}


/*
 * @function: LLDMonReadIO
 * @details : 上位机读IO
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
uint8_t LLDMonReadIO(uint8_t *buf, uint8_t num)
{
	uint16_t    ret = 0;
	
	
	buf[7] = DIMan[num].Status;
	
	
	return (ret);
}



/*
 * @function: LLD_OutPutPulse
 * @details : 根据探测液面结果，输出脉冲信号
 * @input   : 探测结果
 * @output  : NULL
 * @return  : 输入值
 */
uint8_t LLD_OutPutPulse(uint8_t sig)
{
	uint8_t    ret = sig;
	
	
	if(IO_HIGH == sig)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_1);
	}
	else
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_1);
	}
	
	
	return (ret);
}


/*
 * @function: FDC2212SetAddr
 * @details : 设置FDC2212通信地址
 * @input   : 设置电平
 * @output  : NULL
 * @return  : 输入值
 */
uint8_t FDC2212SetAddr(uint8_t sig)
{
	uint8_t    ret = sig;
	
	
	if(IO_HIGH == sig)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_1);
	}
	else
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_1);
	}
	
	
	return (ret);
}


/*
 * @function: LLD_ShutDownFDC2212Input
 * @details : 关闭输入信号
 * @input   : 设置电平
 * @output  : NULL
 * @return  : 输入值
 */
uint8_t FDC2212ShutDownInput(uint8_t sig)
{
	uint8_t    ret = sig;
	
	
	if(IO_HIGH == sig)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_5);
	}
	else
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_5);
	}
	
	
	return (ret);
}




/*
 * @function: DipSWInit
 * @details : 拨码开关初始化
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
uint8_t DipSWInit(void)
{
	uint8_t    ret = 0;
	
	
	DipSW[0].HighCnt = 255;
	DipSW[0].LowCnt = 255;
	DipSW[0].Status = 0;
	
	DipSW[1].HighCnt = 255;
	DipSW[1].LowCnt = 255;
	DipSW[1].Status = 0;
	
	DipSW[2].HighCnt = 255;
	DipSW[2].LowCnt = 255;
	DipSW[2].Status = 0;
	
	DipSW[3].HighCnt = 255;
	DipSW[3].LowCnt = 255;
	DipSW[3].Status = 0;
	
	
	return (ret);
}

/*
 * @function: DipCheck
 * @details : 检测DI
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
uint8_t DipCheck(struct tagDIMan *io, uint8_t state)
{
	uint8_t    ret = 0;
	
	
	if(IO_HIGH == state)
	{
		Accumulation8(&io->HighCnt);
		io->LowCnt = 0;
	}
	else
	{
		Accumulation8(&io->LowCnt);
		io->HighCnt = 0;
	}
	
	if(io->HighCnt > SW_PASS_TIME)
	{
		io->Status = IO_HIGH;
	}
	
	if(io->LowCnt > SW_PASS_TIME)
	{
		io->Status = IO_LOW;
	}
	
	
	return (ret);
}


/*
 * @function: DipSWCheck
 * @details : 检测拨码开关
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
uint8_t DipSWCheck(void)
{
	uint8_t    ret = 0;
	uint8_t    temp = 0;
	
	
	//SW1.1
	temp = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);
	DipCheck(&DipSW[0], temp);
	
	//SW1.2
	temp = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13);
	DipCheck(&DipSW[1], temp);
	
	//SW1.3
	temp = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14);
	DipCheck(&DipSW[2], temp);
	
	//SW1.4
	temp = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15);
	DipCheck(&DipSW[3], temp);
	
	
	if(DipSW[0].Status)
	{
		BITCLR(SoftSys.DipSW[0], 0);
	}
	else
	{
		BITSET(SoftSys.DipSW[0], 0);
	}
	
	if(DipSW[1].Status)
	{
		BITCLR(SoftSys.DipSW[0], 1);
	}
	else
	{
		BITSET(SoftSys.DipSW[0], 1);
	}
	
	if(DipSW[2].Status)
	{
		BITCLR(SoftSys.DipSW[0], 2);
	}
	else
	{
		BITSET(SoftSys.DipSW[0], 2);
	}
	
	if(DipSW[3].Status)
	{
		BITCLR(SoftSys.DipSW[0], 3);
	}
	else
	{
		BITSET(SoftSys.DipSW[0], 3);
	}
	
	
	
	return (ret);
}



/*
 * @function: MidScan
 * @details :  二分查找温度，此算法仅针对RT表按照降序---硬件设计为上拉电阻式设计
 * @input   : 1.source: 要查找的AD值。
              2.tabt：RT对应表TemperTab。
			  3.tabl：TemperTab表的大小。
			  4.tabdata：TemperTab表第一个数据大小。
			  5.ad_sort：表格ad值排序。如果表内的值从tabt[0]开始的往后的值是从小到大排列的 则传入值应该为ASC，否则为DESC。
			  6.data_sort：若表内的值从索引 0 代表的值（可能是温度/压力）开始的往后的值逐渐变大，则传入值应该为ASC，否则为DESC。
 * @output  : NULL
 * @return  : 温度值
 */
int16_t MidScan(uint16_t source, const uint16_t *tabt, uint8_t tabl, int16_t tabdata, uint8_t ad_sort, uint8_t data_sort)
{
	uint8_t    i = 0;
	uint8_t    min = 0, max = 0;
	uint16_t   k = 0, l = 0, query_max_cnt = 0;
	int16_t    result = 0, temp = 0;
	
	uint16_t   srck = 0;
	uint16_t   comk = 0;
	uint16_t   srcl = 0;
	uint16_t   coml = 0;
	uint16_t   query_cnt = 0;
	
	
	
	min = 0;
	max = tabl-1;
	query_cnt = tabl;
	
	
	//计算二分法查表最多查询次数query_max_cnt，防止死循环
	do
	{
		query_cnt = query_cnt / 2;
		query_max_cnt ++;
	}
	while(query_cnt > 0);
	
	
	
	//异常检测处理
	{
		if(DESC == ad_sort)
		{
			//异常处理，如果传入值不合法则直接退出
			if(source > tabt[0])
			{
				return (ERR_TEMP_MAX);
			}
			else if(source < tabt[tabl-1])
			{
				return (ERR_TEMP_MIN);
			}
		}
		else if(ASC == data_sort)
		{
			//异常处理，如果传入值不合法则直接退出
			if(source < tabt[0])
			{
				return (ERR_TEMP_MIN);
			}
			else if(source > tabt[tabl-1])
			{
				return (ERR_TEMP_MAX);
			}
		}
		else
		{
			return (ERR_TEMP_MAX);
		}
	}
	
	//查找过界则不再进行查找
	while(min <= max && query_max_cnt > 0)
	{
		//最多查query_max_cnt次就要退出循环
		if(query_max_cnt != 0)
		{
			query_max_cnt --;
		}
		i = min + max;
		i /= 2;
		k = *(tabt+i);
		
		if(DESC == ad_sort)
		{
			srck = source;
			comk = k;
		}
		else
		{
			srck = k;
			comk = source;
		}
		
		if ( srck <= comk )
		{
			l = *(tabt+i+1);
			
			if( DESC == ad_sort )
			{
				srcl = source;
				coml = l;
			}
			else
			{
				srcl = l;
				coml = source;
			}
			
			//计算相邻两点数据的斜率
			//然后根据斜率、ADC值计算温度
			if ( srcl >= coml )
			{
				result = source - k;
				temp = l - k;
				result *= 10;        //相邻数据相差1度
				result /= temp;
				temp = i*10;
				result += temp;
				
				break;
			}
			else
			{
				min = i;
			}
		}
		else
		{
			max = i;
		}
	}
	
	if(ASC == data_sort)
	{
		result += tabdata;
	}
	else
	{
		result = tabdata - result;
	}
	
	
	return result;
}


/*
 * @function: GetTemperature
 * @details : 开启ADC
 * @input   : sensor
 * @output  : NULL
 * @return  : NULL
 */
int16_t GetTemperature(struct tagADValue* objdev)
{
	int16_t    temprature;
	
	
	objdev->IsErr = 0;
	temprature = MidScan(objdev->OrigValue, TemperTab, RT_TABLE_NUM, RT_TEMP, ASC, ASC);
	if((ERR_TEMP_MAX == temprature) || (ERR_TEMP_MIN == temprature))
	{
		objdev->IsErr = 1;
	}
	else
	{
		objdev->IsErr = 0;
	}
	
	
	return temprature;
}


/*
 * @function: TempNTC
 * @details : 获取NTC温度
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
uint8_t TempNTC(void)
{
	uint8_t    ret = FALSE;
	int16_t    temp_dis = 0;
	float      lpf_ratio = 0.2;
	
	
	if(TRUE == TempSensor[TEMP_PIPE_IN].UpdateFlag)
	{
		TempSensor[TEMP_PIPE_IN].UpdateFlag = FALSE;
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		
		
		temp_dis = GetTemperature(&TempSensor[TEMP_PIPE_IN].AirPump);
		
		
		//一阶滤波
		temp_dis = LPF2(temp_dis, TempSensor[TEMP_PIPE_IN].AirPump.Value, lpf_ratio);
		TempSensor[TEMP_PIPE_IN].AirPump.Value = temp_dis;
		
		
		//滑动平均滤波
//		temp = Smoothfilter(TempSensor.AppFilterBuff, temp_dis, 6);
//		SmoothPipeline16(TempDataDis, temp_dis, TEMP_DIS_NUM);
	}
	
	return ret;
}


/*
 * @function: WriteTemp
 * @details : 填充通信缓存
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @output  : NULL
 * @return  : 0：成功；1：参数错误。
 */
uint8_t WriteTemp(uint8_t *buf, uint8_t num)
{
	uint8_t    ret = FALSE;
	
	
	return (ret);
}

/*
 * @function: ReadTemp
 * @details : 填充通信缓存
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @return  : NULL
 */
uint8_t ReadTemp(uint8_t *buf, uint8_t num)
{
	uint8_t    ret = 0;
	uint32_t   temp = 0xFFFF;
	
	
	//检测故障
	if(TEMP_PIPE_IN == num)
	{
		if(TRUE == TempSensor[TEMP_PIPE_IN].AirPump.IsErr)
		{
			buf[2] = ERR_TEMP1_FAILD;
		}
	}
	else if(TEMP_PIPE_OUT == num)
	{
		if(TRUE == TempSensor[TEMP_PIPE_OUT].AirPump.IsErr)
		{
			buf[2] = ERR_TEMP2_FAILD;
		}
	}
	
	//上传温度
	if(TEMP_PIPE_IN == num)
	{
		temp = TempSensor[TEMP_PIPE_IN].AirPump.Value;
	}
	else if(TEMP_PIPE_OUT == num)
	{
		temp = TempSensor[TEMP_PIPE_OUT].AirPump.Value;
	}
	
	buf[4] = temp>>24;
	buf[5] = temp>>16;
	buf[6] = temp>>8;
	buf[7] = temp;
	
	
	return (ret);
}

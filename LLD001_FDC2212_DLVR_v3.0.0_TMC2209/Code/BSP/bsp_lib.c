/*****************************************************************************
Copyright  : BGI
File name  : bsp_lib.c
Description: 公用函数库
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#include "bsp_lib.h" 
#include "include.h"



//定义变量---------------------------------------------------------------------//




/*
 * @function: BSP_Crc8
 * @details : CRC8校验
 * @input   : 1.frame: 被检验的数据。
              2.n: 被校验数据的长度。
 * @output  : NULL
 * @return  : 检验值。
 */
uint8_t BSP_Crc8(uint8_t *pframe, uint8_t len)
{
	uint8_t    i = 0;
	uint8_t    j = 0;
	uint8_t    carry_flag = 0;
	uint8_t    temp_crc = 0;
	uint8_t    crc = 0;
	
	
	crc = 0xff;
	for(i = 0; i < len; i ++)
	{
		crc = (uint8_t)(crc ^ pframe[i]);
		
		for(j = 0; j < 8; j ++)
		{
			temp_crc = crc;
			carry_flag = (uint8_t)(temp_crc & 0x01);
			crc >>= 1;
			
			if(carry_flag == 1)
			{
				crc ^= 0xe0;
			}
		}
	}
	
	return crc;
}

/*
 * @function: BSP_Crc16
 * @details : CRC16校验，长度不可为0。
 * @input   : 1.frame: 被检验的数据。
              2.len: 被校验数据的长度。
 * @output  : NULL
 * @return  : 检验值。
 */
uint16_t BSP_Crc16(uint8_t* frame, uint32_t len)
{
	uint32_t   i;
	uint16_t   j;
	uint16_t   carry_flag;
	uint16_t   crc;
	uint16_t   temp_crc;
	
	
	crc = 0xffff;
	for (i = 0; i < len; i++)
	{
		crc = crc ^ frame[i];
		
		for(j = 0; j < 8; j++)
		{
			temp_crc = crc;
			carry_flag = temp_crc & 0x0001;
			crc = crc >> 1;
			
			if(carry_flag == 1)
			{
				crc = crc ^ 0xa001;        //1010 0000 0000 0001 (Poly的逆向值)
			}
		}
	}

//	temp = crc & 0xff00;
//	temp >>= 8;
//	*high_data =(uint8_t) temp;
//	temp = crc & 0x00ff;
//	*low_data =(uint8_t)temp;


	return crc;
}

/*
 * @function: Smoothfilter
 * @details : 滑动滤波。把数组总的数去掉最大最小值后取平均，并把所有的数据向后移动一位.
 * @input   : 1.buf: 数据缓存。
              2.data: 数据。
              3.len: 需要求平均的个数。
 * @output  : NULL
 * @return  : 计算结果。
 */
uint16_t Smoothfilter(uint16_t *buf, uint16_t data, uint8_t len)
{
	uint8_t    i = 0;
	uint16_t   max = 0;
	uint16_t   min = 0xFFFF;	
	uint32_t   ave_value = 0;
	
	
	if((len <= 2) || (len > 10))
	{		
		return 0;
	}
	
	
	//滑动存放数据
	for(i = (len-1); i > 0; i--)
	{
		buf[i] = buf[i-1];
	}
	buf[0] = data;
	
	
	//计算平均值
	for(i = 0; i < len; i++)
	{
		if(buf[i] > max)
		{
			max = buf[i];
		}
		
		if(buf[i] < min)
		{
			min = buf[i];
		}
		ave_value += buf[i];
	}
	ave_value -= max;
	ave_value -= min;
	ave_value = ave_value / (len - 2);
	
	
	return (ave_value);
}


/*
 * @function: Smoothfilter32
 * @details : 滑动滤波。把数组总的数去掉最大最小值后取平均，并把所有的数据向后移动一位.
 * @input   : 1.buf: 数据缓存。
              2.data: 数据。
              3.len: 需要求平均的个数。
 * @output  : NULL
 * @return  : 计算结果。
 */
uint32_t Smoothfilter32(uint32_t *buf, uint32_t data, uint8_t len)
{
	uint8_t    i = 0;
	uint32_t   max = 0;
	uint32_t   min = 0xFFFFFFFF;
	uint64_t   ave_value = 0;
	
	
	if((len <= 2) || (len > 10))
	{
		return 0;
	}
	
	
	//滑动存放数据
	for(i = (len-1); i > 0; i--)
	{
		buf[i] = buf[i-1];
	}
	buf[0] = data;
	
	
	//计算平均值
	for(i = 0; i < len; i++)
	{
		if (buf[i] > max)
		{
			max = buf[i];
		}
		
		if(buf[i] < min)
		{
			min = buf[i];
		}
		ave_value += buf[i];
	}
	ave_value -= max;
	ave_value -= min;
	ave_value = ave_value / (len - 2);
	
	
	return (ave_value);
}


/*
 * @function: LPF1
 * @details : 一阶低通滤波算法。
              取本次采样值的ratio%，加上上一次采样值的1-ratio%。
 * @input   : 1.data: 数据。
              2.pre_data: 上一个数据。
			  3.ratio: 滤波系数，当前数据的权重,0.0-1.0之间。
 * @output  : NULL
 * @return  : 计算结果。
 */
uint32_t LPF1(uint32_t data, uint32_t pre_data, float ratio)
{
	uint32_t    temp_data = 0;
	
	
	temp_data = (data * ratio) + (1 - ratio) * pre_data;
	
	
	return (temp_data);
}


/*
 * @function: LPF2
 * @details : 一阶低通滤波算法。
              取本次采样值的ratio%，加上上一次采样值的1-ratio%。
 * @input   : 1.data: 数据。
              2.pre_data: 上一个数据。
			  3.ratio: 滤波系数，当前数据的权重,0.0-1.0之间。
 * @output  : NULL
 * @return  : 计算结果。
 */
uint32_t LPF2(int32_t data, int32_t pre_data, float ratio)
{
	int32_t    temp_data = 0;
	
	
	temp_data = (data * ratio) + (1 - ratio) * pre_data;
	
	
	return (temp_data);
}


/*
 * @function: SmoothPipeline16
 * @details : 管道
 * @input   : 1.buf: 数据缓存。
              2.data: 数据。
              3.len: 数据个数。
 * @output  : NULL
 * @return  : 计算结果。
 */
uint8_t SmoothPipeline16(uint16_t *buf, uint16_t data, uint8_t len)
{
	uint8_t    ret = 0;
	uint8_t    i = 0;
	
	
	//滑动存放数据
	for(i = (len-1); i > 0; i--)
	{
		buf[i] = buf[i-1];
	}
	buf[0] = data;
	
	
	return (ret);
}


/*
 * @function: SmoothPipeline32
 * @details : 管道
 * @input   : 1.buf: 数据缓存。
              2.data: 数据。
              3.len: 数据个数。
 * @output  : NULL
 * @return  : 计算结果。
 */
uint8_t SmoothPipeline32(uint32_t *buf, uint32_t data, uint8_t len)
{
	uint8_t    ret = 0;
	uint8_t    i = 0;
	
	
	//滑动存放数据
	for(i = (len-1); i > 0; i--)
	{
		buf[i] = buf[i-1];
	}
	buf[0] = data;
	
	
	return (ret);
}


/*
 * @function: Accumulation8
 * @details : 累加u8变量
 * @input   : 1.var: 自增的变量。
 * @output  : NULL
 * @return  : 检验值。
 */
void Accumulation8(uint8_t* var)
{
	if((*var) < 0xFF)
	{
		(*var)++;
	}
}

/*
 * @function: DecreaseUint8
 * @details : 递减u8变量
 * @input   : 1.var: 递减的变量。
 * @output  : NULL
 * @return  : 检验值。
 */
void DecreaseUint8(uint8_t* var)
{
	if((*var) > 0)
	{
		(*var)--;
	}
}

/*
 * @function: Accumulation16
 * @details : 累加u16变量
 * @input   : 1.var: 自增的变量。
 * @output  : NULL
 * @return  : 检验值。
 */
void Accumulation16(uint16_t* var)
{
	if((*var) < 0xFFFF)
	{
		(*var)++;
	}
}

/*
 * @function: DecreaseUint16
 * @details : 递减u16变量
 * @input   : 1.var: 递减的变量。
 * @output  : NULL
 * @return  : 检验值。
 */
void DecreaseUint16(uint16_t* var)
{
	if((*var) > 0)
	{
		(*var)--;
	}
}

/*
 * @function: Accumulation16
 * @details : 累加u16变量
 * @input   : 1.var: 自增的变量。
 * @output  : NULL
 * @return  : 检验值。
 */
void Accumulation32(uint32_t* var)
{
	if((*var) < 0xFFFFFFFF)
	{
		(*var)++;
	}
}


/*
 * @function: GenDelay_nop
 * @details : 普通延时nop级别函数，延时值越大，偏差越大。
 * @input   : 1.ntimer: 延时us时间。
 * @output  : NULL
 * @return  : 检验值。
 */
void GenDelay_nop(uint32_t ntimer)
{
	uint32_t   cnt = 0;
	
	for(cnt = 0; cnt < ntimer; cnt++)
	{
		__NOP();
	}
}


/*
 * @function: CheckUint16ParaValid
 * @details : 检测参数是否在合法范围内。
 * @input   : 1.check_val：被检测的值。
              2.min：被检测的值下限值。
              3.max：被检测的值最大值。	 			   
              4.def_val：默认值。	 
			  5.var_type：变量类型 
 * @output  : NULL
 * @return  : 是否在合法范围内，FALSE：不在； TRUE：在。
 */
uint8_t CheckUint16ParaValid(uint16_t *check_val, uint16_t min, uint16_t max, uint16_t def_val)
{
	uint8_t  ret = TRUE;
	
	
	if((*check_val < min) || (*check_val > max))
	{
		*check_val = def_val;
		
		ret = FALSE;
	}
	
	return ( ret );
}


/*
 * @function: ArrayClear
 * @details : 清除数组
 * @input   : 1.buf: 数据。
              2.len: 数据个数。
 * @output  : NULL
 * @return  : 计算结果。
 */
uint8_t Array32Clear(uint32_t *buf, uint8_t len)
{
	uint8_t    ret = 0;
	uint8_t    i = 0;
	
	
	//清除数组
	for(i = 0; i < len; i++)
	{
		buf[i] = 0;
	}
	
	
	return (ret);
}






/*
 * @function: QueueLoopInit
 * @details : 循环队列初始化
 * @input   : 1.queue：队列。
              2.buf：队列缓存。
			  3.len：队列长度。
 * @output  : NULL
 * @return  : 0：成功；其他：。
 */
uint8_t QueueLoopInit(struct tagQueueLoop *queue, void *buf, uint16_t len)
{
	uint8_t    ret = 0;
	
	
	queue->Front = 0;
	queue->Rear = 0;
	queue->Len = len;
	queue->DataNum = 0;
	queue->Buf = buf;
	
	
	return (ret);
}

/*
 * @function: QueueLoopClear
 * @details : 循环队列清空
 * @input   : 1.queue：队列。
 * @output  : NULL
 * @return  : 0：成功；其他：。
 */
uint8_t QueueLoopClear(struct tagQueueLoop *queue)
{
	uint8_t    ret = 0;
	
	
	queue->Front = 0;
	queue->Rear = 0;
	queue->DataNum = 0;
	
	return (ret);
}


/*
 * @function: QueueLoopDataNum
 * @details : 循环队列剩余数据个数
 * @input   : 1.queue：队列。
 * @output  : NULL
 * @return  : 数据个数。
 */
uint16_t QueueLoopDataNum(struct tagQueueLoop *queue)
{
	uint16_t    ret = 0;
	
	
	//计算队列剩余数据个数
	queue->DataNum = queue->Rear - queue->Front;
	queue->DataNum = queue->DataNum + queue->Len;
	queue->DataNum = queue->DataNum % queue->Len;
	ret = queue->DataNum;
	
	
	return (ret);
}


/*
 * @function: QueueLoopIsEmpty
 * @details : 循环队列是否为空
 * @input   : 1.queue：队列。
 * @output  : NULL
* @return  : 0：为空；其他：不为空。
 */
uint16_t QueueLoopIsEmpty(struct tagQueueLoop *queue)
{
	uint16_t    ret = 1;
	
	
	//计算队列剩余数据个数
	if(queue->Rear == queue->Front)
	{
		ret = 0;
	}
	
	
	return (ret);
}


/*
 * @function: QueueLoopIsFull
 * @details : 循环队列是否满栈
 * @input   : 1.queue：队列。
 * @output  : NULL
* @return  : 0：不满；其他：不为空。
 */
uint16_t QueueLoopIsFull(struct tagQueueLoop *queue)
{
	uint16_t    ret = 0;
	int16_t     temp = 1;
	
	
	temp = queue->Rear + 1;
	temp = temp % queue->Len;
	
	if(queue->Front == temp)
	{
		ret = 1;
	}
	
	
	return (ret);
}

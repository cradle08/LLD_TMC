/*****************************************************************************
Copyright  : BGI
File name  : monitor.c
Description: 上位机监控
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#include "monitor_can.h" 
#include "include.h"
#include "stdlib.h"
#include "time.h"

//
#include "event.h"
#include "param.h"

//定义变量---------------------------------------------------------------------//
struct tagMonCan    MonCan;




//定义函数---------------------------------------------------------------------//
/*
 * @function: QueueLoopPop
 * @details : 循环队列数据出栈
 * @input   : 1.queue：队列。
              2.des_buf：目的地缓存。
              3.type_len：数据类型。
 * @output  : NULL
 * @return  : 0：成功；1：缓存数据为空。
 */
uint16_t QueueLoopCanPop(struct tagQueueLoop *queue, uint8_t *des_buf)
{
	uint16_t   ret = 0;
	uint8_t    i = 0;
	
	
	if(queue->Rear == queue->Front)
	{
		ret = 1;
	}
	else
	{
		for(i = 0; i < CAN_QUEUE_LOOP_WIDTH; i++)
		{
			des_buf[i] = queue->Buf[(queue->Front * CAN_QUEUE_LOOP_WIDTH) + i];
		}
		
		queue->Front++;
		queue->Front = queue->Front % queue->Len;
	}
	
	
	return (ret);
}

/*
 * @function: QueueLoopInsert
 * @details : 循环队列插入数据
 * @input   : 1.queue：监控管理指针。
              2.src_data：源数据。
 * @output  : NULL
 * @return  : 0：成功；其他：故障指令。
 */
uint16_t QueueLoopCanInsert(struct tagQueueLoop *queue, uint8_t *src_data)
{
	uint16_t   ret = 0;
	uint16_t   temp = 0;
	uint8_t    i = 0;
	
	
	temp = QueueLoopIsFull(queue);
	if(temp)
	{
		ret = 1;
	}
	else
	{
		for(i = 0; i < CAN_QUEUE_LOOP_WIDTH; i++)
		{
			queue->Buf[(queue->Rear * CAN_QUEUE_LOOP_WIDTH) + i] = src_data[i];
		}
		
		queue->Rear++;
		queue->Rear = queue->Rear % queue->Len;
	}
	
	
	return (ret);
}





/*
 * @function: CAN_ParaInit
 * @details : Can初始化参数
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void CAN_ParaInit(void)
{
	QueueLoopInit(&MonCan.RxQueue, MonCan.RxBuf, CAN_QUEUE_LOOP_LEN);
}


///*
// * @function: CAN_MonInit
// * @details : Can监控初始化
// * @input   : NULL
// * @output  : NULL
// * @return  : NULL
// */
//uint8_t CAN_MonInit(void)
//{
//	extern __IO GlobalParam_t g_tGlobalParam; 
//	uint8_t     ret = FALSE;
//	uint16_t    can_id = 0, can_id_motor = 0;
//	uint8_t     bps = 0;
//	uint8_t     offset = 0;
//	
//	
//	
//	//寻找Can通信故障的原因，实测仍然出现通信故障。
////	if(SWSysTimeTick.PowerOnS >= 5)
////	{
////		return (ret);
////	}
//	
//	//设置Can ID
//	offset = SoftSys.DipSW[0] & 0x07;
//	can_id = CAN_ID_MIM + offset;
//	if((can_id < CAN_ID_MIM) || (can_id > CAN_ID_MAX))
//	{
//		//默认通信地址
//		can_id = CAN_ID_MIM;
//	}
//	
//	//设置Can波特率模式
//	offset = SoftSys.DipSW[0] & 0x08;
//	if(0x00 == offset)
//	{
//		bps = CAN_BPS_MODE_1M;
//	}
//	else if(0x08 == offset)
//	{
//		bps = CAN_BPS_MODE_500K;
//	}
//	
//	
//	if((can_id != MonCan.Confg.ModuleID) || (bps != MonCan.Confg.BpsMode))
//	{
//		CAN_ITConfig(CAN1, CAN_IT_FMP0, DISABLE);
//		
//		CAN_ParaInit();
//		QueueLoopClear(&MonCan.RxQueue);
//		
//		MonCan.Confg.BpsMode = bps;
//		MonCan.Confg.ModuleID = can_id;
//		MonCan.Confg.BroadcastID = 0x7FE;
//		Init_RxMes(&MonCan.RxMsg);
//		
//		CAN_GPIO_Config();
//		CAN_Config(CAN1, &MonCan.Confg);
//		CAN_NVIC_Config();
//	}
//	return ret;
//}


/*
 * @function: CAN_MonInit
 * @details : Can监控初始化
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
uint8_t CAN_MonInit(void)
{
	extern __IO GlobalParam_t g_tGlobalParam; 
	uint8_t     ret = FALSE;
//	uint16_t    can_id = 0, can_id_motor = 0;
//	uint8_t     bps = 0;
//	uint8_t     offset = 0;

	CAN_ITConfig(CAN1, CAN_IT_FMP0, DISABLE);
	
	CAN_ParaInit();
	QueueLoopClear(&MonCan.RxQueue);
	Init_RxMes(&MonCan.RxMsg);
	
	CAN_NVIC_Config();
	CAN_GPIO_Config();
	CAN_Config(CAN1);
	
	
	return ret;
}


//uint8_t CAN_MonInit(void)
//{
//	uint8_t     ret = FALSE;
//	
//	
//	CAN_ParaInit();
//	QueueLoopClear(&MonCan.RxQueue);
//	
//	MonCan.Confg.BpsMode = CAN_BPS_MODE_1M;
//	MonCan.Confg.ModuleID = 201;
//	MonCan.Confg.BroadcastID = 0x7FE;
//	Init_RxMes(&MonCan.RxMsg);
//	
//	CAN_NVIC_Config();
//	CAN_GPIO_Config();
//	CAN_Config(CAN1, &MonCan.Confg);
//	
//	
//	return ret;
//}



/*
 * @function: CanIRQRecv
 * @details : 接收报文。
 * @input   : NULL
 * @output  : NULL
 * @return  : 接收报文的结果，0：失败；1：成功。
 */
uint8_t CanIRQRecv(void)
{
	extern __IO LLDParam_t g_tLLDParam;
	extern __IO GlobalParam_t 	 g_tGlobalParam;
	uint8_t     ret = FALSE;
	uint8_t     index = 0;
	uint8_t     rly1 = FALSE;
	uint8_t     rly2 = FALSE;
	uint16_t    result = FALSE;
	CanRxMsg    rx_msg;
	
	
	//从邮箱中读出报文
	CAN_Receive(CAN1, CAN_FIFO0, &rx_msg);
	
	//比较ID
	if((rx_msg.StdId == g_tLLDParam.CanConfig.ModuleID) && (CAN_ID_STD == rx_msg.IDE) && (8 == rx_msg.DLC))
	{
		//接收成功
		rly1 = TRUE;
		MonCan.ReceFinish = TRUE;
	}
	else if((rx_msg.StdId == LLD_CAN_BROADCAST_ID_MOTOR) && (CAN_ID_STD == rx_msg.IDE) && (8 == rx_msg.DLC))
	{
		//接收成功
		rly2 = TRUE;
		MonCan.ReceBroadcastFinish = TRUE;	
	}
	else if(((rx_msg.StdId == g_tGlobalParam.ulRecvCanID) || (rx_msg.StdId == CAN_BROADCAST_ID_MOTOR)) && (CAN_ID_STD == rx_msg.IDE) && (8 == rx_msg.DLC))
	{
		//接收成功	
		SysEvent_t *ptSysEvent = NULL;
		ptSysEvent = SysEventAlloc();
		if(NULL != ptSysEvent)
		{
			ptSysEvent->eMsgType = MSG_TYPE_CAN;
			memmove((void*)ptSysEvent->tMsg.tMsgCan.ucaBuffer, (void*)rx_msg.Data, CAN_MSG_DATA_LENGTH);
			ptSysEvent->tMsg.tMsgCan.ulRecvCanID = rx_msg.StdId;
			SysEventPut(ptSysEvent, 0);
		}
	}
	
	//装进缓存
	if(TRUE == rly1)
	{
//		//暂停使用Can接收队列。
//		for(index = 0; index < CAN_QUEUE_LOOP_WIDTH; index++)
//		{
//			MonCan.RxMsg.Data[index] = rx_msg.Data[index];
//		}
		
		
		//使用Can接收队列。
		result = QueueLoopCanInsert(&MonCan.RxQueue, rx_msg.Data);
		//入队成功
//		if(0 == result)
//		{
//			;
//		}
	}
	
	
	return ret;
}














/*
 * @function: MonOperateIO
 * @details : 上位机操作IO。
 * @input   : 1.mon：监控管理指针。
              2.err：故障信息。
 * @output  : NULL
 * @return  : 0：成功；其他：故障指令。
 */
uint8_t MonOperateIO(struct tagMonCan *mon, uint8_t err)
{
	uint8_t    ret = ERR_CAN_NULL;
	
	
	//故障指令或操作码，不进行任何操作
	if(ERR_CAN_NULL != err)
	{
		return (err);
	}
	
	//上位机写操作
	if(OPE_WRITE == mon->CanMess.OpeCode)
	{
	}
	
	
	return (ret);
}

/*
 * @function: MonOperateIOReply
 * @details : 应答上位机读写IO。
 * @input   : 1.mon：管理指针。
              2.buff：缓存。
 * @output  : NULL
 * @return  : 应答状态。
 */
uint8_t MonOperateIOReply(struct tagMonCan *mon, uint8_t *buff)
{
	uint8_t    ret = CAN_OPE_REPLY;
	
	
	LLDMonReadIO(buff, 0);
	
	
	return (ret);
}


/*
 * @function: MonTemp
 * @details : 上位机读取温度。
 * @input   : 1.mon：监控管理指针。
              2.err：故障信息。
 * @output  : NULL
 * @return  : 0：成功；其他：故障指令。
 */
uint8_t MonTemp(struct tagMonCan *mon, uint8_t err)
{
	uint8_t    ret = ERR_CAN_NULL;
//	uint16_t   w_buf[4] = {0};
	
	
	//故障指令或操作码，不进行任何操作
	if(ERR_CAN_NULL != err)
	{
		return (err);
	}
	
	//上位机写操作
	if(OPE_WRITE == mon->CanMess.OpeCode)
	{
	}
	
	
	return (ret);
}

/*
 * @function: MonTempReply
 * @details : 应答上位机读温度。
 * @input   : 1.mon：管理指针。
              2.buff：缓存。
 * @output  : NULL
 * @return  : 应答状态。
 */
uint8_t MonTempReply(struct tagMonCan *mon, uint8_t *buff)
{
	uint8_t    ret = CAN_OPE_REPLY;
	
	
	ReadTemp(buff, mon->CanMess.DevNo);
	
	
	return(ret);
}


/*
 * @function: MonConfigAirSen
 * @details : 上位机配置气压式传感器参数。
 * @input   : 1.mon：监控管理指针。
              2.err：故障信息。
 * @output  : NULL
 * @return  : 0：成功；其他：故障指令。
 */
uint8_t MonConfigAirSen(struct tagMonCan *mon, uint8_t err)
{
	uint8_t    ret = 0;
	
	
	//故障指令或操作码，不进行任何操作
	if(ERR_CAN_NULL != err)
	{
		return (err);
	}
	
	//上位机写操作
	if(OPE_WRITE == mon->CanMess.OpeCode)
	{
	}
	
	
	return (ret);
}

/*
 * @function: MonConfigAirSenReply
 * @details : 应答上位机读写气压传感器配置。
 * @input   : 1.mon：管理指针。
              2.buff：缓存。
 * @output  : NULL
 * @return  : 应答状态。
 */
uint8_t MonConfigAirSenReply(struct tagMonCan *mon, uint8_t *buff)
{
	uint8_t    ret = CAN_OPE_REPLY;
	
	
	ReadAirSenReg(buff, 0);
	
	
	return(ret);
}


/*
 * @function: MonSelectSen
 * @details : 上位机启用传感器参数。
 * @input   : 1.mon：监控管理指针。
              2.err：故障信息。
 * @output  : NULL
 * @return  : 0：成功；其他：故障指令。
 */
uint8_t MonSelectSen(struct tagMonCan *mon, uint8_t err)
{
	uint8_t    ret = 0;
	
	
	
	//故障指令或操作码，不进行任何操作
	if(ERR_CAN_NULL != err)
	{
		return (err);
	}
	
	//上位机写操作
	if(OPE_WRITE == mon->CanMess.OpeCode)
	{
		//赋值操作	
		WriteSeleSen(mon->RxMsg.Data, 0);
	}
	
	
	return (ret);
}

/*
 * @function: MonSelectSen
 * @details : 应答上位机读取启用传感器。
 * @input   : 1.mon：管理指针。
              2.buff：缓存。
 * @output  : NULL
 * @return  : 应答状态。
 */
uint8_t MonSelectSenReply(struct tagMonCan *mon, uint8_t *buff)
{
	uint8_t    ret = CAN_OPE_REPLY;
	
	
	ReadSelectSen(buff, 0);
	
	
	return(ret);
}


/*
 * @function: MonReadLLD
 * @details : 上位机读液面探测数据。
 * @input   : 1.mon：监控管理指针。
              2.err：故障信息。
 * @output  : NULL
 * @return  : 0：成功；其他：故障指令。
 */
uint8_t MonReadLLD(struct tagMonCan *mon, uint8_t err)
{
	uint8_t    ret = 0;
	
	
	//故障指令或操作码，不进行任何操作
	if(ERR_CAN_NULL != err)
	{
		return (err);
	}
	
	//上位机写操作
	if(OPE_WRITE == mon->CanMess.OpeCode)
	{	
	}	
	
	
	return (ret);
}

/*
 * @function: MonReadLLDReply
 * @details : 应答上位机读液面探测数据。
 * @input   : 1.mon：管理指针。
              2.buff：缓存。
 * @output  : NULL
 * @return  : 应答状态。
 */
uint8_t MonReadLLDReply(struct tagMonCan *mon, uint8_t *buff)
{
	uint8_t    ret = CAN_OPE_REPLY;
	
	
	//阻塞应答
	if(MonCan.AckBlockTime < CAN_ACK_BLOCK_WAIT)
	{
		if(TRUE == AirSenPara.CommAckBlock)
		{
			return (CAN_OPE_WAIT);
		}
	}

	
	ReadLLDResult(buff, 0);
	
	
	return(ret);
}


/*
 * @function: MonRead204
 * @details : 预留
 * @input   : 1.mon：监控管理指针。
              2.err：故障信息。
 * @output  : NULL
 * @return  : 0：成功；其他：故障指令。
 */
uint8_t MonRead204(struct tagMonCan *mon, uint8_t err)
{
	uint8_t    ret = 0;
	
	
	//故障指令或操作码，不进行任何操作
	if(ERR_CAN_NULL != err)
	{
		return (err);
	}

	//上位机写操作
	if(OPE_WRITE == mon->CanMess.OpeCode)
	{	
	}	
	
	
	return (ret);	
}

/*
 * @function: MonRead204Reply
 * @details : 
 * @input   : 1.mon：管理指针。
              2.buff：缓存。
 * @output  : NULL
 * @return  : 应答状态。
 */
uint8_t MonRead204Reply(struct tagMonCan *mon, uint8_t *buff)
{
	uint8_t    ret = CAN_OPE_REPLY;
	
	
	return(ret);
}


/*
 * @function: MonRead205
 * @details : 上位机读电容数据
 * @input   : 1.mon：监控管理指针。
              2.err：故障信息。
 * @output  : NULL
 * @return  : 0：成功；其他：故障指令。
 */
uint8_t MonRead205(struct tagMonCan *mon, uint8_t err)
{
	uint8_t    ret = 0;
	
	
	//故障指令或操作码，不进行任何操作
	if(ERR_CAN_NULL != err)
	{
		return (err);
	}
	
	//上位机写操作
	if(OPE_WRITE == mon->CanMess.OpeCode)
	{	
	}
	
	
	return (ret);
}

/*
 * @function: MonRead205Reply
 * @details : 
 * @input   : 1.mon：管理指针。
              2.buff：缓存。
 * @output  : NULL
 * @return  : 应答状态。
 */
uint8_t MonRead205Reply(struct tagMonCan *mon, uint8_t *buff)
{
	uint8_t    ret = CAN_OPE_REPLY;
	
	
	
	return(ret);
}


/*
 * @function: MonRead206
 * @details : 预留
 * @input   : 1.mon：监控管理指针。
              2.err：故障信息。
 * @output  : NULL
 * @return  : 0：成功；其他：故障指令。
 */
uint8_t MonRead206(struct tagMonCan *mon, uint8_t err)
{
	uint8_t    ret = 0;
	
	
	//故障指令或操作码，不进行任何操作
	if(ERR_CAN_NULL != err)
	{
		return (err);
	}
	
	//上位机写操作
	if(OPE_WRITE == mon->CanMess.OpeCode)
	{
	}
	
	
	return (ret);
}

/*
 * @function: MonRead206Reply
 * @details : 
 * @input   : 1.mon：管理指针。
              2.buff：缓存。
 * @output  : NULL
 * @return  : 应答状态。
 */
uint8_t MonRead206Reply(struct tagMonCan *mon, uint8_t *buff)
{
	uint8_t    ret = CAN_OPE_REPLY;
	
	
	return(ret);
}


/*
 * @function: MonCapLLDPara1
 * @details : 上位机设置电容探测档位
 * @input   : 1.mon：监控管理指针。
              2.err：故障信息。
 * @output  : NULL
 * @return  : 0：成功；其他：故障指令。
 */
uint8_t MonCapLLDPara1(struct tagMonCan *mon, uint8_t err)
{
	uint8_t    ret = 0;
	
	
	//故障指令或操作码，不进行任何操作
	if(ERR_CAN_NULL != err)
	{
		return (err);
	}
	
	//上位机写操作
	if(OPE_WRITE == mon->CanMess.OpeCode)
	{
		//赋值操作
		WriteCLLDPara1(mon->RxMsg.Data, 0xFF);
	}
	
	
	return (ret);
}

/*
 * @function: MonCapLLDPara1Reply
 * @details : 应答上位机读设置电容探测灵敏度档位。
 * @input   : 1.mon：管理指针。
              2.buff：缓存。
 * @output  : NULL
 * @return  : 应答状态。
 */
uint8_t MonCapLLDPara1Reply(struct tagMonCan *mon, uint8_t *buff)
{
	uint8_t    ret = CAN_OPE_REPLY;
	
	
	ReadCLLDPara1(buff, 0);
	
	
	return(ret);
}


/*
 * @function: MonRead208
 * @details : 预留
 * @input   : 1.mon：监控管理指针。
              2.err：故障信息。
 * @output  : NULL
 * @return  : 0：成功；其他：故障指令。
 */
uint8_t MonRead208(struct tagMonCan *mon, uint8_t err)
{
	uint8_t    ret = 0;
	
	
	//故障指令或操作码，不进行任何操作
	if(ERR_CAN_NULL != err)
	{
		return (err);
	}
	
	//上位机写操作
	if(OPE_WRITE == mon->CanMess.OpeCode)
	{
	}
	
	
	return (ret);
}

/*
 * @function: MonRead208Reply
 * @details : 应答上位机读新增气压探测液面参数。
 * @input   : 1.mon：管理指针。
              2.buff：缓存。
 * @output  : NULL
 * @return  : 应答状态。
 */
uint8_t MonRead208Reply(struct tagMonCan *mon, uint8_t *buff)
{
	uint8_t    ret = CAN_OPE_REPLY;
	
	
	return(ret);
}


/*
 * @function: MonCapLLDPara3
 * @details : 上位机读写电容探测参数
 * @input   : 1.mon：监控管理指针。
              2.err：故障信息。
 * @output  : NULL
 * @return  : 0：成功；其他：故障指令。
 */
uint8_t MonCapLLDPara3(struct tagMonCan *mon, uint8_t err)
{
	uint8_t    ret = 0;
	
	
	//故障指令或操作码，不进行任何操作
	if(ERR_CAN_NULL != err)
	{
		return (err);
	}
	
	//上位机写操作
	if(OPE_WRITE == mon->CanMess.OpeCode)
	{	
		WriteCLLDPara3(mon->RxMsg.Data, 0);
	}
	
	
	return (ret);
}

/*
 * @function: MonCapLLDPara3Reply
 * @details : 应答上位机设置电容探测液面参数。
 * @input   : 1.mon：管理指针。
              2.buff：缓存。
 * @output  : NULL
 * @return  : 应答状态。
 */
uint8_t MonCapLLDPara3Reply(struct tagMonCan *mon, uint8_t *buff)
{
	uint8_t    ret = CAN_OPE_REPLY;
	
	
	//阻塞应答
//	if(CP_COMM_NORMAL != CapSenPara.ResStatu)
//	{
//		return (CAN_OPE_WAIT);
//	}
	
	ReadCLLDPara3(buff, 0);
	
	
	return(ret);
}


/*
 * @function: MonReadCapVal
 * @details : 上位读电容映射数据
 * @input   : 1.mon：监控管理指针。
              2.err：故障信息。
 * @output  : NULL
 * @return  : 0：成功；其他：故障指令。
 */
uint8_t MonReadCapVal(struct tagMonCan *mon, uint8_t err)
{
	uint8_t    ret = 0;
	
	
	//故障指令或操作码，不进行任何操作
	if(ERR_CAN_NULL != err)
	{
		return (err);
	}
	
	//上位机写操作
	if(OPE_WRITE == mon->CanMess.OpeCode)
	{
		WriteCapMapVal(mon->RxMsg.Data, 0);
	}
	
	
	return (ret);
}

/*
 * @function: MonReadCapValReply
 * @details : 应答上位机读取电容值。
 * @input   : 1.mon：管理指针。
              2.buff：缓存。
 * @output  : NULL
 * @return  : 应答状态。
 */
uint8_t MonReadCapValReply(struct tagMonCan *mon, uint8_t *buff)
{
	uint8_t    ret = CAN_OPE_REPLY;
	
	
	ReadCapMapVal(buff, 0);
	
	
	return(ret);
}


/*
 * @function: MonAirLLDPara1
 * @details : 上位机设置气压探测参数1
 * @input   : 1.mon：监控管理指针。
              2.err：故障信息。
 * @output  : NULL
 * @return  : 0：成功；其他：故障指令。
 */
uint8_t MonAirLLDPara1(struct tagMonCan *mon, uint8_t err)
{
	uint8_t    ret = 0;
	
	
	//故障指令或操作码，不进行任何操作
	if(ERR_CAN_NULL != err)
	{
		return (err);
	}
	
	//上位机写操作
	if(OPE_WRITE == mon->CanMess.OpeCode)
	{	
		WriteAirLLDPara1(mon->RxMsg.Data, mon->CanMess.DevNo);
	}
	
	
	return (ret);
}

/*
 * @function: MonAirLLDPara1Reply
 * @details : 上位机设置气压探测参数1应答
 * @input   : 1.mon：管理指针。
              2.buff：缓存。
 * @output  : NULL
 * @return  : 应答状态。
 */
uint8_t MonAirLLDPara1Reply(struct tagMonCan *mon, uint8_t *buff)
{
	uint8_t    ret = CAN_OPE_REPLY;
	
	
	ReadAirLLDPara1(buff, mon->CanMess.DevNo);
	
	
	return(ret);
}


/*
 * @function: MonAirLLDPara2
 * @details : 上位机设置气压探测参数2
 * @input   : 1.mon：监控管理指针。
              2.err：故障信息。
 * @output  : NULL
 * @return  : 0：成功；其他：故障指令。
 */
uint8_t MonAirLLDPara2(struct tagMonCan *mon, uint8_t err)
{
	uint8_t    ret = 0;
	
	
	//故障指令或操作码，不进行任何操作
	if(ERR_CAN_NULL != err)
	{
		return (err);
	}
	
	//上位机写操作
	if(OPE_WRITE == mon->CanMess.OpeCode)
	{
		WriteAirLLDPara2(mon->RxMsg.Data, 0);
	}
	
	
	return (ret);
}

/*
 * @function: MonAirLLDPara2Reply
 * @details : 上位机设置气压探测参数2应答
 * @input   : 1.mon：管理指针。
              2.buff：缓存。
 * @output  : NULL
 * @return  : 应答状态。
 */
uint8_t MonAirLLDPara2Reply(struct tagMonCan *mon, uint8_t *buff)
{
	uint8_t    ret = CAN_OPE_REPLY;
	
	
	ReadAirLLDPara2(buff, 0);
	
	
	return(ret);
}


/*
 * @function: MonAirLLDPara3
 * @details : 上位机设置气压探测参数
 * @input   : 1.mon：监控管理指针。
              2.err：故障信息。
 * @output  : NULL
 * @return  : 0：成功；其他：故障指令。
 */
uint8_t MonAirLLDPara3(struct tagMonCan *mon, uint8_t err)
{
	uint8_t    ret = 0;
	
	
	//故障指令或操作码，不进行任何操作
	if(ERR_CAN_NULL != err)
	{
		return (err);
	}
	
	//上位机写操作
	if(OPE_WRITE == mon->CanMess.OpeCode)
	{
		WriteAirLLDPara3(mon->RxMsg.Data, 0);
	}
	
	
	return (ret);
}

/*
 * @function: MonAirLLDPara3Reply
 * @details : 应答上位机读取气压探测参数。
 * @input   : 1.mon：管理指针。
              2.buff：缓存。
 * @output  : NULL
 * @return  : 应答状态。
 */
uint8_t MonAirLLDPara3Reply(struct tagMonCan *mon, uint8_t *buff)
{
	uint8_t    ret = CAN_OPE_REPLY;
	
	
	ReadAirLLDPara3(buff, 0);
	
	
	return(ret);
}


/*
 * @function: MonReadAirVal
 * @details : 上位读气压映射数据
 * @input   : 1.mon：监控管理指针。
              2.err：故障信息。
 * @output  : NULL
 * @return  : 0：成功；其他：故障指令。
 */
uint8_t MonReadAirVal(struct tagMonCan *mon, uint8_t err)
{
	uint8_t    ret = 0;
	
	
	//故障指令或操作码，不进行任何操作
	if(ERR_CAN_NULL != err)
	{
		return (err);
	}
	
	//上位机写操作
	if(OPE_WRITE == mon->CanMess.OpeCode)
	{
		WriteAirMapVal(mon->RxMsg.Data, 0);
	}
	
	
	return (ret);
}

/*
 * @function: MonReadAirValReply
 * @details : 应答上位机读取气压值。
 * @input   : 1.mon：管理指针。
              2.buff：缓存。
 * @output  : NULL
 * @return  : 应答状态。
 */
uint8_t MonReadAirValReply(struct tagMonCan *mon, uint8_t *buff)
{
	uint8_t    ret = CAN_OPE_REPLY;
	
	
	ReadAirMapVal(buff, 0);
	
	
	return(ret);
}


/*
 * @function: MonAirLLDPara4
 * @details : 上位机设置气压探测参数
 * @input   : 1.mon：监控管理指针。
              2.err：故障信息。
 * @output  : NULL
 * @return  : 0：成功；其他：故障指令。
 */
uint8_t MonAirLLDPara4(struct tagMonCan *mon, uint8_t err)
{
	uint8_t    ret = 0;
	
	
	//故障指令或操作码，不进行任何操作
	if(ERR_CAN_NULL != err)
	{
		return (err);
	}
	
	//上位机写操作
	if(OPE_WRITE == mon->CanMess.OpeCode)
	{
		WriteAirLLDPara4(mon->RxMsg.Data, 0);
	}
	
	
	return (ret);
}

/*
 * @function: MonAirLLDPara4Reply
 * @details : 应答上位机读取气压探测参数。
 * @input   : 1.mon：管理指针。
              2.buff：缓存。
 * @output  : NULL
 * @return  : 应答状态。
 */
uint8_t MonAirLLDPara4Reply(struct tagMonCan *mon, uint8_t *buff)
{
	uint8_t    ret = CAN_OPE_REPLY;
	
	
	ReadAirLLDPara4(buff, 0);
	
	
	return(ret);
}


/*
 * @function: MonAirLLDPara5
 * @details : 上位机设置气压探测参数
 * @input   : 1.mon：监控管理指针。
              2.err：故障信息。
 * @output  : NULL
 * @return  : 0：成功；其他：故障指令。
 */
uint8_t MonAirLLDPara5(struct tagMonCan *mon, uint8_t err)
{
	uint8_t    ret = 0;
	
	
	//故障指令或操作码，不进行任何操作
	if(ERR_CAN_NULL != err)
	{
		return (err);
	}
	
	//上位机写操作
	if(OPE_WRITE == mon->CanMess.OpeCode)
	{
		WriteAirLLDPara5(mon->RxMsg.Data, 0);
	}
	
	
	return (ret);
}

/*
 * @function: MonAirLLDPara5Reply
 * @details : 应答上位机读取气压探测参数。
 * @input   : 1.mon：管理指针。
              2.buff：缓存。
 * @output  : NULL
 * @return  : 应答状态。
 */
uint8_t MonAirLLDPara5Reply(struct tagMonCan *mon, uint8_t *buff)
{
	uint8_t    ret = CAN_OPE_REPLY;
	
	
	ReadAirLLDPara5(buff, 0);
	
	
	return(ret);
}





/*
 * @function: MonReadSoftWareName
 * @details : 监控读软件名称
 * @input   : 1.mon：监控管理指针。
              2.err：故障信息。
 * @output  : NULL
 * @return  : 0：成功；其他：故障指令。
 */
uint8_t MonReadSoftWareName(struct tagMonCan *mon, uint8_t err)
{
	uint8_t    ret = 0;
	
	
	//故障指令或操作码，不进行任何操作
	if(ERR_CAN_NULL != err)
	{
		return (err);
	}
	
	
	//上位机写操作
	if(OPE_WRITE == mon->CanMess.OpeCode)
	{
	}
	
	
	//上位机读操作
	if(OPE_READ == mon->CanMess.OpeCode)
	{
		ReadSoftWareNameIns(mon->RxMsg.Data, 0);
	}
	
	
	return (ret);
}

/*
 * @function: MonReadSoftWareNameReply
 * @details : 应答读软件名称。
 * @input   : 1.mon：管理指针。
              2.buff：缓存。
 * @output  : NULL
 * @return  : 应答状态。
 */
uint8_t MonReadSoftWareNameReply(struct tagMonCan *mon, uint8_t *buff)
{
	uint8_t    ret = CAN_OPE_REPLY;
	
	
	ReadSoftWareName(buff, 0);
	
	
	return(ret);
}


/*
 * @function: MonReadSoftWareVer
 * @details : 监控读软件版本
 * @input   : 1.mon：监控管理指针。
              2.err：故障信息。
 * @output  : NULL
 * @return  : 0：成功；其他：故障指令。
 */
uint8_t MonReadSoftWareVer(struct tagMonCan *mon, uint8_t err)
{
	uint8_t    ret = 0;
	
	
	//故障指令或操作码，不进行任何操作
	if(ERR_CAN_NULL != err)
	{
		return (err);
	}
	
	
	//上位机写操作
	if(OPE_WRITE == mon->CanMess.OpeCode)
	{
	}
	
	
	return (ret);
}

/*
 * @function: MonReadSoftWareVerReply
 * @details : 应答读软件版本。
 * @input   : 1.mon：管理指针。
              2.buff：缓存。
 * @output  : NULL
 * @return  : 应答状态。
 */
uint8_t MonReadSoftWareVerReply(struct tagMonCan *mon, uint8_t *buff)
{
	uint8_t    ret = CAN_OPE_REPLY;
	
	
	ReadSoftWareVer(buff, 0);
	
	
	return(ret);
}


/*
 * @function: MonRWpara
 * @details : 电容读写参数
 * @input   : 1.mon：监控管理指针。
              2.err：故障信息。
 * @output  : NULL
 * @return  : 0：成功；其他：故障指令。
 */
uint8_t MonRWpara(struct tagMonCan *mon, uint8_t err)
{
	uint8_t    ret = 0;
	int32_t    lValue = 0;
	
	
	//故障指令或操作码，不进行任何操作
	if(ERR_CAN_NULL != err)
	{
		return (err);
	}
	Storage.ParaNo = mon->RxMsg.Data[3];
	
	//上位机写操作
	if(OPE_WRITE == mon->CanMess.OpeCode)
	{
		lValue = ((mon->RxMsg.Data[4]<<24) | (mon->RxMsg.Data[5]<<16) | (mon->RxMsg.Data[6]<<8) |(mon->RxMsg.Data[7]));		
		//赋值操作		
		//WriteUsePara(mon->RxMsg.Data, Storage.ParaNo);
		LLD_Param(TMC_WRITE, Storage.ParaNo, &lValue);
	}
	else
	{
		//读取
		LLD_Param(TMC_READ, Storage.ParaNo, &lValue);
	}
	
	
	return (ret);
}
/*
 * @function: MonRWparaReply
 * @details : 应答读写参数。
 * @input   : 1.mon：管理指针。
              2.buff：缓存。
 * @output  : NULL
 * @return  : 应答状态。
 */
uint8_t MonRWparaReply(struct tagMonCan *mon, uint8_t *buff)
{
	uint8_t    ret = CAN_OPE_REPLY;
	
	
	ReadUsePara(buff, 0);
	
	
	return(ret);
}













/*
 * @function: CheckFrame
 * @details : 检查报文格式。
 * @input   : 1.mon：监控管理指针。
 * @output  : NULL
 * @return  : 0：无错误；1：指令错误；2：操作码错误。
 */
uint8_t CheckFrame(struct tagMonCan *mon)
{
	uint8_t    ret = ERR_CAN_NULL;
	
	
	//检查操作码
	if((OPE_READ != mon->CanMess.OpeCode) && (OPE_WRITE != mon->CanMess.OpeCode))
	{
		ret = ERR_CAN_OPE;
	}	
	
	//检查指令
	if((INS_IO != mon->CanMess.Ins)
		&& (INS_TEMP != mon->CanMess.Ins)
		&& (INS_RESERVE_201 != mon->CanMess.Ins)
		&& (INS_SELECT_SEN != mon->CanMess.Ins)
		&& (INS_READ_LLD != mon->CanMess.Ins)
		&& (INS_RESERVE_204 != mon->CanMess.Ins)
		&& (INS_RESERVE_205 != mon->CanMess.Ins)
		&& (INS_RESERVE_206 != mon->CanMess.Ins)
		
		&& (INS_CAPLLD_PARA1 != mon->CanMess.Ins)
		&& (INS_RESERVE_208 != mon->CanMess.Ins)
		&& (INS_CAPLLD_PARA3 != mon->CanMess.Ins)
		&& (INS_CAP_READ_VAL != mon->CanMess.Ins)
		
		&& (INS_AIRLLD_PARA1 != mon->CanMess.Ins)
		&& (INS_AIRLLD_PARA2 != mon->CanMess.Ins)
		&& (INS_AIRLLD_PARA3 != mon->CanMess.Ins)
		&& (INS_AIR_READ_VAL != mon->CanMess.Ins)
		&& (INS_AIRLLD_PARA4 != mon->CanMess.Ins)
		&& (INS_AIRLLD_PARA5 != mon->CanMess.Ins)
		
		&& (INS_R_SOFTWARE_NAME != mon->CanMess.Ins)
		&& (INS_R_SOFTWARE_VER != mon->CanMess.Ins)
	
		&& (INS_RW_PARA != mon->CanMess.Ins))
	{
		ret = ERR_CAN_INS;
	}
	
	mon->CanMess.ErrCode = ret;
	
	
	return (ret);
}










/*
 * @function: MonAnalyseCanMess
 * @details : 解析报文。
 * @input   : 1.pdata：数据缓存。
 * @output  : NULL
 * @return  : 故障类型。
 */
uint8_t MonAnalyseCanMess(void)
{
	uint8_t    ret = CAN_OPE_REPLY;
	uint8_t    reply = ERR_CAN_NULL;
	uint8_t    err = ERR_CAN_NULL;	
	
	
	MonCan.CanMess.MessID = MonCan.RxMsg.Data[0];
	MonCan.CanMess.Ins = MonCan.RxMsg.Data[1];
	MonCan.CanMess.OpeCode = MonCan.RxMsg.Data[2];
	MonCan.CanMess.DevNo = MonCan.RxMsg.Data[3];
	
	err = CheckFrame(&MonCan);
	switch(MonCan.CanMess.Ins)
	{
		case INS_IO:
		{
			reply = MonOperateIO(&MonCan, err);
		}
		break;
		
		case INS_TEMP:
		{
			reply = MonTemp(&MonCan, err);
		}
		break;
		
		case INS_RESERVE_201:
		{
			reply = MonConfigAirSen(&MonCan, err);
		}
		break;
		
		case INS_SELECT_SEN:
		{
			reply = MonSelectSen(&MonCan, err);
		}
		break;
		
		case INS_READ_LLD:
		{
			reply = MonReadLLD(&MonCan, err);
		}
		break;
		
		case INS_RESERVE_204:
		{
			reply = MonRead204(&MonCan, err);
		}
		break;	
		
		case INS_RESERVE_205:
		{
			reply = MonRead205(&MonCan, err);
		}
		break;
		
		case INS_RESERVE_206:
		{
			reply = MonRead206(&MonCan, err);
		}
		break;
		
		
		
		case INS_CAPLLD_PARA1:
		{
			reply = MonCapLLDPara1(&MonCan, err);
		}
		break;
		
		case INS_RESERVE_208:
		{
			reply = MonRead208(&MonCan, err);
		}
		break;
		
		case INS_CAPLLD_PARA3:
		{
			reply = MonCapLLDPara3(&MonCan, err);
		}
		break;
		
		case INS_CAP_READ_VAL:
		{
			reply = MonReadCapVal(&MonCan, err);
		}
		break;
		
		
		
		case INS_AIRLLD_PARA1:
		{
			reply = MonAirLLDPara1(&MonCan, err);
		}
		break;
		
		case INS_AIRLLD_PARA2:
		{
			reply = MonAirLLDPara2(&MonCan, err);
		}
		break;
		
		case INS_AIRLLD_PARA3:
		{
			reply = MonAirLLDPara3(&MonCan, err);
		}
		break;
		
		case INS_AIR_READ_VAL:
		{
			reply = MonReadAirVal(&MonCan, err);
		}
		break;
		
		case INS_AIRLLD_PARA4:
		{
			reply = MonAirLLDPara4(&MonCan, err);
		}
		break;
		
		case INS_AIRLLD_PARA5:
		{
			reply = MonAirLLDPara5(&MonCan, err);
		}
		break;
		
		
		
		case INS_R_SOFTWARE_NAME:
		{
			reply = MonReadSoftWareName(&MonCan, err);
		}
		break;
		
		case INS_R_SOFTWARE_VER:
		{
			reply = MonReadSoftWareVer(&MonCan, err);
		}
		break;		
		
		
		
		case INS_RW_PARA:
		{
			reply = MonRWpara(&MonCan, err);
		}
		break;
		
		
		
		
		default:
		{
		}
		break;
	}
	
	
//	if(ERR_CAN_NULL != reply)
//	{
//		ret = CAN_OPE_NO_REPLY;
//	}
	
	
	
	return (ret);
}














/*
 * @function: MonFillSendBuff
 * @details : 填充发送缓存。
 * @input   : 1.mon：监控管理指针。
              2.buff：数据缓存。
 * @output  : NULL
 * @return  : 0：成功；其他：故障指令。
 */
uint8_t MonFillSendBuff(struct tagMonCan *mon, uint8_t *buff)
{
	uint8_t    mailbox = 0;
	
	extern __IO LLDParam_t g_tLLDParam;
	
	//填充报文
	mon->TxMsg.StdId = g_tLLDParam.CanConfig.ModuleID;//mon->Confg.ModuleID;       //ID
	mon->TxMsg.ExtId = 0x00;
	mon->TxMsg.IDE = CAN_ID_STD;                 //标准模式
	mon->TxMsg.RTR = CAN_RTR_DATA;               //发送的是数据
	mon->TxMsg.DLC = 8;                          //数据长度为8字节	
	mon->TxMsg.Data[0] = mon->CanMess.MessID;
	mon->TxMsg.Data[1] = mon->CanMess.Ins;
	mon->TxMsg.Data[2] = buff[2];
	mon->TxMsg.Data[3] = buff[3];
	mon->TxMsg.Data[4] = buff[4];
	mon->TxMsg.Data[5] = buff[5];
	mon->TxMsg.Data[6] = buff[6];
	mon->TxMsg.Data[7] = buff[7];
	
	//发送报文
	mailbox = CAN_Transmit(CAN1, &mon->TxMsg);
	
	
	return(mailbox);
}


/*
 * @function: MonSendCanMess
 * @details : 模块发送报文。
 * @input   : 1.TxMessage：报文指针。
 * @output  : NULL
 * @return  : 发送报文的结果，0-2：成功；4：邮箱为空；5：等待发送。
 */
uint8_t MonSendCanMess(void)
{
	uint8_t    ret = CAN_TxStatus_Ok;
	uint8_t    reply = ERR_CAN_NULL;
	uint8_t    buff[8] = {0};

	
	
	switch(MonCan.CanMess.Ins)
	{
		case INS_IO:
		{
			reply = MonOperateIOReply(&MonCan, buff);
		}
		break;
		
		case INS_TEMP:
		{
			reply = MonTempReply(&MonCan, buff);
		}
		break;
		
		case INS_RESERVE_201:
		{
			reply = MonConfigAirSenReply(&MonCan, buff);
		}
		break;
		
		case INS_SELECT_SEN:
		{
			reply = MonSelectSenReply(&MonCan, buff);
		}
		break;
		
		case INS_READ_LLD:
		{
			reply = MonReadLLDReply(&MonCan, buff);
		}
		break;
		
		case INS_RESERVE_204:
		{
			reply = MonRead204Reply(&MonCan, buff);
		}
		break;	
		
		case INS_RESERVE_205:
		{
			reply = MonRead205Reply(&MonCan, buff);
		}
		break;
		
		case INS_RESERVE_206:
		{
			reply = MonRead206Reply(&MonCan, buff);
		}
		break;
		
		
		
		case INS_CAPLLD_PARA1:
		{
			reply = MonCapLLDPara1Reply(&MonCan, buff);
		}
		break;
		
		case INS_RESERVE_208:
		{
			reply = MonRead208Reply(&MonCan, buff);
		}
		break;
		
		case INS_CAPLLD_PARA3:
		{
			reply = MonCapLLDPara3Reply(&MonCan, buff);
		}
		break;
		
		case INS_CAP_READ_VAL:
		{
			reply = MonReadCapValReply(&MonCan, buff);
		}
		break;
		
		
		
		case INS_AIRLLD_PARA1:
		{
			reply = MonAirLLDPara1Reply(&MonCan, buff);
		}
		break;
		
		case INS_AIRLLD_PARA2:
		{
			reply = MonAirLLDPara2Reply(&MonCan, buff);
		}
		break;
		
		case INS_AIRLLD_PARA3:
		{
			reply = MonAirLLDPara3Reply(&MonCan, buff);
		}
		break;
		
		case INS_AIR_READ_VAL:
		{
			reply = MonReadAirValReply(&MonCan, buff);
		}
		break;
		
		case INS_AIRLLD_PARA4:
		{
			reply = MonAirLLDPara4Reply(&MonCan, buff);
		}
		break;
		
		case INS_AIRLLD_PARA5:
		{
			reply = MonAirLLDPara5Reply(&MonCan, buff);
		}
		break;
		
		
		
		case INS_R_SOFTWARE_NAME:
		{
			reply = MonReadSoftWareNameReply(&MonCan, buff);
		}
		break;
		
		case INS_R_SOFTWARE_VER:
		{
			reply = MonReadSoftWareVerReply(&MonCan, buff);
		}
		break;
		
		
		case INS_RW_PARA:
		{
			reply = MonRWparaReply(&MonCan, buff);
		}
		break;
		
		
		
		default:
		{
		}
		break;
	}
	
	
	//优先故障/状态码
	if((ERR_CAN_INS == MonCan.CanMess.ErrCode)
		|| (ERR_CAN_OPE == MonCan.CanMess.ErrCode)
		|| (ERR_CAN_DATA_FAILD == MonCan.CanMess.ErrCode))
	{
		buff[2] = MonCan.CanMess.ErrCode;
	}
	
	
	
	
	if(CAN_OPE_REPLY == reply)
	{
		//正常应答
		ret = MonFillSendBuff(&MonCan, buff);
	}
	else if(CAN_OPE_WAIT == reply)
	{
		//阻塞应答
		ret = CAN_OPE_WAIT;
	}
	
	
	
	return (ret);
}


/*
 * @function: CanMonComStage
 * @details : 监听Can通信
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void CanMonComStage(void)
{
	uint8_t    ret = FALSE;
	uint8_t    reply = CAN_OPE_NO_REPLY;
	
	
	//各个状态下的动作
	switch(MonCan.ComStage)
	{
		case MON_CAN_NULL:
		{
			MonCan.ComStage = MON_CAN_RECE;
		}
		break;
		
		case MON_CAN_RECE:
		{
//			//解析数据，未使用Can接收队列。
//			if(TRUE == MonCan.ReceFinish)
//			{
//				reply = MonAnalyseCanMess();
//				MonCan.ReceFinish = FALSE;
//			}
//			
//			//广播
//			if(TRUE == MonCan.ReceBroadcastFinish)
//			{
////				MonAnalyseBroadcast();
//				reply = CAN_OPE_REPLY;
//				MonCan.ReceBroadcastFinish = FALSE;
//			}
			
			
			//从Can接收队列中取出数据
			ret = QueueLoopCanPop(&MonCan.RxQueue, MonCan.RxMsg.Data);
			
			//解析数据
			if(CAN_OPE_REPLY == ret)
			{
				reply = MonAnalyseCanMess();
				MonCan.ReceFinish = FALSE;
				
				MonCan.AckBlockTime = 0;
			}
			
			
			
			if(CAN_OPE_REPLY == reply)
			{
				MonCan.ComStage = MON_CAN_REPLY;
			}
		}
		break;
		
		case MON_CAN_REPLY:
		{
			//阻塞应答延时
			Accumulation16(&MonCan.AckBlockTime);
			
			reply = MonSendCanMess();
			if(CAN_TxStatus_Failed == reply)
			{
				//发送失败
			}
			else if(CAN_TxStatus_NoMailBox == reply)
			{
				//发送邮箱满
				//CAN_Config(CAN1, &MonCan.Confg);
				CAN_Config(CAN1);
			}
			
			
			if((CAN_OPE_WAIT != reply)
				|| (MonCan.AckBlockTime > CAN_ACK_TIMEOUT))
			{
				MonCan.ComStage = MON_CAN_RECE;
			}
		}
		break;
		
		default:
		{
			MonCan.ComStage = MON_CAN_NULL;
		}
		break;
	}
}




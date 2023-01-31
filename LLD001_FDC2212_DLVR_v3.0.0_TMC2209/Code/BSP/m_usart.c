/*****************************************************************************
Copyright  : BGI
File name  : m_usart.c
Description: 串口通信管理模块
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#include "m_usart.h"




//定义变量---------------------------------------------------------------------//
/*
 * @function: UsartIRQReceiveHandler
 * @details : 处理串口接收数据
 * @input   : 1.pLpcUart：串口指针。
              2.ch：接收的数据。
 * @output  : NULL
 * @return  : NULL
 */
void UsartIRQReceiveHandler(struct tagUartCom *pUart, uint8_t ch)
{
	if(pUart->RecvCnt < UART_RECV_MAXNUM)
	{
		pUart->RecvBuf[pUart->RecvCnt] = ch;
		pUart->Receiving = TRUE;
		pUart->RecvTimeCnt = 0;
		pUart->RecvCnt++;		
	}
}


/*
 * @function: BSP_UartCommIsSendEnd
 * @details : 判断串口是否发送完成。
 * @input   : 1.pCom：串口结构体。
 * @output  : NULL
 * @return  : 0：未完成；1：已完成。
 */ 
static uint8_t BSP_UartCommIsSendEnd(struct tagUartCom* pCom)
{
	//发送完成
	if(pCom->NetworkIdleTime >= pCom->SendEndTime)
	{
		return TRUE;
	}
	
	return FALSE; 
}

/*
 * @function: BSP_UartCommIsRecvEnd
 * @details : 判断串口是否接收完成。
 * @input   : 1.pCom：串口结构体。
 * @output  : NULL
 * @return  : 0：未完成；1：已完成。
 */ 
static uint8_t BSP_UartCommIsRecvEnd(struct tagUartCom* pCom)
{
	if(pCom->RecvTimeCnt >= pCom->RecvEndTime)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*
 * @function: BSP_UartCommIsNeedSend
 * @details : 判断是否需要发送数据。
 * @input   : 1.pCom：串口结构体。
 * @output  : NULL
 * @return  : 0：否；1：是。
 */ 
static uint8_t BSP_UartCommIsNeedSend(struct tagUartCom* pCom)
{
	if(pCom->NetworkIdleTime >= pCom->SendIntervalTime)
	{
		pCom->NetworkIdleTime = 0;//网络空闲时间清零
		pCom->ForceJumpDlyCnt = 0;//强制跳转时间清零
		
		return TRUE;
	}	
	else	
	{
		return FALSE;
	}
}

/*
 * @function: BSP_UartCommIsNeedSend
 * @details : 发送数据之前的等待时间。
 * @input   : 1.pCom：串口结构体。
 * @output  : NULL
 * @return  : 等待结果，0：未完成；1：已完成。
 */ 
static uint8_t BSP_UartCommSendWaitTime(struct tagUartCom* pCom)
{
	if(pCom->SendDlyCnt >= pCom->SendDlyTime)
	{
		return TRUE;
	}	
	else	
	{
		return FALSE;
	}
}

/*
 * @function: BSP_UartCommIsNeedSend
 * @details : 判断串口控制是否需要重新初始化。
 * @input   : 1.pCom：串口结构体。
 * @output  : NULL
 * @return  : 0：否；1：是。
 */
static uint8_t BSP_UartCommIsRepeatInit(struct tagUartCom* pCom)
{
	if(pCom->RepeatInitFlag == 1)
	{
		pCom->RepeatInitFlag = 0;
		return TRUE;
	}	
	else	
	{
		return FALSE;
	}
}

/*
 * @function: BSP_UartCommClrSendBuffer
 * @details : 清除发送缓存区
 * @input   : 1.pCom：串口结构体。
 * @output  : NULL
 * @return  : NULL
 */
static void BSP_UartCommClrSendBuffer(struct tagUartCom* pCom)
{
	uint16_t i=0;
	
	for(i = 0; i < UART_SEND_MAXNUM; i++)
	{
		pCom->SendBuf[i] = 0;
	}
}

/*
 * @function: BSP_UartCommClrRecvBuffer
 * @details : 清除接收缓存区
 * @input   : 1.pCom：串口结构体。
 * @output  : NULL
 * @return  : NULL
 */
static void BSP_UartCommClrRecvBuffer(struct tagUartCom* pCom)
{
 	uint16_t i=0;
	
	for(i = 0; i < UART_RECV_MAXNUM; i++)
	{
		pCom->RecvBuf[i] = 0;
	}
}

/*
 * @function: BSP_UartCommTimeHandler
 * @details : 通信计时变量，1ms计时。
 * @input   : 1.pCom：串口结构体。
 * @output  : NULL
 * @return  : NULL
 */ 
void BSP_UartCommTimeHandler(void* com)
{	
	struct tagUartCom* pCom = (struct tagUartCom*)com;
	
	
	if(TRUE == pCom->Receiving)	//接收完成计时
	{
		if(pCom->RecvTimeCnt < 250)
		{
			pCom->RecvTimeCnt++;
		}
	}
	else
	{
		pCom->RecvTimeCnt = 0;
	}

	if(pCom->SendDlyCnt < 250)
	{
		pCom->SendDlyCnt++;
	}				
	
	if(pCom->NetworkIdleTime < 65530)
	{
		pCom->NetworkIdleTime++;
	}
	
	if(pCom->ForceJumpDlyCnt < 65530)
	{
		pCom->ForceJumpDlyCnt++;
	}	
} 

/*
 * @function: BSP_UartCommStage
 * @details : 串口通信状态机，半双工模式。
 * @input   : 1.pCom：串口结构体。
 * @output  : NULL
 * @return  : NULL
 */ 
void BSP_UartCommStage(void* com)
{	
	struct tagUartCom* pCom = (struct tagUartCom*)com;
	uint8_t ret = 0;
	
	
	if(pCom->Stage == COMM_NULL)
	{
		;//当初始时，或配置错误，就会跳到这个阶段
	}
	else if(pCom->Stage == COMM_INIT)//初始化阶段
	{
		pCom->Stage = COMM_WAIT;//跳转到等待阶段			
		pCom->SendNum = 0;				 
		pCom->RecvCnt = 0;			 
		pCom->SendCnt = 0;	
		pCom->Receiving = 0;
		
		pCom->RecvTimeCnt = 0;
		pCom->SendDlyCnt = 0;
		pCom->ForceJumpDlyCnt = 0;
		pCom->NetworkIdleTime =0;
		
		BSP_UartCommClrSendBuffer(pCom);//清除发送缓冲区
		BSP_UartCommClrRecvBuffer(pCom);//清除接收缓冲区
		if(pCom->Set485OnOff != NULL)
		{ 
			pCom->Set485OnOff(UART_REC_MODE);// 打开485接收
		} 
		if(pCom->SetRecvOrSend != NULL)
		{ 
			pCom->SetRecvOrSend(UART_REC_MODE, pCom);// 串口切换至输入
		}
	}
	else if(pCom->Stage == COMM_WAIT)//空闲阶段
	{
		if(TRUE == BSP_UartCommIsRecvEnd( pCom ))//如果接收完成
		{
			pCom->Stage = COMM_UPDATA;	//跳转到解析阶段	
			if(pCom->SetRecvOrSend != NULL)
			{ 
				pCom->SetRecvOrSend(UART_CLOSE_MODE, pCom);// 串口关闭
			} 
			if(pCom->Set485OnOff != NULL)
			{ 
				pCom->Set485OnOff(UART_REC_MODE);// 打开485接收
			} 
			pCom->Receiving = 0;//解析完成清除
			pCom->RecvTimeCnt = 0;//完成时间清零
		}
		else if((pCom->CommMode == UART_MODE_MASTER)//如果是主模式
				&& (TRUE == BSP_UartCommIsNeedSend( pCom )))//如果到达主机发送时间
		{	
			pCom->Stage = COMM_SNED_READY;//跳转到发送准备阶段	
		}
		else if(BSP_UartCommIsRepeatInit(pCom) == TRUE)
		{
			pCom->Stage = COMM_INIT;
		}
	}
	if(pCom->Stage == COMM_SNED_READY)//如果是发送准备阶段
	{
		ret = pCom->CommSend(pCom);
		if(TRUE == ret)	//如果发送程序决定需要发送
		{
			pCom->Stage = COMM_SEND_WAIT;	//跳转到发送等待阶段
			pCom->SendDlyCnt = 0;//发送时间清零 
			if(pCom->Set485OnOff != NULL)
			{ 
				pCom->Set485OnOff(UART_SEND_MODE);// 打开485接收
			} 
		}
		else if((FALSE == ret) || (pCom->ForceJumpDlyCnt >= pCom->ForceJumpDlyTime))//发送程序决定不发 或者 发送阶段等的太久了
		{
			pCom->Stage = COMM_WAIT;		//不需要发送就跳转到等待阶段
			pCom->ForceJumpDlyCnt = 0;//强制跳转时间清零 
			if(pCom->SetRecvOrSend != NULL)
			{ 
				pCom->SetRecvOrSend(UART_REC_MODE, pCom);// 串口切换至输入
			} 
			if(pCom->Set485OnOff != NULL)
			{ 
				pCom->Set485OnOff(UART_REC_MODE);// 打开485接收
			} 
		}
		else//if(ret == 2)//发送程序决定再看看
		{
		
		}
	}
	if(pCom->Stage == COMM_SEND_WAIT)//如果是发送等待阶段
	{		
		if(TRUE == BSP_UartCommSendWaitTime(pCom))//发送短等待完成
		{
			pCom->NetworkIdleTime = 0;//发送完成时间清零（发送前的准备）
			pCom->Stage = COMM_SEND_ING;//发送嘤嘤嘤
			pCom->SendCnt = 0;			//发送个数清除,防止数据不为0而漏发 
			if(pCom->SetRecvOrSend != NULL)
			{ 
				pCom->SetRecvOrSend(UART_SEND_MODE, pCom);// 串口切换至输入
			}
		}
	}
	
	if(pCom->Stage == COMM_SEND_ING)//如果在发送中阶段
	{
		if(TRUE == BSP_UartCommIsSendEnd(pCom))//如果发送完成了
		{
			pCom->SendCnt = 0;//发送计数清零
			pCom->SendDlyCnt = 0;
			
			if(TRUE == pCom->IsAnotherPackFlag)
			{				
				pCom->Stage = COMM_SNED_READY;
			}
			else 
			{
				pCom->Stage = COMM_WAIT;//跳转到等待阶段
				if(pCom->SetRecvOrSend != NULL)
				{ 
					pCom->SetRecvOrSend(UART_REC_MODE, pCom);// 串口切换至输入
				}
				if(pCom->Set485OnOff != NULL)
				{ 
					pCom->Set485OnOff(UART_REC_MODE);// 打开485接收
				}
				pCom->NetworkIdleTime = 0;//发送等待时间清零
				BSP_UartCommClrSendBuffer(pCom);//清除发送缓冲区				
			}
		}
	}
	
	if(pCom->Stage == COMM_UPDATA)//解析阶段
	{
		if(TRUE == pCom->CommUpdate(pCom))//如果解析后确定要发送
		{
			pCom->Stage = COMM_SNED_READY;//跳转到发送准备阶段 
			pCom->ForceJumpDlyCnt = 0;	//清零强制发送时间
		}
		else
		{
			pCom->Stage = COMM_WAIT;//不需要应答，就调回等待阶段
			if(pCom->SetRecvOrSend != NULL)
			{ 
				pCom->SetRecvOrSend(UART_REC_MODE, pCom);// 串口切换至输入
			} 
			if(pCom->Set485OnOff != NULL)
			{ 
				pCom->Set485OnOff(UART_REC_MODE);// 打开485接收
			} 
		}
		pCom->NetworkIdleTime = 0;//发送等待时间清零
		BSP_UartCommClrRecvBuffer(pCom);//清除接收缓冲区
		pCom->RecvCnt = 0;//接收个数清零
	}
}


/*
 * @function: BSP_CommUartInit
 * @details : 串口通信状态机，半双工模式。
 * @input   : 1.pCom：串口结构体。
 * @output  : NULL
 * @return  : NULL
 */ 
uint8_t BSP_CommUartInit(void* com,
						 uint8_t (*recv_fun)(void *),
						 uint8_t (*send_fun)(void *),
						 void ( *set_recv_send )(uint8_t recv_send_flag, struct tagUartCom *),
						 void ( *set_485_on_off)(uint8_t set_value),
						 uint8_t comm_mode,						 
						 uint8_t recv_end_time,						 
						 uint8_t send_dly,
						 uint8_t send_end_time,
						 uint16_t send_cycle,
						 uint16_t force_jump_time)
{
	struct tagUartCom* pCom = (struct tagUartCom*)com;
	
	
	pCom->CommUpdate = recv_fun;
	pCom->CommSend = send_fun;
	pCom->SetRecvOrSend = set_recv_send;
	pCom->Set485OnOff = set_485_on_off;
	pCom->CommMode = comm_mode;	
	pCom->RecvEndTime = recv_end_time;	
	pCom->SendDlyTime = send_dly;
	pCom->SendEndTime = send_end_time; 
	pCom->SendIntervalTime = send_cycle;
	pCom->ForceJumpDlyTime = force_jump_time;
	
	


	if((pCom->CommUpdate == NULL) || (pCom->CommSend == NULL))
	{
		return FALSE;
	}
	pCom->Stage = COMM_INIT;
	
	return TRUE;
}












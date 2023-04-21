/*****************************************************************************
Copyright  : BGI
File name  : bsp_can.c
Description: can通信
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#include "bsp_can.h" 
#include "include.h"
#include "monitor_can.h"


//定义变量---------------------------------------------------------------------//
//CAN不同波特率，配置参数
static const CanCfg_t g_taCanCfg[EN_CAN_BAUD_END] = {
	//ucPrescaler, ulSeg1, ulSeg2, ulSJW
	{600, CAN_BS1_8tq,  CAN_BS2_3tq, CAN_SJW_2tq}, //5
	{300, CAN_BS1_8tq,  CAN_BS2_3tq, CAN_SJW_2tq}, //10
	{150, CAN_BS1_8tq,  CAN_BS2_3tq, CAN_SJW_2tq}, //20
	{60,  CAN_BS1_8tq,  CAN_BS2_3tq, CAN_SJW_2tq}, //50
	{30,  CAN_BS1_8tq,  CAN_BS2_3tq, CAN_SJW_2tq}, //100
	{24,  CAN_BS1_8tq,  CAN_BS2_3tq, CAN_SJW_2tq}, //125	
	{12,  CAN_BS1_8tq,  CAN_BS2_3tq, CAN_SJW_2tq}, //250	
	{6,   CAN_BS1_8tq,  CAN_BS2_3tq, CAN_SJW_2tq}, //500	
	{3,   CAN_BS1_10tq, CAN_BS2_4tq, CAN_SJW_2tq}, //800
	{3,  CAN_BS1_8tq,   CAN_BS2_3tq, CAN_SJW_2tq}  //1000
};




/*
 * @function: CAN_GPIO_Config
 * @details : 配置CAN引脚
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void CAN_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	
	//Enable CAN clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	
	//Enable GPIO clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	
	//Configure CAN TX pins
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		         // 复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//Configure CAN RX  pins
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	             // 上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);		
}

/*
 * 函数名：CAN_GPIO_Config
 * 描述  ：CAN的GPIO 配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：内部调用
 */
//static void CAN_GPIO_Config(void)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;   	

//	/* Enable GPIO clock */
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);
//	
//	//重映射引脚
//	GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);

//	/* Configure CAN TX pins */
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		         // 复用推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

//	GPIO_Init(GPIOB, &GPIO_InitStructure);

//	/* Configure CAN RX  pins */
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	             // 上拉输入
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//}


///*
// * @function: CAN_Mode_Config
// * @details : 配置CAN模式
// * @input   : NULL
// * @output  : NULL
// * @return  : NULL
// */
//void CAN_Mode_Config(CAN_TypeDef* can, struct tagCanConfig  *confg)
//{
//	CAN_InitTypeDef        CAN_InitStructure;
//	
//	//CAN寄存器初始化
//	CAN_DeInit(can);
//	CAN_StructInit(&CAN_InitStructure);

//	//CAN单元初始化
//	CAN_InitStructure.CAN_TTCM = DISABLE;                  //MCR-TTCM  关闭时间触发通信模式使能
//	
//	//为了方便调试，开启自动离线管理
//	CAN_InitStructure.CAN_ABOM = ENABLE;                   //MCR-ABOM  自动离线管理。ENABLE-使用该功能，可以在节点出错离线后适时的自动恢复，不需要软件干预。
//	
//	CAN_InitStructure.CAN_AWUM = ENABLE;                   //MCR-AWUM  自动唤醒模式。ENABLE-使用该功能，可以在监测到总线活动后自动唤醒。
//	CAN_InitStructure.CAN_NART = DISABLE;                  //MCR-NART  禁止报文自动重传	  DISABLE-使用自动重传。当仲裁失败后，Can会自动重发。
//	CAN_InitStructure.CAN_RFLM = DISABLE;                  //MCR-RFLM  接收FIFO 锁定模式  DISABLE-溢出时新报文会覆盖原有报文  
//	CAN_InitStructure.CAN_TXFP = DISABLE;                  //MCR-TXFP  发送FIFO优先级 DISABLE-优先级取决于报文标示符 
//	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;          //正常工作模式
////	CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack;        //回环模式，用于自测


//	//------------计算Can通信波特率-----------------------------------------//
////	CAN 波特率 = RCC_APB1Periph_CAN / Prescaler / (SJW + BS1 + BS2);
////	SJW = synchronisation_jump_width 
////	BS = bit_segment

////	周立功推荐：
////	1.保证TSEG1+TSEG2的时间份额在10-20之间。
////	2.（TSEG1+1）/（TSEG1+TSEG2+1）的采样点位置在75%~81.5%为宜，极限情况下不得在70%~87.5%之外。
////	3.同步跳转SJW取（TSEG2-1）为宜。

////	时间份额（tq）    TSEG1    TSEG2    SJW
////	10               6        3        2
////	12               8        3        2
////	14               9        4        3
////	16               11       4        3
////	18               12       5        3
////	20               14       5        3
//	//----------------------------------------------------------------------//
//	
//	if(CAN_BPS_MODE_1M == confg->BpsMode)
//	{
//		//波特率1Mbps
//		//使用HSE时钟，APB1 = 36 MHz，配置ss=1（同步段，恒为1） ，时间宽度为(BS1+BS2+ss) ，波特率即为时钟周期tq*(BS1+BS2+ss)。
//		CAN_InitStructure.CAN_SJW = CAN_SJW_2tq;               //BTR-SJW 重新同步跳跃宽度（不参与波特率计算）
//		CAN_InitStructure.CAN_BS1 = CAN_BS1_8tq;               //BTR-TS1 时间段1
//		CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;               //BTR-TS1 时间段2
//		CAN_InitStructure.CAN_Prescaler = 3;                   //BTR-BRP 波特率分频器
//	}
//	else if(CAN_BPS_MODE_500K == confg->BpsMode)
//	{
//		//波特率500kbps
//		//使用HSE时钟，APB1 = 36 MHz，配置ss=1（同步段，恒为1） ，时间宽度为(BS1+BS2+ss) ，波特率即为时钟周期tq*(BS1+BS2+ss)。
//		CAN_InitStructure.CAN_SJW = CAN_SJW_3tq;               //BTR-SJW 重新同步跳跃宽度（不参与波特率计算）
//		CAN_InitStructure.CAN_BS1 = CAN_BS1_12tq;              //BTR-TS1 时间段1
//		CAN_InitStructure.CAN_BS2 = CAN_BS2_5tq;               //BTR-TS1 时间段2
//		CAN_InitStructure.CAN_Prescaler = 4;                   //BTR-BRP 波特率分频器
//	}
//	else
//	{
//		//默认波特率1Mbps
//		//使用HSE时钟，APB1 = 36 MHz，配置ss=1（同步段，恒为1） ，时间宽度为(BS1+BS2+ss) ，波特率即为时钟周期tq*(BS1+BS2+ss)。
//		CAN_InitStructure.CAN_SJW = CAN_SJW_2tq;               //BTR-SJW 重新同步跳跃宽度（不参与波特率计算）
//		CAN_InitStructure.CAN_BS1 = CAN_BS1_8tq;               //BTR-TS1 时间段1
//		CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;               //BTR-TS1 时间段2
//		CAN_InitStructure.CAN_Prescaler = 3;                   //BTR-BRP 波特率分频器
//	}
//	
//	
//	CAN_Init(can, &CAN_InitStructure);
//}



/*
 * @function: CAN_Mode_Config
 * @details : 配置CAN模式
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void CAN_Mode_Config(CAN_TypeDef* can, CanBaud_e eCanBaud)
{
	CAN_InitTypeDef        CAN_InitStructure;
	
	//CAN寄存器初始化
	CAN_DeInit(can);
	CAN_StructInit(&CAN_InitStructure);

	//CAN单元初始化
	CAN_InitStructure.CAN_TTCM = DISABLE;                  //MCR-TTCM  关闭时间触发通信模式使能
	
	//为了方便调试，开启自动离线管理
	CAN_InitStructure.CAN_ABOM = ENABLE;                   //MCR-ABOM  自动离线管理。ENABLE-使用该功能，可以在节点出错离线后适时的自动恢复，不需要软件干预。
	
	CAN_InitStructure.CAN_AWUM = ENABLE;                   //MCR-AWUM  自动唤醒模式。ENABLE-使用该功能，可以在监测到总线活动后自动唤醒。
	CAN_InitStructure.CAN_NART = DISABLE;                  //MCR-NART  禁止报文自动重传	  DISABLE-使用自动重传。当仲裁失败后，Can会自动重发。
	CAN_InitStructure.CAN_RFLM = DISABLE;                  //MCR-RFLM  接收FIFO 锁定模式  DISABLE-溢出时新报文会覆盖原有报文  
	CAN_InitStructure.CAN_TXFP = DISABLE;                  //MCR-TXFP  发送FIFO优先级 DISABLE-优先级取决于报文标示符 
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;          //正常工作模式
//	CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack;        //回环模式，用于自测


	//------------计算Can通信波特率-----------------------------------------//
//	CAN 波特率 = RCC_APB1Periph_CAN / Prescaler / (SJW + BS1 + BS2);
//	SJW = synchronisation_jump_width 
//	BS = bit_segment

//	周立功推荐：
//	1.保证TSEG1+TSEG2的时间份额在10-20之间。
//	2.（TSEG1+1）/（TSEG1+TSEG2+1）的采样点位置在75%~81.5%为宜，极限情况下不得在70%~87.5%之外。
//	3.同步跳转SJW取（TSEG2-1）为宜。

//	时间份额（tq）    TSEG1    TSEG2    SJW
//	10               6        3        2
//	12               8        3        2
//	14               9        4        3
//	16               11       4        3
//	18               12       5        3
//	20               14       5        3
	//----------------------------------------------------------------------//
	
	CAN_InitStructure.CAN_SJW = g_taCanCfg[eCanBaud].ulSJW;               //BTR-SJW 重新同步跳跃宽度（不参与波特率计算）
	CAN_InitStructure.CAN_BS1 = g_taCanCfg[eCanBaud].ulSeg1;              //BTR-TS1 时间段1
	CAN_InitStructure.CAN_BS2 = g_taCanCfg[eCanBaud].ulSeg2;               //BTR-TS1 时间段2
	CAN_InitStructure.CAN_Prescaler = g_taCanCfg[eCanBaud].ulPrescaler;    //分频               
	
	CAN_Init(can, &CAN_InitStructure);
}


/*
 * @function: CAN_Filter_Config
 * @details : 配置CAN过滤器。配置多个CanID，未成功。
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void CAN_Filter_Config(CAN_TypeDef* can)
{
	extern __IO LLDParam_t g_tLLDParam;
	extern __IO GlobalParam_t  g_tGlobalParam;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	
	
	//CAN筛选器初始化
	CAN_FilterInitStructure.CAN_FilterNumber = 0;                              //筛选器组0（有0-13组）
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;            //工作在列表模式
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;           //筛选器位宽为单个16位。
	
	//扩展帧，使能筛选器，按照标志的内容进行比对筛选，扩展ID不是如下的就抛弃掉，是的话，会存入FIFO0。
//	CAN_FilterInitStructure.CAN_FilterIdHigh = ((((u32)0x1314<<3) | CAN_ID_EXT | CAN_RTR_DATA) & 0xFFFF0000)>>16;    //要筛选的ID高位 
//	CAN_FilterInitStructure.CAN_FilterIdLow = (((u32)0x1314<<3) | CAN_ID_EXT | CAN_RTR_DATA) & 0xFFFF;               //要筛选的ID低位 
//	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0xFFFF;                     //筛选器高16位每位必须匹配
//	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0xFFFF;                      //筛选器低16位每位必须匹配
	
	//只接收标准数据帧
	//设置标识符寄存器高字节.这里为什么是左移5位呢？标准帧ID只有11位，CanMan.ID是16位变量，高5位无效。
	uint32_t lld_RecvCanID = LLD_Recv_CanID();
	uint32_t mtr_RecvCanID = Recv_CanID();
	CAN_FilterInitStructure.CAN_FilterIdHigh = ((lld_RecvCanID<<21) & 0xFFFF0000)>>16;    //CanID1
	CAN_FilterInitStructure.CAN_FilterIdLow = ((mtr_RecvCanID<<21) & 0xFFFF0000)>>16;  //CanID2，广播ID
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = ((LLD_CAN_BROADCAST_ID_MOTOR<<21) & 0xFFFF0000)>>16;   //CanID3，预留ID
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = ((CAN_BROADCAST_ID_MOTOR <<21) & 0xFFFF0000)>>16;  //CanID4，预留ID
	//CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x00;                                    //CanID3，预留ID
	//CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x00;                                     //CanID4，预留ID                                    //CanID4，预留ID
	
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;       //筛选器被关联到FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;                     //使能筛选器
	
	
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	//CAN通信中断使能，使能接收中断
	CAN_ITConfig(can, CAN_IT_FMP0, ENABLE);
}


/*
 * @function: CAN_NVIC_Config
 * @details : 配置CAN中断向量
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void CAN_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//Configure one bit for preemption priority
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	//中断设置
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;     //CAN1 RX0中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;      //抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;             //子优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
//	NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);

//	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX1_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);

//	NVIC_InitStructure.NVIC_IRQChannel = CAN1_SCE_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	
//	NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);

	
	//CAN通信中断使能，使能fifo0相关中断
//	CAN_ITConfig(CAN1, CAN_IT_FF0 | CAN_IT_FOV0, ENABLE);
//	CAN_ITConfig(CAN1, CAN_IT_WKU | CAN_IT_SLK | CAN_IT_EWG | CAN_IT_EPV, ENABLE);
//	CAN_ITConfig(CAN1, CAN_IT_BOF | CAN_IT_LEC | CAN_IT_ERR , ENABLE);
//	//CAN通信中断使能，使能接收中断
//	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}

/*
 * @function: CAN_Config
 * @details : CAN初始化
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void CAN_Config(CAN_TypeDef* can)
{
	extern __IO GlobalParam_t g_tGlobalParam;
	CAN_Mode_Config(can, g_tGlobalParam.eCanBaud);
	CAN_Filter_Config(can); 
}


/*
 * @function: Init_RxMes
 * @details : 初始化 Rx Message数据结构体
 * @input   : 1.RxMessage：指向要初始化的数据结构体。
 * @output  : NULL
 * @return  : NULL
 */
void Init_RxMes(CanRxMsg *rx_msg)
{
	uint8_t ubCounter = 0;
	
	
	/*把接收结构体清零*/
	rx_msg->StdId = 0x00;
	rx_msg->ExtId = 0x00;
	rx_msg->IDE = CAN_ID_STD;
	rx_msg->DLC = 0;
	rx_msg->FMI = 0;
	
	for(ubCounter = 0; ubCounter < 8; ubCounter++)
	{
		rx_msg->Data[ubCounter] = 0x00;
	}
}





/***** 液面探测部分 ****************************************************/
uint32_t LLD_Recv_CanID(void)
{
	/* 存储在EEPROM中 */
	extern __IO LLDParam_t g_tLLDParam;
	return g_tLLDParam.CanConfig.ModuleID;
}



/**
*  获取当前模块的发送 CAN ID号(Motor)
  */
uint32_t LLD_Send_CanID(void)
{
	/* 存储在EEPROM中 */
	extern __IO LLDParam_t g_tLLDParam;
	return g_tLLDParam.CanConfig.ReplyID;
}



/***** 电机部分 *************************************************************/
/**
  *  获取当前模块的接受 CAN ID号(Motor)
  */
uint32_t Recv_CanID(void)
{
	/* 存储在EEPROM中 */
	
	extern __IO GlobalParam_t 	 g_tGlobalParam;
	return g_tGlobalParam.ulRecvCanID;
}



/**
*  获取当前模块的发送 CAN ID号(Motor)
  */
uint32_t Send_CanID(void)
{
	/* 存储在EEPROM中 */
	extern __IO GlobalParam_t 	 g_tGlobalParam;
	return g_tGlobalParam.ulSendCanID;
}




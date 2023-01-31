/*****************************************************************************
Copyright  : BGI
File name  : monitor_usart.c
Description: 上位机监控
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#include "monitor_usart.h" 
#include "include.h"




//定义变量---------------------------------------------------------------------//
//Modbus管理
struct tagModbusMon    ModbusMon;

//Modbus数据表格
const struct tagPollingPacket ModbusMonPacket[] = //不可超过 DATA_PACKET_MAX
{
	//设备地址         数组类型                  起始地址          长度                        数组指针 
	{0xFF,         MODBUS_INPUT_REG,     0,            ARRAY_SIZE(ModbusMon.InputReg),        ModbusMon.InputReg},
	{0xFF,         MODBUS_HOLDING_REG,   0,            ARRAY_SIZE(ModbusMon.HoldingReg),      ModbusMon.HoldingReg},

	{NULL,         NULL,                         NULL,             NULL,                       NULL},
};




//定义函数---------------------------------------------------------------------//
/*
 * @function: Usart2RecvHandler
 * @details : 对接收到的数据进行校验。
 * @input   : 1.com：串口指针。
 * @output  : NULL
 * @return  : 是否应答，0：否；其他：应答。
 */
uint8_t Usart2RecvHandler(void *com)
{
	uint8_t    is_ask = FALSE;
	uint8_t    low_data = 0;
	uint8_t    high_data = 0;
	uint16_t   crc = 0;
	struct tagModbusMon *mon = (struct tagModbusMon *)com;
	struct tagUartCom* pCom = &mon->Usart;
	
	
	if(pCom->RecvCnt>2)
	{
		crc = BSP_Crc16( pCom->RecvBuf, pCom->RecvCnt-2);
		high_data = crc>>8;
		low_data = crc & 0x00ff;

		if(( low_data == pCom->RecvBuf[pCom->RecvCnt - 2])
			&& ( high_data == pCom->RecvBuf[pCom->RecvCnt - 1]))
		{	
			pCom->OutTime = 0;
			
			is_ask = Usart2Updata((struct tagModbusMon *)com);
		}
	}
	
	return is_ask;
}

/*
 * @function: Usart2SendHandler
 * @details : 将待发送的数据填充发送缓冲区。
 * @input   : 1.com：串口指针。
 * @output  : NULL
 * @return  : 是否发送，0：否；1：是。
 */
uint8_t Usart2SendHandler(void* com)
{
	uint16_t   crc = 0;
	struct tagModbusMon* mon = (struct tagModbusMon *)com;
	struct tagUartCom* pCom = &mon->Usart;
	
	
	
	pCom->SendNum = Usart2FillBuff( mon );	
	if( pCom->SendNum <= 2 )
	{
		return FALSE;
	}
	else
	{
		crc = BSP_Crc16(pCom->SendBuf, pCom->SendNum-2);
		
		pCom->SendBuf[pCom->SendNum-2] = crc;
		pCom->SendBuf[pCom->SendNum-1] = crc>>8;

		return TRUE;		
	}
}

/*
 * @function: Usart2RecvOrSend
 * @details : 控制串口2收发模式。
 * @input   : 1.mode：收发模式。
 * @output  : NULL
 * @return  : NULL
 */ 
void Usart2RecvOrSend(uint8_t mode, struct tagUartCom* pCom)
{
	if( UART_SEND_MODE == mode )
	{
		Usart_SendArray(USART2, pCom->SendBuf, pCom->SendNum);
	}
	else if( UART_REC_MODE == mode )
	{
	}
	else if( UART_CLOSE_MODE == mode)
	{
	}
}

/*
 * @function: Usart2CommInit
 * @details : 初始化Usart。
 * @input   : 。
 * @output  : NULL
 * @return  : 是否发送，0：否；1：是。
 */
void MonModbusCommInit(void)
{
	ModbusMon.Modbus.pTable = ModbusMonPacket;
	
	//调试：目前通信地址暂时设置为1
	ModbusMon.Modbus.VariableAddr = 0x01;
	
	BSP_CommUartInit( &ModbusMon.Usart,
					  Usart2RecvHandler,
					  Usart2SendHandler,
					  Usart2RecvOrSend,
					  NULL,
					  UART_MODE_SLAVE,
					  5,          //接收完成时间
					  2,          //发送前延时
					  5,          //发送完成时间 					  
					  100,        //发送间隔判断时间（判断是否发送)
					  500);       //强制发送时间（主机模式下最大发送间隔）
					  
					  

}




/*
 * @function: Usart2Updata
 * @details : 解析数据。
 * @input   : 1.mon：Modbus参数。
 * @output  : NULL
 * @return  : 是否应答，0：否；其他：应答。。
 */
uint8_t Usart2Updata(struct tagModbusMon *mon)
{
	uint8_t    ret = 0;
	
	
	if(UART_MODE_SLAVE == mon->Usart.CommMode)
	{
		mon->Usart.OutTime = 0;
		
		
		if(mon->Usart.RecvBuf[0] == mon->Modbus.VariableAddr)	
		{		
			if((0x03 == mon->Usart.RecvBuf[1])
				|| (0x04 == mon->Usart.RecvBuf[1])
				|| (0x06 == mon->Usart.RecvBuf[1])
				|| (0x10 == mon->Usart.RecvBuf[1]))
			{					
				ret = ModbusSlaveAnalyse(&mon->Modbus, mon->Usart.RecvBuf);
				
				if((0x10 == mon->Usart.RecvBuf[1]) || (0x06 == mon->Usart.RecvBuf[1]))
				{
					MonWriteHoldingReg(mon);
				}		
			}
		}	
	}
	else 
	{
//		ModbusMasterAnalyse(&mon->Modbus, mon->Usart.SendBuf);	
	}
	
	return (ret);
}

/*
 * @function: Usart2FillBuff
 * @details : 填充发送缓存。
 * @input   : 1.mon：Modbus参数。
 * @output  : NULL
 * @return  : 数据帧长度。
 */
uint8_t Usart2FillBuff(struct tagModbusMon* mon)
{
	uint8_t    ret = 0;
	
		
	if(MODBUS_SLAVE == mon->Usart.CommMode)
	{
		if(0x03 == mon->Modbus.FunctionCode)
		{
			MonReadHoldingReg(mon);
		}
		else if(0x04 == mon->Modbus.FunctionCode)
		{
			MonReadInputReg(mon);
		}
		
		ret = ModbusSlaveSend(&mon->Modbus, mon->Usart.SendBuf);
	}
	else 
	{
//		MonWriteHoldingReg(mon);
//		MonWirteInputReg(mon);
		ret = ModbusMasterSend(&mon->Modbus, mon->Usart.SendBuf); 
	}	
	
	
	return (ret);
}


/*
 * @function: MonReadHoldingReg
 * @details : 监控读Holding寄存器。
 * @input   : 1.mon：Modbus参数。
 * @output  : NULL
 * @return  : NULL
 */
void MonReadHoldingReg(struct tagModbusMon* mon)
{
	uint16_t   temp = 0;
	
	
	//40001，状态寄存器
	mon->HoldingReg[0] = CapSenPara.Buff[0];
	
	
	//40002，ch1数据
	temp = CapSenPara.Buff[1];
	temp = temp<<8;
	temp = temp | CapSenPara.Buff[2];
	mon->HoldingReg[1] = temp;
	
	
	//40003，ch2数据
	temp = 0;
	temp = CapSenPara.Buff[3];
	temp = temp<<8;
	temp = temp | CapSenPara.Buff[4];
	mon->HoldingReg[2] = temp;
	
	
	//40004，ch1平均数据
	temp = 0;
	temp = CapSenPara.Buff[5];
	temp = temp<<8;	
	temp = temp | CapSenPara.Buff[6];
	mon->HoldingReg[3] = temp;

	
	//40005，ch2平均数据
	temp = 0;
	temp = CapSenPara.Buff[7];
	temp = temp<<8;	
	temp = temp | CapSenPara.Buff[8];
	mon->HoldingReg[4] = temp;
	
	
	//40006
	temp = 0;
	temp = CapSenPara.Buff[9];                   //Ch1通道灵敏度（自适应阈值模式）/阈值高电平字节（固定阈值模式）
	temp = temp<<8;	
	temp = temp | CapSenPara.Buff[10];           //Ch1通道超时（自适应阈值模式）/阈值低电平字节（固定阈值模式）
	mon->HoldingReg[5] = temp;	
	
	
	//40007，Ch1通道设置
	mon->HoldingReg[6] = CapSenPara.Buff[11];
	
	
	//40008
	temp = 0;
	temp = CapSenPara.Buff[12];                  //Ch2通道灵敏度（自适应阈值模式）/阈值高电平字节（固定阈值模式）
	temp = temp<<8;	
	temp = temp | CapSenPara.Buff[13];           //Ch2通道超时（自适应阈值模式）/阈值低电平字节（固定阈值模式）
	mon->HoldingReg[7] = temp;
	
	
	//40009，Ch2通道设置
	mon->HoldingReg[8] = CapSenPara.Buff[14];
	
	
	//40010，配置
	mon->HoldingReg[9] = CapSenPara.Buff[15];
	
	
	//40011，关断定时器（用于低功耗场景）
	mon->HoldingReg[10] = CapSenPara.Buff[16];
	
	
	//40012，Ch1使能容性DAC
	mon->HoldingReg[11] = CapSenPara.Buff[17];
	
	
	//40013，Ch2使能容性DAC
	mon->HoldingReg[12] = CapSenPara.Buff[18];
	
	
	//40014，序列号3
	mon->HoldingReg[13] = CapSenPara.Buff[19];
	
	
	//40015，序列号2
	mon->HoldingReg[14] = CapSenPara.Buff[20];
	

	//40016，序列号1
	mon->HoldingReg[15] = CapSenPara.Buff[21];
	

	//40017，序列号0
	mon->HoldingReg[16] = CapSenPara.Buff[22];
	
	
	//40018，ID
	mon->HoldingReg[17] = CapSenPara.Buff[23];
	
	
	
	
	//40019，电容AD值
	mon->HoldingReg[18] = CapSenPara.ChColl[CLLD_CH0_DATA].FilterValue;
	
	//40020，电容值
	mon->HoldingReg[19] = CapSenPara.CapVal;		
}

/*
 * @function: MonReadInputReg
 * @details : 监控读Input寄存器。
 * @input   : 1.mon：Modbus参数。
 * @output  : NULL
 * @return  : NULL
 */
void MonReadInputReg(struct tagModbusMon* mon)
{
}


/*
 * @function: MonWriteHoldingReg
 * @details : 写holding寄存器。
 * @input   : 1.mon：Modbus参数。
 * @output  : NULL
 * @return  : NULL
 */
void MonWriteHoldingReg(struct tagModbusMon* mon)
{
}


/*
 * @function: MonWirteInputReg
 * @details : 将Holding寄存器数据赋值给变量。
 * @input   : 1.mon：Modbus参数。
 * @output  : NULL
 * @return  : NULL
 */
void MonWirteInputReg(struct tagModbusMon* mon)
{
}


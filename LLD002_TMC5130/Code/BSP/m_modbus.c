/*****************************************************************************
Copyright  : BGI
File name  : m_modbus.c
Description: modbus协议
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#include "m_modbus.h" 
#include "include.h"



//定义变量---------------------------------------------------------------------//





//定义函数---------------------------------------------------------------------//
/*
 * @function: WriteModbusReg
 * @details : 收到modbus数据帧，解析数据，将数据填入对应的寄存器。
 * @input   : 1.data_packet：Modbus管理数据结构。
              2.buffer：收到的Modbus数据帧。
 * @output  : NULL
 * @return  : 写结果，0：失败，1：成功。
 */
static uint8_t WriteModbusReg(struct tagDataPacket *data_packet, uint8_t *buffer)
{
	uint8_t    table_line = 0;                   //表格行号（每一行对应一组寄存器）	
	uint8_t    reg_type = 0;                     //寄存器类型
	uint8_t    reg_num = 0;                      //寄存器个数	
	
	uint16_t   *data_p = NULL;
	uint16_t   data_offset = 0;                  //数据偏移
	union tagModbusReg  reg_temp;                //临时转换变量  
	uint8_t    search_start_reg = FALSE;         //寻找寄存器起始地址
	uint8_t    ret = FALSE;
	
	
	if(data_packet->FunctionCode == 0x06)
	{
		data_offset = 4;
		reg_type = MODBUS_HOLDING_REG;
	}
	else if(data_packet->FunctionCode == 0x10)
	{
		data_offset = 7;
		reg_type = MODBUS_HOLDING_REG;
	}
	else if(data_packet->FunctionCode == 0x04)
	{
		data_offset = 3;
		reg_type = MODBUS_INPUT_REG;
	}
	else if(data_packet->FunctionCode == 0x03)
	{
		data_offset = 3;
		reg_type = MODBUS_INPUT_REG;	
	}	
	else
	{
		//return 
	}

	//在Modbus管理表格中找出寄存器
	for(table_line = 0; table_line < BSP_MODBUS_PACKET_TABLE_MAX; table_line++)
	{	
		//遍历表格完毕
		if(data_packet->pTable[table_line].TableType == NULL)
		{			
			break;
		}
		
		//确认设备地址
		if((data_packet->pTable[table_line].DeviceAddr == data_packet->DeviceAddr)    //设备地址写死			
			||((0xff == data_packet->pTable[table_line].DeviceAddr) && (data_packet->VariableAddr == data_packet->DeviceAddr) && (0 != data_packet->VariableAddr))    //可动态设置设备地址
			||(data_packet->DeviceAddr == 0x00))    //广播数据，可以解析。因为从机不会应答00，所以这种情况只有从机收到主机的控制命令
		{
			search_start_reg = TRUE;
		}
			
		if(TRUE == search_start_reg)
		{
			//确认寄存器类型
			if(data_packet->pTable[table_line].TableType != reg_type)
			{
				search_start_reg = FALSE;
			}
		}
		
		if(TRUE == search_start_reg)
		{		
			//确认寄存器地址
			if(((data_packet->StartAddr + data_packet->RegNum) <= data_packet->pTable[table_line].StartAddr)	
				|| ((data_packet->StartAddr + data_packet->RegNum) > ((data_packet->pTable[table_line].StartAddr) + data_packet->pTable[table_line].Length)))
			{
				search_start_reg = FALSE;
			}
		}
		
		//找到起始寄存器地址
		if(TRUE == search_start_reg)
		{			
			break;
		}
	}		
		
	
	//将数据写入寄存器
	if(TRUE == search_start_reg)
	{
		ret = TRUE;
		for(reg_num = 0; reg_num < data_packet->RegNum; reg_num++)
		{
			//数据帧的寄存器起始地址落在modbus管理表格某一行的起始地址与结束地址之间，则表示客户需要访问这一行的寄存器
			if(((data_packet->StartAddr + reg_num) >= data_packet->pTable[table_line].StartAddr)	
				&& ((data_packet->StartAddr + reg_num) < ((data_packet->pTable[table_line].StartAddr) + data_packet->pTable[table_line].Length )))
			{
				//从缓冲中读取数据
				reg_temp.H8L8.H8 = buffer[reg_num * 2 + data_offset] ;	
				reg_temp.H8L8.L8 = buffer[reg_num * 2 + data_offset + 1];
				
				//点表地址在数组中的位置 = 数组起始地址 + （待查询点表地址 - 数组起始对应的点表地址） 
				data_p = data_packet->pTable[table_line].Buffer + (data_packet->StartAddr + reg_num - data_packet->pTable[table_line].StartAddr);
				
				//将数据写入寄存器
				*data_p = reg_temp.TempUInt;	
			}
			else
			{
				ret = FALSE;
			}
		}
	}
	
	
	return (ret);
}

/*
 * @function: ReadModbusReg
 * @details : 读取Modbus寄存器。
 * @input   : 1.data_packet：Modbus管理数据结构。
              2.buffer：发送缓存。
 * @output  : NULL
 * @return  : 读结果，0：失败，1：成功。
 */
static uint8_t ReadModbusReg(struct tagDataPacket* data_packet, uint8_t *buffer)
{
	uint8_t    table_line = 0;                   //表格行号（每一行对应一组寄存器）	
	uint8_t    reg_type = 0;                     //寄存器类型
	uint8_t    reg_num = 0;                      //寄存器个数
	
	uint16_t   *data_p = NULL;
	uint16_t   data_offset = 0;                  //数据偏移
	union tagModbusReg  reg_temp;                //临时转换变量  
	uint8_t    search_start_reg = FALSE;         //寻找寄存器起始地址
	uint8_t    ret = FALSE;	
	
	
	
	if(data_packet->FunctionCode == 0x06)//单写走第四位,多写从第七位开始解析
	{
		data_offset = 4;
		reg_type = MODBUS_HOLDING_REG;
	}
	else if( data_packet->FunctionCode == 0x10 )
	{
		data_offset = 7;
		reg_type = MODBUS_HOLDING_REG;
	}
	else if( data_packet->FunctionCode == 0x04 )//input
	{
		data_offset = 3;
		reg_type = MODBUS_INPUT_REG;
	}
	else if( data_packet->FunctionCode == 0x03 )
	{
		data_offset = 3;
		reg_type = MODBUS_HOLDING_REG;
	}
	else
	{
		//return 
	}
		

	//在Modbus管理表格中找出寄存器
	for(table_line = 0; table_line < BSP_MODBUS_PACKET_TABLE_MAX; table_line++)
	{	
		//遍历表格完毕
		if(data_packet->pTable[table_line].TableType == NULL)
		{			
			break;
		}
		
		//确认设备地址
		if((data_packet->pTable[table_line].DeviceAddr == data_packet->DeviceAddr)    //设备地址写死			
			||((0xff == data_packet->pTable[table_line].DeviceAddr) && (data_packet->VariableAddr == data_packet->DeviceAddr) && (0 != data_packet->VariableAddr))    //可动态设置设备地址
			||(data_packet->DeviceAddr == 0x00))    //广播数据，可以解析。因为从机不会应答00，所以这种情况只有从机收到主机的控制命令
		{
			search_start_reg = TRUE;
		}
			
		if(TRUE == search_start_reg)
		{
			//确认寄存器类型
			if(data_packet->pTable[table_line].TableType != reg_type)
			{
				search_start_reg = FALSE;
			}
		}
		
		if(TRUE == search_start_reg)
		{
			//确认寄存器地址
			if(((data_packet->StartAddr + data_packet->RegNum) <= data_packet->pTable[table_line].StartAddr)	
				|| ((data_packet->StartAddr + data_packet->RegNum) > ((data_packet->pTable[table_line].StartAddr) + data_packet->pTable[table_line].Length)))
			{
				search_start_reg = FALSE;
			}		
		}
		
		//找到起始寄存器地址
		if(TRUE == search_start_reg)
		{			
			break;
		}
	}
	
	
	//将寄存器填入发送缓存
	if(TRUE == search_start_reg)
	{	
		ret = TRUE;
		for(reg_num = 0; reg_num < data_packet->RegNum; reg_num++)
		{
			//数据帧的寄存器起始地址落在modbus管理表格某一行的起始地址与结束地址之间，则表示客户需要访问这一行的寄存器
			if((data_packet->StartAddr + reg_num >= data_packet->pTable[table_line].StartAddr)	
				&&(data_packet->StartAddr + reg_num < ((data_packet->pTable[table_line].StartAddr) + data_packet->pTable[table_line].Length )))
			{
				//读取寄存器数据到缓存
				reg_temp.H8L8.H8 = buffer[reg_num * 2 + data_offset] ;	
				reg_temp.H8L8.L8 = buffer[reg_num * 2 + data_offset + 1];
				
				//点表地址在数组中的位置 = 数组起始地址 + （待查询点表地址 - 数组起始对应的点表地址） 
				data_p = data_packet->pTable[table_line].Buffer + (data_packet->StartAddr + reg_num - data_packet->pTable[table_line].StartAddr);
				
				//将数据写入寄存器
				reg_temp.TempUInt = *data_p;
				
				buffer[reg_num * 2 + data_offset] = reg_temp.H8L8.H8;
				buffer[reg_num * 2 + data_offset + 1] = reg_temp.H8L8.L8;			
			}
			else
			{
				ret = FALSE;
			}
		}
	}
	
	
	return (ret);
}

/*
 * @function: ModbusSlaveAnalyse
 * @details : 解析数据帧。
 * @input   : 1.data_packet：Modbus管理数据结构。
              2.buffer：接收缓存。
 * @output  : NULL
 * @return  : 解析结果，0：失败，1：成功。
 */
uint8_t ModbusSlaveAnalyse(struct tagDataPacket* data_packet, uint8_t *buffer)
{
	uint16_t   max_point_table = MAX_OPERATE_REGISTER_NUM;
	uint8_t    is_ask = FALSE;                   //是否应答
	uint8_t    is_broadcast = TRUE;              //广播包
	uint8_t    table_line = 0;                   //表格行号（每一行对应一组寄存器）	


	
	//不符合设计范围，返回
	if(0xFF == buffer[0])
	{	
		return FALSE;
	}
	
	for(table_line = 0; table_line < BSP_MODBUS_PACKET_TABLE_MAX; table_line++)
	{
		//访问固定地址
		if(data_packet->pTable[table_line].DeviceAddr == buffer[0])
		{
			is_ask = TRUE;
			break;
		}
		
		//访问可动态设置地址
		if((data_packet->pTable[table_line].DeviceAddr == 0xff) 
			&& (buffer[0] == data_packet->VariableAddr) 
			&& (data_packet->VariableAddr != 0))
		{
			is_ask = TRUE;
			break;
		}
		
		
		//没有了或者查到末尾了,用Buffer指针为0判断结尾
		if((data_packet->pTable[table_line].Buffer == NULL)
			|| (table_line == (BSP_MODBUS_PACKET_TABLE_MAX - 1)))
		{
			is_ask = FALSE;//地址不匹配，不应答
			break;
		}
		
		//如果地址是广播包，就不需要再查了，一定解析
		if(buffer[0] == 0x00)
		{
			is_ask = FALSE;
			is_broadcast = TRUE;
			break;
		}
	}
	
	
	//当前设备地址需要应答  
	if((is_ask == TRUE) || (is_broadcast == TRUE))
	{	
		data_packet->DeviceAddr   = buffer[0];    //设备地址
		data_packet->FunctionCode = buffer[1];    //功能码		
		data_packet->StartAddr    = buffer[2];    //起始地址高字节
		data_packet->StartAddr    = data_packet->StartAddr<<8;
		data_packet->StartAddr    = data_packet->StartAddr | buffer[3];    //起始地址低字节
		data_packet->RegNum   = buffer[4];    //数据长度
		data_packet->RegNum   = data_packet->RegNum<<8;
		data_packet->RegNum   = data_packet->RegNum | buffer[5];	
				
		if(data_packet->FunctionCode == 0x06)		//写单个Hoding寄存器   
		{
			data_packet->RegNum = 1;					
			data_packet->DataContent = buffer[4];
			data_packet->DataContent <<= 8;
			data_packet->DataContent |= buffer[5]; 	//单写的值
		}	

		data_packet->ExceptionCode = MODBUS_EXCEPTION_NULL;//默认无异常;								
	
		if((data_packet->RegNum != 0)&&( data_packet->RegNum <= max_point_table ))//操作寄存器个数在范围内
		{
			switch(data_packet->FunctionCode)//功能码
			{
				case 0x04:	//读输入寄存器
				case 0x03:	//读多个保持寄存器
				break;

				case 0x06:	//写单个保持寄存器
				case 0x10:	//写多个保持寄存器
				{
					is_ask = WriteModbusReg(data_packet, buffer);
//					if(data_packet->WriteHoldingCallback != NULL)
//					{
//						//应用层处理
//						data_packet->WriteHoldingCallback(data_packet->StartAddr, data_packet->DataLength, data_packet->DeviceAddr);
//					}
				}
				break;
				
				default:
				{
					//非法功能
					data_packet->ExceptionCode = MODBUS_EXCEPTION_1;
				}
				break;	
			}	
		}
		else
		{
			//超过允许长度
			data_packet->ExceptionCode = MODBUS_EXCEPTION_3;
		}
	}
	
	return is_ask;
}


/*
 * @function: ModbusSlaveAnalyse
 * @details : 解析数据帧。
 * @input   : 1.data_packet：Modbus管理数据结构。
              2.buffer：发送缓存。
 * @output  : NULL
 * @return  : 解析结果，0：失败，1：成功。
 */
uint8_t ModbusSlaveSend(struct tagDataPacket* data_packet, uint8_t* buffer)
{
	uint8_t    ret = 0;//数组填充长度
	
	
	if(data_packet->ExceptionCode == MODBUS_EXCEPTION_NULL)
	{
		switch(data_packet->FunctionCode)
		{
			
			case 0x03://读取 Holding寄存器
			case 0x04://读取 Input寄存器
			{
				buffer[0] = data_packet->DeviceAddr;       //设备地址
				buffer[1] = data_packet->FunctionCode;     //功能码	
				buffer[2] = data_packet->RegNum * 2;   //字节长度
				ReadModbusReg(data_packet,buffer);			
				ret = (data_packet->RegNum * 2 + 5);			
			}
			break;	

			case 0x06://写单个Holding寄存器	
			case 0x10://写多个Holding寄存器
			{
				buffer[0] = data_packet->DeviceAddr;       //设备地址
				buffer[1] = data_packet->FunctionCode;     //功能码						
				buffer[2] = data_packet->StartAddr>>8;
				buffer[3] = data_packet->StartAddr;
				if(0x06 == data_packet->FunctionCode )     //通信状态 为MODBUS协议的06功能码
				{
					buffer[4] = data_packet->DataContent>>8;
					buffer[5] = data_packet->DataContent;
				}
				else
				{
					//通信状态 为MODBUS协议的16功能码
					buffer[4] = data_packet->RegNum>>8;
					buffer[5] = data_packet->RegNum;
				}
				ret = 8; //返回数据长度 + CRC(2)
			}
			break;	
			default:
			{
				
			}
		}
	}
	else
	{
		buffer[0] = data_packet->DeviceAddr;               //设备地址
		buffer[1] = data_packet->FunctionCode | 0x80;      //错误标志功能码
		buffer[2] = data_packet->ExceptionCode;            //异常代码
		ret = 5;	//数据长度+CRC(2)
	}
	
	
	return ret;//返回发送长度
}

/*
 * @function: ModbusSlaveAnalyse
 * @details : 解析数据帧。
 * @input   : 1.data_packet：Modbus管理数据结构。
              2.buffer：发送缓存。
 * @output  : NULL
 * @return  : 解析结果，0：失败，1：成功。
 */
uint8_t API_ModbusIsUpdata(struct tagDataPacket* data_packet, uint16_t reg_num)
{
	if(reg_num >= (data_packet->StartAddr) 
		&& reg_num < (data_packet->StartAddr + data_packet->RegNum))
	{
		return 1;
	}
	
	return 0;
}

/*
 * @function: ModbusMasterAnalyse
 * @details : 主机解析数据帧。
 * @input   : 1.data_packet：Modbus管理数据结构。
              2.buffer：接收缓存。
 * @output  : NULL
 * @return  : 解析结果，0：失败，1：成功。
 */
uint8_t ModbusMasterAnalyse(struct tagDataPacket* data_packet, uint8_t *buffer)
{
	uint8_t    is_ask_ok = FALSE;                //应答是否正确
	uint8_t    table_line = 0;                   //表格行号（每一行对应一组寄存器）	
	uint16_t   temp = 0;
	
	
	
	//1.检查通信地址是否正确
	if(data_packet->pTable[table_line].DeviceAddr == buffer[0])
	{
		//访问固定地址
		is_ask_ok = TRUE;
	}
	if((data_packet->pTable[table_line].DeviceAddr == 0xff) 
		&& (buffer[0] == data_packet->VariableAddr) 
		&& (data_packet->VariableAddr != 0))
	{
		//访问可动态设置地址
		is_ask_ok = TRUE;
	}	
	
	
	//2.检查功能码、寄存器地址、寄存器数量等是否正确
	switch(data_packet->FunctionCode)
	{
		case 0x03://读取 Holding寄存器
		case 0x04://读取 Input寄存器
		{
			if((data_packet->RegNum * 2) != buffer[2])
			{
				is_ask_ok = FALSE;
			}			
		}
		break;

		case 0x06://写单个Holding寄存器	
		{
			temp = MERGE_UINT16(buffer[2], buffer[3]);
			if(temp != data_packet->StartAddr)
			{
				is_ask_ok = FALSE;
			}
			
			temp = MERGE_UINT16(buffer[4], buffer[5]);
			if(temp != data_packet->DataContent)
			{
				is_ask_ok = FALSE;
			}
		}
		break;
		
		case 0x10://写多个Holding寄存器
		{				
			temp = MERGE_UINT16(buffer[2], buffer[3]);
			if(temp != data_packet->StartAddr)
			{
				is_ask_ok = FALSE;
			}
			
			temp = MERGE_UINT16(buffer[4], buffer[5]);
			if(temp != data_packet->DataContent)
			{
				is_ask_ok = FALSE;
			}	
			
			if((data_packet->RegNum * 2) != buffer[6])
			{
				is_ask_ok = FALSE;
			}					
		}
		break;		
		
		default:
		{
			//非法功能
			is_ask_ok = FALSE;		
		}
	}
	
	
	return is_ask_ok;
}


/*
 * @function: ModbusMasterSend
 * @details : 主机解析数据帧。
 * @input   : 1.data_packet：Modbus管理数据结构。
              2.buffer：发送缓存。
 * @output  : NULL
 * @return  : 解析结果，0：失败，1：成功。
 */
uint8_t ModbusMasterSend(struct tagDataPacket* data_packet, uint8_t* buffer)
{
	uint8_t    ret = 0;//数组填充长度
	
	
	switch(data_packet->FunctionCode)
	{
		
		case 0x03://读取 Holding寄存器
		case 0x04://读取 Input寄存器
		{
			data_packet->DeviceAddr = buffer[0];       //设备地址
			data_packet->FunctionCode = buffer[1];     //功能码	
			data_packet->RegNum = buffer[2] / 2;   //寄存器数量			
		}
		break;

		case 0x06://写单个Holding寄存器	
		{
			data_packet->DeviceAddr = buffer[0];       //设备地址
			data_packet->FunctionCode = buffer[1];     //功能码					
			data_packet->StartAddr = MERGE_UINT16(buffer[2], buffer[3]);
			data_packet->DataContent = MERGE_UINT16(buffer[4], buffer[5]);
			
			//返回数据长度 + CRC(2)
			ret = 8;
		}
		break;
		
		case 0x10://写多个Holding寄存器
		{
			data_packet->DeviceAddr = buffer[0];       //设备地址
			data_packet->FunctionCode = buffer[1];     //功能码					
			data_packet->StartAddr = MERGE_UINT16(buffer[2], buffer[3]);
			data_packet->RegNum = MERGE_UINT16(buffer[4], buffer[5]);
			
			//返回数据长度 + CRC(2)
			ret = 8;
		}
		break;		
		
		default:
		{
		}
	}
	
	
	return ret;//返回发送长度
}


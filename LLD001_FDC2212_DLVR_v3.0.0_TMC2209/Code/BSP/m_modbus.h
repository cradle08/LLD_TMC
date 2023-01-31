/*****************************************************************************
Copyright  : BGI
File name  : m_modbus.h
Description: 定时器
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#ifndef __M_MODBUS_H
#define __M_MODBUS_H


#include "stm32f10x.h"





//宏定义----------------------------------------------------------------------//
//MODBUS故障码
#define  MODBUS_EXCEPTION_NULL         0        //无异常
#define  MODBUS_EXCEPTION_1            1        //非法功能码
#define  MODBUS_EXCEPTION_2            2        //非法数据地址
#define  MODBUS_EXCEPTION_3            3        //非法数据值（超过寄存器最大可操作长度）（数据值不判断了）
#define  MODBUS_EXCEPTION_4            4        //从设备故障 

//Modbus主从模式
#define  MODBUS_SLAVE                  0        //从模式
#define  MODBUS_MASTER                 1        //主模式

//Modbus寄存器
#define  MODBUS_INPUT_REG              0x04     //input
#define  MODBUS_HOLDING_REG            0x03     //holding


//Modbus管理表格最大管理数据行数
#define  BSP_MODBUS_PACKET_TABLE_MAX   20		//modbus包中表格最大数（表中最多有多少行）


//部分设置参数
#define  MAX_OPERATE_REGISTER_NUM      80       //最大寄存器可操作数 （由发送/接收缓冲区决定）





//定义结构体--------------------------------------------------------------------//
//Modbus数据管理表格
struct tagPollingPacket
{
	uint8_t    DeviceAddr;                                                     //归属设备地址		
	uint8_t    TableType;                                                      //点表数组类型
 	uint16_t   StartAddr;                                                      //点表起始地址
 	uint16_t   Length;                                                         //点表数据长度
 	uint16_t   *Buffer;                                                        //对应数组指针   
};

//Modbus数据包 
struct tagDataPacket 
{
	const struct tagPollingPacket    *pTable;                                  //总数据包指针
	uint8_t    VariableAddr;                                                   //Modbus通信地址（可动态设置）
	uint16_t   StartAddr;                                                      //寄存器起始地址   
	uint16_t   RegNum;                                                         //寄存器操作数目
	uint8_t    FunctionCode;                                                   //功能码
	uint8_t    DeviceAddr;                                                     //设备地址		
	uint16_t   DataContent;                                                    //信息格式   在预设单个Hoding寄存器时返回的是预设值,不能返回实际值
	uint8_t    ExceptionCode;                                                  //异常码    	 
	uint8_t    PresentRegisterPacketNum;                                       //当前包数组的偏移量 相当于table[num]	
};

//定义取位转换变量（注意大小端）
union tagModbusReg
{
	struct
	{
		uint8_t     L8;	
		uint8_t     H8;
	}H8L8;
	
	uint16_t    TempUInt;
};



//声明变量----------------------------------------------------------------------//



//声明函数----------------------------------------------------------------------//
uint8_t ModbusSlaveAnalyse(struct tagDataPacket* data_packet, uint8_t *buffer);
uint8_t ModbusSlaveSend(struct tagDataPacket* data_packet, uint8_t* buffer);
uint8_t ModbusMasterAnalyse(struct tagDataPacket *data_packet, uint8_t* buffer);
uint8_t ModbusMasterSend(struct tagDataPacket* data_packet, uint8_t* buffer);


#endif



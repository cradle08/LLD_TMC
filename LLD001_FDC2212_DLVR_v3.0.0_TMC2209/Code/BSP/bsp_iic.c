/*****************************************************************************
Copyright  : BGI
File name  : bsp_iic.c
Description: iic通信，从野火移植而来。
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/ 
#include "bsp_iic.h"   
#include "include.h"





//定义变量---------------------------------------------------------------------//
static __IO uint32_t  I2CTimeout = I2CT_LONG_TIMEOUT; 



//定义函数---------------------------------------------------------------------//
/*
 * @function: IIC1_GPIO_Config
 * @details : 配置IIC1控制器的管脚
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
static void IIC1_GPIO_Config(void)
{	
	GPIO_InitTypeDef  GPIO_InitStructure; 

	//使能iic时钟（使用内部HSI时钟，导致APB1的时钟频率变为32MHz
	//（使用HSE时钟的话，APB1的时钟频率是36MHz.），是否对IIC通信产生影响？？？）
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);


	//I2C_SCL
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;	
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//I2C_SDA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}


/*
 * @function: IIC1_Mode_Configu
 * @details : 配置iic
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
static void IIC1_Mode_Configu(void)
{
	I2C_InitTypeDef  I2C_InitStructure; 

	//I2C配置
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;

	//高电平数据稳定，低电平数据变化 SCL 时钟线的占空比
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;

	I2C_InitStructure.I2C_OwnAddress1 = I2Cx_OWN_ADDRESS7; 
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable ;
	 
	//I2C的寻址模式
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

	//通信速率
	I2C_InitStructure.I2C_ClockSpeed = IIC1_SPEED;

	//I2C初始化
	I2C_Init(I2C1, &I2C_InitStructure);

	//使能 I2C
	I2C_Cmd(I2C1, ENABLE); 
}

/*
 * @function: IIC1_Init
 * @details : 初始化iic1
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void IIC1_Init(void)
{
	IIC1_GPIO_Config(); 
	IIC1_Mode_Configu();
}



/*
 * @function: IIC1_GPIO_Config
 * @details : 配置IIC1控制器的管脚
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
static void IIC2_GPIO_Config(void)
{	
	GPIO_InitTypeDef  GPIO_InitStructure; 
	
	
	//只能在此打开IIC2时钟，否则导致IIC2信号线异常。
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	//打开信号线引脚时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	
	//I2C_SCL、I2C_SDA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*
 * @function: IIC2_Mode_Configu
 * @details : 配置iic
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
static void IIC2_Mode_Configu(void)
{
	I2C_InitTypeDef  I2C_InitStructure; 
	
	
	//在此打开IIC2时钟，会导致IIC2信号线异常。
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	
	//I2C配置
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;

	//高电平数据稳定，低电平数据变化 SCL 时钟线的占空比
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;

	I2C_InitStructure.I2C_OwnAddress1 = I2Cx_OWN_ADDRESS7; 
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	 
	//I2C的寻址模式
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

	//通信速率
	I2C_InitStructure.I2C_ClockSpeed = IIC2_SPEED;

	//I2C初始化
	I2C_Init(I2C2, &I2C_InitStructure);

	//使能 I2C
	I2C_Cmd(I2C2, ENABLE);
}

/*
 * @function: IIC2_Init
 * @details : 初始化iic1
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void IIC2_Init(void)
{
	IIC2_GPIO_Config();
	IIC2_Mode_Configu();
}


/*
 * @function: IIC2_Reset
 * @details : IIC2重置
              1、当硬件IIC重新初始化之后，不能立即使用，需要等待若干毫秒之后再使用。
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void IIC2_Reset(void)
{
	I2C_DeInit(I2C2);
	
	//软件复位IIC，在复位该位前确信I2C的引脚被释放，总线是空的。
	//解决动态初始化IIC时，总线工作不正常问题。
//	I2C_SoftwareResetCmd(I2C2, ENABLE);
//	GenDelay_nop(10);
	
	IIC2_GPIO_Config();
	IIC2_Mode_Configu();
}



/*
 * @function: I2C_TIMEOUT_UserCallback
 * @details : 错误代码，可以用来定位是哪个环节出错.
 * @input   : NULL
 * @output  : NULL
 * @return  : 返回0，表示IIC读取失败.
 */
static uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode)
{
	//Block communication and all processes
//	USART2_printf("I2C 等待超时!errorCode = %d",errorCode);

	return errorCode;
}


/*
 * @function: I2C_Mem_Write
 * @details : IIC写入数据
 * @input   : 1.I2Cx：IIC设备
              2.DevAddress：通信设备地址。
              3.pBuffer：缓冲区指针。
			  4.WriteAddr：寄存器地址。
			  5.NumByteToWrite：写入字节数。
 * @output  : NULL
 * @return  : 操作结果，1：成功。其他：表示IIC读取失败.
 */
uint32_t I2C_Mem_Write(I2C_TypeDef* I2Cx, uint16_t DevAddress, uint8_t* pBuffer, uint8_t WriteAddr, uint8_t NumByteToWrite)
{
	I2CTimeout = I2CT_LONG_TIMEOUT;

	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))   
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(4);
	}
	
	//1.Send START condition
	I2C_GenerateSTART(I2Cx, ENABLE);
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	//Test on EV5 and clear it
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))  
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(5);
	}
	
	
	//2.Send EEPROM address for write
	I2C_Send7bitAddress(I2Cx, DevAddress, I2C_Direction_Transmitter);

	I2CTimeout = I2CT_FLAG_TIMEOUT;
	//Test on EV6 and clear it
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))  
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(6);
	}
	
	
	//3.Send the EEPROM's internal address to write to   
	I2C_SendData(I2Cx, WriteAddr);

	I2CTimeout = I2CT_FLAG_TIMEOUT;
	//Test on EV8 and clear it
	while(! I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(7);
	} 
	
	
	//While there is data to be written
	while(NumByteToWrite--)  
	{
		/* Send the current byte */
		I2C_SendData(I2Cx, *pBuffer); 

		/* Point to the next byte to be written */
		pBuffer++; 

		I2CTimeout = I2CT_FLAG_TIMEOUT;

		/* Test on EV8 and clear it */
		while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		{
			if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(8);
		} 
	}

	/* Send STOP condition */
	I2C_GenerateSTOP(I2Cx, ENABLE);

	return 1;
}


/*
 * @function: I2C_Mem_Read
 * @details : IIC读取数据
 * @input   : 1.I2Cx：IIC设备
              2.DevAddress：通信设备地址。
              3.pBuffer：缓冲区指针。
			  4.ReadAddr：寄存器地址。
			  5.NumByteToRead：读取字节数。
 * @output  : NULL
 * @return  : 操作结果，1：成功。其他：表示IIC读取失败.
 */
uint32_t I2C_Mem_Read(I2C_TypeDef* I2Cx, uint16_t DevAddress, uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
{
	I2CTimeout = I2CT_LONG_TIMEOUT;

	//*((u8 *)0x4001080c) |=0x80; 
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(9);
	}
	
	
	//1.Send START condition
	I2C_GenerateSTART(I2Cx, ENABLE);
	//*((u8 *)0x4001080c) &=~0x80;

	I2CTimeout = I2CT_FLAG_TIMEOUT;
	//Test on EV5 and clear it
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(10);
	}
	
	
	//2.Send EEPROM address for write
	I2C_Send7bitAddress(I2Cx, DevAddress, I2C_Direction_Transmitter);

	I2CTimeout = I2CT_FLAG_TIMEOUT;
	//Test on EV6 and clear it
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(11);
	}
	
	
	//Clear EV6 by setting again the PE bit
	I2C_Cmd(I2Cx, ENABLE);
	
	
	//3.Send the EEPROM's internal address to write to
	I2C_SendData(I2Cx, ReadAddr);  
	
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	//Test on EV8 and clear it
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(12);
	}
	
	
	//4.Send STRAT condition a second time  
	I2C_GenerateSTART(I2Cx, ENABLE);

	I2CTimeout = I2CT_FLAG_TIMEOUT;
	//Test on EV5 and clear it
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(13);
	}
	
	
	//5.Send EEPROM address for read
	I2C_Send7bitAddress(I2Cx, DevAddress, I2C_Direction_Receiver);

	I2CTimeout = I2CT_FLAG_TIMEOUT;
	//Test on EV6 and clear it
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(14);
	}
	
	
	//While there is data to be read
	while(NumByteToRead)  
	{
		if(NumByteToRead == 1)
		{
		//Disable Acknowledgement
		I2C_AcknowledgeConfig(I2Cx, DISABLE);

		//Send STOP Condition
		I2C_GenerateSTOP(I2Cx, ENABLE);
		}

		//Test on EV7 and clear it   
		I2CTimeout = I2CT_LONG_TIMEOUT;

		while(I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)==0)  
		{
			if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(3);
		}
		
		{      
			//Read a byte from the EEPROM
			*pBuffer = I2C_ReceiveData(I2Cx);

			//Point to the next location where the byte read will be saved
			pBuffer++; 

			//Decrement the read bytes counter
			NumByteToRead--;        
		}   
	}

	//Enable Acknowledgement to be ready for another reception
	I2C_AcknowledgeConfig(I2Cx, ENABLE);

	return 1;
}


/*
 * @function: I2C_Mem_Read2
 * @details : IIC读取数据
 * @input   : 1.I2Cx：IIC设备
              2.DevAddress：通信设备地址。
              3.pBuffer：缓冲区指针。
			  4.ReadAddr：寄存器地址。
			  5.NumByteToRead：读取字节数。
 * @output  : NULL
 * @return  : 操作结果，1：成功。其他：表示IIC读取失败.
 */
uint32_t I2C_Mem_Read2(I2C_TypeDef* I2Cx, uint16_t DevAddress, uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
{
	I2CTimeout = I2CT_LONG_TIMEOUT;
	
	
	//*((u8 *)0x4001080c) |=0x80; 
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(9);
	}
	
	
	//1.Send START condition
	I2C_GenerateSTART(I2Cx, ENABLE);
	//*((u8 *)0x4001080c) &=~0x80;
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	//Test on EV5 and clear it
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(10);
	}
	

	//5.Send EEPROM address for read
	I2C_Send7bitAddress(I2Cx, DevAddress, I2C_Direction_Receiver);
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	//Test on EV6 and clear it
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(14);
	}
	


	//While there is data to be read
	while(NumByteToRead)  
	{
		if(NumByteToRead == 1)
		{
		//Disable Acknowledgement
		I2C_AcknowledgeConfig(I2Cx, DISABLE);

		//Send STOP Condition
		I2C_GenerateSTOP(I2Cx, ENABLE);
		}

		//Test on EV7 and clear it   
		I2CTimeout = I2CT_LONG_TIMEOUT;

		while(I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)==0)  
		{
			if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(3);
		}
		
		{      
			//Read a byte from the EEPROM
			*pBuffer = I2C_ReceiveData(I2Cx);

			//Point to the next location where the byte read will be saved
			pBuffer++; 

			//Decrement the read bytes counter
			NumByteToRead--;        
		}   
	}

	//Enable Acknowledgement to be ready for another reception
	I2C_AcknowledgeConfig(I2Cx, ENABLE);

	return 1;
}


/*
 * @function: I2C_CheckDevice
 * @details : 查询通信设备是否存在
 * @input   : 1.I2Cx：IIC设备
              2.DevAddress：通信设备地址。
 * @output  : NULL
 * @return  : 操作结果，1：成功。其他：表示IIC读取失败.
 */
uint32_t I2C_CheckDevice(I2C_TypeDef* I2Cx, uint16_t DevAddress, uint8_t rw)
{
	I2CTimeout = I2CT_LONG_TIMEOUT;
	
	
	//*((u8 *)0x4001080c) |=0x80; 
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(9);
	}
	
	
	//1.Send START condition
	I2C_GenerateSTART(I2Cx, ENABLE);
	//*((u8 *)0x4001080c) &=~0x80;
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	//Test on EV5 and clear it
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(10);
	}
	

	//5.Send EEPROM address for write/read
	I2C_Send7bitAddress(I2Cx, DevAddress, rw);
	I2CTimeout = I2CT_FLAG_TIMEOUT;
	//Test on EV6 and clear it
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
	{
		if((I2CTimeout--) == 0) return I2C_TIMEOUT_UserCallback(14);
	}
	
	
	//Disable Acknowledgement
	I2C_AcknowledgeConfig(I2Cx, DISABLE);

	//Send STOP Condition
	I2C_GenerateSTOP(I2Cx, ENABLE);	
	

	//Enable Acknowledgement to be ready for another reception
	I2C_AcknowledgeConfig(I2Cx, ENABLE);

	return 1;
}


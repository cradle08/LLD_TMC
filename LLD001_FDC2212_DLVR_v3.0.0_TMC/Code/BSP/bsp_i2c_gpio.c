/*****************************************************************************
Copyright  : BGI
File name  : bsp_i2c_gpio.c
Description: iic模拟通信，从野火移植而来。
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#include "bsp_i2c_gpio.h"
#include "stm32f10x.h"
#include "bsp_misc.h" 




//定义变量---------------------------------------------------------------------//
struct tagIICDriver    GPIOIIC[IIC_NUM];



//定义函数---------------------------------------------------------------------//
/*
 * @function: i2c1_CfgGpio
 * @details : 配置IIC通信引脚
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void i2c1_CfgGpio(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_I2C_PORT, ENABLE);          //打开GPIO时钟

	GPIO_InitStructure.GPIO_Pin = I2C1_SCL_PIN | I2C1_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;       //开漏输出
	GPIO_Init(GPIO_PORT_I2C, &GPIO_InitStructure);
}

/*
 * @function: i2c_CfgGpio
 * @details : 配置IIC通信引脚
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void i2c2_CfgGpio(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_I2C_PORT, ENABLE);	//打开GPIO时钟
	
	GPIO_InitStructure.GPIO_Pin = I2C2_SCL_PIN | I2C2_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;       //野火使用开漏输出
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       //原子使用推完输出。
	GPIO_Init(GPIO_PORT_I2C, &GPIO_InitStructure);
}



/*
 * @function: I2C1_SCL
 * @details : IIC1时钟输出
 * @input   : dat：高低电平值
 * @output  : NULL
 * @return  : NULL
 */
uint8_t I2C1_SCL(uint8_t dat)
{
	if(0 == dat)
	{
		GPIO_ResetBits(GPIO_PORT_I2C, I2C1_SCL_PIN);
	}
	else
	{
		GPIO_SetBits(GPIO_PORT_I2C, I2C1_SCL_PIN);
	}
	
	return (0);
}


/*
 * @function: I2C1_SDA_Write
 * @details : IIC1写数据
 * @input   : dat：数据
 * @output  : NULL
 * @return  : NULL
 */
uint8_t I2C1_SDA_Write(uint8_t dat)
{
	if(0 == dat)
	{
		GPIO_ResetBits(GPIO_PORT_I2C, I2C1_SDA_PIN);
	}
	else
	{
		GPIO_SetBits(GPIO_PORT_I2C, I2C1_SDA_PIN);
	}
	
	return (0);
}


/*
 * @function: I2C1_SDA_Read
 * @details : IIC1读数据
 * @input   : dat：数据
 * @output  : NULL
 * @return  : NULL
 */
uint8_t I2C1_SDA_Read(void)
{
	return (GPIO_ReadInputDataBit(GPIO_PORT_I2C, I2C1_SDA_PIN));
}


/*
 * @function: I2C2_SCL
 * @details : IIC2时钟输出
 * @input   : dat：高低电平值
 * @output  : NULL
 * @return  : NULL
 */
uint8_t I2C2_SCL(uint8_t dat)
{
	if(0 == dat)
	{
		GPIO_ResetBits(GPIO_PORT_I2C, I2C2_SCL_PIN);
	}
	else
	{
		GPIO_SetBits(GPIO_PORT_I2C, I2C2_SCL_PIN);
	}
	
	return (0);
}


/*
 * @function: I2C2_SDA_Write
 * @details : IIC2写数据
 * @input   : dat：数据
 * @output  : NULL
 * @return  : NULL
 */
uint8_t I2C2_SDA_Write(uint8_t dat)
{
	if(0 == dat)
	{
		GPIO_ResetBits(GPIO_PORT_I2C, I2C2_SDA_PIN);
	}
	else
	{
		GPIO_SetBits(GPIO_PORT_I2C, I2C2_SDA_PIN);
	}
	
	return (0);
}


/*
 * @function: I2C2_SDA_Read
 * @details : IIC2读数据
 * @input   : dat：数据
 * @output  : NULL
 * @return  : NULL
 */
uint8_t I2C2_SDA_Read(void)
{
	return (GPIO_ReadInputDataBit(GPIO_PORT_I2C, I2C2_SDA_PIN));
}


/*
 * @function: GPIO_IIC_Init
 * @details : 模拟iic初始化
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void GPIO_IIC1_Init(void)
{
//	i2c1_CfgGpio();
		
	//iic1
	GPIOIIC[IIC_1].GPIO_Cfg = i2c1_CfgGpio;
	GPIOIIC[IIC_1].SCL_Out = I2C1_SCL;
	GPIOIIC[IIC_1].SDA_Write = I2C1_SDA_Write;
	GPIOIIC[IIC_1].SDA_Read = I2C1_SDA_Read;
	GPIOIIC[IIC_1].State = IIC_RESET_DEV;
}


/*
 * @function: GPIO_IIC_Init
 * @details : 模拟iic初始化
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void GPIO_IIC2_Init(void)
{
//	i2c2_CfgGpio();
	
	//iic2
	GPIOIIC[IIC_2].GPIO_Cfg = i2c2_CfgGpio;
	GPIOIIC[IIC_2].SCL_Out = I2C2_SCL;
	GPIOIIC[IIC_2].SDA_Write = I2C2_SDA_Write;
	GPIOIIC[IIC_2].SDA_Read = I2C2_SDA_Read;
	GPIOIIC[IIC_2].State = IIC_RESET_DEV;
	GPIOIIC[IIC_2].Dev1Err = FALSE;
	GPIOIIC[IIC_2].Dev2Err = FALSE;
	GPIOIIC[IIC_2].Dev1CommReset = FALSE;
	GPIOIIC[IIC_2].Dev2CommReset = FALSE;
}








/*
 * @function: i2c_Delay
 * @details : I2C总线位延迟，最快400KHz
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void i2c_Delay(void)
{
	uint8_t i;

	/*　
	 	下面的时间是通过逻辑分析仪测试得到的。
    工作条件：CPU主频72MHz ，MDK编译环境，1级优化
  
		循环次数为10时，SCL频率 = 205KHz 
		循环次数为7时，SCL频率 = 347KHz， SCL高电平时间1.5us，SCL低电平时间2.87us 
	 	循环次数为5时，SCL频率 = 421KHz， SCL高电平时间1.25us，SCL低电平时间2.375us 
	*/
	for(i = 0; i < 20; i++);
}


/*
 * @function: i2c_Start
 * @details : CPU发起I2C总线启动信号
 * @input   : iic：通信接口
 * @output  : NULL
 * @return  : NULL
 */
void i2c_Start(struct tagIICDriver *iic)
{
	//当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号
	iic->SDA_Write(1);
	iic->SCL_Out(1);
	i2c_Delay();
	
	iic->SDA_Write(0);
	i2c_Delay();
	
	iic->SCL_Out(0);
	i2c_Delay();
}

/*
 * @function: i2c_Stop
 * @details : CPU发起I2C总线停止信号
 * @input   : iic：通信接口
 * @output  : NULL
 * @return  : NULL
 */
void i2c_Stop(struct tagIICDriver *iic)
{
	//当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号
	iic->SDA_Write(0);
	iic->SCL_Out(1);
	i2c_Delay();
	
	iic->SDA_Write(1);
}


/*
 * @function: i2c_WaitAck
 * @details : CPU产生一个时钟，并读取器件的ACK应答信号
 * @input   : iic：通信接口
 * @output  : NULL
 * @return  : 返回0表示正确应答，1表示无器件响应
 */
uint8_t i2c_WaitAck(struct tagIICDriver *iic)
{
	uint8_t    ret = 0;
	
	
	//CPU释放SDA总线
	iic->SDA_Write(1);
	i2c_Delay();
	
	//CPU驱动SCL = 1, 此时器件会返回ACK应答
	iic->SCL_Out(1);
	i2c_Delay();
	
	//CPU读取SDA口线状态
	ret = iic->SDA_Read();
	
	iic->SCL_Out(0);
	i2c_Delay();
	
	
	return ret;
}


/*
 * @function: i2c_Ack
 * @details : CPU产生一个ACK信号
 * @input   : iic：通信接口
 * @output  : NULL
 * @return  : NULL
 */
void i2c_Ack(struct tagIICDriver *iic)
{
	//CPU驱动SDA = 0
	iic->SDA_Write(0);
	i2c_Delay();
	
	//CPU产生1个时钟
	iic->SCL_Out(1);	
	i2c_Delay();
	iic->SCL_Out(0);
	i2c_Delay();
	
	//CPU释放SDA总线
	iic->SDA_Write(1);
}

/*
 * @function: i2c_NAck
 * @details : CPU产生1个NACK信号
 * @input   : iic：通信接口
 * @output  : NULL
 * @return  : NULL
 */
void i2c_NAck(struct tagIICDriver *iic)
{
	//CPU驱动SDA = 1
	iic->SDA_Write(1);
	i2c_Delay();
	
	//CPU产生1个时钟
	iic->SCL_Out(1);
	i2c_Delay();
	iic->SCL_Out(0);
	i2c_Delay();	
}


/*
 * @function: i2c_SendByte
 * @details : CPU向I2C总线设备发送8bit数据
 * @input   : 1.iic：通信接口
              2.dat：等待发送的字节
 * @output  : NULL
 * @return  : NULL
 */
void i2c_SendByte(struct tagIICDriver *iic, uint8_t dat)
{
	uint8_t    i = 0;
	
	
	//先发送字节的高位bit7
	for(i = 0; i < 8; i++)
	{		
		if(dat & 0x80)
		{
			iic->SDA_Write(1);
		}
		else
		{
			iic->SDA_Write(0);
		}
		
		i2c_Delay();
		iic->SCL_Out(1);
		i2c_Delay();	
		iic->SCL_Out(0);
		if(i == 7)
		{
			// 释放总线
			iic->SDA_Write(1);
		}
		
		//左移一个bit
		dat = dat<<1;
		i2c_Delay();
	}
}


/*
 * @function: i2c_ReadByte
 * @details : CPU从I2C总线设备读取8bit数据
 * @input   : 1.iic：通信接口
 * @output  : NULL
 * @return  : 读到的数据
 */
uint8_t i2c_ReadByte(struct tagIICDriver *iic)
{
	uint8_t    i = 0;
	uint8_t    value = 0;
	
	
	//读到第1个bit为数据的bit7，数据由高bit->低bit传输
	for(i = 0; i < 8; i++)
	{
		value = value<<1;
		iic->SCL_Out(1);
		i2c_Delay();
		if(iic->SDA_Read())
		{
			value++;
		}
		iic->SCL_Out(0);
		i2c_Delay();
	}
	
	return value;
}











/*
 * @function: i2c_CheckDevice
 * @details : 检测I2C总线设备，CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
 * @input   : 1.iic：通信接口
              2.dev_addr：设备的I2C总线地址.
 * @output  : NULL
 * @return  : 返回值，0：表示正确；1：表示未探测到
 */
uint8_t i2c_CheckDevice(struct tagIICDriver *iic, uint8_t dev_addr)
{
	uint8_t    ucAck = 0;
	
	
	//发送启动信号
	i2c_Start(iic);

	//发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传
	i2c_SendByte(iic, dev_addr | I2C_WR);
	
	//检测设备的ACK应答
	ucAck = i2c_WaitAck(iic);	

	//发送停止信号
	i2c_Stop(iic);
	
	//若输入的是读地址，需要产生非应答信号
//	i2c_NAck(iic);	
	
	
	return ucAck;
}

/*
 * @function: i2c_CheckDevice
 * @details : 检测I2C总线设备，CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
 * @input   : 1.iic：通信接口
              2.dev_addr：设备的I2C总线地址.
 * @output  : NULL
 * @return  : 返回值，0：表示正确；1：表示未探测到
 */
uint8_t i2c_CheckDevice2(struct tagIICDriver *iic, uint8_t dev_addr)
{
	uint8_t    ucAck = 0;
	
	
	//发送启动信号
	i2c_Start(iic);

	//发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传
	i2c_SendByte(iic, dev_addr | I2C_RD);
	
	//检测设备的ACK应答
	ucAck = i2c_WaitAck(iic);	

	//发送停止信号
	i2c_Stop(iic);
	
	//若输入的是读地址，需要产生非应答信号
	i2c_NAck(iic);	
	
	
	return ucAck;
}





/*
 * @function: i2c_WaitStandby
 * @details : 等待目标设备进入准备状态。在写入数据后，必须调用本函数
 * @input   : 1.iic：通信接口
              2.dev_addr：设备的I2C总线地址.
 * @output  : NULL
 * @return  : 返回值，0：表示正常；1：表示等待超时
 */
uint8_t i2c_WaitStandby(struct tagIICDriver *iic, uint8_t dev_addr)
{
	uint32_t    wait_count = 0;
	
	
	while(i2c_CheckDevice(iic, dev_addr))
	{
		//若检测超过次数，退出循环
		if(wait_count > 0xFFFF)
		{
			//等待超时
			return 1;
		}
		
		wait_count++;
	}
	
	//等待完成
	return 0;
}


/*
 * @function: i2c_ReadBytes
 * @details : 从串行EEPROM指定地址处开始读取若干数据
 * @input   : 1.iic：通信接口
              2.dev_addr：设备的I2C总线地址.
              3.reg_addr
              4.buf: 存放读到的数据的缓冲区指针。
              5.size: 数据长度，单位为字节。			  
 * @output  : NULL
 * @return  : 返回值，0：表示成功；1：表示失败。
 */
uint8_t i2c_ReadBytes(struct tagIICDriver *iic, uint8_t dev_addr, uint16_t reg_addr, uint8_t *buf, uint16_t size)
{
	uint16_t    i = 0;
	
	
	//采用串行EEPROM随即读取指令序列，连续读取若干字节
	
	//第1步：发起I2C总线启动信号
	i2c_Start(iic);
	
	//第2步：发起控制字节，bit7-1是地址；bit0是读写控制位，0表示写，1表示读
	i2c_SendByte(iic, dev_addr | I2C_WR);
	 
	//第3步：等待ACK
	if(i2c_WaitAck(iic) != 0)
	{
		//EEPROM器件无应答
		goto cmd_fail;
	}

	//第4步：发送字节地址，24C02只有256字节，因此1个字节就够了，如果是24C04以上，那么此处需要连发多个地址
	i2c_SendByte(iic, reg_addr);
	
	//第5步：等待ACK
	if(i2c_WaitAck(iic) != 0)
	{
		//EEPROM器件无应答
		goto cmd_fail;
	}
	
	//第6步：重新启动I2C总线。前面的代码的目的向EEPROM传送地址，下面开始读取数据
	i2c_Start(iic);
	
	//第7步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读
	i2c_SendByte(iic, dev_addr | I2C_RD);
	
	//第8步：发送ACK
	if(i2c_WaitAck(iic) != 0)
	{
		//EEPROM器件无应答
		goto cmd_fail;
	}	
	
	//第9步：循环读取数据
	for(i = 0; i < size; i++)
	{
		//读1个字节
		buf[i] = i2c_ReadByte(iic);
		
		//每读完1个字节后，需要发送Ack， 最后一个字节不需要Ack，发Nack
		if(i != size - 1)
		{
			//中间字节读完后，CPU产生ACK信号(驱动SDA = 0)
			i2c_Ack(iic);
		}
		else
		{
			//最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1)
			i2c_NAck(iic);
		}
	}
	//发送I2C总线停止信号
	i2c_Stop(iic);
	
	
	return 0;	//执行成功
	

//命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备
cmd_fail:
	//发送I2C总线停止信号
	i2c_Stop(iic);
	
	
	return 1;
}


/*
 * @function: i2c_ReadBytes2
 * @details : 从串行EEPROM指定地址处开始读取若干数据
 * @input   : 1.iic：通信接口
              2.dev_addr：设备的I2C总线地址.
              3.reg_addr
              4.buf: 存放读到的数据的缓冲区指针。
              5.size: 数据长度，单位为字节。			  
 * @output  : NULL
 * @return  : 返回值，0：表示成功；1：表示失败。
 */
uint8_t i2c_ReadBytes2(struct tagIICDriver *iic, uint8_t dev_addr, uint16_t reg_addr, uint8_t *buf, uint16_t size)
{
	uint16_t    i = 0;
	
	
	//采用串行EEPROM随即读取指令序列，连续读取若干字节
	
	//第1步：发起I2C总线启动信号
	i2c_Start(iic);
	
	//第2步：发起控制字节，bit7-1是地址；bit0是读写控制位，0表示写，1表示读
	i2c_SendByte(iic, dev_addr | I2C_RD);
	
	//第3步：等待ACK
	if(i2c_WaitAck(iic) != 0)
	{
		//EEPROM器件无应答
		goto cmd_fail;
	}
	
//	//第4步：发送字节地址，24C02只有256字节，因此1个字节就够了，如果是24C04以上，那么此处需要连发多个地址
//	i2c_SendByte(iic, reg_addr);
//	
//	//第5步：等待ACK
//	if(i2c_WaitAck(iic) != 0)
//	{
//		//EEPROM器件无应答
//		goto cmd_fail;
//	}
//	
//	//第6步：重新启动I2C总线。前面的代码的目的向EEPROM传送地址，下面开始读取数据
//	i2c_Start(iic);
//	
//	//第7步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读
//	i2c_SendByte(iic, dev_addr | I2C_RD);
//	
//	//第8步：发送ACK
//	if(i2c_WaitAck(iic) != 0)
//	{
//		//EEPROM器件无应答
//		goto cmd_fail;
//	}	
	
	//第9步：循环读取数据
	for(i = 0; i < size; i++)
	{
		//读1个字节
		buf[i] = i2c_ReadByte(iic);
		
		//每读完1个字节后，需要发送Ack， 最后一个字节不需要Ack，发Nack
		if(i != (size - 1))
		{
			//中间字节读完后，CPU产生ACK信号(驱动SDA = 0)
			i2c_Ack(iic);
		}
		else
		{
			//最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1)
			i2c_NAck(iic);
		}
	}
	//发送I2C总线停止信号
	i2c_Stop(iic);
	
	
	return 0;	//执行成功
	

//命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备
cmd_fail:
	//发送I2C总线停止信号
	i2c_Stop(iic);
	
	
	return 1;
}



/*
 * @function: i2c_WriteBytes
 * @details : 向串行EEPROM指定地址写入若干数据，采用页写操作提高写入效率
 * @input   : 1.iic：通信接口
              2.dev_addr：设备的I2C总线地址.
              3.reg_addr
              4.buf: 存放读到的数据的缓冲区指针。
              5.size: 数据长度，单位为字节。			  
 * @output  : NULL
 * @return  : 返回值，0：表示正常；1：表示失败
 */
uint8_t i2c_WriteBytes(struct tagIICDriver *iic, uint8_t dev_addr, uint16_t reg_addr, uint8_t *buf, uint16_t size)
{
	uint8_t    ret = 0;
	uint8_t    pointer = 0;
	
	
	i2c_Start(iic);
	i2c_SendByte(iic, dev_addr);
	i2c_SendByte(iic, reg_addr);

	for(pointer = 0; pointer < size; pointer++)
	{
		i2c_SendByte(iic, *(buf + pointer));
	}	

	
	//命令执行成功，发送I2C总线停止信号
	i2c_Stop(iic);
	
	
	//等待最后一次EEPROM内部写入完成
//	if(i2c_WaitStandby(dev_addr) == 1) //等于1表示超时
//	{
//		/* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
//		i2c_Stop();
//		ret = 1;
//	}
	
	
	return ret;
}


/*
 * @function: i2c_WriteBytes2
 * @details : 向串行EEPROM指定地址写入若干数据，采用页写操作提高写入效率
 * @input   : 1.iic：通信接口
              2.dev_addr：设备的I2C总线地址.
              3.reg_addr
              4.buf: 存放读到的数据的缓冲区指针。
              5.size: 数据长度，单位为字节。			  
 * @output  : NULL
 * @return  : 返回值，0：表示正常；1：表示失败
 */
uint8_t i2c_WriteBytes2(struct tagIICDriver *iic, uint8_t dev_addr, uint16_t reg_addr, uint8_t *buf, uint16_t size)
{
	uint8_t    ret = 0;
	uint8_t    pointer = 0;
	
	
	i2c_Start(iic);
	i2c_SendByte(iic, dev_addr);
//	i2c_SendByte(iic, reg_addr);

	for(pointer = 0; pointer < size; pointer++)
	{
		i2c_SendByte(iic, *(buf + pointer));
	}	

	
	//命令执行成功，发送I2C总线停止信号
	i2c_Stop(iic);
	
	
	//等待最后一次EEPROM内部写入完成
//	if(i2c_WaitStandby(dev_addr) == 1) //等于1表示超时
//	{
//		/* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
//		i2c_Stop();
//		ret = 0;
//	}
	
	
	return ret;
}




/*
 * @function: ResetIIC
 * @details : 复位IIC.
问题描述：
在程序正常运行的过程中，拔掉设备电源并迅速插回，可能出现主机不能与从机进行连接并且主机持续检测到I2C总线忙，
导致无法读取温度，并且复位也不能解决。使用示波器测量SCL和SDA，发现SDA为低，SCL为高。但当断电时间长一点再次上电，通信正常。

问题原因：
板间存在较大电容，在拔掉电源的后，单片机迅速掉电停止工作，而从机在电容板间电容的的支撑下能够继续工作一段时间，
并且掉电的时间处于主从机通信的过程，从机保持数据线为低状态，并等待主机的时钟信号。但是此时主机复位将不再继续
产生时钟信号，并且上复位后重新初始化硬件I2C模块，硬件I2C检测到SDA为低且不是自身占用的总线，所以将置为BUSY状态，
导致主机不能访问I2C总线。

解决方案：
由于是从机持有数据线并等待时钟信号，所以我们在上电第一次就检测到BUSY状态时，将I2C的IO口设置为普通的GPIO输出口，
模拟100K的时钟频率，并在检测到SDA为高时，发送一个STOP信号，用以完成（中断）从机的上次通信过程，使I2C总线恢复空闲状态，
然后在软复位I2C。（本方法仅适用于单主机模式）

 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
uint8_t ReleaseIIC(I2C_TypeDef* I2Cx, struct tagIICDriver* iic)
{
	uint8_t    index = 0;
	uint8_t    ret = 0;
	
	
	//暂停硬件iic接口工作
	I2C_Cmd(I2Cx, DISABLE);
	I2C_DeInit(I2Cx);
	i2c_Delay();
	
	//配置模拟iic
	iic->GPIO_Cfg();
	i2c_Delay();
	
	
	ret = iic->SDA_Read();
	if(RESET == ret)
	{
		for(index = 0; index < 9; index++)
		{
			iic->SCL_Out(0);
			i2c_Delay();
			
			iic->SCL_Out(1);
			i2c_Delay();
			
			ret = iic->SDA_Read();
			if(RESET != ret)
			{
				break;
			}
		}
	}
	i2c_Stop(iic);

	
	return (0);
}

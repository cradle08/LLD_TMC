/*****************************************************************************
Copyright  : BGI
File name  : bsp_flash.c
Description: 提供flash读写接口
             1.写flash最好四字节对齐，或者写入时就固定使用一种方式，要么是1字节写入要么2字节或者4字节写入。之所以这样是因为避免一个数据放在两个128位上去。
			 2.片内flash擦除及写入的时序由芯片内自动控制，当发出擦除或写入指令时，CPU暂时停止工作，外围设备（串行口、ADC、Timer等仍处于活动状态），
			 外围设备产生的中断此时被挂起，中断在擦除或写入完成后按优先级顺序执行,所以片内Flash的擦除占用了CPU的时间，这段时间还不少呢？
			 16bit编程写是40-70us。
			 擦除一块扇区是20-40ms。
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#include "bsp_flash.h" 
#include "include.h"



//定义变量---------------------------------------------------------------------//

struct tagSTMFlash    STMFlash;





//定义函数---------------------------------------------------------------------//
/*
 * @function: STMFLASH_ReadHalfWord
 * @details : 从指定地址位置读取半字数据
 * @input   : 1.addr: 指定起始地址。
 * @output  : NULL
 * @return  : 返回的数据。
 */
uint16_t STMFLASH_ReadHalfWord(uint32_t addr)
{
	//将addr强行转化为vu16指针，然后去该指针所真相的地址的值，即得到了addr地址的值
	return (*(vu16*)addr);
}

/*
 * @function: STMFLASH_Read
 * @details : 从指定地址开始读出指定长度的数据
 * @input   : 1.addr: 指定起始地址。
 * @output  : NULL
 * @return  : 返回的数据。
 */
uint16_t STMFLASH_Read(uint32_t addr, uint16_t *buff, uint16_t num)   	
{
	uint16_t    i = 0;
	
	
	for(i = 0; i < num; i++)
	{
		buff[i] = STMFLASH_ReadHalfWord(addr);
		addr += 2;
	}
	
	return (0);
}


/*
 * @function: STMFLASH_SeqRead
 * @details : 从指定地址开始读出指定长度的数据
              1.写flash最好四字节对齐。
			  2.以2字节作为一个存储空间单位。
 * @input   : 1.sector: 扇区编号，0-63。
              2.addr: 指定起始地址。
              3.buff: 数据缓存。
			  4.num：读取长度。
 * @output  : NULL
 * @return  : 返回的数据。
 */
uint16_t STMFLASH_SeqRead(uint32_t sector, uint32_t addr, uint16_t *buff, uint16_t num)	
{
	uint16_t   ret = 0;
	uint32_t   sector_start_addr = 0;      //某个扇区起始地址
	uint32_t   sector_r_addr = 0;          //读起始地址(16位字计算)
	uint16_t   i = 0;

	
	
	//超过扇区编号
	if(sector >= STM32_FLASH_SIZE)
	{
		return (1);
	}
	
	//由于每个存储单元占2字节，因此读起始地址需要2字节对齐
	if(0 != (addr % 2))
	{
		return (2);
	}		
	
	//超过扇区存储空间范围（每个存储参数占2个字节，最后一个存储参数不能超过扇区结束地址）
	if((addr + (num * 2)) > STM_SECTOR_SIZE)
	{
		return (3);
	}
	
	
	//扇区起始地址
	sector_start_addr = sector * STM_SECTOR_SIZE + STM32_FLASH_BASE;
	
	//读起始地址
	sector_r_addr = sector_start_addr + addr;
	
	for(i = 0; i < num; i++)
	{
		buff[i] = STMFLASH_ReadHalfWord(sector_r_addr);
		sector_r_addr += 2;
	}	
	
	
	return (ret);
}







/*
 * @function: STMFLASH_Write_NoCheck
 * @details : 写数据，不判断写入结果。
              FLASH Status: The returned value can be: FLASH_BUSY, FLASH_ERROR_PG,
              FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
              写flash最好四字节对齐。
 * @input   : 1.addr: 指定起始地址。
              2.buff: 数据缓存。
			  3.num：写入长度。
 * @output  : NULL
 * @return  : 返回的结果。
 */
uint16_t STMFLASH_Write_NoCheck(uint32_t addr, uint16_t *buff, uint16_t num)   
{ 			 		 
	uint16_t   i = 0;
	uint16_t   ret = 0;
	
	
	//地址增加2(以半字进行操作)写flash最好四字节对齐
	for(i = 0; i < num; i++)
	{
		ret = FLASH_ProgramHalfWord(addr, buff[i]);
	    addr += 2;
	} 
	
	return (ret);
}


/*
 * @function: STMFLASH_Write_Check
 * @details : 写数据，判断写入结果。
              FLASH Status: The returned value can be: FLASH_BUSY, FLASH_ERROR_PG,
              FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
              写flash最好四字节对齐。
 * @input   : 1.addr: 指定起始地址。
              2.buff: 数据缓存。
			  3.num：写入长度。
 * @output  : NULL
 * @return  : 返回的结果。
 */
uint16_t STMFLASH_Write_Check(uint32_t addr, uint16_t *buff, uint16_t num)   
{ 			 		 
	uint16_t   i = 0;
	uint16_t   ret = FLASH_BUSY;
	
	 
	//地址增加2(以半字进行操作)写flash最好四字节对齐
	for(i = 0; i < num; i++)
	{
		ret = FLASH_ProgramHalfWord(addr, buff[i]);
		addr += 2;
		
		if(FLASH_COMPLETE != ret)
		{
			break;
		}
	}
	
	
	return (ret);
}


/*
 * @function: STMFLASH_Write
 * @details : 写数据，过程当中有校验。
              1.写flash最好四字节对齐。
			  2.以2字节作为一个存储空间单位。
 * @input   : 1.addr: 指定起始地址。
              2.buff: 数据缓存。
			  3.num：写入长度。
 * @output  : NULL
 * @return  : 返回的数据。
 */
//uint16_t STMFLASH_Write(uint32_t addr, uint16_t *buff, uint16_t num)	
//{
//	uint32_t   sector_position = 0;        //扇区地址
//	uint16_t   sector_offset = 0;          //扇区内偏移地址(16位字计算)
//	uint16_t   sector_remain = 0;          //扇区内剩余地址(16位字计算)       
//	uint32_t   offset_address = 0;         //去掉0X08000000后的地址(偏移地址)
//	uint16_t   i = 0;
//	uint32_t   flash_addr_end = 0;
//	
//	
//	
//	//写入的地址范围需要是在0x0800 0000-0x0807 FFFF之间
//	flash_addr_end = 1024 * STM32_FLASH_SIZE + STM32_FLASH_BASE;
//	if((addr < STM32_FLASH_BASE) || (addr >= flash_addr_end))	
//	{
//		//非法地址
//		return 1;															
//	}
//	
//	//1.解锁flash
//	FLASH_Unlock ();													
//	
//	
//	//2.计算地址。以2字节作为一个存储空间单位。
//	//实际偏移地址
//	offset_address = addr - STM32_FLASH_BASE;	

//	//扇区地址  0~127 for STM32F103RBT6
//	sector_position = offset_address / STM_SECTOR_SIZE;	
//	
//	//在扇区内的偏移(2个字节为基本单位.)
//	sector_offset = (offset_address % STM_SECTOR_SIZE) / 2;	

//	//扇区剩余空间大小(2个字节为基本单位.)
//	sector_remain = (STM_SECTOR_SIZE / 2) - sector_offset;				 

//	//要写入的数据长度小于扇区剩余的存储单元长度
//	if(num <= sector_remain)
//	{
//		//不大于该扇区范围
//		sector_remain = num;
//	}
//	
//	while(1) 
//	{	
//		//读出整个扇区的内容，因为接下来会擦除扇区
//		STMFLASH_Read(sector_position * STM_SECTOR_SIZE + STM32_FLASH_BASE, STMFlash.SectorBuf, STM_SECTOR_SIZE / 2);
//		
//		//检查校验数据
//		for(i = 0; i < sector_remain; i++)
//		{
//			//写入地址的Flash存储的值是0xFFFF才能重新写入新的数据 
//			if(STMFlash.SectorBuf[sector_offset + i] != 0XFFFF) break;
//		}
//		
//		//需要擦除
//		if(i < sector_remain)
//		{
//			//擦除整个扇区，擦除以页为单位，因为无法对单个存储单元进行擦除
//			FLASH_ErasePage(sector_position * STM_SECTOR_SIZE+STM32_FLASH_BASE);
//			
//			//复制
//			for(i = 0; i < sector_remain; i++)
//			{
//				//将数据装入缓存
//				STMFlash.SectorBuf[i + sector_offset] = buff[i];  
//			}
//			
//			//写入整个扇区
//			STMFLASH_Write_NoCheck(sector_position * STM_SECTOR_SIZE + STM32_FLASH_BASE, STMFlash.SectorBuf, STM_SECTOR_SIZE / 2);  
//		}
//		else 
//		{
//			//写已经擦除了的,直接写入扇区剩余区间.
//			STMFLASH_Write_NoCheck (addr, buff, sector_remain);				 	
//		}	
//		
//		if(num == sector_remain) 
//		{
//			//写入结束了,跳出while()循环
//			break;
//		}
//		else																		
//		{
//			//写入未结束
//			
//			//扇区地址增1
//			sector_position++;
//			
//			//偏移位置归0
//			sector_offset = 0;

//			//指针偏移
//		   	buff += sector_remain;
//			
//			//写地址偏移
//			addr += sector_remain;

//			//字节(16位)数递减
//		   	num -= sector_remain;
//			
//			//如果一个扇区容量还不够，就继续写下一个扇区
//			if(num > (STM_SECTOR_SIZE / 2))									
//			{
//				//下一个扇区还是写不完
//				sector_remain = STM_SECTOR_SIZE / 2;
//			}
//			else
//			{
//				//下一个扇区可以写完了
//				sector_remain = num;											
//			}
//		}	 
//	}
//	
//	//上锁
//	FLASH_Lock();
//	
//	
//	
//	return (0);
//}


/*
 * @function: STMFLASH_SeqWrite
 * @details : 写数据，过程当中有校验。
              1.写flash最好四字节对齐。
			  2.以2字节作为一个存储空间单位。
 * @input   : 1.sector: 扇区编号，0-63。
              2.addr: 指定起始地址。
              3.buff: 数据缓存。
			  4.num：写入长度。
 * @output  : NULL
 * @return  : 返回的数据。
 */
uint16_t STMFLASH_SeqWrite(uint32_t sector, uint32_t addr, uint16_t *buff, uint16_t num)	
{
	uint16_t   ret = 0;
	uint32_t   sector_start_addr = 0;      //某个扇区起始地址
//	uint32_t   sector_w_addr = 0;          //写起始地址(16位字计算)
	uint16_t   sector_cell_offset = 0;     //扇区内的存储单元偏移地址(16位字计算)
//	uint16_t   sector_cell_remain = 0;     //扇区内的存储单元偏移地址(16位字计算)
	uint16_t   i = 0;
	
	
	
	//超过扇区编号
	if(sector >= STM32_FLASH_SIZE)
	{
		return (1);
	}
	
	//由于每个存储单元占2字节，因此写起始地址需要2字节对齐
	if(0 != (addr % 2))
	{
		return (2);
	}		
	
	//超过扇区存储空间范围（每个存储参数占2个字节，最后一个存储参数不能超过扇区结束地址）
	if((addr + (num * 2)) > STM_SECTOR_SIZE)
	{
		return (3);
	}	
	
	
	
	//扇区起始地址
	sector_start_addr = sector * STM_SECTOR_SIZE + STM32_FLASH_BASE;
	
	//写起始地址
//	sector_w_addr = sector_start_addr + addr;
	
	//扇区内的偏移存储单元地址
	sector_cell_offset = addr / 2;
	
	//扇区剩余的存储单元
//	sector_cell_remain = STM_SECTOR_STORE_CELL - sector_cell_offset;
	
	
	//1.读出整个扇区的内容，因为接下来会擦除扇区
	STMFLASH_Read(sector_start_addr, STMFlash.SectorBuf, STM_SECTOR_STORE_CELL);
	
	//2.查看扇区剩余的存储空间是否都为空(可选项)
//	for(i = sector_cell_remain; i < STM_SECTOR_STORE_CELL; i++)
//	{
//		//写入地址的Flash存储的值是0xFFFF，表示为空
//		if(STMFlash.SectorBuf[i] != 0XFFFF)
//		{
//			return (4);
//		}
//	}

	//3.解锁flash
	FLASH_Unlock();	
	
	//4.清除标志位
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR | FLASH_FLAG_OPTERR);

	//5.擦除扇区（擦除以页为单位，因为无法对单个存储单元进行擦除）
	FLASH_ErasePage(sector_start_addr);
	
	//6.检空(可选项)
//	STMFLASH_Read(sector_start_addr, STMFlash.SectorBuf, STM_SECTOR_STORE_CELL);
//	for(i = 0; i < STM_SECTOR_STORE_CELL; i++)
//	{
//		//写入地址的Flash存储的值是0xFFFF才能重新写入新的数据 
//		if(STMFlash.SectorBuf[i] != 0XFFFF)
//		{
//			return (5);
//		}
//	}
	
	//7.写入数据
	for(i = 0; i < num; i++)
	{
		//将数据装入缓存
		STMFlash.SectorBuf[sector_cell_offset + i] = buff[i];  
	}	
	//写入整个扇区
	ret = STMFLASH_Write_NoCheck(sector_start_addr, STMFlash.SectorBuf, STM_SECTOR_STORE_CELL);
//	ret = STMFLASH_Write_Check(sector_start_addr, STMFlash.SectorBuf, STM_SECTOR_STORE_CELL);
	
	//8.上锁
	FLASH_Lock();
	
	
	
	return (ret);
}


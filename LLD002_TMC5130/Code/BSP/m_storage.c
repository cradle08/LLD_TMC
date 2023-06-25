/*****************************************************************************
Copyright  : BGI
File name  : m_storage.c
Description: 存储管理
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#include "m_storage.h"
#include "include.h"




//定义变量---------------------------------------------------------------------//
struct tagStorageMan    StorageMan;



//定义函数---------------------------------------------------------------------//
/*
 * @function: SetMemPageErr
 * @details : 存储页有故障，则记录该页的信息。
 * @input   : 1.man_tab：管理存储表的指针。
              2.page_num：页编号。
 * @output  : NULL
 * @return  : 设置情况，0：成功；1：失败。
 */
uint8_t SetMemPageErr(struct tagStorageMan *man_tab, uint16_t page_num)
{
	if(page_num > (EEPROM_PAGE_MAX_NUM - 1))//0~max可置位，其它是错误的
	{
		return 1;
	}
	man_tab->PageErrStatus[( page_num / 8 )] |= ( uint8_t )( 1 << ( page_num % 8 ) );
	
	return 0;
}

/*
 * @function: ClrMemPageErr
 * @details : 存储页没有故障，则将该页故障信息清除。
 * @input   : 1.man_tab：管理存储表的指针。
              2.page_num：页编号。
 * @output  : NULL
 * @return  : 清除情况，0：成功；1：失败。
 */
uint8_t ClrMemPageErr(struct tagStorageMan *man_tab, uint16_t page_num)
{
	if(page_num > (EEPROM_PAGE_MAX_NUM - 1))//0~max可清除，其它是错误的
	{
		return 1;
	}
	man_tab->PageErrStatus[( page_num / 8 )] &= ( uint8_t )( ~( 1 << ( page_num % 8 ) ) );
	
	return 0;
}

/*
 * @function: ReadMemPage
 * @details : 从存储器逐页读取数据，赋值给变量
 * @input   : 1.man_tab：管理存储表的指针。
 * @output  : NULL
 * @return  : NULL
 */
void ReadMemPage(struct tagStorageMan *man_tab)
{ 
	uint8_t    crc;
	uint16_t   i,j;
	uint16_t   tab_var_index = 0;                          //存储参数管理表成员变量索引
	uint16_t   page_var_offset = 0;                        //存储页的变量偏移
	uint8_t    read_state = 0;                             //读取页数据的状态，0:准备读取或读取中；1：读完该页的数据；2：异常，退出。
	uint8_t    use_default = FALSE;                        //是否使用默认值	
	uint16_t   value_u16 = 0;                              //临时变量.
	
	
	//逐页读取数据。
	for(i = 0; i < man_tab->UsePageNum; i++)
	{
		//从存储器读取一页数据
		man_tab->ReadMem(man_tab->tempdata, (i * EEPROM_PAGE_LENGTH), EEPROM_PAGE_LENGTH);
		crc = man_tab->CalcCRC8(man_tab->tempdata, EEPROM_PAGE_LENGTH - 1);
		
		//检查CRC
		if(crc == man_tab->tempdata[EEPROM_PAGE_LENGTH - 1])
		{
			ClrMemPageErr(man_tab, i);
			use_default = FALSE;
		}
		else
		{
			//连续重读3次
			for(j = 0; j < 3; j++)
			{
				man_tab->ReadMem(man_tab->tempdata, (i * EEPROM_PAGE_LENGTH), EEPROM_PAGE_LENGTH);
				crc = man_tab->CalcCRC8(man_tab->tempdata, EEPROM_PAGE_LENGTH - 1);
				
				if(crc == man_tab->tempdata[EEPROM_PAGE_LENGTH - 1])
				{
					break;
				}
			}
			
			if(j < 3)
			{
				//重读成功
				ClrMemPageErr(man_tab, i);
				use_default = FALSE;
			}
			else
			{
				//重读3次都不成功
				SetMemPageErr(man_tab, i);
				use_default = TRUE;
			} 
		}
		
		
		
		page_var_offset = 0;
		read_state = 0;
		while(read_state == 0)
		{
			if(man_tab->StorageTab[tab_var_index].pVariable != NULL)
			{
				switch(man_tab->StorageTab[tab_var_index].Type)
				{
					case TYPE_INT8:
					{
					}
					break;
					
					case TYPE_UINT8:
					{
					}
					break;
					
					case TYPE_INT16:
					{
					}
					break;
					
					case TYPE_UINT16:
					{
						//检查变量是否超出存储页范围内
						if((page_var_offset + 1) < EEPROM_PAGE_LENGTH)
						{
							if(use_default == TRUE)
							{
								//默认值
								*(uint16_t*)man_tab->StorageTab[tab_var_index].pVariable = (uint16_t)man_tab->StorageTab[page_var_offset].DefaultValue;
							}
							else
							{
								value_u16 = man_tab->tempdata[page_var_offset];
								value_u16 <<= 8;
								value_u16 |= man_tab->tempdata[page_var_offset + 1];
								*(uint16_t*)man_tab->StorageTab[tab_var_index].pVariable = value_u16;
							}
							//每个变量占2字字节
							page_var_offset += 2;
						}
						else
						{
							//存储页数据读取完毕
							read_state = 1;
						}
					}
					break;
					
					default:
					{
						read_state = 2;//异常，退出
					}
					break;
				}
				
				//此次读取正常则++，如果是段满则不查询下一个成员
				if(0 == read_state)
				{
					tab_var_index++;
				}			
			}
			else
			{
				//遍历存储参数表完毕
				read_state = 2;
			}
		}
		
		//异常或遍历完毕，结束
		if(read_state == 2)
		{
			break;
		}
	}
}


/*
 * @function: GetManTabPageData
 * @details : 获取在内存中的参数表的变量
 * @input   : 1.man_tab：管理存储表的指针。
              2.page_num：页编号。
 * @output  : NULL
 * @return  : NULL
 */
uint8_t GetManTabPageData(struct tagStorageMan *man_tab, uint16_t page_num)
{ 
	uint16_t   tab_var_index = 0;                          //存储参数管理表成员变量索引
	uint16_t   page_var_offset = 0;                        //存储页的变量偏移
	uint8_t    read_state = 0;                             //读取页数据的状态，0:准备读取或读取中；1：读完该页的数据；2：异常，退出。
	uint16_t   value_u16 = 0;                              //临时变量.
	uint8_t    ret = 0;                                    //变量缓存 

	
	if(page_num > EEPROM_PAGE_MAX_NUM)
	{
		read_state = 2;
	}
		
	while(read_state == 0)		
	{
		if(NULL == man_tab->StorageTab[tab_var_index].pVariable)
		{
			//校验完所有的数据
			read_state = 2;
		}
		else if(TYPE_UINT16 == man_tab->StorageTab[tab_var_index].Type)
		{
			//检查变量是否超出存储页范围内
			if((page_var_offset + 1) < EEPROM_PAGE_LENGTH)
			{
				value_u16 = *(uint16_t*)man_tab->StorageTab[tab_var_index].pVariable;
				man_tab->WriteData[page_var_offset] = (uint8_t)((value_u16>>8) & 0xFF);
				man_tab->WriteData[page_var_offset + 1] = (uint8_t)((value_u16) & 0xFF);
				page_var_offset += 2;
			}
			else
			{
				//存储页数据读取完毕
				read_state = 1;
			}
		}
		else
		{
			//错误类型
			read_state = 2;
		}
		
		//查询下一个成员
		if(0 == read_state)
		{
			tab_var_index++;
		}
	}	
	
	if(1 == read_state)
	{
		//剩余部分填写0xff
		for(; page_var_offset < (EEPROM_PAGE_LENGTH - 1); page_var_offset++)
		{
			man_tab->WriteData[tab_var_index] = 0xff;
		}
	}
	man_tab->WriteData[EEPROM_PAGE_LENGTH - 1] = man_tab->CalcCRC8(man_tab->WriteData, (EEPROM_PAGE_LENGTH - 1));
	
	
	return (ret);
}


/*
 * @function: VerifyMemPage
 * @details : 校验存储页的参数
 * @input   : 1.man_tab：管理存储表的指针。
              2.app_buff：app变量。
 * @output  : NULL
 * @return  : 校验内存页参数，0：通过；1：不通过。
 */
uint8_t VerifyMemPage(struct tagStorageMan *man_tab, uint8_t *app_buff)
{
	uint8_t   i = 0;
	uint8_t   ret = 0;
	

	//从存储器读取一页数据
	man_tab->ReadMem(man_tab->tempdata, (man_tab->PagePtr * EEPROM_PAGE_LENGTH), EEPROM_PAGE_LENGTH);
	if(man_tab->tempdata[EEPROM_PAGE_LENGTH - 1] == man_tab->CalcCRC8(man_tab->tempdata, EEPROM_PAGE_LENGTH - 1))
	{
		for(i = 0; i < (EEPROM_PAGE_LENGTH - 1); i++)
		{
			if(man_tab->tempdata[i] != app_buff[i])
			{
				ret = 1;
				break;
			}
		}
	}
	else
	{
		ret = 1;
	}

	return ret;
}


/*
 * @function: GetMemPageIsErr
 * @details : 检测存储页是否有软硬件故障
 * @input   : 1.man_tab：管理存储表的指针。
              2.page_num：页编号。
 * @output  : NULL
 * @return  : 0:正常；1：异常；2：我们的硬件没做那么大；3：我们的软件没用那么大。
 */
uint8_t GetMemPageIsErr(struct tagStorageMan *man_tab, uint8_t page_num)
{
	uint8_t byte_num = page_num / 8;
	uint8_t bit_num = page_num % 8;
	uint8_t flag = 1;
	
	
	if(page_num > man_tab->UsePageNum)
	{
		return 3;//我们的软件没用那么大
	}
	else if(byte_num >= PAGE_ERR_MAX)
	{
		return 2;//我们的硬件没做那么大
	}
	
	flag <<= bit_num;
	if((man_tab->PageErrStatus[byte_num] & flag) == 0 )
	{
		return 0;//正常
	}
	return 1;//异常
}


/*
 * @function: GetMemPageIsProtected
 * @details : 检测存储页是否有写保护
 * @input   : 1.man_tab：管理存储表的指针。
              2.page_num：页编号。
 * @output  : NULL
 * @return  : 校验内存页参数，0：没有；1：有。
 */
uint8_t GetMemPageIsProtected(struct tagStorageMan *man_tab)
{
	if((man_tab->PagePtr / 8) < PAGE_ERR_MAX)
	{
		if((man_tab->WriteProtect[(man_tab->PagePtr / 8)] & (0x01<<(man_tab->PagePtr % 8 ))) != 0)
		{
			return 1;
		}
	}
	
	return 0;
}


/*
 * @function: SetMemPageProtect
 * @details : 设置存储页的页保护
 * @input   : 1.man_tab：管理存储表的指针。
              2.page_num：页编号。
 * @output  : NULL
 * @return  : 0:正常；1：设置失败。
 */
uint8_t SetMemPageProtect(struct tagStorageMan *man_tab, uint8_t page_num)
{
	uint8_t   ret = 0;
	
	
	if((page_num / 8) < PAGE_ERR_MAX)
	{
		man_tab->WriteProtect[(page_num / 8)] |= (0x01<<(page_num % 8 )); 
	}
	else
	{
		ret = 1;
	}
	
	return (ret);
}


/*
 * @function: ClrMemPageProtect
 * @details : 清除存储页的页保护
 * @input   : 1.man_tab：管理存储表的指针。
              2.page_num：页编号。
 * @output  : NULL
 * @return  : 0:正常；1：清除失败。
 */
uint8_t ClrMemPageProtect(struct tagStorageMan *man_tab, uint8_t page_num)
{
	uint8_t   ret = 0;
	
	
	if((page_num / 8)< PAGE_ERR_MAX)
	{
		man_tab->WriteProtect[(page_num / 8)] &= ~(0x01<<(page_num % 8 )); 
	}
	else
	{
		ret = 1;
	}
	
	return (ret);	
}


/*
 * @function: MemManStage
 * @details : 存储管理状态机
 * @input   : 1.man_tab：管理存储表的指针。
              2.page_num：页编号。
 * @output  : NULL
 * @return  : 0:正常；1：参数管理表格为空。
 */
#define  EEPROM_ERASE                  FALSE
#define  ERASE_PAGE_HEAD               0
#define  ERASE_PAGE_TAIL               10
uint8_t MemManStage(struct tagStorageMan *man_tab)
{
	uint8_t    i = 0;
	uint8_t    temp = 0;
	uint8_t    jump_stage = FALSE;
	uint8_t    need_write = FALSE;
	
	
	if(man_tab->StorageTab == NULL)
	{
		return 1;
	}
	
	if(man_tab->CallCnt < man_tab->CallTime)
	{
		man_tab->CallCnt += man_tab->CallInc;
	}
	
	switch(man_tab->Stage)
	{
		case EEPROM_NULL:    //空闲阶段
		{
			if(man_tab->CallCnt >= man_tab->CallTime )
			{
				man_tab->CallCnt = 0;

				for(i = 0; i < 8; i++)
				{
					man_tab->WriteData[i] = 0;
				}

				man_tab->Stage = EEPROM_VERIFY;
			}
		}
		break;

		case EEPROM_VERIFY:    //验证阶段
		{
			for(i = 0; i < EEPROM_PAGE_LENGTH; i++)
			{
				man_tab->WriteData[i] = 0;
			}
			//获取应用层参数
			GetManTabPageData(man_tab, man_tab->PagePtr);
			
	 
#if(EEPROM_ERASE)
			//强制EEPROM进入写阶段.
			man_tab->Stage = EEPROM_WRITE;
#else
			//读取eep数据进行比较 
			if(0 == VerifyMemPage(man_tab, man_tab->WriteData))
			{
				//清除eep故障错误
				ClrMemPageErr(man_tab, man_tab->PagePtr);

				if((man_tab->PagePtr + 1) < man_tab->UsePageNum)
				{
					//校验下一页
					man_tab->PagePtr++;
				}
				else
				{
					//存储参数管理表格校验完毕。
					man_tab->PagePtr = 0;
					man_tab->Stage = EEPROM_NULL;
					//校验结束
					man_tab->IsCompareOver = TRUE;					
				}
			}
			else
			{
				need_write = TRUE;
			}
			
			//有写存储需求&&此页没有页保护
			if((TRUE == need_write) && (FALSE == GetMemPageIsProtected(man_tab)))
			{
				man_tab->Stage = EEPROM_WRITE;
			}
#endif			
		}
		break;
		
		case EEPROM_WRITE:    //EEP写阶段
		{
			//获取应用层参数
			GetManTabPageData(man_tab, man_tab->PagePtr);

#if(EEPROM_ERASE)
			//强制向EEPROM页写入0xFF
			if((man_tab->PagePtr >= ERASE_PAGE_HEAD)
				&& (man_tab->PagePtr < ERASE_PAGE_TAIL))
			{
				for(i = 0; i < 8; i++)
				{
					man_tab->WriteData[i] = 0xFF;
				}
			}
#endif
			man_tab->WriteMem(man_tab->WriteData, (man_tab->PagePtr * EEPROM_PAGE_LENGTH), EEPROM_PAGE_LENGTH);
			man_tab->Stage = EEPROM_WRVERIFY;			
		}
		break;

		case EEPROM_WRVERIFY:    //写后验证阶段
		{
			temp = VerifyMemPage(man_tab, man_tab->WriteData);
			if(0 == temp)
			{
				man_tab->PageWriteCnt = 0;
				ClrMemPageErr(man_tab, man_tab->PagePtr);
				
				jump_stage = TRUE;
			}
			else
			{
				man_tab->PageWriteCnt++;
			}
			
			//重写超过次数后判定为故障
			if(man_tab->PageWriteCnt > man_tab->PageErrNum)
			{
				man_tab->PageWriteCnt = 0;
				SetMemPageErr(man_tab, man_tab->PagePtr);
				
				jump_stage = TRUE;
			}
			
			//是否结束该次页操作
			if(TRUE == jump_stage)
			{
				//当前页地址与用户设置最大页数
				if(man_tab->PagePtr < ((man_tab->UseBytes / 8) - 1))
				{
					man_tab->PagePtr++;
					man_tab->Stage = EEPROM_VERIFY;
				}
				else
				{
					man_tab->PagePtr = 0;
					man_tab->Stage = EEPROM_NULL;
					
					man_tab->IsCompareOver = TRUE;
				}
			}
			else
			{
				man_tab->Stage = EEPROM_WRITE;
			}
		}
		break;
		
		default:
		{
			man_tab->Stage = EEPROM_NULL;
		}
		break;
	}
	
	return 0;
}

/*
 * @function: MemManInit
 * @details : 存储管理表初始化
 * @input   : 1.man_tab：管理存储表的指针。
              2.CallBackFun：回调函数。
			  3.ReadMem：读存储函数指针。
			  4.WriteMem：写存储函数指针。
			  5.CalcCRC8：CRC8校验函数指针。
			  6.call_inc：自增周期。
			  7.call_time：检验参数周期。			  
 * @output  : NULL
 * @return  : 0:正常；1：故障。
 */
uint8_t MemManInit(struct tagStorageMan *man_tab,
					const struct tagStorageTab *tab,
					uint8_t call_inc,
					uint8_t call_time,
					void (*CallBackFun)(void),
					uint8_t (*ReadMem)(uint8_t *buf, uint16_t addr, uint16_t num),
					uint8_t (*WriteMem)(uint8_t *buf, uint16_t addr, uint16_t num),
					uint8_t (*CalcCRC8)(uint8_t *buf, uint8_t num))
{
	uint16_t   i = 0;
	uint16_t   page_cnt = 0;     //统计存储页数量
	uint8_t    char_length = 0;  //拼的数据的字节长度
	
	
	//空指针
	if(NULL == tab)
	{
		return 3;
	}
	
	//统计实际使用的存储页数
	for(i = 0; i < EEPROM_PAGE_MAX_NUM; i++)
	{
		//遍历完所有的变量，退出。
		if(NULL == tab[i].pVariable)
		{
			if(char_length != 0)
			{
				//末尾的变量个数可能不够4个，但也需要分配存储页。
				page_cnt++;
			}
			break;
		}
		
		//目前只支持uint16_t类型的变量，其他变量不支持。
		if(tab[i].Type == TYPE_UINT16)
		{
			char_length += 2;
		}
		else
		{
			return 1;
		}
		
		if(char_length == EEPROM_PAGE_LENGTH)
		{
			//统计完一页
			page_cnt++;
			char_length = 0;
		}
		else if(char_length > EEPROM_PAGE_LENGTH)
		{
			page_cnt++;
			char_length = 0;
			//i不增加，这次超了范围。
			i--;
		}
	}
	
	//统计得到的页数超过存储芯片实际页数
	if((page_cnt + 1) > EEPROM_PAGE_LENGTH)
	{
		return 2;
	}
	
	
	if(NULL == man_tab->StorageTab)
	{
		man_tab->SizePageNum  = EEPROM_PAGE_MAX_NUM;	
		man_tab->SizeBytes    = EEPROM_PAGE_MAX_NUM * EEPROM_PAGE_LENGTH;	
		
		man_tab->UsePageNum   = page_cnt;
		man_tab->UseBytes     = man_tab->UsePageNum * EEPROM_PAGE_LENGTH;		
		
		
		//使用页数不能超过存储器实际页数
		if(man_tab->UsePageNum > man_tab->SizePageNum)
		{
			man_tab->UsePageNum = man_tab->SizePageNum;
		}		
		
		//使用字节数不能超过存储器实际字节数
		if(man_tab->UseBytes > man_tab->SizeBytes)
		{
			man_tab->UseBytes = man_tab->SizeBytes;
		}
		
		man_tab->CallInc      = call_inc;                  //计时增量单位
		man_tab->CallTime     = call_time * 1000;	       //存储频率
		
		man_tab->CallCnt      = 5 * 1000;                  //计时变量  第一次只要5s后就检测一遍
		man_tab->StorageTab   = tab;
		
		man_tab->Stage        = EEPROM_NULL;
		man_tab->PagePtr      = 0;
		
		man_tab->PageWriteCnt = 0;                         //重写计数
		man_tab->PageErrNum   = 3;                         //重写允许次数
		man_tab->IsErr        = FALSE;
		
		man_tab->ReadMem      = ReadMem;
		man_tab->WriteMem     = WriteMem;
		man_tab->CalcCRC8     = CalcCRC8;
		
		
		//初始化参数
		ReadMemPage(man_tab);
		
		if(CallBackFun != NULL)//回调函数编写出
		{
			CallBackFun();
		}
	}
	else
	{
		return 4;//当前成员表正在被使用，无法替换。
	}
	
	return 0;
}

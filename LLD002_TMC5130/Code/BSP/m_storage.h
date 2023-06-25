/*****************************************************************************
Copyright  : BGI
File name  : m_storage.h
Description: 存储管理
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#ifndef __M_STORAGE_H
#define __M_STORAGE_H

#include "stm32f10x.h"




//宏定义----------------------------------------------------------------------//
//EEPROM状态机
#define  EEPROM_NULL                   0    //空等待阶段
#define  EEPROM_VERIFY                 1    //验证阶段
#define  EEPROM_WRITE                  2    //EEP写阶段
#define  EEPROM_WRVERIFY               3    //写后验证阶段


//ee存储表相关参数配置
#define  EEPROM_PAGE_LENGTH            8    //存储页字节数 
#define  EEPROM_PAGE_MAX_NUM           64   //存储页数数量，根据实际情况修改
#define  EEPROM_VAR_MAX_NUM            256  //存储变量数量										  

#define  PAGE_ERR_MAX                  32	//记录每一页是否有CRC，取值为PAGE_MAX_NUM / 8




//定义结构体--------------------------------------------------------------------//
//存储参数变量类型
enum tagEEParaTabVarType
{
	TYPE_INT8 = 0,
	TYPE_UINT8 = 1,
	TYPE_INT16 = 2,
	TYPE_UINT16 = 3,
};

//存储参数表格
struct tagStorageTab
{
	uint16_t   DefaultValue;                                                   //默认值
	void *pVariable;                                                           //变量指针
	enum tagEEParaTabVarType    Type;                                          //变量类型  
};

//存储参数管理
struct tagStorageMan
{	
	uint16_t   SizePageNum;                                                    //存储页数
	uint16_t   SizeBytes;                                                      //存储设备字节数
	uint16_t   UsePageNum;                                                     //使用的页数
	uint16_t   UseBytes;                                                       //用户实际使用字节数	
	uint16_t   CallInc;                                                        //计时增量单位
	uint16_t   CallTime;                                                       //设定周期
	uint16_t   CallCnt;                                                        //计时
	const struct tagStorageTab *StorageTab;                                    //参数成员
	
	uint8_t    Stage;                                                          //EEP运行状态机
	uint8_t    PagePtr;                                                        //当前页编号
	uint8_t    PageErrStatus[PAGE_ERR_MAX];                                    //页状态，0表示正常  1表示故障   从[0]bit0 代表第0页
	uint8_t    WriteProtect[PAGE_ERR_MAX];                                     //写保护标志 0表示不保护  1表示保护（免检）  从[0]bit0 代表第0页
	uint8_t    WriteData[EEPROM_PAGE_LENGTH];                                  //写数据缓存
	uint8_t    tempdata[EEPROM_PAGE_LENGTH];                                   //数据缓存
	
	uint8_t    PageWriteCnt;                                                   //重写当前页的次数
	uint8_t    PageErrNum;                                                     //重写判EEP故障次数
	uint8_t    IsErr;                                                          //故障，0：无故障；1：有故障。
	
	uint8_t    IsCompareOver;                                                  //是否比较完成  / 0没   1完成	
		
	uint8_t    (*ReadMem)(uint8_t *buf, uint16_t addr, uint16_t num);  
	uint8_t    (*WriteMem)(uint8_t *buf, uint16_t addr, uint16_t num);  
	uint8_t    (*CalcCRC8)(uint8_t *buf, uint8_t num);
};



//声明变量----------------------------------------------------------------------//
extern struct tagStorageMan    StorageMan;



//声明函数----------------------------------------------------------------------//
uint8_t GetMemPageIsErr(struct tagStorageMan *man_tab, uint8_t page_num);
uint8_t MemManStage(struct tagStorageMan *man_tab);
uint8_t MemManInit(struct tagStorageMan *man_tab,
					const struct tagStorageTab *tab,
					uint8_t call_inc,
					uint8_t call_time,
					void (*CallBackFun)(void),
					uint8_t (*ReadMem)(uint8_t *buf, uint16_t addr, uint16_t num),
					uint8_t (*WriteMem)(uint8_t *buf, uint16_t addr, uint16_t num),
					uint8_t (*CalcCRC8)(uint8_t *buf, uint8_t num));


#endif


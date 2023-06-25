/*****************************************************************************
Copyright  : BGI
File name  : bsp_misc.c
Description: 乱七八糟的东西
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#include "bsp_misc.h" 
#include "include.h"



//定义变量---------------------------------------------------------------------//

//程序编译时间
//程序名称和版本号
#if defined DEBUG
const char SoftWareName[SOFTWARE_NAME_LEN] = "LLD002";
const uint16_t SoftWareVer[SOFTWARE_VER_LEN] = {1, 1, 0};
#else    //被占用很大的RO data空间，等待优化
const char CompileInfo[3][17] __attribute__((at(0x8004000))) = { "CompileTime&Date:", __DATE__, __TIME__ };
const char SoftWareChar[SOFTWARE_NAME_LEN] __attribute__((at(0x8004040))) = "LLD002";
const uint16_t SoftWareVer[SOFTWARE_VER_LEN] __attribute__((at(0x8004058))) = {1, 1, 0};
#endif



//定义函数---------------------------------------------------------------------//

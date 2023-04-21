/*****************************************************************************
Copyright  : BGI
File name  : liquid_level.h
Description: 检测液面
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#ifndef __LIQUID_LEVEL_H
#define __LIQUID_LEVEL_H


#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_misc.h" 




//宏定义----------------------------------------------------------------------//
//电容通信地址
#define  CAP_DEV_ADDR                  0x54

//FDC2212寄存器地址（所有寄存器都是2字节）
#define  CAP_DATA_CH0_REG              0x00      //信道0 MSB转换结果和状态
#define  CAP_DATA_LSB_CH0_REG          0x01      //信道0 LSB转换结果，必须在寄存器地址0x00之后读取
#define  CAP_DATA_CH1_REG              0x02      //信道1 MSB转换结果和状态
#define  CAP_DATA_LSB_CH1_REG          0x03      //信道1 LSB转换结果，必须在寄存器地址0x00之后读取
#define  CAP_RCOUNT_CH0_REG            0x08      //通道0 的参考计数设置
#define  CAP_OFFSET_CH0_REG            0x0C      //通道0 的偏移值
#define  CAP_SETTLECOUNT_CH0_REG       0x10      //通道0 沉降参考计数
#define  CAP_CLOCK_DIVIDERS_CH0_REG    0x14      //通道0 的基准除法器设置

#define  CAP_STATE_REG                 0x18      //状态寄存器地址
#define  CAP_STATE_CFG_REG             0x19      //状态配置寄存器地址
#define  CAP_CONFIG_REG                0x1A      //转换配置
#define  CAP_MUX_CONFIG_REG            0x1B      //信道复用配置
#define  CAP_RESET_CMD_REG             0x1C      //复位寄存器
#define  CAP_DRIVE_CURRENT_CH0_REG     0x1E      //通道0 传感器电流驱动配置

#define  CAP_MANUFACTURER_ID           (0x7E)    //供应商身份
#define  CAP_DEVICE_ID                 (0x7F)    //设备ID



//气压式传感器通信地址
#define  AIR_SEN_DEV_ADDR              0x50

//气压式传感器寄存器地址
#define  AIR_SEN_REG_INVALID_ADDR      0u        //无效寄存器地址

//气压式传感器压力平衡点
#define  AIR_SEN_ZERO_PRESS            8192u

//气压式传感器压力报警点
#define  AIR_SEN_ERR_PRESS             16000u

//气压式传感器正压、负压最大值
#define  AIR_SEN_PRESS_MAX             16383u
#define  AIR_SEN_PRESS_MIN             0u



//电容气压传感器状态机
#define  CP_COMM_RESET_DEV             0u        //复位设备
#define  CP_COMM_INIT                  1u        //初始化
#define  CP_COMM_NORMAL                2u        //通信正常
#define  CP_COMM_RELEASE_IIC           3u        //释放IIC
#define  CP_COMM_RESET_IIC             4u        //复位IIC



//检测液面状态机
#define  LLD_STAGE_NULL                0u        //空闲
#define  LLD_STAGE_START               1u        //启动
#define  LLD_STAGE_CONTACT             2u        //探针接触到液面，确认中
#define  LLD_STAGE_LIQ                 3u        //探测到液面
#define  LLD_STAGE_FAIL                4u        //探测故障
#define  LLD_STAGE_BUBBLE              5u        //探测到气泡
#define  LLD_STAGE_ABS_BLOCK           6u        //检测到吸液堵塞
#define  LLD_STAGE_ABS_AIR             7u        //检测到全程吸空/部分行程吸空
#define  LLD_STAGE_ABS_AIR_MOMENT      8u        //检测到间断吸空



//气压探测阶段
#define  PLLD_STAGE_NULL               0u        //空闲
#define  PLLD_STAGE_START              1u        //启动信号
#define  PLLD_STAGE_TOUCH              2u        //接触到物体
#define  PLLD_STAGE_BUBBLE             3u        //吸到气泡
#define  PLLD_STAGE_ABS_BLOCK          4u        //吸液被堵
#define  PLLD_STAGE_AIR_MOMENT         5u        //间断空吸
#define  PLLD_STAGE_AIR_ALWAYS         6u        //始终吸空/部分行程空吸
#define  PLLD_STAGE_ABS_BLOCK_AIR      7u        //检测凝块和吸空
#define  PLLD_STAGE_LIQ                8u        //识别出液面
#define  PLLD_STAGE_DIS_BLOCK          9u        //排液被堵
#define  PLLD_STAGE_DIS_AIR            10u       //排液排空



//检测结果
#define  LLD_IDLE                      0u        //探测空闲/正常
#define  LLD_LIQUL                     1u        //探测液面
#define  LLD_BUBBLE                    2u        //探测气泡
#define  LLD_ABS_BLOCK                 3u        //检测吸液被堵（检测凝块）
#define  LLD_ABS_AIR                   4u        //检测全程吸空
#define  LLD_ABS_BLOCK_AIR             5u        //检测吸液被堵和检测全程吸空
#define  LLD_DIS_BLOCK                 6u        //检测排液被堵
#define  LLD_DIS_AIR                   7u        //检测排液排空
#define  LLD_ABS_MOM_AIR               8u        //检测吸液吸空



//传感器功能
#define  LLD_CLOSE                     0u        //关闭探测（输出）
#define  CLLD_LIQUL                    1u        //开启电容探测液面
#define  PLLD_LIQUL                    2u        //开启气压探测液面
#define  LLD_CAP_AIR_LIQUL             3u        //开启电容气压探测液面

#define  CLLD_BUBBLE                   0x10      //探测气泡
#define  CLLD_ABS_BLOCK                0x11      //检测吸液被堵（检测凝块）
#define  CLLD_ABS_AIR                  0x12      //检测吸空
#define  CLLD_ABS_BLOCK_FIXED_AIR      0x13      //检测吸液被堵和检测吸空
#define  CLLD_DIS_BLOCK                0x14      //检测排液被堵
#define  CLLD_ABS_FOLLOW               0x15      //探测追随（预留）
#define  CLLD_TWO_DETE                 0x16      //探测两次（预留）

#define  PLLD_BUBBLE                   0x20      //探测气泡
#define  PLLD_ABS_BLOCK                0x21      //检测吸液被堵（检测凝块）
#define  PLLD_ABS_FIXED_AIR            0x22      //检测固定点吸空
#define  PLLD_ABS_BLOCK_FIXED_AIR      0x23      //检测吸液被堵和检测固定点吸空
#define  PLLD_DIS_BLOCK                0x24      //检测排液被堵
#define  PLLD_ABS_FOLLOW               0x25      //探测追随（预留）
#define  PLLD_TWO_DETE                 0x26      //探测两次（预留）
#define  PLLD_ABS_FOLLOW_AIR           0x27      //检测追随吸空
#define  PLLD_ABS_BLOCK_FOLLOW_AIR     0x28      //检测吸液被堵和检测追随吸空
#define  PLLD_ABS_BLOCK_FIXED_FOLLOW   0x29      //检测吸液被堵、固定点吸空、追随吸空




//电容通道数据
#define  CLLD_CH0_DATA                 0u
#define  CLLD_CH1_DATA                 1u
#define  CLLD_CH_NUM                   2u


//电容转换时间
#define  CLLD_TRANSFER_TIME0           1u
#define  CLLD_TRANSFER_TIME1           11u
#define  CLLD_TRANSFER_TIME2           21u
#define  CLLD_TRANSFER_TIME3           31u
#define  CLLD_TRANSFER_TIME4           41u
#define  CLLD_TRANSFER_TIME5           51u
#define  CLLD_TRANSFER_TIME6           61u
#define  CLLD_TRANSFER_TIME7           71u
#define  CLLD_TRANSFER_TIME8           81u
#define  CLLD_TRANSFER_TIME9           91u
#define  CLLD_TRANSFER_TIME10          101u


//电容开窗大小
#define  CLLD_SLIDEWIND_NUM1           16u
#define  CLLD_SLIDEWIND_NUM2           16u
#define  CLLD_SLIDEWIND_NUM3           13u
#define  CLLD_SLIDEWIND_NUM4           12u
#define  CLLD_SLIDEWIND_NUM5           9u
#define  CLLD_SLIDEWIND_NUM6           8u
#define  CLLD_SLIDEWIND_NUM7           7u
#define  CLLD_SLIDEWIND_NUM8           6u
#define  CLLD_SLIDEWIND_NUM9           5u
#define  CLLD_SLIDEWIND_NUM10          4u


//电容开始转换，丢弃掉波动干扰数据数量
#define  CLLD_RES_WAIT_TIME1           14u
#define  CLLD_RES_WAIT_TIME2           6u
#define  CLLD_RES_WAIT_TIME3           6u
#define  CLLD_RES_WAIT_TIME4           6u
#define  CLLD_RES_WAIT_TIME5           6u
#define  CLLD_RES_WAIT_TIME6           6u
#define  CLLD_RES_WAIT_TIME7           5u
#define  CLLD_RES_WAIT_TIME8           5u
#define  CLLD_RES_WAIT_TIME9           4u
#define  CLLD_RES_WAIT_TIME10          4u



//采集数据索引
#define  PLLD_CH0_DATA                 0u
//#define  PLLD_CH1_DATA                 1u
#define  PLLD_CH_NUM                   1u


//电容、气压数据个数
#define  CLLD_NUM                      16u
#define  PLLD_NUM                      32u


//吸液状态
#define  PLLD_ABS_IDLE                 0u
#define  PLLD_ABS_END                  1u
#define  PLLD_ABS_START                2u






//定义结构体--------------------------------------------------------------------//

//选择通道
typedef enum
{
    FDC2214_Channel_0 = 0x00, 
    FDC2214_Channel_1 = 0x01, 
    FDC2214_Channel_2 = 0x02, 
    FDC2214_Channel_3 = 0x03  
}FDC2214_channel_t;


//通道切换顺序
typedef enum
{
    FDC2214_Channel_Sequence_0_1      = 0x00,
    FDC2214_Channel_Sequence_0_1_2    = 0x01,
    FDC2214_Channel_Sequence_0_1_2_3  = 0x02, 
}FDC2214_channel_sequence_t;

//通道传感器带宽
typedef enum
{
    FDC2214_Bandwidth_1M   = 0x01, //1MHz
    FDC2214_Bandwidth_3_3M = 0x04, //3.3MHz
    FDC2214_Bandwidth_10M  = 0x05, //10MHz
    FDC2214_Bandwidth_33M  = 0x07  //33MHz
}FDC2214_filter_bandwidth_t;

//通道传感器电流
typedef enum
{
	FDC2214_Drive_Current_0_016 = 0x00, //0.016mA
	FDC2214_Drive_Current_0_018 = 0x01, //0.018mA
	FDC2214_Drive_Current_0_021 = 0x02, //0.021mA
	FDC2214_Drive_Current_0_025 = 0x03, //0.025mA
	FDC2214_Drive_Current_0_028 = 0x04, //0.028mA
	FDC2214_Drive_Current_0_033 = 0x05, //0.033mA
	FDC2214_Drive_Current_0_038 = 0x06, //0.038mA
	FDC2214_Drive_Current_0_044 = 0x07, //0.044mA
	FDC2214_Drive_Current_0_052 = 0x08, //0.052mA
	FDC2214_Drive_Current_0_060 = 0x09, //0.060mA
	FDC2214_Drive_Current_0_069 = 0x0A, //0.069mA
	FDC2214_Drive_Current_0_081 = 0x0B, //0.081mA
	FDC2214_Drive_Current_0_093 = 0x0C, //0.093mA
	FDC2214_Drive_Current_0_108 = 0x0D, //0.108mA
	FDC2214_Drive_Current_0_126 = 0x0E, //0.126mA
	FDC2214_Drive_Current_0_146 = 0x0F, //0.146mA
	FDC2214_Drive_Current_0_169 = 0x10, //0.169mA
	FDC2214_Drive_Current_0_196 = 0x11, //0.196mA
	FDC2214_Drive_Current_0_228 = 0x12, //0.228mA
	FDC2214_Drive_Current_0_264 = 0x13, //0.264mA
	FDC2214_Drive_Current_0_307 = 0x14, //0.307mA
	FDC2214_Drive_Current_0_356 = 0x15, //0.356mA
	FDC2214_Drive_Current_0_413 = 0x16, //0.413mA
	FDC2214_Drive_Current_0_479 = 0x17, //0.479mA
	FDC2214_Drive_Current_0_555 = 0x18, //0.555mA
	FDC2214_Drive_Current_0_644 = 0x19, //0.644mA
	FDC2214_Drive_Current_0_747 = 0x1A, //0.747mA
	FDC2214_Drive_Current_0_867 = 0x1B, //0.867mA
	FDC2214_Drive_Current_1_006 = 0x1C, //1.006mA
	FDC2214_Drive_Current_1_167 = 0x1D, //1.167mA
	FDC2214_Drive_Current_1_354 = 0x1E, //1.354mA
	FDC2214_Drive_Current_1_571 = 0x1F  //1.571mA
}FDC2214_drive_current_t;


//通道切换顺序
enum tagAirPressState
{
	AIRPRESS_SUP_SHORT_PASS_NUM   = 2,
	AIRPRESS_SHORTET_PASS_NUM     = 3,
	AIRPRESS_SHORT_PASS_NUM       = 6,
	AIRPRESS_MID_PASS_NUM         = 13,
	AIRPRESS_LONG_PASS_NUM        = 20,
	AIRPRESS_SUP_LONG_PASS_NUM    = 26
};




//存放数据
struct tagLLDData
{
	uint32_t   *OrigData;                        //原始AD值
	uint32_t   *FilterData;                      //经过滤波后的值
	uint32_t   OrigValue;	                     //原始AD值
	uint32_t   FilterValue;                      //经过滤波后的值
	
	int32_t    Value;                            //经过各种转换后的值
	uint8_t    ErrTime;                          //传感器异常计时
	uint8_t    IsErr;                            //是否故障  0:无故障   1:故障	
};


//电容传感器数据
struct tagCapSenPara
{
	uint8_t    ComStage;                         //通信阶段	
	uint8_t    State;                            //状态
	uint8_t    Buff[30];                         //数据缓存
	
	uint8_t    Rdy;                              //数据准备完毕
	uint8_t    RdyTime;                          //数据准备持续时间
	uint8_t    Ch0UnRead;                        //通道0数据未读走
	uint8_t    Ch1UnRead;                        //通道1数据未读走
	uint8_t    UpdateFlag;                       //数据更新
	uint16_t   UpdateCnt;                        //数据更新统计
	uint16_t   UpdateNoCnt;                      //数据未更新统计
	uint32_t   ReadCnt;                          //数据读取次数
	
	
	
	struct tagLLDData ChColl[CLLD_CH_NUM];
	uint32_t   AppOrigBuff[CLLD_NUM];            //应用数据缓存
	uint32_t   AppFilterBuff[CLLD_NUM];          //应用数据滤波后缓存
	
	uint8_t    SlideWindOpen;                    //窗口开启
	uint8_t    SlideWindNum;                     //窗口数量
	uint64_t   SlideWindSum;                     //窗口数据和
	uint32_t   AveVal;	                         //平均值
	uint32_t   ChlPreData;                       //ch1上一次数据
	uint16_t   SlideCnt;                         //滑动计数
	uint16_t   SlidePassSingle;                  //滑动通过设置
	uint16_t   SlidePassAll;                     //滑动通过设置
	
	uint16_t   RCOUNT_CH0;                       //通道0 的参考计数设置
	uint16_t   OFFSET_CH0;                       //通道0 的偏移值
	uint16_t   SETTLECOUNT_CH0;                  //通道0 沉降参考计数
	uint16_t   CLOCK_DIVIDERS_CH0;               //通道0 的基准除法器设置
	uint16_t   STATUS_CONFIG;                    //状态配置
	uint16_t   CONFIG;                           //转换配置
	uint16_t   MUX_CONFIG;                       //信道复用配置
	uint16_t   DRIVE_CURRENT_CH0;                //通道0 传感器电流驱动配置
	
	
	uint32_t   ContactThreshold;                 //接触液面阈值
	uint32_t   LeaveThreshold;                   //离开液面阈值
	
	uint8_t    ConTime;                          //转换时间
	
	uint8_t    ResStatu;                         //复位设备阶段/状态
	uint8_t    ResWaitTime;                      //复位设备等待时间
	uint8_t    SleepOpen;                        //睡眠开启
	uint8_t    SleepManualCtl;                   //睡眠手动控制
	uint8_t    SleepStatu;                       //睡眠状态
	
	uint8_t    ChlNmu;                           //使用通道编号
	uint8_t    ChlNeedSwitch;                    //通道需要切换
	
	uint8_t    LLDStage;                         //检测液面阶段	
	int16_t    CapVal;                           //ch1电容值
	uint16_t   LLDResult;                        //液面探测结果	
};









//测量气压曲线
struct tagAirPress
{
	uint8_t    Stage;                            //阶段
	uint8_t    AspState;                         //吸气状态
};

//气压式
struct tagAirSenPara
{
	uint8_t    ComStage;                         //通信阶段	
	uint8_t    State;                            //状态
	uint8_t    Buff[16];                         //数据缓存
	
	struct tagAirPress    AirPress;
	struct tagLLDData     ChColl[PLLD_CH_NUM];
	uint32_t   AppOrigBuff[PLLD_NUM];            //应用数据缓存
	uint32_t   AppFilterBuff[PLLD_NUM];          //应用数据滤波后缓存

	uint8_t    SlideWindNum;                     //开窗数量
	uint64_t   SlideWindSum;                     //开窗数据和
	uint32_t   AveVal;	                         //平均值
	uint16_t   PreData;                          //ch1上一次数据
	uint16_t   SlidePassSingle;                  //滑动通过设置
	uint16_t   SlidePassAll;                     //滑动通过设置
	
	uint8_t    Config[2];                        //配置

	int32_t    Atmos;                            //大气压
	int16_t    AtmosAve;                         //大气压平均值
	int32_t    AtmosAbs;                         //吸气时检测到的大气压
	int16_t    AtmosAbsAve;                      //吸气时检测到的大气压平均值
	int16_t    AtmosNoise;                       //大气压噪声
	
	int16_t    RakeRatio1Max;                    //曲线斜率最大值
	int16_t    RakeRatio1Min;                    //曲线斜率最小值
	int16_t    RakeRatio2Max;                    //曲线斜率最大值
	int16_t    RakeRatio2Min;                    //曲线斜率最小值

	int16_t    AspLiqNoise;                      //吸液时噪声
	int16_t    RakeRatio3Max;                    //吸液气压上限
	int16_t    RakeRatio3Min;                    //吸液气压下限
	
	int16_t    ABS_StartWait;                    //吸液启动等待
	int16_t    ABS_StartDly;                     //吸液启动延时
	int16_t    ABS_EndDly;                       //吸液结束延时
	int16_t    ABS_StartAirPress;                //吸液启动时气压
	
	
	uint8_t    CommAckBlock;                     //通信应答阻塞
	uint8_t    CommAckBlockDly;                  //通信应答阻塞延时
	uint8_t    ABS_Start_End;                    //吸液开始、吸液结束
	uint8_t    Dis_Start_End;                    //排液开始、吸液结束
	uint8_t    ABS_Real_Start_End;               //吸液真实开始、吸液真实结束
	
	uint8_t    sig1_cnt_1;                       //信号1计数
	uint8_t    sig1_cnt_2;                       //信号1计数
	uint8_t    sig2_cnt_1;                       //信号2计数
	uint8_t    sig2_cnt_2;                       //信号2计数
	uint8_t    sig3_cnt;                         //信号3计数
	uint8_t    sig4_cnt;                         //信号4计数
	uint8_t    sig5_cnt_1;                       //信号5计数
	uint8_t    sig5_cnt_2;                       //信号5计数
	uint8_t    sig6_cnt_1;                       //信号6计数
	uint8_t    sig6_cnt_2;                       //信号6计数
	uint8_t    sig7_cnt_1;                       //信号7计数
	uint8_t    sig7_cnt_2;                       //信号7计数
	uint8_t    sig8_cnt_1;                       //信号8计数
	uint8_t    sig8_cnt_2;                       //信号8计数
	uint8_t    sig8_cnt_3;                       //信号8计数
	uint8_t    sig8_cnt_4;                       //信号8计数
	uint8_t    sig8_cnt_5;                       //信号8计数
	
	
	uint8_t    AbsState1_1;                      //吸液状态1
	uint8_t    AbsState1_2;                      //吸液状态1
	uint8_t    AbsState2_1;                      //吸液状态2
	uint8_t    AbsState2_2;                      //吸液状态2
	uint8_t    AbsState3;                        //吸液状态3
	uint8_t    AbsState4;                        //吸液状态4
	uint8_t    AbsState5_1;                      //吸液状态5
	uint8_t    AbsState5_2;                      //吸液状态5
	uint8_t    AbsState6_1;                      //吸液状态6
	uint8_t    AbsState6_2;                      //吸液状态6
	uint8_t    AbsState7_1;                      //吸液状态7
	uint8_t    AbsState7_2;                      //吸液状态7
	uint8_t    AbsState8_1;                      //吸液状态8
	uint8_t    AbsState8_2;                      //吸液状态8
	uint8_t    AbsState8_3;                      //吸液状态8
	uint8_t    AbsState8_4;                      //吸液状态8
	uint8_t    AbsState8_5;                      //吸液状态8
	
	uint8_t    AbsState8_Timer;                  //吸液状态8持续时间
	uint8_t    AbsState8_Timer1;                 //吸液状态8持续时间
	uint8_t    AbsState8_Timer2;                 //吸液状态8持续时间
	uint8_t    AbsAirFollowState1;               //追随吸空状态1
	uint8_t    AbsAirFollowState2;               //追随吸空状态2
	
	uint8_t    LLDStage;                         //检测液面阶段	
	int16_t    PressVal;                         //气压值
	uint16_t   LLDResult;                        //液面探测结果	
};






//气压式
struct tagLLDMan
{
	uint8_t    SenType;                          //启用传感器	
	uint8_t    Result;                           //液面探测结果	
};








//声明变量----------------------------------------------------------------------//
extern struct tagCapSenPara    CapSenPara;
extern struct tagAirSenPara    AirSenPara;
extern struct tagLLDMan        LLDMan;




//声明函数----------------------------------------------------------------------//
uint8_t CapSenReadChl(void);
uint8_t CapSenChData(void);
uint8_t CapSenDeteLiqLevel(void);
void CapSensor(void);
void AirSensor(void);


uint8_t WriteCapSenReg(uint8_t *buf, uint8_t num);
uint8_t ReadCapSenReg(uint8_t *buf, uint8_t num);

uint8_t WriteAirSenReg(uint8_t *buf, uint8_t num);
uint8_t ReadAirSenReg(uint8_t *buf, uint8_t num);

uint8_t WriteSeleSen(uint8_t *buf, uint8_t num);
uint8_t ReadSelectSen(uint8_t *buf, uint8_t num);


uint8_t WriteCLLDPara1(uint8_t *buf, uint8_t num);
uint8_t ReadCLLDPara1(uint8_t *buf, uint8_t num);

uint8_t WriteCLLDPara3(uint8_t *buf, uint8_t num);
uint8_t ReadCLLDPara3(uint8_t *buf, uint8_t num);

uint8_t WriteCapMapVal(uint8_t *buf, uint8_t num);
uint8_t ReadCapMapVal(uint8_t *buf, uint8_t num);


uint8_t WriteAirLLDPara1(uint8_t *buf, uint8_t num);
uint8_t ReadAirLLDPara1(uint8_t *buf, uint8_t num);

uint8_t WriteAirLLDPara2(uint8_t *buf, uint8_t num);
uint8_t ReadAirLLDPara2(uint8_t *buf, uint8_t num);

uint8_t WriteAirLLDPara3(uint8_t *buf, uint8_t num);
uint8_t ReadAirLLDPara3(uint8_t *buf, uint8_t num);

uint8_t WriteAirMapVal(uint8_t *buf, uint8_t num);
uint8_t ReadAirMapVal(uint8_t *buf, uint8_t num);

uint8_t WriteAirLLDPara4(uint8_t *buf, uint8_t num);
uint8_t ReadAirLLDPara4(uint8_t *buf, uint8_t num);

uint8_t WriteAirLLDPara5(uint8_t *buf, uint8_t num);
uint8_t ReadAirLLDPara5(uint8_t *buf, uint8_t num);


uint8_t WriteUsePara(uint8_t *buf, uint8_t num);
uint8_t ReadUsePara(uint8_t *buf, uint8_t num);


uint8_t ReadLLDResult(uint8_t *buf, uint8_t num);
uint8_t ReadCapAirVal(uint8_t *buf, uint8_t num);


void LLDReslut(void);
void LLDInit(void);



#endif



/*****************************************************************************
Copyright  : BGI
File name  : liquid_level.c
Description: 检测液面
             1、当硬件IIC重新初始化之后，不能立即使用，需要等待若干毫秒之后再使用。
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#include "liquid_level.h"
#include "include.h"




//定义变量---------------------------------------------------------------------//
struct tagCapSenPara    CapSenPara;
struct tagAirSenPara    AirSenPara;
struct tagLLDMan        LLDMan;




//定义函数---------------------------------------------------------------------//
/*
 * @function: CapSenReset
 * @details : 复位传感器。复位传感器之后，总线被强占，奇怪。
 * @input   : NULL
 * @output  : NULL
 * @return  : 1：成功。其他：失败.
 */
uint8_t CapSenReset(void)
{
	uint8_t     result = 0;
	uint8_t     temp_dat[2] = {0x00};
	
	
	
	temp_dat[0] = 0x80;
	temp_dat[1] = 0x00;
	
	//硬件IIC
	result = I2C_Mem_Write(I2C2, CAP_DEV_ADDR, temp_dat, CAP_RESET_CMD_REG, 2);
	
	//模拟IIC
//	result = i2c_WriteBytes(&GPIOIIC[IIC_2], CAP_DEV_ADDR, CAP_RESET_CMD_REG, temp_dat, 2);	
//	if(0 == result)
//	{
//		result = 1;
//	}
//	else
//	{
//		result = 0;
//	}
	
	
	return (result);
}

/*
 * @function: CapSenConfig
 * @details : 配置传感器
 * @input   : NULL
 * @output  : NULL
 * @return  : 写结果，1：成功。其他：表示IIC写失败.
 */
uint8_t CapSenConfig(void)
{
	uint8_t     result = 0;
	uint8_t     temp_dat[2] = {0};
	
	
	
	
	//通道0、通道1的参考计数设置
	temp_dat[0] = CapSenPara.RCOUNT_CH0>>8;
	temp_dat[1] = CapSenPara.RCOUNT_CH0;
	if(0 == CapSenPara.ChlNmu)
	{
		result = I2C_Mem_Write(I2C2, CAP_DEV_ADDR, &temp_dat[0], (CAP_RCOUNT_CH0_REG + 0), 2);
	}
	else
	{
		result = I2C_Mem_Write(I2C2, CAP_DEV_ADDR, &temp_dat[0], (CAP_RCOUNT_CH0_REG + 1), 2);
	}
	
	//通道0、通道1的偏移值
//	temp_dat[2] = CapSenPara.OFFSET_CH0>>8;
//	temp_dat[3] = CapSenPara.OFFSET_CH0;
//	if(0 == CapSenPara.ChlNmu)
//	{
//		result = I2C_Mem_Write(I2C2, CAP_DEV_ADDR, &temp_dat[0], (CAP_OFFSET_CH0_REG + 0), 2);
//	}
//	else
//	{
//		result = I2C_Mem_Write(I2C2, CAP_DEV_ADDR, &temp_dat[0], (CAP_OFFSET_CH0_REG + 1), 2);
//	}
	
	//通道0、通道1沉降参考计数
	temp_dat[0] = CapSenPara.SETTLECOUNT_CH0>>8;
	temp_dat[1] = CapSenPara.SETTLECOUNT_CH0;
	if(0 == CapSenPara.ChlNmu)
	{
		result = I2C_Mem_Write(I2C2, CAP_DEV_ADDR, &temp_dat[0], (CAP_SETTLECOUNT_CH0_REG + 0), 2);
	}
	else
	{
		result = I2C_Mem_Write(I2C2, CAP_DEV_ADDR, &temp_dat[0], (CAP_SETTLECOUNT_CH0_REG + 1), 2);
	}
	
	//通道0、通道1的基准除法器设置
	temp_dat[0] = CapSenPara.CLOCK_DIVIDERS_CH0>>8;
	temp_dat[1] = CapSenPara.CLOCK_DIVIDERS_CH0;
	if(0 == CapSenPara.ChlNmu)
	{
		result = I2C_Mem_Write(I2C2, CAP_DEV_ADDR, &temp_dat[0], (CAP_CLOCK_DIVIDERS_CH0_REG + 0), 2);
	}
	else
	{
		result = I2C_Mem_Write(I2C2, CAP_DEV_ADDR, &temp_dat[0], (CAP_CLOCK_DIVIDERS_CH0_REG + 1), 2);
	}
	
	//通道0、通道1的传感器电流驱动配置
	temp_dat[0] = CapSenPara.DRIVE_CURRENT_CH0>>8;
	temp_dat[1] = CapSenPara.DRIVE_CURRENT_CH0;
	if(0 == CapSenPara.ChlNmu)
	{
		result = I2C_Mem_Write(I2C2, CAP_DEV_ADDR, &temp_dat[0], (CAP_DRIVE_CURRENT_CH0_REG + 0), 2);
	}
	else
	{
		result = I2C_Mem_Write(I2C2, CAP_DEV_ADDR, &temp_dat[0], (CAP_DRIVE_CURRENT_CH0_REG + 1), 2);
	}
	
	//状态配置
	temp_dat[0] = CapSenPara.STATUS_CONFIG>>8;
	temp_dat[1] = CapSenPara.STATUS_CONFIG;
	result = I2C_Mem_Write(I2C2, CAP_DEV_ADDR, &temp_dat[0], CAP_STATE_CFG_REG, 2);
	
	//信道复用配置
	temp_dat[0] = CapSenPara.MUX_CONFIG>>8;
	temp_dat[1] = CapSenPara.MUX_CONFIG;
	result = I2C_Mem_Write(I2C2, CAP_DEV_ADDR, &temp_dat[0], CAP_MUX_CONFIG_REG, 2);
	
	//转换配置
	temp_dat[0] = CapSenPara.CONFIG>>8;
	temp_dat[1] = CapSenPara.CONFIG;
	result = I2C_Mem_Write(I2C2, CAP_DEV_ADDR, &temp_dat[0], CAP_CONFIG_REG, 2);
	
	
	//复位设备阶段
	CapSenPara.ResStatu = CP_COMM_INIT;
	CapSenPara.UpdateCnt = 0;
	CapSenPara.UpdateNoCnt = 0;
	CapSenPara.ReadCnt = 0;
	
	
	
	return (result);
}

/*
 * @function: CapSenResetRCOUNT
 * @details : 重新配置传感器转换时间
 * @input   : NULL
 * @output  : NULL
 * @return  : 写结果，1：成功。其他：表示IIC写失败.
 */
uint8_t CapSenResetRCOUNT(void)
{
	uint8_t     result = 0;
//	uint8_t     temp_dat[2] = {0};
	uint32_t    calc_time = 0;
	
	
	
	//数据转换时间：tCx = (CHx_RCOUNT * 16) / fREFx
	//CHx_RCOUNT = tCx * fREFx / 16.
	calc_time = UserPara[CAP_CLOCK_DIVIDERS_CH0].Value & 0x00FF;    //获取fREFx，放大100倍进行计算。
	calc_time = 40000 * 100 / calc_time;
	
	calc_time = calc_time * UserPara[CAP_SET_CONTIME].Value;
	UserPara[CAP_RCOUNT_CH0].Value = calc_time / 16 / 100;
	
	UserPara[CAP_RCOUNT_CH0].Value = UserPara[CAP_RCOUNT_CH0].Value + 1;
	CapSenPara.RCOUNT_CH0 = UserPara[CAP_RCOUNT_CH0].Value;
	
	
	return (result);
}

/*
 * @function: CapSenResetDriveCurrent
 * @details : 重新配置传感器驱动电流
 * @input   : NULL
 * @output  : NULL
 * @return  : 写结果，1：成功。其他：表示IIC写失败.
 */
uint8_t CapSenResetDriveCurrent(void)
{
	uint8_t     result = 0;
	
	
	return (result);
}

/*
 * @function: CapSenReadStatus
 * @details : 读状态寄存器
 * @input   : NULL
 * @output  : NULL
 * @return  : 读结果，1：成功。其他：表示IIC读取失败.
 */
uint8_t CapSenReadStatus(void)
{
	uint8_t     result = 0;
	
	
	__disable_irq();
	//数据高位寄存器、低位寄存器分开读，不能连续读。
	result = I2C_Mem_Read(I2C2, CAP_DEV_ADDR, &CapSenPara.Buff[0], CAP_STATE_REG, 2);
	__enable_irq(); 
	
	
	return (result);
}


/*
 * @function: CapSenReadChl
 * @details : 读ch1的数据。
              1.当数据转换完毕，RDY信号由高电平转为低电平；读取数据寄存器之后，RDY信号由低电平转为高电平。
 * @input   : NULL
 * @output  : NULL
 * @return  : 读结果，1：成功。其他：表示IIC读取失败.
 */
uint8_t CapSenReadChl(void)
{
	uint8_t     result = 0;
	
	
	//数据高位寄存器、低位寄存器分开读，不能连续读。
	__disable_irq();
	if(0 == CapSenPara.ChlNmu)
	{
		result = I2C_Mem_Read(I2C2, CAP_DEV_ADDR, &CapSenPara.Buff[2], CAP_DATA_CH0_REG, 2);
		result = I2C_Mem_Read(I2C2, CAP_DEV_ADDR, &CapSenPara.Buff[4], CAP_DATA_LSB_CH0_REG, 2);
	}
	else
	{
		result = I2C_Mem_Read(I2C2, CAP_DEV_ADDR, &CapSenPara.Buff[6], CAP_DATA_CH1_REG, 2);
		result = I2C_Mem_Read(I2C2, CAP_DEV_ADDR, &CapSenPara.Buff[8], CAP_DATA_LSB_CH1_REG, 2);
	}
	__enable_irq(); 
	
	
	return (result);
}

/*
 * @function: CapSenSleepEn
 * @details : 传感器睡眠
 * @input   : 1.enable：使能睡眠？
 * @output  : NULL
 * @return  : 写结果，1：成功。其他：表示IIC写失败.
 */
uint8_t CapSenSleepEn(uint8_t enable)
{
	uint8_t     result = 0;
	uint8_t     temp_dat[2] = {0};
	
	
	//转换配置
	temp_dat[0] = CapSenPara.CONFIG>>8;
	if(TRUE == enable)
	{
		temp_dat[0] = temp_dat[0] | 0x20;
	}
	
	temp_dat[1] = CapSenPara.CONFIG;
	
	result = I2C_Mem_Write(I2C2, CAP_DEV_ADDR, &temp_dat[0], CAP_CONFIG_REG, 2);
	
	
	return (result);
}


/*
 * @function: CapSenChlEn
 * @details : 启用通道0、通道1，默认启用通道0,。
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
uint8_t CapSenChlEn(void)
{
	uint8_t     result = 0;
	
	
	if(1 == CapSenPara.ChlNmu)
	{
		UserPara[CAP_CONFIG].Value = 0x5601;
	}
	else
	{
		UserPara[CAP_CONFIG].Value = 0x1601;
	}
	CapSenPara.CONFIG = UserPara[CAP_CONFIG].Value;
	
	
	
	return (result);
}


/*
 * @function: CapSenChlSelect
 * @details : 自动启用通道0、通道1，默认启用通道0,。
 * @input   : NULL
 * @output  : NULL
 * @return  : 0：无需切换；1：需要切换.
 */
uint8_t CapSenChlSelect(void)
{
	uint8_t     result = FALSE;
	
	
	if(1 == CapSenPara.ChlNmu)
	{
		CapSenPara.ChlNmu = 0;
		result = TRUE;
	}
	else
	{
		CapSenPara.ChlNmu = 1;
		result = TRUE;
	}
	
	
	return (result);
}




/*
 * @function: CapSenChData
 * @details : 提取通道检测的数据
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
#ifdef CLLD_DEBUG
#define    CAP_DIS_NUM    75
int32_t    CapDataDis[CAP_DIS_NUM] = {0};
int32_t    cap_offset = 123000000;
#endif
uint8_t CapSenChData(void)
{
	uint8_t    ret = FALSE;
	uint8_t    index = 0;
	
	uint8_t    reg_rdy = 0;
	uint8_t    reg_ch0 = 0;
	uint8_t    reg_ch1 = 0;
	
	uint32_t   cal_temp = 0;
	uint32_t   ch0_temp = 0;
	uint32_t   ch1_temp = 0;
	
	float      lpf_ratio = 0;
	int32_t    cal_dis = 0;
	
	
	
	//虽然使用硬件INTB信号下降沿作为判断电容通道数据转换是否完成的标志，
	//不过使用寄存器DRDY判断一次，双保险。
	
	//bit6：DRDY寄存器，至少有一个容性通道的转换转换完成。
	reg_rdy = CapSenPara.Buff[1] & 0x40;
	if(reg_rdy)
	{
		Accumulation16(&CapSenPara.UpdateCnt);
	}
	else
	{
		Accumulation16(&CapSenPara.UpdateNoCnt);
	}
	Accumulation32(&CapSenPara.ReadCnt);
	
	
	
	CapSenPara.UpdateFlag = FALSE;
	CapSenPara.Ch0UnRead = FALSE;
	CapSenPara.Ch1UnRead = FALSE;
	//bit3：表示容性通0上的数据是否被读走。0：没有未读取的数据；1：有未读取的数据。
	reg_ch0 = CapSenPara.Buff[1] & 0x08;
	if(reg_ch0)
	{
		CapSenPara.Ch0UnRead = TRUE;
		CapSenPara.UpdateFlag = TRUE;
	}
	
	//bit2：表示容性通1上的数据是否被读走。0：没有未读取的数据；1：有未读取的数据。
	reg_ch1 = CapSenPara.Buff[1] & 0x04;
	if(reg_ch1)
	{
		CapSenPara.Ch1UnRead = TRUE;
		CapSenPara.UpdateFlag = TRUE;
	}
	
	
	if(TRUE == CapSenPara.UpdateFlag)
	{
		//电容芯片初始化成功，并且数据稳定
//		if((CP_COMM_INIT == CapSenPara.ResStatu)
//			&& (CapSenPara.UpdateCnt > CapSenPara.ResWaitTime))
		if(CP_COMM_INIT == CapSenPara.ResStatu)
		{
			CapSenPara.ResStatu = CP_COMM_NORMAL;
			
			CapSenPara.UpdateCnt = 0;
			CapSenPara.UpdateNoCnt = 0;
			CapSenPara.ReadCnt = 0;
		}
		
		
		
		//通道0数据
		ch0_temp = MERGE_UINT16(CapSenPara.Buff[2], CapSenPara.Buff[3]);
		ch0_temp = ch0_temp<<16;
		ch0_temp = ch0_temp | MERGE_UINT16(CapSenPara.Buff[4], CapSenPara.Buff[5]);
		ch0_temp = ch0_temp & 0x0FFFFFFF;
		
		//通道1数据
		ch1_temp = MERGE_UINT16(CapSenPara.Buff[6], CapSenPara.Buff[7]);
		ch1_temp = ch1_temp<<16;
		ch1_temp = ch1_temp | MERGE_UINT16(CapSenPara.Buff[8], CapSenPara.Buff[9]);
		ch1_temp = ch1_temp & 0x0FFFFFFF;
		
		
		CapSenPara.ChColl[CLLD_CH0_DATA].IsErr = FALSE;
		CapSenPara.ChColl[CLLD_CH0_DATA].OrigValue = ch0_temp;
		CapSenPara.ChColl[CLLD_CH1_DATA].IsErr = FALSE;
		CapSenPara.ChColl[CLLD_CH1_DATA].OrigValue = ch1_temp;
		
		
		
		if(1 == CapSenPara.ChlNmu)
		{
			cal_temp = ch1_temp;
		}
		else
		{
			cal_temp = ch0_temp;
		}
		
		//一阶滤波（防干扰作用非常小，有干扰时根本防不住）
//		lpf_ratio = 0.10;
//		lpf_ratio = 0.50;
//		lpf_ratio = 0.60;
//		lpf_ratio = 0.75;
		lpf_ratio = 0.80;    //经过大量测试，能用。
//		lpf_ratio = 0.85;
//		lpf_ratio = 0.90;    //测试中。
//		lpf_ratio = 0.95;
//		lpf_ratio = 1.00;
		CapSenPara.ChColl[CLLD_CH0_DATA].FilterValue = LPF1(cal_temp, CapSenPara.ChColl[CLLD_CH0_DATA].FilterValue, lpf_ratio);
		cal_temp = CapSenPara.ChColl[CLLD_CH0_DATA].FilterValue;
		
		
#ifdef CLLD_DEBUG
		cal_dis = CapSenPara.ChColl[CLLD_CH0_DATA].FilterValue - cap_offset;
		SmoothPipeline32(CapDataDis, cal_dis, CAP_DIS_NUM);
#endif	
		
		//丢掉前几个不稳定数据
		if((CapSenPara.UpdateCnt <= CapSenPara.ResWaitTime)
			|| (LLD_CLOSE == UserPara[LLD_SEN_FUN].Value))
		{
			cal_temp = 0;
		}
		SmoothPipeline32(CapSenPara.ChColl[CLLD_CH0_DATA].FilterData, cal_temp, CLLD_NUM);
		
		//检查窗口有效数据
		CapSenPara.SlideWindOpen = TRUE;
		for(index = 0; index < CapSenPara.SlideWindNum; index++)
		{
			if(0 == CapSenPara.ChColl[CLLD_CH0_DATA].FilterData[index])
			{
				CapSenPara.SlideWindOpen = FALSE;
			}
		}
		
		
		
		
		//重庆普思平均滤波
//		CapSenPara.SlideWindSum = CapSenPara.SlideWindSum - CapSenPara.ChColl[CLLD_CH0_DATA].OrigData[CLLD_NUM - 1];
//		SmoothPipeline32(CapSenPara.ChColl[CLLD_CH0_DATA].OrigData, cal_temp, CLLD_NUM);
//		CapSenPara.SlideWindSum = CapSenPara.SlideWindSum + cal_temp;
//		CapSenPara.AveVal = CapSenPara.SlideWindSum / CLLD_NUM;
//		SmoothPipeline32(CapSenPara.ChColl[CLLD_CH0_DATA].FilterData, CapSenPara.AveVal, CLLD_NUM);
		
		
		
		
		//计算电容值（电容没有实际用途）
	}
	
	
	return (ret);
}


/*
 * @function: CapSenDeteLiqLevel
 * @details : 检测探针是否接触到液面（判断电容相对变化量）。探针接触到液面，电容值会骤然下降。

              优点是抗干扰能力强。
              缺点：
              1.代码复杂；2.相邻两次探测时间间隔长（100ms以上）。
              3.与TIP的Z轴电机运动速度有关系，AD缓存数量是10个值，
              当电机运动较慢时，10个AD值相差不大。
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
uint8_t CapSenDeteLiqLevel(void)
{
	uint8_t    ret = 0;
	uint8_t    i = 0;
	uint8_t    vaild_cnt = 0;            //有效信号次数
//	uint8_t    invaild_cnt = 0;          //无效信号次数
//	uint8_t    check_point_num = 0;      //检查点数量
	uint8_t    set_compare_num = 0;      //设置比较次数
	uint8_t    set_pass_single = 0;      //设置单次通过
	uint8_t    set_pass_all = 0;         //设置全部通过
	int32_t    temp = 0;
	int32_t    sub1 = 0;
	int32_t    sub2 = 0;
	
	
	set_compare_num = CapSenPara.SlideWindNum;
	set_pass_single = CapSenPara.SlidePassSingle;
	set_pass_all = CapSenPara.SlidePassAll;
	
	
	//各个状态下执行的动作
	switch(CapSenPara.LLDStage)
	{
		case LLD_STAGE_NULL:
		{
			CapSenPara.LLDResult = LLD_IDLE;
			
			//数据有更新
			if((TRUE == CapSenPara.UpdateFlag) && (TRUE == CapSenPara.SlideWindOpen))
			{
				//算法2：判断多次相对变化，提高抗干扰能力，但耗时较多，吸头过冲稍微大一些。
//				for(i = 1; i < set_compare_num; i++)
//				{
//					if(CLLD_ABS_BLOCK == UserPara[LLD_SEN_FUN].Value)
//					{
//						sub1 = CapSenPara.ChColl[CLLD_CH0_DATA].FilterData[0];
//						sub2 = CapSenPara.ChColl[CLLD_CH0_DATA].FilterData[i];
//					}
//					else
//					{
//						sub1 = CapSenPara.ChColl[CLLD_CH0_DATA].FilterData[i];
//						sub2 = CapSenPara.ChColl[CLLD_CH0_DATA].FilterData[0];
//					}
//					
//					temp = sub1 - sub2;
//					if(temp > (int32_t)CapSenPara.ContactThreshold)
//					{
//						vaild_cnt++;
//					}
//				}
//				
//				//此次识别边沿正常。
//				if(vaild_cnt >= set_pass_single)
//				{
//					CapSenPara.SlideCnt++;
//				}
//				else
//				{
//					CapSenPara.SlideCnt = 0;
//				}
//				
//				//连续n次识别边沿成功
//				if(CapSenPara.SlideCnt >= set_pass_all)
//				{
//					CapSenPara.SlideCnt = 0;
//					
//					//保存前几次数据，下面还会使用。
//					CapSenPara.ChlPreData = CapSenPara.ChColl[CLLD_CH0_DATA].OrigValue;
//					CapSenPara.LLDStage = LLD_STAGE_LIQ;
//				}
				
				
				
				//算法5：简化算法，抗干扰能力弱，但耗时较少，吸头过冲较好。
				for(i = 1; i < set_compare_num; i++)
				{
					sub1 = CapSenPara.ChColl[CLLD_CH0_DATA].FilterData[i];
					sub2 = CapSenPara.ChColl[CLLD_CH0_DATA].FilterData[0];
					
					temp = sub1 - sub2;	
					if(temp > (int32_t)CapSenPara.ContactThreshold)
					{
						vaild_cnt++;
					}
				}
				
				if(vaild_cnt >= set_pass_single)
				{
					//保存前几次数据，下面还会使用。
					CapSenPara.ChlPreData = CapSenPara.ChColl[CLLD_CH0_DATA].FilterData[0];
					
					CapSenPara.LLDStage = LLD_STAGE_LIQ;
				}
				
				
				
				
				//重庆普思滑动窗识别算法
//				temp = CapSenPara.AveVal - CapSenPara.ChColl[CLLD_CH0_DATA].OrigValue;
//				if(temp > (int32_t)CapSenPara.ContactThreshold)
//				{
//					//保存前几次数据，下面还会使用。
//					CapSenPara.ChlPreData = CapSenPara.AveVal;
//					
//					CapSenPara.LLDStage = LLD_STAGE_LIQ;
//				}
			}
			
			
			
			
			//开启电容探测，才需要往下走
			//这动作十分重要，否则上电后会误判
			if(LLD_CLOSE == UserPara[LLD_SEN_FUN].Value)
			{
				CapSenPara.LLDStage = LLD_STAGE_NULL;
			}
		}
		break;
		
		case LLD_STAGE_START:
		{
		}
		break;
		
		case LLD_STAGE_CONTACT:
		{
		}
		break;
		
		case LLD_STAGE_LIQ:
		{
			CapSenPara.LLDResult = LLD_LIQUL;
			
			
			//数据有更新
			if((TRUE == CapSenPara.UpdateFlag) && (TRUE == CapSenPara.SlideWindOpen))
			{			
				if(FALSE == CapSenPara.ChColl[CLLD_CH0_DATA].IsErr)
				{			
//					//单次判断TIP是否离开液面。抗干扰性能差。
//					temp = CapSenPara.ChlPreData - CapSenPara.ChColl[CLLD_CH0_DATA].FilterData[0];
//					if(temp < (int16_t)CapSenPara.LeaveThreshold)
//					{
//						CapSenPara.LLDStage = LLD_STAGE_NULL;
//					}
//					
//					
//					//比较多组数据，提高抗干扰能力。
//					//所有的数据都接近CapSenPara.Ch1PreData，才认为TIP头离开了液面
//					for(i = 0; i < set_compare_num; i++)
//					{				
//						temp = CapSenPara.ChColl[CLLD_CH0_DATA].FilterData[i] - CapSenPara.ChlPreData;
//						if(temp > (int32_t)CapSenPara.LeaveThreshold)
//						{
//							vaild_cnt++;
//						}
//					}
//					if(vaild_cnt >= set_compare_num)
//					{
//						CapSenPara.LLDStage = LLD_STAGE_NULL;
//					}
					
					
					//加大开窗宽度，否则可能看不到数据变化。
					for(i = 1; i < set_compare_num; i++)
					{
						sub1 = CapSenPara.ChColl[CLLD_CH0_DATA].FilterData[0];
						sub2 = CapSenPara.ChColl[CLLD_CH0_DATA].FilterData[i];
						
						temp = sub1 - sub2;	
						if(temp > (int32_t)CapSenPara.ContactThreshold)
						{
							vaild_cnt++;
						}
					}
				}
			}
			
			//吸头脱离液面
			if(vaild_cnt >= set_pass_single)
			{
				CapSenPara.LLDStage = LLD_STAGE_NULL;
			}
			
			//开启电容探测，才需要往下走
			//这动作十分重要，否则上电后会误判
			if(LLD_CLOSE == UserPara[LLD_SEN_FUN].Value)
			{
				CapSenPara.LLDStage = LLD_STAGE_NULL;
			}
			
			//传感器故障
			if(TRUE == CapSenPara.ChColl[CLLD_CH0_DATA].IsErr)
			{
				CapSenPara.LLDStage = LLD_STAGE_NULL;
			}
		}
		break;
		
		case LLD_STAGE_FAIL:
		{
		}
		break;
		
		case LLD_STAGE_BUBBLE:
		{
		}
		break;
		
		case LLD_STAGE_ABS_BLOCK:
		{
		}
		break;
		
		default:
		{
			CapSenPara.LLDStage = LLD_STAGE_NULL;
		}
		break;
	}
	
	
	return (ret);
}



/*
 * @function: CapSenSlideWind
 * @details : 开窗数量、开窗后丢弃干扰数据
 * @input   : NULL
 * @output  : NULL
 * @return  : 开窗档位
 */
uint8_t CapSenSlideWind(void)
{
	uint8_t    ret = FALSE;
	uint16_t   temp = 0;
	
	
	ret = CapSenPara.ConTime / 10;
	temp = (ret * 10) + 1;
	switch(temp)
	{
		case CLLD_TRANSFER_TIME0:
		case CLLD_TRANSFER_TIME1:
		{
			CapSenPara.SlideWindNum = CLLD_SLIDEWIND_NUM1;
			CapSenPara.ResWaitTime = CLLD_RES_WAIT_TIME1;
		}
		break;
		
		case CLLD_TRANSFER_TIME2:
		{
			CapSenPara.SlideWindNum = CLLD_SLIDEWIND_NUM2;
			CapSenPara.ResWaitTime = CLLD_RES_WAIT_TIME2;
		}
		break;
		
		case CLLD_TRANSFER_TIME3:
		{
			CapSenPara.SlideWindNum = CLLD_SLIDEWIND_NUM3;
			CapSenPara.ResWaitTime = CLLD_RES_WAIT_TIME3;
		}
		break;
		
		case CLLD_TRANSFER_TIME4:
		{
			CapSenPara.SlideWindNum = CLLD_SLIDEWIND_NUM4;
			CapSenPara.ResWaitTime = CLLD_RES_WAIT_TIME4;
		}
		break;
		
		case CLLD_TRANSFER_TIME5:
		{
			CapSenPara.SlideWindNum = CLLD_SLIDEWIND_NUM5;
			CapSenPara.ResWaitTime = CLLD_RES_WAIT_TIME5;
		}
		break;
		
		case CLLD_TRANSFER_TIME6:
		{
			CapSenPara.SlideWindNum = CLLD_SLIDEWIND_NUM6;
			CapSenPara.ResWaitTime = CLLD_RES_WAIT_TIME6;
		}
		break;
		
		case CLLD_TRANSFER_TIME7:
		{
			CapSenPara.SlideWindNum = CLLD_SLIDEWIND_NUM7;
			CapSenPara.ResWaitTime = CLLD_RES_WAIT_TIME7;
		}
		break;
		
		case CLLD_TRANSFER_TIME8:
		{
			CapSenPara.SlideWindNum = CLLD_SLIDEWIND_NUM8;
			CapSenPara.ResWaitTime = CLLD_RES_WAIT_TIME8;
		}
		break;
		
		case CLLD_TRANSFER_TIME9:
		{
			CapSenPara.SlideWindNum = CLLD_SLIDEWIND_NUM9;
			CapSenPara.ResWaitTime = CLLD_RES_WAIT_TIME9;
		}
		break;
		
		default:
		{
			CapSenPara.SlideWindNum = CLLD_SLIDEWIND_NUM10;
			CapSenPara.ResWaitTime = CLLD_RES_WAIT_TIME10;
		}
		break;
	}
	
	
	return (ret);
}


/*
 * @function: CapSenCheckRCOUNT
 * @details : 检查是否需要配置电容传感器转换时间
 * @input   : NULL
 * @output  : NULL
 * @return  : 是否需要配置
 */
uint8_t CapSenCheckRCOUNT(void)
{
	uint8_t    ret = FALSE;
	
	
	//转换时间
	if(CapSenPara.ConTime != UserPara[CAP_SET_CONTIME].Value)
	{
		CapSenPara.ConTime = UserPara[CAP_SET_CONTIME].Value;
		
		ret = TRUE;
	}
	
	
	return (ret);
}


/*
 * @function: CapSenCheckDriveCurrent
 * @details : 检查是否需要配置驱动电流
 * @input   : NULL
 * @output  : NULL
 * @return  : 是否需要重写传感器的配置参数
 */
uint8_t CapSenCheckDriveCurrent(void)
{
	uint8_t    ret = FALSE;
	uint16_t   temp = 0;
	
	
	//驱动电流
	temp = UserPara[CAP_DRIVE_CURRENT_CH0].Value<<11;
	if(CapSenPara.DRIVE_CURRENT_CH0 != temp)
	{
		CapSenPara.DRIVE_CURRENT_CH0 = temp;
		
		ret = TRUE;
	}
	
	
	return (ret);
}


/*
 * @function: CapSensor
 * @details : 电容传感器
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void CapSensor(void)
{
	uint8_t    result = 0;
	uint8_t    iic_ack = IIC_ACR_NORMAL;
	uint8_t    cfg_rcount = FALSE;
	uint8_t    cfg_current = FALSE;
	
	
	
	//各个状态下执行的动作
	switch(CapSenPara.ComStage)
	{
		case CP_COMM_RESET_DEV:
		{
			GPIOIIC[IIC_2].State = IIC_RESET_DEV;
			
			
			//退出关机模式
//			FDC2212ShutDownInput(IO_LOW);
			
			rt_enter_critical();    //进入临界段
			//IIC通信复位
			iic_ack = CapSenReset();
			rt_exit_critical();     //退出临界段
			
			CapSenPara.ComStage = CP_COMM_INIT;
		}
		break;
		
		case CP_COMM_INIT:
		{
			GPIOIIC[IIC_2].State = IIC_INIT;
			
			
			//退出关机模式
//			FDC2212ShutDownInput(IO_LOW);
			
			//选择通道0或通道1
			CapSenChlEn();
			
			rt_enter_critical();    //进入临界段
			//发送配置数据
			iic_ack = CapSenConfig();
			rt_exit_critical();     //退出临界段
			
			if(IIC_ACR_NORMAL == iic_ack)
			{
				CapSenPara.ComStage = CP_COMM_NORMAL;
			}
			else
			{
				CapSenPara.ComStage = CP_COMM_RELEASE_IIC;
			}
		}
		break;
		
		case CP_COMM_NORMAL:
		{
			//开窗数量、丢弃不稳定数据
			CapSenSlideWind();
			
			
			//通过RDY信号下降沿判断数据是否准备完毕。
			if(TRUE == CapSenPara.Rdy)
			{
				CapSenPara.Rdy = FALSE;
				CapSenPara.RdyTime = 0;
				
				//1、进入临界段，有改善效果，但仍然偶发故障，概率约0.5%。可能是原子锁有时间限制。
				//2、使用Delay_MS_NOBlock替换掉电机驱动函数、存储函数中的rt_thread_delay，无改善。
				rt_enter_critical();    //进入临界段。
				//读取状态寄存器
				iic_ack = CapSenReadStatus();
				
				//读取电容数据
				iic_ack = CapSenReadChl();
				rt_exit_critical();     //退出临界段
				
				
				//提取数据
				CapSenChData();
			}
			
			
			//1.配置转换/采样时间
			cfg_rcount = CapSenCheckRCOUNT();
			if(TRUE == cfg_rcount)
			{
				//动态直接修改寄存器值，可能不利于传感器稳定工作。
				result = CapSenResetRCOUNT();
				
				CapSenPara.ResStatu = CP_COMM_RESET_DEV;
			}
			
			
			//2.配置驱动电流
			cfg_current = CapSenCheckDriveCurrent();
			if(TRUE == cfg_current)
			{
				//动态直接修改寄存器值，可能不利于传感器稳定工作。
				result = CapSenResetDriveCurrent();
				
				CapSenPara.ResStatu = CP_COMM_RESET_DEV;
			}
			
			
			//3.Z轴电机变速点容易出现干扰，清除波动数据。
			//收到开启探测指令，实际丢弃数据个数为(CapSenPara.ResStatu + CapSenPara.SlideWindNum)
			if(CP_COMM_INIT == CapSenPara.ResStatu)
			{
				CapSenPara.SlideWindOpen = FALSE;
				
				Array32Clear(CapSenPara.ChColl[CLLD_CH0_DATA].FilterData, CLLD_NUM);
			}
			else if(CP_COMM_NORMAL == CapSenPara.ResStatu)
			{
				if(LLD_CLOSE == UserPara[LLD_SEN_FUN].Value)
				{
					CapSenPara.SlideWindOpen = FALSE;
					CapSenPara.UpdateCnt = 0;
					CapSenPara.UpdateNoCnt = 0;
					CapSenPara.ReadCnt = 0;
					
					Array32Clear(CapSenPara.ChColl[CLLD_CH0_DATA].FilterData, CLLD_NUM);
				}
			}
			
//			//测试变速点与收到探测指令时间间隔
//			if(LLD_CLOSE != UserPara[LLD_SEN_FUN].Value)
//			{
//				UserPara[LLD_SEN_FUN].Value;
//			}
			
			
			
			//4.检测液面
			CapSenDeteLiqLevel();
			
			
			//5.传感器设备通信是否正常
			if(IIC_ACR_NORMAL != iic_ack)
			{
				GPIOIIC[IIC_2].Dev1Err = TRUE;
				GPIOIIC[IIC_2].Dev1CommReset = TRUE;
				
				CapSenPara.ChColl[CLLD_CH0_DATA].OrigValue = 0;
				CapSenPara.ChColl[CLLD_CH1_DATA].OrigValue = 0;
			}
			else
			{
				GPIOIIC[IIC_2].Dev1Err = FALSE;
			}
			
			
			//6.探测到液面，关闭电容探测信号，降低相互干扰。
//			if(LLD_STAGE_LIQ == CapSenPara.LLDStage)
//			{
//				CapSenPara.SleepOpen = TRUE;
//			}
//			else
//			{
//				CapSenPara.SleepOpen = FALSE;
//			}
			if(CLLD_LIQUL == UserPara[LLD_SEN_FUN].Value)
			{
				if(LLD_STAGE_LIQ == CapSenPara.LLDStage)
				{
					CapSenPara.SleepOpen = TRUE;
				}
				else
				{
					CapSenPara.SleepOpen = FALSE;
				}
			}
			else
			{
				;
			}

			
			
			//7.关闭电容探测信号，为了让注检EMC通过。
			if(LLD_CLOSE == UserPara[LLD_SEN_FUN].Value)
			{
				CapSenPara.SleepOpen = TRUE;
			}
			
			
			//8.传感器未初始化完成，不进入睡眠
			//手动打开调试
			if((CP_COMM_INIT == CapSenPara.ResStatu)
				|| (TRUE == CapSenPara.SleepManualCtl))
			{
				CapSenPara.SleepOpen = FALSE;
			}
			
			
			//9.电容传感器进入睡眠
			if(CapSenPara.SleepStatu != CapSenPara.SleepOpen)
			{
				rt_enter_critical();    //进入临界段
				iic_ack = CapSenSleepEn(CapSenPara.SleepOpen);
				rt_exit_critical();     //退出临界段
				
				CapSenPara.SleepStatu = CapSenPara.SleepOpen;
			}
			
			
			//10.IIC总线状态
			if((TRUE == GPIOIIC[IIC_2].Dev1CommReset)
				|| (TRUE == GPIOIIC[IIC_2].Dev2CommReset))
			{
				GPIOIIC[IIC_2].State = IIC_RELEASE;
				GPIOIIC[IIC_2].Dev1CommReset = FALSE;
				GPIOIIC[IIC_2].Dev2CommReset = FALSE;
			}
			else
			{
				GPIOIIC[IIC_2].State = IIC_BUS_NORMAL;
			}
			
			
			
			
			//11.使用Rdy信号作为触发读取电容数据的依据，当芯片出现异常时，超时未读取到数据，认为芯片有故障，或者通道未工作。
			//芯片睡眠期间，不采集数据，因此Rdy信号不出现是正常现象。
			if(TRUE == CapSenPara.SleepStatu)
			{
				CapSenPara.RdyTime = 0;
			}
			if(CapSenPara.RdyTime >= 3)
			{
				CapSenPara.RdyTime = 0;
				
				//自动选择电容通道
				result = CapSenChlSelect();
				
				CapSenPara.ResStatu = CP_COMM_RESET_DEV;
			}
			
			
			
			
			
			
			
			
			if(CP_COMM_RESET_DEV == CapSenPara.ResStatu)
			{
				//重启设备
				CapSenPara.ComStage = CP_COMM_RESET_DEV;
			}
			
			if(IIC_RELEASE == GPIOIIC[IIC_2].State)
			{
				//IIC2总线被强占，需要释放总线。
				CapSenPara.ComStage = CP_COMM_RELEASE_IIC;
			}
		}
		break;
		
		case CP_COMM_RELEASE_IIC:
		{
			rt_enter_critical();    //进入临界段
			
			//释放通信总线
			GPIOIIC[IIC_2].State = IIC_RELEASE;
			ReleaseIIC(I2C2, &GPIOIIC[IIC_2]);
			
			rt_exit_critical();     //退出临界段
			
			
			CapSenPara.ComStage = CP_COMM_RESET_IIC;
		}
		break;
		
		case CP_COMM_RESET_IIC:
		{
			rt_enter_critical();    //进入临界段
			
			//硬件IIC重置
			GPIOIIC[IIC_2].State = IIC_RESET;
			IIC2_Reset();
			
			rt_exit_critical();     //退出临界段
			
			
			CapSenPara.ComStage = CP_COMM_RESET_DEV;
		}
		break;
		
		default:
		{
			CapSenPara.ComStage = CP_COMM_RESET_DEV;
		}
		break;
	}
}


/*
 * @function: CapSenInit
 * @details : 初始化
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void CapSenInit(void)
{
	uint32_t    con_liq_threshold = 0;      //识别接触液面阈值
	uint32_t    leave_liq_threshold = 0;    //识别脱离液面阈值
//	uint8_t     result = 0;
//	
//	
//	result = I2C_Mem_Read(I2C2, CAP_DEV_ADDR, &CapSenPara.Buff[0], CAP_DEVICE_ID, 2);
//	if((0x30 == CapSenPara.Buff[0]) && (0x55 == CapSenPara.Buff[1]))
//	{
//		//芯片是FDC2212
//	}
	
	
	
	
	//当FDC开启多信道模式时，每个信道的工作时间是三个部分的总和：
	//（1）传感器激活时间sensor activation time.
	//（2）转换时间conversion time
	//（3）通道切换延迟channel switch delay
	
	
	//数据转换时间：tCx = (CHx_RCOUNT * 16) / fREFx。
	//0x0000-0x00FF：预留；
	//当驱动电流越大，需要的转换时间越长
	
	//使用外部晶振，当fREFx=40MHz时：
//	UserPara[CAP_RCOUNT_CH0].Value = 0x2FDA;    //示波器实测5ms
//	UserPara[CAP_RCOUNT_CH0].Value = 0x6B6C;    //示波器实测11ms
//	UserPara[CAP_RCOUNT_CH0].Value = 0xCD14;    //示波器实测21ms
//	UserPara[CAP_RCOUNT_CH0].Value = 0xFDE8;    //示波器实测26ms
	
	//使用外部晶振，当fREFx=20MHz时：
//	UserPara[CAP_RCOUNT_CH0].Value = 0x35B6;    //示波器实测11ms
//	UserPara[CAP_RCOUNT_CH0].Value = 0x668A;    //示波器实测21ms
	
	//使用外部晶振，单通道，当fREFx=5MHz时：
//	UserPara[CAP_RCOUNT_CH0].Value = 0x061B;    //计算结果5.0024ms，示波器实测6ms
//	UserPara[CAP_RCOUNT_CH0].Value = 0x0753;    //计算结果6.0008ms，示波器实测6ms
//	UserPara[CAP_RCOUNT_CH0].Value = 0x0D6E;    //计算结果11.0024ms，示波器实测11ms
//	UserPara[CAP_RCOUNT_CH0].Value = 0x1388;    //计算结果16.0008ms，示波器实测16ms
//	UserPara[CAP_RCOUNT_CH0].Value = 0x19A3;    //计算结果21.0024ms，示波器实测21ms。示波器抓到了累积误差。
//	UserPara[CAP_RCOUNT_CH0].Value = 0x1FBD;    //计算结果26.0008ms，示波器实测26ms
//	UserPara[CAP_RCOUNT_CH0].Value = 0x25D8;    //计算结果31.0024ms，示波器实测31ms
//	UserPara[CAP_RCOUNT_CH0].Value = 0x2BF2;    //计算结果36.0008ms，示波器实测36ms
//	UserPara[CAP_RCOUNT_CH0].Value = 0x320D;    //计算结果41.0024ms，示波器实测41ms
//	UserPara[CAP_RCOUNT_CH0].Value = 0x3827;    //计算结果46.0008ms，示波器实测46ms
//	UserPara[CAP_RCOUNT_CH0].Value = 0x3E42;    //计算结果51.0024ms，示波器实测51ms
//	UserPara[CAP_RCOUNT_CH0].Value = 0x445C;    //计算结果56.0008ms，示波器实测56ms
	UserPara[CAP_RCOUNT_CH0].Value = 0x4A77;    //计算结果61.0024ms，示波器实测61ms。保守取值。
	
	//使用外部晶振，当fREFx=4MHz时：
//	UserPara[CAP_RCOUNT_CH0].Value = 0x1482;    //示波器实测21ms
	
	//使用外部晶振，当fREFx=2MHz时：
//	UserPara[CAP_RCOUNT_CH0].Value = 0x0A41;    //示波器实测21ms
	
	//使用外部晶振，当fREFx=1MHz时：
//	UserPara[CAP_RCOUNT_CH0].Value = 0x0520;    //示波器实测21ms
	
	
	
	
	
	//通道0 偏移：Channel 0 Conversion Offset. fOFFSET0 = CH0_OFFSET * fREF0 / 65536。
	UserPara[CAP_OFFSET_CH0].Value = 0x0000;
	
	
	
	
	
	//（1）通道0 沉降参考计数设定一个时间，等待LC传感器稳定。也称为通道传感器激活时间。示波器测量LC激活至稳定，时间不超过10us
	//（2）必须满足：CHx_SETTLECOUNT > Vpk × fREFx × C × π2 / (32 × IDRIVEX)
	//（3）设置值为0、1时，Settle Time (tS0)= 32 ÷ fREF0。
	//（4）设置值为2-65535时，Settle Time(tS0)= (CH0_SETTLECOUNTˣ16) / fREF0。
	//（5）假设谐振信号振幅为 Vpk=1.68 V(pk)，fREFx=40MHz，电容（包括寄生电容）C=53pF，驱动电流IDRIVEX=0.146mA
	//则：1.68 * 40*1000000 * 53/1000 000 000 000 * 3.14 * 3.14 / (32 * 146/1000000) = 7.5
	//四舍五入为8。为了提供冗余以保证系统公差，选择更高的值10。
	//tS0=10 * 16 / 40 000 000 = 4us
//	UserPara[CAP_SETTLECOUNT_CH0].Value = 0x1388;
	UserPara[CAP_SETTLECOUNT_CH0].Value = 0x2710;
//	UserPara[CAP_SETTLECOUNT_CH0].Value = 0xFFFF;
//	UserPara[CAP_SETTLECOUNT_CH0].Value = 200;
	
	
	
	
	
	//通道0 时钟分频设置。
	//bit15-14：预留；
	//bit13-12：设置通道传感器分频值CHx_FIN_SEL，使得fINx=fSENSORx/CHx_FIN_SEL。
	//LC谐振频率范围在不同范围，应选择不同分频系数，使得fINx与fREFx满足约束关系：fINx < fREFx/4。
	//对于差分配置，推荐设置：b01：0.01MHz~8.75MHz，选择1分频；b10：5MHz~10MHz，选择2分频。
	//对于单端配置，推荐设置：b10：0.01MHz~10MHz，选择2分频。
	//bit11-10：预留；
	//bit9-0：频率测量参考时钟，fREFx = fCLK / CHx_FREF_DIVIDER，使得fINx与fREFx满足约束关系：fINx < fREFx/4
	
	//（1）时钟配置要求：
	//（a）多通道+内部晶振，fREFx ≤ 55MHz；多通道+外部晶振，fREFx ≤ 40MHz；单通道+内部/外部晶振，fREFx ≤ 35.
	//（b）fINx与fREFx有约束关系：fINx < fREFx/4。
	//（2）内部晶振频率35~55MHz，典型值是43.4MHz。外部晶振频率40MHz。
	//（3）以官方评估板为例，使用18μH电感和33pF电容，组成谐振LC，引脚，走线和导线电容占20pF，所以总电容为53pF。
	//33pF谐振电容，LC谐振频率为：fSENSOR = 1 /2π√（LC）= 1 /2π√（18 * 10-6 * 53 * 10-12）= 5.152831MHz。
	//18pF谐振电容，LC谐振频率为：fSENSOR = 1 /2π√（LC）= 1 /2π√（18 * 10-6 * 38 * 10-12）= 6.085441MHz。
	//这代表LC传感器最大频率，当传感器电容增加时，频率会降低。
	
	//由于传感器处于单端配置，因此推荐设置CH0_FIN_SEL为b10，即2分频。
	//33pF谐振电容，fINx = 5.152MHz / 2 = 2.576MHz。约束关系：fREFx > 10.304MHz
	//18pF谐振电容，fINx = 6.085MHz / 2 = 3.042MHz。约束关系：fREFx > 12.170MHz
	//推荐fREFx选择20MHz、40MHz。
	//备注：按照推荐设置，探测液面性能反而不理想，因此可不严格按照推荐配置。
	
//	UserPara[CAP_CLOCK_DIVIDERS_CH0].Value = 0x2001;    //40MHz       //性能一般，灵敏度最低，抗干扰较好。
//	UserPara[CAP_CLOCK_DIVIDERS_CH0].Value = 0x2002;    //20MHz       //综合较好，灵敏度较高，抗干扰较好。
//	UserPara[CAP_CLOCK_DIVIDERS_CH0].Value = 0x2003;    //13.33MHz
//	UserPara[CAP_CLOCK_DIVIDERS_CH0].Value = 0x2004;    //10MHz
//	UserPara[CAP_CLOCK_DIVIDERS_CH0].Value = 0x2005;    //8MHz
//	UserPara[CAP_CLOCK_DIVIDERS_CH0].Value = 0x2006;    //6.66MHz
//	UserPara[CAP_CLOCK_DIVIDERS_CH0].Value = 0x2007;    //5.714MHz
	UserPara[CAP_CLOCK_DIVIDERS_CH0].Value = 0x2008;    //5MHz        //保守取值，灵敏度最高，抗干扰一般。
//	UserPara[CAP_CLOCK_DIVIDERS_CH0].Value = 0x200A;    //4MHz
//	UserPara[CAP_CLOCK_DIVIDERS_CH0].Value = 0x2014;    //2MHz
//	UserPara[CAP_CLOCK_DIVIDERS_CH0].Value = 0x2028;    //1MHz
	
	//fINx = 5.15MHz.
	//fREFx = 20MHz
//	UserPara[CAP_CLOCK_DIVIDERS_CH0].Value = 0x1002;    //20MHz       //比0x2002好。
	
	
	
	
	
	//状态配置/故障配置
	//bit15-14：预留。
	//bit13：看门狗超时，，是否在DATA_CHx寄存器对应位置位。0：不需要；1：需要。
	//bit12：幅值过高报警，是否在DATA_CHx寄存器对应位置位。0：不需要；1：需要。
	//bit11：幅值过低报警，是否在DATA_CHx寄存器对应位置位。0：不需要；1：需要。
	//bit10-6：预留。
	//bit5：看门狗超时，是否需要INTB引脚输出信号、状态寄存器对应位置置位。0：不需要；1：需要。
	//bit4-1：预留。
	//bit0：数据准备好之后，是否需要INTB引脚输出信号、状态寄存器对应的位置，0：关闭；1：开启。
	UserPara[CAP_STATUS_CONFIG].Value = 0x0001;
	
	
	
	
	
	//配置
	//bit15-14：启动电容通道。b00：通道0；b01：通道1；b10：通道2.(仅FDC2114、FDC2214有效) b11：通道3.(仅FDC2114、FDC2214有效) 
	//bit13：睡眠模式使能。0：设备激活；1：设备睡眠。
	//bit12：预留。要求设置为1.
	//bit11：激活传感器时，传感器初始化。0：以最大电流快速启动；1：根据编程设置的参数来启动。
	//bit10：预留。要求设置为1.
	//bit9：选择时钟，0：内部时钟；1：外部时钟。
	//bit8：预留。
	//bit7：INTB引脚输出失能。当status寄存器更新时，INTB引脚是否置位。b0：置位；b1：不置位。
	//bit6：电容通道驱动电流。b0：设置所有通道的驱动电流为正常模式（最大1.5mA）；b1：设置通道0驱动电流>1.5mA（不支持多通道扫描模式）。
	//bit5-0：预留，复位后b000001。
//	UserPara[CAP_CONFIG].Value = 0x1501;    //启用通道0；设备激活，内部晶振。
//	UserPara[CAP_CONFIG].Value = 0x1601;    //启用通道0；设备激活，外部晶振。
//	UserPara[CAP_CONFIG].Value = 0x5501;    //启用通道1；设备激活，内部晶振。
	UserPara[CAP_CONFIG].Value = 0x5601;    //启用通道1；设备激活，外部晶振。
	
	
	
	
	//信道复用配置
	//bit15：自动扫描模式使能，0：连续转换由CONFIG.ACTIVE_CHAN选中的通道；1：自动扫描由MUX_CONFIG.RR_SEQUENCE选中的通道。
	//bit14-13：配置自动扫描的通道。b00：通道0、1；b01：通道0、1、2.(仅FDC2114、FDC2214有效) b10：通道0、1、2、3.(仅FDC2114、FDC2214有效) b10：通道0、1。	
	//bit12-3：预留。复位值00 0100 0001。
	//bit2-0：设置带宽，b001：1MHz；b100:3.3MHz；b101：10MHz；b111：33MHz。输入抗尖峰脉冲滤波器带宽设置为10MHz，这是超出振荡振荡器频率的最低设置。
//	UserPara[CAP_MUX_CONFIG].Value = 0x0209;
//	UserPara[CAP_MUX_CONFIG].Value = 0x020C;
	UserPara[CAP_MUX_CONFIG].Value = 0x020D;    //保守值
//	UserPara[CAP_MUX_CONFIG].Value = 0x020F;
	
	//多通道，自动扫描通道0、1
//	UserPara[CAP_MUX_CONFIG].Value = 0x820D;
	
	
	
	
	
	//通道0 传感器驱动电流配置。
	//驱动电流越大，抗干扰能力越强，灵敏度越低。
	//虽然加大驱动电流会提高抗干扰性能，但对外干扰会增大，最终效果并不好。
	//驱动电流越大，谐振频率越低。
	//数据手册要求：1.2V ≤sensor oscillation amplitude (pk) ≤ 1.8V
	//bit15-11：多个电流选项。
	//bit10-0：预留
	UserPara[CAP_DRIVE_CURRENT_CH0].Value = FDC2214_Drive_Current_0_016;
	
	
	
	
	
	//默认不开启传感器
	UserPara[LLD_SEN_FUN].Value = LLD_CLOSE;
	
	
	//当8孔液面高度不在同一水平面时，非常容易出问题。
	//（1）设置较小阈值，有利于识别液面，但容易出现误停。
	//（2）设置较大阈值，有利抵抗误停干扰，但容易导致识别不到液面，特别是相邻孔位液面高度较高，会削弱当前移液器数据增量，导致识别不到液面。
	//探测液面默认阈值。
	con_liq_threshold = 50000;
	leave_liq_threshold = 30000;
	UserPara[CAP_CON_THRESHOLD_H].Value = con_liq_threshold>>16;
	UserPara[CAP_CON_THRESHOLD_L].Value = con_liq_threshold;
	UserPara[CAP_LEV_THRESHOLD_H].Value = leave_liq_threshold>>16;
	UserPara[CAP_LEV_THRESHOLD_L].Value = leave_liq_threshold;
	
	
	//电容探测档位、转换时间
	UserPara[CAP_SET_GEAR].Value = 0;
	UserPara[CAP_SET_CONTIME].Value = 61;
	
	
	
	
	
	CapSenPara.RCOUNT_CH0 = UserPara[CAP_RCOUNT_CH0].Value;
	CapSenPara.OFFSET_CH0 = UserPara[CAP_OFFSET_CH0].Value;
	CapSenPara.SETTLECOUNT_CH0 = UserPara[CAP_SETTLECOUNT_CH0].Value;
	CapSenPara.CLOCK_DIVIDERS_CH0 = UserPara[CAP_CLOCK_DIVIDERS_CH0].Value;
	CapSenPara.STATUS_CONFIG = UserPara[CAP_STATUS_CONFIG].Value;
	CapSenPara.CONFIG = UserPara[CAP_CONFIG].Value;
	CapSenPara.MUX_CONFIG = UserPara[CAP_MUX_CONFIG].Value;
	CapSenPara.DRIVE_CURRENT_CH0 = UserPara[CAP_DRIVE_CURRENT_CH0].Value<<11;
	CapSenPara.ContactThreshold = UserPara[CAP_CON_THRESHOLD_H].Value<<16;
	CapSenPara.ContactThreshold = CapSenPara.ContactThreshold | UserPara[CAP_CON_THRESHOLD_L].Value;
	CapSenPara.LeaveThreshold = UserPara[CAP_LEV_THRESHOLD_H].Value<<16;
	CapSenPara.LeaveThreshold = CapSenPara.LeaveThreshold | UserPara[CAP_LEV_THRESHOLD_L].Value;
	CapSenPara.ConTime = UserPara[CAP_SET_CONTIME].Value;
	
	
	
	
	//开窗数量；电容数据连续2次超过阈值，判断识到别液面。
	CapSenPara.SlideWindNum = CLLD_SLIDEWIND_NUM6;
	CapSenPara.SlidePassSingle = 2;
	CapSenPara.SlidePassAll = 2;
	
	//对接应用数据缓存
	CapSenPara.ChColl[CLLD_CH0_DATA].OrigData = CapSenPara.AppOrigBuff;
	CapSenPara.ChColl[CLLD_CH0_DATA].FilterData = CapSenPara.AppFilterBuff;
	
	//是否复位设备
	CapSenPara.ResStatu = CP_COMM_NORMAL;
	CapSenPara.ResWaitTime = 6;
	
	//默认开启睡眠
	CapSenPara.SleepOpen = FALSE;
	CapSenPara.SleepStatu = FALSE;
	CapSenPara.ChlNeedSwitch = FALSE;
	CapSenPara.ChlNmu = 1;
	
	//设置通信地址
	FDC2212SetAddr(IO_LOW);
	
	//FDC2212的SD（关闭/开启输入信号）
	FDC2212ShutDownInput(IO_LOW);
}

















/*
 * @function: AirSenPressState
 * @details : 探测液面曲线状态。
 * @input   : 1、state：状态。
              2、sig：计数值。
 * @output  : NULL
 * @return  : 脉冲类型
 */
uint8_t AirSenPressState1(uint8_t *state, uint8_t sig)
{
	uint8_t    ret = 0;
	
	
	if(sig >= AIRPRESS_SUP_LONG_PASS_NUM)
	{
		*state = 5;
	}
	else if(sig >= AIRPRESS_LONG_PASS_NUM)
	{
		*state = 4;
	}
	else if(sig >= AIRPRESS_MID_PASS_NUM)
	{
		*state = 3;
	}
	else if(sig >= AIRPRESS_SHORT_PASS_NUM)
	{
		*state = 2;
	}
	else if(sig >= AIRPRESS_SHORTET_PASS_NUM)
	{
		*state = 1;
	}
	else
	{
		*state = 0;
	}
	
	
	return (ret);
}


/*
 * @function: AirSenPressState2
 * @details : 探测液面曲线状态。
 * @input   : 1、state：状态。
              2、sig：计数值。
 * @output  : NULL
 * @return  : 脉冲类型
 */
uint8_t AirSenPressState2(uint8_t *state, uint8_t sig, uint8_t passline)
{
	uint8_t    ret = 0;
	
	
	if(sig >= passline)
	{
		Accumulation8(state);
	}
	else
	{
		*state = 0;
	}
	
	
	return (ret);
}


/*
 * @function: AirSenCheckAll
 * @details : 探测液面总判断。
 * @input   : 1、air_sen：传感器参数见。
              2、is_check：是否检查气泡。
 * @output  : NULL
 * @return  : 脉冲类型
 */
uint8_t AirSenCheckAll(struct tagAirSenPara* air_sen, uint8_t is_check)
{
	int16_t     temp = 0;
	
	
	switch(air_sen->AirPress.Stage)
	{
		case PLLD_STAGE_NULL:
		{
			if((PLLD_LIQUL == is_check)
				|| (PLLD_BUBBLE == is_check))
			{
				//吸头吸到物质，曲线高于参考大气压
				if(air_sen->AbsState5_1 >= 1)
				{
					air_sen->AirPress.Stage = PLLD_STAGE_TOUCH;
				}
				
				//吸头吸到物质，曲线低于大气压
				if(air_sen->AbsState5_2 >= 1)
				{
					air_sen->AirPress.Stage = PLLD_STAGE_TOUCH;
				}
			}
			else if((is_check >= PLLD_ABS_BLOCK)
					&& (is_check <= PLLD_ABS_BLOCK_FIXED_FOLLOW))
			{
//				//检测到P电机启动吸液、吸气（有滤芯吸头，P电机器启动时，吸气的气压与吸小容量液体的气压几乎一模一样）
//				if(air_sen->AbsState8_1 >= 1)
//				{
//					air_sen->AbsState5_1 = 0;
//					air_sen->AbsState8_1 = 0;
//					air_sen->AbsState8_2 = 0;
//					air_sen->AbsState8_3 = 0;
//					air_sen->AbsState8_4 = 0;
//					air_sen->AbsState8_5 = 0;
//					air_sen->AbsState8_Timer = 0;
//					air_sen->AbsAirFollowState1 = 0;
//					air_sen->AbsAirFollowState2 = 0;
//					//检测到P电机开始吸液，向前移动（AIRPRESS_SHORTET_PASS_NUM + 3）单位，取气压作为吸液前参考气压
//					air_sen->ABS_StartAirPress = air_sen->ChColl[PLLD_CH0_DATA].FilterData[AIRPRESS_SHORTET_PASS_NUM + 3];
//					
//					
//					air_sen->AirPress.Stage = PLLD_STAGE_START;
//				}
				
				
				
				//P电机真实启动
				if(PLLD_ABS_START == air_sen->ABS_Real_Start_End)
				{
					air_sen->AbsState5_1 = 0;
					air_sen->AbsState8_1 = 0;
					air_sen->AbsState8_2 = 0;
					air_sen->AbsState8_3 = 0;
					air_sen->AbsState8_4 = 0;
					air_sen->AbsState8_5 = 0;
					air_sen->AbsState8_Timer = 0;
					air_sen->AbsState8_Timer1 = 0;
					air_sen->AbsState8_Timer2 = 0;
					air_sen->AbsAirFollowState1 = 0;
					air_sen->AbsAirFollowState2 = 0;
					//检测到P电机开始吸液，向前移动（AIRPRESS_SHORTET_PASS_NUM + 3）单位，取气压作为吸液前参考气压
					air_sen->ABS_StartAirPress = air_sen->ChColl[PLLD_CH0_DATA].FilterData[AIRPRESS_SHORTET_PASS_NUM + 3];
					
					
					air_sen->AirPress.Stage = PLLD_STAGE_START;
				}
				
				
				
//				//等待超时
//				Accumulation8(&air_sen->AbsState8_Timer);
//				if(air_sen->AbsState8_Timer >= air_sen->ABS_StartWait)
//				{
//					//检测到P电机开始吸液，向前移动（AIRPRESS_SHORTET_PASS_NUM + 3）单位，取气压作为吸液前参考气压
//					air_sen->ABS_StartAirPress = air_sen->ChColl[PLLD_CH0_DATA].FilterData[AIRPRESS_SHORTET_PASS_NUM + 3];
//					
//					
//					air_sen->AirPress.Stage = PLLD_STAGE_AIR_ALWAYS;
//				}
			}
		}
		break;
		
		case PLLD_STAGE_START:
		{
			//等待吸液泵启动吸液稳定
			Accumulation8(&air_sen->AbsState8_Timer);
			if(air_sen->AbsState8_Timer >= air_sen->ABS_StartDly)
			{
				air_sen->AbsState5_1 = 0;
				air_sen->AbsState8_1 = 0;
				air_sen->AbsState8_2 = 0;
				air_sen->AbsState8_3 = 0;
				air_sen->AbsState8_4 = 0;
				air_sen->AbsState8_5 = 0;
				air_sen->AbsState8_Timer = 0;
				air_sen->AbsState8_Timer1 = 0;
				air_sen->AbsState8_Timer2 = 0;
				air_sen->AbsAirFollowState1 = 0;
				air_sen->AbsAirFollowState2 = 0;
				
				
				air_sen->AirPress.Stage = PLLD_STAGE_TOUCH;
			}
		}
		break;
		
		case PLLD_STAGE_TOUCH:
		{
			//曲线高于阈值
			if(PLLD_LIQUL == is_check)
			{
				if(air_sen->AbsState5_1 >= 1)
				{
					air_sen->AirPress.Stage = PLLD_STAGE_LIQ;
				}
				
				if(air_sen->AbsState5_2 >= 1)
				{
					air_sen->AirPress.Stage = PLLD_STAGE_LIQ;
				}
			}
			
			
			
			//曲线上升，吸头可能吸到气泡或者液面。
			if(PLLD_BUBBLE == is_check)
			{
				if(air_sen->AbsState5_1 >= 2)
				{
					air_sen->AirPress.Stage = PLLD_STAGE_BUBBLE;
				}
			}
			
			
			
			//吸液完毕，判断吸液是否存在固定点吸空
			if((PLLD_ABS_FIXED_AIR == is_check)
				|| (PLLD_ABS_BLOCK_FIXED_AIR == is_check)
				|| (PLLD_ABS_BLOCK_FIXED_FOLLOW == is_check))
			{
				if(PLLD_ABS_END == air_sen->ABS_Real_Start_End)
				{
					//检测全程吸空。不推荐使用绝对变化量，传感器工作时间长，零点偏移。
//					temp = air_sen->ChColl[PLLD_CH0_DATA].FilterData[0] - AIR_SEN_ZERO_PRESS;
//					if(temp < air_sen->RakeRatio3Min)
//					{
//						air_sen->AirPress.Stage = PLLD_STAGE_AIR_ALWAYS;
//					}
					
					//使用相对变化量
					temp = air_sen->ChColl[PLLD_CH0_DATA].FilterData[0] - air_sen->ABS_StartAirPress;
					if(temp < air_sen->RakeRatio3Min)
					{
						air_sen->AirPress.Stage = PLLD_STAGE_AIR_ALWAYS;
					}
				}
			}
			
			
			
			//检测追随吸空/部分行程吸空
			if((PLLD_ABS_FIXED_AIR == is_check)
				|| (PLLD_ABS_BLOCK_FIXED_AIR == is_check)
				|| (PLLD_ABS_FOLLOW_AIR == is_check)
				|| (PLLD_ABS_BLOCK_FOLLOW_AIR == is_check)
				|| (PLLD_ABS_BLOCK_FIXED_FOLLOW == is_check))
//			if((PLLD_ABS_FOLLOW_AIR == is_check)
//				|| (PLLD_ABS_BLOCK_FOLLOW_AIR == is_check)
//				|| (PLLD_ABS_BLOCK_FIXED_FOLLOW == is_check))
			{
				//检测气压剧烈跳动变小，有4种情况：
				//（1）P电机启动吸液。
				//（2）P电机吸液中，吸头从空气进入液体，即部分吸空。
				//（3）追随吸液时吸空，气压先变小后变大。检测吸气是否上升。
				//（4）吸液时被异物短暂堵塞，然后被吸进吸头，气压短暂变小。
				if(air_sen->AbsState8_4 >= 1)
				{
					air_sen->AbsAirFollowState1 = 1;
				}
				if(air_sen->AbsAirFollowState1 >= 1)
				{
					Accumulation8(&air_sen->AbsState8_Timer1);
				}
				//P电机加速，可能误触发瞬间空吸
				if(air_sen->AbsAirFollowState1 >= 1)
				{
					//收到P电机吸液完毕信息，查看是否有误判间断吸空问题
					if(PLLD_ABS_END == air_sen->ABS_Start_End)
					{
						if(air_sen->AbsState8_Timer1 >= air_sen->ABS_EndDly)
						{
							air_sen->AbsState8_Timer1 = 0;
						
						
							air_sen->AirPress.Stage = PLLD_STAGE_AIR_MOMENT;
						}
						else
						{
							air_sen->AbsAirFollowState1 = 0;
						}
					}
				}
				
				
				
				//检测气压剧烈跳动下降，有2种情况：
				//（1）吸液即将结束，P电机减速。
				//（2）追随吸液时吸空，气压先变小后变大。检测吸气是否下降。
				if(air_sen->AbsState8_5 >= 1)
				{
					air_sen->AbsAirFollowState2 = 1;
				}
				if(air_sen->AbsAirFollowState2 >= 1)
				{
					Accumulation8(&air_sen->AbsState8_Timer2);
				}
				
				
				//P电机减速，可能误触发瞬间空吸
				if(air_sen->AbsAirFollowState2 >= 1)
				{
					//收到P电机吸液完毕信息，查看是否有误判间断吸空问题
					if(PLLD_ABS_END == air_sen->ABS_Real_Start_End)
					{
						if(air_sen->AbsState8_Timer2 > air_sen->ABS_EndDly)
						{
							air_sen->AbsState8_Timer2 = 0;
						
						
							air_sen->AirPress.Stage = PLLD_STAGE_AIR_MOMENT;
						}
						else
						{
							air_sen->AbsAirFollowState2 = 0;
						}
					}
				}
			}
			
			
			
			//检测凝块
			//检测凝块和固定点吸空
			if((PLLD_ABS_BLOCK == is_check)
				|| (PLLD_ABS_BLOCK_FIXED_AIR == is_check)
				|| (PLLD_ABS_BLOCK_FOLLOW_AIR == is_check)
				|| (PLLD_ABS_BLOCK_FIXED_FOLLOW == is_check))
			{
				if(PLLD_ABS_START == air_sen->ABS_Real_Start_End)
				{
					//吸液过程中，查看气压值是否超过吸液堵塞阈值
					if(air_sen->ChColl[PLLD_CH0_DATA].FilterData[0] >= AIR_SEN_ERR_PRESS)
					{
						air_sen->AirPress.Stage = PLLD_STAGE_ABS_BLOCK;
					}
				}
				else if(PLLD_ABS_END == air_sen->ABS_Real_Start_End)
				{
					//吸液完毕，查看气压值是否超过吸液堵塞阈值
					if(air_sen->AbsState8_3 >= 1)
					{
						air_sen->AirPress.Stage = PLLD_STAGE_ABS_BLOCK;
					}
				}
			}
			
			
			
		}
		break;
		
		case PLLD_STAGE_BUBBLE:
		{
//			//大斜率
//			if((3 == air_sen->AbsState6)
//				|| (2 == air_sen->AbsState6))
//			{
//				air_sen->AirPress.Stage = PLLD_STAGE_LIQ;
//			}
			
			//小斜率
			//(1)失败
//			if((air_sen->AbsState2 <= 2)
//				&& (air_sen->AbsState6 <= 2)
//				&& (air_sen->AbsState7 >= 2))
//			{
//				air_sen->AirPress.Stage = PLLD_STAGE_LIQ;
//			}
			
			//(2)效果不佳
//			if((air_sen->AbsState1_1 >= 4)
//				&& (air_sen->AbsState2_1 <= 2)
//				&& (air_sen->AbsState6 <= 1)
//				&& (air_sen->AbsState7_1 >= 3))
//			{
//				air_sen->AirPress.Stage = PLLD_STAGE_LIQ;
//			}
			
			//(3)效果不佳
//			if((air_sen->AbsState1_1 >= 4)
//				&& (air_sen->AbsState1_2 <= 2)
//				
//				&& (air_sen->AbsState2_1 >= 2)
//				
//				&& (air_sen->AbsState6 <= 1)
//				&& (air_sen->AbsState7_1 >= 3))
//			{
//				air_sen->AirPress.Stage = PLLD_STAGE_LIQ;
//			}
			
			//(4)测试中
			if((air_sen->AbsState6_1 <= 3)
				&& (air_sen->AbsState7_1 >= 2))
			{
				air_sen->AirPress.Stage = PLLD_STAGE_LIQ;
			}
			
			
			//探测耗时过长
		}
		break;
		
		case PLLD_STAGE_ABS_BLOCK:
		{
		}
		break;
		
		case PLLD_STAGE_AIR_MOMENT:
		{
		}
		break;
		
		case PLLD_STAGE_AIR_ALWAYS:
		{
		}
		break;
		
		case PLLD_STAGE_ABS_BLOCK_AIR:
		{
		}
		break;
		
		case PLLD_STAGE_LIQ:
		{
		}
		break;
		
		default:
		{
			air_sen->AirPress.Stage = PLLD_STAGE_NULL;
		}
		break;
	}
	
	
	return (air_sen->AirPress.Stage);
}


/*
 * @function: AirSenDeteLiqLevel
 * @details : 气压探测液面
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void AirSenDeteLiqLevel(void)
{
	int8_t     i = 0;
	uint8_t    set_comp_num1 = 0;        //设置比较次数
//	uint8_t    set_comp_num2 = 0;        //设置比较次数
//	uint8_t    set_pass_single = 0;      //设置单次通过
//	uint8_t    set_pass_all = 0;         //设置全部通过
	
	int16_t    curve_rake_ratio = 0;     //曲线斜率
	
	int16_t    temp = 0;
	int16_t    sub1 = 0;
	int16_t    sub2 = 0;
	uint8_t    ret = 0;
	
	
	
	
	//开窗数据个数，不能超过数据长度。
	set_comp_num1 = AirSenPara.SlideWindNum - 1;
//	set_comp_num2 = (AirSenPara.SlideWindNum / 2);
//	set_pass_single = AirSenPara.SlidePassSingle;
//	set_pass_all = AirSenPara.SlidePassAll;
	
	
	
	//吸液结束，应答通信查询探测结果
	if(TRUE == AirSenPara.CommAckBlock)
	{
		Accumulation8(&AirSenPara.CommAckBlockDly);
	}
	
	//等待一段时间后再应答
	if(AirSenPara.CommAckBlockDly >= 3)
	{
		AirSenPara.CommAckBlockDly = 0;
		AirSenPara.CommAckBlock = FALSE;
	}
	
	
	//各个状态下的执行动作
	switch(AirSenPara.ABS_Real_Start_End)
	{
		case PLLD_ABS_IDLE:
		{
		}
		break;
		
		case PLLD_ABS_END:
		{
		}
		break;
		
		case PLLD_ABS_START:
		{
			ret = TMC5160_FIELD_READ(TMC_0, TMC5160_RAMPSTAT, TMC5160_RAMPSTAT_POS_REACH_MASK, TMC5160_RAMPSTAT_POS_REACH_SHIFT);
			//LOG_Info("RAMP_STAT=%X", TMC_ReadInt(eTMC, TMC5160_RAMPSTAT));
			
			//检查电机是否真实结束
			if(0x01 == ret)
			{
				AirSenPara.ABS_Real_Start_End = PLLD_ABS_END;
			}
		}
		break;
		
		default:
		{	
		}
		break;
	}
	
	
	
	//各个状态下的执行动作
	switch(AirSenPara.LLDStage)
	{
		case LLD_STAGE_NULL:
		{
			//计算大气压值
			AirSenPara.Atmos = 0;
			for(i = 0; i < set_comp_num1; i++)
			{
				AirSenPara.Atmos = AirSenPara.Atmos + AirSenPara.ChColl[PLLD_CH0_DATA].FilterData[i];
			}
			AirSenPara.AtmosAve = AirSenPara.Atmos / set_comp_num1;
			
			
			
			//开启气压探测
			if(LLD_CLOSE != UserPara[LLD_SEN_FUN].Value)
			{
				AirSenPara.AirPress.Stage = PLLD_STAGE_NULL;
				AirSenPara.AirPress.AspState = 0;
				
				AirSenPara.CommAckBlock = FALSE;
				AirSenPara.ABS_Start_End = PLLD_ABS_START;
				AirSenPara.LLDResult = LLD_IDLE;
				
				AirSenPara.AbsState8_Timer = 0;
				AirSenPara.AbsState8_Timer1 = 0;
				AirSenPara.AbsState8_Timer2 = 0;
				
				
				AirSenPara.LLDStage = LLD_STAGE_START;
			}
		}
		break;
		
		case LLD_STAGE_START:
		{
			//计算吸气后大气压值
			AirSenPara.AtmosAbs = 0;
			for(i = 0; i < PLLD_NUM; i++)
			{
				sub1 = AirSenPara.ChColl[PLLD_CH0_DATA].FilterData[i];
				sub2 = AirSenPara.AtmosAve;
				temp = sub1 - sub2;
				if((temp > AirSenPara.AtmosNoise) || (temp < -AirSenPara.AtmosNoise))
				{
					AirSenPara.AtmosAbs = AirSenPara.AtmosAbs + AirSenPara.ChColl[PLLD_CH0_DATA].FilterData[i];
				}
			}
			AirSenPara.AtmosAbsAve = AirSenPara.AtmosAbs / PLLD_NUM;
			
			
			
			//探测液面
			if((PLLD_LIQUL == UserPara[LLD_SEN_FUN].Value)
				|| (PLLD_BUBBLE == UserPara[LLD_SEN_FUN].Value))
			{
				sub1 = AirSenPara.AtmosAbsAve;
				sub2 = AirSenPara.AtmosAve;
				temp = sub1 - sub2;
				if((temp > AirSenPara.AtmosNoise) || (temp < -AirSenPara.AtmosNoise))
				{
					AirSenPara.LLDStage = LLD_STAGE_CONTACT;
				}
			}
			
			
			//检测凝块、空吸，直接往下走，无需等待
			if((UserPara[LLD_SEN_FUN].Value >= PLLD_ABS_BLOCK)
				&& (UserPara[LLD_SEN_FUN].Value <= PLLD_ABS_BLOCK_FIXED_FOLLOW))
			{
				//直接给平均气压值，然后往下走
				AirSenPara.AtmosAbsAve = AirSenPara.AtmosAve;
				
				
				AirSenPara.LLDStage = LLD_STAGE_CONTACT;
			}
			
			
			
			
			//开启气压探测，才需要往下走
			//这动作十分重要，否则上电后会误判
			if(LLD_CLOSE == UserPara[LLD_SEN_FUN].Value)
			{
				AirSenPara.CommAckBlock = FALSE;
				AirSenPara.ABS_Start_End = PLLD_ABS_IDLE;
				AirSenPara.ABS_Real_Start_End = PLLD_ABS_IDLE;
				AirSenPara.LLDResult = LLD_IDLE;
				
				
				AirSenPara.LLDStage = LLD_STAGE_NULL;
			}
		}
		break;
		
		case LLD_STAGE_CONTACT:
		{
			AirSenPara.sig1_cnt_1 = 0;
			AirSenPara.sig1_cnt_2 = 0;
			AirSenPara.sig2_cnt_1 = 0;
			AirSenPara.sig2_cnt_2 = 0;
			AirSenPara.sig3_cnt = 0;
			AirSenPara.sig4_cnt = 0;
			AirSenPara.sig5_cnt_1 = 0;
			AirSenPara.sig5_cnt_2 = 0;
			AirSenPara.sig6_cnt_1 = 0;
			AirSenPara.sig6_cnt_2 = 0;
			AirSenPara.sig7_cnt_1 = 0;
			AirSenPara.sig7_cnt_2 = 0;
			AirSenPara.sig8_cnt_1 = 0;
			AirSenPara.sig8_cnt_2 = 0;
			AirSenPara.sig8_cnt_3 = 0;
			AirSenPara.sig8_cnt_4 = 0;
			AirSenPara.sig8_cnt_5 = 0;
			
			
			
			AirSenPara.AtmosNoise = UserPara[AIR_ATMOS_NOISE].Value;
			
			AirSenPara.RakeRatio1Max = UserPara[AIR_RAKERATIO1_MAX].Value;
			AirSenPara.RakeRatio1Min = UserPara[AIR_RAKERATIO1_MIN].Value;
			
			AirSenPara.RakeRatio2Max = UserPara[AIR_RAKERATIO2_MAX].Value;
			AirSenPara.RakeRatio2Min = UserPara[AIR_RAKERATIO2_MIN].Value;
			
			
			AirSenPara.AspLiqNoise = UserPara[AIR_ASP_LIQ_NOISE].Value;
			AirSenPara.RakeRatio3Max = UserPara[AIR_RAKERATIO3_MAX].Value;
			AirSenPara.RakeRatio3Min = UserPara[AIR_RAKERATIO3_MIN].Value;
			
			AirSenPara.ABS_StartWait = UserPara[AIR_ABS_WAIT_DLY].Value;
			AirSenPara.ABS_StartDly = UserPara[AIR_ABS_START_DLY].Value;
			AirSenPara.ABS_EndDly = UserPara[AIR_ABS_END_DLY].Value;
			
			
			
			//1、检查曲线状态
			for(i = set_comp_num1; i > 0; i--)
			{
				//曲线上升或下降（查看开窗数据波动情况；用途不大，考虑删除掉。）
//				sub1 = AirSenPara.ChColl[PLLD_CH0_DATA].FilterData[i - 1];
//				sub2 = AirSenPara.ChColl[PLLD_CH0_DATA].FilterData[i];
//				curve_rake_ratio = sub1 - sub2;
//				if(curve_rake_ratio >= AIRPRESS_SUP_SHORT_PASS_NUM)
//				{
//					Accumulation8(&AirSenPara.sig1_cnt_1);
//				}
//				else if(curve_rake_ratio <= -AIRPRESS_SUP_SHORT_PASS_NUM)
//				{
//					Accumulation8(&AirSenPara.sig1_cnt_2);
//				}
				
				
//				//曲线高于或低于大气压（用途不大，考虑删除掉）
//				sub1 = AirSenPara.ChColl[PLLD_CH0_DATA].FilterData[i - 1];
//				sub2 = AirSenPara.AtmosAbsAve;
//				curve_rake_ratio = sub1 - sub2;
//				if(curve_rake_ratio > AirSenPara.AbsLiqSetVal)
//				{
//					Accumulation8(&AirSenPara.sig3_cnt);
//				}
//				else if(curve_rake_ratio < -AirSenPara.AbsLiqSetVal)
//				{
//					Accumulation8(&AirSenPara.sig4_cnt);
//				}
				
				
				//曲线相对变化量与阈值
				sub1 = AirSenPara.ChColl[PLLD_CH0_DATA].FilterData[0];
				sub2 = AirSenPara.ChColl[PLLD_CH0_DATA].FilterData[i];
				curve_rake_ratio = sub1 - sub2;
				if(curve_rake_ratio > AirSenPara.RakeRatio1Max)
				{
					Accumulation8(&AirSenPara.sig5_cnt_1);
				}
				else if(curve_rake_ratio < -AirSenPara.RakeRatio1Max)
				{
					Accumulation8(&AirSenPara.sig5_cnt_2);
				}
				
				
				//曲线大斜率（只能检测快速上升或下降信号）
				sub1 = AirSenPara.ChColl[PLLD_CH0_DATA].FilterData[i - 1];
				sub2 = AirSenPara.ChColl[PLLD_CH0_DATA].FilterData[i];
				curve_rake_ratio = sub1 - sub2;
				if(curve_rake_ratio > AirSenPara.RakeRatio1Min)
				{
					Accumulation8(&AirSenPara.sig6_cnt_1);
				}
				else if(curve_rake_ratio < -AirSenPara.RakeRatio1Min)
				{
					Accumulation8(&AirSenPara.sig6_cnt_2);
				}
			}
			
			for(i = set_comp_num1; i >= 5; i--)
			{
				//曲线快速上升或下降（只能检测较短快速变化信号）
				sub1 = AirSenPara.ChColl[PLLD_CH0_DATA].FilterData[i - 5];
				sub2 = AirSenPara.ChColl[PLLD_CH0_DATA].FilterData[i];
				curve_rake_ratio = sub1 - sub2;
				
				
				
				//吸气噪声
				if(curve_rake_ratio >= AirSenPara.AtmosNoise)
				{
					//开始吸气
					Accumulation8(&AirSenPara.sig8_cnt_1);
				}
				else
				{
					AirSenPara.sig8_cnt_1 = 0;
				}
				
				
				
				if(AirSenPara.RakeRatio3Max >= AIR_SEN_ZERO_PRESS)
				{
					//气压大于特定值，使用气压阈值判断凝块
					temp = AirSenPara.ChColl[PLLD_CH0_DATA].OrigValue;
				}
				else
				{
					//使用斜率判断
					temp = curve_rake_ratio;
				}
				if(temp >= AirSenPara.RakeRatio3Max)
				{
					//凝块
					Accumulation8(&AirSenPara.sig8_cnt_3);
				}
				else
				{
					AirSenPara.sig8_cnt_3 = 0;
				}
				
				
				
				if(curve_rake_ratio > AirSenPara.AspLiqNoise)
				{
					//开启吸液，短暂吸空到真正吸液
					Accumulation8(&AirSenPara.sig8_cnt_4);
				}
				else
				{
					AirSenPara.sig8_cnt_4 = 0;
				}
				if(curve_rake_ratio < -AirSenPara.AspLiqNoise)
				{
					//吸液过程吸空
					Accumulation8(&AirSenPara.sig8_cnt_5);
				}
				else
				{
					AirSenPara.sig8_cnt_5 = 0;
				}
			}
			
			for(i = set_comp_num1; i >= 10; i--)
			{
				//曲线小斜率（只能检测慢速上升信号）
				sub1 = AirSenPara.ChColl[PLLD_CH0_DATA].FilterData[i - 10];
				sub2 = AirSenPara.ChColl[PLLD_CH0_DATA].FilterData[i];
				curve_rake_ratio = sub1 - sub2;
				if((curve_rake_ratio >= AirSenPara.RakeRatio2Min)
					&& (curve_rake_ratio <= AirSenPara.RakeRatio2Max))
				{
					Accumulation8(&AirSenPara.sig7_cnt_1);
				}
			}
			
			
//			//曲线不断上升
//			AirSenPressState1(&AirSenPara.AbsState1_1, AirSenPara.sig1_cnt_1);
			
			
//			//曲线不断下降
//			AirSenPressState1(&AirSenPara.AbsState1_2, AirSenPara.sig1_cnt_2);
			
			
			//曲线快速上升或下降
//			AirSenPressState1(&AirSenPara.AbsState2_1, AirSenPara.sig2_cnt_1);
//			AirSenPressState1(&AirSenPara.AbsState2_2, AirSenPara.sig2_cnt_2);
			
			
//			//曲线高于大气压
//			AirSenPressState1(&AirSenPara.AbsState3, AirSenPara.sig3_cnt);
			
			
//			//曲线低于大气压
//			AirSenPressState1(&AirSenPara.AbsState4, AirSenPara.sig4_cnt);
			
			
			//曲线大于吸液阈值
			AirSenPressState2(&AirSenPara.AbsState5_1, AirSenPara.sig5_cnt_1, AIRPRESS_SUP_SHORT_PASS_NUM);
			AirSenPressState2(&AirSenPara.AbsState5_2, AirSenPara.sig5_cnt_2, AIRPRESS_SUP_SHORT_PASS_NUM);
			
			
			//大斜率
			AirSenPressState1(&AirSenPara.AbsState6_1, AirSenPara.sig6_cnt_1);
			AirSenPressState1(&AirSenPara.AbsState6_2, AirSenPara.sig6_cnt_2);
			
			//小斜率
			AirSenPressState1(&AirSenPara.AbsState7_1, AirSenPara.sig7_cnt_1);
			AirSenPressState1(&AirSenPara.AbsState7_2, AirSenPara.sig7_cnt_2);
			
			
			//启动吸气/吸液、启动吸液、凝块、间断空吸、全程空吸、吸头吸到液体。
			AirSenPressState2(&AirSenPara.AbsState8_1, AirSenPara.sig8_cnt_1, AIRPRESS_SHORTET_PASS_NUM);
//			AirSenPressState2(&AirSenPara.AbsState8_2, AirSenPara.sig8_cnt_2, AIRPRESS_SHORTET_PASS_NUM);
			AirSenPressState2(&AirSenPara.AbsState8_3, AirSenPara.sig8_cnt_3, AIRPRESS_SHORTET_PASS_NUM);
			AirSenPressState2(&AirSenPara.AbsState8_4, AirSenPara.sig8_cnt_4, AIRPRESS_SHORTET_PASS_NUM);
			AirSenPressState2(&AirSenPara.AbsState8_5, AirSenPara.sig8_cnt_5, AIRPRESS_SHORTET_PASS_NUM);
			
			
			
			//汇总
			AirSenCheckAll(&AirSenPara, UserPara[LLD_SEN_FUN].Value);
			
			
			
			
			
			if(PLLD_STAGE_ABS_BLOCK == AirSenPara.AirPress.Stage)
			{
				AirSenPara.LLDResult = LLD_ABS_BLOCK;
				
				
				AirSenPara.LLDStage = LLD_STAGE_ABS_BLOCK;
			}
			else if(PLLD_STAGE_AIR_MOMENT == AirSenPara.AirPress.Stage)
			{
				AirSenPara.LLDResult = LLD_STAGE_ABS_AIR_MOMENT;
				
				
				AirSenPara.LLDStage = LLD_STAGE_ABS_AIR;
			}
			else if(PLLD_STAGE_AIR_ALWAYS == AirSenPara.AirPress.Stage)
			{
				AirSenPara.LLDResult = LLD_ABS_AIR;
				
				
				AirSenPara.LLDStage = LLD_STAGE_ABS_AIR;
			}
			else if(PLLD_STAGE_ABS_BLOCK_AIR == AirSenPara.AirPress.Stage)
			{
				AirSenPara.LLDResult = LLD_ABS_BLOCK_AIR;
				
				
				AirSenPara.LLDStage = LLD_STAGE_ABS_BLOCK;
			}
			else if(PLLD_STAGE_LIQ == AirSenPara.AirPress.Stage)
			{
				AirSenPara.LLDResult = LLD_LIQUL;
				
				
				AirSenPara.LLDStage = LLD_STAGE_LIQ;
			}
			
			
			
			
			//开启气压探测，才需要往下走
			//这动作十分重要，否则上电后会误判
			if(LLD_CLOSE == UserPara[LLD_SEN_FUN].Value)
			{
				AirSenPara.CommAckBlock = FALSE;
				AirSenPara.ABS_Start_End = PLLD_ABS_IDLE;
				AirSenPara.ABS_Real_Start_End = PLLD_ABS_IDLE;
				AirSenPara.LLDResult = LLD_IDLE;
				
				
				AirSenPara.LLDStage = LLD_STAGE_NULL;
			}
		}
		break;
		
		case LLD_STAGE_LIQ:
		{
			if(LLD_CLOSE == UserPara[LLD_SEN_FUN].Value)
			{
				AirSenPara.CommAckBlock = FALSE;
				AirSenPara.LLDResult = LLD_IDLE;
				
				
				AirSenPara.LLDStage = LLD_STAGE_NULL;
			}
		}
		break;
		
		case LLD_STAGE_FAIL:
		{
			AirSenPara.LLDStage = LLD_STAGE_NULL;
		}
		break;
		
		case LLD_STAGE_BUBBLE:
		{
			AirSenPara.LLDStage = LLD_STAGE_NULL;
		}
		break;
		
		case LLD_STAGE_ABS_BLOCK:
		{
			if(LLD_CLOSE == UserPara[LLD_SEN_FUN].Value)
			{
				AirSenPara.ABS_Start_End = PLLD_ABS_IDLE;
				AirSenPara.ABS_Real_Start_End = PLLD_ABS_IDLE;
				AirSenPara.LLDResult = LLD_IDLE;
				
				
				AirSenPara.LLDStage = LLD_STAGE_NULL;
			}
		}
		break;
		
		case LLD_STAGE_ABS_AIR:
		{
			if(LLD_CLOSE == UserPara[LLD_SEN_FUN].Value)
			{
				AirSenPara.ABS_Start_End = PLLD_ABS_IDLE;
				AirSenPara.ABS_Real_Start_End = PLLD_ABS_IDLE;
				AirSenPara.LLDResult = LLD_IDLE;
				
				
				AirSenPara.LLDStage = LLD_STAGE_NULL;
			}
		}
		break;
		
		case LLD_STAGE_ABS_AIR_MOMENT:
		{
			if(LLD_CLOSE == UserPara[LLD_SEN_FUN].Value)
			{
				AirSenPara.ABS_Start_End = PLLD_ABS_IDLE;
				AirSenPara.ABS_Real_Start_End = PLLD_ABS_IDLE;
				AirSenPara.LLDResult = LLD_IDLE;
				
				
				AirSenPara.LLDStage = LLD_STAGE_NULL;
			}
		}
		break;
		
		default:
		{
			AirSenPara.LLDStage = LLD_STAGE_NULL;
		}
		break;
	}
}





/*
 * @function: AirSenConfig气压传感器
 * @details : 在气压芯片工作期间，再次发送查询设备指令，气压芯片强占IIC总线，奇怪。
 * @input   : NULL
 * @output  : NULL
 * @return  : 结果，1：成功；其他：失败。
 */
uint8_t AirSenConfig(void)
{
	uint8_t     result = 0;
	
	
	//硬件IIC接口
	result = I2C_CheckDevice(I2C2, AIR_SEN_DEV_ADDR, I2C_Direction_Transmitter);
	
	//模拟IIC接口
//	result = i2c_CheckDevice(&GPIOIIC[IIC_2], AIR_SEN_DEV_ADDR);
//	if(0 == result)
//	{
//		result = 1;
//	}
//	else
//	{
//		result = 0;
//	
	
	
	return (result);
}

/*
 * @function: AirSenReadPressADC
 * @details : 读检测值
 * @input   : NULL
 * @output  : NULL
 * @return  : 结果，1：成功；其他：失败。
 */
uint8_t AirSenReadPressADC(void)
{
	uint8_t     result = 0;
	
	
	__disable_irq();
	//硬件IIC接口（气压传感器一旦通信失败，难以恢复。暂时不用硬件通信）
	result = I2C_Mem_Read2(I2C2, AIR_SEN_DEV_ADDR, AirSenPara.Buff, AIR_SEN_REG_INVALID_ADDR, 2);
	__enable_irq(); 
	
	
	return (result);
}

/*
 * @function: AirSenConvertAD
 * @details : AD滤波
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
#ifdef PLLD_DEBUG
#define  AIR_DIS_NUM    150
int16_t  AirDataDis[AIR_DIS_NUM] = {0};
int16_t  air_offset = 8192;
#endif
uint8_t AirSenConvertAD(void)
{
	uint8_t     result = 0;
	uint8_t     status = 0;
	uint16_t    press_data = 0;
//	uint16_t    temp_data = 0;
//	int32_t     temp_calc = 0;
	int16_t     press_dis = 0;
	
	
	
	//气压值
	press_data = MERGE_UINT16(AirSenPara.Buff[0], AirSenPara.Buff[1]);
	
	status = press_data>>14;
	status = status & 0x03;
	if(0 == status)
	{
		press_data = press_data & 0x3FFF;
		
		AirSenPara.ChColl[PLLD_CH0_DATA].IsErr = FALSE;
		AirSenPara.ChColl[PLLD_CH0_DATA].OrigValue = press_data;
		
		
		//一阶滤波
//		press_data = LPF1(press_data, AirSenPara.ChColl[PLLD_CH0_DATA].FilterValue, 0.3);
//		AirSenPara.ChColl[PLLD_CH0_DATA].FilterValue = press_data;
		SmoothPipeline32(AirSenPara.ChColl[PLLD_CH0_DATA].FilterData, press_data, PLLD_NUM);


#ifdef PLLD_DEBUG
		//获取启动气压检测信号
		if(LLD_CLOSE == UserPara[LLD_SEN_FUN].Value)
		{
			air_offset = 8142;
		}
		else
		{
			air_offset = 8192;
		}
		press_dis = press_data - air_offset;
		SmoothPipeline16(AirDataDis, press_dis, AIR_DIS_NUM);
#endif	
		
		
//		//计算压力值，放大100倍。压力单位是inH2O
//		press_data = AirSenPara.ChColl[PLLD_CH0_DATA].OrigValue - 8192;
//		press_data = 125 * 20 * press_data;
//		press_data = calc_temp / 16384;
//		
//		//将压力转为Pa。
//		AirSenPara.ChColl[PLLD_CH0_DATA].Value = press_data * 249;
		
		
		
		
		//计算温度
//		temp_calc = temp_data>>5;
//		temp_calc = temp_calc * 200 / 2047;
//		AirSenPara.ChColl[PLLD_CH1_DATA].OrigValue = temp_calc - 50;
	}
	else if(2 == status)
	{
		//数据未更新
		
	}
	else if(3 == status)
	{
		//故障，供电和配置错误
		AirSenPara.ChColl[PLLD_CH0_DATA].IsErr = TRUE;
	}
	
	
	return (result);
}


/*
 * @function: AirSensor
 * @details : 各个状态下执行的动作
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void AirSensor(void)
{
	uint8_t    iic_ack = IIC_ACR_NORMAL;
	
	
	//各个状态下执行的动作
	switch(AirSenPara.ComStage)
	{
		case CP_COMM_RESET_DEV:
		{
			//Dev1是重要设备，有通信故障后，iic总线重置。
			//因此等待Dev1复位完成后再往下运行。
			if(FALSE == GPIOIIC[IIC_2].Dev1Err)
			{
				AirSenPara.ComStage = CP_COMM_INIT;
			}
		}
		break;
		
		case CP_COMM_INIT:
		{
			//Dev1是重要设备，有通信故障后，iic总线重置。
			//因此等待Dev1复位完成后再往下运行。
			if(FALSE == GPIOIIC[IIC_2].Dev1Err)
			{
				rt_enter_critical();    //进入临界段。有改善效果，但仍然偶发故障，概率约0.5%
				iic_ack = AirSenConfig();
				rt_exit_critical();     //退出临界段
				
				
				AirSenPara.ComStage = CP_COMM_NORMAL;
			}
		}
		break;
		
		case CP_COMM_NORMAL:
		{
			if(IIC_BUS_NORMAL == GPIOIIC[IIC_2].State)
			{
				rt_enter_critical();    //进入临界段
				iic_ack = AirSenReadPressADC();
				rt_exit_critical();     //退出临界段
				
				if(IIC_ACR_NORMAL != iic_ack)
				{
					GPIOIIC[IIC_2].Dev2Err = TRUE;
					GPIOIIC[IIC_2].Dev2CommReset = TRUE;
				}
				else
				{
					GPIOIIC[IIC_2].Dev2Err = FALSE;
				}
				
				
				AirSenConvertAD();
				AirSenDeteLiqLevel();
			}
			
			
			
			
			if(TRUE == GPIOIIC[IIC_2].Dev2Err)
			{
				//IIC2总线被强占，需要释放总线，该动作在别处完成。
				AirSenPara.ComStage = CP_COMM_RELEASE_IIC;
			}
		}
		break;
		
		case CP_COMM_RELEASE_IIC:
		{
			AirSenPara.ComStage = CP_COMM_RESET_IIC;
		}
		break;
		
		case CP_COMM_RESET_IIC:
		{
			AirSenPara.ComStage = CP_COMM_RESET_DEV;
		}
		break;
		
		default:
		{
		}
		break;
	}
}


/*
 * @function: AirSenInit
 * @details : 初始化
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void AirSenInit(void)
{
	//大气压噪声（吸气速度越慢，噪声越小）
	UserPara[AIR_ATMOS_NOISE].Value = 6;
	
	//吸液大斜率，探测液面
	UserPara[AIR_RAKERATIO1_MAX].Value = 70;
	UserPara[AIR_RAKERATIO1_MIN].Value = 16;
	
	//吸液小斜率，探测气泡
	UserPara[AIR_RAKERATIO2_MAX].Value = 14;
	UserPara[AIR_RAKERATIO2_MIN].Value = 2;
	
	//吸液中断空吸、吸到凝块阈值/吸头堵塞、全程空吸
	UserPara[AIR_ASP_LIQ_NOISE].Value = 260;
	UserPara[AIR_RAKERATIO3_MAX].Value = 190;
	UserPara[AIR_RAKERATIO3_MIN].Value = 50;
	
	
	//气压检测吸空，等待一段时间仍然没检测到吸液动作
	UserPara[AIR_ABS_WAIT_DLY].Value = 70;
	//P电机启动吸液时，气压跳动剧烈，延时一会
	UserPara[AIR_ABS_START_DLY].Value = 20;
	//P电机结束吸液时，气压跳动剧烈，延时一会
	UserPara[AIR_ABS_END_DLY].Value = 20;
	
	
	
	AirSenPara.AtmosNoise = UserPara[AIR_ATMOS_NOISE].Value;
	
	AirSenPara.RakeRatio1Max = UserPara[AIR_RAKERATIO1_MAX].Value;
	AirSenPara.RakeRatio1Min = UserPara[AIR_RAKERATIO1_MIN].Value;
	
	AirSenPara.RakeRatio2Max = UserPara[AIR_RAKERATIO2_MAX].Value;
	AirSenPara.RakeRatio2Min = UserPara[AIR_RAKERATIO2_MIN].Value;
	
	AirSenPara.AspLiqNoise = UserPara[AIR_ASP_LIQ_NOISE].Value;
	AirSenPara.RakeRatio3Min = UserPara[AIR_RAKERATIO3_MIN].Value;
	AirSenPara.RakeRatio3Max = UserPara[AIR_RAKERATIO3_MAX].Value;
	
	AirSenPara.ABS_StartWait = UserPara[AIR_ABS_WAIT_DLY].Value;
	AirSenPara.ABS_StartDly = UserPara[AIR_ABS_START_DLY].Value;
	AirSenPara.ABS_EndDly = UserPara[AIR_ABS_END_DLY].Value;
	
	
	
	
	//开窗数量；气压数据连续n超过阈值，判断识到别液面。
	AirSenPara.SlideWindNum = PLLD_NUM;
	AirSenPara.SlidePassSingle = PLLD_NUM / 2;
	AirSenPara.SlidePassAll = PLLD_NUM / 4;
	
	//对接应用数据缓存
	AirSenPara.ChColl[PLLD_CH0_DATA].OrigData = AirSenPara.AppOrigBuff;
	AirSenPara.ChColl[PLLD_CH0_DATA].FilterData = AirSenPara.AppFilterBuff;
}








/*
 * @function: WriteCapSenReg
 * @details : 上位机配置电容传感器寄存器参数
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @output  : NULL
 * @return  : 0：成功；1：参数错误。
 */
uint8_t WriteCapSenReg(uint8_t *buf, uint8_t num)
{
	uint8_t    ret = FALSE;
	
	
	return (ret);
}

/*
 * @function: ReadCapSenReg
 * @details : 上位机读电容传感器寄存器
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @return  : NULL
 */
uint8_t ReadCapSenReg(uint8_t *buf, uint8_t num)
{
	uint8_t    ret = 0;
	
	
	return (ret);
}


/*
 * @function: WriteAirSenReg
 * @details : 上位机配置气压传感器寄存器参数
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @return  : 0：成功；1：参数错误。
 */
uint8_t WriteAirSenReg(uint8_t *buf, uint8_t num)
{
	uint8_t    ret = FALSE;
	
	
	return (ret);
}

/*
 * @function: ReadAirSenReg
 * @details : 上位机读气压传感器寄存器
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @return  : NULL
 */
uint8_t ReadAirSenReg(uint8_t *buf, uint8_t num)
{
	uint8_t    ret = 0;
	
	
	return (ret);
}


/*
 * @function: WriteCLLDPara1
 * @details : 上位机配置电容液面探测参数
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @return  : 0：成功；1：参数错误。
 */
uint8_t WriteCLLDPara1(uint8_t *buf, uint8_t num)
{
	uint8_t     ret = FALSE;
	uint16_t    w_buf[4] = {0};
	
	
	w_buf[0] = buf[3];
	ret = SetPara16(&UserPara[LLD_SEN_FUN], w_buf[0]);
	
	
	return (ret);
}

/*
 * @function: ReadCLLDPara1
 * @details : 上位机读电容液面探测参数
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @return  : NULL
 */
uint8_t ReadCLLDPara1(uint8_t *buf, uint8_t num)
{
	uint8_t    ret = 0;
	
	
	buf[3] = UserPara[LLD_SEN_FUN].Value;
	
	
	return (ret);
}


/*
 * @function: WriteCLLDPara3
 * @details : 上位机设置电容探测参数
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @return  : 0：成功；1：参数错误。
 */
uint8_t WriteCLLDPara3(uint8_t *buf, uint8_t num)
{
	uint8_t     ret = FALSE;
	uint8_t     result1 = FALSE;
	uint8_t     result2 = FALSE;
	uint8_t     result3 = FALSE;
//	uint16_t    temp = 0;
	uint16_t    w_buf[4] = {0};
	
	
	w_buf[0] = buf[3];
	w_buf[1] = buf[4];
	w_buf[2] = buf[5];
	w_buf[3] = MERGE_UINT16(buf[6], buf[7]);
	
	
	result1 = SetPara16(&UserPara[CAP_SET_CONTIME], w_buf[0]);
	if(TRUE == result1)
	{
		ret = TRUE;
	}
	
	result2 = SetPara16(&UserPara[CAP_DRIVE_CURRENT_CH0], w_buf[1]);
	if(TRUE == result2)
	{
		ret = TRUE;
	}
	
	result3 = SetPara16(&UserPara[CAP_CON_THRESHOLD_H], w_buf[2]);
	if(TRUE == result3)
	{
		ret = TRUE;
	}
	
	result3 = SetPara16(&UserPara[CAP_CON_THRESHOLD_L], w_buf[3]);
	if(TRUE == result3)
	{
		ret = TRUE;
	}
	
	
	
//	if(FALSE == result1)
//	{
//		if(CapSenPara.ConTime != UserPara[CAP_SET_CONTIME].Value)
//		{
//			//需要复位设备
//			CapSenPara.ResStatu = CP_COMM_RESET_DEV;
//		}
//	}
//	
//	if(FALSE == result2)
//	{
//		temp = UserPara[CAP_DRIVE_CURRENT_CH0].Value<<11;
//		if(CapSenPara.DRIVE_CURRENT_CH0 != temp)
//		{
//			//需要复位设备
//			CapSenPara.ResStatu = CP_COMM_RESET_DEV;
//		}
//	}
	
	if(FALSE == result3)
	{
		//参数范围不做检查
		CapSenPara.ContactThreshold = UserPara[CAP_CON_THRESHOLD_H].Value<<16;
		CapSenPara.ContactThreshold = CapSenPara.ContactThreshold | UserPara[CAP_CON_THRESHOLD_L].Value;
		CapSenPara.LeaveThreshold = CapSenPara.ContactThreshold;
	}
	
	
	return (ret);
}

/*
 * @function: ReadCLLDPara3
 * @details : 上位机读电容探测参数
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @return  : NULL
 */
uint8_t ReadCLLDPara3(uint8_t *buf, uint8_t num)
{
	uint8_t    ret = 0;
	
	
	buf[3] = UserPara[CAP_SET_CONTIME].Value;
	buf[4] = UserPara[CAP_DRIVE_CURRENT_CH0].Value;
	buf[5] = UserPara[CAP_CON_THRESHOLD_H].Value;
	buf[6] = UserPara[CAP_CON_THRESHOLD_L].Value>>8;
	buf[7] = UserPara[CAP_CON_THRESHOLD_L].Value;
	
	
	return (ret);
}


/*
 * @function: WriteCapMapVal
 * @details : 上位机写电容映射
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @return  : 0：成功；1：参数错误。
 */
uint8_t WriteCapMapVal(uint8_t *buf, uint8_t num)
{
	uint8_t    ret = FALSE;
	
	
	
	return (ret);
}

/*
 * @function: ReadCapMapVal
 * @details : 上位机读电容映射值
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @return  : NULL
 */
uint8_t ReadCapMapVal(uint8_t *buf, uint8_t num)
{
	uint8_t    ret = 0;
	uint32_t   temp = 0;
	
	
	if(TRUE == GPIOIIC[IIC_2].Dev1Err)
	{
		buf[2] = ERR_DEV1_FAILD;
	}
	temp = CapSenPara.ChColl[CLLD_CH0_DATA].FilterData[0];
	
	buf[4] = temp>>24;
	buf[5] = temp>>16;
	buf[6] = temp>>8;
	buf[7] = temp;
	
	
	return (ret);
}


/*
 * @function: WriteAirLLDPara1
 * @details : 上位机写气压探测参数1
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @return  : 0：成功；1：参数错误。
 */
uint8_t WriteAirLLDPara1(uint8_t *buf, uint8_t num)
{
	uint8_t     ret = FALSE;
	uint8_t     index = 0xFF;
	uint16_t    w_buf[4] = {0};
	
	
	w_buf[3] = MERGE_UINT16(buf[6], buf[7]);
	
	if(0 == num)
	{
		index = AIR_ATMOS_NOISE;
	}
	else if(1 == num)
	{
		index = AIR_ASP_LIQ_NOISE;
	}
	else if(2 == num)
	{
		index = AIR_RAKERATIO1_MAX;
	}
	else if(3 == num)
	{
		index = AIR_RAKERATIO1_MIN;
	}
	else if(4 == num)
	{
		index = AIR_RAKERATIO2_MAX;
	}
	else if(5 == num)
	{
		index = AIR_RAKERATIO2_MIN;
	}
	else if(6 == num)
	{
		index = AIR_RAKERATIO3_MAX;
	}
	else if(7 == num)
	{
		index = AIR_RAKERATIO3_MIN;
	}
	else if(8 == num)
	{
		index = AIR_ABS_WAIT_DLY;
	}
	else if(9 == num)
	{
		index = AIR_ABS_START_DLY;
	}
	else if(10 == num)
	{
		index = AIR_ABS_END_DLY;
	}
	
	if(index <= AIR_ABS_END_DLY)
	{
		ret = SetPara16(&UserPara[index], w_buf[3]);
	}
	
	
	return (ret);
}

/*
 * @function: ReadAirLLDPara1
 * @details : 上位机读气压探探测参数1
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @return  : 0：成功；1：参数错误。
 */
uint8_t ReadAirLLDPara1(uint8_t *buf, uint8_t num)
{
	uint8_t     ret = FALSE;
	uint8_t     index = 0;
	
	
	if(0 == num)
	{
		index = AIR_ATMOS_NOISE;
	}
	else if(1 == num)
	{
		index = AIR_ASP_LIQ_NOISE;
	}
	else if(2 == num)
	{
		index = AIR_RAKERATIO1_MAX;
	}
	else if(3 == num)
	{
		index = AIR_RAKERATIO1_MIN;
	}
	else if(4 == num)
	{
		index = AIR_RAKERATIO2_MAX;
	}
	else if(5 == num)
	{
		index = AIR_RAKERATIO2_MIN;
	}
	else if(6 == num)
	{
		index = AIR_RAKERATIO3_MAX;
	}
	else if(7 == num)
	{
		index = AIR_RAKERATIO3_MIN;
	}
	else if(8 == num)
	{
		index = AIR_ABS_WAIT_DLY;
	}
	else if(9 == num)
	{
		index = AIR_ABS_START_DLY;
	}
	else if(10 == num)
	{
		index = AIR_ABS_END_DLY;
	}
	buf[4] = UserPara[index].Value>>24;
	buf[5] = UserPara[index].Value>>16;
	buf[6] = UserPara[index].Value>>8;
	buf[7] = UserPara[index].Value;
	
	
	return (ret);
}


/*
 * @function: WriteAirLLDPara2
 * @details : 上位机写气压探测参数2
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @return  : 0：成功；1：参数错误。
 */
uint8_t WriteAirLLDPara2(uint8_t *buf, uint8_t num)
{
	uint8_t     ret = FALSE;
	uint8_t     index = 0xFF;
	uint16_t    w_buf[4] = {0};
	
	
	w_buf[3] = MERGE_UINT16(buf[6], buf[7]);
	
	if(0 == num)
	{
		index = AIR_ABS_START_END;
	}
	else if(1 == num)
	{
		index = AIR_DIS_START_END;
	}
	
	if(index <= AIR_DIS_START_END)
	{
		ret = SetPara16(&UserPara[index], w_buf[3]);
	}
	
	if(0 == num)
	{
		AirSenPara.ABS_Start_End = UserPara[index].Value;
		
		
		if(PLLD_ABS_END == AirSenPara.ABS_Start_End)
		{
			//阻塞应答
			AirSenPara.CommAckBlock = TRUE;
		}
	}
	else if(1 == num)
	{
		AirSenPara.Dis_Start_End = UserPara[index].Value;
	}
	
	
	
	return (ret);
}

/*
 * @function: ReadAirLLDPara2
 * @details : 上位机读气压探探测参数2
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @return  : 0：成功；1：参数错误。
 */
uint8_t ReadAirLLDPara2(uint8_t *buf, uint8_t num)
{
	uint8_t     ret = FALSE;
	uint8_t     index = 0;
	
	
	if(0 == num)
	{
		index = AIR_ABS_START_END;
	}
	else if(1 == num)
	{
		index = AIR_DIS_START_END;
	}
	
	buf[4] = UserPara[index].Value>>24;
	buf[5] = UserPara[index].Value>>16;
	buf[6] = UserPara[index].Value>>8;
	buf[7] = UserPara[index].Value;
	
	
	return (ret);
}


/*
 * @function: WriteAirLLDPara3
 * @details : 上位机写气压探测参数3
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @return  : 0：成功；1：参数错误。
 */
uint8_t WriteAirLLDPara3(uint8_t *buf, uint8_t num)
{
	uint8_t     ret = FALSE;
	uint8_t     result = FALSE;
	uint16_t    w_buf[4] = {0};
	
	
	w_buf[0] = MERGE_UINT16(buf[4], buf[5]);
	w_buf[1] = MERGE_UINT16(buf[6], buf[7]);
	
	result = SetPara16(&UserPara[AIR_RAKERATIO1_MAX], w_buf[0]);
	if(TRUE == result)
	{
		ret = TRUE;
	}
	
	result = SetPara16(&UserPara[AIR_RAKERATIO1_MIN], w_buf[1]);
	if(TRUE == result)
	{
		ret = TRUE;
	}
	
	if(FALSE == ret)
	{
		AirSenPara.RakeRatio1Max = UserPara[AIR_RAKERATIO1_MAX].Value;
		AirSenPara.RakeRatio1Min = UserPara[AIR_RAKERATIO1_MIN].Value;
	}
	
	
	return (ret);
}

/*
 * @function: ReadAirLLDPara3
 * @details : 上位机读气压探测参数3
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @return  : 0：成功；1：参数错误。
 */
uint8_t ReadAirLLDPara3(uint8_t *buf, uint8_t num)
{
	uint8_t    ret = 0;
	
	
	buf[4] = AirSenPara.RakeRatio1Max>>8;
	buf[5] = AirSenPara.RakeRatio1Max;
	
	buf[6] = AirSenPara.RakeRatio1Min>>8;
	buf[7] = AirSenPara.RakeRatio1Min;
	
	
	return (ret);
}


/*
 * @function: WriteAirMapVal
 * @details : 上位机写气压映射
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @return  : 0：成功；1：参数错误。
 */
uint8_t WriteAirMapVal(uint8_t *buf, uint8_t num)
{
	uint8_t    ret = FALSE;
	
	
	
	return (ret);
}

/*
 * @function: ReadAirMapVal
 * @details : 上位机读气压映射值
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @return  : NULL
 */
uint8_t ReadAirMapVal(uint8_t *buf, uint8_t num)
{
	uint8_t    ret = 0;
	
	
	if(TRUE == GPIOIIC[IIC_2].Dev2Err)
	{
		buf[2] = ERR_DEV2_FAILD;
	}
	buf[4] = AirSenPara.ChColl[PLLD_CH0_DATA].OrigValue>>24;
	buf[5] = AirSenPara.ChColl[PLLD_CH0_DATA].OrigValue>>16;
	buf[6] = AirSenPara.ChColl[PLLD_CH0_DATA].OrigValue>>8;
	buf[7] = AirSenPara.ChColl[PLLD_CH0_DATA].OrigValue;
	
	
	return (ret);
}


/*
 * @function: WriteAirLLDPara4
 * @details : 上位机写气压探测参数4
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @return  : 0：成功；1：参数错误。
 */
uint8_t WriteAirLLDPara4(uint8_t *buf, uint8_t num)
{
	uint8_t     ret = FALSE;
	uint8_t     result = FALSE;
	uint16_t    w_buf[4] = {0};
	
	
	w_buf[0] = MERGE_UINT16(buf[4], buf[5]);
	w_buf[1] = MERGE_UINT16(buf[6], buf[7]);
	
	result = SetPara16(&UserPara[AIR_RAKERATIO2_MAX], w_buf[0]);
	if(TRUE == result)
	{
		ret = TRUE;
	}
	
	result = SetPara16(&UserPara[AIR_RAKERATIO2_MIN], w_buf[1]);
	if(TRUE == result)
	{
		ret = TRUE;
	}
	
	if(FALSE == ret)
	{
		AirSenPara.RakeRatio2Max = UserPara[AIR_RAKERATIO2_MAX].Value;
		AirSenPara.RakeRatio2Min = UserPara[AIR_RAKERATIO2_MIN].Value;
	}
	
	
	return (ret);
}

/*
 * @function: ReadAirLLDPara4
 * @details : 上位机读气压探测参数4
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @return  : 0：成功；1：参数错误。
 */
uint8_t ReadAirLLDPara4(uint8_t *buf, uint8_t num)
{
	uint8_t    ret = 0;
	
	
	buf[4] = AirSenPara.RakeRatio2Max>>8;
	buf[5] = AirSenPara.RakeRatio2Max;
	
	buf[6] = AirSenPara.RakeRatio2Min>>8;
	buf[7] = AirSenPara.RakeRatio2Min;
	
	
	return (ret);
}


/*
 * @function: WriteAirLLDPara5
 * @details : 上位机写气压探测参数5
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @return  : 0：成功；1：参数错误。
 */
uint8_t WriteAirLLDPara5(uint8_t *buf, uint8_t num)
{
	uint8_t     ret = FALSE;
	uint8_t     result = FALSE;
	uint16_t    w_buf[4] = {0};
	
	
	w_buf[0] = MERGE_UINT16(buf[4], buf[5]);
	w_buf[1] = MERGE_UINT16(buf[6], buf[7]);
	
	result = SetPara16(&UserPara[AIR_RAKERATIO3_MAX], w_buf[0]);
	if(TRUE == result)
	{
		ret = TRUE;
	}
	
	result = SetPara16(&UserPara[AIR_RAKERATIO3_MIN], w_buf[1]);
	if(TRUE == result)
	{
		ret = TRUE;
	}
	
	if(FALSE == ret)
	{
		AirSenPara.RakeRatio3Max = UserPara[AIR_RAKERATIO3_MAX].Value;
		AirSenPara.RakeRatio3Min = UserPara[AIR_RAKERATIO3_MIN].Value;
	}
	
	
	return (ret);
}

/*
 * @function: ReadAirLLDPara5
 * @details : 上位机读气压探测参数5
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @return  : 0：成功；1：参数错误。
 */
uint8_t ReadAirLLDPara5(uint8_t *buf, uint8_t num)
{
	uint8_t    ret = 0;
	
	
	buf[4] = AirSenPara.RakeRatio3Max>>8;
	buf[5] = AirSenPara.RakeRatio3Max;
	
	buf[6] = AirSenPara.RakeRatio3Min>>8;
	buf[7] = AirSenPara.RakeRatio3Min;
	
	
	return (ret);
}





/*
 * @function: WriteUsePara
 * @details : 上位机写存储参数
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @return  : 0：成功；1：参数错误。
 */
uint8_t WriteUsePara(uint8_t *buf, uint8_t num)
{
	uint8_t     ret = FALSE;
	uint16_t    w_buf[4] = {0};


	w_buf[2] = MERGE_UINT16(buf[4], buf[5]);
	w_buf[3] = MERGE_UINT16(buf[6], buf[7]);
	
	
	if(num < USER_PARA_NUM)
	{
		ret = 0;
	}
	else
	{
		//有错误
		ret = 1;
	}
	
	
	//更新给应用变量
	if(0 == ret)
	{
		UserPara[num].Value = w_buf[3];
		
//		if(LLD_CARRIER == num)
//		{
//		}
//		else if(LLD_SEN_FUN == num)
//		{
//		}		
	}
	
	
	return (ret);
}

/*
 * @function: ReadUsePara
 * @details : 上位机读存储参数
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @return  : 0：成功；1：参数错误。
 */
uint8_t ReadUsePara(uint8_t *buf, uint8_t num)
{
	uint8_t    ret = FALSE;
	
	
	if(num < USER_PARA_NUM)
	{
		ret = 0;
	}
	else
	{
		ret = 1;
	}
	
	//无错误
	if(0 == ret)
	{
		buf[0] = UserPara[num].Value;
	}
	
	
	return (ret);
}



/*
 * @function: WriteSeleSen
 * @details : 上位机启用传感器
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @return  : 0：成功；1：参数错误。
 */
uint8_t WriteSeleSen(uint8_t *buf, uint8_t num)
{
	uint8_t    ret = FALSE;
	uint16_t   w_buf[4] = {0};
	
	
	w_buf[3] = buf[7];
	
	//启用传感器
	ret = SetPara16(&UserPara[LLD_SEN_FUN], w_buf[3]);
	
	
	return (ret);
}

/*
 * @function: ReadSelectSen
 * @details : 上位机读传感器
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @return  : NULL
 */
uint8_t ReadSelectSen(uint8_t *buf, uint8_t num)
{
	uint8_t    ret = ERR_CAN_NULL;
	
	
	if(TRUE == GPIOIIC[IIC_2].Dev1Err)
	{
		buf[2] = ERR_DEV1_FAILD;
	}
	else if(TRUE == GPIOIIC[IIC_2].Dev2Err)
	{
		buf[2] = ERR_DEV2_FAILD;
	}
	
	buf[7] = UserPara[LLD_SEN_FUN].Value;
	
	
	return (ret);
}


/*
 * @function: ReadLLDResult
 * @details : 上位机读液面探测结果
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @return  : NULL
 */
uint8_t ReadLLDResult(uint8_t *buf, uint8_t num)
{
	uint8_t    ret = ERR_CAN_NULL;
	
	
	buf[7] = LLDMan.Result;
	
	
	return (ret);
}


/*
 * @function: LLDReslut
 * @details : 液面探测结果
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void LLDReslut(void)
{
	uint8_t    lld_output = IO_LOW;
	
	
	switch(UserPara[LLD_SEN_FUN].Value)
	{
		case LLD_CLOSE:
		{
			LLDMan.Result = LLD_IDLE;
		}
		break;
		
		case CLLD_LIQUL:
		case CLLD_BUBBLE:
		case CLLD_ABS_BLOCK:
		case CLLD_ABS_AIR:
		case CLLD_ABS_BLOCK_FIXED_AIR:
		case CLLD_DIS_BLOCK:
		case CLLD_ABS_FOLLOW:
		case CLLD_TWO_DETE:
		{
			LLDMan.Result = CapSenPara.LLDResult;
		}
		break;
		
		case PLLD_LIQUL:
		case PLLD_BUBBLE:
		case PLLD_ABS_BLOCK:
		case PLLD_ABS_FIXED_AIR:
		case PLLD_ABS_BLOCK_FIXED_AIR:
		case PLLD_DIS_BLOCK:
		case PLLD_ABS_FOLLOW:
		case PLLD_TWO_DETE:
		case PLLD_ABS_FOLLOW_AIR:
		case PLLD_ABS_BLOCK_FOLLOW_AIR:
		case PLLD_ABS_BLOCK_FIXED_FOLLOW:
		{
			LLDMan.Result = AirSenPara.LLDResult;
		}
		break;
		
		case LLD_CAP_AIR_LIQUL:
		{
			if((LLD_LIQUL == CapSenPara.LLDResult)
				&& (LLD_LIQUL == AirSenPara.LLDResult))
			{
				LLDMan.Result = LLD_LIQUL;
			}
			else
			{
				LLDMan.Result = LLD_IDLE;
			}
		}
		break;
		
		default:
		{
			UserPara[LLD_SEN_FUN].Value = LLD_CLOSE;
		}
		break;
	}
	
	

	if(LLD_CLOSE != UserPara[LLD_SEN_FUN].Value)
	{
		//输出脉冲信号
		if(LLD_IDLE != LLDMan.Result)
		{
			lld_output = IO_HIGH;
		}
		else
		{
			lld_output = IO_LOW;
		}
	}
	else
	{
		//收到关闭液面探测的指令，不能输出脉冲信号
		lld_output = IO_LOW;
	}
	LLD_OutPutPulse(lld_output);
}


/*
 * @function: LLDInit
 * @details : 液面探测初始化
 * @input   : NULL
 * @output  : NULL
 * @return  : NULL
 */
void LLDInit(void)
{
	CapSenInit();
	AirSenInit();
	
	
	UserPara[LLD_SEN_FUN].Value = LLD_CLOSE;
	
	LLDMan.SenType = UserPara[LLD_SEN_FUN].Value;
	LLDMan.Result = LLD_IDLE;
}


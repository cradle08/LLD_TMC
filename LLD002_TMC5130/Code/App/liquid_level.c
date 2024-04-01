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
//	if(0 == CapSenPara.ChlNmu)
//	{
		result = I2C_Mem_Write(I2C2, CAP_DEV_ADDR, &temp_dat[0], (CAP_RCOUNT_CH0_REG + 0), 2);
//	}
//	else
//	{
		result = I2C_Mem_Write(I2C2, CAP_DEV_ADDR, &temp_dat[0], (CAP_RCOUNT_CH0_REG + 1), 2);
//	}
	
	//通道0、通道1的偏移值
//	temp_dat[0] = CapSenPara.OFFSET_CH0>>8;
//	temp_dat[1] = CapSenPara.OFFSET_CH0;
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
//	if(0 == CapSenPara.ChlNmu)
//	{
		result = I2C_Mem_Write(I2C2, CAP_DEV_ADDR, &temp_dat[0], (CAP_SETTLECOUNT_CH0_REG + 0), 2);
//	}
//	else
//	{
		result = I2C_Mem_Write(I2C2, CAP_DEV_ADDR, &temp_dat[0], (CAP_SETTLECOUNT_CH0_REG + 1), 2);
//	}
	
	//通道0、通道1的基准除法器设置
	temp_dat[0] = CapSenPara.CLOCK_DIVIDERS_CH0>>8;
	temp_dat[1] = CapSenPara.CLOCK_DIVIDERS_CH0;
//	if(0 == CapSenPara.ChlNmu)
//	{
		result = I2C_Mem_Write(I2C2, CAP_DEV_ADDR, &temp_dat[0], (CAP_CLOCK_DIVIDERS_CH0_REG + 0), 2);
//	}
//	else
//	{
		result = I2C_Mem_Write(I2C2, CAP_DEV_ADDR, &temp_dat[0], (CAP_CLOCK_DIVIDERS_CH0_REG + 1), 2);
//	}
	
	//通道0、通道1的传感器电流驱动配置
	temp_dat[0] = CapSenPara.DRIVE_CURRENT_CH0>>8;
	temp_dat[1] = CapSenPara.DRIVE_CURRENT_CH0;
//	if(0 == CapSenPara.ChlNmu)
//	{
		result = I2C_Mem_Write(I2C2, CAP_DEV_ADDR, &temp_dat[0], (CAP_DRIVE_CURRENT_CH0_REG + 0), 2);
//	}
//	else
//	{
		result = I2C_Mem_Write(I2C2, CAP_DEV_ADDR, &temp_dat[0], (CAP_DRIVE_CURRENT_CH0_REG + 1), 2);
//	}
	
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
	CapSenPara.IsClearBuff = TRUE;
	
	
	
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
	uint64_t    calc_time = 0;
	
	
	
	//1、数据转换时间：tCx = (CHx_RCOUNT * 16) / fREFx，则CHx_RCOUNT = tCx * fREFx / 16.
	//2、晶振频率40MHz，即40000000Hz，由于时间单位是ms，于是40000000 / 1000 = 40000
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
 * @function: CapSenReadChl
 * @details : 读ch1的数据。
              1.当数据转换完毕，RDY信号由高电平转为低电平；读取数据寄存器之后，RDY信号由低电平转为高电平。
 * @input   : NULL
 * @output  : NULL
 * @return  : 读结果，1：成功。其他：表示IIC读取失败.
 */
uint8_t CapSenReadChl(void)
{
	uint8_t    result = 0;
	uint8_t    reg_ch0 = 0;
	uint8_t    reg_ch1 = 0;
	
	
	
	__disable_irq();
	//必须读状态寄存器，否则rdy信号不更新
	result = I2C_Mem_Read(I2C2, CAP_DEV_ADDR, &CapSenPara.Buff[0], CAP_STATE_REG, 2);
	__enable_irq();
	
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
	
	//数据高位寄存器、低位寄存器分开读，不能连续读。
//	if(TRUE == CapSenPara.Ch0UnRead)
//	{
//		__disable_irq();
//		result = I2C_Mem_Read(I2C2, CAP_DEV_ADDR, &CapSenPara.Buff[2], CAP_DATA_CH0_REG, 2);
////		__enable_irq();
////		
////		__disable_irq();
//		result = I2C_Mem_Read(I2C2, CAP_DEV_ADDR, &CapSenPara.Buff[4], CAP_DATA_LSB_CH0_REG, 2);
//		
////		//实测连续读高低数据寄存器，读到高寄存器数据有效，读取低寄存器数据始终是0xFFFF。
////		result = I2C_Mem_Read(I2C2, CAP_DEV_ADDR, &CapSenPara.Buff[2], CAP_DATA_CH0_REG, 4);
//		__enable_irq();
//	}
	
//	if(TRUE == CapSenPara.Ch1UnRead)
//	{
		__disable_irq();
		result = I2C_Mem_Read(I2C2, CAP_DEV_ADDR, &CapSenPara.Buff[6], CAP_DATA_CH1_REG, 2);
//		__enable_irq();
//		
//		__disable_irq();
		result = I2C_Mem_Read(I2C2, CAP_DEV_ADDR, &CapSenPara.Buff[8], CAP_DATA_LSB_CH1_REG, 2);
		
//		//实测连续读高低数据寄存器，读到高寄存器数据有效，读取低寄存器数据始终是0xFFFF。
//		result = I2C_Mem_Read(I2C2, CAP_DEV_ADDR, &CapSenPara.Buff[6], CAP_DATA_CH1_REG, 4);
		__enable_irq();
//	}
	
	
	
	
	
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
	
	__disable_irq();
	result = I2C_Mem_Write(I2C2, CAP_DEV_ADDR, &temp_dat[0], CAP_CONFIG_REG, 2);
	__enable_irq();
	
	
	return (result);
}


/*
 * @function: CapSenChgChl
 * @details : 切换通道
 * @input   : 1.enable：使能睡眠？
 * @output  : NULL
 * @return  : 写结果，1：成功。其他：表示IIC写失败.
 */
uint8_t CapSenChgChl(void)
{
	uint8_t     result = 0;
	uint8_t     temp_dat[2] = {0};
	
	
	if(CapSenPara.CONFIG != UserPara[CAP_CONFIG].Value)
	{
		CapSenPara.CONFIG = UserPara[CAP_CONFIG].Value;
		
		temp_dat[0] = CapSenPara.CONFIG>>8;
		temp_dat[1] = CapSenPara.CONFIG;
		result = I2C_Mem_Write(I2C2, CAP_DEV_ADDR, &temp_dat[0], CAP_CONFIG_REG, 2);
	}
	
	
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
		UserPara[CAP_CONFIG].Value = 0x1E01;
		result = TRUE;
	}
	else
	{
		CapSenPara.ChlNmu = 1;
		UserPara[CAP_CONFIG].Value = 0x5E01;
		result = TRUE;
	}
	CapSenPara.CONFIG = UserPara[CAP_CONFIG].Value;
	
	
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
int32_t    CapDataDis0[CAP_DIS_NUM] = {0};
int32_t    CapDataDis1[CAP_DIS_NUM] = {0};
int32_t    cap_offset = 123000000;
#endif
uint8_t CapSenChData(void)
{
	uint8_t    ret = FALSE;
	
//	uint32_t   ch0_temp = 0;
	uint32_t   ch1_temp = 0;
	
	float      lpf_ratio = 0;
	int32_t    cal_dis = 0;
	
	
	
	//虽然使用硬件INTB信号下降沿作为判断电容通道数据转换是否完成的标志，
	//不过使用寄存器DRDY判断一次，双保险。
	
	//bit6：DRDY寄存器，至少有一个容性通道的转换转换完成。
//	reg_rdy = CapSenPara.Buff[1] & 0x40;
//	if(reg_rdy)
//	{
		Accumulation16(&CapSenPara.UpdateCnt);
//	}
//	else
//	{
//		Accumulation16(&CapSenPara.UpdateNoCnt);
//	}
//	Accumulation32(&CapSenPara.ReadCnt);
	
	
	

	
//	if(TRUE == CapSenPara.Ch0UnRead)
//	{
//		//电容芯片初始化成功，并且数据稳定
////		if((CP_COMM_INIT == CapSenPara.ResStatu)
////			&& (CapSenPara.UpdateCnt > CapSenPara.ResWaitTime))
//		if(CP_COMM_INIT == CapSenPara.ResStatu)
//		{
//			CapSenPara.ResStatu = CP_COMM_NORMAL;
//			
//			CapSenPara.UpdateCnt = 0;
//			CapSenPara.UpdateNoCnt = 0;
//			CapSenPara.ReadCnt = 0;
//		}
//		
//		
//		
//		//通道0数据
//		ch0_temp = MERGE_UINT16(CapSenPara.Buff[2], CapSenPara.Buff[3]);
//		ch0_temp = ch0_temp<<16;
//		ch0_temp = ch0_temp | MERGE_UINT16(CapSenPara.Buff[4], CapSenPara.Buff[5]);
//		ch0_temp = ch0_temp & 0x0FFFFFFF;
//		
//		CapSenPara.ChColl[CLLD_CH0_DATA].IsErr = FALSE;
//		CapSenPara.ChColl[CLLD_CH0_DATA].OrigValue = ch0_temp;
//		
//		
//		
//		//一阶滤波（防干扰作用非常小，有干扰时根本防不住）
////		lpf_ratio = 0.10;
////		lpf_ratio = 0.50;
////		lpf_ratio = 0.60;
////		lpf_ratio = 0.75;
//		lpf_ratio = 0.80;    //经过大量测试，能用。
////		lpf_ratio = 0.85;
////		lpf_ratio = 0.90;    //测试中。
////		lpf_ratio = 0.95;
////		lpf_ratio = 1.00;
//		CapSenPara.ChColl[CLLD_CH0_DATA].FilterValue = LPF1(ch0_temp, CapSenPara.ChColl[CLLD_CH0_DATA].FilterValue, lpf_ratio);
//		ch0_temp = CapSenPara.ChColl[CLLD_CH0_DATA].FilterValue;
//		
//		
//#ifdef CLLD_DEBUG
//		cal_dis = CapSenPara.ChColl[CLLD_CH0_DATA].FilterValue - cap_offset;
//		SmoothPipeline32(CapDataDis0, cal_dis, CAP_DIS_NUM);
//#endif



//	}
	
	
//	if(TRUE == CapSenPara.Ch1UnRead)
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
		
		
		
		//通道1数据
		ch1_temp = MERGE_UINT16(CapSenPara.Buff[6], CapSenPara.Buff[7]);
		ch1_temp = ch1_temp<<16;
		ch1_temp = ch1_temp | MERGE_UINT16(CapSenPara.Buff[8], CapSenPara.Buff[9]);
		ch1_temp = ch1_temp & 0x0FFFFFFF;
		
		CapSenPara.ChColl[CLLD_CH1_DATA].IsErr = FALSE;
		CapSenPara.ChColl[CLLD_CH1_DATA].OrigValue = ch1_temp;
		
		
		
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
		CapSenPara.ChColl[CLLD_CH1_DATA].FilterValue = LPF1(ch1_temp, CapSenPara.ChColl[CLLD_CH1_DATA].FilterValue, lpf_ratio);
		
		
#ifdef CLLD_DEBUG
		cal_dis = CapSenPara.ChColl[CLLD_CH1_DATA].FilterValue - cap_offset;
		SmoothPipeline32(CapDataDis1, cal_dis, CAP_DIS_NUM);
#endif
		
		
		SmoothPipeline32(CapSenPara.ChColl[CLLD_CH1_DATA].FilterData, CapSenPara.ChColl[CLLD_CH1_DATA].FilterValue, CLLD_NUM);
		

		switch(CapSenPara.SlideWindOpenState)
		{
			case CLLD_SLIDEWIND_STATE0:
			{
				//数据稳定后再开窗
				if(CapSenPara.UpdateCnt >= CapSenPara.SlideWindWaitNum)
				{
					//将缓存区设置为相同数据，防止出现误判。
					MyMemSet32(CapSenPara.ChColl[CLLD_CH1_DATA].FilterData, ch1_temp, CLLD_NUM);
					
					CapSenPara.SlideWindOpenState = CLLD_SLIDEWIND_STATE1;
				}
			}
			break;
			
			case CLLD_SLIDEWIND_STATE1:
			{
			}
			break;
			
			default:
			{
				CapSenPara.SlideWindOpenState = CLLD_SLIDEWIND_STATE0;
			}
			break;
		}
		
		
		
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
			//数据有更新
			if(CLLD_SLIDEWIND_STATE1 == CapSenPara.SlideWindOpenState)
			{
				//算法2：判断多次相对变化，提高抗干扰能力，但耗时较多，吸头过冲稍微大一些。
//				for(i = 1; i < set_compare_num; i++)
//				{
//					if(CLLD_ABS_BLOCK == UserPara[LLD_SEN_FUN].Value)
//					{
//						sub1 = CapSenPara.ChColl[CapSenPara.ChlNmu].FilterData[0];
//						sub2 = CapSenPara.ChColl[CapSenPara.ChlNmu].FilterData[i];
//					}
//					else
//					{
//						sub1 = CapSenPara.ChColl[CapSenPara.ChlNmu].FilterData[i];
//						sub2 = CapSenPara.ChColl[CapSenPara.ChlNmu].FilterData[0];
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
//					CapSenPara.ChlPreData = CapSenPara.ChColl[CapSenPara.ChlNmu].OrigValue;
//					CapSenPara.LLDStage = LLD_STAGE_LIQ;
//				}
				
				
				
				//算法5：简化算法，抗干扰能力弱，但耗时较少，吸头过冲较好。
				for(i = 1; i < set_compare_num; i++)
				{
					sub1 = CapSenPara.ChColl[CLLD_CH1_DATA].FilterData[i];
					sub2 = CapSenPara.ChColl[CLLD_CH1_DATA].FilterData[0];
					
					temp = sub1 - sub2;	
					if(temp > (int32_t)CapSenPara.ContactThreshold)
					{
						vaild_cnt++;
					}
				}
				
				if(vaild_cnt >= set_pass_single)
				{
					//保存前几次数据，下面还会使用。
					CapSenPara.ChlPreData = CapSenPara.ChColl[CLLD_CH1_DATA].FilterData[0];
					CapSenPara.LLDResult = LLD_LIQUL;
					
					
					CapSenPara.LLDStage = LLD_STAGE_LIQ;
				}
				else
				{
//					CapSenPara.LLDResult = LLD_ING;
					CapSenPara.LLDResult = LLD_IDLE;
				}
				
				
				
				//重庆普思滑动窗识别算法
//				temp = CapSenPara.AveVal - CapSenPara.ChColl[CLLD_CH1_DATA].OrigValue;
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
				CapSenPara.LLDResult = LLD_IDLE;
				CapSenPara.SlideWindOpenState = CLLD_SLIDEWIND_STATE0;
				
				
				CapSenPara.LLDStage = LLD_STAGE_NULL;
			}
		}
		break;
		
		case LLD_STAGE_START:
		case LLD_STAGE_CONTACT:
		case LLD_STAGE_CHECK_FAIL:
		{
		}
		break;
		
		case LLD_STAGE_LIQ:
		{
			//数据有更新
			if(CLLD_SLIDEWIND_STATE1 == CapSenPara.SlideWindOpenState)
			{
				if(FALSE == CapSenPara.ChColl[CLLD_CH1_DATA].IsErr)
				{
//					//单次判断TIP是否离开液面。抗干扰性能差。
//					temp = CapSenPara.ChlPreData - CapSenPara.ChColl[CapSenPara.ChlNmu].FilterData[0];
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
//						temp = CapSenPara.ChColl[CapSenPara.ChlNmu].FilterData[i] - CapSenPara.ChlPreData;
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
						sub1 = CapSenPara.ChColl[CLLD_CH1_DATA].FilterData[0];
						sub2 = CapSenPara.ChColl[CLLD_CH1_DATA].FilterData[i];
						
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
				CapSenPara.LLDResult = LLD_IDLE;
				
				
				CapSenPara.LLDStage = LLD_STAGE_NULL;
			}
			
			//传感器故障
			if(TRUE == CapSenPara.ChColl[CLLD_CH1_DATA].IsErr)
			{
				CapSenPara.LLDResult = LLD_IDLE;
				
				
				CapSenPara.LLDStage = LLD_STAGE_NULL;
			}
		}
		break;
		
		case LLD_STAGE_FAIL_AIR:
		{
		}
		break;
		
		case LLD_STAGE_BUBBLE:
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
	
	
	
	if(UserPara[CAP_DRIVE_CURRENT_CH0].Value >= FDC2214_Drive_Current_0_413)
	{
		CapSenPara.SlideWindNum = CLLD_LOW_SLIDEWIND_TIME / CapSenPara.ConTime;
	}
	else if(UserPara[CAP_DRIVE_CURRENT_CH0].Value >= FDC2214_Drive_Current_0_108)
	{
		CapSenPara.SlideWindNum = CLLD_MID_SLIDEWIND_TIME / CapSenPara.ConTime;
	}
	else
	{
		CapSenPara.SlideWindNum = CLLD_FAST_SLIDEWIND_TIME / CapSenPara.ConTime;
	}
	CapSenPara.SlideWindWaitNum = CLLD_SLIDEWIND_WAIT_TIME / CapSenPara.ConTime;
	
	
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
	
	extern __IO LLDParam_t g_tLLDParam;
	
	
	
	
	//各个状态下执行的动作
	switch(CapSenPara.ComStage)
	{
		case CP_COMM_RESET_DEV:
		{
			GPIOIIC[IIC_2].State = IIC_RESET_DEV;
			
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
				//iic通信速度100kbps，总耗时436us
				
				CapSenPara.Rdy = FALSE;
				CapSenPara.RdyTime = 0;
				
				//1、进入临界段，有改善效果，但仍然偶发故障，概率约0.5%。可能是原子锁有时间限制。
				//2、使用Delay_MS_NOBlock替换掉电机驱动函数、存储函数中的rt_thread_delay，无改善。
//				rt_enter_critical();    //进入临界段。
				//读取电容数据
				iic_ack = CapSenReadChl();
//				rt_exit_critical();     //退出临界段
				
				
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
			//收到开启探测指令，实际丢弃若干数据
			if(LLD_CLOSE == UserPara[LLD_SEN_FUN].Value)
			{
				if(CP_COMM_NORMAL == CapSenPara.ResStatu)
				{
					CapSenPara.IsClearBuff = TRUE;
				}
			}
			if(TRUE == CapSenPara.IsClearBuff)
			{
				CapSenPara.IsClearBuff = FALSE;
				
				CapSenPara.SlideWindOpenState = CLLD_SLIDEWIND_STATE0;
				CapSenPara.UpdateCnt = 0;
				CapSenPara.UpdateNoCnt = 0;
				CapSenPara.ReadCnt = 0;
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
				
//				CapSenPara.ChColl[CLLD_CH0_DATA].OrigValue = 0;
				CapSenPara.ChColl[CLLD_CH1_DATA].OrigValue = 0;
			}
			else
			{
				GPIOIIC[IIC_2].Dev1Err = FALSE;
			}
			
			
			//6.探测到液面，关闭电容探测信号，降低相互干扰。
			switch(UserPara[LLD_SEN_FUN].Value)
			{
				case LLD_CLOSE:
				{
					//关闭电容探测信号，为了让注检EMC通过。
					CapSenPara.SleepOpen = TRUE;
				}
				break;
				
				case CLLD_RT_LIQUL:
				case LLD_CAP_RT_AIR_LIQUL:
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
				break;
				
				case CLLD_ST_LIQUL:
				case LLD_CAP_ST_AIR_LIQUL:
				{
					//分时探测
					result = g_tLLDParam.CanConfig.ModuleID % 2;
					if(0 == result)
					{
						//偶数通道
						CapSenPara.SleepTimeShareCtl = (~CapSenPara.SynSigal) & 0x01;
					}
					else
					{
						//奇数通道
						CapSenPara.SleepTimeShareCtl = CapSenPara.SynSigal;
					}
					
					
					if(LLD_STAGE_LIQ == CapSenPara.LLDStage)
					{
						CapSenPara.SleepOpen = TRUE;
					}
					else 
					{
						CapSenPara.SleepOpen = CapSenPara.SleepTimeShareCtl;
					}
				}
				break;
				
				default:
				{
				}
				break;
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
//				rt_enter_critical();    //进入临界段
				iic_ack = CapSenSleepEn(CapSenPara.SleepOpen);
//				rt_exit_critical();     //退出临界段
				
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
//				result = CapSenChlSelect();
				
				
				CapSenPara.ResStatu = CP_COMM_RESET_DEV;
			}
			
			
			
			
			
			
			
			
			if(CP_COMM_RESET_DEV == CapSenPara.ResStatu)
			{
				//重置传感器
				iic_ack = CapSenReset();
				
				
				//初始化设备
				CapSenPara.ComStage = CP_COMM_INIT;
			}
			
			if(IIC_RELEASE == GPIOIIC[IIC_2].State)
			{
				CapSenPara.RdyTime = 0;
				
				
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
			CapSenPara.RdyTime = 0;
			
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
	UserPara[CAP_RCOUNT_CH0].Value = 0x25D8;    //计算结果31.0024ms，示波器实测31ms
//	UserPara[CAP_RCOUNT_CH0].Value = 0x2BF2;    //计算结果36.0008ms，示波器实测36ms
//	UserPara[CAP_RCOUNT_CH0].Value = 0x320D;    //计算结果41.0024ms，示波器实测41ms
//	UserPara[CAP_RCOUNT_CH0].Value = 0x3827;    //计算结果46.0008ms，示波器实测46ms
//	UserPara[CAP_RCOUNT_CH0].Value = 0x3E42;    //计算结果51.0024ms，示波器实测51ms
//	UserPara[CAP_RCOUNT_CH0].Value = 0x445C;    //计算结果56.0008ms，示波器实测56ms
//	UserPara[CAP_RCOUNT_CH0].Value = 0x4A77;    //计算结果61.0024ms，示波器实测61ms。保守取值。
	
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
	//四舍五入为8。为了提供冗余以保证系统公差，选择更高的值10，tS0=10 * 16 / 40 000 000 = 4us
//	UserPara[CAP_SETTLECOUNT_CH0].Value = 0x1388;
	// UserPara[CAP_SETTLECOUNT_CH0].Value = 0x2710;
//	UserPara[CAP_SETTLECOUNT_CH0].Value = 0xFFFF;
//	UserPara[CAP_SETTLECOUNT_CH0].Value = 200;
	UserPara[CAP_SETTLECOUNT_CH0].Value = 100;
	
	
	
	
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
//	UserPara[CAP_CLOCK_DIVIDERS_CH0].Value = 0x2008;    //5MHz        //保守取值，灵敏度最高，抗干扰一般。
//	UserPara[CAP_CLOCK_DIVIDERS_CH0].Value = 0x200A;    //4MHz
//	UserPara[CAP_CLOCK_DIVIDERS_CH0].Value = 0x2014;    //2MHz
//	UserPara[CAP_CLOCK_DIVIDERS_CH0].Value = 0x2028;    //1MHz
	
	UserPara[CAP_CLOCK_DIVIDERS_CH0].Value = 0x1004;    //1分频；10MHz参考频率。
	
	
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
//	UserPara[CAP_CONFIG].Value = 0x1E01;    //启用通道0；设备激活，外部晶振，以选择电流启动。
//	UserPara[CAP_CONFIG].Value = 0x5501;    //启用通道1；设备激活，内部晶振。
//	UserPara[CAP_CONFIG].Value = 0x5601;    //启用通道1；设备激活，外部晶振。
	UserPara[CAP_CONFIG].Value = 0x5E01;    //启用通道1；设备激活，外部晶振，以选择电流启动。
	
	
	
	//信道复用配置
	//bit15：自动扫描模式使能，0：连续转换由CONFIG.ACTIVE_CHAN选中的通道；1：自动扫描由MUX_CONFIG.RR_SEQUENCE选中的通道。
	//bit14-13：配置自动扫描的通道。b00：通道0、1；b01：通道0、1、2.(仅FDC2114、FDC2214有效) b10：通道0、1、2、3.(仅FDC2114、FDC2214有效) b11：通道0、1。
	//bit12-3：预留。复位值00 0100 0001。
	//bit2-0：设置带宽，b001：1MHz；b100:3.3MHz；b101：10MHz；b111：33MHz。输入抗尖峰脉冲滤波器带宽设置为10MHz，这是超出振荡振荡器频率的最低设置。
//	UserPara[CAP_MUX_CONFIG].Value = 0x0209;
//	UserPara[CAP_MUX_CONFIG].Value = 0x020C;
	UserPara[CAP_MUX_CONFIG].Value = 0x020D;
//	UserPara[CAP_MUX_CONFIG].Value = 0x020F;
	
	//多通道，自动扫描通道0、1
//	UserPara[CAP_MUX_CONFIG].Value = 0x820D;
//	UserPara[CAP_MUX_CONFIG].Value = 0xE20D;
	
	
	
	
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
	con_liq_threshold = 200000;
	leave_liq_threshold = 180000;
	UserPara[CAP_CON_THRESHOLD_H].Value = con_liq_threshold>>16;
	UserPara[CAP_CON_THRESHOLD_L].Value = con_liq_threshold;
	UserPara[CAP_LEV_THRESHOLD_H].Value = leave_liq_threshold>>16;
	UserPara[CAP_LEV_THRESHOLD_L].Value = leave_liq_threshold;
	
	
	//电容探测档位、转换时间
	UserPara[CAP_SET_GEAR].Value = 0;
	UserPara[CAP_SET_CONTIME].Value = 31;
	
	
	
	
	
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
	CapSenPara.SlideWindNum = CLLD_NUM;
	CapSenPara.SlidePassSingle = 2;
	CapSenPara.SlidePassAll = 2;
	
	//对接应用数据缓存
//	CapSenPara.ChColl[CLLD_CH0_DATA].OrigData = CapSenPara.AppOrigBuffCh0;
//	CapSenPara.ChColl[CLLD_CH0_DATA].FilterData = CapSenPara.AppFilterBuffCh0;
//	CapSenPara.ChColl[CLLD_CH1_DATA].OrigData = CapSenPara.AppOrigBuffCh1;
	CapSenPara.ChColl[CLLD_CH1_DATA].FilterData = CapSenPara.AppFilterBuffCh1;
	
	//是否复位设备
	CapSenPara.ResStatu = CP_COMM_NORMAL;
	
	//默认开启睡眠
	CapSenPara.SleepOpen = FALSE;
	CapSenPara.SleepStatu = FALSE;
	CapSenPara.ChlNmu = 1;
	
	//切换通道
//	CapSenPara.ChlNeedSwitch = FALSE;
//	CapSenPara.ChlIsSwitch = FALSE;
//	CapSenPara.ChlCnt = 0;
//	CapSenPara.ChlThrowAwayNum = 3;
	
	
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
	
	
	if(sig >= AIRPRESS_PASS_26)
	{
		*state = 5;
	}
	else if(sig >= AIRPRESS_PASS_18)
	{
		*state = 4;
	}
	else if(sig >= AIRPRESS_PASS_12)
	{
		*state = 3;
	}
	else if(sig >= AIRPRESS_PASS_6)
	{
		*state = 2;
	}
	else if(sig >= AIRPRESS_PASS_3)
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
	int32_t     temp = 0;
	TMC_e       eTMC = TMC_0;
	
	
	switch(air_sen->AirPress.Stage)
	{
		case PLLD_STAGE_NULL:
		{
			if((is_check >= PLLD_ABS_BLOCK) && (is_check <= PLLD_ABS_BLOCK_FOLLOW_AIR))
			{
				//P电机真实启动，与气压出现变化，大约有2*5=10ms的延时
				if(PLLD_ABS_START == air_sen->ABS_Real_Start_End)
				{
					Accumulation16(&air_sen->AbsState_Timer1);
					if(air_sen->AbsState_Timer1 >= air_sen->ABS_StartDly)
					{
//						air_sen->AbsState1_1 = 0;
						
						air_sen->AbsState2_1 = 0;
						
//						air_sen->AbsState3_1 = 0;
						air_sen->AbsState3_2 = 0;
						
						air_sen->AbsState_Timer1 = 0;
//						air_sen->AbsState_Timer2 = 0;
						air_sen->AbsState_Timer3 = 0;
						air_sen->AbsState_Timer4 = 0;
						air_sen->AbsAirFollowState1 = 0;
						air_sen->AbsAirFollowState2 = 0;
						
						
						//检测到P电机开始吸液，向前移动 n个单位，取气压作为吸液前参考气压
						air_sen->ABS_StartAirPress = air_sen->ChColl[PLLD_CH0_DATA].FilterData[AIRPRESS_PASS_8];
						
						
						air_sen->AirPress.Stage = PLLD_STAGE_START;
					}
				}
			}
			else if(PLLD_NO_AIR_LIQUL == is_check)
			{
				Accumulation16(&air_sen->AbsState_Timer1);
				if(air_sen->AbsState_Timer1 >= air_sen->ABS_StartDly)
				{
					air_sen->AbsState_Timer1 = 0;
					
					//将缓存区设置为相同数据，防止出现误判。
					temp = air_sen->ChColl[PLLD_CH0_DATA].FilterData[0] + air_sen->ChColl[PLLD_CH0_DATA].FilterData[1];
					temp = temp / 2;
					MyMemSet16(air_sen->ChColl[PLLD_CH0_DATA].FilterData, temp, PLLD_NUM);
					
					
					
					air_sen->AirPress.Stage = PLLD_STAGE_START;
				}
			}
			else if((PLLD_LOW_SPEED_LIQUL == is_check)
					|| (PLLD_BUBBLE == is_check))
			{
				//吸气探测气泡
				//P电机真实启动，与气压出现变化，不同的吸气速度，延时不同。
				//P电机吸气速度越慢，气压变化幅度越小。
				//电机收到指令开始计时。
				//(1)100ul/s吸气速度，延时约2*10ms。
				//(2)200ul/s吸气速度，延时约2*5ms。
				//(3)20ul/s吸气速度，延时约117ms，达到匀速。
				//(3)20ul/s吸气速度，延时约163ms，达到匀速。
				//(3)15ul/s吸气速度，延时约216ms，达到匀速。
				//(3)10ul/s吸气速度，延时约465ms，达到匀速。
				//(3)6ul/s吸气速度，延时约448ms，达到匀速。
				//(3)5ul/s吸气速度，经过约445ms，达到匀速。
				//(4)4ul/s吸气速度，经过约814ms，达到匀速。
				//(4)3ul/s吸气速度，经过约1100ms，达到匀速。
				//(5)2ul/s吸气速度，经过约1649ms，达到匀速。
				//(6)1ul/s吸气速度，经过约2033ms，达到匀速。
				if(PLLD_ABS_START == air_sen->ABS_Real_Start_End)
				{
					Accumulation16(&air_sen->AbsState_Timer1);
					if(air_sen->AbsState_Timer1 >= air_sen->ABS_StartDly)
					{
						air_sen->AbsState_Timer1 = 0;
						air_sen->AtmosAbsRef = air_sen->AtmosAbsAve;
						air_sen->AbsState1_3 = 0;
						
						//将缓存区设置为相同数据，防止出现误判。
						temp = air_sen->ChColl[PLLD_CH0_DATA].FilterData[0] + air_sen->ChColl[PLLD_CH0_DATA].FilterData[1];
						temp = temp / 2;
						MyMemSet16(air_sen->ChColl[PLLD_CH0_DATA].FilterData, temp, PLLD_NUM);
						
						
						
						air_sen->AirPress.Stage = PLLD_STAGE_START;
					}
				}
			}
			else if((LLD_CAP_RT_AIR_LIQUL == is_check)
					|| (LLD_CAP_ST_AIR_LIQUL == is_check)
					|| (PLLD_DIS_BLOCK == is_check)
					|| (PLLD_DIS_AIR == is_check))
			{
				if((PLLD_ABS_START == air_sen->ABS_Real_Start_End)
					|| (PLLD_ABS_END == air_sen->ABS_Real_Start_End))
				{
					air_sen->AirPress.Stage = PLLD_STAGE_START;
				}
			}
			
			
		}
		break;
		
		case PLLD_STAGE_START:
		{
			if(PLLD_NO_AIR_LIQUL == is_check)
			{
				temp = air_sen->AtmosAbsAve - air_sen->AtmosAve;
				if((temp <= (-air_sen->AtmosNoise)) || (temp >= air_sen->AtmosNoise))
				{
					//关闭P轴
					TMC_Stop(eTMC);
					
					air_sen->LLDResult = LLD_LIQUL;
//					
					air_sen->AirPress.Stage = PLLD_STAGE_LIQ;
				}
			}
			else if(PLLD_LOW_SPEED_LIQUL == is_check)
			{
				if(air_sen->AbsState1_3 >= 1)
				{
					//关闭P轴
					TMC_Stop(eTMC);
					
					air_sen->LLDResult = LLD_LIQUL;
					
					air_sen->AirPress.Stage = PLLD_STAGE_LIQ;
				}
			}
			else if(PLLD_BUBBLE == is_check)
			{
				//发现气压值微微增大或变小，吸头触碰了气泡触或液面
				temp = air_sen->AtmosAbsAve - air_sen->AtmosAbsRef;
				if((temp <= (-air_sen->RakeRatio2Min)) || (temp >= air_sen->RakeRatio2Min))
				{
					air_sen->AbsState1_6 = 0;
					
					
					air_sen->AirPress.Stage = PLLD_STAGE_BUBBLE;
				}
			}
			else if((is_check >= PLLD_ABS_BLOCK)
					&& (is_check <= PLLD_ABS_BLOCK_FOLLOW_AIR))
			{
				//等待吸液泵启动吸液稳定
				Accumulation16(&air_sen->AbsState_Timer1);
				if(air_sen->AbsState_Timer1 >= air_sen->ABS_StartDly)
				{
//					air_sen->AbsState1_1 = 0;
					
					air_sen->AbsState2_1 = 0;
					
//					air_sen->AbsState3_1 = 0;
					air_sen->AbsState3_2 = 0;
					
					air_sen->AbsState_Timer1 = 0;
//					air_sen->AbsState_Timer2 = 0;
					air_sen->AbsState_Timer3 = 0;
					air_sen->AbsState_Timer4 = 0;
					air_sen->AbsAirFollowState1 = 0;
					air_sen->AbsAirFollowState2 = 0;
					
					
					air_sen->AirPress.Stage = PLLD_STAGE_TOUCH;
				}
			}
			else if((LLD_CAP_RT_AIR_LIQUL == is_check)
					|| (LLD_CAP_ST_AIR_LIQUL == is_check))
			{
				if(PLLD_ABS_END == air_sen->ABS_Real_Start_End)
				{
					temp = air_sen->ChColl[PLLD_CH0_DATA].FilterData[0] - air_sen->AtmosAve;
					//使用相对变化量
					if(temp <= air_sen->RakeRatio3Min)
					{
						air_sen->LLDResult = LLD_FAIL_AIR;
						
						air_sen->AirPress.Stage = PLLD_STAGE_COMPLETE;
					}
					else
					{
						air_sen->LLDResult = LLD_LIQUL;
						
						air_sen->AirPress.Stage = PLLD_STAGE_LIQ;
					}
				}
			}
			else if(PLLD_DIS_BLOCK == is_check)
			{
				if(PLLD_ABS_START == air_sen->ABS_Real_Start_End)
				{
					//排液过程中，查看气压值是否低于排液堵塞阈值
					if(air_sen->ChColl[PLLD_CH0_DATA].FilterData[0] <= (AIR_SEN_PRESS_MAX - AIR_SEN_ERR_PRESS))
					{
						air_sen->LLDResult = PLLD_STAGE_DIS_BLOCK;
						
						air_sen->AirPress.Stage = PLLD_STAGE_DIS_BLOCK;
					}
				}
				else if(PLLD_ABS_END == air_sen->ABS_Real_Start_End)
				{
					Accumulation16(&air_sen->AbsState_Timer4);

					if(air_sen->AbsState_Timer4 >= air_sen->ABS_EndDly)
					{
						air_sen->AbsState_Timer4 = 0;
						
						
						//排液完毕，查看气压值是否超过吸液堵塞阈值
						if(air_sen->AbsState2_1 >= 1)
						{
							air_sen->LLDResult = PLLD_STAGE_DIS_BLOCK;
							
							air_sen->AirPress.Stage = PLLD_STAGE_DIS_BLOCK;
						}
						else
						{
							air_sen->LLDResult = LLD_IDLE;
							
							air_sen->AirPress.Stage = PLLD_STAGE_COMPLETE;
						}
					}
				}
			}
			else if(PLLD_DIS_AIR == is_check)
			{
				if(PLLD_ABS_END == air_sen->ABS_Real_Start_End)
				{
					Accumulation16(&air_sen->AbsState_Timer4);

					if(air_sen->AbsState_Timer4 >= air_sen->ABS_EndDly)
					{
						air_sen->AbsState_Timer4 = 0;
					
						temp = air_sen->ChColl[PLLD_CH0_DATA].FilterData[0] - air_sen->AtmosAve;
						//使用相对变化量
						if(temp <= air_sen->RakeRatio3Min)
						{
							air_sen->LLDResult = LLD_IDLE;
							
							air_sen->AirPress.Stage = PLLD_STAGE_COMPLETE;
						}
						else
						{
							air_sen->LLDResult = LLD_DIS_NO_AIR;
							
							air_sen->AirPress.Stage = PLLD_STAGE_COMPLETE;
						}
					}
				}
			}
			
			
		}
		break;
		
		case PLLD_STAGE_TOUCH:
		{
//			//吸液完毕，判断吸液是否全程
//			{
//				if(PLLD_ABS_END == air_sen->ABS_Real_Start_End)
//				{
//					//检测全程吸空。不推荐使用绝对变化量，传感器工作时间长，零点偏移。
////					temp = air_sen->ChColl[PLLD_CH0_DATA].FilterData[0] - AIR_SEN_ZERO_PRESS;
////					if(temp < air_sen->RakeRatio3Min)
////					{
////						air_sen->AirPress.Stage = PLLD_STAGE_AIR_ALWAYS;
////					}
//					
//					//使用相对变化量
//					temp = air_sen->ChColl[PLLD_CH0_DATA].FilterData[0] - air_sen->ABS_StartAirPress;
//					if(temp <= air_sen->RakeRatio3Min)
//					{
//						air_sen->AirPress.Stage = PLLD_STAGE_ABS_AIR;
//					}
//				}
//			}
			
			
			
			//吸液时吸空
			//吸液堵塞和检测吸液时吸空
			if((PLLD_ABS_FOLLOW_AIR == is_check)
				|| (PLLD_ABS_BLOCK_FOLLOW_AIR == is_check))
			{
				//有更好办法检测部分行程吸空
				
//				//检测气压剧烈跳动变小，有4种情况：
//				//（1）P电机启动吸液。
//				//（2）P电机吸液中，吸头从空气进入液体，即部分行程吸空。
//				//（3）追随吸液时吸空，气压先变小后变大。检测吸气是否上升。
//				//（4）吸液时被异物短暂堵塞，然后被吸进吸头，气压短暂变小，然后恢复正常。
//				if(air_sen->AbsState3_1 >= 1)
//				{
//					air_sen->AbsAirFollowState1 = 1;
//				}
//				if(air_sen->AbsAirFollowState1 >= 1)
//				{
//					Accumulation16(&air_sen->AbsState_Timer2);
//				}
//				//P电机加速，可能误触发部分行程吸空
//				if(air_sen->AbsAirFollowState1 >= 1)
//				{
//					//收到P电机吸液完毕信息，查看是否有误判
//					if(PLLD_ABS_END == air_sen->ABS_Real_Start_End)
//					{
//						if(air_sen->AbsState_Timer2 >= air_sen->ABS_EndDly)
//						{
//							air_sen->AbsState_Timer2 = 0;
//						
//						
//							air_sen->AirPress.Stage = PLLD_STAGE_AIR_PART;
//						}
//						else
//						{
//							air_sen->AbsAirFollowState1 = 0;
//						}
//					}
//				}
				
				
				
				//检测气压剧烈跳动下降，有2种情况：
				//（1）吸液即将结束，P电机减速。
				//（2）吸液时吸空，气压出现尖峰跳变，或者单边沿跳变。
				if(air_sen->AbsState3_2 >= 1)
				{
					air_sen->AbsAirFollowState2 = 1;
				}
				if(air_sen->AbsAirFollowState2 >= 1)
				{
					Accumulation16(&air_sen->AbsState_Timer3);
				}
				
				
				//P电机减速，可能误触发吸液时吸空
				if(air_sen->AbsAirFollowState2 >= 1)
				{
					//收到P电机吸液完毕信息，查看是否有误判
					if(PLLD_ABS_END == air_sen->ABS_Real_Start_End)
					{
						if(air_sen->AbsState_Timer3 >= air_sen->ABS_EndDly)
						{
							air_sen->AbsState_Timer3 = 0;
							
							air_sen->LLDResult = LLD_ABS_AIR;
							air_sen->AirPress.Stage = PLLD_STAGE_ABS_AIR;
						}
						else
						{
							air_sen->AbsAirFollowState2 = 0;
							
							air_sen->LLDResult = LLD_IDLE;
						}
					}
				}
				
				
				//全程吸空
				if(PLLD_ABS_END == air_sen->ABS_Real_Start_End)
				{
					temp = air_sen->ChColl[PLLD_CH0_DATA].FilterData[0] - air_sen->AtmosAve;
					
					if(temp <= air_sen->RakeRatio3Min)
					{
						air_sen->LLDResult = LLD_ABS_AIR;
						
						air_sen->AirPress.Stage = PLLD_STAGE_ABS_AIR;
					}
				}
			}
			
			
			
			//检测凝块。最高优先级。
			if((PLLD_ABS_BLOCK == is_check)
				|| (PLLD_ABS_BLOCK_FOLLOW_AIR == is_check))
			{
				if(PLLD_ABS_START == air_sen->ABS_Real_Start_End)
				{
					//吸液过程中，查看气压值是否超过吸液堵塞阈值
					if(air_sen->ChColl[PLLD_CH0_DATA].FilterData[0] >= AIR_SEN_ERR_PRESS)
					{
						air_sen->LLDResult = LLD_ABS_BLOCK;
						
						air_sen->AirPress.Stage = PLLD_STAGE_ABS_BLOCK;
					}
				}
				else if(PLLD_ABS_END == air_sen->ABS_Real_Start_End)
				{
					Accumulation16(&air_sen->AbsState_Timer4);

					if(air_sen->AbsState_Timer4 >= air_sen->ABS_EndDly)
					{
						air_sen->AbsState_Timer4 = 0;
						
						
						//吸液完毕，查看气压值是否超过吸液堵塞阈值
						if(air_sen->AbsState2_1 >= 1)
						{
							air_sen->LLDResult = LLD_ABS_BLOCK;
							
							air_sen->AirPress.Stage = PLLD_STAGE_ABS_BLOCK;
						}
					}
					
//					//无异常
//					if(LLD_ING == air_sen->LLDResult)
//					{
//						air_sen->LLDResult = LLD_IDLE;
//					}
				}
			}
			
			
		}
		break;
		
		case PLLD_STAGE_BUBBLE:
		{
			temp = air_sen->AtmosAbsAve - air_sen->AtmosAbsRef;
			
			//吸头穿过气泡，扎入液面。
			//1、气压逐渐下降。
			//2、气压低于参考线。
			if((air_sen->AbsState1_6 >= 1)
				&& (temp >= air_sen->RakeRatio2Max))
			{
				air_sen->AirPress.Stage = PLLD_STAGE_LIQ;
			}
			
			
			//最后的保险：
			//1、吸头扎入深度较深，吸头内部出现短暂正压。
//			temp = air_sen->ChColl[PLLD_CH0_DATA].FilterData[0] - air_sen->AtmosAve;
//			if(temp <= (-air_sen->RakeRatio1Min))
			temp = air_sen->ChColl[PLLD_CH0_DATA].FilterData[0] + air_sen->RakeRatio1Min;
			if(temp <= air_sen->AtmosAve)
			{
				//关闭P轴
				TMC_Stop(eTMC);
				
				air_sen->LLDResult = LLD_LIQUL;
				
				air_sen->AirPress.Stage = PLLD_STAGE_LIQ;
			}
			
			
			//安全检查：吸液过程中，查看液体流动是否到吸头滤芯位置
			//2、吸头吸液增多，吸头内部出现持续负压。
			temp = air_sen->AtmosAbsAve + air_sen->RakeRatio3Max;
			if(air_sen->ChColl[PLLD_CH0_DATA].FilterData[0] >= temp)
			{
				//关闭P轴
				TMC_Stop(eTMC);
				
				air_sen->LLDResult = LLD_ABS_BLOCK;
				
				air_sen->AirPress.Stage = PLLD_STAGE_ABS_BLOCK;
			}
		}
		break;
		
		case PLLD_STAGE_ABS_BLOCK:
		case PLLD_STAGE_ABS_AIR:
		case PLLD_STAGE_LIQ:
		case PLLD_STAGE_DIS_BLOCK:
		case PLLD_STAGE_DIS_NO_AIR:
		case PLLD_STAGE_COMPLETE:
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
	uint8_t    ret = 0;
	uint8_t    i = 0;
	uint8_t    set_comp_num1 = 0;        //设置比较次数
	uint8_t    set_comp_num2 = 0;        //设置比较次数
	uint8_t    set_comp_num3 = 0;        //设置比较次数
	uint8_t    set_comp_num4 = 0;        //设置比较次数
	uint8_t    set_comp_num5 = 0;        //设置比较次数	
	int16_t    curve_rake_ratio = 0;     //曲线斜率
	
	int32_t    sub1 = 0;
	int32_t    sub2 = 0;
	int32_t    temp = 0;
	
//	int32_t  tarstep;
//	int32_t  currentstep;
//	TMC_e eTMC = TMC_0;
	
	
	
	
	//开窗，不能超过数据长度。
	set_comp_num1 = PLLD_SLIDEWIND_TIME / SoftSys.LLDPeriod;
	set_comp_num2 = PLLD_NO_AIR_SLIDEWIND_TIME / SoftSys.LLDPeriod;
	set_comp_num3 = PLLD_AIR_SLIDEWIND_TIME / SoftSys.LLDPeriod;
	set_comp_num4 = PLLD_BUBBLE_SLIDEWIND_TIME / SoftSys.LLDPeriod;
	set_comp_num5 = PLLD_ASP_SLIDEWIND_TIME / SoftSys.LLDPeriod;
	
	
	//P电机运行状态
	switch(AirSenPara.ABS_Real_Start_End)
	{
		case PLLD_ABS_IDLE:
		{
			AirSenPara.AbsState_Timer0 = 0;
		}
		break;
		
		case PLLD_ABS_END:
		{
			Accumulation16(&AirSenPara.AbsState_Timer0);
		}
		break;
		
		case PLLD_ABS_START:
		{
			ret = TMC5160_FIELD_READ(TMC_0, TMC5160_RAMPSTAT, TMC5160_RAMPSTAT_POS_REACH_MASK, TMC5160_RAMPSTAT_POS_REACH_SHIFT);
			//LOG_Info("RAMP_STAT=%X", TMC_ReadInt(eTMC, TMC5160_RAMPSTAT));
			
			//检查电机是否真实结束
			if(0x01 == ret)
			{
//				//读取目标位置
//				tarstep = TMC5160_ReadInt(eTMC, TMC5160_XTARGET);
//				
//				//读取当前位置
//				currentstep = TMC5160_ReadInt(eTMC, TMC5160_XACTUAL);
				
				
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
			//计算最新大气压值
			AirSenPara.Atmos = 0;
			for(i = 0; i < set_comp_num2; i++)
			{
				AirSenPara.Atmos = AirSenPara.Atmos + AirSenPara.ChColl[PLLD_CH0_DATA].FilterData[i];
			}
			AirSenPara.AtmosAve = AirSenPara.Atmos / set_comp_num2;
			
			
			
			
			//开启气压探测
			if(LLD_CLOSE != UserPara[LLD_SEN_FUN].Value)
			{
				AirSenPara.AirPress.Stage = PLLD_STAGE_NULL;
				AirSenPara.LLDResult = LLD_IDLE;
				
//				AirSenPara.CommAckIsBlock = FALSE;
//				AirSenPara.ABS_Start_End = PLLD_ABS_START;
				
				AirSenPara.AbsState_Timer1 = 0;
//				AirSenPara.AbsState_Timer2 = 0;
				AirSenPara.AbsState_Timer3 = 0;
				AirSenPara.AbsState_Timer4 = 0;
				
				
				AirSenPara.LLDStage = LLD_STAGE_START;
			}
			
			
			//仅开启电容探测，不往下走
			if((CLLD_RT_LIQUL == UserPara[LLD_SEN_FUN].Value)
				|| (CLLD_ST_LIQUL == UserPara[LLD_SEN_FUN].Value))
			{
				AirSenPara.LLDStage = LLD_STAGE_NULL;
			}
		}
		break;
		
		case LLD_STAGE_START:
		{
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
			
			
			
			
			if((LLD_CAP_RT_AIR_LIQUL == UserPara[LLD_SEN_FUN].Value)
				|| (LLD_CAP_ST_AIR_LIQUL == UserPara[LLD_SEN_FUN].Value)
				|| (PLLD_DIS_BLOCK == UserPara[LLD_SEN_FUN].Value)
				|| (PLLD_DIS_AIR == UserPara[LLD_SEN_FUN].Value))
			{
//				AirSenPara.LLDResult = LLD_ING;
				
				AirSenPara.LLDStage = LLD_STAGE_CHECK_FAIL;
			}
			
			
			
			
			//无需吸气探测
			//低速度探测
			//探测气泡气泡
			if((PLLD_NO_AIR_LIQUL == UserPara[LLD_SEN_FUN].Value)
				|| (PLLD_LOW_SPEED_LIQUL == UserPara[LLD_SEN_FUN].Value)
				|| (PLLD_BUBBLE == UserPara[LLD_SEN_FUN].Value))
			{
//				AirSenPara.LLDResult = LLD_ING;
				
				AirSenPara.LLDStage = LLD_STAGE_CONTACT;
			}
			
			
			//检测凝块、空吸，直接往下走，无需等待
			if((UserPara[LLD_SEN_FUN].Value >= PLLD_ABS_BLOCK)
				&& (UserPara[LLD_SEN_FUN].Value <= PLLD_ABS_BLOCK_FOLLOW_AIR))
			{
//				AirSenPara.LLDResult = LLD_ING;
				
				AirSenPara.LLDStage = LLD_STAGE_CONTACT;
			}
			
			
			//开启气压探测，才需要往下走
			//这动作十分重要，否则上电后会误判
			if(LLD_CLOSE == UserPara[LLD_SEN_FUN].Value)
			{
//				AirSenPara.CommAckIsBlock = FALSE;
//				AirSenPara.ABS_Start_End = PLLD_ABS_IDLE;
				AirSenPara.ABS_Real_Start_End = PLLD_ABS_IDLE;
				AirSenPara.LLDResult = LLD_IDLE;
				
				
				AirSenPara.LLDStage = LLD_STAGE_NULL;
			}
		}
		break;
		
		case LLD_STAGE_CONTACT:
		{
//			AirSenPara.sig1_cnt_1 = 0;
//			AirSenPara.sig1_cnt_2 = 0;
			AirSenPara.sig1_cnt_3 = 0;
//			AirSenPara.sig1_cnt_4 = 0;
//			AirSenPara.sig1_cnt_5 = 0;
			AirSenPara.sig1_cnt_6 = 0;
			
			AirSenPara.sig2_cnt_1 = 0;
//			AirSenPara.sig2_cnt_2 = 0;
			
//			AirSenPara.sig3_cnt_1 = 0;
			AirSenPara.sig3_cnt_2 = 0;
			
			
			
			
			
			
			AirSenPara.Atmos = 0;
			//检查曲线状态
			for(i = set_comp_num1; i > 0; i--)
			{
				//1、不吸不排气探测
				if(i <= set_comp_num2)
				{
					AirSenPara.Atmos = AirSenPara.Atmos + AirSenPara.ChColl[PLLD_CH0_DATA].FilterData[i - 1];
				}
				
				
				
				//2、吸气探测液面。
				if(i >= set_comp_num3)
				{
					sub1 = (AirSenPara.ChColl[PLLD_CH0_DATA].FilterData[0] + AirSenPara.ChColl[PLLD_CH0_DATA].FilterData[1]) / 2;
					sub2 = (AirSenPara.ChColl[PLLD_CH0_DATA].FilterData[i - 1] + AirSenPara.ChColl[PLLD_CH0_DATA].FilterData[i]) / 2;
					curve_rake_ratio = sub1 - sub2;
					//快速识别跳变幅值。在此不清零
					if((curve_rake_ratio >= AirSenPara.RakeRatio1Max) || (curve_rake_ratio <= (-AirSenPara.RakeRatio1Max)))
					{
						Accumulation8(&AirSenPara.sig1_cnt_3);
					}
				}
				
				
				if(i >= set_comp_num4)
				{
					//探测气泡
					sub1 = AirSenPara.ChColl[PLLD_CH0_DATA].FilterData[i - set_comp_num4];
					sub2 = AirSenPara.ChColl[PLLD_CH0_DATA].FilterData[i];
					curve_rake_ratio = sub1 - sub2;
					
					
					//吸头扎进液面，气压变化受到Z轴下降速度、P轴吸气速度、吸头口径、试剂粘稠度、毛细作用影响。
					//(1)Z轴下降：使得吸头内部气体受到挤压，因此气压逐渐增大。速度越快，气压越大。
					//(2)P轴吸气：使得吸头内部气体被抽走，因此气压逐渐减小。速度越快，气压越小。
					//(3)吸头口径：液体从吸头尖端进入，挤压吸头内部空气。头口径越大，进入吸头内部的液体体积越多，挤压越大，气压越大。
					//(4)试剂粘稠度：液体与吸头内壁附着，阻碍液体进入吸头内部。液体越粘稠，液体进入吸头内部的体积越少，挤压空气效果越小，气压增大效果越小。
					//(5)毛细作用：试剂沿着吸头内壁往上爬，挤压内部空气，使得气压增大。乙醇毛细作用特别明显。
					if(AirSenPara.ChColl[PLLD_CH0_DATA].FilterData[0] >= AirSenPara.AtmosAbsRef)
					{
						if((curve_rake_ratio >= AirSenPara.RakeRatio2Min)
							&& (curve_rake_ratio <= AirSenPara.RakeRatio2Max))
						{
							Accumulation8(&AirSenPara.sig1_cnt_6);
						}
						else
						{
							AirSenPara.sig1_cnt_6 = 0;
						}
					}
				}
				
				
				
				
				if(i >= set_comp_num5)
				{
					//曲线快速上升或下降（只能检测较短快速变化信号）
					sub1 = AirSenPara.ChColl[PLLD_CH0_DATA].FilterData[i - set_comp_num5];
					sub2 = AirSenPara.ChColl[PLLD_CH0_DATA].FilterData[i];
					curve_rake_ratio = sub1 - sub2;
					
					
					
					
					//2、探测吸液堵塞
					//气压大于特定值，使用气压阈值判断凝块
					if(AirSenPara.ChColl[PLLD_CH0_DATA].OrigValue >= AirSenPara.RakeRatio3Max)
					{
						//凝块
						Accumulation8(&AirSenPara.sig2_cnt_1);
					}
					else
					{
						AirSenPara.sig2_cnt_1 = 0;
					}
					
					
					
//					//3、部分行程吸空
//					//前部分行程吸空，后部分行程吸到液体。即短暂吸空到真正吸液。
//					if(curve_rake_ratio >= AirSenPara.AspLiqNoise)
//					{
//						Accumulation8(&AirSenPara.sig3_cnt_1);
//					}
//					else
//					{
//						AirSenPara.sig3_cnt_1 = 0;
//					}
					
					//3、吸液时吸空
					if(curve_rake_ratio <= (-AirSenPara.AspLiqNoise))
					{
						Accumulation8(&AirSenPara.sig3_cnt_2);
					}
					else
					{
						AirSenPara.sig3_cnt_2 = 0;
					}
					
					
				}
			}
			AirSenPara.AtmosAbsAve = AirSenPara.Atmos / set_comp_num2;
			
			
			
			
			//1、启动吸气
//			AirSenPressState2(&AirSenPara.AbsState1_1, AirSenPara.sig1_cnt_1, AIRPRESS_PASS_1);
			
			//2、探测无气泡液面
			AirSenPressState2(&AirSenPara.AbsState1_3, AirSenPara.sig1_cnt_3, AIRPRESS_PASS_1);
//			AirSenPressState2(&AirSenPara.AbsState1_4, AirSenPara.sig1_cnt_4, AIRPRESS_PASS_3);
			
			//3、探测有气泡液面
//			AirSenPressState2(&AirSenPara.AbsState1_5, AirSenPara.sig1_cnt_5, AIRPRESS_PASS_6);
			AirSenPressState2(&AirSenPara.AbsState1_6, AirSenPara.sig1_cnt_6, AIRPRESS_PASS_6);
			
			
			//4.探测吸液堵塞
			//曲线大于吸液阈值
			AirSenPressState2(&AirSenPara.AbsState2_1, AirSenPara.sig2_cnt_1, AIRPRESS_PASS_2);
			
			
			//3.吸液吸空
			//(1)部分行程吸空
//			AirSenPressState2(&AirSenPara.AbsState3_1, AirSenPara.sig3_cnt_1, AIRPRESS_PASS_3);
			
			//(2)吸液时吸空
			AirSenPressState2(&AirSenPara.AbsState3_2, AirSenPara.sig3_cnt_2, AIRPRESS_PASS_3);
			
			
			//汇总
			AirSenCheckAll(&AirSenPara, UserPara[LLD_SEN_FUN].Value);
			
			
			
			
			
			if(PLLD_STAGE_ABS_BLOCK == AirSenPara.AirPress.Stage)
			{
				AirSenPara.LLDStage = LLD_STAGE_ABS_BLOCK;
			}
			else if(PLLD_STAGE_ABS_AIR == AirSenPara.AirPress.Stage)
			{
				AirSenPara.LLDStage = LLD_STAGE_ABS_AIR;
			}
			else if(PLLD_STAGE_LIQ == AirSenPara.AirPress.Stage)
			{
				AirSenPara.LLDStage = LLD_STAGE_LIQ;
			}
			
			
			
			
			//开启气压探测，才需要往下走
			//这动作十分重要，否则上电后会误判
			if(LLD_CLOSE == UserPara[LLD_SEN_FUN].Value)
			{
//				AirSenPara.CommAckIsBlock = FALSE;
//				AirSenPara.ABS_Start_End = PLLD_ABS_IDLE;
				AirSenPara.ABS_Real_Start_End = PLLD_ABS_IDLE;
				AirSenPara.LLDResult = LLD_IDLE;
				
				
				AirSenPara.LLDStage = LLD_STAGE_NULL;
			}
		}
		break;
		
		
		case LLD_STAGE_CHECK_FAIL:
		{
			AirSenCheckAll(&AirSenPara, UserPara[LLD_SEN_FUN].Value);
			
			
			if(LLD_CLOSE == UserPara[LLD_SEN_FUN].Value)
			{
//				AirSenPara.CommAckIsBlock = FALSE;
//				AirSenPara.ABS_Start_End = PLLD_ABS_IDLE;
				AirSenPara.ABS_Real_Start_End = PLLD_ABS_IDLE;
				AirSenPara.LLDResult = LLD_IDLE;
				
				
				AirSenPara.LLDStage = LLD_STAGE_NULL;
			}
		}
		break;
		
		case LLD_STAGE_LIQ:
		case LLD_STAGE_FAIL_AIR:
		case LLD_STAGE_BUBBLE:
		case LLD_STAGE_ABS_BLOCK:
		case LLD_STAGE_ABS_AIR:
		case LLD_STAGE_COMPLETE:
		{
			if(LLD_CLOSE == UserPara[LLD_SEN_FUN].Value)
			{
//				AirSenPara.CommAckIsBlock = FALSE;
//				AirSenPara.ABS_Start_End = PLLD_ABS_IDLE;
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
	int16_t     press_dis = 0;
//	int64_t     temp_data = 0;	
	
	
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
////		press_data = LPF1(press_data, AirSenPara.ChColl[PLLD_CH0_DATA].FilterValue, 0.3);
////		AirSenPara.ChColl[PLLD_CH0_DATA].FilterValue = press_data;
//		SmoothPipeline16(AirSenPara.ChColl[PLLD_CH0_DATA].OrigData, press_data, PLLD_NUM);
		SmoothPipeline16(AirSenPara.ChColl[PLLD_CH0_DATA].FilterData, press_data, PLLD_NUM);
		
		
#ifdef PLLD_DEBUG
		if(PLLD_ABS_START == AirSenPara.ABS_Real_Start_End)
		{
			AirSenPara.ChColl[PLLD_CH0_DATA].OrigValue = press_data + 500;
		}
		else
		{
			AirSenPara.ChColl[PLLD_CH0_DATA].OrigValue = press_data;
		}
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
		
//		//计算压力值：放大100倍；量程10 inH2O；差分型号需要乘上2；气孔装反，乘上-1;将压力转为Pa，乘上249.082。
//		temp_calc = 8192 - (int16_t)AirSenPara.ChColl[PLLD_CH0_DATA].OrigValue;
////		temp_calc = 1.25 * 100 * 10 * 2 * temp_calc;
//		temp_calc = 2500 * temp_calc;
//		
//		//将气压单位inH2O转为帕斯卡Pa
//		//1 inH2O = 249.08891 Pa
//		temp_calc = temp_calc * 249088 / 1000;
//		
////		temp_calc = temp_calc / 16384;
//		AirSenPara.ChColl[PLLD_CH0_DATA].Value = temp_calc>>14;
		
		
		
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
 * @function: AirPreventDripping
 * @details : 防止滴液
 * @input   : NULL
 * @output  : NULL
 * @return  : 电机执行动作结果
 */
uint8_t AirPreventDripping(void)
{
	uint8_t     result = 0;
	int8_t      step = 0;
	int16_t     temp = 0;
	int32_t     tarstep;
	
	
	
//	ErrorType_e eError = ERROR_TYPE_SUCCESS;
	TMC_e eTMC = TMC_0;
	extern TMCStatus_t g_tTMCStatus;
	
	
	if(PLLD_AGA_DRIPPING != UserPara[LLD_SEN_FUN].Value)
	{
		return (result);
	}
		
	
	//到了调节周期
	if(AirSenPara.DrippingTime >= AirSenPara.DrippingPeriod)
	{
		AirSenPara.DrippingTime = 0;
		
		
		//读取电机是否走完当前步数
		result = TMC5160_FIELD_READ(TMC_0, TMC5160_RAMPSTAT, TMC5160_RAMPSTAT_POS_REACH_MASK, TMC5160_RAMPSTAT_POS_REACH_SHIFT);
		if(0x01 == result)
		{
			//读取目标位置
//			tarstep = TMC5160_ReadInt(eTMC, TMC5160_XTARGET);
			
			//读取当前位置
			tarstep = TMC5160_ReadInt(eTMC, TMC5160_XACTUAL);
			
			
			temp = AirSenPara.ChColl[PLLD_CH0_DATA].OrigValue - AirSenPara.DrippingTarAirPress;
			
			if(temp >= (4 * AirSenPara.DrippingErr))
			{
				step = AirSenPara.DrippingStep * (-3);
			}
			else if(temp >= (2 * AirSenPara.DrippingErr))
			{
				step = AirSenPara.DrippingStep * (-2);
			}
			else if(temp >= AirSenPara.DrippingErr)
			{
				step = AirSenPara.DrippingStep * (-1);
			}
			else if(temp <= (-4 * AirSenPara.DrippingErr))
			{
				step = AirSenPara.DrippingStep * 3;
			}
			else if(temp <= (-2 * AirSenPara.DrippingErr))
			{
				step = AirSenPara.DrippingStep * 2;
			}
			else if(temp <= (-AirSenPara.DrippingErr))
			{
				step = AirSenPara.DrippingStep;
			}
			tarstep = tarstep + step;
			
			
			
			
			if(tarstep >= 0)
			{
				AirSenPara.DrippingTarStep = tarstep;
			}
		}
		
		
		//不在复位状态
		if(g_tTMCStatus.tMotorResetInfo[eTMC].eResetStatus != MOTOR_RESET_STATUS_ING)
		{
			//关闭编码器失步检测功能
			TMC_WriteInt(eTMC, TMC5160_ENC_DEVIATION, 0);
			
			//绝对移动
			result = TMC_MoveTo(eTMC, AirSenPara.DrippingTarStep);
			
			TMC_SetPMode_V(eTMC, 0);
		}
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
				
				//示波器抓取运行时间：46us
				AirSenConvertAD();
				AirSenDeteLiqLevel();
				
				
				//防滴液
				AirPreventDripping();
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
			AirSenPara.ComStage = CP_COMM_RESET_DEV;
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
	UserPara[AIR_ATMOS_NOISE].Value = 5;           //无吸气探测
	
	//探测液面
	//1、1000ul吸头，阈值7，3ul/s、6ul/s时出现误判
	//2、1000ul吸头，阈值13，3ul/s、6ul/s时，正常
	//3、50ul吸头，10ul/s吸气速度，噪声有10.
	UserPara[AIR_RAKERATIO1_MAX].Value = 30;       //吸气探测
	UserPara[AIR_RAKERATIO1_MIN].Value = 90;       //吸头扎入深度较深，吸头内部出现短暂正压。
	
	//探测液+气泡
	UserPara[AIR_RAKERATIO2_MAX].Value = 22;      //吸气触碰液面
	UserPara[AIR_RAKERATIO2_MIN].Value = 9;       //吸气触碰气泡
	
	//部分行程吸空/吸液时空吸、吸到凝块阈值/吸头堵塞、全程空吸
	UserPara[AIR_ASP_LIQ_NOISE].Value = 260;
	UserPara[AIR_RAKERATIO3_MAX].Value = 11300;
	UserPara[AIR_RAKERATIO3_MIN].Value = 50;
	
	
	//气压检测吸空，等待一段时间仍然没检测到吸液动作
	UserPara[AIR_ABS_WAIT_DLY].Value = 400;
	//P电机启动吸液时，气压跳动剧烈，延时一会
	UserPara[AIR_ABS_START_DLY].Value = 200;
	//P电机结束吸液时，气压跳动剧烈，延时一会
	UserPara[AIR_ABS_END_DLY].Value = 75;
	
	
	//气压某通信延时n * 2ms
//	AirSenPara.CommAckBlockDly = 15;
	
	
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
//	AirSenPara.ChColl[PLLD_CH0_DATA].OrigData = AirSenPara.AppOrigBuff;
	AirSenPara.ChColl[PLLD_CH0_DATA].FilterData = AirSenPara.AppFilterBuff;
	
	
	AirSenPara.AtmosRef = AIR_SEN_ZERO_PRESS;
//	AirSenPara.AtmosRefUpdate = FALSE;

	//P轴电机，吸液速度高达1000ul/s，脉冲值62930‬，1ms可运行62步数.
	//气压传感器采集数据周期典型值为1.3ms~3.1ms
	//气压传感器噪声约为6，因此允许误差设置为6.
	AirSenPara.DrippingPeriod = 10;
	AirSenPara.DrippingErr = 6;
	AirSenPara.DrippingStep = 2;
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
	uint8_t     result1 = FALSE;
	uint8_t     result2 = FALSE;
	uint8_t     index1 = 0xFF;
	uint8_t     index2 = 0xFF;
	uint16_t    w_buf[4] = {0};
	
	
	w_buf[2] = MERGE_UINT16(buf[4], buf[5]);
	w_buf[3] = MERGE_UINT16(buf[6], buf[7]);
	
	if(0 == num)
	{
		//设置阈值
		index1 = CAP_CON_THRESHOLD_H;
		index2 = CAP_CON_THRESHOLD_L;
	}
	else if(1 == num)
	{
		//设置驱动电流
		index1 = CAP_DRIVE_CURRENT_CH0;
	}
	else if(2 == num)
	{
		//设置转换时间
		index1 = CAP_SET_CONTIME;
	}
	
	
	
	if(0 == num)
	{
		if((index1 < USER_PARA_NUM)
			&&(index2 < USER_PARA_NUM))
		{
			result1 = SetPara16(&UserPara[index1], w_buf[2]);
			if(TRUE == result1)
			{
				ret = TRUE;
			}
			
			result2 = SetPara16(&UserPara[index2], w_buf[3]);
			if(TRUE == result2)
			{
				ret = TRUE;
			}
			
		
			if(FALSE == ret)
			{
				//参数范围不做检查
				CapSenPara.ContactThreshold = UserPara[CAP_CON_THRESHOLD_H].Value<<16;
				CapSenPara.ContactThreshold = CapSenPara.ContactThreshold | UserPara[CAP_CON_THRESHOLD_L].Value;
				CapSenPara.LeaveThreshold = CapSenPara.ContactThreshold;
			}
		}
	}
	else
	{
		if(index1 < USER_PARA_NUM)
		{
			result1 = SetPara16(&UserPara[index1], w_buf[3]);
			if(TRUE == result1)
			{
				ret = TRUE;
			}
		}
	}
	
	
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
	uint8_t     ret = FALSE;
	uint8_t     index1 = 0xFF;
	uint8_t     index2 = 0xFF;
	
	
	if(0 == num)
	{
		//设置阈值
		index1 = CAP_CON_THRESHOLD_H;
		index2 = CAP_CON_THRESHOLD_L;
		
		buf[4] = UserPara[index1].Value>>8;
		buf[5] = UserPara[index1].Value;
		buf[6] = UserPara[index2].Value>>8;
		buf[7] = UserPara[index2].Value;
	}
	else if(1 == num)
	{
		//设置驱动电流
		index1 = CAP_DRIVE_CURRENT_CH0;
	}
	else if(2 == num)
	{
		//设置转换时间
		index1 = CAP_SET_CONTIME;
	}
	
	
	if((num >= 1)
		&& (num < USER_PARA_NUM))
	{
		buf[6] = UserPara[index1].Value>>8;
		buf[7] = UserPara[index1].Value;
	}
	
	
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
	temp = CapSenPara.ChColl[CLLD_CH1_DATA].FilterValue;
	
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
//		AirSenPara.ABS_Start_End = UserPara[index].Value;
//		
//		
//		if(PLLD_ABS_END == AirSenPara.ABS_Start_End)
//		{
//			//阻塞应答
//			AirSenPara.CommAckIsBlock = TRUE;
//		}
	}
	else if(1 == num)
	{
//		AirSenPara.Dis_Start_End = UserPara[index].Value;
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
	switch(UserPara[LLD_SEN_FUN].Value)
	{
		case CLLD_RT_LIQUL:
		case LLD_CAP_RT_AIR_LIQUL:
		case CLLD_ST_LIQUL:
		case LLD_CAP_ST_AIR_LIQUL:
		case CLLD_RT_TWO_DETE:
		case CLLD_ST_TWO_DETE:
		{
			//1、收到开启探测指令
			//2、再次收到开启探测指令
			CapSenPara.IsClearBuff = TRUE;
		}
		break;
		
		case PLLD_AGA_DRIPPING:
		{
			AirSenPara.DrippingTarAirPress = AirSenPara.ChColl[PLLD_CH0_DATA].OrigValue;
		}
		break;
		
		default:
		{
		}
		break;
	}
	
	
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
		
		case CLLD_RT_LIQUL:
		case CLLD_ST_LIQUL:
		case CLLD_BUBBLE:
//		case CLLD_ABS_BLOCK:
//		case CLLD_ABS_AIR:
//		case CLLD_ABS_BLOCK_FIXED_AIR:
//		case CLLD_DIS_BLOCK:
//		case CLLD_DIS_AIR:
//		case CLLD_RT_TWO_DETE:
//		case CLLD_ST_TWO_DETE:
		{
			if(LLD_LIQUL == CapSenPara.LLDResult)
			{
				lld_output = IO_HIGH;
			}
			
			LLDMan.Result = CapSenPara.LLDResult;
		}
		break;
		
		case LLD_CAP_RT_AIR_LIQUL:
		case LLD_CAP_ST_AIR_LIQUL:
		{
			//电容探测成功
			if(LLD_LIQUL == CapSenPara.LLDResult)
			{
				lld_output = IO_HIGH;
			}
			
			
			if((LLD_LIQUL == CapSenPara.LLDResult)
				&& (LLD_LIQUL == AirSenPara.LLDResult))
			{
				//电容、气压探测都成功
				LLDMan.Result = LLD_LIQUL;
			}
			else if(LLD_FAIL_AIR == AirSenPara.LLDResult)
			{
				//气压确认电容探测出现悬空误判故障
				LLDMan.Result = LLD_FAIL_AIR;
			}
			else
			{
				LLDMan.Result = LLD_IDLE;
			}
		}
		break;
		
		case PLLD_NO_AIR_LIQUL:
		case PLLD_LOW_SPEED_LIQUL:
		case PLLD_BUBBLE:
		{
			if(LLD_LIQUL == AirSenPara.LLDResult)
			{
				lld_output = IO_HIGH;
			}
			
			LLDMan.Result = AirSenPara.LLDResult;
		}
		break;
		
		case PLLD_ABS_BLOCK:
		case PLLD_ABS_FOLLOW_AIR:
		case PLLD_ABS_BLOCK_FOLLOW_AIR:
		{
			LLDMan.Result = AirSenPara.LLDResult;
		}
		break;
		
		case PLLD_AGA_DRIPPING:
		{
		}
		break;
		
		default:
		{
			UserPara[LLD_SEN_FUN].Value = LLD_CLOSE;
		}
		break;
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
	
	
	//探测线程周期
	SoftSys.LLDPeriod = 2;
}


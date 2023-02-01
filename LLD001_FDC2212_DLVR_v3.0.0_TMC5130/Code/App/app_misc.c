/*****************************************************************************
Copyright  : BGI
File name  : app_misc.c
Description: 乱七八糟的东西
Author     : lmj
Version    : 0.0.0.1
Date       : 完成日期
History    : 修 改 历 史 记 录 列 表 ， 每 条 修 改 记 录 应 包 括 修 改 日 期 、 修 改
者及修改内容简述。
*****************************************************************************/
#include "app_misc.h" 
#include "include.h"



//定义变量---------------------------------------------------------------------//
struct tagSoftSys    SoftSys;
struct tagMotorCtr   MotorCtr;




//定义函数---------------------------------------------------------------------//
/*
 * @function: ReadSoftWareNameIns
 * @details : 写软件名称
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @return  : 0：成功；1：参数错误。
 */
uint8_t ReadSoftWareNameIns(uint8_t *buf, uint8_t num)
{
	uint16_t    ret = FALSE;
	
	
	SoftSys.SoftWare.InsCtrl = buf[3];
	
	
	return (ret);
}


/*
 * @function: ReadSoftWareName
 * @details : 软件名称
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @return  : 0：成功；1：参数错误。
 */
uint8_t ReadSoftWareName(uint8_t *buf, uint8_t num)
{
	uint8_t    inc = 0;
	uint8_t    offset = 0;
	uint8_t    dat = 0;
	uint8_t    ret = 0;
	
	
	if(INS_RESET_NAME == SoftSys.SoftWare.InsCtrl)
	{
		SoftSys.SoftWare.NameSegNo = 0;
		
		//上位机发送重置指令
		SoftSys.SoftWare.InsResult = INS_RESET_NAME;
	}
	else if(INS_READ_SW_NAME == SoftSys.SoftWare.InsCtrl)
	{
		//检查读取程序名称是否超范围，一次数据帧最多只能读4字节。
		if((SoftSys.SoftWare.NameSegNo * 4) >= SOFTWARE_NAME_LEN)
		{
			SoftSys.SoftWare.InsResult = INS_READ_SW_OUTLEN;
		}
		else
		{
			offset = SoftSys.SoftWare.NameSegNo * 4;
			for(inc = 0; inc < 4; inc++)
			{
				dat = SoftWareName[offset + inc];
				buf[4 + inc] = dat;
				
				//读完字符串
				if('\0' == dat)
				{
					break;
				}
			}
			SoftSys.SoftWare.NameSegNo++;
			
			
			if('\0' == dat)
			{
				SoftSys.SoftWare.NameSegNo = 0;
				
				//读完字符串
				SoftSys.SoftWare.InsResult = INS_READ_SW_FINISH;
			}
			else
			{
				//继续读取
				SoftSys.SoftWare.InsResult = INS_READ_SW_CONTINUE;
			}
		}
	}
	
	buf[3] = SoftSys.SoftWare.InsResult;
	
	
	
	return (ret);
}


/*
 * @function: ReadSoftWareVer
 * @details : 读取软件版本
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @return  : 0：成功；1：参数错误。
 */
uint8_t ReadSoftWareVer(uint8_t *buf, uint8_t num)
{
	uint8_t    ret = 0;
	
	
	buf[4] = SoftWareVer[0];
	buf[5] = SoftWareVer[1];
	buf[6] = SoftWareVer[2]>>8;
	buf[7] = SoftWareVer[2];
	
	
	return (ret);
}



/*
 * @function: MotorMoveUpAndDownTest
 * @details : 电机上下运动。
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @return  : 0：成功；1：参数错误。
 */
//uint8_t MotorMoveUpAndDownTest(void)
//{
//	uint8_t    ret = 0;
//	
//	
//	switch(MotorCtr.RunState)
//	{
//		case MOTOR_STAGE_NULL:
//		{
//			if(MotorCtr.TarPulse > 0)
//			{
//				MotorCtr.RunState = MOTOR_STAGE_ACCE;
//			}
//		}
//		break;
//		
//		case MOTOR_STAGE_ACCE:
//		{
//			MotorCtr.TrigPWM = 1;
//			MotorCtr.RunPulse = MotorCtr.TarPulse;
//			
//			
//			MotorCtr.RunState = MOTOR_STAGE_RUN;
//		}
//		break;
//		
//		case MOTOR_STAGE_RUN:
//		{
//			//触发输出PWM
//			if(1 == MotorCtr.TrigPWM)
//			{
//				MotorCtr.TrigPWM = 0;
//				TIM_Cmd(TIM3, ENABLE);
//			}
//			
//			
//			if(0 == MotorCtr.RunPulse)
//			{
//				MotorCtr.RunState = MOTOR_STAGE_DECE;
//			}
//		}
//		break;
//		
//		case MOTOR_STAGE_DECE:
//		{
//			//换向
//			if(1 == MotorCtr.Dir)
//			{
//				GPIO_SetBits(GPIOB, GPIO_Pin_4);
//				MotorCtr.Dir = 0;
//			}
//			else
//			{
//				GPIO_ResetBits(GPIOB, GPIO_Pin_4);
//				MotorCtr.Dir = 1;
//			}
//			
//			
//			MotorCtr.RunState = MOTOR_STAGE_NULL;
//		}
//		break;
//		
//		default:
//		{
//			MotorCtr.RunState = MOTOR_STAGE_NULL;
//		}
//		break;
//	}
//	
//	
//	return (ret);
//}
/*
 * @function: MotorMoveUpAndDown
 * @details : 电机上下运动。
 * @input   : 1.buf：缓存。
              2.num：编号。
 * @output  : NULL
 * @return  : 0：成功；1：参数错误。
 */
uint8_t MotorMoveUpAndDown(void)
{
	uint8_t    ret = 0;
	
	
	switch(MotorCtr.RunState)
	{
		case MOTOR_STAGE_NULL:
		{
			//判断转向
			if(MotorCtr.TarPulse > MotorCtr.CurPulse)
			{
				MotorCtr.Dir = 0;
				MotorCtr.RunPulse = MotorCtr.TarPulse - MotorCtr.CurPulse;
			}
			else if(MotorCtr.TarPulse < MotorCtr.CurPulse)
			{
				MotorCtr.Dir = 1;
				MotorCtr.RunPulse = MotorCtr.CurPulse - MotorCtr.TarPulse;
			}
			
			
			if(MotorCtr.TarPulse !=  MotorCtr.CurPulse)
			{
				MotorCtr.RunState = MOTOR_STAGE_ACCE;
			}
		}
		break;
		
		case MOTOR_STAGE_ACCE:
		{
			MotorCtr.TrigPWM = 1;
			
			//换向
			if(0 == MotorCtr.Dir)
			{
				GPIO_SetBits(GPIOB, GPIO_Pin_4);
			}
			else
			{
				GPIO_ResetBits(GPIOB, GPIO_Pin_4);
			}
			
			
			MotorCtr.RunState = MOTOR_STAGE_RUN;
		}
		break;
		
		case MOTOR_STAGE_RUN:
		{
			//触发输出PWM
			if(1 == MotorCtr.TrigPWM)
			{
				MotorCtr.TrigPWM = 0;
				TIM_Cmd(TIM3, ENABLE);
			}
			
			
			if(0 == MotorCtr.RunPulse)
			{
				MotorCtr.RunState = MOTOR_STAGE_NULL;
			}
		}
		break;
		
		case MOTOR_STAGE_DECE:
		{
			MotorCtr.RunState = MOTOR_STAGE_NULL;
		}
		break;
		
		default:
		{
			MotorCtr.RunState = MOTOR_STAGE_NULL;
		}
		break;
	}
	
	
	return (ret);
}


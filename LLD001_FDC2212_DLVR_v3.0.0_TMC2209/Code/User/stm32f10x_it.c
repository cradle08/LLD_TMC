/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTI
  
  AL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "bsp_timer.h"
#include "monitor_can.h" 
#include "monitor_usart.h" 
#include "gpio.h"



/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
//void HardFault_Handler(void)
//{
//  /* Go to infinite loop when Hard Fault exception occurs */
//  while (1)
//  {
//  }
//}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
//void PendSV_Handler(void)
//{
//}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    //进入中断
    rt_interrupt_enter();

    //更新时基
    rt_tick_increase();

    //离开中断
    rt_interrupt_leave();
}

/**
  * @brief  This function handles ADC Handler.
  * @param  None
  * @retval None
  */
void ADC1_2_IRQHandler(void)
{
	rt_interrupt_enter(); //通知操作系统此时进入中断状态
	
	if (ADC_GetITStatus(ADC1, ADC_IT_EOC)==SET) 
	{
		// 读取ADC的转换值
		TempSensor[TEMP_PIPE_IN].AirPump.OrigValue = ADC_GetConversionValue(ADC1);
		
		TempSensor[TEMP_PIPE_IN].UpdateFlag = TRUE;
	}
	ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
	
	rt_interrupt_leave();//通知操作系统此时离开中断状态
}

/**
  * @brief  This function handles TIM2 interrupt request.
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{
	rt_interrupt_enter(); //通知操作系统此时进入中断状态
	
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) 
	{
		TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update); 

		SWSysTimerAdd();
		BSP_UartCommTimeHandler(&ModbusMon.Usart);
	}
	
	rt_interrupt_leave();//通知操作系统此时离开中断状态
}

/**
  * @brief  This function handles TIM3 interrupt request.
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)
{
	rt_interrupt_enter(); //通知操作系统此时进入中断状态
	
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) 
	{
		//运行脉冲计数。
		if(MotorCtr.RunPulse > 0)
		{
			MotorCtr.RunPulse--;
			
			if(1 == MotorCtr.Dir) 
			{
				if(MotorCtr.CurPulse > 0)
				{
					MotorCtr.CurPulse--;
				}
				else
				{
					//报错
				}
			}
			else if(0 == MotorCtr.Dir) 
			{
				MotorCtr.CurPulse++;
			}
		}
		else
		{
			MotorTimerPWMStop();
		}


		
		TIM_ClearITPendingBit(TIM3, TIM_FLAG_Update); 
	}
	
	rt_interrupt_leave();//通知操作系统此时离开中断状态
}





/**
  * @brief  This function handles USART2 interrupt request.
  * @param  None
  * @retval None
  */
void USART2_IRQHandler(void)
{
	rt_interrupt_enter(); //通知操作系统此时进入中断状态
	
	uint8_t ucTemp;
	
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{		
		ucTemp = USART_ReceiveData(USART2);
		
		UsartIRQReceiveHandler(&ModbusMon.Usart, ucTemp);
	}
	
	rt_interrupt_leave();//通知操作系统此时离开中断状态
}


/**
  * @brief  This function handles CAN Rx interrupt request.
  * @param  None
  * @retval None
  */
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	rt_interrupt_enter(); //通知操作系统此时进入中断状态
	
	//自研三轴板出现过异常
//	if(SET == CAN_GetITStatus(CAN1, CAN_IT_FMP0))
//	{
//		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
//	}
//	if(SET == CAN_GetITStatus(CAN1, CAN_IT_FF0))
//	{
//		CAN_ClearITPendingBit(CAN1, CAN_IT_FF0);
//	}
//	if(SET == CAN_GetITStatus(CAN1, CAN_IT_FOV0))
//	{
//		CAN_ClearITPendingBit(CAN1, CAN_IT_FOV0);
//	}
//	if(SET == CAN_GetITStatus(CAN1, CAN_IT_FMP1))
//	{
//		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP1);
//	}
//	if(SET == CAN_GetITStatus(CAN1, CAN_IT_FF1))
//	{
//		CAN_ClearITPendingBit(CAN1, CAN_IT_FF1);
//	}
//	if(SET == CAN_GetITStatus(CAN1, CAN_IT_FOV1))
//	{
//		CAN_ClearITPendingBit(CAN1, CAN_IT_FOV1);
//	}
	
	CanIRQRecv();
	
	rt_interrupt_leave();//通知操作系统此时离开中断状态
}


/**
  * @brief  This function handles EXTI4 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI4_IRQHandler(void)
{
	rt_interrupt_enter(); //通知操作系统此时进入中断状态
	
	//确保是否产生了EXTI Line中断
	if(RESET != EXTI_GetITStatus(EXTI_Line4)) 
	{
		CapSenPara.Rdy = TRUE;
		
		//清除中断标志位
		EXTI_ClearITPendingBit(EXTI_Line4);     
	}
	
	rt_interrupt_leave();//通知操作系统此时离开中断状态
}




/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

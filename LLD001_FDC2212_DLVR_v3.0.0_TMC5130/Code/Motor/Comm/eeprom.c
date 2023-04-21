#include "main.h"
#include "bsp_spi.h"
#include "m95xxx.h"
#include "eeprom.h"
#include "log.h"
//#include "bsp_outin.h"
#include "project_config.h"



//EEPROM 
EEPROM_t g_tEEPROM = {0};




/*
*	EEPROM�ṹ��
*/
void EEPROM_Init(void)
{

#if (CURRENT_USE_EEPROM_TYPE == EEPROM_TYPE_M95xxx)
	
	g_tEEPROM.ucEepromType 	 	= EEPROM_TYPE_M95xxx;
	g_tEEPROM.ucStatus    	 	= 0;
	g_tEEPROM.usVolume			= 8120;	//8KByte
	g_tEEPROM.usPageSize		= 32;	//32Byte
	g_tEEPROM.pfWrite     		= M95xxx_Write;
	g_tEEPROM.pfRead			= M95xxx_Read;
	g_tEEPROM.pfCS			 	= M95640_CS;
	g_tEEPROM.pfDelay		 	= M95xxx_Delay;
	g_tEEPROM.pfWriteProtect 	= M95xxx_WP;
	g_tEEPROM.pfHold		 	= M95xxx_Hold;
	g_tEEPROM.pfStatusRegister 	= M95xxx_StatusRegister;
	
#elif  (CURRENT_USE_EEPROM_TYPE == EEPROM_TYPE_AT25640)
	//
	g_tEEPROM.ucEepromType 	 	= EEPROM_TYPE_AT25640;
	g_tEEPROM.ucStatus    	 	= 0;
	g_tEEPROM.usVolume			= 4096;	//8KByte
	g_tEEPROM.usPageSize		= 32;	//32Byte
	g_tEEPROM.pfWrite     		= M95xxx_Write;
	g_tEEPROM.pfRead			= M95xxx_Read;
	g_tEEPROM.pfCS			 	= EEPROM_CS;
	g_tEEPROM.pfDelay		 	= M95xxx_Delay;
	g_tEEPROM.pfWriteProtect 	= M95xxx_WP;
	g_tEEPROM.pfHold		 	= M95xxx_Hold;
	g_tEEPROM.pfStatusRegister 	= M95xxx_StatusRegister;
	
#endif
	
}




/*
*	EEPROM write
*/
void EEPROM_Write(uint16_t usAddr, __IO uint8_t *pucData, uint16_t usLen)
{
	
	uint8_t ucInPageAddr = 0, ucWritePageNum = 0, i = 0, ucLessSize = 0;
	__IO uint8_t *pData = NULL;
	uint16_t usWriteAddr = usAddr;
	const uint8_t c_ucPageSize = g_tEEPROM.usPageSize;
	
	//�������
	if(pucData == NULL || usLen == 0 || usAddr+usLen > g_tEEPROM.usVolume) 
	{
		LOG_Error("Param Error");
		return;
	}
	
	pData = pucData;
	//page addr
	ucInPageAddr = usAddr%c_ucPageSize;
	//ucPageIndex = usTempAddr/c_ucPageSize;
	if(ucInPageAddr == 0)
	{
		//д����ʼ��ַΪ������ҳ��ʼ��ַ
		ucWritePageNum = usLen/c_ucPageSize;
		
		//write full page
		for(i = 0; i < ucWritePageNum; i++)
		{
			g_tEEPROM.pfWrite(usWriteAddr, pData, c_ucPageSize);
			usWriteAddr += c_ucPageSize;
			pData 		+= c_ucPageSize;
		}
		
		
		//the part less than full page
		ucLessSize = usLen%c_ucPageSize;
		if(ucLessSize > 0)
		{
			g_tEEPROM.pfWrite(usWriteAddr, pData, ucLessSize);
		}

	}else{
		//д����ʼ��ַΪ����������ҳ��ʼ��ַ
		
		//��һҳʣ�೤��
		uint8_t ucFirstParttLen = c_ucPageSize - ucInPageAddr;
		if(usLen <= ucFirstParttLen)
		{
			//��һ��ʣ�೤�ȣ��㹻�洢����
			g_tEEPROM.pfWrite(usWriteAddr, pData, usLen);
			
		}else{
		
			//��һ��ʣ�೤�ȣ������洢����
			g_tEEPROM.pfWrite(usWriteAddr, pData, ucFirstParttLen);
			usWriteAddr += ucFirstParttLen;
			pData       += ucFirstParttLen;
			
			//�ڶ��� ���㣬����Ҫ���ٸ�ҳ�洢
			ucWritePageNum = (usLen-ucFirstParttLen)/c_ucPageSize;
			if(ucWritePageNum >= 1)
			{
				for(i = 0; i < ucWritePageNum; i++)
				{
					g_tEEPROM.pfWrite(usWriteAddr, pData, c_ucPageSize);
					usWriteAddr += c_ucPageSize;
					pData 		+= c_ucPageSize;
				}
			}
			
			//�����Σ������β��
			uint8_t ucLastLen = (usLen-ucFirstParttLen)%c_ucPageSize;
			if(ucLastLen > 0)
			{
				//�������ݣ���Ҫ�洢
				g_tEEPROM.pfWrite(usWriteAddr, pData, ucLastLen);
			}
		}
	}
}




/*
*	EEPROM read
*/
void EEPROM_Read(uint16_t usAddr, __IO uint8_t *pucData, uint16_t usLen)
{
	uint8_t ucInPageAddr = 0, ucReadPageNum = 0, i = 0, ucLessSize = 0;
	__IO uint8_t *pData = NULL;
	uint16_t usReadAddr = usAddr;
	const uint8_t c_ucPageSize = g_tEEPROM.usPageSize;
	
	
	//�������
	if(pucData == NULL || usLen == 0 || usAddr+usLen > g_tEEPROM.usVolume) 
	{
		LOG_Error("Param Error");
		return;
	}
	
	pData = pucData;
	//in page addr
	ucInPageAddr = usReadAddr%c_ucPageSize;
	if(ucInPageAddr == 0)
	{
		//read��ʼ��ַΪ������ҳ��ʼ��ַ
		ucReadPageNum = usLen/c_ucPageSize;
		
		//read
		for(i = 0; i < ucReadPageNum; i++)
		{
			g_tEEPROM.pfRead(usReadAddr, pData, c_ucPageSize);
			usReadAddr += c_ucPageSize;
			pData 	   += c_ucPageSize;
		}

		
		//read last part, less than page
		ucLessSize = usLen%c_ucPageSize;
		if(ucLessSize > 0)
		{
			g_tEEPROM.pfRead(usReadAddr, pData, ucLessSize);
		}
	
	}else{
		//read��ʼ��ַΪ����������ҳ��ʼ��ַ
		
		//��һҳʣ�೤��
		uint8_t ucFirstParttLen = c_ucPageSize - ucInPageAddr;
		if(usLen <= ucFirstParttLen)
		{
			//��һ�γ��ȣ���ȡ����
			g_tEEPROM.pfRead(usReadAddr, pData, usLen);
			//usWriteAddr += ucFirstParttLen;
			
		}else{
		
			//��һ��ʣ�೤�ȣ�������
			g_tEEPROM.pfRead(usReadAddr, pData, ucFirstParttLen);
			usReadAddr += ucFirstParttLen;
			pData       += ucFirstParttLen;
			
			//�ڶ��� ���㣬����Ҫ���ٸ�ҳ�洢
			ucReadPageNum = (usLen-ucFirstParttLen)/c_ucPageSize;
			if(ucReadPageNum >= 1)
			{
				for(i = 0; i < ucReadPageNum; i++)
				{
					g_tEEPROM.pfRead(usReadAddr, pData, c_ucPageSize);
					usReadAddr += c_ucPageSize;
					pData 		+= c_ucPageSize;
				}
			}
			
			//�����Σ������β��
			uint8_t ucLastLen = (usLen-ucFirstParttLen)%c_ucPageSize;
			if(ucLastLen > 0)
			{
				//�������ݣ���Ҫ�洢
				g_tEEPROM.pfRead(usReadAddr, pData, ucLastLen);
			}
		}	
	}
}






/*
*    Param_Write
*/
void Param_Write(SaveParamType_e eType, __IO uint8_t *pucData, uint16_t usLen)
{
	rt_enter_critical();
	switch(eType)
	{
		case EN_SAVE_PARAM_TYPE_AXIS:
		{
			EEPROM_Write(AXIS_PARAM_SAVE_ADDR, pucData, usLen);
		}
		break;
		case EN_SAVE_PARAM_TYPE_GLOBAL:
		{
			EEPROM_Write(GLOBAL_PARAM_SAVE_ADDR, pucData, usLen);
		}
		break;	
		case EN_SAVE_PARAM_TYPE_PROCESS:
		{
			EEPROM_Write(PROCESS_SAVE_ADDR, pucData, usLen);
		}
		break;
		case EN_SAVE_PARAM_TYPE_LLD_PARAM:
		{
			EEPROM_Write(LLD_PARAM_SAVE_ADDR, pucData, usLen);
		}
		break;
		default:break;
	}
	rt_exit_critical();
}



/*
*   Param_read
*/
void Param_Read(SaveParamType_e eType, __IO uint8_t *pucData, uint16_t usLen)
{
	rt_enter_critical();
	switch(eType)
	{
		case EN_SAVE_PARAM_TYPE_AXIS:
		{
			EEPROM_Read(AXIS_PARAM_SAVE_ADDR, pucData, usLen);
		}
		break;
		case EN_SAVE_PARAM_TYPE_GLOBAL:
		{
			EEPROM_Read(GLOBAL_PARAM_SAVE_ADDR, pucData, usLen);
		}
		break;	
		case EN_SAVE_PARAM_TYPE_PROCESS:
		{
			EEPROM_Read(PROCESS_SAVE_ADDR, pucData, usLen);
		}
		break;
		case EN_SAVE_PARAM_TYPE_LLD_PARAM:
		{
			EEPROM_Read(LLD_PARAM_SAVE_ADDR, pucData, usLen);
		}
		break;
		default:break;
	}
	rt_exit_critical();

}













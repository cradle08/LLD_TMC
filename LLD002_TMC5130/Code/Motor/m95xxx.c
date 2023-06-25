#include "m95xxx.h"
//#include "bsp_outin.h"
#include "bsp_misc.h"
#include "bsp_spi.h"
#include "public.h"
#include "eeprom.h"




/*
*	 写，
*/
void M95xxx_Write(uint16_t usAddr, uint8_t *pucData, uint16_t usLen)
{
	extern EEPROM_t g_tEEPROM;
	uint8_t ucaData[4] = {0}, ucCount = 0;	
	
	/* write enable */
	ucaData[0] = EEPROM_CMD_WREN;
	//CS EN
	g_tEEPROM.pfCS(TRUE);
	EEPROM_SPI_WriteArr(&ucaData[0], 1);
	//HAL_SPI_Transmit(&hspi3, &ucaData[0], 1, 500);
	//CS Disable
	g_tEEPROM.pfCS(FALSE);
	
	
	ucaData[0] = EEPROM_CMD_WRITE;
	//CS EN
	g_tEEPROM.pfCS(TRUE);	
	
	/* write cmd */
	EEPROM_SPI_WriteArr(&ucaData[0], 1);

	/* write addr and data */
	ucaData[0] = usAddr >> 8;
	ucaData[1] = usAddr;
	EEPROM_SPI_WriteArr(ucaData, 2);
	
	//write data
	EEPROM_SPI_WriteArr(pucData, usLen); 
	//CS Disable
	g_tEEPROM.pfCS(FALSE);
	
	/* wait write finished */
	while(g_tEEPROM.pfStatusRegister() & 0x01)
	{
		g_tEEPROM.pfDelay(1);
		ucCount++;
		if(ucCount > 200) break;
		
	}
}



/*
*	读
*/
void M95xxx_Read(uint16_t usAddr, uint8_t *pucData, uint16_t usLen)
{
	extern EEPROM_t g_tEEPROM;
	uint8_t ucaData[4] = {0};
	
	
	
	//CS EN
	g_tEEPROM.pfCS(TRUE);
	
	/* write cmd */
	ucaData[0] = EEPROM_CMD_READ;
	EEPROM_SPI_WriteArr(&ucaData[0], 1);	
	
	/* write addr and read data */
	ucaData[0] = usAddr >> 8;
	ucaData[1] = usAddr;
	//write addr
	EEPROM_SPI_WriteArr(&ucaData[0], 2);
	
	/* read data */
	EEPROM_SPI_ReadArr(pucData, usLen);
	
	//CS Disable
	g_tEEPROM.pfCS(FALSE);	
}




/*
*	ms延时
*/
void M95xxx_Delay(uint32_t ulTick)
{
	//HAL_Delay(ulTick);
	Delay_US(500);

}




/*
*	M95320 片选
*/
void M95320_CS(Bool_e eBool)
{
	if(TRUE == eBool)
	{
		EEPROM_CS_LOW;
	
	}else{
		EEPROM_CS_HIGH;

	}
}




/*
*	M95640 片选
*/
void M95640_CS(Bool_e eBool)
{
	if(TRUE == eBool)
	{
		EEPROM_CS_LOW;
	}else{
		EEPROM_CS_HIGH;
	}

}






/*
*	写保护
*/
void M95xxx_WP(void)
{
	

}




/*
*	保持，和主机通讯中断
*/
void M95xxx_Hold(void)
{
	

}





/*
*	获取状态，寄存器RDSR
*/
uint8_t M95xxx_StatusRegister(void)
{
	
	extern EEPROM_t g_tEEPROM;
	uint8_t ucCMD = EEPROM_CMD_RDSR, ucStatus = 0;
	
	/* write cmd */
	//CS EN
	g_tEEPROM.pfCS(TRUE);
	
	//cmd
	EEPROM_SPI_WriteArr(&ucCMD, 1);
	//get status
	EEPROM_SPI_ReadArr(&ucStatus, 1);
	
	//CS Disable
	g_tEEPROM.pfCS(FALSE);	
	
	return ucStatus;
}











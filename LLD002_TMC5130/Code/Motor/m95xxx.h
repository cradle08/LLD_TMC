#ifndef __M95XXX_H__
#define __M95XXX_H__

#include "main.h"
#include "bsp_spi.h"
#include "public.h"



#define EEPROM_CMD_WREN		0x06	//set write enabel
#define EEPROM_CMD_WRDI		0x04	//reset write enable 
#define EEPROM_CMD_RDSR		0x05	//read status
#define EEPROM_CMD_WRSR		0x01 	//write status
#define EEPROM_CMD_WRITE	0x02	//write
#define EEPROM_CMD_READ		0x03 	//read
#define EEPROM_CMD_RDID		0x83	//read id
#define EEPROM_CMD_WRID		0x82 	//write id




void M95xxx_Write(uint16_t usAddr, uint8_t *pucData, uint16_t usLen);
void M95xxx_Read(uint16_t usAddr, uint8_t *pucData, uint16_t usLen);
void M95xxx_Delay(uint32_t ulTick);
void M95xxx_WP(void);
void M95xxx_Hold(void);
uint8_t M95xxx_StatusRegister(void);

void M95320_CS(Bool_e eBool);
void M95640_CS(Bool_e eBool);


#endif 









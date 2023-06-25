#ifndef __CRC16_H__
#define __CRC16_H__


#include "main.h"

uint16_t CRC16(uint8_t* Buf,uint32_t Len);




void Crc32_Init(uint32_t *pulCrcVal);
uint32_t Crc32_Update(uint32_t ulCrc32, uint8_t *pucData, uint32_t ulLen);
uint32_t Crc32_Finished(uint32_t ulCrc32);




#endif




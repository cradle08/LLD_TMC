#include "crc.h"





/****************************************************************************************

CRC16

*****************************************************************************************/


/* 高位字节的 CRC 值 */
static unsigned char auchCRCHi[] = {
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40
};

/* 低位字节的 CRC 值 */
static char auchCRCLo[] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
	0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
	0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
	0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
	0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
	0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
	0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
	0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
	0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
	0x40
};



/**
  * 计算CRC16，返回值为低字节在前
  */
uint16_t CRC16(uint8_t* Buf, uint32_t Len)
{
	uint8_t CRCHi = 0xFF; 		//CRC 的高字节初始化
	uint8_t CRCLo = 0xFF; 		//CRC 的低字节初始化
	uint8_t Index; 				//CRC 查询表索引
	
	//完成整个报文计算
	while(Len--) 		
	{
		Index = CRCLo ^ *Buf++;
		CRCLo = CRCHi ^ auchCRCHi[Index];
		CRCHi = auchCRCLo[Index];
	}
	
	return(CRCHi << 8 | CRCLo);
}











/****************************************************************************************

CRC32

*****************************************************************************************/
#define CRC_START_32	0xFFFFFFFFul
#define	CRC_POLY_32		0xEDB88320ul


#if 1

const uint32_t crc32_tbl[256] = {
    0x00000000L, 0x77073096L, 0xEE0E612CL, 0x990951BAL, 0x076DC419L,
    0x706AF48FL, 0xE963A535L, 0x9E6495A3L, 0x0EDB8832L, 0x79DCB8A4L,
    0xE0D5E91EL, 0x97D2D988L, 0x09B64C2BL, 0x7EB17CBDL, 0xE7B82D07L,
    0x90BF1D91L, 0x1DB71064L, 0x6AB020F2L, 0xF3B97148L, 0x84BE41DEL,
    0x1ADAD47DL, 0x6DDDE4EBL, 0xF4D4B551L, 0x83D385C7L, 0x136C9856L,
    0x646BA8C0L, 0xFD62F97AL, 0x8A65C9ECL, 0x14015C4FL, 0x63066CD9L,
    0xFA0F3D63L, 0x8D080DF5L, 0x3B6E20C8L, 0x4C69105EL, 0xD56041E4L,
    0xA2677172L, 0x3C03E4D1L, 0x4B04D447L, 0xD20D85FDL, 0xA50AB56BL,
    0x35B5A8FAL, 0x42B2986CL, 0xDBBBC9D6L, 0xACBCF940L, 0x32D86CE3L,
    0x45DF5C75L, 0xDCD60DCFL, 0xABD13D59L, 0x26D930ACL, 0x51DE003AL,
    0xC8D75180L, 0xBFD06116L, 0x21B4F4B5L, 0x56B3C423L, 0xCFBA9599L,
    0xB8BDA50FL, 0x2802B89EL, 0x5F058808L, 0xC60CD9B2L, 0xB10BE924L,
    0x2F6F7C87L, 0x58684C11L, 0xC1611DABL, 0xB6662D3DL, 0x76DC4190L,
    0x01DB7106L, 0x98D220BCL, 0xEFD5102AL, 0x71B18589L, 0x06B6B51FL,
    0x9FBFE4A5L, 0xE8B8D433L, 0x7807C9A2L, 0x0F00F934L, 0x9609A88EL,
    0xE10E9818L, 0x7F6A0DBBL, 0x086D3D2DL, 0x91646C97L, 0xE6635C01L,
    0x6B6B51F4L, 0x1C6C6162L, 0x856530D8L, 0xF262004EL, 0x6C0695EDL,
    0x1B01A57BL, 0x8208F4C1L, 0xF50FC457L, 0x65B0D9C6L, 0x12B7E950L,
    0x8BBEB8EAL, 0xFCB9887CL, 0x62DD1DDFL, 0x15DA2D49L, 0x8CD37CF3L,
    0xFBD44C65L, 0x4DB26158L, 0x3AB551CEL, 0xA3BC0074L, 0xD4BB30E2L,
    0x4ADFA541L, 0x3DD895D7L, 0xA4D1C46DL, 0xD3D6F4FBL, 0x4369E96AL,
    0x346ED9FCL, 0xAD678846L, 0xDA60B8D0L, 0x44042D73L, 0x33031DE5L,
    0xAA0A4C5FL, 0xDD0D7CC9L, 0x5005713CL, 0x270241AAL, 0xBE0B1010L,
    0xC90C2086L, 0x5768B525L, 0x206F85B3L, 0xB966D409L, 0xCE61E49FL,
    0x5EDEF90EL, 0x29D9C998L, 0xB0D09822L, 0xC7D7A8B4L, 0x59B33D17L,
    0x2EB40D81L, 0xB7BD5C3BL, 0xC0BA6CADL, 0xEDB88320L, 0x9ABFB3B6L,
    0x03B6E20CL, 0x74B1D29AL, 0xEAD54739L, 0x9DD277AFL, 0x04DB2615L,
    0x73DC1683L, 0xE3630B12L, 0x94643B84L, 0x0D6D6A3EL, 0x7A6A5AA8L,
    0xE40ECF0BL, 0x9309FF9DL, 0x0A00AE27L, 0x7D079EB1L, 0xF00F9344L,
    0x8708A3D2L, 0x1E01F268L, 0x6906C2FEL, 0xF762575DL, 0x806567CBL,
    0x196C3671L, 0x6E6B06E7L, 0xFED41B76L, 0x89D32BE0L, 0x10DA7A5AL,
    0x67DD4ACCL, 0xF9B9DF6FL, 0x8EBEEFF9L, 0x17B7BE43L, 0x60B08ED5L,
    0xD6D6A3E8L, 0xA1D1937EL, 0x38D8C2C4L, 0x4FDFF252L, 0xD1BB67F1L,
    0xA6BC5767L, 0x3FB506DDL, 0x48B2364BL, 0xD80D2BDAL, 0xAF0A1B4CL,
    0x36034AF6L, 0x41047A60L, 0xDF60EFC3L, 0xA867DF55L, 0x316E8EEFL,
    0x4669BE79L, 0xCB61B38CL, 0xBC66831AL, 0x256FD2A0L, 0x5268E236L,
    0xCC0C7795L, 0xBB0B4703L, 0x220216B9L, 0x5505262FL, 0xC5BA3BBEL,
    0xB2BD0B28L, 0x2BB45A92L, 0x5CB36A04L, 0xC2D7FFA7L, 0xB5D0CF31L,
    0x2CD99E8BL, 0x5BDEAE1DL, 0x9B64C2B0L, 0xEC63F226L, 0x756AA39CL,
    0x026D930AL, 0x9C0906A9L, 0xEB0E363FL, 0x72076785L, 0x05005713L,
    0x95BF4A82L, 0xE2B87A14L, 0x7BB12BAEL, 0x0CB61B38L, 0x92D28E9BL,
    0xE5D5BE0DL, 0x7CDCEFB7L, 0x0BDBDF21L, 0x86D3D2D4L, 0xF1D4E242L,
    0x68DDB3F8L, 0x1FDA836EL, 0x81BE16CDL, 0xF6B9265BL, 0x6FB077E1L,
    0x18B74777L, 0x88085AE6L, 0xFF0F6A70L, 0x66063BCAL, 0x11010B5CL,
    0x8F659EFFL, 0xF862AE69L, 0x616BFFD3L, 0x166CCF45L, 0xA00AE278L,
    0xD70DD2EEL, 0x4E048354L, 0x3903B3C2L, 0xA7672661L, 0xD06016F7L,
    0x4969474DL, 0x3E6E77DBL, 0xAED16A4AL, 0xD9D65ADCL, 0x40DF0B66L,
    0x37D83BF0L, 0xA9BCAE53L, 0xDEBB9EC5L, 0x47B2CF7FL, 0x30B5FFE9L,
    0xBDBDF21CL, 0xCABAC28AL, 0x53B39330L, 0x24B4A3A6L, 0xBAD03605L,
    0xCDD70693L, 0x54DE5729L, 0x23D967BFL, 0xB3667A2EL, 0xC4614AB8L,
    0x5D681B02L, 0x2A6F2B94L, 0xB40BBE37L, 0xC30C8EA1L, 0x5A05DF1BL,
    0x2D02EF8DL
};



#else


/*
*	CRC32 table
*/
static __IO uint32_t crc32_tbl[256] = {0};


/*
*	CRC32 Table Init
*/
void Crc32_Table_Init()
{
	uint32_t i;
	uint32_t j;
	uint32_t ulCrc;
	
	for(i = 0; i < 256; i++)
	{
		ulCrc = i;
		for(j = 0; j < 8; j++)
		{
			if(ulCrc & 0x00000001L)  ulCrc = (ulCrc >> 1)^CRC_POLY_32;
			else ulCrc = ulCrc >> 1;
		
		}
		
		crc32_tbl[i] = ulCrc;
	}
}

#endif





/*
*	CRC32 Init
*/
void Crc32_Init(uint32_t *pulCrcVal)
{
	*pulCrcVal = CRC_START_32;
}




/*
*	CRC32 更新计算
*/
uint32_t Crc32_Update(uint32_t ulCrc32, uint8_t *pucData, uint32_t ulLen)
{
	uint32_t i = 0, ulCrc32Temp = 0;
	
	ulCrc32Temp = ulCrc32;
	for (i = 0; i < ulLen; i++)
	{
	   ulCrc32Temp = (ulCrc32Temp >> 8) ^ crc32_tbl[(ulCrc32Temp & 0xFF) ^  *pucData++];
	 }

	return ulCrc32Temp;
}




/*
*	CRC32 结束
*/
uint32_t Crc32_Finished(uint32_t ulCrc32)
{
	return (ulCrc32 ^ 0xFFFFFFFFul);
}






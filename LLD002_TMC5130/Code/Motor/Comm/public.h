#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"


#define __IO	volatile


//液面探测  默认Can ID
#define LLD_CAN_DEFAULT_RECV_ID		199		//默认接受CanID  0xC7
#define LLD_CAN_DEFAULT_SEND_ID		199		//默认发送CanID

//电机 默认Can ID
#define CAN_DEFAULT_RECV_ID			200		//默认接受CanID  0xC8
#define CAN_DEFAULT_SEND_ID			200		//默认发送CanID



/*
*	CAN波特率定义
*/
typedef enum {
	EN_CAN_BAUD_5		= 0x00,
	EN_CAN_BAUD_10,
	EN_CAN_BAUD_20,
	EN_CAN_BAUD_50,
	EN_CAN_BAUD_100,
	EN_CAN_BAUD_125,
	EN_CAN_BAUD_250,
	EN_CAN_BAUD_500,
	EN_CAN_BAUD_800,
	EN_CAN_BAUD_1000,
	EN_CAN_BAUD_END

}CanBaud_e;






/*
*	Bool
*/
typedef enum{
	false	= 0,
	true    = !false

}Bool_e;



/*
 * read write flag
 */
typedef enum {
	TMC_READ = 0,
	TMC_WRITE,

}ReadWrite_e;





/*
*	错误类型
*/
typedef enum {
	ERROR_TYPE_SUCCESS = 0x00,	//无故障
	ERROR_TYPE_CMD,				//命令错误	
	ERROR_TYPE_DEVICE_ID,		//设备ID错误
	ERROR_TYPE_TYPE,			//Type错误
	ERROR_TYPE_DATA,			//数据错误
	ERROR_TYPE_CRC,				//消息校验错误
	ERROR_TYPE_RW_RIGHT,		//读写权限错误
	ERROR_TYPE_EXEC,			//执行错误
	ERROR_TYPE_EEPROM,			//EEPROM读写异常
	ERROR_TYPE_TMC_RW,			//TMC芯片读写
	ERROR_TYPE_FLASH,			//FLASH读写异常,用于Bootloader
	ERROR_TYPE_EXEC_RIGH,		//无执行权限
	
}ErrorType_e;
	



/*
*	四字节数据类型，枚举
*/
typedef __packed union{				
		uint8_t ucData[4];
		uint32_t ulData;
		int32_t  lData;
		float	 fData;
	
}Data4Byte_u;





#define BYTE2Int(Byte3, Byte2, Byte1, Byte0)  ((((Byte3)&0xFF)<<24) | (((Byte2)&0xFF)<<16) | (((Byte1)&0xFF)<<8) | (((Byte0)&0xFF)<<0))


/* Cast a n bit signed int to a 32 bit signed int
 * This is done by checking the MSB of the signed int (Bit n).
 * If it is 1, the value is negative and the Bits 32 to n+1 are set to 1
 * If it is 0, the value remains unchanged
 */
#define CAST_Sn_TO_S32(value, n) ((value) | (((value) & (1<<((n)-1)))? ~((0x1<<(n))-1) : 0 ))


#define BIT_SET(a, n)    ((a) |= (1<<(n)))
#define BIT_RESET(a, n)  ((a) &= (~(1<<(n))))


// Min/Max macros
#ifndef MIN
	#define MIN(a,b) (((a)<(b)) ? (a) : (b))
#endif
#ifndef MAX
	#define MAX(a,b) (((a)>(b)) ? (a) : (b))
#endif

// Static Array length
//#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

// Generic mask/shift macros
#define FIELD_GET(data, mask, shift) \
	(((data) & (mask)) >> (shift))
#define FIELD_SET(data, mask, shift, value) \
	(((data) & (~(mask))) | (((value) << (shift)) & (mask)))

// Register read/write/update macros using Mask/Shift:
#define FIELD_READ(read, motor, address, mask, shift) \
	FIELD_GET(read(motor, address), mask, shift)
#define FIELD_WRITE(write, motor, address, mask, shift, value) \
	(write(motor, address, ((value)<<(shift)) & (mask)))
#define FIELD_UPDATE(read, write, motor, address, mask, shift, value) \
	(write(motor, address, FIELD_SET(read(motor, address), mask, shift, value)))

// Macro to surpress unused parameter warnings
#ifndef UNUSED
	#define UNUSED(x) (void)(x)
#endif

// Memory access helpers
// Force the compiler to access a location exactly once
#define ACCESS_ONCE(x) *((volatile typeof(x) *) (&x))

// Macro to remove write bit for shadow register array access
#define TMC_ADDRESS(x) ((x) & (TMC_ADDRESS_MASK))




/*
*   us延时函数
*/
void Delay_US(uint32_t ulUS);
uint8_t Delay_MS_NOBlock(uint32_t ulMS);
uint8_t Delay_MS_NOBlock_10ms(void);




  
#ifdef __cplusplus
}
#endif

#endif //__PUBLIC_H__








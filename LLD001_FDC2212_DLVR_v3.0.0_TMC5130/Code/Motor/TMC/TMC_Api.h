#ifndef __TMC_API_H__
#define __TMC_API_H__

#include "main.h"
#include "project.h"
#include "public.h"


//spi communication invalid value (none)
#define TMC_SPI_INVALID_VALUE	0x00
#define TMC_ADDRESS_MASK 		0x7F
#define TMC_WRITE_BIT 			0x80
#define TMC_ADDRESS(x) 			((x) & (TMC_ADDRESS_MASK))
#define BYTE(value, n)    		(((value) >> ((n) << 3)) & 0xFF)

/*
 * TMC Chip Index, ���֧������
 */
typedef enum {
	TMC_0 = 0,	//TMC5160
	TMC_1,		//TMC5160
	TMC_2,		//TMC5160
	TMC_3,		//TMC5160
	TMC_4,		//TMC5160
	TMC_5,		//TMC5160
	TMC_END

}TMC_e;





/*
 * TMC Work Mode
 */
typedef enum {
	TMC_WORK_MODE_1 = 0,	//SD_MODE=0,SPI_MODE=1, only use spi
	TMC_WORK_MODE_2,		//SD_MODE=1,SPI_MODE=1, use spi set parameter, and than use step/dir to driver
	TMC_WORK_MODE_3,		//SD_MODE=1,SPI_MODE=0, only use step/dir to driver
	TMC_WORK_MODE_END

}TMC_Work_Mode_e;



/*
 * refer point
 */
typedef enum {
	TMC_REF_LEFT = 0,
	TMC_REF_RIGHT,

}TMC_Ref_e;




/*
 * ��ת����
 */
typedef enum {
	ROTATE_DIRECT_CLOCKWISE		 = 0,	//˳ʱ��
	ROTATE_DIRECT_ANTI_CLOCKWISE,		//��ʱ��

}RotateDirect_e;








/*
 * TMC������ϸ��
 */
typedef enum {
	TMC_MICRO_STEP_RESOLUTION_256 = 0,
	TMC_MICRO_STEP_RESOLUTION_128,
	TMC_MICRO_STEP_RESOLUTION_64,
	TMC_MICRO_STEP_RESOLUTION_32,
	TMC_MICRO_STEP_RESOLUTION_16,
	TMC_MICRO_STEP_RESOLUTION_8,
	TMC_MICRO_STEP_RESOLUTION_4,
	TMC_MICRO_STEP_RESOLUTION_2,
	TMC_MICRO_STEP_RESOLUTION_1,
	TMC_MICRO_STEP_RESOLUTION_END

	
}TMC_MicroStep_Resolution_e;





//
void TMC_Init(void);
//
uint8_t TMC_WriteInt(TMC_e eTMC, uint8_t ucAddr, uint32_t ulValue);
//uint8_t TMC_ReadInt(TMC_e eTMC, uint8_t ucAddr, int32_t *plValue);
int32_t TMC_ReadInt(TMC_e eTMC, uint8_t ucAddr);
uint8_t TMC_WriteDataGram(TMC_e eTMC, uint8_t ucAddr, uint8_t Byte1, uint8_t Byte2, uint8_t Byte3, uint8_t Byte4);
//void TMC_FillShadowRegisters(TMC_e eTMC);

//
//��ʼ��
void TMC_SetVMode_V(TMC_e eTMC, uint8_t ucOpt);
void TMC_SetPMode_V(TMC_e eTMC, uint8_t ucOpt);
//
ErrorType_e TMC_Mode_Set(TMC_e eTMC, TMC_Work_Mode_e eMode);
void TMC_Enable(TMC_e eTMC);
void TMC_Disable(TMC_e eTMC);
//uint8_t TMC_Restore(TMC_e eTMC);
//void TMC_SetRegisterResetState(TMC_e eTMC, const int32_t *pulState);
void Print_Register_Value(TMC_e eTMC,  uint8_t ucAddr);
void Print_AllRegister_Value(TMC_e eTMC);

//
ErrorType_e TMC_Rotate(TMC_e eTMC, RotateDirect_e eDirect, uint32_t lVelocity);
ErrorType_e TMC_Stop(TMC_e eTMC);
ErrorType_e TMC_Stop_Urgent(TMC_e eTMC);
ErrorType_e TMC_MoveTo(TMC_e eTMC, int32_t lPosition);
ErrorType_e TMC_MoveBy(TMC_e eTMC, int32_t lTicks);
//������λ
uint8_t TMC_Reset(TMC_e eTMC, TMC_Ref_e eRef);

















#endif //__TMC_API_H__















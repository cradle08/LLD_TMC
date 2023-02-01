#ifndef __TMC5130_H__
#define __TMC5130_H__

#include "main.h"
#include "TMC_Api.h"
#include "TMC5130_Register.h"
#include "TMC5130_Fields.h"


#define TMC5160_WRITE_BIT       TMC_WRITE_BIT



/*
*	Init default value
*/
#define DEFAULT_FULL_STEP_PER_ROUND		200		  						//每转全步数，默认200
#define DEFAULT_MICRO_STEP_RESULTION	TMC_MICRO_STEP_RESOLUTION_256	//默认256细分

//驱动频率
#define DEFAULT_TMC_CLK					16000000	//16M

//速度，微步和寄存器数据转换关系. Page_133
#define V_CHANGE_CONST_16M				1.048576	//V[ustep]=V[5160]*(Fclk_16M/2/2^23) => V[5160] = V[ustep]*2*2^23/Fcllk_16M				
#define V_CHANGE_CONST_12M				1.398101	//V[ustep]=V[5160]*(Fclk_12M/2/2^23) => V[5160] = V[ustep]*2*2^23/Fcllk_12M				
#define V_CHANGE_CONST					V_CHANGE_CONST_16M

//加速度，微步和寄存器数据转换关系. Page_133
#define A_CHANGE_CONST_16M				0.008590	//A[ustep]=A[5160]*(Fclk_16M^2/(512*256)/2^24) => A[5160] = A[ustep]*(512*256)*2^24/Fcllk_16M^2				
#define A_CHANGE_CONST_12M				0.015271	//A[ustep]=A[5160]*(Fclk_12M^2/(512*256)/2^24) => A[5160] = A[ustep]*(512*256)*2^24/Fcllk_12M^2
#define A_CHANGE_CONST					A_CHANGE_CONST_16M


//设置电流值和实际配置值，倍数关系， Real = Set/8
#define CURRENT_CHANGE_CONST			8
//电流最大设置值
#define CURRENT_MAX_VALUE				255

/************* 编码器常数，该值和：每转全步数，步进分辨率，编码器分辨率。这三个参数有关系 ******
*				每转全步数：每转全步数，一般二相步进电机该值为：200（1.8度）。
*				步进分辨率：TMC一般有：256、128、64、32、16、8、4、2、1
*				编码器分辨率：一般有：360线，500线，750线，1000线，1024线， 4000线等
*				编码器常数转换关系为：
*					编码器常数：（编码器因子（整数部分）*2^16 + 编码器因子（小数部分）*10000）/4
*					编码器因子：步进分辨率*每转全步数/编码器分辨率
*
*	2^16=65536
*				
*	200*256/1000/4=12.8, 12*2^16+0.8*10000=0x0C1F40
*	200*128/1000/4=6.4,  6*2^16+0.4*10000=0x060FA0
*   200*64/1000/4=3.2,   3*2^16+0.2*10000=0x0307D0
*   200*32/1000/4=1.6,   1*2^16+0.6*10000=0x01176F
*   200*16/1000/4=0.8,   0*2^16+0.8*10000=0x001F40
*   200*8/1000/4=0.4,    0*2^16+0.4*10000=0x000FA0
*
*
*
	200*64/360/4=8.888888,   8*2^16+0.888889*10000=0x822B1
	200*64/400/4=8,   		 8*2^16+0*10000=0x80000
	200*64/500/4=6.4,   	 6*2^16+0.4*10000=0x60FA0
		
	200*64/720/4=4.444444,   4*2^16+0.444444*10000=0x04115C
	200*64/1000/4=3.2,   	 3*2^16+0.2*10000=0x0307D0
    200*64/1024/4=3.125,     3*2^16+0.125*10000=0x304E2
	200*64/4000/4=0.8,   	 0*2^16+0.8*10000=0x1F40
	200*64/4096/4=0.78125,   8*2^16+0*10000=0x1E84
*
*	
*
*/
//默认编码器分辨率定义
#define DEFAULT_ENC_RESULTION_360			360		//编码器分辨率 360
#define DEFAULT_ENC_RESULTION_400			400		//编码器分辨率 400
#define DEFAULT_ENC_RESULTION_500			500		//编码器分辨率 500
#define DEFAULT_ENC_RESULTION_720			720		//编码器分辨率 720
#define DEFAULT_ENC_RESULTION_1000			1000	//编码器分辨率 1000
#define DEFAULT_ENC_RESULTION_1024			1024	//编码器分辨率 1024
#define DEFAULT_ENC_RESULTION_4000			4000	//编码器分辨率 4000
#define DEFAULT_ENC_RESULTION_4096			4096	//编码器分辨率 4096

//编码器常数，前提是：64细分，没转200全步，AB增量编码器。
#define DEFAULT_ENC_CONST_VALUE_TMC_360		0x80000		//360线，编码器常数
#define DEFAULT_ENC_CONST_VALUE_TMC_400		0x80000		//400线，编码器常数
#define DEFAULT_ENC_CONST_VALUE_TMC_500		0x80000		//500线，编码器常数
#define DEFAULT_ENC_CONST_VALUE_TMC_720		0x04115C   	//720线，编码器常数	
#define DEFAULT_ENC_CONST_VALUE_TMC_1000	0x0307D0   	//1000线，编码器常数	
#define DEFAULT_ENC_CONST_VALUE_TMC_1024	0x304E2   	//1024线，编码器常数
#define DEFAULT_ENC_CONST_VALUE_TMC_4000	0x1F40  	//4000线，编码器常数
#define DEFAULT_ENC_CONST_VALUE_TMC_4096	0x1E84   	//4096线，编码器常数
//
#define DEFAULT_ENC_CONST_VALUE_TMC_0	DEFAULT_ENC_CONST_VALUE_TMC_1000     	
#define DEFAULT_ENC_CONST_VALUE_TMC_1	DEFAULT_ENC_CONST_VALUE_TMC_1000   		
#define DEFAULT_ENC_CONST_VALUE_TMC_2	DEFAULT_ENC_CONST_VALUE_TMC_1000   		
#define DEFAULT_ENC_CONST_VALUE_TMC_3	DEFAULT_ENC_CONST_VALUE_TMC_1000   		
#define DEFAULT_ENC_CONST_VALUE_TMC_4	DEFAULT_ENC_CONST_VALUE_TMC_1000   		
#define DEFAULT_ENC_CONST_VALUE_TMC_5	DEFAULT_ENC_CONST_VALUE_TMC_1000   		
//	
#define DEFAULT_ENC_CONST_VALUE_Y       DEFAULT_ENC_CONST_VALUE_TMC_1000
#define DEFAULT_ENC_CONST_VALUE_Z		DEFAULT_ENC_CONST_VALUE_TMC_720  		
#define DEFAULT_ENC_CONST_VALUE_P		DEFAULT_ENC_CONST_VALUE_TMC_1000   		



//十进制模式，AB极性高有效，失能N
#define DEFAULT_ENC_MODE_VALUE		0x0407    		
//寄存器个数
#define TMC5160_REGISTER_COUNT		128

/*
*	TMC5160定义
*/
typedef __packed struct {
	uint32_t ulOldTick;
	int32_t lVelocity;
	int32_t lOldX;
//	int32_t laRegisterResetStatus[TMC5160_REGISTER_COUNT];
	uint8_t	ucaRegisterAccess[TMC5160_REGISTER_COUNT];
	int32_t laShadowRegister[TMC5160_REGISTER_COUNT];
	
} TMC5160_t;



//读写
#define TMC5160_FIELD_READ(motor, address, mask, shift)           FIELD_READ(TMC5160_ReadInt, motor, address, mask, shift)
#define TMC5160_FIELD_WRITE(motor, address, mask, shift, value)   FIELD_WRITE(TMC5160_WriteInt, motor, address, mask, shift, value)
#define TMC5160_FIELD_UPDATE(motor, address, mask, shift, value)  FIELD_UPDATE(TMC5160_ReadInt, TMC5160_WriteInt, motor, address, mask, shift, value)



void TMC5160_PrintSixPoint_V(TMC_e eTMC);
void TMC5160_Init(TMC_e eTMC);
void TMC5160_I_Set(TMC_e eTMC);
uint8_t TMC5160_Restore(TMC_e eTMC);
ErrorType_e CheckRegister_Addr(uint8_t ucAddr);

//读写接口
ErrorType_e TMC5160_WriteInt(TMC_e eTMC, uint8_t ucAddr, int32_t lValue);
int32_t TMC5160_ReadInt(TMC_e eTMC, uint8_t ucAddr);
uint8_t TMC5160_WriteDataGram(TMC_e eTMC, uint8_t ucAddr, uint8_t Byte1, uint8_t Byte2, uint8_t Byte3, uint8_t Byte4);
void TMC5160_FillShadowRegisters(TMC_e eTMC);
void TMC5160_SetRegisterResetState(TMC_e eTMC, const int32_t *pulState);

//步进细分
uint16_t MicroStep_Register2SetValue(TMC_MicroStep_Resolution_e eMicroStep);
ErrorType_e MicroStep_SetValue2Register(uint16_t usMicroStep, uint8_t *pucMicroStep_Rg);

//运动
ErrorType_e TMC5160_Reset(TMC_e eTMC, TMC_Ref_e eRef);
ErrorType_e TMC5160_Rotate(TMC_e eTMC, RotateDirect_e eDrect, int32_t lVelocity);
ErrorType_e TMC5160_Stop(TMC_e eTMC);
ErrorType_e TMC5160_MoveTo(TMC_e eTMC, int32_t lPosition);
ErrorType_e TMC5160_MoveBy(TMC_e eTMC, int32_t lTicks);



void TMC_Delay(uint32_t ulMS);



#endif /* __TMC5130_H__ */



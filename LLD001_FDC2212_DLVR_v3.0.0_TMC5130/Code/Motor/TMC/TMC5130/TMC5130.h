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
#define DEFAULT_FULL_STEP_PER_ROUND		200		  						//ÿתȫ������Ĭ��200
#define DEFAULT_MICRO_STEP_RESULTION	TMC_MICRO_STEP_RESOLUTION_256	//Ĭ��256ϸ��

//����Ƶ��
#define DEFAULT_TMC_CLK					16000000	//16M

//�ٶȣ�΢���ͼĴ�������ת����ϵ. Page_133
#define V_CHANGE_CONST_16M				1.048576	//V[ustep]=V[5160]*(Fclk_16M/2/2^23) => V[5160] = V[ustep]*2*2^23/Fcllk_16M				
#define V_CHANGE_CONST_12M				1.398101	//V[ustep]=V[5160]*(Fclk_12M/2/2^23) => V[5160] = V[ustep]*2*2^23/Fcllk_12M				
#define V_CHANGE_CONST					V_CHANGE_CONST_16M

//���ٶȣ�΢���ͼĴ�������ת����ϵ. Page_133
#define A_CHANGE_CONST_16M				0.008590	//A[ustep]=A[5160]*(Fclk_16M^2/(512*256)/2^24) => A[5160] = A[ustep]*(512*256)*2^24/Fcllk_16M^2				
#define A_CHANGE_CONST_12M				0.015271	//A[ustep]=A[5160]*(Fclk_12M^2/(512*256)/2^24) => A[5160] = A[ustep]*(512*256)*2^24/Fcllk_12M^2
#define A_CHANGE_CONST					A_CHANGE_CONST_16M


//���õ���ֵ��ʵ������ֵ��������ϵ�� Real = Set/8
#define CURRENT_CHANGE_CONST			8
//�����������ֵ
#define CURRENT_MAX_VALUE				255

/************* ��������������ֵ�ͣ�ÿתȫ�����������ֱ��ʣ��������ֱ��ʡ������������й�ϵ ******
*				ÿתȫ������ÿתȫ������һ����ಽ�������ֵΪ��200��1.8�ȣ���
*				�����ֱ��ʣ�TMCһ���У�256��128��64��32��16��8��4��2��1
*				�������ֱ��ʣ�һ���У�360�ߣ�500�ߣ�750�ߣ�1000�ߣ�1024�ߣ� 4000�ߵ�
*				����������ת����ϵΪ��
*					�����������������������ӣ��������֣�*2^16 + ���������ӣ�С�����֣�*10000��/4
*					���������ӣ������ֱ���*ÿתȫ����/�������ֱ���
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
//Ĭ�ϱ������ֱ��ʶ���
#define DEFAULT_ENC_RESULTION_360			360		//�������ֱ��� 360
#define DEFAULT_ENC_RESULTION_400			400		//�������ֱ��� 400
#define DEFAULT_ENC_RESULTION_500			500		//�������ֱ��� 500
#define DEFAULT_ENC_RESULTION_720			720		//�������ֱ��� 720
#define DEFAULT_ENC_RESULTION_1000			1000	//�������ֱ��� 1000
#define DEFAULT_ENC_RESULTION_1024			1024	//�������ֱ��� 1024
#define DEFAULT_ENC_RESULTION_4000			4000	//�������ֱ��� 4000
#define DEFAULT_ENC_RESULTION_4096			4096	//�������ֱ��� 4096

//������������ǰ���ǣ�64ϸ�֣�ûת200ȫ����AB������������
#define DEFAULT_ENC_CONST_VALUE_TMC_360		0x80000		//360�ߣ�����������
#define DEFAULT_ENC_CONST_VALUE_TMC_400		0x80000		//400�ߣ�����������
#define DEFAULT_ENC_CONST_VALUE_TMC_500		0x80000		//500�ߣ�����������
#define DEFAULT_ENC_CONST_VALUE_TMC_720		0x04115C   	//720�ߣ�����������	
#define DEFAULT_ENC_CONST_VALUE_TMC_1000	0x0307D0   	//1000�ߣ�����������	
#define DEFAULT_ENC_CONST_VALUE_TMC_1024	0x304E2   	//1024�ߣ�����������
#define DEFAULT_ENC_CONST_VALUE_TMC_4000	0x1F40  	//4000�ߣ�����������
#define DEFAULT_ENC_CONST_VALUE_TMC_4096	0x1E84   	//4096�ߣ�����������
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



//ʮ����ģʽ��AB���Ը���Ч��ʧ��N
#define DEFAULT_ENC_MODE_VALUE		0x0407    		
//�Ĵ�������
#define TMC5160_REGISTER_COUNT		128

/*
*	TMC5160����
*/
typedef __packed struct {
	uint32_t ulOldTick;
	int32_t lVelocity;
	int32_t lOldX;
//	int32_t laRegisterResetStatus[TMC5160_REGISTER_COUNT];
	uint8_t	ucaRegisterAccess[TMC5160_REGISTER_COUNT];
	int32_t laShadowRegister[TMC5160_REGISTER_COUNT];
	
} TMC5160_t;



//��д
#define TMC5160_FIELD_READ(motor, address, mask, shift)           FIELD_READ(TMC5160_ReadInt, motor, address, mask, shift)
#define TMC5160_FIELD_WRITE(motor, address, mask, shift, value)   FIELD_WRITE(TMC5160_WriteInt, motor, address, mask, shift, value)
#define TMC5160_FIELD_UPDATE(motor, address, mask, shift, value)  FIELD_UPDATE(TMC5160_ReadInt, TMC5160_WriteInt, motor, address, mask, shift, value)



void TMC5160_PrintSixPoint_V(TMC_e eTMC);
void TMC5160_Init(TMC_e eTMC);
void TMC5160_I_Set(TMC_e eTMC);
uint8_t TMC5160_Restore(TMC_e eTMC);
ErrorType_e CheckRegister_Addr(uint8_t ucAddr);

//��д�ӿ�
ErrorType_e TMC5160_WriteInt(TMC_e eTMC, uint8_t ucAddr, int32_t lValue);
int32_t TMC5160_ReadInt(TMC_e eTMC, uint8_t ucAddr);
uint8_t TMC5160_WriteDataGram(TMC_e eTMC, uint8_t ucAddr, uint8_t Byte1, uint8_t Byte2, uint8_t Byte3, uint8_t Byte4);
void TMC5160_FillShadowRegisters(TMC_e eTMC);
void TMC5160_SetRegisterResetState(TMC_e eTMC, const int32_t *pulState);

//����ϸ��
uint16_t MicroStep_Register2SetValue(TMC_MicroStep_Resolution_e eMicroStep);
ErrorType_e MicroStep_SetValue2Register(uint16_t usMicroStep, uint8_t *pucMicroStep_Rg);

//�˶�
ErrorType_e TMC5160_Reset(TMC_e eTMC, TMC_Ref_e eRef);
ErrorType_e TMC5160_Rotate(TMC_e eTMC, RotateDirect_e eDrect, int32_t lVelocity);
ErrorType_e TMC5160_Stop(TMC_e eTMC);
ErrorType_e TMC5160_MoveTo(TMC_e eTMC, int32_t lPosition);
ErrorType_e TMC5160_MoveBy(TMC_e eTMC, int32_t lTicks);



void TMC_Delay(uint32_t ulMS);



#endif /* __TMC5130_H__ */



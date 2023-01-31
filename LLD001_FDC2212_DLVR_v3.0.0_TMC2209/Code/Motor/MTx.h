// MTx.h


//--------------------------------------
// Copyright    Editor     Time
// Rayto        LiLiang    2016_07_25
//--------------------------------------   


#ifndef   __MTX_H__
#define   __MTX_H__

#include "main.h"



//-----------------------------------------------------------------------------------------
// enumeration, status of the movement

// level, high or low
enum eMotorPinLevel		            
{
    Motor_Pin_Low    = 0,             // level low
    Motor_Pin_High   = 1,             // level high

};

// direction
enum eMotorDirection
{
    Motor_Direction_Negative  = 0,    // negative direction
    Motor_Direction_Positive  = 1,    // positive direction

};

// phase of the movement
enum eMotorPhase		  
{
    Motor_Phase_IDLE    = 0,          // idle
	Motor_Phase_LOAD    = 1,	      // load value the frist time
	
    Motor_Phase_ACC     = 2,          // speed up
	Motor_Phase_EQU     = 3, 	      // speed keep
    Motor_Phase_DEC     = 4,          // speed reduction
    
    Motor_Phase_FIN     = 5,          // finish

};

//-----------------------------------------------------------------------------------------
// <3>. struct, attention: the alignment of members in the struct

// parameters of a movement, 4 * 12 = 48 bytes
struct tMotorControl  
{
    __IO uint32_t   nCurFreq;     // current frequence of the motor     
    __IO uint32_t   nTimeLoad;    // the next value reloaded of the timer     
    
    __IO uint32_t   nFreqMin;	   // the frequence at the begining or ending  
    __IO uint32_t   nFreqMax;	   // the max frequence of the motor     
    __IO uint32_t   nFreqInc;	   // the increment of frequence changing 
    __IO uint32_t   nFreqSam;     // the width of a step when frequence changing

    __IO uint32_t   nStepsAcc;	   // steps, phase speed up 
    __IO uint32_t   nStepsEqu;    // steps, phase speed keep
	__IO uint32_t   nStepsDec;	   // steps, phase speed reduction
    __IO uint32_t   nStepsTotal;  // steps, total at the movement

    __IO enum eMotorDirection eDirection;  // direction
    __IO enum eMotorPhase     ePhase;      // phase

};

// status of the motor,  4 * 5 = 20 bytes
struct tMotorStatus
{ 
    // __IO enum eMotorPinLevel  eLastLimitIn; // last level of limit switch  
    // __IO enum eMotorPinLevel  eLastCodeIn;  // last level of encoder
      		
    __IO uint32_t  nStepsCur;                    // the absolute steps of current distance
    // __IO uint32_t  nStepsHome;                // the absolute steps of mechanical zero
    // __IO uint32_t  nCodeNum;                  // the value of the encoder

};

#if 1
// monitor the change of the IO's level
struct tMonitorIo
{
    // 1) functional seting
    __IO enum eFlag bAble;          // whether to enable the function
    // 2) record the status
    __IO uint8_t      chLevelLst;     // record the last IO's level
	__IO uint32_t     nStepsSlow;     // use the steps to slow down
};
#endif


//-----------------------------------------------------------------------------------------
// function declaration

// initialize the module of motor driver
uint8_t  MTx_InitMotor(uint8_t chIndex);

// for driver, lock or unlock
uint8_t  MTx_DriverEnable(uint8_t chIndex, enum eFlag bAble);   

// setting moving parameters for the motor's driver, and start a movement.
uint8_t  MTx_Move(uint8_t  chIndex,
                      uint32_t nFreqMin, 
                      uint32_t nFreqMax, 
                      uint32_t nFreqInc,
                      uint32_t nFreqSam,
                      uint32_t nSteps, 
                      enum eMotorDirection eDir);

// stop the movemnet, two ways: immediately or slowly. But must wait until it has finished
uint8_t  MTx_Stop(uint8_t chIndex, uint32_t nSteps, enum eFlag bSlowStop);

// waiting the movement until it has finished
uint8_t  MTx_Wait(uint8_t chIndex, uint32_t nDlyMs, enum eFlag bDlyType);  

// get the steps has left befor stopping in a complete movement
uint32_t MTx_GetStepsLeft(uint8_t chIndex);

// get the position(steps) in the absolute coordinates
uint32_t MTx_GetPosCur(uint8_t chIndex);

// set the position(steps) in the absolute coordinates
uint8_t  MTx_SetPosCur(uint8_t chIndex, uint32_t nSteps);

#if 0
// set current position(steps), home offset
_EXT_ uint8_t  MTx_SetPosHome(uint8_t chIndex, uint32_t nSteps);

// get current pulses of encoder
_EXT_ uint32_t MTx_GetCode(uint8_t chIndex);
#endif


//-----------------------------------------------------------------------------------------
// configure the IO's detection
uint8_t  MTx_IoInit(uint8_t chIndex);
uint8_t  MTx_IoConfig(uint8_t chIndex, enum eFlag bAble, uint32_t nStepsSlow);
void   MTx_IoMinitor(uint8_t chIndex);
uint8_t  MTx_GetIoLevel(uint8_t chIndex);

uint8_t mtx_get_io_level_changed(uint8_t chIndex);

//-----------------------------------------------------------------------------------------
// function declaration, using the timer's interrupt

// generating pulses to drive the motor
void MTx_PWM_ISR(uint8_t chIndex); 




#endif




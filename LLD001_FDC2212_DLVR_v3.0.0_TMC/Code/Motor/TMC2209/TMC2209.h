

#ifndef TMC_IC_TMC2209_H_
#define TMC_IC_TMC2209_H_

//#include "Constants.h"
//#include "API_Header.h"
#include "stdint.h"
#include "TMC2209_Register.h"
#include "TMC2209_Constants.h"
#include "TMC2209_Fields.h"

#define TMC_REGISTER_COUNT 128 // Default register count
#define ____ 0x00
#define TMC2209_DEFAULT_ADDR	2
#define size_t uint32_t

#define TMC_ACCESS_NONE        0x00

#define TMC_ACCESS_READ        0x01
#define TMC_ACCESS_WRITE       0x02
                            // 0x04 is currently unused
#define TMC_ACCESS_DIRTY       0x08  // Register has been written since reset -> shadow register is valid for restore

// Special Register bits
#define TMC_ACCESS_RW_SPECIAL  0x10  // Read and write are independent - different values and/or different functions
#define TMC_ACCESS_FLAGS       0x20  // Register has read or write to clear flags.
#define TMC_ACCESS_HW_PRESET   0x40  // Register has hardware presets (e.g. Factory calibrations) - do not write a default value
                            // 0x80 is currently unused

// Permission combinations
#define TMC_ACCESS_RW              (TMC_ACCESS_READ  | TMC_ACCESS_WRITE)        // 0x03 - Read and write
#define TMC_ACCESS_RW_SEPARATE     (TMC_ACCESS_RW    | TMC_ACCESS_RW_SPECIAL)   // 0x13 - Read and write, with separate values/functions
#define TMC_ACCESS_R_FLAGS         (TMC_ACCESS_READ  | TMC_ACCESS_FLAGS)        // 0x21 - Read, has flags (read to clear)
#define TMC_ACCESS_RW_FLAGS        (TMC_ACCESS_RW    | TMC_ACCESS_FLAGS)        // 0x23 - Read and write, has flags (read or write to clear)
#define TMC_ACCESS_W_PRESET        (TMC_ACCESS_WRITE | TMC_ACCESS_HW_PRESET)    // 0x42 - Write, has hardware preset - skipped in reset routine
#define TMC_ACCESS_RW_PRESET       (TMC_ACCESS_RW    | TMC_ACCESS_HW_PRESET)    // 0x43 - Read and write, has hardware presets - skipped in reset routine

// Helper macros
#define TMC_IS_READABLE(x)    ((x) & TMC_ACCESS_READ)
#define TMC_IS_WRITABLE(x)    ((x) & TMC_ACCESS_WRITE)
#define TMC_IS_DIRTY(x)       ((x) & TMC_ACCESS_DIRTY)
#define TMC_IS_PRESET(x)      ((x) & TMC_ACCESS_HW_PRESET)
#define TMC_IS_RESETTABLE(x)  (((x) & (TMC_ACCESS_W_PRESET)) == TMC_ACCESS_WRITE) // Write bit set, Hardware preset bit not set
#define TMC_IS_RESTORABLE(x)  (((x) & TMC_ACCESS_WRITE) && (!(x & TMC_ACCESS_HW_PRESET) || (x & TMC_ACCESS_DIRTY))) // Write bit set, if it's a hardware preset register, it needs to be dirty


// Helper macros
#define TMC2209_FIELD_READ(tdef, address, mask, shift) \
	FIELD_GET(tmc2209_readInt(tdef, address), mask, shift)
#define TMC2209_FIELD_UPDATE(tdef, address, mask, shift, value) \
	(tmc2209_writeInt(tdef, address, FIELD_SET(tmc2209_readInt(tdef, address), mask, shift, value)))

// Usage note: use 1 TypeDef per IC
//typedef struct {
//	ConfigurationTypeDef *config;

//	int32_t registerResetState[TMC2209_REGISTER_COUNT];
//	uint8_t registerAccess[TMC2209_REGISTER_COUNT];

//	uint8_t slaveAddress;
//} TMC2209TypeDef;

//typedef void (*tmc2209_callback)(TMC2209TypeDef*, ConfigState);

// Default Register values
#define R00 0x00000040  // GCONF
#define R10 0x00071703  // IHOLD_IRUN
#define R11 0x00000014  // TPOWERDOWN
#define R6C 0x10000053  // CHOPCONF
#define R70 0xC10D0024  // PWMCONF

// Register access permissions:
//   0x00: none (reserved)
//   0x01: read
//   0x02: write
//   0x03: read/write
//   0x13: read/write, separate functions/values for reading or writing
//   0x23: read/write, flag register (write to clear)
//   0x42: write, has hardware presets on reset
static const uint8_t tmc2209_defaultRegisterAccess[TMC2209_REGISTER_COUNT] =
{
//  0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
	0x03, 0x23, 0x01, 0x02, 0x02, 0x01, 0x01, 0x03, ____, ____, ____, ____, ____, ____, ____, ____, // 0x00 - 0x0F
	0x02, 0x02, 0x01, 0x02, 0x02, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, // 0x10 - 0x1F
	____, ____, 0x02, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, // 0x20 - 0x2F
	____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, // 0x30 - 0x3F
	0x02, 0x01, 0x02, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, // 0x40 - 0x4F
	____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, // 0x50 - 0x5F
	____, ____, ____, ____, ____, ____, ____, ____, ____, ____, 0x01, 0x01, 0x03, ____, ____, 0x01, // 0x60 - 0x6F
	0x03, 0x01, 0x01, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____, ____  // 0x70 - 0x7F
};

static const int32_t tmc2209_defaultRegisterResetState[TMC2209_REGISTER_COUNT] =
{
//	0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
	R00, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 0x00 - 0x0F
	R10, R11, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 0x10 - 0x1F
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 0x20 - 0x2F
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 0x30 - 0x3F
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 0x40 - 0x4F
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 0x50 - 0x5F
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   R6C, 0,   0,   0, // 0x60 - 0x6F
	R70, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0  // 0x70 - 0x7F
};

// Undefine the default register values.
// This prevents warnings in case multiple TMC-API chip headers are included at once
#undef R00
#undef R10
#undef R11
#undef R6C
#undef R70



#define TMC_WRITE_BIT 0x80
#define TMC_ADDRESS_MASK 0x7F
// Macro to remove write bit for shadow register array access
#define TMC_ADDRESS(x) ((x) & (TMC_ADDRESS_MASK))
// Macro to remove write bit for shadow register array access
#define TMC_ADDRESS(x) ((x) & (TMC_ADDRESS_MASK))



typedef __packed struct {
	uint32_t ulOldTick;
	int32_t lVelocity;
	int32_t lOldX;
	
	uint8_t Addr;
//	int32_t laRegisterResetStatus[TMC5160_REGISTER_COUNT];
	uint8_t	registerAccess[TMC2209_REGISTER_COUNT];
	int32_t shadowRegister[TMC2209_REGISTER_COUNT];
	
} TMC2209_t;



//Ê¹ÄÜ
#define TMC2209_ENABLE 		GPIO_ResetBits(GPIOA, GPIO_Pin_6)
//Ê§ÄÜ
#define TMC2209_DISABLE 	GPIO_SetBits(GPIOA, GPIO_Pin_6)

void TMC2209_ReadWriteArray(uint8_t *data, size_t writeLength, size_t readLength);
void TMC2209_WriteInt(uint8_t address, int32_t value);
void TMC2209_Init(void);

//
void TMC2209_WriteRegister(uint8_t ucAddress, int32_t lValue);
void TMC2209_ReadRegister(uint8_t ucAddress, int32_t *plValue);










//// Communication
//void tmc2209_writeInt(TMC2209TypeDef *tmc2209, uint8_t address, int32_t value);
//int32_t tmc2209_readInt(TMC2209TypeDef *tmc2209, uint8_t address);

//void tmc2209_init(TMC2209TypeDef *tmc2209, uint8_t channel, uint8_t slaveAddress, ConfigurationTypeDef *tmc2209_config, const int32_t *registerResetState);
//uint8_t tmc2209_reset(TMC2209TypeDef *tmc2209);
//uint8_t tmc2209_restore(TMC2209TypeDef *tmc2209);
//void tmc2209_setRegisterResetState(TMC2209TypeDef *tmc2209, const int32_t *resetState);
//void tmc2209_setCallback(TMC2209TypeDef *tmc2209, tmc2209_callback callback);
//void tmc2209_periodicJob(TMC2209TypeDef *tmc2209, uint32_t tick);

//uint8_t tmc2209_get_slave(TMC2209TypeDef *tmc2209);
//void tmc2209_set_slave(TMC2209TypeDef *tmc2209, uint8_t slaveAddress);

#endif /* TMC_IC_TMC2209_H_ */




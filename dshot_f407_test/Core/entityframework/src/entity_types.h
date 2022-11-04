/*
 * entity_types.c
 *
 *  Created on: Dec 01, 2021
 *      Author: Shpegun60
 */

#ifndef INC_ENTITY_TYPES_H_
#define INC_ENTITY_TYPES_H_

#include "stdint.h"
#include "stddef.h"
#include "entity_atomic.h"

// types mapping -----------------------------
#define VOID_TYPE 			((uint8_t) 0)
// unsigned type
#define UINT8_TYPE  		((uint8_t) 1)
#define UINT16_TYPE 		((uint8_t) 2)
#define UINT32_TYPE 		((uint8_t) 3)
#define UINT64_TYPE 		((uint8_t) 4)
//signed type
#define CHAR_TYPE 			((uint8_t) 5)
#define INT8_TYPE 			((uint8_t) 6)
#define INT16_TYPE 			((uint8_t) 7)
#define INT32_TYPE 			((uint8_t) 8)
#define INT64_TYPE 			((uint8_t) 9)
// floating point type
#define FLOAT_TYPE 			((uint8_t) 10)
#define DOUBLE_TYPE 		((uint8_t) 11)
#define LONG_DOUBLE_TYPE 	((uint8_t) 12)
// other types
#define BOOL_TYPE 			((uint8_t) 13)
#define UINT24_TYPE 		((uint8_t) 14)
#define INT24_TYPE 			((uint8_t) 15)
#define TYPE_ARRAY_LENGTH   ((uint8_t)(INT24_TYPE + 1))
//------------------------------------

#define ENTITY_MSB ((unsigned char)0x01)
#define ENTITY_LSB ((unsigned char)0x00)

typedef void     u0;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u24;
typedef uint32_t u32;
typedef uint64_t u64;

typedef char     c8;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i24;
typedef int32_t  i32;
typedef int64_t  i64;

typedef float       f32;
typedef double      f64;
typedef long double f128;

typedef unsigned char       b;

// bus types defining ----------------------------------------------
typedef size_t reg;  // entity pointer type
#define ENTITY_PERIF_BUS_TYPE_IND UINT32_TYPE
//------------------------------------------------------------------


/*
 * *************************************************
 * Define user copy NON ATOMIC OPERATIONS!!!!!!!!
 * *************************************************
 */
#define USER_DATA_COPY(n, from, to, pos, direction) do {\
		if(direction) {\
			while((n)--) {\
				*(to)++ = *((from) + (n));\
				(*(pos))++;\
			}\
		} else {\
			while((n)--) {\
				*(to)++ = *(from)++;\
				(*(pos))++;\
			}\
		}\
} while(0UL)


/*
 * *************************************************
 * Define user copy ATOMIC OPERATIONS!!!!!!!!
 * WARNING!!! this function without direction
 * and platform dependent
 * *************************************************
 */
#define USER_DATA_COPY_REGISTER(n, from, to, pos) do {\
		*(to) = *(from);\
		(*(pos)) += n;\
} while(0UL)


#define ENTITY_SET_BIT(to, pos, type) (to) |= ((type)(1UL << (pos)))
#define ENTITY_RESET_BIT(to, pos, type) (to) &= ~((type)(1UL << (pos)))
#define ENTITY_TOGGLE_BIT(to, pos, type) (to) ^= (type)(1UL << (pos))
#define ENTITY_CHECK_BIT(from, pos) ((from) >> (pos)) & 1UL
#define ENTITY_WRITE_BIT(to, pos, value, type) (to) = (type)(((to) & ~(1UL << (pos))) | ((value) << (pos)));
//*********************************************


// type ------------------------------------------------------------------------------------------------
extern const u8 typeLengthMappingArray[TYPE_ARRAY_LENGTH];

// getting length of type
static inline u8 getTypeLen(u8* type) {
	if(*type < TYPE_ARRAY_LENGTH) {
		return typeLengthMappingArray[*type];
	}
	return 0;
}

static inline u8 getTypeLen_t(u8 type) {
	if(type < TYPE_ARRAY_LENGTH) {
		return typeLengthMappingArray[type];
	}
	return 0;
}


//-----------------------------------------------------------------------------------------------------------
// entity memory copy for n bytes
void entityPointerCopy(u8 n, u8* from, u8* to, u16* pos, b direction);
void entityPointerCopyOnce(u8 n, u8* from, u8* to, b direction);

void to_pos_copy(u8 n, u8* from, u8* to, u16* pos, u8 direction);
void from_pos_copy(u8 n, u8* from, u8* to, u16* pos, u8 direction);
void from_to_pos_copy(u8 n, u8* from, u8* to, u16* pos, u8 direction);

// is compleate string
u8 entityStrnCmp(const c8* str1, const c8* str2, u8 n);

// fill all data to 0
void entityPointerInit(u8 n, u8* ptr);
void entityPointerInit_t(u8 type, u8* ptr);

//-----------------------------------------------------------------------------------------------------------
// entity memory copy for type len bytes
void entityPointerCopy_t(u8 type, u8* from, u8* to, u16* pos, b direction);
void to_pos_copy_t(u8 type, u8* from, u8* to, u16* pos, u8 direction);
void from_pos_copy_t(u8 type, u8* from, u8* to, u16* pos, u8 direction);
void from_to_pos_copy_t(u8 type, u8* from, u8* to, u16* pos, u8 direction);

void entityPointerInit_tArray(u8 type, u8 len, u8* ptr);

//-----------------------------------------------------------------------------------------------------------


//pointer copy register type (bus aligned fast copy must specify reg, ENTITY_PERIF_BUS_TYPE_IND)
void entityPointerCopy_register(u8* from, u8* to, u16* pos);
void to_pos_copy_register(u8* from, u8* to, u16* pos);
void from_pos_copy_register(u8* from, u8* to, u16* pos);
void from_to_pos_copy_register(u8* from, u8* to, u16* pos);

//-----------------------------------------------------------------------------------------------------------
/*
 * ****************************
 * write / read functions
 * ****************************
 */

#define DATA_WRITE_T(type, value_ptr, data_ptr, pos_ptr, direction)  to_pos_copy_t(type, value_ptr, data_ptr, pos_ptr, direction)
#define DATA_READ_T(type, value_ptr, data_ptr, pos_ptr, direction)   from_pos_copy_t(type, data_ptr, value_ptr, pos_ptr, direction)
#define DATA_WRITE(n, value_ptr, data_ptr, pos_ptr, direction)  to_pos_copy(n, value_ptr, data_ptr, pos_ptr, direction)
#define DATA_READ(n, value_ptr, data_ptr, pos_ptr, direction)   from_pos_copy(n, data_ptr, value_ptr, pos_ptr, direction)

//unsigned ------------------------------------------
u8 readU8  (u8 *data, u16 *pos, b bn);
u16 readU16(u8 *data, u16 *pos, b bn);
u32 readU24(u8 *data, u16 *pos, b bn);
u32 readU32(u8 *data, u16 *pos, b bn);
u64 readU64(u8 *data, u16 *pos, b bn);

void writeU8 (u8 *data, u16 *pos, u8 value, b bn);
void writeU16(u8 *data, u16 *pos, u16 value, b bn);
void writeU24(u8 *data, u16 *pos, u32 value, b bn);
void writeU32(u8 *data, u16 *pos, u32 value, b bn);
void writeU64(u8 *data, u16 *pos, u64 value, b bn);

//signed ------------------------------------------
i8 readI8  (u8 *data, u16 *pos, b bn);
i16 readI16(u8 *data, u16 *pos, b bn);
i32 readI24(u8 *data, u16 *pos, b bn);
i32 readI32(u8 *data, u16 *pos, b bn);

void writeI8 (u8 *data, u16 *pos, i8 value, b bn);
void writeI16(u8 *data, u16 *pos, i16 value, b bn);
void writeI24(u8 *data, u16 *pos, i32 value, b bn);
void writeI32(u8 *data, u16 *pos, i32 value, b bn);

// floating -----------------------------------------
f32 readFloat   (u8 *data, u16 *pos, b bn);
f64 readDouble  (u8 *data, u16 *pos, b bn);

void writeFloat (u8 *data, u16 *pos, f32 value, b bn);
void writeDouble(u8 *data, u16 *pos, f64 value, b bn);

#endif /* INC_ENTITY_TYPES_H_ */

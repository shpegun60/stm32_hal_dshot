/*
 * entity_types.c
 *
 *  Created on: Dec 01, 2021
 *      Author: Shpegun60
 */

#include "entity_types.h"

const u8 typeLengthMappingArray[TYPE_ARRAY_LENGTH] = {
		//0     1     2     3     4     5     6	   7      8     9	 10    11    12    13    14    15
		0x00, 0x01, 0x02, 0x04, 0x08, 0x01, 0x01, 0x02, 0x04, 0x08, 0x04, 0x08, 0x10, 0x01, 0x03, 0x03};

/*
 * **************************************************************************************
 * pointer copy without type for n bytes
 * **************************************************************************************
 */
void entityPointerCopy(u8 n, u8* from, u8* to, u16* pos, b direction) {
	USER_DATA_COPY(n, from, to, pos, direction);
}

void entityPointerCopyOnce(u8 n, u8* from, u8* to, b direction) {
	reg pos = 0;
	USER_DATA_COPY(n, from, to, &pos, direction);
	(void)pos;
}

void to_pos_copy(u8 n, u8* from, u8* to, u16* pos, u8 direction) {
	u8 * ptr_to = (u8 *)(to + *pos);
	USER_DATA_COPY(n, from, ptr_to, pos, direction);
}

void from_pos_copy(u8 n, u8* from, u8* to, u16* pos, u8 direction) {
	u8 * ptr_from = (u8 *)(from + *pos);
	USER_DATA_COPY(n, ptr_from, to, pos, direction);
}

void from_to_pos_copy(u8 n, u8* from, u8* to, u16* pos, u8 direction) {
	u8 * ptr_from = (u8 *)(from + *pos);
	u8 * ptr_to = (u8 *)(to + *pos);
	USER_DATA_COPY(n, ptr_from, ptr_to, pos, direction);
}

void entityPointerInit(u8 n, u8* ptr) {
	while(n--) {
		*ptr++ = 0;
	}
}

u8 entityStrnCmp(const c8* str1, const c8* str2, u8 n) {
	while(n--) {
		if(*str1++ != *str2++) {
			return 1;
		}
	}
	return 0;
}


/*
 * **************************************************************************************
 * pointer copy with type for type-len bytes WARNING!!! NON ATOMIC OPERATIONS
 * **************************************************************************************
 */

void entityPointerCopy_t(u8 type, u8* from, u8* to, u16* pos, b direction) {
	if(type < TYPE_ARRAY_LENGTH) {
		u8 n = typeLengthMappingArray[type];
		USER_DATA_COPY(n, from, to, pos, direction);
	}
}

void to_pos_copy_t(u8 type, u8* from, u8* to, u16* pos, u8 direction) {
	if(type < TYPE_ARRAY_LENGTH) {
		u8 * ptr_to = (u8 *)(to + *pos);
		u8 n = typeLengthMappingArray[type];
		USER_DATA_COPY(n, from, ptr_to, pos, direction);
	}
}

void from_pos_copy_t(u8 type, u8* from, u8* to, u16* pos, u8 direction) {
	if(type < TYPE_ARRAY_LENGTH) {
		u8 * ptr_from = (u8 *)(from + *pos);
		u8 n = typeLengthMappingArray[type];
		USER_DATA_COPY(n, ptr_from, to, pos, direction);
	}
}

void from_to_pos_copy_t(u8 type, u8* from, u8* to, u16* pos, u8 direction) {
	if(type < TYPE_ARRAY_LENGTH) {
		u8 * ptr_from = (u8 *)(from + *pos);
		u8 * ptr_to = (u8 *)(to + *pos);
		u8 n = typeLengthMappingArray[type];
		USER_DATA_COPY(n, ptr_from, ptr_to, pos, direction);
	}
}

void entityPointerInit_t(u8 type, u8* ptr) {
	if(type < TYPE_ARRAY_LENGTH) {
		u8 n = typeLengthMappingArray[type];
		while(n--) {
			*ptr++ = 0;
		}
	}
}

void entityPointerInit_tArray(u8 type, u8 len, u8* ptr) {
	if(type < TYPE_ARRAY_LENGTH) {
		u8 n = typeLengthMappingArray[type] * len;
		while(n--) {
			*ptr++ = 0;
		}
	}
}


/*
 * **************************************************************************************
 * pointer copy register type (bus aligned fast copy must specify reg, ENTITY_PERIF_BUS_TYPE_IND)
 * **************************************************************************************
 */

void entityPointerCopy_register(u8* from, u8* to, u16* pos) {
	reg * tmp_from = (reg *)from;
	reg * tmp_to = (reg *)to;
	USER_DATA_COPY_REGISTER(typeLengthMappingArray[ENTITY_PERIF_BUS_TYPE_IND], tmp_from, tmp_to, pos);
}

void to_pos_copy_register(u8* from, u8* to, u16* pos) {
	u8 * ptr_to = (u8 *)(to + *pos);
	reg * tmp_from = (reg *)from;
	reg * tmp_to = (reg *)ptr_to;
	USER_DATA_COPY_REGISTER(typeLengthMappingArray[ENTITY_PERIF_BUS_TYPE_IND], tmp_from, tmp_to, pos);
}

void from_pos_copy_register(u8* from, u8* to, u16* pos) {
	u8 * ptr_from = (u8 *)(from + *pos);
	reg * tmp_from = (reg *)ptr_from;
	reg * tmp_to = (reg *)to;
	USER_DATA_COPY_REGISTER(typeLengthMappingArray[ENTITY_PERIF_BUS_TYPE_IND], tmp_from, tmp_to, pos);
}

void from_to_pos_copy_register(u8* from, u8* to, u16* pos) {
	u8 * ptr_from = (u8 *)(from + *pos);
	u8 * ptr_to = (u8 *)(to + *pos);
	reg * tmp_from = (reg *)ptr_from;
	reg * tmp_to = (reg *)ptr_to;
	USER_DATA_COPY_REGISTER(typeLengthMappingArray[ENTITY_PERIF_BUS_TYPE_IND], tmp_from, tmp_to, pos);
}

/*
 * **************************************************************************************
 * write / read functions
 * **************************************************************************************
 */

//unsigned ------------------------------------------
u8 readU8(u8 *data, u16 *pos, b bn) {
	u8 n = 1;
	u8 value = 0;
	u8 * ptr_from = (u8 *)(data + *pos);
	u8 * ptr_to = (u8 *)(&value);
	USER_DATA_COPY(n, ptr_from, ptr_to, pos, bn);
	return value;
}

u16 readU16(u8 *data, u16 *pos, b bn) {
	u8 n = 2;
	u16 value = 0;
	u8 * ptr_from = (u8 *)(data + *pos);
	u8 * ptr_to = (u8 *)(&value);
	USER_DATA_COPY(n, ptr_from, ptr_to, pos, bn);
	return value;
}

u32 readU24(u8 *data, u16 *pos, b bn) {
	u8 n = 3;
	u32 value = 0;
	u8 * ptr_from = (u8 *)(data + *pos);
	u8 * ptr_to = (u8 *)(&value);
	USER_DATA_COPY(n, ptr_from, ptr_to, pos, bn);
	return value;
}

u32 readU32(u8 *data, u16 *pos, b bn) {
	u8 n = 4;
	u32 value = 0;
	u8 * ptr_from = (u8 *)(data + *pos);
	u8 * ptr_to = (u8 *)(&value);
	USER_DATA_COPY(n, ptr_from, ptr_to, pos, bn);
	return value;
}

u64 readU64(u8 *data, u16 *pos, b bn) {
	u8 n = 8;
	u64 value = 0;
	u8 * ptr_from = (u8 *)(data + *pos);
	u8 * ptr_to = (u8 *)(&value);
	USER_DATA_COPY(n, ptr_from, ptr_to, pos, bn);
	return value;
}


void writeU8 (u8 *data, u16 *pos, u8 value, b bn) {
	u8 n = 1;
	u8 * ptr_from = (u8 *)(&value);
	u8 * ptr_to = (u8 *)(data + *pos);
	USER_DATA_COPY(n, ptr_from, ptr_to, pos, bn);
}


void writeU16(u8 *data, u16 *pos, u16 value, b bn) {
	u8 n = 2;
	u8 * ptr_from = (u8 *)(&value);
	u8 * ptr_to = (u8 *)(data + *pos);
	USER_DATA_COPY(n, ptr_from, ptr_to, pos, bn);
}

void writeU24(u8 *data, u16 *pos, u32 value, b bn) {
	u8 n = 3;
	u8 * ptr_from = (u8 *)(&value);
	u8 * ptr_to = (u8 *)(data + *pos);
	USER_DATA_COPY(n, ptr_from, ptr_to, pos, bn);
}

void writeU32(u8 *data, u16 *pos, u32 value, b bn) {
	u8 n = 4;
	u8 * ptr_from = (u8 *)(&value);
	u8 * ptr_to = (u8 *)(data + *pos);
	USER_DATA_COPY(n, ptr_from, ptr_to, pos, bn);
}

void writeU64(u8 *data, u16 *pos, u64 value, b bn) {
	u8 n = 8;
	u8 * ptr_from = (u8 *)(&value);
	u8 * ptr_to = (u8 *)(data + *pos);
	USER_DATA_COPY(n, ptr_from, ptr_to, pos, bn);
}

//signed ------------------------------------------
i8 readI8(u8 *data, u16 *pos, b bn) {
	u8 n = 1;
	i8 value = 0;
	u8 * ptr_from = (u8 *)(data + *pos);
	u8 * ptr_to = (u8 *)(&value);
	USER_DATA_COPY(n, ptr_from, ptr_to, pos, bn);
	return value;
}

i16 readI16(u8 *data, u16 *pos, b bn) {
	u8 n = 2;
	i16 value = 0;
	u8 * ptr_from = (u8 *)(data + *pos);
	u8 * ptr_to = (u8 *)(&value);
	USER_DATA_COPY(n, ptr_from, ptr_to, pos, bn);
	return value;
}

i32 readI24(u8 *data, u16 *pos, b bn) {
	u8 n = 3;
	i32 value = 0;
	u8 * ptr_from = (u8 *)(data + *pos);
	u8 * ptr_to = (u8 *)(&value);
	USER_DATA_COPY(n, ptr_from, ptr_to, pos, bn);

	if(value & 0x00800000) {
		value |= (u32)(0xFF000000);
	}
	return value;
}

i32 readI32(u8 *data, u16 *pos, b bn) {
	u8 n = 4;
	i32 value = 0;
	u8 * ptr_from = (u8 *)(data + *pos);
	u8 * ptr_to = (u8 *)(&value);
	USER_DATA_COPY(n, ptr_from, ptr_to, pos, bn);
	return value;
}


void writeI8 (u8 *data, u16 *pos, i8 value, b bn) {
	u8 n = 1;
	u8 * ptr_from = (u8 *)(&value);
	u8 * ptr_to = (u8 *)(data + *pos);
	USER_DATA_COPY(n, ptr_from, ptr_to, pos, bn);
}


void writeI16(u8 *data, u16 *pos, i16 value, b bn) {
	u8 n = 2;
	u8 * ptr_from = (u8 *)(&value);
	u8 * ptr_to = (u8 *)(data + *pos);
	USER_DATA_COPY(n, ptr_from, ptr_to, pos, bn);
}

void writeI24(u8 *data, u16 *pos, i32 value, b bn) {
	if(value & 0x80000000) {
		value |= (u32)(0xFF800000);
	}

	u8 n = 3;
	u8 * ptr_from = (u8 *)(&value);
	u8 * ptr_to = (u8 *)(data + *pos);
	USER_DATA_COPY(n, ptr_from, ptr_to, pos, bn);
}

void writeI32(u8 *data, u16 *pos, i32 value, b bn) {
	u8 n = 4;
	u8 * ptr_from = (u8 *)(&value);
	u8 * ptr_to = (u8 *)(data + *pos);
	USER_DATA_COPY(n, ptr_from, ptr_to, pos, bn);
}


// floating -----------------------------------------
f32 readFloat(u8 *data, u16 *pos, b bn) {
	u8 n = 4;
	f32 value = 0.0;
	u8 * ptr_from = (u8 *)(data + *pos);
	u8 * ptr_to = (u8 *)(&value);
	USER_DATA_COPY(n, ptr_from, ptr_to, pos, bn);
	return value;
}


void writeFloat(u8 *data, u16 *pos, f32 value, b bn) {
	u8 n = 4;
	u8 * ptr_from = (u8 *)(&value);
	u8 * ptr_to = (u8 *)(data + *pos);
	USER_DATA_COPY(n, ptr_from, ptr_to, pos, bn);
}

f64 readDouble(u8 *data, u16 *pos, b bn) {
	u8 n = 8;
	f64 value = 0.0;
	u8 * ptr_from = (u8 *)(data + *pos);
	u8 * ptr_to = (u8 *)(&value);
	USER_DATA_COPY(n, ptr_from, ptr_to, pos, bn);
	return value;
}


void writeDouble(u8 *data, u16 *pos, f64 value, b bn) {
	u8 n = 8;
	u8 * ptr_from = (u8 *)(&value);
	u8 * ptr_to = (u8 *)(data + *pos);
	USER_DATA_COPY(n, ptr_from, ptr_to, pos, bn);
}


/*
 * entity_flash_writer.h
 *
 *  Created on: Dec 01, 2021
 *      Author: Shpegun60
 */


#ifndef INC_ENTITY_I2C_FLASH_WRITER_H_
#define INC_ENTITY_I2C_FLASH_WRITER_H_


//#define I2C_EEPROM
//****************************************************************************************************************************
// CROSSPLATFORM SETTING
//****************************************************************************************************************************
#ifdef I2C_EEPROM
	#include "eepromi2c.h"

	#define EEPROM_OBJ_TYPE eepromI2C_t
	#define EEPROM_READ(obj, addr, data_ptr, size) eepromI2C_Read((obj), (addr), (data_ptr), (size))
	#define EEPROM_WRITE_BYTE(obj, addr, byte_data) eepromI2C_WriteByte(obj, addr, byte_data)
	#define EEPROM_WRITE(obj, addr, data_ptr, size) eepromI2C_Write((obj), (addr), (data_ptr), (size))
#else
	#define EEPROM_OBJ_TYPE void
	#define EEPROM_READ(obj, addr, data_ptr, size)
	#define EEPROM_WRITE_BYTE(obj, addr, byte_data)
	#define EEPROM_WRITE(obj, addr, data_ptr, size)
#endif

#include "entity_types.h"
#include "entity_header.h"

#ifdef USE_ENTITY_FLASH

// ---------------------------------------------------------------------------------------------------------------------------

/*
 * memory map
 * [DATA_TO_START_ADDRESS : 1] [entities_count:2] [Total Fields Write To Flash Count:4] [end of data POINTER:2] :::> | [entity num 0: 2] [field count:1] [type:1] [bit flag:1]..... [entity num X: 2] [field count:1] [type:1] [bit flag:1].....
 *
 * entity map pos = START_ADDRESS + 9 + (entities_count * 3) + totalFieldCount + 1;
 *
 * entity data map
 * [data1: X] ... [dataN: X]
 *
 * log data
 * [Total Fields Write To Log Count:4] [sizeof one log:4] [log data1: X] ... [log dataN: X]
 *
 */

// FLASH START MAP ------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define START_ADDRESS 			((u32)0x00)
#define DATA_TO_START_ADDRESS 	((u32)0x5B)

#define SIZEOF_ENTITIES_COUNT 	((u32)sizeof(entities_count)) //0x02
#define ENTITIES_COUNT_ADDRESS 	((u32)(START_ADDRESS + 0x01))

#define SIZEOF_TOTAL_FIELD_COUNT			((u32)sizeof(u32)) //0x04
#define TOTAL_FIELD_COUNT_ADDRESS 			((u32)(ENTITIES_COUNT_ADDRESS + SIZEOF_ENTITIES_COUNT))

#define	SIZEOF_END_OF_DATA_POINTER			((u32)sizeof(u16)) //0x02
#define END_OF_DATA_POINTER_ADDRESS 		((u32)(TOTAL_FIELD_COUNT_ADDRESS + SIZEOF_TOTAL_FIELD_COUNT))

// ENTITY MAP ----------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define SIZEOF_ONE_FIELDS_COUNT			((u32)sizeof(((Entity *)(0))->fields_count)) //0x01
#define SIZEOF_BIT_FLAGS				((u32)sizeof(((EntityField *)(0))->bitFlags))
#define SIZEOF_TYPE						((u32)sizeof(((EntityField *)(0))->type))

#define ENTITY_MAP_ADDRESS 		((u32)(END_OF_DATA_POINTER_ADDRESS + SIZEOF_END_OF_DATA_POINTER))

#define ENTITY_MAP_HEADER_LENGTH ((u32) SIZEOF_ONE_FIELDS_COUNT + SIZEOF_ENTITIES_COUNT)
#define ENTITY_MAP_FLAG_LENGTH 	 ((u32) SIZEOF_BIT_FLAGS + SIZEOF_TYPE)
#define GET_DATA_ADDRESS(totalEntitiesCount, totalFieldCount) (ENTITY_MAP_ADDRESS + ((totalEntitiesCount) * ENTITY_MAP_HEADER_LENGTH) + ((totalFieldCount) * ENTITY_MAP_FLAG_LENGTH) + 4)

// ENTITY LOG ---------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define SIZEOF_TIME_LOG			((u8) 7) //0x04
#define SIZEOF_ONE_LOG			((u8)sizeof(u32)) //0x04

#define SIZEOF_LOG_COUNTER			((u8)sizeof(u32)) //0x04
#define LOG_COUNTER_SHIFT 			(SIZEOF_TOTAL_FIELD_COUNT + SIZEOF_ONE_LOG)

#define MAXIMUM_LOG_CELLS_MSK ((u8)0xFF)

// MAIN FUNCTION ----------------------------------------------------------------------------------------------------------------------------------------------------------------------

void readEntitiesFromFlash(EEPROM_OBJ_TYPE *eeprom);
void writeEntitiesToFlash(EEPROM_OBJ_TYPE *eeprom);
void writeLogEntitiesToFlash(EEPROM_OBJ_TYPE *eeprom, u8 SS, u8 MI, u8 HH, u8 DD, u8 MM, u8 YY);
void readLogEntitiesFromFlash(EEPROM_OBJ_TYPE *eeprom);
#endif

#endif /* INC_ENTITY_I2C_FLASH_WRITER_H_ */

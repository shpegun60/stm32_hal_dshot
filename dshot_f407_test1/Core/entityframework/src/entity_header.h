/*
 * entity_header.h
 *
 *  Created on: Dec 01, 2021
 *      Author: Shpegun60
 */


#ifndef ENTITY_HEADER_H_
#define ENTITY_HEADER_H_

// defined entity prefix and null ptr ------------------------------------------------
#define ENTITY_PREF(x)                     entity_ ## x
#define ENTITY_PREF_CAPS(x)                ENTITY_ ## x


#ifndef NULL
	#define NULL ((void *)0)
#endif

#ifndef MIN
	#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
	#define MAX(a,b) (((a) < (b)) ? (b) : (a))
#endif

#ifndef ABS
	#define ABS(N) (((N) < 0.0) ? (-1.0 * (N)) : (N))
#endif



//-----------------------------------------------------------------------------------------------------------------------------------
#define ENTITY_DIR_COPY ENTITY_LSB
#define ENTITY_DECRIPTION_LEN 	((u8) 0x04)
//-----------------------------------------------------------------------------------------------------------------------------------

#define NULL_CMD ((u8) 0x00)
#define DATA_CMD ((u8) 0x01)

#define ENTITY_ERROR ((u16) 0xFFFF)
#define ENTITY_OK 	 ((u16) 0x0001)
//------------------------------------



/*
 * **********************************************************************************************************************************
 *  define use callbacks
 *
 *  if you want use read and write callbacks uncommit this define
 *  WARNING!!!! This feature adds 4 * sizeof(void pointer) byte to each EntityField
 * **********************************************************************************************************************************
*/
#ifndef USE_ENTITY_CALLBACKS
	#define USE_ENTITY_CALLBACKS
#endif


/*
 * **********************************************************************************************************************************
 *  define use atomic, register, pointer operations
 *  WARNING!!! USER must be specify entity_atomic.h file and bus type in entity_types.h (reg, ENTITY_PERIF_BUS_TYPE_IND)
 * **********************************************************************************************************************************
*/
#ifndef USE_ENTITY_ATOMIC
	//#define USE_ENTITY_ATOMIC
#endif

#ifndef USE_ENTITY_REGISTER
	//#define USE_ENTITY_REGISTER
#endif

#ifndef USE_ENTITY_POINTER
	#define USE_ENTITY_POINTER
#endif

/*
 * **********************************************************************************************************************************
 *  define use flash operations
 *  WARNING!!! USER must be specify in entity_flash_writer.h next defines
 *  #define EEPROM_OBJ_TYPE void
 *	#define EEPROM_READ(obj, addr, data_ptr, size)
 *	#define EEPROM_WRITE_BYTE(obj, addr, byte_data)
 *	#define EEPROM_WRITE(obj, addr, data_ptr, size)
 * **********************************************************************************************************************************
*/
#ifndef USE_ENTITY_FLASH
	#define USE_ENTITY_FLASH
#endif

/*
 * **********************************************************************************************************************************
 *  define use entity errors handling
 *  WARNING!!!! This feature adds any bytes in .bss
 * **********************************************************************************************************************************
*/
#ifndef USE_ENTITY_ERRORS_HANDLING
	//#define USE_ENTITY_ERRORS_HANDLING
#endif

//-----------------------------------------------------------------------------------------------------------------------------------


#endif /* ENTITY_HEADER_H_ */

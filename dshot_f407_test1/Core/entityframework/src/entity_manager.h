/*
 * entity_manager.h
 *
 *  Created on: Dec 01, 2021
 *      Author: Shpegun60
 */


#ifndef INC_ENTITY_MANAGER_H_
#define INC_ENTITY_MANAGER_H_

#include "entity_flash_writer.h"
#include "entity_types.h"
#include "entity_header.h"
#include "entity_error_handler.h"


// bit flags mask
#define EN_EMPTY_FLAG    	((u8)0x00) // nothing (ordinary field)
#define EN_READ_ONLY_MSK 	((u8)0x01) // its read only field (write protected field)
#define EN_PARAM_MSK 		((u8)0x02) // its parameter field (writed to flash once and read before start, must uncommitted USE_ENTITY_FLASH)
#define EN_LOG_MSK 			((u8)0x04) // its log (writed to flash continious, must uncommitted USE_ENTITY_FLASH)
#define EN_POINTER_MSK 		((u8)0x08) // its pointer, must specify reg in entity_types.h (reading and writing from addres which store in this field)
#define EN_REGISTER_MSK 	((u8)0x10) // its register, must specify reg and ENTITY_PERIF_BUS_TYPE_IND in entity_types.h (bus aligned read and write, it happens for one insrtuction, must uncommitted USE_ENTITY_REGISTER)
#define EN_ATOMIC_MSK 		((u8)0x20) // its atomic field must specify entity_atomic.h file (before write and read disabling interrupts and after restore it, must uncommitted USE_ENTITY_ATOMIC)
#define EN_ALL_BIT_MSK 		((u8)0xFF) // its mask for counting all fields

#ifdef USE_ENTITY_CALLBACKS
// ---------------------
#define EN_WRITE_CALLBACK ((u8)0x00)
#define EN_READ_CALLBACK  ((u8)0x01)
//--------------------------------------
#endif //USE_ENTITY_CALLBACKS


typedef struct {
#ifdef USE_ENTITY_CALLBACKS
	/**
	  * @brief  entityCallback[x](Entity, Field, args). Pointer to function array callbacks. [0] -> write callback; [1] -> read callback
	  * @param  Entity_ptr - void pointer to Entity (programmer must cast to *Entity)
	  * @param  Field_ptr  - void pointer to EntityField (programmer must cast to *EntityField)
	  * @param  args   - pointer to data for callaback. [0] - write args; [1] - read args (programmer must cast to other last defined value)
	  * @retval None
	  */
	void (*entityCallback[2])(void* Entity_ptr, void* Field_ptr, void* args);
	void * args[2]; // callbacs arguments pointer; [0] - write args, [1] - read args
#endif // USE_ENTITY_CALLBACKS
	/**
	 * @brief  entityCallback[x](Entity, Field, args). Pointer to function array callbacks. [0] -> write callback; [1] -> read callback see callback helpers
	 * bit[0] - read only, bit[1] - isParam, bit[2] - is log, bit[3] - is pointer ... see bit flags mask
	 */
	u8 bitFlags;
	u16 shift;
	u8 type;
	char descr[ENTITY_DECRIPTION_LEN];
} EntityField;

typedef struct {
	void *pointer;
	char descr[ENTITY_DECRIPTION_LEN];
	u8 fields_count;
	EntityField* fields;
} Entity;


extern u16 allocated_entity_pointers;
extern u16 entities_count;
extern Entity ** entities;


/*
 * **********************************************************************************************************************************
 *  help functions
 * **********************************************************************************************************************************
*/

// getter entity pointer
static inline Entity * getEntityPointer(u16 entity_num) {
	if(entities_count > entity_num) {
		return entities[entity_num];
	}
	return 0;
}

// getter data pointer
static inline void * getVoidPointer(u16 entity_num) {
	if(entities_count > entity_num) {
		return entities[entity_num]->pointer;
	}
	return 0;
}

// clear all entities and deallocation all memory
void clearEntities();

// clear some entity
void clearEntitityFields(u16 EntityNumber);

/*
 * **********************************************************************************************************************************
 *  entity functions
 * **********************************************************************************************************************************
*/

// malloc new entities pointers
u16 newEntities(u16 nomberOfEntities);

// malloc entitites pointer & fields
u16 initEntity(u8 NumberOfFields, u16 pointerSize, char * descr, b isUserSpace, void * arg);

/*
 * **********************************************************************************************************************************
 *  field functions
 * **********************************************************************************************************************************
*/

// init existing field with auto number incrementation
u8 initFieldShiftPtr(Entity * entityInst, u8 * filedNum, u8 bitFlags, u16 * shift, u8 type, char * descr, void * field_ptr) ;

// init existing field without auto shift incrementation
u8 initField(Entity * entityInst, u8 * filedNum, u8 bitFlags, u16 shift, u8 type, char * descr, void * field_ptr);

// init existing field by field ptr
u8 initFieldPtr(EntityField * fieldInst, u8 bitFlags, u16 * shift, u8 type, char * descr);

u8 initFieldArray(Entity * entityInst, u8 * filedNum, u8 bitFlags, u16 shift, u8 type, u8 arrayLen, char * descr, void * field_ptr, int startNum);

// rename field function
u8 fieldRename(Entity * entityInst, u8 fieldNumber, char * descr);

#ifdef USE_ENTITY_CALLBACKS
/*
 * ****************************************************************************************************
 * callBack functions
 * ***************************************************************************************************
 */

// init callback function by fieldNumber
u8 initCallback(Entity * entityInst, u8 filedNum, void (*readCallback)(void*, void*, void*), void* rArgs, void (*writeCallback)(void*, void*, void*), void* wArgs);

// init callback function by description
u8 initCallback_txt(Entity * entityInst, char* desc, void (*readCallback)(void*, void*, void*), void* rArgs, void (*writeCallback)(void*, void*, void*), void* wArgs);

#endif //USE_ENTITY_CALLBACKS

/*
 * **********************************************************************************************************************************
 *  data functions
 * **********************************************************************************************************************************
*/
void read_entities_descriptions(u16 startEntityNum, u8 len, u8 *outputData, u16 *size);
void read_entity_fields(u16 entityNum, u8 startFieldNum, u8 len, u8 *outputData, u16 *size);
void read_field_value(u16 entityNum, u8 filedNum, u8 *outputData, u16 *size);
void set_field_value(u16 entityNum, u8 filedNum, u8* data);
void read_several_fields_values(u8 *inputData, u8 *outputData, u16 *size);
void set_several_fields_values(u8 *inputData, u8 *outputData, u16 *size);

#ifdef USE_ENTITY_FLASH
void set_several_bit_flags(u8 *inputData, u8 *outputData, u16 *size, EEPROM_OBJ_TYPE *eeprom);
#endif //USE_ENTITY_FLASH

#endif /* INC_ENTITY_MANAGER_H_ */

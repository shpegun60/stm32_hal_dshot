/*
 * entity_error_handler.h
 *
 *  Created on: Dec 01, 2021
 *      Author: Shpegun60
 */

#ifndef ENTITY_ERROR_HANDLER_H_
#define ENTITY_ERROR_HANDLER_H_


#include "entity_header.h"
#include "entity_types.h"

#ifdef USE_ENTITY_ERRORS_HANDLING
#define MAXIMUM_NUMBER_OF_ERROR_ENTITY_OBJ 		2
#define ENTITY_ERROR_BIT_TYPE_NUM				UINT32_TYPE
#define ENTITY_ERROR_BIT_TYPE					u32

#define ENTITY_IS_CRITICAL_ERROR_MSK 	((u8)(0x01))
#define ENTITY_IS_FAST_ERROR_MSK 		((u8)(0x02))

typedef struct {
	u8 (*entityErrorHandler)(void * data, u32 time); // (0) --> data; (3) --> time
	void * pointerData;

	u32 minErrorTime;
	u32 lastNotErrorTime;
	u32 lastErrorTime;
	u8 errorBitFlags;
} errorField_t;


typedef struct {
	ENTITY_ERROR_BIT_TYPE errorBits;
	ENTITY_ERROR_BIT_TYPE onlyCriticalErrorMSK;

	errorField_t * errorsArray;
	u16 entityNum;
	u8 usedPos;
	u8 size;
} errObj_t;

void deleteAllErrors(void);
void clearAllErrors(u32 time);
void clearError(errObj_t * errorObjPointer, u32 time);


u16 createErrors(errObj_t * errorObjPointer, u8 numberOfErrors,  c8 * descr);
u16 initError(errObj_t * errorObjPointer, u8 (*errorHandler)(void * data, u32 time), void * pointerData, u8 flags, u32 errorTime, c8 * errorDescr);
void entityErrorProceed(errObj_t * errorObjPointer, u32 currentTime);
#endif

#endif /* ENTITY_ERROR_HANDLER_H_ */

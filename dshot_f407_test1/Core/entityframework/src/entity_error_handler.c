/*
 * entity_error_handler.c
 *
 *  Created on: Dec 01, 2021
 *      Author: Shpegun60
 */


#include "entity_error_handler.h"
#include "entity_manager.h"
#include <stdlib.h>

#ifdef USE_ENTITY_ERRORS_HANDLING
errObj_t * ErrorsObj[MAXIMUM_NUMBER_OF_ERROR_ENTITY_OBJ] = {NULL};
u8 entityErrorCounter = 0;


void deleteAllErrors(void) {
	for(u8 i = 0; i < entityErrorCounter; ++i) {
		free(ErrorsObj[i]->errorsArray);
		ErrorsObj[i] = NULL;
	}
	entityErrorCounter = 0;
}

void clearAllErrors(u32 time) {
	for(u8 i = 0; i < entityErrorCounter; ++i) {
		if(ErrorsObj[i]) {
			u8 * ptr = (u8 *) getVoidPointer(ErrorsObj[i]->entityNum);
			entityPointerInit(ErrorsObj[i]->size, ptr);

			ErrorsObj[i]->errorBits = 0;
			for(u8 j = 0; j < ErrorsObj[i]->size; ++j) {
				ErrorsObj[i]->errorsArray[j].lastErrorTime = time;
				ErrorsObj[i]->errorsArray[j].lastNotErrorTime = time;
			}
		}
	}
}

void clearError(errObj_t * errorObjPointer, u32 time) {
	if(!errorObjPointer) {
		return;
	}

	u8 * ptr = (u8 *) getVoidPointer(errorObjPointer->entityNum);
	entityPointerInit(errorObjPointer->size, ptr);

	errorObjPointer->errorBits = 0;

	for(u8 i = 0; i < errorObjPointer->size; ++i) {
		errorObjPointer->errorsArray[i].lastErrorTime = time;
		errorObjPointer->errorsArray[i].lastNotErrorTime = time;
	}
}



u16 createErrors(errObj_t * errorObjPointer, u8 numberOfErrors,  c8 * descr) {
	if(!errorObjPointer || !descr || (numberOfErrors > getTypeLen_t(ENTITY_ERROR_BIT_TYPE_NUM) * 8) || (entityErrorCounter == MAXIMUM_NUMBER_OF_ERROR_ENTITY_OBJ)) {
		return ENTITY_ERROR;
	}

	errorObjPointer->entityNum = initEntity((numberOfErrors + 1), numberOfErrors + getTypeLen_t(ENTITY_ERROR_BIT_TYPE_NUM), descr, 0, 0);

	if(ENTITY_ERROR == errorObjPointer->entityNum) {
		return ENTITY_ERROR;
	}

	u8 pos = 0;
	Entity * tempEntity = getEntityPointer(errorObjPointer->entityNum);
	u8 * ptr = (u8 *) tempEntity->pointer;
	ENTITY_ERROR_BIT_TYPE * ptr_msk = (ENTITY_ERROR_BIT_TYPE *)(ptr + numberOfErrors);

	initFieldArray(tempEntity, &pos, (EN_READ_ONLY_MSK), 0, UINT8_TYPE, numberOfErrors, "E", ptr, 0);
	initField(tempEntity, &pos, (EN_PARAM_MSK), numberOfErrors, ENTITY_ERROR_BIT_TYPE_NUM, "Emsk", ptr_msk);
	*ptr_msk = (ENTITY_ERROR_BIT_TYPE)(0xFFFFFFFFFFFFFFFF);

	errorObjPointer->errorsArray = (errorField_t*) calloc(numberOfErrors, sizeof(errorField_t));
	if(NULL == errorObjPointer->errorsArray) {
		clearEntitityFields(errorObjPointer->entityNum);
		return ENTITY_ERROR;
	}
	errorObjPointer->size = numberOfErrors;
	errorObjPointer->errorBits = 0;
	errorObjPointer->usedPos = 0;
	errorObjPointer->onlyCriticalErrorMSK = 0;
	ErrorsObj[entityErrorCounter] = errorObjPointer;
	entityErrorCounter++;
	return (entityErrorCounter - 1);
}


u16 initError(errObj_t * errorObjPointer, u8 (*errorHandler)(void * data, u32 time), void * pointerData, u8 flags, u32 errorTime, c8 * errorDescr) {
	if(!errorObjPointer || errorObjPointer->usedPos == errorObjPointer->size || !errorObjPointer->errorsArray) {
		return ENTITY_ERROR;
	}

	errorObjPointer->errorsArray[errorObjPointer->usedPos].entityErrorHandler = errorHandler;
	errorObjPointer->errorsArray[errorObjPointer->usedPos].pointerData = pointerData;
	errorObjPointer->errorsArray[errorObjPointer->usedPos].minErrorTime = errorTime;
	errorObjPointer->errorsArray[errorObjPointer->usedPos].lastErrorTime = 0;
	errorObjPointer->errorsArray[errorObjPointer->usedPos].lastNotErrorTime = 0;
	errorObjPointer->errorsArray[errorObjPointer->usedPos].errorBitFlags = flags;


	if(errorDescr != NULL) {
		fieldRename(getEntityPointer(errorObjPointer->entityNum), errorObjPointer->usedPos, errorDescr);
	}

	errorObjPointer->onlyCriticalErrorMSK |= (flags & ENTITY_IS_CRITICAL_ERROR_MSK) ? (ENTITY_ERROR_BIT_TYPE)(1UL << errorObjPointer->usedPos) : (ENTITY_ERROR_BIT_TYPE)(0UL);


	errorObjPointer->usedPos++;
	return (errorObjPointer->usedPos - 1);
}

void entityErrorProceed(errObj_t * errorObjPointer, u32 currentTime) {
	if(!errorObjPointer) {
		return;
	}
	u8 * ptr = (u8 *) getVoidPointer(errorObjPointer->entityNum);
	if(!ptr || !errorObjPointer->errorsArray) {
		return;
	}

	ENTITY_ERROR_BIT_TYPE * ptr_msk = (ENTITY_ERROR_BIT_TYPE *)(ptr + errorObjPointer->size);

	u8 tmp = 0;
	for(u32 i = 0; i < errorObjPointer->usedPos; ++i) {
		if(!errorObjPointer->errorsArray[i].entityErrorHandler) {
			continue;
		}

		tmp = errorObjPointer->errorsArray[i].entityErrorHandler(errorObjPointer->errorsArray[i].pointerData, currentTime) & (((*ptr_msk) >> i) & 0x01);

		if(errorObjPointer->errorsArray[i].errorBitFlags & ENTITY_IS_FAST_ERROR_MSK) {
			if(tmp) {
				ENTITY_SET_BIT(errorObjPointer->errorBits, i, ENTITY_ERROR_BIT_TYPE);
			} else {
				ENTITY_RESET_BIT(errorObjPointer->errorBits, i, ENTITY_ERROR_BIT_TYPE);
			}
			ptr[i] = tmp;
		} else {

			if((errorObjPointer->errorsArray[i].lastNotErrorTime > currentTime) || (errorObjPointer->errorsArray[i].lastErrorTime > currentTime)) { // simple overflow checking
				errorObjPointer->errorsArray[i].lastNotErrorTime = currentTime;
				errorObjPointer->errorsArray[i].lastErrorTime = currentTime;
				continue;
			} else if(tmp) {
				errorObjPointer->errorsArray[i].lastErrorTime = currentTime;
			} else {
				errorObjPointer->errorsArray[i].lastNotErrorTime = currentTime;
			}

			if((currentTime - errorObjPointer->errorsArray[i].lastNotErrorTime) > errorObjPointer->errorsArray[i].minErrorTime) {
				ptr[i] = 1;
				ENTITY_SET_BIT(errorObjPointer->errorBits, i, ENTITY_ERROR_BIT_TYPE);
			} else if((currentTime - errorObjPointer->errorsArray[i].lastErrorTime) > errorObjPointer->errorsArray[i].minErrorTime) {
				ptr[i] = 0;
				ENTITY_RESET_BIT(errorObjPointer->errorBits, i, ENTITY_ERROR_BIT_TYPE);
			}
		}
	}
}



#endif

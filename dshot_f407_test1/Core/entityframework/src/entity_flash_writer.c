/*
 * entity_flash_writer.c
 *
 *  Created on: Dec 01, 2021
 *      Author: Shpegun60
 */

#include "entity_flash_writer.h"
#include "entity_manager.h"

#ifdef USE_ENTITY_FLASH
//*****************************************************************
// helpers ---------------------------------------------------
//*****************************************************************

u32 getTotalFieldsCount(u8 msk) {
	u32 totalFieldCount = 0;

	if(msk == EN_ALL_BIT_MSK) {
		for(u32 i = 0; i < entities_count; ++i) {
			totalFieldCount += entities[i]->fields_count;
		}
	} else {
		for(u32 i = 0; i < entities_count; ++i) {
			for(u32 j = 0; j < entities[i]->fields_count; ++j) {
				if(entities[i]->fields[j].bitFlags & msk) {
					totalFieldCount++;
				}
			}
		}
	}
	return totalFieldCount;
}

u32 getTotalFieldsSize(u8 msk) {
	u32 totalFieldSize = 0;

	if(msk == EN_ALL_BIT_MSK) {
		for(u32 i = 0; i < entities_count; ++i) {
			for(u32 j = 0; j < entities[i]->fields_count; ++j) {
				totalFieldSize += getTypeLen_t(entities[i]->fields[j].type);
			}
		}
	} else {
		for(u32 i = 0; i < entities_count; ++i) {
			for(u32 j = 0; j < entities[i]->fields_count; ++j) {
				if(entities[i]->fields[j].bitFlags & msk) {
					totalFieldSize += getTypeLen_t(entities[i]->fields[j].type);
				}
			}
		}
	}
	return totalFieldSize;
}



u16 getStartLogPointer(EEPROM_OBJ_TYPE *eeprom) {
	u16 readByte = 0;
	EEPROM_READ(eeprom, END_OF_DATA_POINTER_ADDRESS, (u8*)&readByte, SIZEOF_END_OF_DATA_POINTER);
	return readByte;
}


// check is flash writed ? ------------------------------
u8 checkFlash(EEPROM_OBJ_TYPE *eeprom) {
	u32 tmpData = 0;

	EEPROM_READ(eeprom, START_ADDRESS, (u8*)&tmpData, 1);
	if(DATA_TO_START_ADDRESS != (u8)tmpData) {
		return 0;
	}

	// read entities count
	EEPROM_READ(eeprom, ENTITIES_COUNT_ADDRESS, (u8*)&tmpData, SIZEOF_ENTITIES_COUNT);
	if(entities_count != (u16)tmpData) {
		return 0;
	}

	// read total writed fields count
	EEPROM_READ(eeprom, TOTAL_FIELD_COUNT_ADDRESS, (u8*)&tmpData, SIZEOF_TOTAL_FIELD_COUNT);
	if(getTotalFieldsCount(EN_PARAM_MSK) != tmpData) {
		return 0;
	}

	return 1;
}



//*****************************************************************
// read / write parameters
//*****************************************************************

void readEntitiesFromFlash(EEPROM_OBJ_TYPE *eeprom) {
	if(eeprom && checkFlash(eeprom)) {
		//---------------------------------------------------------------------

		u32 totalFieldCount = getTotalFieldsCount(EN_ALL_BIT_MSK);
		u16 tmp_MapAddress = ENTITY_MAP_ADDRESS;
		u16 tmp_DataAdress = GET_DATA_ADDRESS(entities_count, totalFieldCount);
		u32 tmpData = 0;

		// read entity data

		for(u32 i = 0; i < entities_count; ++i) {
			EEPROM_READ(eeprom, tmp_MapAddress, (u8*)&tmpData, SIZEOF_ENTITIES_COUNT);
			if(i != (u16)tmpData) {
				return;
			}
			tmp_MapAddress += SIZEOF_ENTITIES_COUNT;

			EEPROM_READ(eeprom, tmp_MapAddress, (u8*)&tmpData, SIZEOF_ONE_FIELDS_COUNT);
			if(entities[i]->fields_count != (u8)tmpData) {
				return;
			}
			tmp_MapAddress += SIZEOF_ONE_FIELDS_COUNT;

			for(u32 j = 0; j < entities[i]->fields_count; ++j) {
				EEPROM_READ(eeprom, tmp_MapAddress, (u8*)&tmpData, SIZEOF_TYPE);
				if(entities[i]->fields[j].type != (u8)tmpData) {
					return;
				}
				tmp_MapAddress += SIZEOF_TYPE;

				EEPROM_READ(eeprom, tmp_MapAddress, (u8*)&tmpData, SIZEOF_BIT_FLAGS);
				if(entities[i]->fields[j].bitFlags != (u8)tmpData) {
					return;
				}
				tmp_MapAddress += SIZEOF_BIT_FLAGS;

				if(entities[i]->fields[j].bitFlags & EN_PARAM_MSK) {
					u8 len = getTypeLen_t(entities[i]->fields[j].type);
					u8 * ptr = (u8 *)(entities[i]->pointer + entities[i]->fields[j].shift);

#ifdef USE_ENTITY_POINTER
					if(entities[i]->fields[j].bitFlags & EN_POINTER_MSK) {
						reg * data_reg = (reg *)ptr;
						if(*data_reg) EEPROM_READ(eeprom, tmp_DataAdress, (u8 *)(*data_reg), len);
					} else {
#endif //USE_ENTITY_POINTER
						EEPROM_READ(eeprom, tmp_DataAdress, ptr, len);
#ifdef USE_ENTITY_POINTER
					}
#endif//USE_ENTITY_POINTER
					tmp_DataAdress += len;
				}
			}
		}

		//---------------------------------------------------------------------
	}
}



void writeEntitiesToFlash(EEPROM_OBJ_TYPE *eeprom) {
	if(eeprom) {

		//clear first byte
		EEPROM_WRITE_BYTE(eeprom, START_ADDRESS, 0x00);
		//--------------------------------------------------------------------------

		{
			u32 totalFieldCount = getTotalFieldsCount(EN_ALL_BIT_MSK);
			u16 tmp_MapAddress = ENTITY_MAP_ADDRESS;
			u16 tmp_DataAdress = GET_DATA_ADDRESS(entities_count, totalFieldCount);

			// write entities count
			EEPROM_WRITE(eeprom, ENTITIES_COUNT_ADDRESS, (u8*)&entities_count, SIZEOF_ENTITIES_COUNT);

			// write total fields count
			totalFieldCount = getTotalFieldsCount(EN_PARAM_MSK);
			EEPROM_WRITE(eeprom, TOTAL_FIELD_COUNT_ADDRESS, (u8*)&totalFieldCount, SIZEOF_TOTAL_FIELD_COUNT);

			// write entity data
			for(u32 i = 0; i < entities_count; ++i) {

				EEPROM_WRITE(eeprom, tmp_MapAddress, (u8*)&i, SIZEOF_ENTITIES_COUNT);
				tmp_MapAddress += SIZEOF_ENTITIES_COUNT;
				EEPROM_WRITE(eeprom, tmp_MapAddress, (u8*)&entities[i]->fields_count, SIZEOF_ONE_FIELDS_COUNT);
				tmp_MapAddress += SIZEOF_ONE_FIELDS_COUNT;

				for(u32 j = 0; j < entities[i]->fields_count; ++j) {
					EEPROM_WRITE(eeprom, tmp_MapAddress, &entities[i]->fields[j].type, SIZEOF_TYPE);
					tmp_MapAddress += SIZEOF_TYPE;
					EEPROM_WRITE(eeprom, tmp_MapAddress, &entities[i]->fields[j].bitFlags, SIZEOF_BIT_FLAGS);
					tmp_MapAddress += SIZEOF_BIT_FLAGS;

					if(entities[i]->fields[j].bitFlags & EN_PARAM_MSK) {
						u8 len = getTypeLen_t(entities[i]->fields[j].type);
						u8 * ptr = (u8 *)(entities[i]->pointer + entities[i]->fields[j].shift);

#ifdef USE_ENTITY_POINTER
						if(entities[i]->fields[j].bitFlags & EN_POINTER_MSK) {
							reg * data_reg = (reg *)ptr;
							if(*data_reg) EEPROM_WRITE(eeprom, tmp_DataAdress, (u8 *)(*data_reg), len);
						} else {
#endif //USE_ENTITY_POINTER
							EEPROM_WRITE(eeprom, tmp_DataAdress, ptr, len);
#ifdef USE_ENTITY_POINTER
						}
#endif //USE_ENTITY_POINTER
						tmp_DataAdress += len;
					}
				}
			}
			tmp_DataAdress += 4;
			EEPROM_WRITE(eeprom, END_OF_DATA_POINTER_ADDRESS, (u8*)&tmp_DataAdress, SIZEOF_END_OF_DATA_POINTER);

			totalFieldCount = getTotalFieldsCount(EN_LOG_MSK);
			EEPROM_WRITE(eeprom, tmp_DataAdress, (u8*)&totalFieldCount, SIZEOF_TOTAL_FIELD_COUNT);
			tmp_DataAdress += SIZEOF_TOTAL_FIELD_COUNT;

			totalFieldCount = getTotalFieldsSize(EN_LOG_MSK) + SIZEOF_TIME_LOG;
			EEPROM_WRITE(eeprom, tmp_DataAdress, (u8*)&totalFieldCount, SIZEOF_ONE_LOG);
			tmp_DataAdress += SIZEOF_ONE_LOG;

			totalFieldCount = 0;
			EEPROM_WRITE(eeprom, tmp_DataAdress, (u8*)&totalFieldCount, SIZEOF_LOG_COUNTER);
			tmp_DataAdress += SIZEOF_LOG_COUNTER;
		}

		// compleate write data-----------------------------------------------------
		EEPROM_WRITE_BYTE(eeprom, START_ADDRESS, DATA_TO_START_ADDRESS);
	}
}


//*****************************************************************
// read / write log
//*****************************************************************

void writeLogEntitiesToFlash(EEPROM_OBJ_TYPE *eeprom, u8 SS, u8 MI, u8 HH, u8 DD, u8 MM, u8 YY)
{
	if(!eeprom) {
		return;
	}

	static u8 timeArr [SIZEOF_TIME_LOG] = {0, };
	static u8 entitiesLogState = 0;
	static u16 tmp_LogAddress = 0;

	static u32 tmp_Data = 0;
	static u32 oneLogSize = 0;

	static u16 entity_it = 0;
	static u8 field_it = 0;

	static u8 last_HH = 0;

	switch(entitiesLogState) {

	case 0:
		if(checkFlash(eeprom)) {
			entitiesLogState++;
		}
		break;

	case 1:
		// call function for write last log data
		last_HH = HH;
		entitiesLogState++;
		break;

	case 2:
		if(last_HH != HH) {
			last_HH = HH;
			entitiesLogState++;
		}
		break;

	case 3:
		tmp_LogAddress = getStartLogPointer(eeprom);

		EEPROM_READ(eeprom, tmp_LogAddress, (u8*)&tmp_Data, SIZEOF_TOTAL_FIELD_COUNT);
		tmp_LogAddress += SIZEOF_TOTAL_FIELD_COUNT;
		if(tmp_Data != getTotalFieldsCount(EN_LOG_MSK)) {
			entitiesLogState = 2;
		}

		EEPROM_READ(eeprom, tmp_LogAddress, (u8*)&oneLogSize, SIZEOF_ONE_LOG);
		tmp_LogAddress += SIZEOF_ONE_LOG;
		if(oneLogSize != (getTotalFieldsSize(EN_LOG_MSK) + SIZEOF_TIME_LOG)) {
			entitiesLogState = 2;
		}

		entitiesLogState++;
		break;

	case 4: {
		u32 logCnt = 0;
		EEPROM_READ(eeprom, tmp_LogAddress, (u8*)&logCnt, SIZEOF_LOG_COUNTER);
		tmp_Data = (logCnt + 1) & MAXIMUM_LOG_CELLS_MSK;
		EEPROM_WRITE(eeprom, tmp_LogAddress, (u8*)&tmp_Data, SIZEOF_LOG_COUNTER);
		tmp_LogAddress += SIZEOF_LOG_COUNTER + (logCnt * oneLogSize);
		entitiesLogState++;
		break;
	}

	case 5:
		timeArr[0] = DATA_TO_START_ADDRESS;
		timeArr[1] = SS;
		timeArr[2] = MI;
		timeArr[3] = HH;
		timeArr[4] = DD;
		timeArr[5] = MM;
		timeArr[6] = YY;
		EEPROM_WRITE(eeprom, tmp_LogAddress, timeArr, SIZEOF_TIME_LOG);
		tmp_LogAddress += SIZEOF_TIME_LOG;
		entitiesLogState++;
		break;

	case 6:

		if(entity_it < entities_count) {
			if(field_it < entities[entity_it]->fields_count) {

				if(entities[entity_it]->fields[field_it].bitFlags & EN_LOG_MSK) {
					u8 * ptr = (u8 *)(entities[entity_it]->pointer + entities[entity_it]->fields[field_it].shift);
					u8 len = getTypeLen(&entities[entity_it]->fields[field_it].type);

					if(entities[entity_it]->fields[field_it].bitFlags & EN_POINTER_MSK) {
						reg * data_reg = (reg *)ptr;
						if(*data_reg) EEPROM_WRITE(eeprom, tmp_LogAddress, (u8 *)(*data_reg), len);
					} else {
						EEPROM_WRITE(eeprom, tmp_LogAddress, ptr, len);
					}
					tmp_LogAddress += len;
				}
				field_it++;

			} else {
				entity_it++;
				field_it = 0;
			}
		} else {
			entity_it = 0;
			field_it = 0;
			entitiesLogState = 2;
		}
		break;
	}

	(void)timeArr;
}


void readLogEntitiesFromFlash(EEPROM_OBJ_TYPE *eeprom) {
	if(eeprom && checkFlash(eeprom)) {

		//---------------------------------------------------------------------

		u16 tmp_MapAddress = ENTITY_MAP_ADDRESS;
		u16 tmp_LogAddress = getStartLogPointer(eeprom);
		u32 oneLogSize = 0;
		u32 tmp_Data = 0;

		EEPROM_READ(eeprom, tmp_LogAddress, (u8*)&tmp_Data, SIZEOF_TOTAL_FIELD_COUNT);
		if(getTotalFieldsCount(EN_LOG_MSK) != tmp_Data) {
			return;
		}

		if(tmp_Data == 0) {
			return;
		}
		tmp_LogAddress += SIZEOF_TOTAL_FIELD_COUNT;

		EEPROM_READ(eeprom, tmp_LogAddress, (u8*)&oneLogSize, SIZEOF_ONE_LOG);

		if(oneLogSize != (getTotalFieldsSize(EN_LOG_MSK) + SIZEOF_TIME_LOG)) {
			return;
		}
		tmp_LogAddress += SIZEOF_ONE_LOG;

		EEPROM_READ(eeprom, tmp_LogAddress, (u8*)&tmp_Data, SIZEOF_LOG_COUNTER);
		tmp_Data = (tmp_Data - 1) & MAXIMUM_LOG_CELLS_MSK;
		tmp_LogAddress += SIZEOF_LOG_COUNTER + (tmp_Data * oneLogSize) + SIZEOF_TIME_LOG - 1;

		EEPROM_READ(eeprom, tmp_LogAddress, (u8*)&tmp_Data, 1);
		if((tmp_Data & (u32)0x000000ff) != DATA_TO_START_ADDRESS) {
			return;
		}
		tmp_LogAddress++;

		// read entity data

		for(u32 i = 0; i < entities_count; ++i) {
			EEPROM_READ(eeprom, tmp_MapAddress, (u8*)&tmp_Data, SIZEOF_ENTITIES_COUNT);
			if(i != (u16)tmp_Data) {
				return;
			}
			tmp_MapAddress += SIZEOF_ENTITIES_COUNT;

			EEPROM_READ(eeprom, tmp_MapAddress, (u8*)&tmp_Data, SIZEOF_ONE_FIELDS_COUNT);
			if(entities[i]->fields_count != (u8)tmp_Data) {
				return;
			}
			tmp_MapAddress += SIZEOF_ONE_FIELDS_COUNT;

			for(u32 j = 0; j < entities[i]->fields_count; ++j) {
				EEPROM_READ(eeprom, tmp_MapAddress, (u8*)&tmp_Data, SIZEOF_TYPE);
				if(entities[i]->fields[j].type != (u8)tmp_Data) {
					return;
				}
				tmp_MapAddress += SIZEOF_TYPE;

				EEPROM_READ(eeprom, tmp_MapAddress, (u8*)&tmp_Data, SIZEOF_BIT_FLAGS);
				if(entities[i]->fields[j].bitFlags != (u8)tmp_Data) {
					return;
				}
				tmp_MapAddress += SIZEOF_BIT_FLAGS;

				if(entities[i]->fields[j].bitFlags & EN_LOG_MSK) {
					u8 * ptr = (u8 *)(entities[i]->pointer + entities[i]->fields[j].shift);
					u8 len = getTypeLen(&entities[i]->fields[j].type);

					if(entities[i]->fields[j].bitFlags & EN_POINTER_MSK) {
						reg * data_reg = (reg *)ptr;
						if(*data_reg) EEPROM_READ(eeprom, tmp_LogAddress, (u8 *)(*data_reg), len);
					} else {
						EEPROM_READ(eeprom, tmp_LogAddress, ptr, len);
					}
					tmp_LogAddress += len;
				}
			}
		}

		//---------------------------------------------------------------------
	}
}
#endif

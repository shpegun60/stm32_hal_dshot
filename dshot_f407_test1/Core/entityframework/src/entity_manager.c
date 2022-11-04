/*
 * entity_manager.c
 *
 *  Created on: Dec 01, 2021
 *      Author: Shpegun60
 */

#include "entity_manager.h"
#include "stdio.h"
#include "stdlib.h"

u16 allocated_entity_pointers;
u16 entities_count;
Entity ** entities;


//------------------- WORKERS ----------------------------------
//basic
void read_entities_descriptions(u16 startEntityNum, u8 len, u8 *outputData, u16 *size) {
	//send {entities[0].descr, ... entities[entities_count].descr}
	u16 pos = 0;
	u8 cmd = DATA_CMD;

	if(entities && (startEntityNum < entities_count)) {
		to_pos_copy(1, &cmd, outputData, &pos, ENTITY_DIR_COPY);

		cmd = ENTITY_DECRIPTION_LEN;
		to_pos_copy(1, &cmd, outputData, &pos, ENTITY_DIR_COPY);

		cmd = MIN((entities_count - startEntityNum), len);
		to_pos_copy(1, (u8 *)&cmd, outputData, &pos, ENTITY_DIR_COPY);

		cmd = cmd + startEntityNum;

		for(u32 i = startEntityNum; i < cmd; ++i) {
			to_pos_copy(ENTITY_DECRIPTION_LEN, (u8 *)entities[i]->descr, outputData, &pos, ENTITY_DIR_COPY);
		}
	} else {
		cmd = NULL_CMD;
		to_pos_copy(1, &cmd, outputData, &pos, ENTITY_DIR_COPY); //send 0x00
	}

	*size = pos;
}


void read_entity_fields(u16 entityNum, u8 startFieldNum, u8 len, u8 *outputData, u16 *size) {
	//send {entities[entityNum].fields[startFieldNum], ... entities[num].fields[entities[num].fields_count]}

	u16 pos = 0;
	u8 cmd = DATA_CMD;

	if((entities_count > entityNum) && entities[entityNum]->fields_count > startFieldNum) {
		to_pos_copy(1, &cmd, outputData, &pos, ENTITY_DIR_COPY);
		to_pos_copy(1, &startFieldNum, outputData, &pos, ENTITY_DIR_COPY);

		cmd = MIN((entities[entityNum]->fields_count - startFieldNum), len);
		to_pos_copy(1, &cmd, outputData, &pos, ENTITY_DIR_COPY);
		cmd = cmd + startFieldNum;

		for(u32 i = startFieldNum; i < cmd; ++i) {
			to_pos_copy(sizeof(entities[entityNum]->fields[i].bitFlags), (u8 *)&entities[entityNum]->fields[i].bitFlags, outputData, &pos, ENTITY_DIR_COPY);
			to_pos_copy(sizeof(entities[entityNum]->fields[i].shift), (u8 *)&entities[entityNum]->fields[i].shift, outputData, &pos, ENTITY_DIR_COPY);
			to_pos_copy(sizeof(entities[entityNum]->fields[i].type), (u8 *)&entities[entityNum]->fields[i].type, outputData, &pos, ENTITY_DIR_COPY);
			to_pos_copy(ENTITY_DECRIPTION_LEN, (u8 *)&entities[entityNum]->fields[i].descr, outputData, &pos, ENTITY_DIR_COPY);
		}
	} else {
		cmd = NULL_CMD;
		to_pos_copy(1, &cmd, outputData, &pos, ENTITY_DIR_COPY); //send 0x00
	}

	*size = pos;
}


void read_field_value(u16 entityNum, u8 fieldNum, u8 *outputData, u16 *size) {
	//send {0x01 , prt + 0, ... , ptr + getTypeLen(entities[no].fields[fieldNo].type)}

	u16 pos = 0;
	u8 cmd = DATA_CMD;

	if((entities_count > entityNum) && (entities[entityNum]->fields_count > fieldNum)) {
		to_pos_copy(1, &cmd, outputData, &pos, ENTITY_DIR_COPY); //send 0x01
		to_pos_copy(1, &entities[entityNum]->fields[fieldNum].type, outputData, &pos, ENTITY_DIR_COPY);

		u8 * ptr = (u8 *) (entities[entityNum]->pointer + entities[entityNum]->fields[fieldNum].shift);

#ifdef USE_ENTITY_ATOMIC
		ATOMIC_BLOCK_RESTORATE_COND((entities[entityNum]->fields[fieldNum].bitFlags & EN_ATOMIC_MSK)) {
#endif //USE_ENTITY_ATOMIC

#ifdef USE_ENTITY_POINTER
			if(entities[entityNum]->fields[fieldNum].bitFlags & EN_POINTER_MSK) {
				reg * data_reg = (reg *)ptr;

#ifdef USE_ENTITY_REGISTER
				if(entities[entityNum]->fields[fieldNum].bitFlags & EN_REGISTER_MSK) {
					to_pos_copy_register(*data_reg ? (u8 *)(*data_reg) : ptr, outputData, &pos);
				} else {
#endif //USE_ENTITY_REGISTER
					to_pos_copy_t(entities[entityNum]->fields[fieldNum].type, *data_reg ? (u8 *)(*data_reg) : ptr, outputData, &pos, ENTITY_DIR_COPY);
#ifdef USE_ENTITY_REGISTER
				}
#endif//USE_ENTITY_REGISTER
			} else {
#endif //USE_ENTITY_POINTER

#ifdef USE_ENTITY_REGISTER
				if(entities[entityNum]->fields[fieldNum].bitFlags & EN_REGISTER_MSK) {
					to_pos_copy_register(ptr, outputData, &pos);
				} else {
#endif //USE_ENTITY_REGISTER
					to_pos_copy_t(entities[entityNum]->fields[fieldNum].type, ptr, outputData, &pos, ENTITY_DIR_COPY);
#ifdef USE_ENTITY_REGISTER
				}
#endif //USE_ENTITY_REGISTER

#ifdef USE_ENTITY_POINTER
			}
#endif//USE_ENTITY_POINTER

#ifdef USE_ENTITY_ATOMIC
		}
#endif //USE_ENTITY_ATOMIC

#ifdef USE_ENTITY_CALLBACKS
		if(entities[entityNum]->fields[fieldNum].entityCallback[EN_READ_CALLBACK] != NULL) {
			entities[entityNum]->fields[fieldNum].entityCallback[EN_READ_CALLBACK]((void*)entities[entityNum], (void*)&entities[entityNum]->fields[fieldNum], entities[entityNum]->fields[fieldNum].args[EN_READ_CALLBACK]);
		}
#endif //USE_ENTITY_CALLBACKS

	} else {
		cmd = NULL_CMD;
		to_pos_copy(1, &cmd, outputData, &pos, ENTITY_DIR_COPY); //send 0x00
	}


	*size = pos;
}


void set_field_value(u16 entityNum, u8 fieldNum, u8* data) {
	if((entities_count > entityNum) && (entities[entityNum]->fields_count > fieldNum)) {
		if((entities[entityNum]->fields[fieldNum].bitFlags & EN_READ_ONLY_MSK) == 0) {
			u16 pos = 0;
			u8 * ptr = (u8 *)(entities[entityNum]->pointer + entities[entityNum]->fields[fieldNum].shift);

#ifdef USE_ENTITY_ATOMIC
		ATOMIC_BLOCK_RESTORATE_COND((entities[entityNum]->fields[fieldNum].bitFlags & EN_ATOMIC_MSK)) {
#endif //USE_ENTITY_ATOMIC

#ifdef USE_ENTITY_POINTER
				if(entities[entityNum]->fields[fieldNum].bitFlags & EN_POINTER_MSK) {
					reg * data_reg = (reg *)ptr;
#ifdef USE_ENTITY_REGISTER
					if(entities[entityNum]->fields[fieldNum].bitFlags & EN_REGISTER_MSK) {
						if(*data_reg) to_pos_copy_register(data, (u8 *)(*data_reg), &pos);
					} else {
#endif //USE_ENTITY_REGISTER
						if(*data_reg) to_pos_copy_t(entities[entityNum]->fields[fieldNum].type, data, (u8 *)(*data_reg), &pos, ENTITY_DIR_COPY);
#ifdef USE_ENTITY_REGISTER
					}
#endif //USE_ENTITY_REGISTER

				} else {
#endif //USE_ENTITY_POINTER

#ifdef USE_ENTITY_REGISTER
					if(entities[entityNum]->fields[fieldNum].bitFlags & EN_REGISTER_MSK) {
						to_pos_copy_register(data, ptr, &pos);
					} else {
#endif //USE_ENTITY_REGISTER
						to_pos_copy_t(entities[entityNum]->fields[fieldNum].type, data, ptr, &pos, ENTITY_DIR_COPY);
#ifdef USE_ENTITY_REGISTER
					}
#endif //USE_ENTITY_REGISTER
#ifdef USE_ENTITY_POINTER
				}
#endif //USE_ENTITY_POINTER
#ifdef USE_ENTITY_ATOMIC
		}
#endif //USE_ENTITY_ATOMIC

#ifdef USE_ENTITY_CALLBACKS
			if(entities[entityNum]->fields[fieldNum].entityCallback[EN_WRITE_CALLBACK] != NULL) {
				entities[entityNum]->fields[fieldNum].entityCallback[EN_WRITE_CALLBACK]((void*)entities[entityNum], (void*)&entities[entityNum]->fields[fieldNum], entities[entityNum]->fields[fieldNum].args[EN_WRITE_CALLBACK]);
			}
#endif //USE_ENTITY_CALLBACKS
		}
	}
}

//extended--------------------------------------------------------------------------------------------------------------------------------------
void read_several_fields_values(u8 *inputData, u8 *outputData, u16 *size) {
	u16 Wpos = 0;
	u16 Rpos = 0;
	u8  cmd  = DATA_CMD;
	to_pos_copy(1, &cmd, outputData, &Wpos, 0); //send 0x01

	u8 * ptr = NULL;
	u16 entityNum = 0;
	u8 fieldNum = 0;


	while((Rpos + 3) < (*size)) {
		from_pos_copy(2, inputData, (u8 *)&entityNum, &Rpos, ENTITY_DIR_COPY);
		from_pos_copy(1, inputData, (u8 *)&fieldNum,  &Rpos, ENTITY_DIR_COPY);

		if((entities_count > entityNum) && (entities[entityNum]->fields_count > fieldNum)) {
			ptr = (u8 *)(entities[entityNum]->pointer + entities[entityNum]->fields[fieldNum].shift);

#ifdef USE_ENTITY_ATOMIC
		ATOMIC_BLOCK_RESTORATE_COND((entities[entityNum]->fields[fieldNum].bitFlags & EN_ATOMIC_MSK)) {
#endif //USE_ENTITY_ATOMIC

#ifdef USE_ENTITY_POINTER
			if(entities[entityNum]->fields[fieldNum].bitFlags & EN_POINTER_MSK) {
				reg * data_reg = (reg *)ptr;
#ifdef USE_ENTITY_REGISTER
				if(entities[entityNum]->fields[fieldNum].bitFlags & EN_REGISTER_MSK) {
					to_pos_copy_register(*data_reg ? (u8 *)(*data_reg) : ptr, outputData, &Wpos);
				} else {
#endif //USE_ENTITY_REGISTER
					to_pos_copy_t(entities[entityNum]->fields[fieldNum].type, *data_reg ? (u8 *)(*data_reg) : ptr, outputData, &Wpos, ENTITY_DIR_COPY);
#ifdef USE_ENTITY_REGISTER
				}
#endif //USE_ENTITY_REGISTER

			} else {
#endif //USE_ENTITY_POINTER
#ifdef USE_ENTITY_REGISTER
				if(entities[entityNum]->fields[fieldNum].bitFlags & EN_REGISTER_MSK) {
					to_pos_copy_register(ptr, outputData, &Wpos);
				} else {
#endif //USE_ENTITY_REGISTER
					to_pos_copy_t(entities[entityNum]->fields[fieldNum].type, ptr, outputData, &Wpos, ENTITY_DIR_COPY);
#ifdef USE_ENTITY_REGISTER
				}
#endif //USE_ENTITY_REGISTER

#ifdef USE_ENTITY_POINTER
			}
#endif //USE_ENTITY_POINTER

#ifdef USE_ENTITY_ATOMIC
		}
#endif //USE_ENTITY_ATOMIC

#ifdef USE_ENTITY_CALLBACKS
			if(entities[entityNum]->fields[fieldNum].entityCallback[EN_READ_CALLBACK] != NULL) {
				entities[entityNum]->fields[fieldNum].entityCallback[EN_READ_CALLBACK]((void*)entities[entityNum], (void*)&entities[entityNum]->fields[fieldNum], entities[entityNum]->fields[fieldNum].args[EN_READ_CALLBACK]);
			}
#endif //USE_ENTITY_CALLBACKS
		} else {
			Wpos = 0;
			cmd = NULL_CMD;
			to_pos_copy(1, &cmd, outputData, &Wpos, ENTITY_DIR_COPY); //send 0x00
			to_pos_copy(2, (u8 *)&entityNum, outputData, &Wpos, ENTITY_DIR_COPY);
			to_pos_copy(1, (u8 *)&fieldNum, outputData, &Wpos, ENTITY_DIR_COPY);

			*size = Wpos;
			return;
		}
	}
	*size = Wpos;
}


void set_several_fields_values(u8 *inputData, u8 *outputData, u16 *size) {
	u16 Wpos = 0;
	u16 Rpos = 0;
	u8  cmd  = DATA_CMD;
	to_pos_copy(1, &cmd, outputData, &Wpos, ENTITY_DIR_COPY); //send 0x01

	u8 * ptr = NULL;
	u16 entityNum = 0;
	u8 fieldNum = 0;


	while((Rpos + 3) < (*size)) {
		from_pos_copy(2, inputData, (u8 *)&entityNum, &Rpos, ENTITY_DIR_COPY);
		from_pos_copy(1, inputData, (u8 *)&fieldNum,  &Rpos, ENTITY_DIR_COPY);

		if((entities_count > entityNum) && (entities[entityNum]->fields_count > fieldNum)) {
			ptr = (u8 *)(entities[entityNum]->pointer + entities[entityNum]->fields[fieldNum].shift);

			if((entities[entityNum]->fields[fieldNum].bitFlags & EN_READ_ONLY_MSK) == 0) {

#ifdef USE_ENTITY_ATOMIC
				ATOMIC_BLOCK_RESTORATE_COND((entities[entityNum]->fields[fieldNum].bitFlags & EN_ATOMIC_MSK)) {
#endif //USE_ENTITY_ATOMIC

#ifdef USE_ENTITY_POINTER
					if(entities[entityNum]->fields[fieldNum].bitFlags & EN_POINTER_MSK) {
						reg * data_reg = (reg *)ptr;

#ifdef USE_ENTITY_REGISTER
						if(entities[entityNum]->fields[fieldNum].bitFlags & EN_REGISTER_MSK) {
							if(*data_reg) from_pos_copy_register(inputData, (u8 *)(*data_reg), &Rpos);
						} else {
#endif //USE_ENTITY_REGISTER
							if(*data_reg) from_pos_copy_t(entities[entityNum]->fields[fieldNum].type, inputData, (u8 *)(*data_reg), &Rpos, ENTITY_DIR_COPY);
#ifdef USE_ENTITY_REGISTER
						}
#endif //USE_ENTITY_REGISTER

					} else {
#endif //USE_ENTITY_POINTER

#ifdef USE_ENTITY_REGISTER
						if(entities[entityNum]->fields[fieldNum].bitFlags & EN_REGISTER_MSK) {
							from_pos_copy_register(inputData, ptr, &Rpos);
						} else {
#endif //USE_ENTITY_REGISTER
							from_pos_copy_t(entities[entityNum]->fields[fieldNum].type, inputData, ptr, &Rpos, ENTITY_DIR_COPY);
#ifdef USE_ENTITY_REGISTER
						}
#endif //USE_ENTITY_REGISTER

#ifdef USE_ENTITY_POINTER
					}
#endif //USE_ENTITY_POINTER

#ifdef USE_ENTITY_ATOMIC
				}
#endif // USE_ENTITY_ATOMIC

#ifdef USE_ENTITY_CALLBACKS
				if(entities[entityNum]->fields[fieldNum].entityCallback[EN_WRITE_CALLBACK] != NULL) {
					entities[entityNum]->fields[fieldNum].entityCallback[EN_WRITE_CALLBACK]((void*)entities[entityNum], (void*)&entities[entityNum]->fields[fieldNum], entities[entityNum]->fields[fieldNum].args[EN_WRITE_CALLBACK]);
				}
#endif //USE_ENTITY_CALLBACKS
			}
		} else {
			Wpos = 0;
			cmd = NULL_CMD;
			to_pos_copy(1, &cmd, outputData, &Wpos, ENTITY_DIR_COPY); //send 0x00
			to_pos_copy(2, (u8 *)&entityNum, outputData, &Wpos, ENTITY_DIR_COPY);
			to_pos_copy(1, (u8 *)&fieldNum, outputData, &Wpos, ENTITY_DIR_COPY);

			*size = Wpos;
			return;
		}
	}
	*size = Wpos;
}

#ifdef USE_ENTITY_FLASH

void set_several_bit_flags(u8 *inputData, u8 *outputData, u16 *size, EEPROM_OBJ_TYPE *eeprom) {

#define SET_ALL_WRITE_FLASH 	0x00
#define RESET_ALL_WRITE_FLASH 	0x01
#define SET_ALL_WRITE_LOG   	0x02
#define RESET_ALL_WRITE_LOG   	0x03

#define PROCEED_WRITE_FLASH 	0x04
#define PROCEED_READ_FLASH 		0x05
#define PROCEED_READ_LOG	   	0x06

	u16 Wpos = 0;
	u16 Rpos = 0;
	u8  cmd  = DATA_CMD;
	to_pos_copy(1, &cmd, outputData, &Wpos, ENTITY_DIR_COPY); //send 0x01


	from_pos_copy(1, inputData, (u8 *)&cmd, &Rpos, ENTITY_DIR_COPY);

	switch(cmd) {

	case SET_ALL_WRITE_FLASH:
		for(u32 i = 0; i < entities_count; ++i) {
			for(u32 j = 0; j < entities[i]->fields_count; ++j) {
				entities[i]->fields[j].bitFlags |= EN_PARAM_MSK;
			}
		}
		break;

	case RESET_ALL_WRITE_FLASH:
		for(u32 i = 0; i < entities_count; ++i) {
			for(u32 j = 0; j < entities[i]->fields_count; ++j) {
				entities[i]->fields[j].bitFlags &= ~EN_PARAM_MSK;
			}
		}
		break;

	case SET_ALL_WRITE_LOG:
		for(u32 i = 0; i < entities_count; ++i) {
			for(u32 j = 0; j < entities[i]->fields_count; ++j) {
				entities[i]->fields[j].bitFlags |= EN_LOG_MSK;
			}
		}
		break;

	case RESET_ALL_WRITE_LOG:
		for(u32 i = 0; i < entities_count; ++i) {
			for(u32 j = 0; j < entities[i]->fields_count; ++j) {
				entities[i]->fields[j].bitFlags &= ~EN_LOG_MSK;
			}
		}
		break;

	case PROCEED_WRITE_FLASH:
		writeEntitiesToFlash(eeprom);
		break;

	case PROCEED_READ_FLASH:
		readEntitiesFromFlash(eeprom);
		break;

	case PROCEED_READ_LOG:
		//readLogEntitiesFromFlash(eeprom);
		break;

	default: {
		u16 entityNum = 0;
		u16 fieldNum = 0;

		while((Rpos + 3) < (*size)) {
			from_pos_copy(2, inputData, (u8 *)&entityNum, &Rpos, ENTITY_DIR_COPY);
			from_pos_copy(1, inputData, (u8 *)&fieldNum,  &Rpos, ENTITY_DIR_COPY);

			if((entities_count > entityNum) && (entities[entityNum]->fields_count > fieldNum)) {
				u8 bitValue = 0;
				from_pos_copy(1, inputData, &bitValue, &Rpos, ENTITY_DIR_COPY);
				// param setting
				if(bitValue & ((u8)0x01)) entities[entityNum]->fields[fieldNum].bitFlags |= EN_PARAM_MSK;
				else entities[entityNum]->fields[fieldNum].bitFlags &= ~EN_PARAM_MSK;

				// log setting
				if(bitValue & ((u8)0x02)) entities[entityNum]->fields[fieldNum].bitFlags |= EN_LOG_MSK;
				else entities[entityNum]->fields[fieldNum].bitFlags &= ~EN_LOG_MSK;
			} else {
				Wpos = 0;
				cmd = NULL_CMD;
				to_pos_copy(1, &cmd, outputData, &Wpos, ENTITY_DIR_COPY); //send 0x00
				to_pos_copy(2, (u8 *)&entityNum, outputData, &Wpos, ENTITY_DIR_COPY);
				to_pos_copy(1, (u8 *)&fieldNum, outputData, &Wpos, ENTITY_DIR_COPY);

				*size = Wpos;
				return;
			}
		}
		break;
	}
	}

	*size = Wpos;

#undef SET_ALL_WRITE_FLASH
#undef RESET_ALL_WRITE_FLASH
#undef SET_ALL_WRITE_LOG
#undef RESET_ALL_WRITE_LOG

#undef PROCEED_WRITE_FLASH
#undef PROCEED_READ_FLASH
#undef PROCEED_READ_LOG
}

#endif // USE_ENTITY_FLASH

//***********************************************************************************************************

/*
 * ****************************************************************************************************
 * help functions
 * ***************************************************************************************************
 */

void clearEntities() {
	for(u32 i = 0; i < entities_count; ++i) {
		free(entities[i]->pointer);
		free(entities[i]->fields);
		free(entities[i]);
	}
	free(entities);
	entities_count = 0;
	allocated_entity_pointers = 0;
	entities = NULL;
}

void clearEntitityFields(u16 EntityNumber) {
	free(entities[EntityNumber]->pointer);
	free(entities[EntityNumber]->fields);
	free(entities[EntityNumber]);
}


/*
 * ****************************************************************************************************
 * entity functions
 * ***************************************************************************************************
 */

// malloc new entities pointers
u16 newEntities(u16 nomberOfEntities) {
	clearEntities(); // clear all entities
#ifdef USE_ENTITY_ERRORS_HANDLING
	deleteAllErrors();
#endif // USE_ENTITY_ERRORS_HANDLING

	if(nomberOfEntities == ENTITY_ERROR) {
		return ENTITY_ERROR;
	}

	entities = (Entity**) calloc(nomberOfEntities, sizeof(Entity*)); // allocation & initialization all entity
	if(entities == NULL) {
		return ENTITY_ERROR;
	}

	allocated_entity_pointers = nomberOfEntities;
	return ENTITY_OK;
}

u16 initEntity(u8 NumberOfFields, u16 pointerSize, char * descr, b isUserSpace, void * arg) {

	if(entities_count == allocated_entity_pointers) {
		return ENTITY_ERROR;
	}

	*(entities + entities_count) = (Entity*) malloc(sizeof(Entity));
	if(*(entities + entities_count) == NULL) {
		clearEntitityFields(entities_count);
		return ENTITY_ERROR;
	}

	u16 pos = 0;
	to_pos_copy(ENTITY_DECRIPTION_LEN, (u8 *)descr, (u8 *)entities[entities_count]->descr, &pos, 0);

	entities[entities_count]->pointer = isUserSpace ? arg : calloc(pointerSize, sizeof(u8));
	if(entities[entities_count]->pointer == NULL) {
		clearEntitityFields(entities_count);
		return ENTITY_ERROR;
	}

	entities[entities_count]->fields = (EntityField*) calloc(NumberOfFields, sizeof(EntityField));
	if(entities[entities_count]->fields == NULL) {
		clearEntitityFields(entities_count);
		return ENTITY_ERROR;
	}
	entities[entities_count]->fields_count = NumberOfFields;

	char str[(ENTITY_DECRIPTION_LEN << 1) + 1] = {'0'};
	for(u8 i = 0; i < NumberOfFields; ++i) {

#ifdef USE_ENTITY_CALLBACKS
		entities[entities_count]->fields[i].entityCallback[EN_WRITE_CALLBACK] = NULL;
		entities[entities_count]->fields[i].entityCallback[EN_READ_CALLBACK] = NULL;
		entities[entities_count]->fields[i].args[EN_WRITE_CALLBACK] = NULL;
		entities[entities_count]->fields[i].args[EN_READ_CALLBACK] = NULL;
#endif // USE_ENTITY_CALLBACKS
		pos = 0;
		sprintf(str, "%d", i);
		entities[entities_count]->fields[i].bitFlags = 0;
		entities[entities_count]->fields[i].shift = 0;
		entities[entities_count]->fields[i].type = VOID_TYPE;
		to_pos_copy(ENTITY_DECRIPTION_LEN, (u8 *)str, (u8 *)entities[entities_count]->fields[i].descr, &pos, ENTITY_DIR_COPY);
	}

	entities_count++;
	return (entities_count - 1);
}

/*
 * ****************************************************************************************************
 * field functions
 * ***************************************************************************************************
 */

u8 initFieldShiftPtr(Entity * entityInst, u8 * filedNo, u8 bitFlags, u16 * shift, u8 type, char * descr, void * field_ptr) {
	if(entityInst && (entityInst->fields_count > (*filedNo))) {
		u16 pos = 0;

		entityInst->fields[*filedNo].bitFlags = bitFlags;
		entityInst->fields[*filedNo].shift = (*shift);
		entityInst->fields[*filedNo].type = type;

		to_pos_copy(ENTITY_DECRIPTION_LEN, (u8 *)descr, (u8 *)entityInst->fields[*filedNo].descr, &pos, ENTITY_DIR_COPY);

		if(bitFlags & EN_POINTER_MSK) {
			(*shift) += sizeof(reg);
		} else {
			(*shift) += getTypeLen_t(type);
		}

		(*filedNo)++;

		entityPointerInit_t(type, (u8 *)field_ptr);

		return (u8)ENTITY_OK;
	}
	return (u8)ENTITY_ERROR;
}

u8 initField(Entity * entityInst, u8 * filedNo, u8 bitFlags, u16 shift, u8 type, char * descr, void * field_ptr) {
	if(entityInst && (entityInst->fields_count > (*filedNo))) {
		u16 pos = 0;

		entityInst->fields[*filedNo].bitFlags = bitFlags;
		entityInst->fields[*filedNo].shift = shift;
		entityInst->fields[*filedNo].type = type;

		to_pos_copy(ENTITY_DECRIPTION_LEN, (u8 *)descr, (u8 *)entityInst->fields[*filedNo].descr, &pos, ENTITY_DIR_COPY);
		(*filedNo)++;

		entityPointerInit_t(type, (u8 *)field_ptr);

		return (u8)ENTITY_OK;
	}
	return (u8)ENTITY_ERROR;
}

u8 initFieldArray(Entity * entityInst, u8 * filedNo, u8 bitFlags, u16 shift, u8 type, u8 arrayLen, char * descr, void * field_ptr, int startNum) {
	if(entityInst && (entityInst->fields_count > (*filedNo)) && field_ptr) {

		u16 pos = 0;
		char str[(ENTITY_DECRIPTION_LEN << 1) + 1] = {'0'};
		for(u8 i = 0; i < arrayLen; ++i) {


			entityInst->fields[*filedNo].bitFlags = bitFlags;
			entityInst->fields[*filedNo].shift = shift;
			entityInst->fields[*filedNo].type = type;

			sprintf(str, "%s%d", descr, (int)(i + startNum));

			to_pos_copy(ENTITY_DECRIPTION_LEN, (u8 *)str, (u8 *)entityInst->fields[*filedNo].descr, &pos, ENTITY_DIR_COPY);
			(*filedNo)++;
			pos = 0;
			shift += getTypeLen_t(type);
		}

		entityPointerInit_tArray(type, arrayLen, field_ptr);

		return (u8)ENTITY_OK;
	}
	return (u8)ENTITY_ERROR;
}


u8 initFieldPtr(EntityField * fieldInst, u8 bitFlags, u16 * shift, u8 type, char * descr) {
	if(fieldInst) {
		u16 pos = 0;

		fieldInst->bitFlags = bitFlags;
		fieldInst->shift = (*shift);
		fieldInst->type = type;
		to_pos_copy(ENTITY_DECRIPTION_LEN, (u8 *)descr, (u8 *)fieldInst->descr, &pos, ENTITY_DIR_COPY);

		if(bitFlags & EN_POINTER_MSK) {
			(*shift) += sizeof(reg);
		} else {
			(*shift) += getTypeLen_t(type);
		}
		return (u8)ENTITY_OK;
	}
	return (u8)ENTITY_ERROR;
}

u8 fieldRename(Entity * entityInst, u8 fieldNumber, char * descr) {
	if(entityInst && (entityInst->fields_count > fieldNumber)) {
		u16 pos = 0;
		to_pos_copy(ENTITY_DECRIPTION_LEN, (u8 *)descr, (u8 *)entityInst->fields[fieldNumber].descr, &pos, ENTITY_DIR_COPY);
		return (u8)ENTITY_OK;
	}
	return (u8)ENTITY_ERROR;
}


#ifdef USE_ENTITY_CALLBACKS
/*
 * ****************************************************************************************************
 * callBack functions
 * ***************************************************************************************************
 */
u8 initCallback(Entity * entityInst, u8 filedNo, void (*readCallback)(void* Entity_ptr, void* Field_ptr, void* args), void* rArgs, void (*writeCallback)(void* Entity_ptr, void* Field_ptr, void* args), void* wArgs) {
	if(entityInst && (entityInst->fields_count > filedNo)) {

		entityInst->fields[filedNo].entityCallback[EN_READ_CALLBACK] = readCallback;
		entityInst->fields[filedNo].args[EN_READ_CALLBACK] = rArgs;

		entityInst->fields[filedNo].entityCallback[EN_WRITE_CALLBACK] = writeCallback;
		entityInst->fields[filedNo].args[EN_WRITE_CALLBACK] = wArgs;

		return (u8)ENTITY_OK;
	}
	return (u8)ENTITY_ERROR;
}

u8 initCallback_txt(Entity * entityInst, char* desc, void (*readCallback)(void* Entity_ptr, void* Field_ptr, void* args), void* rArgs, void (*writeCallback)(void* Entity_ptr, void* Field_ptr, void* args), void* wArgs) {
	if(entityInst) {

		for(u8 it = 0; it < entityInst->fields_count; ++it) {
			if(entityStrnCmp(desc, entityInst->fields[it].descr, ENTITY_DECRIPTION_LEN) == 0) {

				entityInst->fields[it].entityCallback[EN_READ_CALLBACK] = readCallback;
				entityInst->fields[it].args[EN_READ_CALLBACK] = rArgs;

				entityInst->fields[it].entityCallback[EN_WRITE_CALLBACK] = writeCallback;
				entityInst->fields[it].args[EN_WRITE_CALLBACK] = wArgs;

				return (u8)ENTITY_OK;
			}
		}
		return (u8)ENTITY_ERROR;
	}
	return (u8)ENTITY_ERROR;
}

#endif // USE_ENTITY_CALLBACKS

/*
 * entity_parser.c
 *
 *  Created on: Dec 01, 2021
 *      Author: Shpegun60
 */

#include "entity_parser.h"


float getFloatFieldPointer(uint16_t entity_num, uint8_t field_num) {
	if(entities_count > entity_num && entities[entity_num]->fields_count > field_num) {

		float value = (float)0;
		void * ptr = (void *)(entities[entity_num]->pointer + entities[entity_num]->fields[field_num].shift);

		switch(entities[entity_num]->fields[field_num].type) {

		case UINT8_TYPE:
			value = (float) (*((uint8_t *)ptr));
			break;

		case UINT16_TYPE:
			value = (float) (*((uint16_t *)ptr));
			break;

		case UINT32_TYPE:
			value = (float) (*((uint32_t *)ptr));
			break;

		case UINT64_TYPE:
			value = (float) (*((uint64_t *)ptr));
			break;

		case CHAR_TYPE:
			value = (float) (*((char *)ptr));
			break;

		case INT8_TYPE:
			value = (float) (*((int8_t *)ptr));
			break;

		case INT16_TYPE:
			value = (float) (*((int16_t *)ptr));
			break;

		case INT32_TYPE:
			value = (float) (*((int32_t *)ptr));
			break;

		case INT64_TYPE:
			value = (float) (*((int64_t *)ptr));
			break;

		case FLOAT_TYPE:
			value = (float) (*((float *)ptr));
			break;

		case DOUBLE_TYPE:
			value = (float) (*((double *)ptr));
			break;

		case LONG_DOUBLE_TYPE:
			value = (float) (*((long double *)ptr));
			break;

		case BOOL_TYPE:
			value = (float) (*((uint8_t *)ptr));
			break;

		default:

			break;
		}

		return value;
	}
	return (float)0;
}

void writeFloatFieldPointer(uint16_t entity_num, uint8_t field_num, float value) {
	if(entities_count > entity_num && entities[entity_num]->fields_count > field_num) {

		void * ptr = (void *)(entities[entity_num]->pointer + entities[entity_num]->fields[field_num].shift);

		if(entities[entity_num]->fields[field_num].bitFlags & EN_READ_ONLY_MSK) {
			return;
		}

		switch(entities[entity_num]->fields[field_num].type) {

		case UINT8_TYPE: {
			uint8_t * data_ptr = (uint8_t *)ptr;
			*data_ptr = (uint8_t)value;
			break;
		}

		case UINT16_TYPE: {
			uint16_t * data_ptr = (uint16_t *)ptr;
			*data_ptr = (uint16_t)value;
			break;
		}

		case UINT32_TYPE: {
			uint32_t * data_ptr = (uint32_t *)ptr;
			*data_ptr = (uint32_t)value;
			break;
		}

		case UINT64_TYPE: {
			uint64_t * data_ptr = (uint64_t *)ptr;
			*data_ptr = (uint64_t)value;
			break;
		}

		case CHAR_TYPE: {
			char * data_ptr = (char *)ptr;
			*data_ptr = (char)value;
			break;
		}

		case INT8_TYPE: {
			int8_t * data_ptr = (int8_t *)ptr;
			*data_ptr = (int8_t)value;
			break;
		}

		case INT16_TYPE: {
			int16_t * data_ptr = (int16_t *)ptr;
			*data_ptr = (int16_t)value;
			break;
		}

		case INT32_TYPE: {
			int32_t * data_ptr = (int32_t *)ptr;
			*data_ptr = (int32_t)value;
			break;
		}

		case INT64_TYPE: {
			int64_t * data_ptr = (int64_t *)ptr;
			*data_ptr = (int64_t)value;
			break;
		}

		case FLOAT_TYPE: {
			float * data_ptr = (float *)ptr;
			*data_ptr = (float)value;
			break;
		}

		case DOUBLE_TYPE: {
			double * data_ptr = (double *)ptr;
			*data_ptr = (double)value;
			break;
		}

		case LONG_DOUBLE_TYPE: {
			long double * data_ptr = (long double *)ptr;
			*data_ptr = (long double)value;
			break;
		}

		case BOOL_TYPE: {
			uint8_t * data_ptr = (uint8_t *)ptr;
			*data_ptr = (uint8_t)value;
			break;
		}

		default:

			break;
		}
	}
}

/*
 * entity_parser.h
 *
 *  Created on: Dec 01, 2021
 *      Author: Shpegun60
 */

#ifndef INC_ENTITY_PARSER_H_
#define INC_ENTITY_PARSER_H_

#include "entity_types.h"
#include "entity_manager.h"

// getter field data pointer
float getFloatFieldPointer(uint16_t entity_num, uint8_t field_num);
//setter field data pointer
void writeFloatFieldPointer(uint16_t entity_num, uint8_t field_num, float value);

#endif /* INC_ENTITY_PARSER_H_ */

#ifndef INC_WORKERS_H_
#define INC_WORKERS_H_

#include "rawparser.h"
#include "director.h"

void Parking(uint8_t *inputData, uint8_t *outputData, uint8_t *size, uint32_t time);
void SetMotorValues(uint8_t *inputData, uint8_t *outputData, uint8_t *size, uint32_t time);
void GetTelemetry(uint8_t *inputData, uint8_t *outputData, uint8_t *size, uint32_t time);

#endif /* INC_WORKERS_H_ */

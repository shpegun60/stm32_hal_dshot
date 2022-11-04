#ifndef INC_DIRECTOR_H_
#define INC_DIRECTOR_H_

#include "stdint.h"
#include "stddef.h"

#define MAX_COMMAND_FUNCTIONS 256UL

typedef struct {
	void (*worker[MAX_COMMAND_FUNCTIONS])(uint8_t *inputData, uint8_t *outputData, uint8_t *size, uint32_t time);
} Director_t;

void Director_Clear(Director_t *inst);
void Director_AddWorker(Director_t *inst, uint8_t id, void (workerHandler)(uint8_t *inputData, uint8_t *outputData, uint8_t *size, uint32_t time));
void Director_ProceedWork(Director_t *inst, uint8_t *inputData, uint8_t *outputData, uint8_t *size, uint32_t time);
uint8_t Director_RefreshWorkers(Director_t *inst, uint8_t *outputData, uint8_t *size, uint32_t time);

#endif /* INC_DIRECTOR_H_ */

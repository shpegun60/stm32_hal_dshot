#include "director.h"

void Director_Clear(Director_t *inst) {
	if(!inst) {
		return;
	}

	for (uint16_t i = 0; i < MAX_COMMAND_FUNCTIONS; ++i) {
		inst->worker[i] = NULL;
	}
}

void Director_AddWorker(Director_t *inst, uint8_t id, void (workerHandler)(uint8_t *inputData, uint8_t *outputData, uint8_t *size, uint32_t time)){
	if(!inst) {
		return;
	}

	if(id < MAX_COMMAND_FUNCTIONS) {
		inst->worker[id] = workerHandler;
	}
}

void Director_ProceedWork(Director_t *inst, uint8_t *inputData, uint8_t *outputData, uint8_t *size, uint32_t time) {
	if (inst && inputData && outputData && size && (*size != 0)) {

		uint8_t messageId = inputData[0];
		(*size)--;

		if (inst->worker[messageId] && (messageId < MAX_COMMAND_FUNCTIONS)) {
			inst->worker[messageId]((inputData + 1), outputData, size, time);
		} else {
			*size = 0;
		}
	}
}

uint8_t Director_RefreshWorkers(Director_t *inst, uint8_t *outputData, uint8_t *size, uint32_t time) {
	static uint8_t id = 0;
	uint8_t state = 0;

	if (!inst || !outputData || !size) {
		return 0;
	}

	if (inst->worker[id] && id != MAX_COMMAND_FUNCTIONS) {
		inst->worker[id](NULL, outputData, size, time);
		state = 1;
		++id;
	} else {
		id = 0;
	}

	return state;
}

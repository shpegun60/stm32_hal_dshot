#ifndef COMM_OBJ_H_
#define COMM_OBJ_H_

#include "main.h"
#include "rawparser.h"
#include "director.h"

#define START_BYTE 		((uint8_t)0x1A)
#define FRAME_SIZE 		((uint8_t)0xFF)
#define BUFF_SIZE 		((uint8_t)0xFF)

#define NO_PING_TIME_MS		2000

typedef struct {
	Director_t director;
	RawParser_t rawparser;
	RawParser_Frame *frameTX;
	RawParser_Frame *frameRX;
	volatile uint8_t receiveByte;
	uint8_t outputData[FRAME_SIZE];

	uint32_t last_receive_time;
} Communicator_t;

extern Communicator_t comm;

void initCommunication(void);
void InitRawParser(void);
void InitDirector(void);

void proceedIncommingMessage(uint32_t currentSystemTime);

#endif /* COMM_OBJ_H_ */

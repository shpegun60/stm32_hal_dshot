#include "comm_obj.h"
#include "usart.h"

#include <stdio.h>
#include <string.h>



#include "workers.h"

//***********************************************************************************
// communication ------------------------------
//***********************************************************************************

Communicator_t comm;

void initCommunication(void)
{
	InitRawParser();
	InitDirector();
}

void InitRawParser(void)
{
	memset(&comm, 0, sizeof(Communicator_t));

	RawParser_Init_t init;
	init.startByte = START_BYTE;
	init.maxFrameSize = FRAME_SIZE;
	init.inputBytesBuffSize = BUFF_SIZE;
	RawParser_Init(&comm.rawparser, &init);

	HAL_UART_Receive_IT(&huart2, (uint8_t*) &comm.receiveByte, 1);
}

void InitDirector(void)
{
	Director_Clear(&comm.director);

	Director_AddWorker(&comm.director, 0, Parking);
	Director_AddWorker(&comm.director, 1, SetMotorValues);
	Director_AddWorker(&comm.director, 2, GetTelemetry);
}


void proceedIncommingMessage(uint32_t currentSystemTime)
{
	comm.frameRX = RawParser_ReadUnshieldedFrame(&comm.rawparser);
	//HAL_UART_Receive_IT(&huart1, (uint8_t*) &receiveByte, 1);

	if(comm.frameRX->size == 0) {
		return;
	}

	Director_ProceedWork(&comm.director, (uint8_t *)(comm.frameRX->data), comm.outputData, &comm.frameRX->size, currentSystemTime);
	if(comm.frameRX->size == 0) {
		return;
	}

	comm.frameTX = RawParser_ShieldFrame(&comm.rawparser, comm.outputData, comm.frameRX->size);
	HAL_UART_Transmit(&huart2, comm.frameTX->data, comm.frameTX->size, 10000);
}
//***********************************************************************************

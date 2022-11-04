#include "workers.h"
#include "entity_manager.h"
#include "comm_obj.h"

#include <string.h>
#include "dshot.h"
#include "dshot_telemetry.h"

void RemoveSetParking();

extern uint16_t my_motor_value[4];

// ------------ Parking ---------------------------------------------------------------------------
void Parking(uint8_t *inputData, uint8_t *outputData, uint8_t *size, uint32_t time)
{
	RemoveSetParking();

	char str[] = "Parking cmd Ok ";
	memcpy(outputData, str, sizeof(str));

	(*size) = sizeof(str);
}



void RemoveSetParking()
{
	my_motor_value[0] = 0;
	my_motor_value[1] = 0;
	my_motor_value[2] = 0;
	my_motor_value[3] = 0;


	uint32_t fixedTime = uwTick;
	while(1) {
		if((uwTick - fixedTime) > 5000) {
			return;
		}
	}
}

// ---------------------------------------------------------------------------------------

void SetMotorValues(uint8_t *inputData, uint8_t *outputData, uint8_t *size, uint32_t time)
{
	uint16_t Rpos = 0;
	float M[4];
	for(int i = 0; i < 4; ++i) {
		M[i] = readFloat(inputData, &Rpos, ENTITY_LSB);

		if(M[i] > 1.0) {
			M[i] = 1.0;
		}

		if(M[i] < 0.0) {
			M[i] = 0.0;
		}

		my_motor_value[i] = (M[i] * DSHOT_RANGE) + DSHOT_MIN_THROTTLE;
	}
	(*size) = 0;
}

void GetTelemetry(uint8_t *inputData, uint8_t *outputData, uint8_t *size, uint32_t time)
{
	extern DshotTelemetry_t telemetry;
	uint16_t Wpos = 0;
	//int i =0;
	for(int i = 0; i < 4; ++i) {
		writeFloat(outputData, &Wpos, telemetry.temperature[i], ENTITY_LSB);
		writeFloat(outputData, &Wpos, telemetry.voltage[i], ENTITY_LSB);
		writeFloat(outputData, &Wpos, telemetry.current[i], ENTITY_LSB);
		writeFloat(outputData, &Wpos, telemetry.consumption[i], ENTITY_LSB);
		writeFloat(outputData, &Wpos, telemetry.ERpmp[i], ENTITY_LSB);
	}
	(*size) = Wpos;
}

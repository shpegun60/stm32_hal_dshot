/*
 * dshot_telemetry.h
 *
 *  Created on: Nov 3, 2022
 *      Author: Shpegun60
 */

#ifndef DSHOT_DSHOT_TELEMETRY_H_
#define DSHOT_DSHOT_TELEMETRY_H_

#include "main.h"

#define TELEMETRY_MOTOR_COUNTS 4

typedef struct {

	UART_HandleTypeDef *huart;
	uint8_t UART_rxBuffer[10];
	uint8_t telemetryState;
	volatile uint8_t packetReady;
	uint8_t telemetryBits[TELEMETRY_MOTOR_COUNTS];
	int telemetryPosition;
	uint32_t lastUpdatetime;

	float temperature[TELEMETRY_MOTOR_COUNTS];
	float voltage[TELEMETRY_MOTOR_COUNTS];
	float current[TELEMETRY_MOTOR_COUNTS];
	float consumption[TELEMETRY_MOTOR_COUNTS];
	float ERpmp[TELEMETRY_MOTOR_COUNTS];
} DshotTelemetry_t;

void dshotTelemetryInit(DshotTelemetry_t * self, UART_HandleTypeDef *huart);
void dshotUpdateAll(DshotTelemetry_t * self, uint16_t* motor_value, uint32_t time);
void dshotUpdateTelemetry(DshotTelemetry_t * self, uint32_t time);
void dshotTelemetrySetReady(DshotTelemetry_t * self);



#endif /* DSHOT_DSHOT_TELEMETRY_H_ */

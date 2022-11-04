/*
 * dshot_telemetry.c
 *
 *  Created on: Nov 3, 2022
 *      Author: admin
 */

#include "dshot_telemetry.h"
#include "dshot.h"
#include <stdio.h>
#include <string.h>


static uint8_t dshotTelemetryUpdateCrc8(uint8_t crc, uint8_t crc_seed);
static uint8_t dshotTelemetryGetCrc8(uint8_t *Buf, uint8_t BufLen);

void dshotTelemetryInit(DshotTelemetry_t * self, UART_HandleTypeDef *huart)
{
	if((huart == NULL) || (self == NULL)) {
		return;
	}
	memset(self, 0, sizeof(DshotTelemetry_t));

	dshot_init(DSHOT600);

	self->huart = huart;
	HAL_UART_Receive_DMA(huart, self->UART_rxBuffer, 10);
}

void dshotUpdateAll(DshotTelemetry_t * self, uint16_t* motor_value, uint32_t time)
{
	if(dshotReady()) {
		dshot_write(motor_value, self->telemetryBits);
	}

	dshotUpdateTelemetry(self, time);
}

void dshotUpdateTelemetry(DshotTelemetry_t * self, uint32_t time)
{
	switch(self->telemetryState) {

	case 0:
		for(int i = 0; i < TELEMETRY_MOTOR_COUNTS; ++i) {
			self->telemetryBits[i] = (self->telemetryPosition == i) ? 1 : 0;
		}
		HAL_UART_Receive_DMA(self->huart, self->UART_rxBuffer, 10);
		self->packetReady = 0;
		self->lastUpdatetime = time;
		++self->telemetryState;
		break;

	case 1:
		if(self->packetReady) {

			if(dshotTelemetryGetCrc8(self->UART_rxBuffer, 9) == self->UART_rxBuffer[9]) {
				uint16_t tmp = 0;

				self->temperature[self->telemetryPosition] = (float)self->UART_rxBuffer[0];

				tmp = (uint16_t)(self->UART_rxBuffer[1] << 8) | self->UART_rxBuffer[2];
				self->voltage[self->telemetryPosition] = (float)tmp * 0.01;

				tmp = (uint16_t)(self->UART_rxBuffer[3] << 8) | self->UART_rxBuffer[4];
				self->current[self->telemetryPosition] = (float)tmp * 0.01;

				tmp = (uint16_t)(self->UART_rxBuffer[5] << 8) | self->UART_rxBuffer[6];
				self->consumption[self->telemetryPosition] = (float)tmp;

				tmp = (uint16_t)(self->UART_rxBuffer[7] << 8) | self->UART_rxBuffer[8];
				self->ERpmp[self->telemetryPosition] = ((float)tmp * 100.0) * 0.166666667;
			}

			HAL_UART_DMAStop(self->huart);
			++self->telemetryState;
		} else if((time - self->lastUpdatetime) > 500) {
			HAL_UART_DMAStop(self->huart);
			self->telemetryState = 0;
		}
		break;

	case 2:
		self->telemetryPosition = ((self->telemetryPosition + 1) < TELEMETRY_MOTOR_COUNTS) ? (self->telemetryPosition + 1) : 0;
		self->lastUpdatetime = time;
		++self->telemetryState;
		break;

	case 3:
		if((time - self->lastUpdatetime) > 100) {
			self->telemetryState = 0;
			self->lastUpdatetime = time;
		}
		break;
	}
}

void dshotTelemetrySetReady(DshotTelemetry_t * self)
{
	self->packetReady = 1;
}

// --------------------dshot CRC calculation-----------------------------------------------------
static uint8_t dshotTelemetryUpdateCrc8(uint8_t crc, uint8_t crc_seed)
{
	uint8_t crc_u, i;
	crc_u = crc;
	crc_u ^= crc_seed;

	for (i = 0; i < 8; ++i) {
		crc_u = ( crc_u & 0x80 ) ? 0x7 ^ ( crc_u << 1 ) : ( crc_u << 1 );
	}
	return (crc_u);
}

static uint8_t dshotTelemetryGetCrc8(uint8_t *Buf, uint8_t BufLen)
{
	uint8_t crc = 0, i;

	for(i = 0; i < BufLen; ++i) {
		crc = dshotTelemetryUpdateCrc8(Buf[i], crc);
	}
	return (crc);
}


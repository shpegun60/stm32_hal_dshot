#include "callbacks.h"
#include "comm_obj.h"
#include "dshot_telemetry.h"



// uart callback
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	extern DshotTelemetry_t telemetry;

	if (huart->Instance == USART2) {

		RawParser_ProceedByte(&comm.rawparser, comm.receiveByte);
		HAL_UART_Receive_IT(huart, (uint8_t*) &comm.receiveByte, 1);
	} else if(huart->Instance == USART1) {
		dshotTelemetrySetReady(&telemetry);
	}
}

// system milliseconds callback
void HAL_SYSTICK_Callback(void)
{

}



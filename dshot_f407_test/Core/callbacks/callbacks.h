#ifndef INC_CALLBACKS_H_
#define INC_CALLBACKS_H_

#include "main.h"

// uart callback
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

// system milliseconds callback
void HAL_SYSTICK_Callback(void);


#endif /* INC_CALLBACKS_H_ */

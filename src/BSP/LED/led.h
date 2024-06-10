#ifndef __LED__H
#define __LED__H

#ifdef __cplusplus
extern "C" {
#endif
	
#include "main.h"

#define LED3_ON() HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET)
#define LED3_OFF() HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET)
#define LED3_Toggle() HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin)

#define LED2_ON() HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET)
#define LED2_OFF() HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET)
#define LED2_Toggle() HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin)

#ifdef __cplusplus
}
#endif

#endif
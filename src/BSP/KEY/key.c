#include "key.h"
#include "main.h"
#include "led.h"

int flag = -1;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == KEY1_Pin)
  {
//    HAL_Delay(2); /* 延时一小段时间，消除抖动 */
//    if (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET)
//      flag = 0;
		LED3_Toggle();
  }
  else if (GPIO_Pin == KEY2_Pin)
  {
//    HAL_Delay(2); /* 延时一小段时间，消除抖动 */
//    if (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_RESET)
//      flag = 1;
		LED3_Toggle();
  }
  else if (GPIO_Pin == KEY3_Pin)
  {
//    HAL_Delay(2); /* 延时一小段时间，消除抖动 */
//    if (HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin) == GPIO_PIN_RESET)
//      flag = 2;
		LED3_Toggle();
  }
}
#include "exti.h"
#include "led.h"
#include "DisplayPrivate.h"

int flag = -1;
Button key1, key2, key3;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == KEY1_Pin)
  {
    HAL_Delay(2); /* 延时一小段时间，消除抖动 */
    if (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET)
    {
      page.PageEventTransmit(&key1, EVENT_ButtonClick);
    }
  }
  else if (GPIO_Pin == KEY2_Pin)
  {
    HAL_Delay(2);
    if (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_RESET)
    {
      page.PageEventTransmit(&key2, EVENT_ButtonClick);
    }
  }
  else if (GPIO_Pin == KEY3_Pin)
  {
    HAL_Delay(2);
    if (HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin) == GPIO_PIN_RESET)
    {
      page.PageEventTransmit(&key3, EVENT_ButtonClick);
    }
  }
}
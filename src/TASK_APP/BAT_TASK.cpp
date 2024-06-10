#include "main.h"
#include "adc.h"
#include "cmsis_os.h"
#include "stdio.h"
#include "DisplayPrivate.h"
extern "C" void BAT_TASK(void *argument);
uint8_t ADC_Conv_Flag = 0;			 // ADC转换完成的标志位
float ADC_VAL = 3.96;						 // 实际电压
static uint32_t ADC_RAW_VAL = 0; // 原始值

void BAT_TASK(void *argument)
{
	// HAL_ADC_Start(&hadc1);
	while (1)
	{
		// if (HAL_OK == HAL_ADC_PollForConversion(&hadc1, 1000))
		// {
		// 	ADC_RAW_VAL = HAL_ADC_GetValue(&hadc1);
		// 	ADC_VAL = ADC_RAW_VAL * 3.3 / 4096; // 因为采样电压是分压后的,所以要乘以2
		// 	// printf("ADC = %.3f V \r\n", ADC_VAL);
		// 	HAL_ADC_Start(&hadc1);
		// }
		// BAT_Update();
		vTaskDelay(1000);
	}
}
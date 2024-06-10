#include "led.h"
#include "main.h"
#include "cmsis_os.h"
#include "stdio.h"
#include "task.h"
#include "DisplayPrivate.h"

extern "C" void LEDTask(void *argument);
extern uint8_t date_idx;
extern uint8_t time_idx;
void LEDTask(void *argument)
{
	while (1)
	{
		/* ��ض�ջ��� */
		// char mytaskstatebuffer[400];
		// printf("==================================\r\n");
		// printf("������   ����״̬  ���ȼ�  ʣ��ջ   �������\r\n");
		// vTaskList(mytaskstatebuffer);//��Ҫ��FreeRTOSConfig.h�￪��һ����
		// printf("%s\r\n", mytaskstatebuffer);
		// printf("132\r\n");
		if (page.NowPage == PAGE_MAIN)
		{
			Time_Update(); // GUI���ʱ�����
		}
		LED2_Toggle();
		vTaskDelay(1000);
	}
}
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
		/* 监控堆栈情况 */
		// char mytaskstatebuffer[400];
		// printf("==================================\r\n");
		// printf("任务名   任务状态  优先级  剩余栈   任务序号\r\n");
		// vTaskList(mytaskstatebuffer);//需要在FreeRTOSConfig.h里开启一个宏
		// printf("%s\r\n", mytaskstatebuffer);
		// printf("132\r\n");
		if (page.NowPage == PAGE_MAIN)
		{
			Time_Update(); // GUI里的时间更新
		}
		LED2_Toggle();
		vTaskDelay(1000);
	}
}
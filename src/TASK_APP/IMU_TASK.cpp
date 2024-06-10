#include "main.h"
#include "cmsis_os.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "DisplayPrivate.h"

extern "C" void IMU_TASK(void *argument);
extern uint8_t Screen_OFF;

IMU_info IMU;
void IMU_TASK(void *argument)
{
	while (MPU_Init())
		;
	while (mpu_dmp_init())
		;

	while (1)
	{
		//		while (mpu_dmp_get_data(&IMU));
		while (MPU_Get_Gyroscope(&IMU))
			;
		while (MPU_Get_Accelerometer(&IMU))
			;
		if (page.NowPage == PAGE_MAIN)
		{
			Steps_Update();
			vTaskDelay(300);
		}
		else if (page.NowPage == PAGE_MONITORING)
		{
			IMU_Update();
			vTaskDelay(300);
		}
		else if (page.NowPage == PAGE_SETTING)
		{
			Screen_awake();
			vTaskDelay(300);
		}
		else if (page.NowPage == PAGE_GAME)
		{
			vTaskDelay(50);
		}
		else
		{
			vTaskDelay(500);
		}
	}
}
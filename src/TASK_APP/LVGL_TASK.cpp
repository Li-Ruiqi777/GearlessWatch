#include "main.h"
#include "cmsis_os.h"
#include "lcd.h"
#include "lcd_init.h"
#include "CST816.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"

#include "DisplayPrivate.h"
#include "PageManager.h"

extern "C" void LVGL_TASK(void *argument);
void LVGL_TASK(void *argument)
{
  lv_init();            // lvgl系统初始化
  lv_port_disp_init();  // lvgl显示接口初始化,放在lv_init()的后面
  lv_port_indev_init(); // lvgl输入接口初始化,放在lv_init()的后面
  Display_Init();
  while (1)
  {
    lv_task_handler();
    vTaskDelay(2);
    page.Running();
  }
}
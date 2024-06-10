#include "DisplayPrivate.h"
#include "mpu6050.h"
#include "adc.h"
static lv_obj_t *appWindow;  /*此页面窗口*/
static lv_obj_t *label_gyro; /*陀螺仪的Label*/
static lv_obj_t *label_acc;  /*加速度计的Label*/
static lv_obj_t *label_bat;  /*电池电量的Label*/
static lv_obj_t *my_btn;

/* 字体声明 */
extern "C" const lv_font_t Calibri_18;

/* style声明 */
static lv_style_t line_style;
static lv_style_t title_style;
static lv_style_t IMU_style;
static lv_style_t BAT_style;
/* ---------------------------- 标签创建相关函数 ---------------------------- */

/**
 * @brief 页面标题的标签创建
 */
static void Intro_Create()
{
  /* 1.创建一条横线 */
  lv_style_init(&line_style);                              // 初始化style
  static lv_point_t line_points[] = {{20, 50}, {220, 50}}; // 设置线经过的点
  lv_obj_t *line1 = lv_line_create(lv_scr_act());          // 创建线
  lv_line_set_points(line1, line_points, 2);
  lv_style_set_line_width(&line_style, 6);                        // 设置线宽
  lv_style_set_line_color(&line_style, lv_color_make(255, 0, 0)); // 设置颜色
  lv_obj_add_style(line1, &line_style, NULL);

  /* 2.创建标题文字 */
  lv_obj_t *title = lv_label_create(appWindow); // 创建Label
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, -5); // 放在合适的位置
                                                // 创建style
  lv_style_init(&title_style);
  lv_style_set_text_font(&title_style, &Calibri_18);                   // 设置字体
  lv_style_set_text_color(&title_style, lv_color_make(255, 255, 255)); // 设置颜色
  lv_obj_add_style(title, &title_style, NULL);
  lv_label_set_text_fmt(title, "DATA INFO");
}

/**
 * @brief IMU标签的创建
 */
static void LabelIMU_Create()
{
  /* 1.创建陀螺仪的label */
  label_gyro = lv_label_create(appWindow);
  lv_obj_align(label_gyro, LV_ALIGN_DEFAULT, 0, 45); // 放在合适的位置

  /* 2.创建加速度计的label */
  label_acc = lv_label_create(appWindow);
  lv_obj_align(label_acc, LV_ALIGN_DEFAULT, 0, 120); // 放在合适的位置

  /* 2.创建style */
  lv_style_init(&IMU_style);
  lv_style_set_text_color(&IMU_style, lv_color_make(255, 255, 255)); // 设置颜色
  lv_obj_add_style(label_gyro, &IMU_style, NULL);
  lv_obj_add_style(label_acc, &IMU_style, NULL);

  /* 4.改变数值 */
  lv_label_set_text_fmt(label_gyro, "gyro_x : %.3f \ngyro_y : %.3f \ngyro_z : %.3f",
                        IMU.gx, IMU.gy, IMU.gz);

  lv_label_set_text_fmt(label_acc, "acc_x : %.3f \nacc_y : %.3f \nacc_z : %.3f",
                        IMU.ax, IMU.ay, IMU.az);
}

/**
 * @brief 电池电量标签的创建
 */
static void LabelBAT_Create()
{
  /* 1.创建电池的label */
  label_bat = lv_label_create(appWindow);
  lv_obj_align(label_bat, LV_ALIGN_DEFAULT, 0, 200); // 放在合适的位置

  /* 2.创建style */
  lv_style_init(&BAT_style);
  lv_style_set_text_color(&BAT_style, lv_color_make(255, 255, 255)); // 设置颜色
  lv_obj_add_style(label_bat, &BAT_style, NULL);

  /* 3.改变数值 */
  lv_label_set_text_fmt(label_bat, "Battery: %.2f V  " LV_SYMBOL_BATTERY_FULL "  %d %%", 4.1, 100);
}

/* ---------------------------- 回调函数 ---------------------------- */
/**
 * @brief 按键的回调函数
 * @param envent 引发的事件
 */
static void btn_event_cb(lv_event_t *envent)
{
  lv_obj_t *obj = lv_event_get_target(envent); // 获取触发事件的对象
  if (obj == my_btn)
  {
    page.PageEventTransmit(0, 1);
  }
}

/**
 * @brief IMU数据更新任务,每300ms运行一次(在IMU_task)
 * @param
 */
void IMU_Update(void)
{
  /* 1.陀螺仪数据更新 */
  lv_label_set_text_fmt(label_gyro, "gyro_x : %.3f \ngyro_y : %.3f \ngyro_z : %.3f",
                        IMU.gx, IMU.gy, IMU.gz);
  /* 2.加速度计数据更新 */
  lv_label_set_text_fmt(label_acc, "acc_x : %.3f \nacc_y : %.3f \nacc_z : %.3f",
                        IMU.ax, IMU.ay, IMU.az);
}

/**
 * @brief 电池电压更新任务,每10s运行一次(在BAT_task)
 * @param
 */
void BAT_Update(void)
{
  if (ADC_VAL <= 3.7)
  {
    lv_label_set_text_fmt(label_bat, "Battery: %.2f V  " LV_SYMBOL_BATTERY_EMPTY "  %d %%",
                          ADC_VAL, 0);
  }
  else if (ADC_VAL > 3.7 && ADC_VAL <= 3.8)
  {
    lv_label_set_text_fmt(label_bat, "Battery: %.2f V  " LV_SYMBOL_BATTERY_1 "  %d %%",
                          ADC_VAL, (ADC_VAL - 3.7) / 0.004);
  }
  else if (ADC_VAL > 3.8 && ADC_VAL <= 3.9)
  {
    lv_label_set_text_fmt(label_bat, "Battery: %.2f V  " LV_SYMBOL_BATTERY_2 "  %d %%",
                          ADC_VAL, (ADC_VAL - 3.7) / 0.004);
  }
  else if (ADC_VAL > 3.9 && ADC_VAL <= 4.1)
  {
    lv_label_set_text_fmt(label_bat, "Battery: %.2f V  " LV_SYMBOL_BATTERY_3 "  %d %%",
                          ADC_VAL, int((ADC_VAL - 3.7) / 0.004));
  }
  else if (ADC_VAL > 4.1)
  {
    lv_label_set_text_fmt(label_bat, "Battery: %.2f V  " LV_SYMBOL_BATTERY_FULL "  %d %%",
                          ADC_VAL, (ADC_VAL - 3.7) / 0.004);
  }
}

/* ---------------------------- 页面调度相关函数 ---------------------------- */
/**
 * @brief  页面初始化事件
 * @param  无
 * @retval 无
 */
static void Setup()
{
  lv_obj_move_foreground(appWindow); // 将此页面移到前台
  Intro_Create();                    // 标题
  LabelIMU_Create();                 // IMU信息
  LabelBAT_Create();                 // 电池电量信息
}

/**
 * @brief  页面退出事件
 * @param  无
 * @retval 无
 */
static void Exit()
{
  lv_obj_clean(appWindow);
  lv_style_reset(&line_style);
  lv_style_reset(&title_style);
  lv_style_reset(&IMU_style);
  lv_style_reset(&BAT_style);
}

/**
 * @brief  页面事件
 * @param  key:发出事件的按键
 * @param  event:事件编号
 * @retval 无
 */
static void Event(void *key, int event)
{
  if (key == &key1)
  {
    if (event == EVENT_ButtonClick)
      page.PagePush(PAGE_SETTING);
  }
  else if (key == &key2)
  {
    if (event == EVENT_ButtonClick)
      page.PagePop();
  }
}

/**
 * @brief  页面注册
 * @param  pageID:为此页面分配的ID号
 * @retval 无
 */
void PageRegister_PAGE_MONITORING(uint8_t pageID)
{
  /*获取分配给此页面的窗口*/
  appWindow = AppWindow_GetScr(pageID);

  /*注册至页面调度器*/
  page.PageRegister(pageID, Setup, NULL, Exit, Event);
}
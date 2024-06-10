#include "DisplayPrivate.h"
#include "tim.h"
#include "main.h"
#include "mpu6050.h"
#include "stdio.h"
static lv_obj_t *appWindow;    /*此页面窗口*/
static lv_obj_t *label_slider; /*slider的Label*/
static lv_obj_t *my_btn;
static lv_obj_t *screen_switch;
int32_t slider_val = 100;

uint8_t Screen_OFF = 0; /* 1:屏幕已关闭、0:未关闭 */
/* 字体声明 */
extern "C" const lv_font_t Calibri_18;

/* style声明 */
static lv_style_t line_style;
static lv_style_t title_style;
static lv_style_t intro_style;

/* 回调函数声明 */
static void slider_event_cb(lv_event_t *event);
static void Power_switch_cb(lv_event_t *event);
static void Screen_switch_cb(lv_event_t *event);
static void Click_event_cb(lv_event_t *event);
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
  lv_obj_t *title = lv_label_create(appWindow);                        // 创建Label
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, -5);                        // 放在合适的位置
  lv_style_init(&title_style);                                         // 创建style
  lv_style_set_text_font(&title_style, &Calibri_18);                   // 设置字体
  lv_style_set_text_color(&title_style, lv_color_make(255, 255, 255)); // 设置颜色
  lv_obj_add_style(title, &title_style, NULL);
  lv_label_set_text_fmt(title, "SETTING");
}

/**
 * @brief 亮度控制的标签创建
 */
static void Brightness_Control_Create()
{
  /* 1.创建标题Label */
  lv_obj_t *intro = lv_label_create(appWindow);  // 创建Label
  lv_obj_align(intro, LV_ALIGN_TOP_LEFT, 0, 45); // 放在合适的位置
  lv_style_init(&intro_style);
  lv_style_set_text_color(&intro_style, lv_color_make(255, 255, 255)); // 设置颜色
  lv_obj_add_style(intro, &intro_style, NULL);
  lv_label_set_text_fmt(intro, "Brightness : ");

  /* 2.创建滑动条 */
  lv_obj_t *slider = lv_slider_create(appWindow);
  lv_obj_set_width(slider, 200);                  // 设置滑动条的长度
  lv_obj_align(slider, LV_ALIGN_TOP_LEFT, 0, 75); // 放在合适的位置

  /* 3.创建滑动条的Label */
  label_slider = lv_label_create(appWindow);
  lv_obj_add_style(label_slider, &intro_style, NULL);
  lv_label_set_text_fmt(label_slider, "%d%%", slider_val); // 设置label初始值
  lv_slider_set_value(slider, slider_val, LV_ANIM_OFF);    // 设置slider初始值
  lv_obj_align(label_slider, LV_ALIGN_TOP_LEFT, 110, 45);  // 放在合适的位置

  /* 4.添加滑动条的回调函数 */
  lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
}

/**
 * @brief 关机控制的标签创建
 */
static void Power_Control_Create()
{
  /* 1.创建标题Label */
  lv_obj_t *intro = lv_label_create(appWindow);   // 创建Label
  lv_obj_align(intro, LV_ALIGN_TOP_LEFT, 0, 120); // 放在合适的位置
  lv_style_init(&intro_style);
  lv_style_set_text_color(&intro_style, lv_color_make(255, 255, 255)); // 设置颜色
  lv_obj_add_style(intro, &intro_style, NULL);
  lv_label_set_text_fmt(intro, "Power : ");

  /* 2.创建开关 */
  lv_obj_t *power_switch = lv_switch_create(appWindow);
  lv_obj_align(power_switch, LV_ALIGN_TOP_LEFT, 100, 115);                          // 放在合适的位置
  lv_obj_set_size(power_switch, 100, 45);                                           // 设置大小
  lv_obj_add_event_cb(power_switch, Power_switch_cb, LV_EVENT_VALUE_CHANGED, NULL); // 添加回调函数
  lv_obj_add_state(power_switch, LV_STATE_CHECKED);                                 // 设置开关初始为ON
}

/**
 * @brief 屏幕关闭控制的标签创建
 */
static void Screen_Control_Create()
{
  /* 1.创建标题Label */
  lv_obj_t *intro = lv_label_create(appWindow);   // 创建Label
  lv_obj_align(intro, LV_ALIGN_TOP_LEFT, 0, 175); // 放在合适的位置
  lv_style_init(&intro_style);
  lv_style_set_text_color(&intro_style, lv_color_make(255, 255, 255)); // 设置颜色
  lv_obj_add_style(intro, &intro_style, NULL);
  lv_label_set_text_fmt(intro, "Screen : ");

  /* 2.创建开关 */
  screen_switch = lv_switch_create(appWindow);
  lv_obj_align(screen_switch, LV_ALIGN_TOP_LEFT, 100, 180);                           // 放在合适的位置
  lv_obj_set_size(screen_switch, 100, 45);                                            // 设置大小
  lv_obj_add_event_cb(screen_switch, Screen_switch_cb, LV_EVENT_VALUE_CHANGED, NULL); // 添加回调函数
  lv_obj_add_state(screen_switch, LV_STATE_CHECKED);                                  // 设置开关初始为ON
}
/* ---------------------------- 回调函数 ---------------------------- */

/**
 * @brief 滑动条的回调函数
 * @param event
 */
static void slider_event_cb(lv_event_t *event)
{
  lv_obj_t *slider = lv_event_get_target(event); // 获取触发回调的slider
  slider_val = lv_slider_get_value(slider);      // 得到当前slider的值

  /* 1.更新slider的Label */
  lv_label_set_text_fmt(label_slider, "%d%%", slider_val); // 刷新Label
  lv_obj_align(label_slider, LV_ALIGN_TOP_LEFT, 110, 45);  // 放在合适的位置

  /* 2.亮度调整 */
  __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, 50 + slider_val * 9.5);
}

/**
 * @brief 电源开关的回调函数
 * @param event
 */
static void Power_switch_cb(lv_event_t *event)
{
  lv_event_code_t code = lv_event_get_code(event); // 获取触发的事件代码

  if (code == LV_EVENT_VALUE_CHANGED)
  {
    HAL_GPIO_WritePin(Power_EN_GPIO_Port, Power_EN_Pin, GPIO_PIN_RESET);
  }
}

/**
 * @brief 屏幕开关的回调函数
 * @param event
 */
static void Screen_switch_cb(lv_event_t *event)
{
  lv_event_code_t code = lv_event_get_code(event); // 获取触发的事件代码

  if (code == LV_EVENT_VALUE_CHANGED)
  {
    Screen_OFF = 1; // 置位
    __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, 0);
    lv_obj_add_event_cb(lv_scr_act(), Click_event_cb, LV_EVENT_SHORT_CLICKED, NULL); // 创建触摸的回调函数
  }
}

/**
 * @brief 屏幕唤醒回调函数,调用于IMU_task()
 * @param
 */
void Screen_awake(void)
{
  if (IMU.gy > 220)
  {
    Screen_OFF = 0;
    lv_obj_add_state(screen_switch, LV_STATE_CHECKED);                  // 重新设置成ON
    __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, 50 + slider_val * 9.5); // 恢复原来的亮度
  }
}

/**
 * @brief 触摸的回调函数
 * @param event
 */
static void Click_event_cb(lv_event_t *event)
{
  if (Screen_OFF == 1)
  {
    lv_event_code_t code = lv_event_get_code(event); // 获取触发的事件代码

    if (code == LV_EVENT_CLICKED)
    {
      Screen_OFF = 0;
      __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, 50 + slider_val * 9.5); // 恢复原来的亮度
      lv_obj_remove_event_cb(lv_scr_act(), Click_event_cb);               // 删除回调函数
    }
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
  Intro_Create();
  Brightness_Control_Create();
  Power_Control_Create();
  Screen_Control_Create();
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
  lv_style_reset(&intro_style);
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
      page.PagePush(PAGE_GAME);
  }
  else if (key == &key2)
  {
    if (event == EVENT_ButtonClick)
      page.PagePop();
  }
  else if (key == &key3)
  {
    if (event == EVENT_ButtonClick)
    {
      Screen_OFF = 1; // 置位
      __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, 0);
      lv_obj_add_event_cb(lv_scr_act(), Click_event_cb, LV_EVENT_SHORT_CLICKED, NULL); // 创建触摸的回调函数
      lv_obj_clear_state(screen_switch, LV_STATE_CHECKED);                             // 设置开关为OFF
    }
  }
}

/**
 * @brief  页面注册
 * @param  pageID:为此页面分配的ID号
 * @retval 无
 */
void PageRegister_PAGE_SETTING(uint8_t pageID)
{
  /*获取分配给此页面的窗口*/
  appWindow = AppWindow_GetScr(pageID);

  /*注册至页面调度器*/
  page.PageRegister(pageID, Setup, NULL, Exit, Event);
}
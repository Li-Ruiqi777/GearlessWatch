#include "DisplayPrivate.h"
#include "lvgl.h"
#include "CommonMacro.h"
#include "rtc.h"
#include "stdio.h"
#include "mpu6050.h"
#include "inv_mpu_dmp_motion_driver.h"

RTC_DateTypeDef GetDate = {
    .WeekDay = 2,
    .Year = 23,
    .Month = 1,
    .Date = 10}; // 获取日期结构体
RTC_TimeTypeDef GetTime = {
    .Hours = 4,
    .Minutes = 20,
    .Seconds = 0};    // 获取时间结构体
uint8_t date_idx = 0; // 用于更新的计数变量
uint8_t time_idx = 0;

static lv_obj_t *appWindow;        /*此页面窗口*/
static lv_obj_t *labelBatt;        /*电池信息*/
static lv_obj_t *labelDate;        /*时间信息*/
static lv_obj_t *ledSec[2];        /*秒指示灯LED*/
static lv_obj_t *labelTime_Grp[4]; /*时间标签*/
static lv_obj_t *imgRun;           /*运动图标*/
static lv_obj_t *labelStepCnt;     /*计步次数标签*/
static lv_obj_t *my_btn;

/* 字体声明 */
extern "C" const lv_img_dsc_t ImgRun;
extern "C" const lv_font_t Calibri_48;
extern "C" const lv_font_t Calibri_28;
extern "C" const lv_font_t Calibri_18;

/* style声明 */
static lv_style_t step_style;
static lv_style_t date_style;
static lv_style_t time_style;
/* ---------------------------- 标签创建相关函数 ---------------------------- */

/**
 * @brief 计步标签的创建
 */
static void LabelStep_Create()
{
  /* 1.加上跑步的图标 */
  imgRun = lv_img_create(appWindow);
  lv_img_set_src(imgRun, &ImgRun);
  lv_obj_align(imgRun, LV_ALIGN_BOTTOM_MID, -50, -40);

  /* 2.创建步数label */
  labelStepCnt = lv_label_create(appWindow);
  lv_obj_align(labelStepCnt, LV_ALIGN_BOTTOM_MID, 0, -30); // 放在合适的位置

  /* 3.加上style */
  lv_style_init(&step_style);
  lv_style_set_text_font(&step_style, &Calibri_18);                   // 设置字体
  lv_style_set_text_color(&step_style, lv_color_make(255, 255, 255)); // 设置颜色
  lv_obj_add_style(labelStepCnt, &step_style, NULL);

  /* 4.改变数值 */
  lv_label_set_recolor(labelStepCnt, true);             // 使字体颜色能够修改
  lv_label_set_text_fmt(labelStepCnt, "%d", IMU.steps); // 更新数值
}

/**
 * @brief 日期标签的创建
 */
static void LabelDate_Create()
{
  /* 1.创建日期label */
  labelDate = lv_label_create(appWindow);
  lv_obj_align(labelDate, LV_ALIGN_TOP_MID, 0, 10); // 设置合适的位置

  /* 2.加上style */
  lv_style_init(&date_style);
  lv_style_set_text_font(&date_style, &Calibri_18);                   // 设置字体
  lv_style_set_text_color(&date_style, lv_color_make(255, 255, 255)); // 设置颜色
  lv_obj_add_style(labelDate, &date_style, NULL);

  /* 3.内容设置 */
  lv_label_set_recolor(labelDate, true);
  lv_label_set_text_fmt(labelDate, "01#FF0000 /#01 MON"); // 先随便设置一个日期,然后由日期更新回调重设
}

/**
 * @brief 时间标签的创建
 */
static void LabelTime_Create()
{
  /* 1.创建2个LED */
  for (int i = 0; i < __Sizeof(ledSec); i++)
  {
    lv_obj_t *led = lv_led_create(appWindow);
    lv_led_set_color(led, lv_color_make(255, 0, 0));          // 设置颜色
    lv_obj_set_size(led, 5, 5);                               // 设置大小
    lv_obj_align(led, LV_ALIGN_CENTER, 0, i == 0 ? -18 : 18); // 设置位置
    ledSec[i] = led;
  }

  /* 2.添加Time的style */
  lv_style_init(&time_style);
  lv_style_set_text_font(&time_style, &Calibri_48);                   // 设置字体
  lv_style_set_text_color(&time_style, lv_color_make(255, 255, 255)); // 设置颜色

  const lv_coord_t x_mod[4] = {-75, -30, 30, 75}; // 设置位置
  for (int i = 0; i < __Sizeof(labelTime_Grp); i++)
  {
    lv_obj_t *label = lv_label_create(appWindow); // 时间的每一位单独为一个Label
    lv_obj_add_style(label, &time_style, NULL);
    lv_label_set_text(label, "0");
    lv_obj_align(label, LV_ALIGN_CENTER, x_mod[i], 0); // 设置位置
    labelTime_Grp[i] = label;                          // 这个数组用来存时间每一位的Label
  }
}

/* ---------------------------- 回调函数 ---------------------------- */

/**
 * @brief 按键的回调函数
 * @param envent
 */
static void btn_event_cb(lv_event_t *envent)
{
  lv_obj_t *obj = lv_event_get_target(envent); // 获取触发事件的对象
  if (obj == my_btn)
  {
    page.PageEventTransmit(0, 2);
  }
}

/**
 * @brief 手势的回调函数
 * @param event
 */
static void ges_cb(lv_event_t *event)
{
  if (lv_event_get_code(event) == LV_EVENT_GESTURE)
  {
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    if (dir == LV_DIR_TOP || dir == LV_DIR_ALL)
    {
      page.PageEventTransmit(0, 1);
    }
  }
}

/**
 * @brief 时间更新任务,每秒运行一次(在led_task)
 * @param
 */
void Time_Update(void)
{
  /* 1.获取时间和日期 */
  // HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
  // HAL_RTC_GetDate(&hrtc, &GetDate, RTC_FORMAT_BIN);
  RTC_READ(&GetTime, &GetDate);
  /* 2.时间的更新 */
  if (time_idx == 0 || time_idx == 10)
  {
    lv_obj_t *now_label;
    uint8_t time_buffer[4];
    time_buffer[0] = GetTime.Hours / 10;
    time_buffer[1] = GetTime.Hours % 10;
    time_buffer[2] = GetTime.Minutes / 10;
    time_buffer[3] = GetTime.Minutes % 10;
    for (int i = 0; i < 4; ++i)
    {
      now_label = labelTime_Grp[i];
      lv_label_set_text_fmt(now_label, "%d", time_buffer[i]); // 分别设置时间的每一个Label
    }
    // printf("%02d:%02d:%02d\r\n", GetTime.Hours, GetTime.Minutes, GetTime.Seconds);
    time_idx = 1;
  }

  /* 3.日期的更新 */
  if (date_idx == 0 || (date_idx % 60 == 0))
  {
    const char *week_str[7] = {"MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN"};
    int8_t index = GetDate.WeekDay - 1;
    /* 格式:月/日 星期 */
    lv_label_set_text_fmt(labelDate, "%2d#FF0000 /#%d  %s", GetDate.Month, GetDate.Date, week_str[index]);
  }

  time_idx++;
  date_idx++;
}

/**
 * @brief 步数更新任务,每300ms运行一次(在IMU_task)
 * @param
 */
void Steps_Update(void)
{
  if (dmp_get_pedometer_step_count(&(IMU.steps)) == 0)
  {
    lv_label_set_text_fmt(labelStepCnt, "%d", IMU.steps); // 更新数值
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

  /*将此页面移到前台*/
  lv_obj_move_foreground(appWindow);
  lv_obj_add_event_cb(lv_scr_act(), ges_cb, LV_EVENT_GESTURE, NULL); // 创建手势的回调函数

  LabelDate_Create(); // 创建日期
  LabelStep_Create(); // 创建计步
  LabelTime_Create(); // 创建时间

  /* 按键相关 */
  // my_btn = lv_btn_create(appWindow);                                 // 创建按钮
  // lv_obj_set_size(my_btn, 5, 5);                                     // 设置大小
  // lv_obj_align(my_btn, LV_ALIGN_BOTTOM_LEFT, 0, 0);                  // 设置位置
  // lv_obj_add_event_cb(my_btn, btn_event_cb, LV_EVENT_CLICKED, NULL); // 按键添加回调

  /* 更新时间 */
  date_idx = 0;
  time_idx = 0;
  Time_Update();
}

/**
 * @brief  页面退出事件
 * @param  无
 * @retval 无
 */
static void Exit()
{
  lv_obj_clean(appWindow);
  lv_style_reset(&time_style);
  lv_style_reset(&date_style);
  lv_style_reset(&step_style);
  lv_obj_remove_event_cb(lv_scr_act(), ges_cb);
}

/**
 * @brief  页面事件
 * @param  key:发出事件的按键
 * @param  event:事件编号
 * @retval 无
 */
static void Event(void *key, int event)
{
  // if (event == 1) // 手势触发事件
  // {
  //   page.PageChangeTo(PAGE_MONITORING);
  // }
  // else if (event == 2)
  //   page.PagePush(PAGE_MONITORING); // 打开表盘
  if (key == &key1)
  {
    if (event == EVENT_ButtonClick)
      page.PagePush(PAGE_MONITORING);
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
void PageRegister_PAGE_MAIN(uint8_t pageID)
{
  /*获取分配给此页面的窗口*/
  appWindow = AppWindow_GetScr(pageID);

  /*注册至页面调度器*/
  page.PageRegister(pageID, Setup, NULL, Exit, Event);
}

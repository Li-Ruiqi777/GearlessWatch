#include "DisplayPrivate.h"

static lv_obj_t *appWindow; /*此页面窗口*/
static lv_obj_t *line1;
/* 字体声明 */
extern "C" const lv_font_t Calibri_18;

/* style声明 */
static lv_style_t line_style;
static lv_style_t title_style;
static lv_style_t intro_style;
/* ---------------------------- 标签创建相关函数 ---------------------------- */

/**
 * @brief 页面标题的标签创建
 */
static void Intro_Create()
{
  /* 1.创建一条横线 */
  lv_style_init(&line_style);                              // 初始化style
  static lv_point_t line_points[] = {{20, 50}, {220, 50}}; // 设置线经过的点
  line1 = lv_line_create(lv_scr_act());                    // 创建线
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
  lv_label_set_text_fmt(title, "ABOUT");
}

/**
 * @brief 简介的标签创建
 */
static void Brief_Create()
{
  /* 1.创建作品名称 */
  lv_obj_t *title1 = lv_label_create(appWindow); // 创建Label
  lv_obj_align(title1, LV_ALIGN_CENTER, 0, -60); // 放在合适的位置
  lv_style_init(&intro_style);
  // lv_style_set_text_font(&title_style, &Calibri_18);                   // 设置字体
  lv_style_set_text_color(&intro_style, lv_color_make(255, 255, 255)); // 设置颜色
  lv_obj_add_style(title1, &intro_style, NULL);
  lv_label_set_text_fmt(title1, "- Gearless watch -");

  /* 2.创建版本信息 */
  lv_obj_t *title2 = lv_label_create(appWindow); // 创建Label
  lv_obj_align(title2, LV_ALIGN_CENTER, 0, -30); // 放在合适的位置
  lv_obj_add_style(title2, &intro_style, NULL);
  lv_label_set_text_fmt(title2, "- Version -  v0.1");

  /* 3.创建作者名称 */
  lv_obj_t *title3 = lv_label_create(appWindow); // 创建Label
  lv_obj_align(title3, LV_ALIGN_CENTER, 0, 10);  // 放在合适的位置
  lv_obj_add_style(title3, &intro_style, NULL);
  lv_label_set_text_fmt(title3, "- Developer -");

  lv_obj_t *title4 = lv_label_create(appWindow); // 创建Label
  lv_obj_align(title4, LV_ALIGN_CENTER, 0, 30);  // 放在合适的位置
  lv_obj_add_style(title4, &intro_style, NULL);
  lv_label_set_text_fmt(title4, "Li Ruiqi");

  /* 4.创建完成日期 */
  lv_obj_t *title5 = lv_label_create(appWindow); // 创建Label
  lv_obj_align(title5, LV_ALIGN_CENTER, 0, 70);  // 放在合适的位置
  lv_obj_add_style(title5, &intro_style, NULL);
  lv_label_set_text_fmt(title5, "January 11 2023");
}
/* ---------------------------- 回调函数 ---------------------------- */

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
  Brief_Create();
}

/**
 * @brief  页面退出事件
 * @param  无
 * @retval 无
 */
static void Exit()
{
  lv_style_reset(&line_style);
  lv_style_reset(&title_style);
  lv_obj_del(line1);
  lv_obj_clean(appWindow);
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
      page.PagePush(PAGE_MAIN);
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
void PageRegister_PAGE_ABOUT(uint8_t pageID)
{
  /*获取分配给此页面的窗口*/
  appWindow = AppWindow_GetScr(pageID);

  /*注册至页面调度器*/
  page.PageRegister(pageID, Setup, NULL, Exit, Event);
}
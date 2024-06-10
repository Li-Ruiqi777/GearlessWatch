#include "DisplayPrivate.h"
#include "cmsis_os.h"
#include "mpu6050.h"
#include "math.h"

#define Refresh_time 5
#define Border_Left 10
#define Border_Right 230
#define Border_Top 50
#define Border_Bottom 230

class BALL_Info
{
public:
  BALL_Info(uint8_t position_x, uint8_t position_y, int8_t velocity_x, int8_t velocity_y)
  {
    this->position_x = position_x;
    this->position_y = position_y;
    this->velocity_x = velocity_x;
    this->velocity_y = velocity_y;
  }
  ~BALL_Info() {}
  void Position_Update(void);
  void Velocity_Update(void);

  uint8_t position_x;
  uint8_t position_y;
  int8_t velocity_x;
  int8_t velocity_y;
};

class BAR_Info
{
public:
  BAR_Info(uint8_t position_x, int8_t velocity_x, uint8_t length)
  {
    this->position_x = position_x;
    this->length = length;
    this->velocity_x = velocity_x;
  }
  ~BAR_Info() {}
  void Position_Update(void);
  void Velocity_Update(void);

  uint8_t position_x; // 左顶点横坐标
  int8_t velocity_x;
  lv_coord_t length; // 长度
};

/**
 * @brief 最大值限幅
 */
static void restrict_max(uint8_t *data, uint8_t limit)
{
  if (*data > limit)
  {
    *data = limit;
  }
}
/**
 * @brief 最小值限幅
 */
static void restrict_min(uint8_t *data, uint8_t limit)
{
  if (*data < limit)
  {
    *data = limit;
  }
}

static lv_obj_t *appWindow;                                         /*此页面窗口*/
static lv_obj_t *title;                                             /*计分板Label*/
static lv_obj_t *ball;                                              /*弹球*/
static lv_obj_t *line1;                                             /*上边线*/
static lv_obj_t *line2;                                             /*左边线*/
static lv_obj_t *line3;                                             /*右边线*/
static lv_obj_t *bar;                                               /*挡板*/
static BALL_Info ball_info(Border_Left + 5, Border_Top + 10, 1, 1); /*球的移动信息*/
static BAR_Info bar_info(15, 3, 60);
static uint16_t Score = 0;       /*当前得分*/
static uint8_t Restart_flag = 0; /*游戏重启标志位*/

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
  lv_style_init(&line_style);                                                                // 初始化style
  static lv_point_t line_points[] = {{Border_Left, Border_Top}, {Border_Right, Border_Top}}; // 设置线经过的点
  lv_obj_t *line1 = lv_line_create(lv_scr_act());                                            // 创建线
  lv_line_set_points(line1, line_points, 2);
  lv_style_set_line_width(&line_style, 6);                        // 设置线宽
  lv_style_set_line_color(&line_style, lv_color_make(255, 0, 0)); // 设置颜色
  lv_obj_add_style(line1, &line_style, NULL);

  /* 2.创建标题文字 */
  lv_obj_t *intro = lv_label_create(appWindow);                        // 创建Label
  lv_obj_align(intro, LV_ALIGN_TOP_MID, 0, -5);                        // 放在合适的位置
  lv_style_init(&intro_style);                                         // 创建style
  lv_style_set_text_font(&intro_style, &Calibri_18);                   // 设置字体
  lv_style_set_text_color(&intro_style, lv_color_make(255, 255, 255)); // 设置颜色
  lv_obj_add_style(intro, &intro_style, NULL);
  lv_label_set_text_fmt(intro, "RELAX");
}

/**
 * @brief 游戏场景的创建
 */
static void Game_Create(void)
{
  /* 1.创建一个小球 */
  ball = lv_led_create(lv_scr_act());
  lv_led_set_color(ball, lv_color_make(0, 255, 0));                        // 设置颜色
  lv_obj_set_size(ball, 8, 8);                                             // 设置大小
  lv_obj_align(ball, LV_ALIGN_DEFAULT, Border_Left + 10, Border_Top + 10); // 设置位置

  /* 2.创建左边界线 */
  static lv_point_t line_points1[] = {{Border_Left, Border_Top}, {Border_Left, Border_Bottom}}; // 设置线经过的点
  line2 = lv_line_create(lv_scr_act());                                                         // 创建线
  lv_line_set_points(line2, line_points1, 2);
  lv_obj_add_style(line2, &line_style, NULL);

  /* 3.创建右边界线 */
  static lv_point_t line_points2[] = {{Border_Right, Border_Top}, {Border_Right, Border_Bottom}}; // 设置线经过的点
  line3 = lv_line_create(lv_scr_act());                                                           // 创建线
  lv_line_set_points(line3, line_points2, 2);
  lv_obj_add_style(line3, &line_style, NULL);

  /* 4.创建挡板 */
  bar = lv_btn_create(lv_scr_act());                                   // 创建按钮
  lv_obj_set_size(bar, bar_info.length, 5);                            // 设置大小
  lv_obj_align(bar, LV_ALIGN_DEFAULT, Border_Left, Border_Bottom + 8); // 设置位置

  /* 5.创建计分板 */
  title = lv_label_create(appWindow);           // 创建Label
  lv_obj_align(title, LV_ALIGN_CENTER, 0, -60); // 放到屏幕中间
  // lv_obj_align(title, LV_ALIGN_DEFAULT, Border_Right - 95, Border_Top + 5); // 放在合适的位置
  lv_style_init(&title_style);                                         // 创建style
  lv_style_set_text_color(&title_style, lv_color_make(255, 255, 255)); // 设置颜色
  lv_obj_add_style(title, &title_style, NULL);
  lv_label_set_text_fmt(title, "Score: %d", Score);
}

/* ---------------------------- 回调函数 ---------------------------- */

/**
 * @brief 根据信息更新ball的速度
 * @param
 */
void BALL_Info::Velocity_Update(void)
{
  if (this->position_x == Border_Left || this->position_x == Border_Right)
  {
    this->velocity_x = -this->velocity_x;
  }
  if (this->position_y == Border_Top || this->position_y == Border_Bottom)
  {
    this->velocity_y = -this->velocity_y;
  }
}

/**
 * @brief 根据信息更新ball在图像中的位置
 * @param
 */
void BALL_Info::Position_Update(void)
{
  /* 1.由速度计算新的坐标 */
  this->position_x += this->velocity_x;
  this->position_y += this->velocity_y;
  /* 2.位置限幅 */
  restrict_max(&this->position_x, Border_Right);
  restrict_min(&this->position_x, Border_Left);
  restrict_max(&this->position_y, Border_Bottom);
  restrict_min(&this->position_y, Border_Top);
  /* 3.设置位置 */
  lv_obj_align(ball, LV_ALIGN_DEFAULT, this->position_x, this->position_y);
}

/**
 * @brief 根据信息更新bar的速度
 * @param
 */
void BAR_Info::Velocity_Update(void)
{
  if (IMU.ay > 0)
  {
    this->velocity_x = -abs(this->velocity_x);
  }
  else
  {
    this->velocity_x = abs(this->velocity_x);
  }
}

/**
 * @brief 根据信息更新bar的位置
 * @param
 */
void BAR_Info::Position_Update(void)
{
  /* 1.由速度计算新的坐标 */
  this->position_x += this->velocity_x;
  /* 2.位置限幅 */
  restrict_max(&this->position_x, Border_Right - this->length);
  restrict_min(&this->position_x, Border_Left);
  /* 3.设置位置 */
  lv_obj_align(bar, LV_ALIGN_DEFAULT, this->position_x, Border_Bottom + 8); // 设置位置
}

/**
 * @brief 游戏控制,如果球没接到,提醒用户已死亡
 * @param bar_info
 * @param ball_info
 */
static void Game_Control(BAR_Info *bar_info, BALL_Info *ball_info)
{
  if (ball_info->position_y == Border_Bottom) // 球触底了
  {
    if (ball_info->position_x <= bar_info->position_x + bar_info->length && ball_info->position_x >= bar_info->position_x)
    {
      Score += 1; // 成功接住球
      lv_label_set_text_fmt(title, "Score: %d", Score);
    }
    else // 接球失败
    {
      Score = 0;
      lv_label_set_text_fmt(title, "You are Dead!");
      Restart_flag = 1; /* 置位游戏重启 */
    }
  }
}

void Game_resart(BAR_Info *bar_info, BALL_Info *ball_info)
{
  if (Restart_flag == 1)
  {
    vTaskDelay(1000);
    vTaskDelay(1000); // 延时2s
    /* 重置位置 */
    lv_obj_align(ball, LV_ALIGN_DEFAULT, Border_Left + 10, Border_Top + 10);
    lv_obj_align(bar, LV_ALIGN_DEFAULT, Border_Left, Border_Bottom + 8);
    bar_info->position_x = Border_Left;
    ball_info->position_x = Border_Left + 10;
    ball_info->position_y = Border_Top + 5;

    Restart_flag = 0;
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
  Game_Create();
}

/**
 * @brief  页面循环事件
 * @param  无
 * @retval 无
 */
static void Loop()
{
  Game_resart(&bar_info, &ball_info);
  bar_info.Velocity_Update();
  bar_info.Position_Update();
  ball_info.Velocity_Update();
  ball_info.Position_Update();
  Game_Control(&bar_info, &ball_info);
  vTaskDelay(Refresh_time);
}

/**
 * @brief  页面退出事件
 * @param  无
 * @retval 无
 */
static void Exit()
{
  lv_style_reset(&title_style);
  lv_style_reset(&line_style);
  lv_style_reset(&intro_style);
  lv_obj_del(line1);
  lv_obj_del(line2);
  lv_obj_del(line3);
  lv_obj_del(bar);
  lv_obj_del(ball);

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
      page.PagePush(PAGE_ABOUT);
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
void PageRegister_PAGE_GAME(uint8_t pageID)
{
  /*获取分配给此页面的窗口*/
  appWindow = AppWindow_GetScr(pageID);

  /*注册至页面调度器*/
  page.PageRegister(pageID, Setup, Loop, Exit, Event);
}
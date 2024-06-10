#include "DisplayPrivate.h"

/*实例化页面调度器*/
PageManager page(PAGE_MAX);

/**
 * @brief  页面初始化
 */
static void Pages_Init()
{
  /* 1.注册所有页面 */
  PageRegister_PAGE_MAIN(PAGE_MAIN);
  PageRegister_PAGE_MONITORING(PAGE_MONITORING);
  PageRegister_PAGE_SETTING(PAGE_SETTING);
  PageRegister_PAGE_GAME(PAGE_GAME);
  PageRegister_PAGE_ABOUT(PAGE_ABOUT);
  /* 2.进入初始页面并压栈 */
  page.PagePush(PAGE_MAIN);
}

/**
 * @brief  显示初始化
 */
void Display_Init()
{
  /*APP窗口初始化*/
  AppWindow_Create();

  /*页面初始化*/
  Pages_Init();
}

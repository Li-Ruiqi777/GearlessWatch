#include "DisplayPrivate.h"

/* 将一个页面封装成了一个结构体,PageManager只是管理页面,页面本身是属于AppWindow_TypeDef */
typedef struct
{
  lv_obj_t *scr;    // 当前页面对应的屏幕
  lv_style_t style; // 当前页面的style
} AppWindow_TypeDef;

static AppWindow_TypeDef appWindow_Grp[PAGE_MAX]; // 存所有页面及其style的一个数组

/**
 * @brief 获取pageID对应的页面
 * @param pageID
 * @return
 */
lv_obj_t *AppWindow_GetScr(uint8_t pageID)
{
  return (pageID < PAGE_MAX) ? appWindow_Grp[pageID].scr : NULL;
}

lv_coord_t AppWindow_GetHeight()
{
  return (lv_obj_get_height(lv_scr_act())); // - BarStatus_GetHeight() - BarNavigation_GetHeight());
}

lv_coord_t AppWindow_GetWidth()
{
  return (lv_obj_get_width(lv_scr_act()));
}

/**
 * @brief 页面创建函数,会调用于每个页面注册的那个接口里
 */
void AppWindow_Create()
{
  for (int i = 0; i < PAGE_MAX; i++)
  {
    /* 1.调用lvgl的创建页面API */
    appWindow_Grp[i].scr = lv_obj_create(lv_scr_act());
    lv_obj_set_size(appWindow_Grp[i].scr, AppWindow_GetWidth(), AppWindow_GetHeight());
    lv_obj_align(appWindow_Grp[i].scr, LV_ALIGN_CENTER, 0, 0);

    /* 2.设置该页面的style,这里只设置背景颜色,其他具体的在标签创建API里设置 */
    lv_style_init(&appWindow_Grp[i].style);
    lv_style_set_bg_color(&appWindow_Grp[i].style, lv_color_make(18, 20, 14)); // 设置所有页面的背景颜色
    lv_obj_add_style(appWindow_Grp[i].scr, &appWindow_Grp[i].style, NULL);
  }
}

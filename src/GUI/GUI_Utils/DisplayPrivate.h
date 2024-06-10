#ifndef __DISPLAYPRIVATE_H
#define __DISPLAYPRIVATE_H

#include "PageManager.h"
#include "lvgl.h"
#include "exti.h"
typedef enum
{
    /*保留*/
    PAGE_NONE = 0,
    /*用户页面*/
    PAGE_MAIN,
    PAGE_MONITORING,
    PAGE_SETTING,
    PAGE_ABOUT,
    PAGE_GAME,
    /*保留*/
    PAGE_MAX
} Page_Order;

typedef enum
{
    EVENT_ButtonClick = 0,
} Event_Type;

extern PageManager page;

#define PageWaitUntil(condition) \
    while (!(condition))         \
    {                            \
        lv_task_handler();       \
    }

/*AppWindow*/
void AppWindow_Create();
lv_obj_t *AppWindow_GetScr(uint8_t pageID);
lv_coord_t AppWindow_GetHeight();
lv_coord_t AppWindow_GetWidth();
#define APP_WIN_HEIGHT AppWindow_GetHeight()
#define APP_WIN_WIDTH AppWindow_GetWidth()

void Display_Init();
void PageRegister_PAGE_MAIN(uint8_t pageID);
void PageRegister_PAGE_MONITORING(uint8_t pageID);
void PageRegister_PAGE_SETTING(uint8_t pageID);
void PageRegister_PAGE_ABOUT(uint8_t pageID);
void PageRegister_PAGE_GAME(uint8_t pageID);

/* PAGE_MAIN */
void Time_Update(void);
void Steps_Update(void);

/* PAGE_MONITORING */
void IMU_Update(void);
void BAT_Update(void);

/* PAGE_SETTING */
void Screen_awake(void);
extern int32_t slider_val;

/* PAGE_GAME */
#endif

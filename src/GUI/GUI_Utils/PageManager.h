/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PAGEMANAGER_H
#define __PAGEMANAGER_H

#include <stdint.h>

typedef void (*CallbackFunction_t)(void);
typedef void (*EventFunction_t)(void *, int);
typedef struct
{
  CallbackFunction_t SetupCallback;
  CallbackFunction_t LoopCallback;
  CallbackFunction_t ExitCallback;
  EventFunction_t EventCallback;
} PageList_TypeDef;

class PageManager
{
public:
  PageManager(uint8_t pageMax, uint8_t pageStackSize = 10);
  ~PageManager();
  uint8_t NowPage, LastPage, NextPage; // 就是Page_Order这个枚举里面的序号
  uint8_t NewPage, OldPage;

  bool PageRegister(
      uint8_t pageID,
      CallbackFunction_t setupCallback,
      CallbackFunction_t loopCallback,
      CallbackFunction_t exitCallback,
      EventFunction_t eventCallback);
  bool PageClear(uint8_t pageID);
  bool PagePush(uint8_t pageID);
  bool PagePop();
  void PageChangeTo(uint8_t pageID);
  void PageEventTransmit(void *obj, int event);
  void PageStackClear();
  void Running();

private:
  PageList_TypeDef *PageList;
  uint8_t *PageStack;
  uint8_t PageStackSize; // 页面栈的容器深度
  uint8_t PageStackTop;  // 当前页面栈中元素个数
  uint8_t MaxPage;
  bool IsPageBusy;
};

#endif

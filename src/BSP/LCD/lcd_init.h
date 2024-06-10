#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "tim.h"
	
#define USE_HORIZONTAL 0 // 设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏
#define Chip_Selection 1 // 设置芯片初始化 0为ILI9341  1为ST7789

#if USE_HORIZONTAL == 0 || USE_HORIZONTAL == 1
#define LCD_W 240
#define LCD_H 280

#else
#define LCD_W 280
#define LCD_H 240
#endif

//-----------------LCD端口定义----------------

#define LCD_RES_Clr() HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET) // RES
#define LCD_RES_Set() HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET)

#define LCD_DC_Clr() HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET) // DC
#define LCD_DC_Set() HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET)

#define LCD_CS_Clr() HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET) // CS
#define LCD_CS_Set() HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET)

#define LCD_BLK_Clr() __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, 0) // BLK
#define LCD_BLK_Set() __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3, 1000)

void LCD_Writ_Bus(uint8_t dat);                                           // 模拟SPI时序
void LCD_WR_DATA8(uint8_t dat);                                           // 写入一个字节
void LCD_WR_DATA(uint16_t dat);                                           // 写入两个字节
void LCD_WR_REG(uint8_t dat);                                             // 写入一个指令
void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2); // 设置坐标函数
void LCD_Init(void);                                                      // LCD初始化

HAL_StatusTypeDef SPI_to16(SPI_HandleTypeDef *hspi);
HAL_StatusTypeDef SPI_to8(SPI_HandleTypeDef *hspi);

typedef enum
{
  DMA_MEMINC_ENABLE = 0,
  DMA_MEMINC_DISABLE
} DMA_MEMINC_STATE;

#define LCD_SPI_TX_DMA DMA2_Stream3              // DMA2数据流2
#define LCD_SPI_TX_DMA_CHANNEL DMA_CHANNEL_3     // 通道2
#define LCD_SPI_TX_DMA_DIR DMA_MEMORY_TO_PERIPH  // 存储器到外设传输
#define LCD_SPI_TX_DMA_PINC DMA_PINC_DISABLE     // 外设地址不增量
#define LCD_SPI_TX_DMA_MINC DMA_MINC_DISABLE     // 存储器地址不增量
#define LCD_SPI_TX_DMA_PDA DMA_PDATAALIGN_BYTE   // 外设数据长度16位
#define LCD_SPI_TX_DMA_MDA DMA_MDATAALIGN_BYTE   // 存储器数据长度16位
#define LCD_SPI_TX_DMA_MODE DMA_NORMAL           // 普通传输模式
#define LCD_SPI_TX_DMA_PRIORITY DMA_PRIORITY_LOW // 优先级中等

#define LCD_SPI_TX_DMA_IRQn DMA2_Stream3_IRQn             // SPI发送DMAx数据流y中断
#define LCD_SPI_TX_DMA_IRQHandler DMA2_Stream3_IRQHandler // SPI发送DMAx数据流y中断服务函数
void LCD_DMA_Transfer16Bit(uint8_t *pData, uint16_t size, DMA_MEMINC_STATE state);
void LCD_DMA_Transfer8Bit(uint8_t *pData, uint16_t size, DMA_MEMINC_STATE state);

#ifdef __cplusplus
}
#endif

#endif

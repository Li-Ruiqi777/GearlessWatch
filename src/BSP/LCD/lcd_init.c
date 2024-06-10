#include "lcd_init.h"
#include "spi.h"
#include "main.h"

void LCD_Writ_Bus(uint8_t dat)
{
	LCD_CS_Clr();
	LCD_DMA_Transfer8Bit((uint8_t *)&dat, 1, DMA_MEMINC_DISABLE); // 启用DMA发送
	while (hspi1.State != HAL_SPI_STATE_READY);
	LCD_CS_Set();
}

/******************************************************************************
			函数说明：SPI向LCD写入8bit的数据
			入口数据：dat 写入的数据
			返回值：  无
******************************************************************************/
void LCD_WR_DATA8(uint8_t dat)
{
	LCD_Writ_Bus(dat);
}

/******************************************************************************
			函数说明：SPI向LCD写入16bit的数据
			入口数据：dat 写入的数据
			返回值：  无
******************************************************************************/
void LCD_WR_DATA(uint16_t dat)
{
	LCD_CS_Clr();
	LCD_DMA_Transfer16Bit((uint8_t *)&dat, 1, DMA_MEMINC_ENABLE); // 启用DMA发送
	LCD_CS_Set();
}

/******************************************************************************
			函数说明：LCD写入命令
			入口数据：dat 写入的命令
			返回值：  无
******************************************************************************/
void LCD_WR_REG(uint8_t dat)
{
	LCD_DC_Clr(); // 写命令
	LCD_Writ_Bus(dat);
	LCD_DC_Set(); // 写数据
}

/******************************************************************************
			函数说明：设置起始和结束地址
			入口数据：x1,x2 设置列的起始和结束地址
								y1,y2 设置行的起始和结束地址
			返回值：  无
******************************************************************************/
void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	LCD_WR_REG(0x2a); // 列地址设置
	LCD_WR_DATA(x1);
	LCD_WR_DATA(x2);
	LCD_WR_REG(0x2b); // 行地址设置
	LCD_WR_DATA(y1);
	LCD_WR_DATA(y2);
	LCD_WR_REG(0x2c); // 储存器写
}

/******************************************************************************
			函数说明：LCD初始化函数
			入口数据：无
			返回值：  无
******************************************************************************/
void LCD_Init(void)
{
	// 初始化ST7789
	LCD_RES_Clr(); // 复位
	HAL_Delay(100);
	LCD_RES_Set();
	HAL_Delay(100);
	LCD_BLK_Set(); // 打开背光
	HAL_Delay(500);
	LCD_CS_Clr(); // 片选

	LCD_WR_REG(0x11);
	HAL_Delay(100); // Delay 120ms

	LCD_WR_REG(0x36); // 扫描和刷新模式
	LCD_WR_DATA8(0x00);

	LCD_WR_REG(0x3A);		// 像素格式		65K;RGB   16位
	LCD_WR_DATA8(0x55); // 0x05

	LCD_WR_REG(0xB2); // porch设置  不知道是啥东西
	LCD_WR_DATA8(0x0B);
	LCD_WR_DATA8(0x0B);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x33);
	LCD_WR_DATA8(0x33);

	LCD_WR_REG(0xB7); // VGH和VGL不知道是啥
	LCD_WR_DATA8(0x11);

	LCD_WR_REG(0xBB); // VMOS不知道是啥
	LCD_WR_DATA8(0x2F);

	LCD_WR_REG(0xC0);
	LCD_WR_DATA8(0x2C);

	LCD_WR_REG(0xC2);
	LCD_WR_DATA8(0x01);

	LCD_WR_REG(0xC3);
	LCD_WR_DATA8(0x0D);

	LCD_WR_REG(0xC4);
	LCD_WR_DATA8(0x20); // VDV, 0x20:0v

	LCD_WR_REG(0xC6);
	LCD_WR_DATA8(0x0F); // 0x13:60Hz   好像是51Hz		0F才是60Hz

	LCD_WR_REG(0xD0); // Power control
	LCD_WR_DATA8(0xA4);
	LCD_WR_DATA8(0xA1);

	LCD_WR_REG(0xD6);		// 这个手册里没写
	LCD_WR_DATA8(0xA1); // sleep in后，gate输出为GND

	LCD_WR_REG(0xE0);
	LCD_WR_DATA8(0xF0);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x13);
	LCD_WR_DATA8(0x25);
	LCD_WR_DATA8(0x33);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x34);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x29);
	LCD_WR_DATA8(0x32);

	LCD_WR_REG(0xE1);
	LCD_WR_DATA8(0xF0);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x0A);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x25);
	LCD_WR_DATA8(0x32);
	LCD_WR_DATA8(0x3B);
	LCD_WR_DATA8(0x3B);
	LCD_WR_DATA8(0x17);
	LCD_WR_DATA8(0x18);
	LCD_WR_DATA8(0x2E);
	LCD_WR_DATA8(0x37);

	LCD_WR_REG(0xE4);
	LCD_WR_DATA8(0x1d); // 使用240根gate  (N+1)*8		25
	LCD_WR_DATA8(0x00); // 设定gate起点位置
	LCD_WR_DATA8(0x00); // 当gate没有用完时，bit4(TMG)设为0

	LCD_WR_REG(0x21); // 反转

	LCD_WR_REG(0x29); // 0X29是使屏幕显示		0x28是使屏幕消隐，但是背光单独控制

	LCD_WR_REG(0x2A); // Column Address Set
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x00); // 0
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0xEF); // 239

	LCD_WR_REG(0x2B); // Row Address Set
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x14); // 0X14		00
	LCD_WR_DATA8(0x01); // 0X01		01
	LCD_WR_DATA8(0x2b); // 0X2B		17

	LCD_WR_REG(0x2C);
}

HAL_StatusTypeDef SPI_to16(SPI_HandleTypeDef *hspi)
{
	hspi1.Instance->CR1 &= ~SPI_CR1_SPE; // 失能SPI
	hspi1.Instance->CR1 |= SPI_CR1_DFF;	 // 设置SPI16位传输模式
	hspi1.Instance->CR1 |= SPI_CR1_SPE;	 // 使能SPI
	return HAL_OK;
}

HAL_StatusTypeDef SPI_to8(SPI_HandleTypeDef *hspi)
{
	hspi1.Instance->CR1 &= ~SPI_CR1_SPE; // 失能SPI
	hspi1.Instance->CR1 &= ~SPI_CR1_DFF; // 设置SPI8位传输模式
	hspi1.Instance->CR1 |= SPI_CR1_SPE;	 // 使能SPI

	return HAL_OK;
}

// 启用SPI DMA连续发送单个16bit数据
void LCD_DMA_Transfer16Bit(uint8_t *pData, uint16_t size, DMA_MEMINC_STATE state)
{

	hspi1.Instance->CR1 &= ~SPI_CR1_SPE; // 失能SPI
	hspi1.Instance->CR1 |= SPI_CR1_DFF;	 // 设置SPI16位传输模式
	hspi1.Instance->CR1 |= SPI_CR1_SPE;	 // 使能SPI

	LCD_SPI_TX_DMA->CR &= ~(DMA_SxCR_MINC | DMA_SxCR_MSIZE | DMA_SxCR_PSIZE); // 清除 存储器地址增量、存储器外设数据长度 的设置

	LCD_SPI_TX_DMA->CR |= DMA_SxCR_MSIZE_0 | DMA_SxCR_PSIZE_0; // 存储器地址不自增、存储器和外设数据长度为半字(16bit)

	if (state == DMA_MEMINC_ENABLE)
		LCD_SPI_TX_DMA->CR |= DMA_SxCR_MINC; // 存储器地址不自增、存储器和外设数据长度为半字(16bit)

	while (hspi1.State != HAL_SPI_STATE_READY); // 等待SPI空闲

	__IO uint8_t sta = HAL_SPI_Transmit_DMA(&hspi1, pData, size); // 启用DMA传输
	while (hspi1.State != HAL_SPI_STATE_READY);
}

// 启用SPI DMA连续发送8bit数据
void LCD_DMA_Transfer8Bit(uint8_t *pData, uint16_t size, DMA_MEMINC_STATE state)
{
	hspi1.Instance->CR1 &= ~SPI_CR1_SPE;																			// 失能SPI
	hspi1.Instance->CR1 &= ~SPI_CR1_DFF;																			// 设置SPI8位传输模式
	hspi1.Instance->CR1 |= SPI_CR1_SPE;																				// 使能SPI
	LCD_SPI_TX_DMA->CR &= ~(DMA_SxCR_MINC | DMA_SxCR_MSIZE | DMA_SxCR_PSIZE); // 清除 存储器地址增量、存储器外设数据长度 的设置

	if (state == DMA_MEMINC_ENABLE)
		LCD_SPI_TX_DMA->CR |= DMA_SxCR_MINC; // 存储器地址自增、存储器和外设数据长度为字节(8bit)

	while (hspi1.State != HAL_SPI_STATE_READY); // 等待SPI空闲

	HAL_SPI_Transmit_DMA(&hspi1, pData, size); // 启用DMA传输
	while (hspi1.State != HAL_SPI_STATE_READY);
}
#include "lcd_init.h"
#include "spi.h"
#include "main.h"

void LCD_Writ_Bus(uint8_t dat)
{
	LCD_CS_Clr();
	LCD_DMA_Transfer8Bit((uint8_t *)&dat, 1, DMA_MEMINC_DISABLE); // ����DMA����
	while (hspi1.State != HAL_SPI_STATE_READY);
	LCD_CS_Set();
}

/******************************************************************************
			����˵����SPI��LCDд��8bit������
			������ݣ�dat д�������
			����ֵ��  ��
******************************************************************************/
void LCD_WR_DATA8(uint8_t dat)
{
	LCD_Writ_Bus(dat);
}

/******************************************************************************
			����˵����SPI��LCDд��16bit������
			������ݣ�dat д�������
			����ֵ��  ��
******************************************************************************/
void LCD_WR_DATA(uint16_t dat)
{
	LCD_CS_Clr();
	LCD_DMA_Transfer16Bit((uint8_t *)&dat, 1, DMA_MEMINC_ENABLE); // ����DMA����
	LCD_CS_Set();
}

/******************************************************************************
			����˵����LCDд������
			������ݣ�dat д�������
			����ֵ��  ��
******************************************************************************/
void LCD_WR_REG(uint8_t dat)
{
	LCD_DC_Clr(); // д����
	LCD_Writ_Bus(dat);
	LCD_DC_Set(); // д����
}

/******************************************************************************
			����˵����������ʼ�ͽ�����ַ
			������ݣ�x1,x2 �����е���ʼ�ͽ�����ַ
								y1,y2 �����е���ʼ�ͽ�����ַ
			����ֵ��  ��
******************************************************************************/
void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	LCD_WR_REG(0x2a); // �е�ַ����
	LCD_WR_DATA(x1);
	LCD_WR_DATA(x2);
	LCD_WR_REG(0x2b); // �е�ַ����
	LCD_WR_DATA(y1);
	LCD_WR_DATA(y2);
	LCD_WR_REG(0x2c); // ������д
}

/******************************************************************************
			����˵����LCD��ʼ������
			������ݣ���
			����ֵ��  ��
******************************************************************************/
void LCD_Init(void)
{
	// ��ʼ��ST7789
	LCD_RES_Clr(); // ��λ
	HAL_Delay(100);
	LCD_RES_Set();
	HAL_Delay(100);
	LCD_BLK_Set(); // �򿪱���
	HAL_Delay(500);
	LCD_CS_Clr(); // Ƭѡ

	LCD_WR_REG(0x11);
	HAL_Delay(100); // Delay 120ms

	LCD_WR_REG(0x36); // ɨ���ˢ��ģʽ
	LCD_WR_DATA8(0x00);

	LCD_WR_REG(0x3A);		// ���ظ�ʽ		65K;RGB   16λ
	LCD_WR_DATA8(0x55); // 0x05

	LCD_WR_REG(0xB2); // porch����  ��֪����ɶ����
	LCD_WR_DATA8(0x0B);
	LCD_WR_DATA8(0x0B);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x33);
	LCD_WR_DATA8(0x33);

	LCD_WR_REG(0xB7); // VGH��VGL��֪����ɶ
	LCD_WR_DATA8(0x11);

	LCD_WR_REG(0xBB); // VMOS��֪����ɶ
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
	LCD_WR_DATA8(0x0F); // 0x13:60Hz   ������51Hz		0F����60Hz

	LCD_WR_REG(0xD0); // Power control
	LCD_WR_DATA8(0xA4);
	LCD_WR_DATA8(0xA1);

	LCD_WR_REG(0xD6);		// ����ֲ���ûд
	LCD_WR_DATA8(0xA1); // sleep in��gate���ΪGND

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
	LCD_WR_DATA8(0x1d); // ʹ��240��gate  (N+1)*8		25
	LCD_WR_DATA8(0x00); // �趨gate���λ��
	LCD_WR_DATA8(0x00); // ��gateû������ʱ��bit4(TMG)��Ϊ0

	LCD_WR_REG(0x21); // ��ת

	LCD_WR_REG(0x29); // 0X29��ʹ��Ļ��ʾ		0x28��ʹ��Ļ���������Ǳ��ⵥ������

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
	hspi1.Instance->CR1 &= ~SPI_CR1_SPE; // ʧ��SPI
	hspi1.Instance->CR1 |= SPI_CR1_DFF;	 // ����SPI16λ����ģʽ
	hspi1.Instance->CR1 |= SPI_CR1_SPE;	 // ʹ��SPI
	return HAL_OK;
}

HAL_StatusTypeDef SPI_to8(SPI_HandleTypeDef *hspi)
{
	hspi1.Instance->CR1 &= ~SPI_CR1_SPE; // ʧ��SPI
	hspi1.Instance->CR1 &= ~SPI_CR1_DFF; // ����SPI8λ����ģʽ
	hspi1.Instance->CR1 |= SPI_CR1_SPE;	 // ʹ��SPI

	return HAL_OK;
}

// ����SPI DMA�������͵���16bit����
void LCD_DMA_Transfer16Bit(uint8_t *pData, uint16_t size, DMA_MEMINC_STATE state)
{

	hspi1.Instance->CR1 &= ~SPI_CR1_SPE; // ʧ��SPI
	hspi1.Instance->CR1 |= SPI_CR1_DFF;	 // ����SPI16λ����ģʽ
	hspi1.Instance->CR1 |= SPI_CR1_SPE;	 // ʹ��SPI

	LCD_SPI_TX_DMA->CR &= ~(DMA_SxCR_MINC | DMA_SxCR_MSIZE | DMA_SxCR_PSIZE); // ��� �洢����ַ�������洢���������ݳ��� ������

	LCD_SPI_TX_DMA->CR |= DMA_SxCR_MSIZE_0 | DMA_SxCR_PSIZE_0; // �洢����ַ���������洢�����������ݳ���Ϊ����(16bit)

	if (state == DMA_MEMINC_ENABLE)
		LCD_SPI_TX_DMA->CR |= DMA_SxCR_MINC; // �洢����ַ���������洢�����������ݳ���Ϊ����(16bit)

	while (hspi1.State != HAL_SPI_STATE_READY); // �ȴ�SPI����

	__IO uint8_t sta = HAL_SPI_Transmit_DMA(&hspi1, pData, size); // ����DMA����
	while (hspi1.State != HAL_SPI_STATE_READY);
}

// ����SPI DMA��������8bit����
void LCD_DMA_Transfer8Bit(uint8_t *pData, uint16_t size, DMA_MEMINC_STATE state)
{
	hspi1.Instance->CR1 &= ~SPI_CR1_SPE;																			// ʧ��SPI
	hspi1.Instance->CR1 &= ~SPI_CR1_DFF;																			// ����SPI8λ����ģʽ
	hspi1.Instance->CR1 |= SPI_CR1_SPE;																				// ʹ��SPI
	LCD_SPI_TX_DMA->CR &= ~(DMA_SxCR_MINC | DMA_SxCR_MSIZE | DMA_SxCR_PSIZE); // ��� �洢����ַ�������洢���������ݳ��� ������

	if (state == DMA_MEMINC_ENABLE)
		LCD_SPI_TX_DMA->CR |= DMA_SxCR_MINC; // �洢����ַ�������洢�����������ݳ���Ϊ�ֽ�(8bit)

	while (hspi1.State != HAL_SPI_STATE_READY); // �ȴ�SPI����

	HAL_SPI_Transmit_DMA(&hspi1, pData, size); // ����DMA����
	while (hspi1.State != HAL_SPI_STATE_READY);
}
#ifndef __W25Q64__H
#define __W25Q64__H

#ifdef __cplusplus
extern "C" {
#endif
	
#include "main.h"

#define W25Q64_FLASH_SIZE 0x7FFFFF /* 64 Mbits => 8M B */
#define W25Q64_BLOCK_SIZE 0xFFFF   /* 16 sectors of 64K Bytes */
#define W25Q64_SECTOR_SIZE 0x1000  /* 2048 sectors of 4k Bytes */
#define W25Q64_PAGE_SIZE 0x100     /* ==256 (Bytes) */

#define W25Q64_DUMMY_CYCLES_READ 4
#define W25Q64_DUMMY_CYCLES_READ_QUAD 10

#define W25Q64_BULK_ERASE_MAX_TIME 400000
#define W25Q64_BLOCK_ERASE_MAX_TIME 5000
#define W25Q64_SECTOR_ERASE_MAX_TIME 2000
#define W25Q64_TIMEOUT_VALUE 1000

/**
 * @brief  W25Q64 Commands
 */
/* Reset Operations */
#define RESET_ENABLE_CMD 0x66
#define RESET_DEVICE_CMD 0x99

#define ENTER_QPI_MODE_CMD 0x38
#define EXIT_QPI_MODE_CMD 0xFF

/* Identification Operations */
#define READ_ID_CMD 0x90
#define DUAL_READ_ID_CMD 0x92
#define QUAD_READ_ID_CMD 0x94
#define READ_JEDEC_ID_CMD 0x9F

/* Read Operations */
#define READ_CMD 0x03
#define FAST_READ_CMD 0x0B
#define DUAL_OUT_FAST_READ_CMD 0x3B
#define DUAL_INOUT_FAST_READ_CMD 0xBB
#define QUAD_OUT_FAST_READ_CMD 0x6B
#define QUAD_INOUT_FAST_READ_CMD 0xEB

/* Write Operations */
#define WRITE_ENABLE_CMD 0x06
#define WRITE_DISABLE_CMD 0x04

/* Register Operations */
#define READ_STATUS_REG1_CMD 0x05
#define READ_STATUS_REG2_CMD 0x35
#define READ_STATUS_REG3_CMD 0x15

#define WRITE_STATUS_REG1_CMD 0x01
#define WRITE_STATUS_REG2_CMD 0x31
#define WRITE_STATUS_REG3_CMD 0x11

/* Program Operations */
#define PAGE_PROG_CMD 0x02
#define QUAD_INPUT_PAGE_PROG_CMD 0x32

/* Erase Operations */
#define SECTOR_ERASE_CMD 0x20
#define BLOCK32_ERASE_CMD 0x52
#define BLOCK64_ERASE_CMD 0xD8
#define CHIP_ERASE_CMD 0xC7

#define PROG_ERASE_RESUME_CMD 0x7A
#define PROG_ERASE_SUSPEND_CMD 0x75

/* Flag Status Register */
#define W25Q64_FSR_BUSY ((uint8_t)0x01) /*!< busy */
#define W25Q64_FSR_WREN ((uint8_t)0x02) /*!< write enable */
#define W25Q64_FSR_QE ((uint8_t)0x02)   /*!< quad enable */

#define W25Q64_Enable() HAL_GPIO_WritePin(Flash_CS_GPIO_Port, Flash_CS_Pin, GPIO_PIN_RESET)
#define W25Q64_Disable() HAL_GPIO_WritePin(Flash_CS_GPIO_Port, Flash_CS_Pin, GPIO_PIN_SET)

#define W25Q64_OK ((uint8_t)0x00)
#define W25Q64_ERROR ((uint8_t)0x01)
#define W25Q64_BUSY ((uint8_t)0x02)
#define W25Q64_TIMEOUT ((uint8_t)0x03)

uint8_t W25Q64_Init(void);
uint8_t W25Q64_Reset(void);
uint8_t W25Q64_GetStatus(void);
uint8_t W25Q64_WriteEnable(void);
HAL_StatusTypeDef W25Q64_Read_ID(uint8_t *ID);
uint8_t W25Q64_Read_Bytes(uint8_t *pData, uint32_t ReadAddr, uint32_t Size);
uint8_t W25Q64_Write_Bytes(uint8_t *pData, uint32_t WriteAddr, uint32_t Size);
uint8_t W25Q64_Erase_Sector(uint32_t Address);
uint8_t W25Q64_Erase_Block(uint32_t Address);
uint8_t W25Q64_Erase_Chip(void);
#ifdef __cplusplus
}
#endif
#endif

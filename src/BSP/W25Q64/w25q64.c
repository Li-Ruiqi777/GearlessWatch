#include "w25q64.h"
#include "spi.h"

/**
 * @brief  Initializes the W25Q64 interface.
 * @retval None
 */
uint8_t W25Q64_Init(void)
{
  /* Reset W25Q64xx */
  __IO uint8_t res = 0;
  res = W25Q64_Reset();
  if (res != W25Q64_OK)
    return 1;
  return 0;
}

/**
 * @brief  This function reset the W25Q64.
 * @retval None
 */
uint8_t W25Q64_Reset(void)
{
  uint32_t tickstart = HAL_GetTick();
  uint8_t cmd[2] = {RESET_ENABLE_CMD, RESET_DEVICE_CMD};

  W25Q64_Enable();
  /* Send the reset command */
  HAL_SPI_Transmit(&hspi2, cmd, 2, W25Q64_TIMEOUT_VALUE);
  W25Q64_Disable();

  /* Wait the end of Flash writing */
  while (W25Q64_GetStatus() == W25Q64_BUSY)
  {
    /* Check for the Timeout */
    if ((HAL_GetTick() - tickstart) > W25Q64_TIMEOUT_VALUE)
    {
      return W25Q64_TIMEOUT;
    }
  }

  return W25Q64_OK;
}

/**
 * @brief  Reads current status of the W25Q64.
 * @retval W25Q64 memory status
 */
uint8_t W25Q64_GetStatus(void)
{
  uint8_t cmd[] = {READ_STATUS_REG1_CMD};
  uint8_t status;
  __IO uint8_t res = 0;
  W25Q64_Enable();
  /* Send the read status command */
  res = HAL_SPI_Transmit(&hspi2, cmd, 1, W25Q64_TIMEOUT_VALUE);
  /* Reception of the data */
  res = HAL_SPI_Receive(&hspi2, &status, 1, W25Q64_TIMEOUT_VALUE);
  W25Q64_Disable();

  /* Check the value of the register */
  if ((status & W25Q64_FSR_BUSY) != 0)
  {
    return W25Q64_BUSY;
  }
  else
  {
    return W25Q64_OK;
  }
}

/*-----------------------------------读数据-----------------------------------------*/
/**
 * @brief  Read Manufacture/Device ID.
 * @param  return value address
 * @retval None
 */
HAL_StatusTypeDef W25Q64_Read_ID(uint8_t *ID)
{
  uint8_t add[2];
  uint8_t cmd[4] = {READ_ID_CMD, 0x00, 0x00, 0x00};
  HAL_StatusTypeDef sta = HAL_OK;

  W25Q64_Enable();

  do
  {
    /* Send the read ID command */
    sta = HAL_SPI_Transmit(&hspi2, cmd, 4, W25Q64_TIMEOUT_VALUE);
    if (HAL_OK != sta)
    {
      break;
    }
    /* Reception of the data */
    sta = HAL_SPI_Receive(&hspi2, ID, 2, W25Q64_TIMEOUT_VALUE);
    if (HAL_OK != sta)
    {
      break;
    }
  } while (0);

  W25Q64_Disable();

  return sta;
}

/**
 * @brief  Reads an amount of bytes from the QSPI memory in normal speed
 * @param  pData: Pointer to data to be read
 * @param  ReadAddr: Read start address
 * @param  Size: Size of data to read
 * @retval QSPI memory status
 */
uint8_t W25Q64_Read_Bytes(uint8_t *pData, uint32_t ReadAddr, uint32_t Size)
{
  uint8_t cmd[4];

  /* Check parameter */
  if (ReadAddr >= W25Q64_FLASH_SIZE || (ReadAddr + Size) >= W25Q64_FLASH_SIZE)
    return W25Q64_ERROR;

  /* Configure the command */
  cmd[0] = READ_CMD; /*FAST_READ_CMD*/
  cmd[1] = (uint8_t)(ReadAddr >> 16);
  cmd[2] = (uint8_t)(ReadAddr >> 8);
  cmd[3] = (uint8_t)(ReadAddr);

  W25Q64_Enable();
  /* Send the read ID command */
  HAL_SPI_Transmit_DMA(&hspi2, cmd, 4);

  /* Reception of the data */
  if (HAL_SPI_Receive_IT(&hspi2, pData, Size) != HAL_OK)
  {
    W25Q64_Disable();
    return W25Q64_ERROR;
  }

  W25Q64_Disable();
  return W25Q64_OK;
}

/*-----------------------------------写数据-----------------------------------------*/

/**
 * @brief  This function send a Write Enable and wait it is effective.
 * @retval None
 */
uint8_t W25Q64_WriteEnable(void)
{
  uint8_t cmd[] = {WRITE_ENABLE_CMD};
  uint32_t tickstart = HAL_GetTick();

  /*Select the FLASH: Chip Select low */
  W25Q64_Enable();
  /* Send the read ID command */
  HAL_SPI_Transmit(&hspi2, cmd, 1, W25Q64_TIMEOUT_VALUE);
  /*Deselect the FLASH: Chip Select high */
  W25Q64_Disable();

  /* Wait the end of Flash writing */
  while (W25Q64_GetStatus() == W25Q64_BUSY)
  {
    /* Check for the Timeout */
    if ((HAL_GetTick() - tickstart) > W25Q64_TIMEOUT_VALUE)
    {
      return W25Q64_TIMEOUT;
    }
  }

  return W25Q64_OK;
}

/**
 * @brief  写若干字节到FLASH,写之前要确保该页为0XFF,不是的话要先擦除(该函数不包含)
 * @param  pData: Pointer to data to be written
 * @param  WriteAddr: Write start address
 * @param  Size: Size of data to write,No more than 64byte.
 * @retval QSPI memory status
 */
uint8_t W25Q64_Write_Bytes(uint8_t *pData, uint32_t WriteAddr, uint32_t Size)
{
  uint8_t cmd[4];
  uint32_t end_addr, current_size, current_addr;
  uint32_t tickstart = HAL_GetTick();

  /* Check parameter */
  if (WriteAddr >= W25Q64_FLASH_SIZE || (WriteAddr + Size) >= W25Q64_FLASH_SIZE)
    return W25Q64_ERROR;

  /* Calculation of the size between the write address and the end of the page */
  current_addr = 0;

  while (current_addr <= WriteAddr)
  {
    current_addr += W25Q64_PAGE_SIZE;
  }

  current_size = current_addr - WriteAddr; // 该页剩余的字节数

  /* Check if the size of the data is less than the remaining place in the page */
  if (current_size > Size)
  {
    current_size = Size;
  }

  /* Initialize the adress variables */
  current_addr = WriteAddr;
  end_addr = WriteAddr + Size;

  /* Perform the write page by page */
  do
  {
    /* Configure the command */
    cmd[0] = PAGE_PROG_CMD;
    cmd[1] = (uint8_t)(current_addr >> 16);
    cmd[2] = (uint8_t)(current_addr >> 8);
    cmd[3] = (uint8_t)(current_addr);

    /* Enable write operations */
    W25Q64_WriteEnable();

    W25Q64_Enable();

    /* Send the command */
    if (HAL_SPI_Transmit(&hspi2, cmd, 4, W25Q64_TIMEOUT_VALUE) != HAL_OK)
    {
      return W25Q64_ERROR;
    }

    /* Transmission of the data */
    if (HAL_SPI_Transmit(&hspi2, pData, current_size, W25Q64_TIMEOUT_VALUE) != HAL_OK)
    {
      return W25Q64_ERROR;
    }

    W25Q64_Disable();

    /* Wait the end of Flash writing */
    while (W25Q64_GetStatus() == W25Q64_BUSY)
    {
      /* Check for the Timeout */
      if ((HAL_GetTick() - tickstart) > W25Q64_TIMEOUT_VALUE)
      {
        return W25Q64_TIMEOUT;
      }
    }

    /* Update the address and size variables for next page programming */
    current_addr += current_size;
    pData += current_size;
    current_size = ((current_addr + W25Q64_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : W25Q64_PAGE_SIZE;
  } while (current_addr < end_addr);

  return W25Q64_OK;
}

/*-----------------------------------擦除-----------------------------------------*/
/**
 * @brief 擦除一个扇区的内存(65ms左右)
 * @param Address
 * @return
 */
uint8_t W25Q64_Erase_Sector(uint32_t Address)
{
  uint8_t cmd[4];
  uint32_t tickstart = HAL_GetTick();

  /* Check parameter */
  if (Address >= W25Q64_FLASH_SIZE)
    return W25Q64_ERROR;

  cmd[0] = SECTOR_ERASE_CMD;
  cmd[1] = (uint8_t)(Address >> 16);
  cmd[2] = (uint8_t)(Address >> 8);
  cmd[3] = (uint8_t)(Address);

  /* Enable write operations */
  W25Q64_WriteEnable();

  /*Select the FLASH: Chip Select low */
  W25Q64_Enable();
  /* Send the read ID command */
  HAL_SPI_Transmit(&hspi2, cmd, 4, W25Q64_TIMEOUT_VALUE);
  /*Deselect the FLASH: Chip Select high */
  W25Q64_Disable();

  /* Wait the end of Flash writing */
  while (W25Q64_GetStatus() == W25Q64_BUSY)
  {
    /* Check for the Timeout */
    if ((HAL_GetTick() - tickstart) > W25Q64_SECTOR_ERASE_MAX_TIME)
    {
      return W25Q64_TIMEOUT;
    }
  }
  return W25Q64_OK;
}

/**
 * @brief  Erases the specified block of the QSPI memory.
 * @param  BlockAddress: Block address to erase
 * @retval QSPI memory status
 */
uint8_t W25Q64_Erase_Block(uint32_t Address)
{
  uint8_t cmd[4];
  uint32_t tickstart = HAL_GetTick();

  if (Address >= W25Q64_FLASH_SIZE)
    return W25Q64_ERROR;

  cmd[0] = BLOCK64_ERASE_CMD;
  cmd[1] = (uint8_t)(Address >> 16);
  cmd[2] = (uint8_t)(Address >> 8);
  cmd[3] = (uint8_t)(Address);

  /* Enable write operations */
  W25Q64_WriteEnable();

  /*Select the FLASH: Chip Select low */
  W25Q64_Enable();
  /* Send the read ID command */
  HAL_SPI_Transmit(&hspi2, cmd, 4, W25Q64_TIMEOUT_VALUE);
  /*Deselect the FLASH: Chip Select high */
  W25Q64_Disable();

  /* Wait the end of Flash writing */
  while (W25Q64_GetStatus() == W25Q64_BUSY)
  {
    /* Check for the Timeout */
    if ((HAL_GetTick() - tickstart) > W25Q64_BLOCK_ERASE_MAX_TIME)
    {
      return W25Q64_TIMEOUT;
    }
  }
  return W25Q64_OK;
}

/**
 * @brief  Erases the entire QSPI memory.This function will take a very long time(21s左右).
 * @retval QSPI memory status
 */
uint8_t W25Q64_Erase_Chip(void)
{
  uint8_t cmd[1];
  uint32_t tickstart = HAL_GetTick();
  cmd[0] = CHIP_ERASE_CMD;

  /* Enable write operations */
  W25Q64_WriteEnable();

  /*Select the FLASH: Chip Select low */
  W25Q64_Enable();
  /* Send the read ID command */
  HAL_SPI_Transmit(&hspi2, cmd, 1, W25Q64_TIMEOUT_VALUE);
  /*Deselect the FLASH: Chip Select high */
  W25Q64_Disable();

  /* Wait the end of Flash writing */
  while (W25Q64_GetStatus() != W25Q64_BUSY)
  {
    /* Check for the Timeout */
    if ((HAL_GetTick() - tickstart) > W25Q64_BULK_ERASE_MAX_TIME)
    {
      return W25Q64_TIMEOUT;
    }
  }
  return W25Q64_OK;
}

/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    rtc.c
 * @brief   This file provides code for the configuration
 *          of the RTC instances.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "rtc.h"

/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only
   */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
  if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != RTC_unique_ID) // 判断是否备份区里面有时间写入
  {
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, (uint16_t)RTC_unique_ID);
    RTC_SET(&GetTime, &GetDate);
  }
  return;
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
   */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_FRIDAY;
  sDate.Month = RTC_MONTH_DECEMBER;
  sDate.Date = 0x22;
  sDate.Year = 0x52;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */
}

void HAL_RTC_MspInit(RTC_HandleTypeDef *rtcHandle)
{

  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if (rtcHandle->Instance == RTC)
  {
    /* USER CODE BEGIN RTC_MspInit 0 */

    /* USER CODE END RTC_MspInit 0 */
    /** Initializes the peripherals clock
     */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();
    /* USER CODE BEGIN RTC_MspInit 1 */

    /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef *rtcHandle)
{

  if (rtcHandle->Instance == RTC)
  {
    /* USER CODE BEGIN RTC_MspDeInit 0 */

    /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();
    /* USER CODE BEGIN RTC_MspDeInit 1 */

    /* USER CODE END RTC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/**
 * @brief RTC设置时间日期,并存入BKP中
 * @param time_handle
 * @param date_handle
 */
void RTC_SET(RTC_TimeTypeDef *time_handle, RTC_DateTypeDef *date_handle)
{
  RTC_DateTypeDef RTC_DateStruct;
  RTC_TimeTypeDef RTC_TimeStruct;

  RTC_DateStruct.Year = date_handle->Year;
  RTC_DateStruct.Month = date_handle->Month;
  RTC_DateStruct.Date = date_handle->Date;
  RTC_DateStruct.WeekDay = date_handle->WeekDay;
  if (HAL_RTC_SetDate(&hrtc, &RTC_DateStruct, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  RTC_TimeStruct.Hours = time_handle->Hours;
  RTC_TimeStruct.Minutes = time_handle->Minutes;
  RTC_TimeStruct.Seconds = time_handle->Seconds;
  if (HAL_RTC_SetTime(&hrtc, &RTC_TimeStruct, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
 * @brief RTC读取时间日期,并存入BKP中
 * @param time_handle
 * @param date_handle
 */
void RTC_READ(RTC_TimeTypeDef *time_handle, RTC_DateTypeDef *date_handle)
{
  RTC_DateTypeDef RTC_DateStruct;
  RTC_TimeTypeDef RTC_TimeStruct;

  HAL_RTC_GetTime(&hrtc, &RTC_TimeStruct, RTC_FORMAT_BIN);
  time_handle->Hours = RTC_TimeStruct.Hours;
  time_handle->Minutes = RTC_TimeStruct.Minutes;
  time_handle->Seconds = RTC_TimeStruct.Seconds;

  HAL_RTC_GetDate(&hrtc, &RTC_DateStruct, RTC_FORMAT_BIN);
  date_handle->Year = RTC_DateStruct.Year;
  date_handle->Month = RTC_DateStruct.Month;
  date_handle->Date = RTC_DateStruct.Date;
}

/* USER CODE END 1 */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usb.c
  * @brief   This file provides code for the configuration
  *          of the USB instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "usb.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

PCD_HandleTypeDef hpcd_USB_FS;

/* USB init function */

void MX_USB_PCD_Init(void)
{

  /* USER CODE BEGIN USB_Init 0 */

  /* USER CODE END USB_Init 0 */

  /* USER CODE BEGIN USB_Init 1 */

  /* USER CODE END USB_Init 1 */
  hpcd_USB_FS.Instance = USB;
  hpcd_USB_FS.Init.dev_endpoints = 8;
  hpcd_USB_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_FS.Init.Sof_enable = ENABLE;
  hpcd_USB_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_FS.Init.battery_charging_enable = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_Init 2 */

  /* USER CODE END USB_Init 2 */

}

void HAL_PCD_MspInit(PCD_HandleTypeDef* pcdHandle)
{

  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(pcdHandle->Instance==USB)
  {
  /* USER CODE BEGIN USB_MspInit 0 */

  /* USER CODE END USB_MspInit 0 */

  /** Initializes the peripherals clocks
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
    PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* USB clock enable */
    __HAL_RCC_USB_CLK_ENABLE();

    /* USB interrupt Init */
    HAL_NVIC_SetPriority(USB_HP_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USB_HP_IRQn);
    HAL_NVIC_SetPriority(USB_LP_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(USB_LP_IRQn);
  /* USER CODE BEGIN USB_MspInit 1 */

  /* USER CODE END USB_MspInit 1 */
  }
}

void HAL_PCD_MspDeInit(PCD_HandleTypeDef* pcdHandle)
{

  if(pcdHandle->Instance==USB)
  {
  /* USER CODE BEGIN USB_MspDeInit 0 */

  /* USER CODE END USB_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USB_CLK_DISABLE();

    /* USB interrupt Deinit */
    HAL_NVIC_DisableIRQ(USB_HP_IRQn);
    HAL_NVIC_DisableIRQ(USB_LP_IRQn);
  /* USER CODE BEGIN USB_MspDeInit 1 */

  /* USER CODE END USB_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

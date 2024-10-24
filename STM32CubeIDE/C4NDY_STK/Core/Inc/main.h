/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "tusb.h"  // IWYU pragma: keep

#include "SEGGER_RTT.h"  // IWYU pragma: keep
    /* USER CODE END Includes */

    /* Exported types ------------------------------------------------------------*/
    /* USER CODE BEGIN ET */

    /* USER CODE END ET */

    /* Exported constants --------------------------------------------------------*/
    /* USER CODE BEGIN EC */

    /* USER CODE END EC */

    /* Exported macro ------------------------------------------------------------*/
    /* USER CODE BEGIN EM */

    /* USER CODE END EM */

    /* Exported functions prototypes ---------------------------------------------*/
    void Error_Handler(void);

    /* USER CODE BEGIN EFP */
    void erase_flash_data(void);
    void write_flash_data(const uint16_t index, const uint8_t val);
    uint64_t read_flash_data(const uint16_t index);
    void setBootDfuFlag(bool is_boot_dfu);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define HC164_CLK_Pin       GPIO_PIN_13
#define HC164_CLK_GPIO_Port GPIOC
#define HC164_A_Pin         GPIO_PIN_14
#define HC164_A_GPIO_Port   GPIOC
#define HC165_CLK_Pin       GPIO_PIN_15
#define HC165_CLK_GPIO_Port GPIOC
#define USER_SW_Pin         GPIO_PIN_10
#define USER_SW_GPIO_Port   GPIOB
#define USER_LED_Pin        GPIO_PIN_12
#define USER_LED_GPIO_Port  GPIOB
#define LP_LED_Pin          GPIO_PIN_13
#define LP_LED_GPIO_Port    GPIOB
#define HC165_SL_Pin        GPIO_PIN_7
#define HC165_SL_GPIO_Port  GPIOB
#define HC165_QH_Pin        GPIO_PIN_9
#define HC165_QH_GPIO_Port  GPIOB

    /* USER CODE BEGIN Private defines */

    /* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

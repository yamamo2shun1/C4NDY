/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "quadspi.h"
#include "sai.h"
#include "tim.h"
#include "usb.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include "usb_descriptors.h"

#include "keyboard.h"
#include "audio_control.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define FLASH_DATA_ADDR 0x0807F000

#define AUDIO_SAMPLE_RATE 48000

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//--------------------------------------------------------------------+
// Application Callback API Implementations
//--------------------------------------------------------------------+
// Invoked when device is mounted
void tud_mount_cb(void)
{
    // blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
    // blink_interval_ms = BLINK_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
    (void) remote_wakeup_en;
    // blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
    // blink_interval_ms = BLINK_MOUNTED;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    if (htim == &htim6)
    {
    }
}

void erase_flash_data(void)
{
    static FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PAGEError;
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Page      = 254;
    EraseInitStruct.NbPages   = 2;

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
    {
        SEGGER_RTT_printf(0, "flash erase error...\n");
    }
}

void write_flash_data(uint16_t index, uint8_t val)
{
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FLASH_DATA_ADDR + 8 * index, val) != HAL_OK)
    {
        SEGGER_RTT_printf(0, "flash program error...\n");
    }
}

uint64_t read_flash_data(uint16_t index)
{
    return *(uint64_t*) (FLASH_DATA_ADDR + 8 * index);
}

void setBootDfuFlag(bool is_boot_dfu)
{
    SEGGER_RTT_printf(0, "erase & write FLASH...\n");
    HAL_FLASH_Unlock();

    uint64_t currentKeyMap[2][MATRIX_ROWS][MATRIX_COLUMNS] = {0x0};

    SEGGER_RTT_printf(0, "current KeyMap\n");
    for (int k = 0; k < 2; k++)
    {
        SEGGER_RTT_printf(0, "Layout:%d\n", k + 1);
        SEGGER_RTT_printf(0, "[\n");
        for (int i = 0; i < MATRIX_ROWS; i++)
        {
            SEGGER_RTT_printf(0, "[");
            for (int j = 0; j < MATRIX_COLUMNS; j++)
            {
                currentKeyMap[k][i][j] = read_flash_data(BASIC_PARAMS_NUM + k * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j);
                SEGGER_RTT_printf(0, "%02X ", currentKeyMap[k][i][j]);
            }
            SEGGER_RTT_printf(0, "]\n");
        }
        SEGGER_RTT_printf(0, "]\n\n");
    }

    erase_flash_data();

    if (is_boot_dfu)
    {
        write_flash_data(0, 1);
    }
    else
    {
        write_flash_data(0, 0);
    }
    write_flash_data(1, getLinePhonoSW());
    write_flash_data(2, getKeymapID());

    SEGGER_RTT_printf(0, "reload KeyMap\n");
    for (int k = 0; k < 2; k++)
    {
        SEGGER_RTT_printf(0, "Layout:%d\n", k + 1);
        SEGGER_RTT_printf(0, "[\n");
        for (int i = 0; i < MATRIX_ROWS; i++)
        {
            SEGGER_RTT_printf(0, "[");
            for (int j = 0; j < MATRIX_COLUMNS; j++)
            {
                write_flash_data(BASIC_PARAMS_NUM + k * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, currentKeyMap[k][i][j]);
                SEGGER_RTT_printf(0, "%02X ", currentKeyMap[k][i][j]);
            }
            SEGGER_RTT_printf(0, "]\n");
        }
        SEGGER_RTT_printf(0, "]\n\n");
    }

    HAL_FLASH_Lock();
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_SAI1_Init();
    MX_QUADSPI1_Init();
    MX_I2C2_Init();
    MX_ADC1_Init();
    MX_USB_PCD_Init();
    MX_TIM6_Init();
    /* USER CODE BEGIN 2 */
    HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_SET);

    SEGGER_RTT_printf(0, "Unique Device ID = %X-%X-%X\n", HAL_GetUIDw0(), HAL_GetUIDw1(), HAL_GetUIDw2());

    SEGGER_RTT_printf(0, "initialize ADAU1761...\n");
    default_download_IC_1();

    SEGGER_RTT_printf(0, "initialize tinyUSB...\n");
    tusb_init();

    SEGGER_RTT_printf(0, "start ADC DMA...\n");
    start_adc();

    SEGGER_RTT_printf(0, "start SAI DMA...\n");
    start_sai();

    // HAL_TIM_Base_Start_IT(&htim6);

    SEGGER_RTT_printf(0, "user_sw = %d\n", HAL_GPIO_ReadPin(USER_SW_GPIO_Port, USER_SW_Pin));
    if (!HAL_GPIO_ReadPin(USER_SW_GPIO_Port, USER_SW_Pin))
    {
        SEGGER_RTT_printf(0, "init flash data");

        factoryReset();
    }

    loadKeyboardSettingsFromFlash();
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        tud_task();

        read_audio_data_from_usb(N_SAMPLES_PER_1_RX * CFG_TUD_AUDIO_FUNC_1_N_BYTES_PER_SAMPLE_RX * CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX);

        codec_control_task();

        hid_keyscan_task();

        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48 | RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_BYPASS;
    RCC_OscInitStruct.HSI48State     = RCC_HSI48_ON;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM       = RCC_PLLM_DIV1;
    RCC_OscInitStruct.PLL.PLLN       = 24;
    RCC_OscInitStruct.PLL.PLLP       = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ       = RCC_PLLQ_DIV6;
    RCC_OscInitStruct.PLL.PLLR       = RCC_PLLR_DIV2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

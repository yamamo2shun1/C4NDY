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
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_audio_if.h"
#include "SEGGER_RTT.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define READ_CMD 0x03
#define QIO_READ_CMD 0xEB
#define WRITE_CMD 0x02
#define QIO_WRITE_CMD 0x38
#define ENTER_QPI_CMD 0x35
#define EXIT_QPI_CMD 0xF5
#define RESET_ENABLE_CMD 0x66
#define RESET_CMD 0x99
#define SET_BURST_LEN_CMD 0xC0
#define READ_ID_CMD 0x9F
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
QSPI_HandleTypeDef hqspi1;

SAI_HandleTypeDef hsai_BlockB1;
DMA_HandleTypeDef hdma_sai1_b;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SAI1_Init(void);
static void MX_QUADSPI1_Init(void);
/* USER CODE BEGIN PFP */
void RTT_Read(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int8_t SRAM_Write(uint32_t address, uint8_t *write_data, uint32_t write_length)
{
	QSPI_CommandTypeDef sCommand;
	HAL_StatusTypeDef res = 0;

	sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	sCommand.Instruction	   = WRITE_CMD;
	sCommand.AddressMode	   = QSPI_ADDRESS_1_LINE;
	sCommand.Address		   = address;
	sCommand.AddressSize	   = QSPI_ADDRESS_24_BITS;
	sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCommand.DataMode		   = QSPI_DATA_1_LINE;
	sCommand.DummyCycles	   = 0;
	sCommand.DdrMode		   = QSPI_DDR_MODE_DISABLE;
	sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	sCommand.SIOOMode		   = QSPI_SIOO_INST_EVERY_CMD;
	sCommand.NbData			   = write_length;

	res += HAL_QSPI_Command(&hqspi1, &sCommand, 0xFFF);
	res += HAL_QSPI_Transmit(&hqspi1, write_data, 0xFFF);

	return res;
}

int8_t SRAM_Read(uint32_t address, uint8_t *read_data, uint32_t read_length)
{
	QSPI_CommandTypeDef sCommand;
	HAL_StatusTypeDef res = 0;

	sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	sCommand.Instruction	   = READ_CMD;
	sCommand.AddressMode	   = QSPI_ADDRESS_1_LINE;
	sCommand.Address		   = address;
	sCommand.AddressSize	   = QSPI_ADDRESS_24_BITS;
	sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCommand.DataMode		   = QSPI_DATA_1_LINE;
	sCommand.DummyCycles	   = 0;
	sCommand.DdrMode		   = QSPI_DDR_MODE_DISABLE;
	sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	sCommand.SIOOMode		   = QSPI_SIOO_INST_EVERY_CMD;
	sCommand.NbData			   = read_length;

	res += HAL_QSPI_Command(&hqspi1, &sCommand, 0xFFF);
	res += HAL_QSPI_Receive(&hqspi1, read_data, 0xFFF);

	return res;
}

int8_t SRAM_QMode(void)
{

	QSPI_CommandTypeDef sCommand;
	HAL_StatusTypeDef res=0;

	sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	sCommand.Instruction	   = ENTER_QPI_CMD;
	sCommand.AddressMode	   = QSPI_ADDRESS_NONE;
	sCommand.Address		   = 0;
	sCommand.AddressSize	   = QSPI_ADDRESS_24_BITS;
	sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCommand.DataMode		   = QSPI_DATA_NONE;
	sCommand.DummyCycles	   = 0;
	sCommand.DdrMode		   = QSPI_DDR_MODE_DISABLE;
	sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	sCommand.SIOOMode		   = QSPI_SIOO_INST_EVERY_CMD;
	sCommand.NbData			   = 0;

	res=HAL_QSPI_Command(&hqspi1, &sCommand, 0xFFF);

	return res;
}

int8_t SRAM_QWrite(uint32_t address, uint8_t *write_data, uint32_t write_length)
{
	QSPI_CommandTypeDef sCommand;
	HAL_StatusTypeDef res = 0;

	sCommand.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	sCommand.Instruction	   = QIO_WRITE_CMD;
	sCommand.AddressMode	   = QSPI_ADDRESS_4_LINES;
	sCommand.Address		   = address;
	sCommand.AddressSize	   = QSPI_ADDRESS_24_BITS;
	sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCommand.DataMode		   = QSPI_DATA_4_LINES;
	sCommand.DummyCycles	   = 0;
	sCommand.DdrMode		   = QSPI_DDR_MODE_DISABLE;
	sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	sCommand.SIOOMode		   = QSPI_SIOO_INST_EVERY_CMD;
	sCommand.NbData			   = write_length;

	res += HAL_QSPI_Command(&hqspi1, &sCommand, 0xFFF);
	res += HAL_QSPI_Transmit(&hqspi1, write_data, 0xFFF);

	return res;
}

int8_t SRAM_QRead(uint32_t address, uint8_t *read_data, uint32_t read_length)
{
	QSPI_CommandTypeDef sCommand;
	HAL_StatusTypeDef res = 0;

	sCommand.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	sCommand.Instruction	   = QIO_READ_CMD;
	sCommand.AddressMode	   = QSPI_ADDRESS_4_LINES;
	sCommand.Address		   = address;
	sCommand.AddressSize	   = QSPI_ADDRESS_24_BITS;
	sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCommand.DataMode		   = QSPI_DATA_4_LINES;
	sCommand.DummyCycles	   = 6;
	sCommand.DdrMode		   = QSPI_DDR_MODE_DISABLE;
	sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	sCommand.SIOOMode		   = QSPI_SIOO_INST_EVERY_CMD;
	sCommand.NbData			   = read_length;

	res += HAL_QSPI_Command(&hqspi1, &sCommand, 0xFFF);
	res += HAL_QSPI_Receive(&hqspi1, read_data, 0xFFF);

	return res;
}

int8_t SRAM_QReadID(uint8_t *read_data)
{
	QSPI_CommandTypeDef sCommand;
	HAL_StatusTypeDef res = 0;

	sCommand.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	sCommand.Instruction	   = READ_ID_CMD;
	sCommand.AddressMode	   = QSPI_ADDRESS_NONE;
	sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCommand.DataMode		   = QSPI_DATA_4_LINES;
	sCommand.DummyCycles	   = 6;
	sCommand.DdrMode		   = QSPI_DDR_MODE_DISABLE;
	sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	sCommand.SIOOMode		   = QSPI_SIOO_INST_EVERY_CMD;
	sCommand.NbData			   = 8;

	res += HAL_QSPI_Command(&hqspi1, &sCommand, 0xFFF);
	res += HAL_QSPI_Receive(&hqspi1, read_data, 0xFFF);

	return res;
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
  MX_USB_Device_Init();
  MX_SAI1_Init();
  MX_QUADSPI1_Init();
  /* USER CODE BEGIN 2 */

  SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
  SEGGER_RTT_printf(0, "Hello RTT world.\n");

  int count = 0;

  uint8_t id[8] = {0x0};

  SRAM_QMode();
  SRAM_QReadID(id);

  for (uint32_t i = 0; i < 8; i++)
  {
	  SEGGER_RTT_printf(0, "%d: %02X\n", i, id[i]);
  }

  uint32_t len = 256;
  uint8_t wData[len];
  uint8_t rData[len];
  for (uint32_t i = 0; i < len; i++)
  {
	  wData[i] = i;
	  rData[i] = 0;
  }

  SRAM_QWrite(0x400, wData, len);
  SRAM_QRead(0x400, rData, len);

  for (uint32_t i = 0; i < len; i++)
  {
	  SEGGER_RTT_printf(0, "rData[%d] = %d(%02X)\n", i, rData[i],rData[i]);
	  HAL_Delay(5);
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  //printf("fresh!\n");
	  //sprintf(buf, "hello %d\n", count++);
	  //CDC_Transmit_FS((uint8_t *)buf, strlen(buf));

	  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_12);
	  HAL_Delay(1000);

	  count = (count + 1) % 1024;
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 24;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV6;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief QUADSPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_QUADSPI1_Init(void)
{

  /* USER CODE BEGIN QUADSPI1_Init 0 */

  /* USER CODE END QUADSPI1_Init 0 */

  /* USER CODE BEGIN QUADSPI1_Init 1 */

  /* USER CODE END QUADSPI1_Init 1 */
  /* QUADSPI1 parameter configuration*/
  hqspi1.Instance = QUADSPI;
  hqspi1.Init.ClockPrescaler = 1;
  hqspi1.Init.FifoThreshold = 4;
  hqspi1.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_NONE;
  hqspi1.Init.FlashSize = 10;
  hqspi1.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_1_CYCLE;
  hqspi1.Init.ClockMode = QSPI_CLOCK_MODE_0;
  hqspi1.Init.FlashID = QSPI_FLASH_ID_1;
  hqspi1.Init.DualFlash = QSPI_DUALFLASH_DISABLE;
  if (HAL_QSPI_Init(&hqspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN QUADSPI1_Init 2 */

  /* USER CODE END QUADSPI1_Init 2 */

}

/**
  * @brief SAI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SAI1_Init(void)
{

  /* USER CODE BEGIN SAI1_Init 0 */

  /* USER CODE END SAI1_Init 0 */

  /* USER CODE BEGIN SAI1_Init 1 */

  /* USER CODE END SAI1_Init 1 */
  hsai_BlockB1.Instance = SAI1_Block_B;
  hsai_BlockB1.Init.Protocol = SAI_FREE_PROTOCOL;
  hsai_BlockB1.Init.AudioMode = SAI_MODEMASTER_TX;
  hsai_BlockB1.Init.DataSize = SAI_DATASIZE_16;
  hsai_BlockB1.Init.FirstBit = SAI_FIRSTBIT_MSB;
  hsai_BlockB1.Init.ClockStrobing = SAI_CLOCKSTROBING_FALLINGEDGE;
  hsai_BlockB1.Init.Synchro = SAI_ASYNCHRONOUS;
  hsai_BlockB1.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
  hsai_BlockB1.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
  hsai_BlockB1.Init.MckOverSampling = SAI_MCK_OVERSAMPLING_DISABLE;
  hsai_BlockB1.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_HF;
  hsai_BlockB1.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_MCKDIV;
  hsai_BlockB1.Init.MckOutput = SAI_MCK_OUTPUT_ENABLE;
  hsai_BlockB1.Init.Mckdiv = 4;
  hsai_BlockB1.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
  hsai_BlockB1.Init.MonoStereoMode = SAI_STEREOMODE;
  hsai_BlockB1.Init.CompandingMode = SAI_NOCOMPANDING;
  hsai_BlockB1.Init.TriState = SAI_OUTPUT_NOTRELEASED;
  hsai_BlockB1.Init.PdmInit.Activation = DISABLE;
  hsai_BlockB1.Init.PdmInit.MicPairsNbr = 0;
  hsai_BlockB1.Init.PdmInit.ClockEnable = SAI_PDM_CLOCK1_ENABLE;
  hsai_BlockB1.FrameInit.FrameLength = 32;
  hsai_BlockB1.FrameInit.ActiveFrameLength = 16;
  hsai_BlockB1.FrameInit.FSDefinition = SAI_FS_STARTFRAME;
  hsai_BlockB1.FrameInit.FSPolarity = SAI_FS_ACTIVE_LOW;
  hsai_BlockB1.FrameInit.FSOffset = SAI_FS_FIRSTBIT;
  hsai_BlockB1.SlotInit.FirstBitOffset = 1;
  hsai_BlockB1.SlotInit.SlotSize = SAI_SLOTSIZE_DATASIZE;
  hsai_BlockB1.SlotInit.SlotNumber = 2;
  hsai_BlockB1.SlotInit.SlotActive = 0x0000FFFF;
  if (HAL_SAI_Init(&hsai_BlockB1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SAI1_Init 2 */

  /* USER CODE END SAI1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMAMUX1_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : USER_SW_Pin */
  GPIO_InitStruct.Pin = USER_SW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(USER_SW_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USER_LED_Pin */
  GPIO_InitStruct.Pin = USER_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USER_LED_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void RTT_Read(void)
{
	char input;
	uint32_t size = SEGGER_RTT_Read(0, &input, 1u);

	if (size > 0)
	{
		SEGGER_RTT_printf(0, "%c", input);
	}
}
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

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

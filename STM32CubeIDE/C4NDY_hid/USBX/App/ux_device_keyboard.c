/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ux_device_keyboard.c
  * @author  MCD Application Team
  * @brief   USBX Device HID Keyboard applicative source file
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
#include "ux_device_keyboard.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MATRIX_ROWS 5
#define MATRIX_COLUMNS 13
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
UX_SLAVE_CLASS_HID *hid_keyboard;
UX_SLAVE_CLASS_HID_EVENT hid_event;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
struct keyboardHID_t {
	uint8_t modifiers;
	uint8_t reserved;
	uint8_t key[6];
} keyboardHID;

uint16_t keyState[MATRIX_ROWS] = {0x0};
uint16_t prevKeyState[MATRIX_ROWS] = {0x0};

bool isKeymapIDChanged = false;
uint8_t keymapID = 0;

// deafult QWERTY layout
uint8_t keymaps_default[MATRIX_ROWS][MATRIX_COLUMNS] = {
//       ESC   1     2     3     4     5     6     7     8     9     0     -_    =+
		{0x29, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x2D, 0x2E},
//       TAB   q     w     e     r     t     y     u     i     o     p     [{    ]}
		{0x2B, 0x14, 0x1A, 0x08, 0x15, 0x17, 0x1C, 0x18, 0x0C, 0x12, 0x13, 0x2F, 0x30},
//       CAPS  a     s     d     f     g     h     j     k     l     ;:    '"    \|
		{0x39, 0x04, 0x16, 0x07, 0x09, 0x0A, 0x0B, 0x0D, 0x0E, 0x0F, 0x33, 0x34, 0x31},
//       LSFT  z     x     c     v     b     n     m     ,<    .>    /?    RSFT  `~
		{0xE1, 0x1D, 0x1B, 0x06, 0x19, 0x05, 0x11, 0x10, 0x36, 0x37, 0x38, 0xE5, 0x35},
//       GUI               LALT  BS    ENT   SPC   Int4  RCTRL ?��?     ▼     ▲     ►
		{0xE3, 0xFE, 0xFF, 0xE2, 0x2A, 0x28, 0x2C, 0x8A, 0xE4, 0x50, 0x51, 0x52, 0x4F}
};

// Pinky-less Dvorak layout
uint8_t keymaps_pinkyless[MATRIX_ROWS][MATRIX_COLUMNS] = {
//       ESC   1     2     3     4     5     6     7     8     9     0     [{    }]
		{0x29, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x2F, 0x30},
//       TAB   '"    ,<    o     u     y     f     g     c     r     l     /?    +=
		{0x2B, 0x34, 0x36, 0x12, 0x18, 0x1C, 0x09, 0x0A, 0x06, 0x15, 0x0F, 0x38, 0x2E},
//       LCTRL p     i     e     a     .>    d     s     t     h     z     -_    \|
		{0xE0, 0x13, 0x0C, 0x08, 0x04, 0x37, 0x07, 0x16, 0x17, 0x0B, 0x1D, 0x2D, 0x31},
//       LSFT  j     q     ;:    k     x     b     m     w     n     v     RSFT  `~
		{0xE1, 0x0D, 0x14, 0x33, 0x0E, 0x1B, 0x05, 0x10, 0x1A, 0x11, 0x19, 0xE5, 0x35},
//       GUI               LALT  BS    DEL   ENT   SPC   CAPS  ?��?     ▼     ▲     ►
		{0xE3, 0xFE, 0xFF, 0xE2, 0x2A, 0x4C, 0x28, 0x2C, 0x39, 0x50, 0x51, 0x52, 0x4F}
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  USBD_HID_Keyboard_Activate
  *         This function is called when insertion of a HID Keyboard device.
  * @param  hid_instance: Pointer to the hid class instance.
  * @retval none
  */
VOID USBD_HID_Keyboard_Activate(VOID *hid_instance)
{
  /* USER CODE BEGIN USBD_HID_Keyboard_Activate */
  hid_keyboard = (UX_SLAVE_CLASS_HID *)hid_instance;
  /* USER CODE END USBD_HID_Keyboard_Activate */

  return;
}

/**
  * @brief  USBD_HID_Keyboard_Deactivate
  *         This function is called when extraction of a HID Keyboard device.
  * @param  hid_instance: Pointer to the hid class instance.
  * @retval none
  */
VOID USBD_HID_Keyboard_Deactivate(VOID *hid_instance)
{
  /* USER CODE BEGIN USBD_HID_Keyboard_Deactivate */
  UX_PARAMETER_NOT_USED(hid_instance);

  hid_keyboard = UX_NULL;
  /* USER CODE END USBD_HID_Keyboard_Deactivate */

  return;
}

/**
  * @brief  USBD_HID_Keyboard_SetReport
  *         This function is invoked when the host sends a HID SET_REPORT
  *         to the application over Endpoint 0.
  * @param  hid_instance: Pointer to the hid class instance.
  * @param  hid_event: Pointer to structure of the hid event.
  * @retval status
  */
UINT USBD_HID_Keyboard_SetReport(UX_SLAVE_CLASS_HID *hid_instance,
                                 UX_SLAVE_CLASS_HID_EVENT *hid_event)
{
  UINT status = UX_SUCCESS;

  /* USER CODE BEGIN USBD_HID_Keyboard_SetReport */
  UX_PARAMETER_NOT_USED(hid_instance);
  UX_PARAMETER_NOT_USED(hid_event);
  /* USER CODE END USBD_HID_Keyboard_SetReport */

  return status;
}

/**
  * @brief  USBD_HID_Keyboard_GetReport
  *         This function is invoked when host is requesting event through
  *         control GET_REPORT request.
  * @param  hid_instance: Pointer to the hid class instance.
  * @param  hid_event: Pointer to structure of the hid event.
  * @retval status
  */
UINT USBD_HID_Keyboard_GetReport(UX_SLAVE_CLASS_HID *hid_instance,
                                 UX_SLAVE_CLASS_HID_EVENT *hid_event)
{
  UINT status = UX_SUCCESS;

  /* USER CODE BEGIN USBD_HID_Keyboard_GetReport */
  UX_PARAMETER_NOT_USED(hid_instance);
  UX_PARAMETER_NOT_USED(hid_event);
  /* USER CODE END USBD_HID_Keyboard_GetReport */

  return status;
}

/* USER CODE BEGIN 1 */
/**
  * @brief  Function implementing usbx_hid_thread_entry.
  * @param  thread_input: Not used
  * @retval none
  */
VOID usbx_hid_thread_entry(ULONG thread_input)
{
  UX_SLAVE_DEVICE *device;
  UX_SLAVE_CLASS_HID_EVENT hid_event;

  UX_PARAMETER_NOT_USED(thread_input);

  device = &_ux_system_slave->ux_system_slave_device;

  ux_utility_memory_set(&hid_event, 0, sizeof(UX_SLAVE_CLASS_HID_EVENT));

  while (1)
  {
    /* Check if the device state already configured */
    if ((device->ux_slave_device_state == UX_DEVICE_CONFIGURED) && (hid_keyboard != UX_NULL))
    {
      /* sleep for 10ms */
      tx_thread_sleep(MS_TO_TICK(1));

      detectSwitches();
#if 0
      /* Check if user button is pressed */
      if (User_Button_State != 0U)
      {
        /* Get the new position */
        GetPointerData(&hid_event);

        /* Send an event to the hid */
        ux_device_class_hid_event_set(hid_keyboard, &hid_event);

        /* Reset User Button state */
        //User_Button_State = 0U;
      }
#endif
    }
    else
    {
      /* Sleep thread for 10ms */
      tx_thread_sleep(MS_TO_TICK(10));
    }
  }
}

uint8_t getKeyCode(uint8_t keymapId, uint8_t x, uint8_t y)
{
	if (keymapId == 0)
	{
		return keymaps_default[x][y];
	}
	else
	{
		return keymaps_pinkyless[x][y];
	}
}

void clearKeys(uint8_t code)
{
	if (code == 0xFF)
	{
		isKeymapIDChanged = false;
	}
	else if (code >= 0xE0 && code <= 0xE7)
	{
		keyboardHID.modifiers &= ~(1 << (code - 0xE0));
	}
	else
	{
		for (int k = 0; k < 6; k++)
		{
			if (keyboardHID.key[k] == code)
			{
				keyboardHID.key[k] = 0;
			}
		}
	}
}

void setKeys(uint8_t code)
{
	if (code == 0xFF)
	{
		if (!isKeymapIDChanged)
		{
			if (keymapID == 0)
			{
				keymapID = 1;
				HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_SET);
			}
			else
			{
				keymapID = 0;
				HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET);
			}
			isKeymapIDChanged = true;
		}
	}
	else if (code >= 0xE0 && code <= 0xE7)
	{
		keyboardHID.modifiers |= 1 << (code - 0xE0);
	}
	else
	{
		for (int k = 0; k < 6; k++)
		{
			if (keyboardHID.key[k] == code)
			{
				break;
			}
			else if (keyboardHID.key[k] == 0x00)
			{
				keyboardHID.key[k] = code;
				break;
			}
		}
	}
}

void detectSwitches(void)
{
	HAL_GPIO_WritePin(HC164_A_GPIO_Port, HC164_A_Pin, GPIO_PIN_RESET);

	for (int i = 0; i < MATRIX_ROWS; i++)
	{
		HAL_GPIO_WritePin(HC164_CLK_GPIO_Port, HC164_CLK_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(HC164_CLK_GPIO_Port, HC164_CLK_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(HC164_A_GPIO_Port, HC164_A_Pin, GPIO_PIN_SET);

		HAL_GPIO_WritePin(HC165_SL_GPIO_Port, HC165_SL_Pin, GPIO_PIN_RESET);
		tx_thread_sleep(MS_TO_TICK(1));
		HAL_GPIO_WritePin(HC165_SL_GPIO_Port, HC165_SL_Pin, GPIO_PIN_SET);

		for (int j = 0; j < 16; j++)
		{
			uint8_t jj = 255;
			if (j < 8)
			{
				jj = j + 5;
			}
			else if (j >= 11 && j < 16)
			{
				jj = j - 11;
			}

			if (jj < MATRIX_COLUMNS)
			{
				if (HAL_GPIO_ReadPin(HC165_QH_GPIO_Port, HC165_QH_Pin))
				{
					keyState[i] &= ~((uint16_t)1 << jj);

					if (keyState[i] != prevKeyState[i])
					{
						uint8_t keycode = getKeyCode(keymapID, i, (MATRIX_COLUMNS - 1) - jj);
						clearKeys(keycode);
					}
				}
				else
				{
					keyState[i] |= ((uint16_t)1 << jj);

					uint8_t keycode = getKeyCode(keymapID, i, (MATRIX_COLUMNS - 1) - jj);
					setKeys(keycode);
				}
			}

			HAL_GPIO_WritePin(HC165_CLK_GPIO_Port, HC165_CLK_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(HC165_CLK_GPIO_Port, HC165_CLK_Pin, GPIO_PIN_RESET);
		}

#if 1
		SEGGER_RTT_printf(0, "key[%d]: 0x%04X\n", i, keyState[i]);
#endif
	}

	for (int i = 0; i < MATRIX_ROWS; i++)
	{
		if (keyState[i] != 0x0 || (keyState[i] == 0x0 && keyState[i] != prevKeyState[i]))
		{
			hid_event.ux_device_class_hid_event_length = sizeof(keyboardHID);
			hid_event.ux_device_class_hid_event_buffer[0] = keyboardHID.modifiers;
			hid_event.ux_device_class_hid_event_buffer[1] = keyboardHID.reserved;
			hid_event.ux_device_class_hid_event_buffer[2] = keyboardHID.key[0];
			hid_event.ux_device_class_hid_event_buffer[3] = keyboardHID.key[1];
			hid_event.ux_device_class_hid_event_buffer[4] = keyboardHID.key[2];
			hid_event.ux_device_class_hid_event_buffer[5] = keyboardHID.key[3];
			hid_event.ux_device_class_hid_event_buffer[6] = keyboardHID.key[4];
			hid_event.ux_device_class_hid_event_buffer[7] = keyboardHID.key[5];

			ux_device_class_hid_event_set(hid_keyboard, &hid_event);

			//USBD_HID_SendReport(&hUsbDeviceFS, &keyboardHID, sizeof(keyboardHID));
			break;
		}
	}

	for (int i = 0; i < MATRIX_ROWS; i++)
	{
		prevKeyState[i] = keyState[i];
	}
}
/* USER CODE END 1 */

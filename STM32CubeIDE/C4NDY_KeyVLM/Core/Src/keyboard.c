/*
 * keyboard.c
 *
 *  Created on: Aug 17, 2023
 *      Author: shun
 */

#include "main.h"
#include "keyboard.h"
#include "audio_control.h"

struct keyboardHID_t {
	uint8_t modifiers;
	uint8_t reserved;
	uint8_t key[6];
} keyboardHID;

uint16_t keyState[MATRIX_ROWS] = {0x0};
uint16_t prevKeyState[MATRIX_ROWS] = {0x0};

bool isSwitchDFUMode = false;
uint32_t longPressCounter = 0;

bool isKeymapIDChanged = false;
uint8_t keymapID = 0;

bool isLinePhonoSWChanged = false;
uint8_t linePhonoSW = 0;

// deafult QWERTY layout
uint8_t keymaps_default[MATRIX_ROWS][MATRIX_COLUMNS] = {
	{SC_ESC,      SC_1,    SC_2,      SC_3,    SC_4,  SC_5,     SC_6,     SC_7,      SC_8,        SC_9,      SC_0,     SC_MINUS,  SC_EQUAL},
	{SC_TAB,      SC_Q,    SC_W,      SC_E,    SC_R,  SC_T,     SC_Y,     SC_U,      SC_I,        SC_O,      SC_P,     SC_LSB,    SC_RSB},
	{SC_CAPSLOCK, SC_A,    SC_S,      SC_D,    SC_F,  SC_G,     SC_H,     SC_J,      SC_K,        SC_L,      SC_SC,    SC_APS,    SC_YEN},
	{SC_LSHIFT,   SC_Z,    SC_X,      SC_C,    SC_V,  SC_B,     SC_N,     SC_M,      SC_COMMA,    SC_PERIOD, SC_SLASH, SC_RSHIFT, SC_GA},
	{SC_LGUI,     SC_LNPH, SC_LAYOUT, SC_LALT, SC_BS, SC_ENTER, SC_SPACE, SC_HENKAN, SC_RCONTROL, SC_LEFT,   SC_DOWN,  SC_UP,     SC_RIGHT}
};

// Improved Dvorak layout
uint8_t keymaps_layout2[MATRIX_ROWS][MATRIX_COLUMNS] = {
	{SC_ESC,      SC_1,    SC_2,      SC_3,    SC_4,  SC_5,      SC_6,     SC_7,     SC_8,        SC_9,    SC_0,    SC_LSB,    SC_RSB},
	{SC_TAB,      SC_APS,  SC_COMMA,  SC_O,    SC_U,  SC_Y,      SC_F,     SC_G,     SC_C,        SC_R,    SC_L,    SC_SLASH,  SC_EQUAL},
	{SC_LCONTROL, SC_P,    SC_I,      SC_E,    SC_A,  SC_PERIOD, SC_D,     SC_S,     SC_T,        SC_H,    SC_Z,    SC_MINUS,  SC_BSLASH},
	{SC_LSHIFT,   SC_J,    SC_Q,      SC_SC,   SC_K,  SC_X,      SC_B,     SC_M,     SC_W,        SC_N,    SC_V,    SC_RSHIFT, SC_GA},
	{SC_LGUI,     SC_LNPH, SC_LAYOUT, SC_LALT, SC_BS, SC_DELETE, SC_ENTER, SC_SPACE, SC_CAPSLOCK, SC_LEFT, SC_DOWN, SC_UP,     SC_RIGHT}
};

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
#if 0
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len)
{
  (void) instance;
  (void) len;

  uint8_t next_report_id = report[0] + 1u;

  if (next_report_id < REPORT_ID_COUNT)
  {
    send_hid_report(next_report_id, board_button_read());
  }
}
#endif

void setLinePhonoSW(uint8_t val)
{
	linePhonoSW = val;
	if (linePhonoSW == 0)
	{
		linePhonoSW = 1;
		send_switch_to_linein();
		HAL_GPIO_WritePin(LP_LED_GPIO_Port, LP_LED_Pin, GPIO_PIN_RESET);
	}
	else
	{
		linePhonoSW = 0;
		send_switch_to_phonoin();
		HAL_GPIO_WritePin(LP_LED_GPIO_Port, LP_LED_Pin, GPIO_PIN_SET);
	}
}

uint8_t getLinePhonoSW(void)
{
	return linePhonoSW;
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  // TODO not Implemented
  (void) instance;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  (void) instance;

  SEGGER_RTT_printf(0, "report_id = %d\n", report_id);
  SEGGER_RTT_printf(0, "report_type = %d\n", report_type);
  for (int i = 0; i < bufsize; i++)
  {
	  SEGGER_RTT_printf(0, "buf[%d] = %d\n", i, buffer[i]);
  }
  SEGGER_RTT_printf(0, "bufsize = %d\n", bufsize);

  uint8_t rbuf[16] = {0x00};
  if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x00)
  {
	  SEGGER_RTT_printf(0, "read:\n");
	  if (keymapID == 0)
	  {
		  for (int j = 0; j < 13; j++)
		  {
			  rbuf[j] = keymaps_default[buffer[0] - 0xF0][j];
		  }
	  }
	  else
	  {
		  for (int j = 0; j < 13; j++)
		  {
			  rbuf[j] = keymaps_layout2[buffer[0] - 0xF0][j];
		  }
	  }

#if 0
	  for (int i = 0; i < 16; i++)
	  {
		  SEGGER_RTT_printf(0, "rbuf[%d] = %d\n", i, rbuf[i]);
	  }
#endif
	  tud_hid_n_report(1, 0, rbuf, 16);

  }
  else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x01)
  {
	  SEGGER_RTT_printf(0, "write to layout1:\n");

	  for (int j = 0; j < 13; j++)
	  {
		  keymaps_default[buffer[0] - 0xF0][j] = buffer[j + 2];
	  }
  }
  else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x02)
  {
	  SEGGER_RTT_printf(0, "write to layout2:\n");

	  for (int j = 0; j < 13; j++)
	  {
		  keymaps_layout2[buffer[0] - 0xF0][j] = buffer[j + 2];
	  }
  }
  else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x03)
  {
	  SEGGER_RTT_printf(0, "read from layout1:\n");
	  for (int j = 0; j < 13; j++)
	  {
		  rbuf[j] = keymaps_default[buffer[0] - 0xF0][j];
	  }

	  tud_hid_n_report(1, 0, rbuf, 16);
  }
  else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x04)
  {
	  SEGGER_RTT_printf(0, "read from layout2:\n");
	  for (int j = 0; j < 13; j++)
	  {
		  rbuf[j] = keymaps_layout2[buffer[0] - 0xF0][j];
	  }

	  tud_hid_n_report(1, 0, rbuf, 16);
  }
  else if (buffer[0] == 0xF5)
  {
	  SEGGER_RTT_printf(0, "erase & write FLASH...\n");
	  HAL_FLASH_Unlock();

	  erase_flash_data();

	  write_flash_data(0, linePhonoSW);
	  write_flash_data(1, 99);

	  for (int i = 0; i < 5; i++)
	  {
		  for (int j = 0; j < 13; j++)
		  {
			  write_flash_data(2 + 0 * 65 + i * 13 + j, keymaps_default[i][j]);
			  write_flash_data(2 + 1 * 65 + i * 13 + j, keymaps_layout2[i][j]);
		  }
	  }

	  HAL_FLASH_Lock();

	  uint8_t rbuf[16] = {0x00};
	  rbuf[1] = 0xF5;
	  rbuf[2] = 0x01;
	  tud_hid_n_report(1, 0, rbuf, 16);
  }

#if 0
  if (report_type == HID_REPORT_TYPE_OUTPUT)
  {
    // Set keyboard LED e.g Capslock, Numlock etc...
    if (report_id == REPORT_ID_KEYBOARD)
    {
      // bufsize should be (at least) 1
      if ( bufsize < 1 ) return;

      uint8_t const kbd_leds = buffer[0];

      if (kbd_leds & KEYBOARD_LED_CAPSLOCK)
      {
        // Capslock On: disable blink, turn led on
        blink_interval_ms = 0;
        board_led_write(true);
      }else
      {
        // Caplocks Off: back to normal blink
        board_led_write(false);
        blink_interval_ms = BLINK_MOUNTED;
      }
    }
  }
#endif
}

uint8_t getKeyCode(uint8_t keymapId, uint8_t x, uint8_t y)
{
	if (keymapId == 0)
	{
		return keymaps_default[x][y];
	}
	else
	{
		return keymaps_layout2[x][y];
	}
}

void setKeyCode(uint8_t keymapId, uint8_t x, uint8_t y, uint8_t code)
{
	if (keymapId == 0)
	{
		keymaps_default[x][y] = code;
	}
	else
	{
		keymaps_layout2[x][y] = code;
	}
}

void resetKeys(void)
{
	keyboardHID.modifiers = 0;
	for (int k = 0; k < 6; k++)
	{
		keyboardHID.key[k] = 0;
	}
}

void clearKeys(uint8_t code)
{
	if (code == SC_LAYOUT)
	{
		isKeymapIDChanged = false;
	}
	else if (code == SC_LNPH)
	{
		isLinePhonoSWChanged = false;
	}
	else if (code >= SC_LCONTROL && code <= SC_RGUI)
	{
		keyboardHID.modifiers &= ~(1 << (code - SC_LCONTROL));
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

	longPressCounter = 0;
}

void setKeys(uint8_t code)
{
	if (code == SC_LAYOUT)
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
	else if (code == SC_LNPH)
	{
		if (!isLinePhonoSWChanged)
		{
			setLinePhonoSW(linePhonoSW);
			isLinePhonoSWChanged = true;
		}
	}
	else if (code >= SC_LCONTROL && code <= SC_RGUI)
	{
		keyboardHID.modifiers |= 1 << (code - SC_LCONTROL);
	}
	else
	{
		for (int k = 0; k < 6; k++)
		{
			if (keyboardHID.key[k] == code)
			{
				// LGUI + ESC 長押しでDFUモーでリセット
				if (keyboardHID.modifiers == 0x08 && keyboardHID.key[k] == SC_ESC)
				{
					longPressCounter++;
					if (longPressCounter == 5000)
					{
						setBootDfuFlag(true);
						SEGGER_RTT_printf(0, "Boot Custom DFU...\n");
						HAL_Delay(100);
						NVIC_SystemReset();
					}
				}
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

void hid_keyscan_task(void)
{
	static int i = 0;

	switch (i)
	{
	case 0:
		HAL_GPIO_WritePin(HC164_A_GPIO_Port, HC164_A_Pin, GPIO_PIN_RESET);
	case 1:
	case 2:
	case 3:
	case 4:
		HAL_GPIO_WritePin(HC164_CLK_GPIO_Port, HC164_CLK_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(HC164_CLK_GPIO_Port, HC164_CLK_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(HC164_A_GPIO_Port, HC164_A_Pin, GPIO_PIN_SET);

		HAL_GPIO_WritePin(HC165_SL_GPIO_Port, HC165_SL_Pin, GPIO_PIN_RESET);
		asm("NOP");
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

		i++;
		break;
	case 5:
		for (int i = 0; i < MATRIX_ROWS; i++)
		{
			if (keyState[i] != 0x0 || (keyState[i] == 0x0 && keyState[i] != prevKeyState[i]))
			{
				if (!tud_hid_ready())
					return;

				tud_hid_keyboard_report(REPORT_ID_KEYBOARD, keyboardHID.modifiers, keyboardHID.key);
				break;
			}
		}

		for (int i = 0; i < MATRIX_ROWS; i++)
		{
			prevKeyState[i] = keyState[i];
		}

		i = 0;
		break;
	default:
		i = 0;
		break;
	}
}

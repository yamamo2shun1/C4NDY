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

// Pinky-less Dvorak layout
uint8_t keymaps_pinkyless[MATRIX_ROWS][MATRIX_COLUMNS] = {
	{SC_ESC,      SC_1,    SC_2,      SC_3,    SC_4,  SC_5,      SC_6,     SC_7,     SC_8,        SC_9,    SC_0,    SC_LSB,    SC_RSB},
	{SC_TAB,      SC_APS,  SC_COMMA,  SC_O,    SC_U,  SC_Y,      SC_F,     SC_G,     SC_C,        SC_R,    SC_L,    SC_SLASH,  SC_EQUAL},
	{SC_LCONTROL, SC_P,    SC_I,      SC_E,    SC_A,  SC_PERIOD, SC_D,     SC_S,     SC_T,        SC_H,    SC_Z,    SC_MINUS,  SC_BSLASH},
	{SC_LSHIFT,   SC_J,    SC_Q,      SC_SC,   SC_K,  SC_X,      SC_B,     SC_M,     SC_W,        SC_N,    SC_V,    SC_RSHIFT, SC_GA},
	{SC_LGUI,     SC_LNPH, SC_LAYOUT, SC_LALT, SC_BS, SC_DELETE, SC_ENTER, SC_SPACE, SC_CAPSLOCK, SC_LEFT, SC_DOWN, SC_UP,     SC_RIGHT}
};

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  (void) instance;

  SEGGER_RTT_printf(0, "report_id = %d\n", report_id);
  SEGGER_RTT_printf(0, "report_type = %d\n", report_type);
  SEGGER_RTT_printf(0, "buf[%d] = %d\n", 0, buffer[0]);
  SEGGER_RTT_printf(0, "bufsize = %d\n", bufsize);
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
		return keymaps_pinkyless[x][y];
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
			if (linePhonoSW == 0)
			{
				linePhonoSW = 1;
				send_switch_to_linein();
			}
			else
			{
				linePhonoSW = 0;
				send_switch_to_phonoin();
			}
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

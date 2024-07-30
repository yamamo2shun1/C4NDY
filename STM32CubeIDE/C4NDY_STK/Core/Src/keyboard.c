/*
 * keyboard.c
 *
 *  Created on: Aug 17, 2023
 *      Author: shun
 */

#include "main.h"
#include "keyboard.h"
#include "audio_control.h"
#include "adc.h"

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

bool isUpper = false;

uint8_t countReturnNeutral = 0;
#define MAX_COUNT_RETURN_NEUTRAL 100

int8_t currentStkH[2] = {0};
int8_t currentStkV[2] = {0};
int8_t prevStkH[2] = {0};
int8_t prevStkV[2] = {0};

// deafult QWERTY layout
uint8_t keymaps_default[MATRIX_ROWS][MATRIX_COLUMNS] = {
	{SC_Q,    SC_W,    SC_E,      SC_R,    SC_T,      SC_Y,    SC_U,      SC_I,        SC_O,      SC_P},
	{SC_A,    SC_S,    SC_D,      SC_F,    SC_G,      SC_H,    SC_J,      SC_K,        SC_L,      SC_SC},
	{SC_Z,    SC_X,    SC_C,      SC_V,    SC_B,      SC_N,    SC_M,      SC_COMMA,    SC_PERIOD, SC_SLASH},
	{SC_NULL, SC_LNPH, SC_LAYOUT, SC_NULL, SC_LSHIFT, SC_NULL, SC_LEFT,   SC_DOWN,     SC_UP,     SC_RIGHT}
};

// Improved Dvorak layout
uint8_t keymaps_layout2[MATRIX_ROWS][MATRIX_COLUMNS] = {
	{SC_APS,  SC_COMMA, SC_O,      SC_U,    SC_Y,      SC_F,    SC_G,    SC_C,    SC_R,  SC_L},
	{SC_P,    SC_I,     SC_E,      SC_A,    SC_PERIOD, SC_D,    SC_S,    SC_T,    SC_H,  SC_Z},
	{SC_J,    SC_Q,     SC_SC,     SC_K,    SC_X,      SC_B,    SC_M,    SC_W,    SC_N,  SC_V},
	{SC_NULL, SC_LNPH,  SC_LAYOUT, SC_NULL, SC_LSHIFT, SC_NULL, SC_LEFT, SC_DOWN, SC_UP, SC_RIGHT}
};

uint8_t keymaps_default_upper[MATRIX_ROWS][MATRIX_COLUMNS] = {
	{SC_1,    SC_2,        SC_3,      SC_4,    SC_5,      SC_6,    SC_7,     SC_8,     SC_9,    SC_0},
	{SC_Q,    SC_W,        SC_NULL,   SC_LGUI, SC_G,      SC_NULL, SC_MINUS, SC_EQUAL, SC_LSB,  SC_APS},
	{SC_NULL, SC_RCONTROL, SC_NULL,   SC_NULL, SC_NULL,   SC_NULL, SC_RSB,   SC_NULL,  SC_NULL, SC_YEN},
	{SC_NULL, SC_LNPH,     SC_LAYOUT, SC_NULL, SC_LSHIFT, SC_NULL, SC_NULL,  SC_NULL,  SC_NULL, SC_GA}
};

uint8_t keymaps_layout2_upper[MATRIX_ROWS][MATRIX_COLUMNS] = {
	{SC_1,    SC_2,        SC_3,      SC_4,    SC_5,      SC_6,    SC_7,     SC_8,    SC_9,     SC_0},
	{SC_APS,  SC_COMMA,    SC_NULL,   SC_LGUI, SC_PERIOD, SC_NULL, SC_LSB,   SC_RSB,  SC_SLASH, SC_MINUS},
	{SC_NULL, SC_CAPSLOCK, SC_NULL,   SC_NULL, SC_NULL,   SC_NULL, SC_EQUAL, SC_NULL, SC_NULL,  SC_BSLASH},
	{SC_NULL, SC_LNPH,     SC_LAYOUT, SC_NULL, SC_LSHIFT, SC_NULL, SC_NULL,  SC_NULL, SC_NULL,  SC_GA}
};

uint8_t keymaps_default_stk[2][4] = {
//   left,   right,     up,        down
	{SC_BS,  SC_TAB,    SC_UPPER, SC_CAPSLOCK}, // left stick
	{SC_ESC, SC_HENKAN, SC_RALT,  SC_SPACE}     // right stick
};

uint8_t keymaps_layout2_stk[2][4] = {
//   left,   right,    up,        down
	{SC_BS,  SC_TAB,   SC_UPPER, SC_LCONTROL}, // left stick
	{SC_ESC, SC_SPACE, SC_RALT,  SC_ENTER}     // right stick
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
		send_switch_to_linein();
		HAL_GPIO_WritePin(LP_LED_GPIO_Port, LP_LED_Pin, GPIO_PIN_RESET);
	}
	else
	{
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
		  for (int j = 0; j < MATRIX_COLUMNS; j++)
		  {
			  rbuf[j] = keymaps_default[buffer[0] - 0xF0][j];
		  }
	  }
	  else
	  {
		  for (int j = 0; j < MATRIX_COLUMNS; j++)
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

	  for (int j = 0; j < MATRIX_COLUMNS; j++)
	  {
		  keymaps_default[buffer[0] - 0xF0][j] = buffer[j + 2];
	  }
  }
  else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x02)
  {
  	  SEGGER_RTT_printf(0, "write to layout1 upper:\n");

  	  for (int j = 0; j < MATRIX_COLUMNS; j++)
  	  {
  		  keymaps_default_upper[buffer[0] - 0xF0][j] = buffer[j + 2];
  	  }
  }
  else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x03)
  {
  	  SEGGER_RTT_printf(0, "write to layout1 stick:\n");

  	  for (int j = 0; j < 4; j++)
  	  {
  		  keymaps_default_stk[buffer[0] - 0xF0][j] = buffer[j + 2];
  	  }
  }
  else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x04)
  {
	  SEGGER_RTT_printf(0, "write to layout2:\n");

	  for (int j = 0; j < MATRIX_COLUMNS; j++)
	  {
		  keymaps_layout2[buffer[0] - 0xF0][j] = buffer[j + 2];
	  }
  }
  else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x05)
  {
  	  SEGGER_RTT_printf(0, "write to layout2 upper:\n");

  	  for (int j = 0; j < MATRIX_COLUMNS; j++)
  	  {
  		  keymaps_layout2_upper[buffer[0] - 0xF0][j] = buffer[j + 2];
  	  }
  }
  else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x06)
  {
   	  SEGGER_RTT_printf(0, "write to layout2 stick:\n");

   	  for (int j = 0; j < 4; j++)
   	  {
    	  keymaps_layout2_stk[buffer[0] - 0xF0][j] = buffer[j + 2];
   	  }
  }
  else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x07)
  {
	  SEGGER_RTT_printf(0, "read from layout1:\n");
	  for (int j = 0; j < MATRIX_COLUMNS; j++)
	  {
		  rbuf[j] = keymaps_default[buffer[0] - 0xF0][j];
	  }

	  tud_hid_n_report(1, 0, rbuf, 16);
  }
  else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x08)
  {
	  SEGGER_RTT_printf(0, "read from layout1 upper:\n");
	  for (int j = 0; j < MATRIX_COLUMNS; j++)
	  {
		  rbuf[j] = keymaps_default_upper[buffer[0] - 0xF0][j];
	  }

	  tud_hid_n_report(1, 0, rbuf, 16);
  }
  else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x09)
  {
	  SEGGER_RTT_printf(0, "read from layout1 stick:\n");
	  for (int j = 0; j < 4; j++)
	  {
		  rbuf[j] = keymaps_default_stk[buffer[0] - 0xF0][j];
	  }

	  tud_hid_n_report(1, 0, rbuf, 16);
  }
  else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x0A)
  {
	  SEGGER_RTT_printf(0, "read from layout2:\n");
	  for (int j = 0; j < MATRIX_COLUMNS; j++)
	  {
		  rbuf[j] = keymaps_layout2[buffer[0] - 0xF0][j];
	  }

	  tud_hid_n_report(1, 0, rbuf, 16);
  }
  else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x0B)
  {
	  SEGGER_RTT_printf(0, "read from layout2 upper:\n");
	  for (int j = 0; j < MATRIX_COLUMNS; j++)
	  {
		  rbuf[j] = keymaps_layout2_upper[buffer[0] - 0xF0][j];
	  }

	  tud_hid_n_report(1, 0, rbuf, 16);
  }
  else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x0C)
  {
	  SEGGER_RTT_printf(0, "read from layout2 stick:\n");
	  for (int j = 0; j < 4; j++)
	  {
		  rbuf[j] = keymaps_layout2_stk[buffer[0] - 0xF0][j];
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

	  for (int i = 0; i < MATRIX_ROWS; i++)
	  {
		  for (int j = 0; j < MATRIX_COLUMNS; j++)
		  {
			  write_flash_data(2 + 0 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, getKeyCode(0, i, j));
			  write_flash_data(2 + 1 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, getKeyCode(1, i, j));

			  write_flash_data(2 + (2 * MATRIX_ROWS * MATRIX_COLUMNS) + 0 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, getUpperKeyCode(0, i, j));
			  write_flash_data(2 + (2 * MATRIX_ROWS * MATRIX_COLUMNS) + 1 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, getUpperKeyCode(1, i, j));
		  }
	  }

	  for (int i = 0; i < 2; i++)
  	  {
	  	  for (int j = 0; j < 4; j++)
	  	  {
			  write_flash_data(2 + (4 * MATRIX_ROWS * MATRIX_COLUMNS) + 0 * (2 * 4) + i * 4 + j, getStickKeyCode(0, i, j));
  			  write_flash_data(2 + (4 * MATRIX_ROWS * MATRIX_COLUMNS) + 1 * (2 * 4) + i * 4 + j, getStickKeyCode(1, i, j));
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

uint8_t getUpperKeyCode(uint8_t keymapId, uint8_t x, uint8_t y)
{
	if (keymapId == 0)
	{
		return keymaps_default_upper[x][y];
	}
	else
	{
		return keymaps_layout2_upper[x][y];
	}
}

uint8_t getStickKeyCode(uint8_t keymapId, uint8_t id, uint8_t direction)
{
	if (keymapId == 0)
	{
		return keymaps_default_stk[id][direction];
	}
	else
	{
		return keymaps_layout2_stk[id][direction];
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

void setUpperKeyCode(uint8_t keymapId, uint8_t x, uint8_t y, uint8_t code)
{
	if (keymapId == 0)
	{
		keymaps_default_upper[x][y] = code;
	}
	else
	{
		keymaps_layout2_upper[x][y] = code;
	}
}

void setStickKeyCode(uint8_t keymapId, uint8_t id, uint8_t direction, uint8_t code)
{
	if (keymapId == 0)
	{
		keymaps_default_stk[id][direction] = code;
	}
	else
	{
		keymaps_layout2_stk[id][direction] = code;
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
		SEGGER_RTT_printf(0, "clear::key = {%X,%X,%X,%X,%X,%X}, modifier = %X\r\n", keyboardHID.key[0], keyboardHID.key[1], keyboardHID.key[2], keyboardHID.key[3], keyboardHID.key[4], keyboardHID.key[5], keyboardHID.modifiers);
		isKeymapIDChanged = false;
	}
	else if (code == SC_LNPH)
	{
		SEGGER_RTT_printf(0, "clear::key = {%X,%X,%X,%X,%X,%X}, modifier = %X\r\n", keyboardHID.key[0], keyboardHID.key[1], keyboardHID.key[2], keyboardHID.key[3], keyboardHID.key[4], keyboardHID.key[5], keyboardHID.modifiers);
		isLinePhonoSWChanged = false;
	}
	else if (code == SC_UPPER)
	{
		isUpper = false;
	}
	else if (code >= SC_LCONTROL && code <= SC_RGUI)
	{
		SEGGER_RTT_printf(0, "clear::code = %d\r\n", code - SC_LCONTROL);
		keyboardHID.modifiers &= ~(1 << (code - SC_LCONTROL));
	}
	else
	{
		SEGGER_RTT_printf(0, "clear0::key = {%X,%X,%X,%X,%X,%X}, modifier = %X\r\n", keyboardHID.key[0], keyboardHID.key[1], keyboardHID.key[2], keyboardHID.key[3], keyboardHID.key[4], keyboardHID.key[5], keyboardHID.modifiers);
		for (int k = 0; k < 6; k++)
		{
			if (keyboardHID.key[k] == code)
			{
				keyboardHID.key[k] = 0;
			}
		}
		SEGGER_RTT_printf(0, "clear1::key = {%X,%X,%X,%X,%X,%X}, modifier = %X\r\n", keyboardHID.key[0], keyboardHID.key[1], keyboardHID.key[2], keyboardHID.key[3], keyboardHID.key[4], keyboardHID.key[5], keyboardHID.modifiers);
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
			setLinePhonoSW(!linePhonoSW);
			setBootDfuFlag(false);
			isLinePhonoSWChanged = true;
		}
	}
	else if (code == SC_UPPER)
	{
		if (!isUpper)
		{
			isUpper = true;
		}
	}
	else if (code >= SC_LCONTROL && code <= SC_RGUI)
	{
		SEGGER_RTT_printf(0, "set::code = %d\r\n", code - SC_LCONTROL);
		keyboardHID.modifiers |= 1 << (code - SC_LCONTROL);
	}
	else
	{
		SEGGER_RTT_printf(0, "set0::key = {%X,%X,%X,%X,%X,%X}, modifier = %X\r\n", keyboardHID.key[0], keyboardHID.key[1], keyboardHID.key[2], keyboardHID.key[3], keyboardHID.key[4], keyboardHID.key[5], keyboardHID.modifiers);
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
		SEGGER_RTT_printf(0, "set1::key = {%X,%X,%X,%X,%X,%X}, modifier = %X\r\n", keyboardHID.key[0], keyboardHID.key[1], keyboardHID.key[2], keyboardHID.key[3], keyboardHID.key[4], keyboardHID.key[5], keyboardHID.modifiers);
	}
}

void controlJoySticks()
{
	// Stk0 Horizontal
	if (pot_value[2] < 2048 - 1500)
	{
		currentStkH[0] = 1;
	}
	else if (pot_value[2] >= 2048 + 1500)
	{
		currentStkH[0] = -1;
	}
	else
	{
		currentStkH[0] = 0;
	}

	if (currentStkH[0] != prevStkH[0])
	{
		switch (currentStkH[0])
		{
		case -1:
			SEGGER_RTT_printf(0, "BS\r\n");
			if (keymapID == 0)
			{
				setKeys(keymaps_default_stk[0][0]);
			}
			else
			{
				setKeys(keymaps_layout2_stk[0][0]);
			}
			break;
		case 1:
			SEGGER_RTT_printf(0, "TAB\r\n");
			if (keymapID == 0)
			{
				setKeys(keymaps_default_stk[0][1]);
			}
			else
			{
				setKeys(keymaps_layout2_stk[0][1]);
			}
			break;
		default:
			SEGGER_RTT_printf(0, "stk0H: Neutral\r\n");
			if (prevStkH[0] == -1)
			{
				if (keymapID == 0)
				{
					clearKeys(keymaps_default_stk[0][0]);
				}
				else
				{
					clearKeys(keymaps_layout2_stk[0][0]);
				}
				resetKeys();
				countReturnNeutral = MAX_COUNT_RETURN_NEUTRAL;
			}
			else if (prevStkH[0] == 1)
			{
				if (keymapID == 0)
				{
					clearKeys(keymaps_default_stk[0][1]);
				}
				else
				{
					clearKeys(keymaps_layout2_stk[0][1]);
				}
				resetKeys();
				countReturnNeutral = MAX_COUNT_RETURN_NEUTRAL;
			}
			break;
		}
	}

	// Stk0 Vertical
	if (pot_value[0] < 2048 - 1500)
	{
		currentStkV[0] = -1;
	}
	else if (pot_value[0] >= 2048 + 1500)
	{
		currentStkV[0] = 1;
	}
	else
	{
		currentStkV[0] = 0;
	}

	if (currentStkV[0] != prevStkV[0])
	{
		switch (currentStkV[0])
		{
		case -1:
			SEGGER_RTT_printf(0, "ESC\r\n");
			if (keymapID == 0)
			{
				setKeys(keymaps_default_stk[0][3]);
			}
			else
			{
				setKeys(keymaps_layout2_stk[0][3]);
			}
			break;
		case 1:
			SEGGER_RTT_printf(0, "UPPER\r\n");
			if (keymapID == 0)
			{
				setKeys(keymaps_default_stk[0][2]);
			}
			else
			{
				setKeys(keymaps_layout2_stk[0][2]);
			}
			break;
		default:
			SEGGER_RTT_printf(0, "stk0V: Neutral\r\n");
			if (prevStkV[0] == -1)
			{
				if (keymapID == 0)
				{
					clearKeys(keymaps_default_stk[0][3]);
				}
				else
				{
					clearKeys(keymaps_layout2_stk[0][3]);
				}
				resetKeys();
				countReturnNeutral = MAX_COUNT_RETURN_NEUTRAL;
			}
			else if (prevStkV[0] == 1)
			{
				if (keymapID == 0)
				{
					clearKeys(keymaps_default_stk[0][2]);
				}
				else
				{
					clearKeys(keymaps_layout2_stk[0][2]);
				}
				resetKeys();
				countReturnNeutral = MAX_COUNT_RETURN_NEUTRAL;
			}
			break;
		}
	}

	// Stk1 Horizontal
	if (pot_value[4] < 2048 - 1500)
	{
		currentStkH[1] = 1;
	}
	else if (pot_value[4] >= 2048 + 1500)
	{
		currentStkH[1] = -1;
	}
	else
	{
		currentStkH[1] = 0;
	}

	if (currentStkH[1] != prevStkH[1])
	{
		switch (currentStkH[1])
		{
		case -1:
			SEGGER_RTT_printf(0, "DEL\r\n");
			if (keymapID == 0)
			{
				setKeys(keymaps_default_stk[1][0]);
			}
			else
			{
				setKeys(keymaps_layout2_stk[1][0]);
			}
			break;
		case 1:
			SEGGER_RTT_printf(0, "SPACE\r\n");
			if (keymapID == 0)
			{
				setKeys(keymaps_default_stk[1][1]);
			}
			else
			{
				setKeys(keymaps_layout2_stk[1][1]);
			}
			break;
		default:
			SEGGER_RTT_printf(0, "stk1H: Neutral\r\n");
			if (prevStkH[1] == -1)
			{
				if (keymapID == 0)
				{
					clearKeys(keymaps_default_stk[1][0]);
				}
				else
				{
					clearKeys(keymaps_layout2_stk[1][0]);
				}
				resetKeys();
				countReturnNeutral = MAX_COUNT_RETURN_NEUTRAL;
			}
			else if (prevStkH[1] == 1)
			{
				if (keymapID == 0)
				{
					clearKeys(keymaps_default_stk[1][1]);
				}
				else
				{
					clearKeys(keymaps_layout2_stk[1][1]);
				}
				resetKeys();
				countReturnNeutral = MAX_COUNT_RETURN_NEUTRAL;
			}
			break;
		}
	}

	// Stk1 Vertical
	if (pot_value[3] < 2048 - 1500)
	{
		currentStkV[1] = -1;
	}
	else if (pot_value[3] >= 2048 + 1500)
	{
		currentStkV[1] = 1;
	}
	else
	{
		currentStkV[1] = 0;
	}

	if (currentStkV[1] != prevStkV[1])
	{
		switch (currentStkV[1])
		{
		case -1:
			SEGGER_RTT_printf(0, "ENTER\r\n");
			if (keymapID == 0)
			{
				setKeys(keymaps_default_stk[1][3]);
			}
			else
			{
				setKeys(keymaps_layout2_stk[1][3]);
			}
			break;
		case 1:
			SEGGER_RTT_printf(0, "RCONTROL\r\n");
			if (keymapID == 0)
			{
				setKeys(keymaps_default_stk[1][2]);
			}
			else
			{
				setKeys(keymaps_layout2_stk[1][2]);
			}
			break;
		default:
			SEGGER_RTT_printf(0, "stk1V: Neutral\r\n");
			if (prevStkV[1] == -1)
			{
				if (keymapID == 0)
				{
					clearKeys(keymaps_default_stk[1][3]);
				}
				else
				{
					clearKeys(keymaps_layout2_stk[1][3]);
				}
				resetKeys();
				countReturnNeutral = MAX_COUNT_RETURN_NEUTRAL;
			}
			else if (prevStkV[1] == 1)
			{
				if (keymapID == 0)
				{
					clearKeys(keymaps_default_stk[1][2]);
				}
				else
				{
					clearKeys(keymaps_layout2_stk[1][2]);
				}
				resetKeys();
				countReturnNeutral = MAX_COUNT_RETURN_NEUTRAL;
			}
			break;
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
		HAL_GPIO_WritePin(HC164_CLK_GPIO_Port, HC164_CLK_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(HC164_CLK_GPIO_Port, HC164_CLK_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(HC164_A_GPIO_Port, HC164_A_Pin, GPIO_PIN_SET);

		HAL_GPIO_WritePin(HC165_SL_GPIO_Port, HC165_SL_Pin, GPIO_PIN_RESET);
		HAL_Delay(1);
		HAL_GPIO_WritePin(HC165_SL_GPIO_Port, HC165_SL_Pin, GPIO_PIN_SET);

		controlJoySticks();

		for (int j = 0; j < 16; j++)
		{
			uint8_t jj = 255;
			if (j < 8)
			{
				jj = j + 2;
			}
			else if (j >= 14 && j < 16)
			{
				jj = j - 14;
			}

			if (i == 3 && (jj == 9 || jj == 6 || jj == 4))
			{
				HAL_GPIO_WritePin(HC165_CLK_GPIO_Port, HC165_CLK_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(HC165_CLK_GPIO_Port, HC165_CLK_Pin, GPIO_PIN_RESET);

				continue;
			}

			if (jj < MATRIX_COLUMNS)
			{
				if (HAL_GPIO_ReadPin(HC165_QH_GPIO_Port, HC165_QH_Pin))
				{
					if (countReturnNeutral > 0)
					{
						countReturnNeutral--;

						break;
					}

					keyState[i] &= ~((uint16_t)1 << jj);

					if (((keyState[i] >> jj) & 0x0001) != ((prevKeyState[i] >> jj) & 0x0001))
					{
						SEGGER_RTT_printf(0, "c::0x%X\r\n", keyState[i]);
						SEGGER_RTT_printf(0, "p::0x%X\r\n", prevKeyState[i]);

						uint8_t keycode = getKeyCode(keymapID, i, (MATRIX_COLUMNS - 1) - jj);
						SEGGER_RTT_printf(0, "%d %d %02X\r\n", i, jj, keycode);

						if (isUpper && keycode == SC_UPPER)
						{
							isUpper = false;
							resetKeys();
						}
						else
						{
							if (isUpper)
							{
								keycode = getUpperKeyCode(keymapID, i, (MATRIX_COLUMNS - 1) - jj);
								//clearKeys(SC_LSHIFT);
							}
							SEGGER_RTT_printf(0, "%d %d %d %02X ", isUpper, i, jj, keycode);
							clearKeys(keycode);
						}
					}
				}
				else
				{
					if (countReturnNeutral > 0)
					{
						countReturnNeutral--;

						break;
					}

					keyState[i] |= ((uint16_t)1 << jj);

					uint8_t keycode = getKeyCode(keymapID, i, (MATRIX_COLUMNS - 1) - jj);
					if (keycode == SC_UPPER)
					{
						isUpper = true;
					}
					else
					{
						if (isUpper)
						{
							keycode = getUpperKeyCode(keymapID, i, (MATRIX_COLUMNS - 1) - jj);
							//setKeys(SC_LSHIFT);
						}
						SEGGER_RTT_printf(0, "%d %d %d %02X ", isUpper, i, jj, keycode);
						setKeys(keycode);
					}
				}
			}

			HAL_GPIO_WritePin(HC165_CLK_GPIO_Port, HC165_CLK_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(HC165_CLK_GPIO_Port, HC165_CLK_Pin, GPIO_PIN_RESET);
		}

		i++;
		break;
	case 4:
		//SEGGER_RTT_printf(0, "pot_val = [%d, %d, %d, %d]\r\n", pot_value[2], pot_value[0], pot_value[4], pot_value[3]);

		for (int k = 0; k < MATRIX_ROWS; k++)
		{
			if (keyState[k] != 0x0 || (keyState[k] == 0x0 && keyState[k] != prevKeyState[k]) ||
					currentStkH[0] != prevStkH[0] ||
					currentStkV[0] != prevStkV[0] ||
					currentStkH[1] != prevStkH[1] ||
					currentStkV[1] != prevStkV[1])
			{
				if (!tud_hid_ready())
					return;

				SEGGER_RTT_printf(0, "modifiers = %d, key = %d\r\n", keyboardHID.modifiers, keyboardHID.key);
				tud_hid_keyboard_report(REPORT_ID_KEYBOARD, keyboardHID.modifiers, keyboardHID.key);
				break;
			}
		}

		for (int k = 0; k < MATRIX_ROWS; k++)
		{
			prevKeyState[k] = keyState[k];
		}

		prevStkH[0] = currentStkH[0];
		prevStkV[0] = currentStkV[0];

		prevStkH[1] = currentStkH[1];
		prevStkV[1] = currentStkV[1];

		i = 0;
		break;
	default:
		i = 0;
		break;
	}
}

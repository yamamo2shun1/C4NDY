/*
 * keyboard.c
 *
 *  Created on: Aug 17, 2023
 *      Author: shun
 */

#include "main.h"
#include "keyboard.h"
#include "audio_control.h"

struct keyboardHID_t
{
    uint8_t modifiers;
    uint8_t reserved;
    uint8_t key[6];
} keyboardHID;

uint16_t keyState[MATRIX_ROWS]     = {0x0};
uint16_t prevKeyState[MATRIX_ROWS] = {0x0};

bool isSwitchDFUMode      = false;
uint32_t longPressCounter = 0;

bool isKeymapIDChanged = false;
uint8_t keymapID       = 0;

bool isLinePhonoSWChanged = false;
uint8_t linePhonoSW       = 0;

const uint8_t keymaps_normal_default[2][MATRIX_ROWS][MATRIX_COLUMNS] = {
    // clang-format off
	{
        {KC_ESC,      KC_1,    KC_2,      KC_3,    KC_4,  KC_5,      KC_6,     KC_7,     KC_8,        KC_9,      KC_0,     KC_MINUS,  KC_EQUAL},
        {KC_TAB,      KC_Q,    KC_W,      KC_E,    KC_R,  KC_T,      KC_Y,     KC_U,     KC_I,        KC_O,      KC_P,     KC_LSB,    KC_RSB  },
        {KC_LCONTROL, KC_A,    KC_S,      KC_D,    KC_F,  KC_G,      KC_H,     KC_J,     KC_K,        KC_L,      KC_SC,    KC_APS,    KC_YEN  },
        {KC_LSHIFT,   KC_Z,    KC_X,      KC_C,    KC_V,  KC_B,      KC_N,     KC_M,     KC_COMMA,    KC_PERIOD, KC_SLASH, KC_RSHIFT, KC_GA   },
        {KC_LGUI,     KC_LNPH, KC_LAYOUT, KC_LALT, KC_BS, KC_DELETE, KC_ENTER, KC_SPACE, KC_CAPSLOCK, KC_LEFT,   KC_DOWN,  KC_UP,     KC_RIGHT}
    },
    {
        {KC_ESC,      KC_1,    KC_2,      KC_3,    KC_4,  KC_5,      KC_6,     KC_7,     KC_8,        KC_9,    KC_0,    KC_LSB,    KC_RSB   },
        {KC_TAB,      KC_APS,  KC_COMMA,  KC_O,    KC_U,  KC_Y,      KC_F,     KC_G,     KC_C,        KC_R,    KC_L,    KC_SLASH,  KC_EQUAL },
        {KC_LCONTROL, KC_P,    KC_I,      KC_E,    KC_A,  KC_PERIOD, KC_D,     KC_S,     KC_T,        KC_H,    KC_Z,    KC_MINUS,  KC_BSLASH},
        {KC_LSHIFT,   KC_J,    KC_Q,      KC_SC,   KC_K,  KC_X,      KC_B,     KC_M,     KC_W,        KC_N,    KC_V,    KC_RSHIFT, KC_GA    },
        {KC_LGUI,     KC_LNPH, KC_LAYOUT, KC_LALT, KC_BS, KC_DELETE, KC_ENTER, KC_SPACE, KC_CAPSLOCK, KC_LEFT, KC_DOWN, KC_UP,     KC_RIGHT }
    }
    // clang-format on
};

uint8_t keymaps_normal[2][MATRIX_ROWS][MATRIX_COLUMNS] = {
    // clang-format off
    {
        {KC_ESC,      KC_1,    KC_2,      KC_3,    KC_4,  KC_5,      KC_6,     KC_7,     KC_8,        KC_9,      KC_0,     KC_MINUS,  KC_EQUAL},
        {KC_TAB,      KC_Q,    KC_W,      KC_E,    KC_R,  KC_T,      KC_Y,     KC_U,     KC_I,        KC_O,      KC_P,     KC_LSB,    KC_RSB  },
        {KC_LCONTROL, KC_A,    KC_S,      KC_D,    KC_F,  KC_G,      KC_H,     KC_J,     KC_K,        KC_L,      KC_SC,    KC_APS,    KC_YEN  },
        {KC_LSHIFT,   KC_Z,    KC_X,      KC_C,    KC_V,  KC_B,      KC_N,     KC_M,     KC_COMMA,    KC_PERIOD, KC_SLASH, KC_RSHIFT, KC_GA   },
        {KC_LGUI,     KC_LNPH, KC_LAYOUT, KC_LALT, KC_BS, KC_DELETE, KC_ENTER, KC_SPACE, KC_CAPSLOCK, KC_LEFT,   KC_DOWN,  KC_UP,     KC_RIGHT}
    },
    {
        {KC_ESC,      KC_1,    KC_2,      KC_3,    KC_4,  KC_5,      KC_6,     KC_7,     KC_8,        KC_9,    KC_0,    KC_LSB,    KC_RSB   },
        {KC_TAB,      KC_APS,  KC_COMMA,  KC_O,    KC_U,  KC_Y,      KC_F,     KC_G,     KC_C,        KC_R,    KC_L,    KC_SLASH,  KC_EQUAL },
        {KC_LCONTROL, KC_P,    KC_I,      KC_E,    KC_A,  KC_PERIOD, KC_D,     KC_S,     KC_T,        KC_H,    KC_Z,    KC_MINUS,  KC_BSLASH},
        {KC_LSHIFT,   KC_J,    KC_Q,      KC_SC,   KC_K,  KC_X,      KC_B,     KC_M,     KC_W,        KC_N,    KC_V,    KC_RSHIFT, KC_GA    },
        {KC_LGUI,     KC_LNPH, KC_LAYOUT, KC_LALT, KC_BS, KC_DELETE, KC_ENTER, KC_SPACE, KC_CAPSLOCK, KC_LEFT, KC_DOWN, KC_UP,     KC_RIGHT }
    }
    // clang-format on
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

void setKeymapID(uint8_t val)
{
    keymapID = val;
    if (keymapID == 0)
    {
        HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_SET);
    }
}

uint8_t getKeymapID(void)
{
    return keymapID;
}

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

void factoryReset(void)
{
    HAL_FLASH_Unlock();

    erase_flash_data();

    setKeymapID(0);
    setLinePhonoSW(0);
    write_flash_data(0, 0);
    write_flash_data(1, 0);

    for (int i = 0; i < MATRIX_ROWS; i++)
    {
        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            write_flash_data(BASIC_PARAMS_NUM + 0 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, keymaps_normal_default[0][i][j]);
            write_flash_data(BASIC_PARAMS_NUM + 1 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, keymaps_normal_default[1][i][j]);
        }
    }

    HAL_FLASH_Lock();
}

void writeAllKeyboardSettings(void)
{
    HAL_FLASH_Unlock();

    erase_flash_data();

    setKeymapID(keymapID);
    setLinePhonoSW(linePhonoSW);
    write_flash_data(0, 0);
    write_flash_data(1, linePhonoSW);
    write_flash_data(2, keymapID);

    for (int i = 0; i < MATRIX_ROWS; i++)
    {
        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            write_flash_data(BASIC_PARAMS_NUM + 0 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, getKeyCode(0, i, j));
            write_flash_data(BASIC_PARAMS_NUM + 1 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, getKeyCode(1, i, j));
        }
    }

    HAL_FLASH_Lock();
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
        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            rbuf[j] = keymaps_normal[keymapID][buffer[0] - 0xF0][j];
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
            keymaps_normal[0][buffer[0] - 0xF0][j] = buffer[j + 2];
        }
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x09)
    {
        SEGGER_RTT_printf(0, "write to layout2:\n");

        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            keymaps_normal[1][buffer[0] - 0xF0][j] = buffer[j + 2];
        }
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x11)
    {
        SEGGER_RTT_printf(0, "read from layout1:\n");
        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            rbuf[j] = keymaps_normal[0][buffer[0] - 0xF0][j];
        }

        tud_hid_n_report(1, 0, rbuf, 16);
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x19)
    {
        SEGGER_RTT_printf(0, "read from layout2:\n");
        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            rbuf[j] = keymaps_normal[1][buffer[0] - 0xF0][j];
        }

        tud_hid_n_report(1, 0, rbuf, 16);
    }
    else if (buffer[0] == 0xF5)
    {
        SEGGER_RTT_printf(0, "erase & write FLASH...\n");

        writeAllKeyboardSettings();

        uint8_t rbuf[16] = {0x00};
        rbuf[1]          = 0xF5;
        rbuf[2]          = 0x01;
        tud_hid_n_report(1, 0, rbuf, 16);
    }
    else if (buffer[0] == 0xF6)
    {
        SEGGER_RTT_printf(0, "reboot...\n");

        uint8_t rbuf[16] = {0x00};
        rbuf[1]          = 0xF6;
        rbuf[2]          = 0x01;
        tud_hid_n_report(ITF_NUM_HID_GIO, 0, rbuf, 16);

        setBootDfuFlag(false);
        HAL_Delay(100);
        NVIC_SystemReset();
    }
    else if (buffer[0] == 0xF8)
    {
        SEGGER_RTT_printf(0, "factory reset...\n");

        factoryReset();

        uint8_t rbuf[16] = {0x00};
        rbuf[1]          = 0xF8;
        rbuf[2]          = 0x01;
        tud_hid_n_report(ITF_NUM_HID_GIO, 0, rbuf, 16);
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
    return keymaps_normal[keymapId][x][y];
}

void setKeyCode(uint8_t keymapId, uint8_t x, uint8_t y, uint8_t code)
{
    keymaps_normal[keymapId][x][y] = code;
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
    if (code == KC_LAYOUT)
    {
        isKeymapIDChanged = false;
    }
    else if (code == KC_LNPH)
    {
        isLinePhonoSWChanged = false;
    }
    else if (code >= KC_LCONTROL && code <= KC_RGUI)
    {
        keyboardHID.modifiers &= ~(1 << (code - KC_LCONTROL));
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
    if (code == KC_LAYOUT)
    {
        if (!isKeymapIDChanged)
        {
            setKeymapID(!keymapID);
            writeAllKeyboardSettings();

            isKeymapIDChanged = true;
        }
    }
    else if (code == KC_LNPH)
    {
        if (!isLinePhonoSWChanged)
        {
            setLinePhonoSW(!linePhonoSW);
            writeAllKeyboardSettings();

            isLinePhonoSWChanged = true;
        }
    }
    else if (code >= KC_LCONTROL && code <= KC_RGUI)
    {
        keyboardHID.modifiers |= 1 << (code - KC_LCONTROL);
    }
    else
    {
        for (int k = 0; k < 6; k++)
        {
            if (keyboardHID.key[k] == code)
            {
                // LGUI + ESC 長押しでDFUモーでリセット
                if (keyboardHID.modifiers == 0x08 && keyboardHID.key[k] == KC_ESC)
                {
                    longPressCounter++;
                    if (longPressCounter == 5000)
                    {
                        setBootDfuFlag(true);
                        SEGGER_RTT_printf(0, "Reboot in custom DFU...\n");
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
        HAL_Delay(1);
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
                    keyState[i] &= ~((uint16_t) 1 << jj);

                    if (keyState[i] != prevKeyState[i])
                    {
                        uint8_t keycode = getKeyCode(keymapID, i, (MATRIX_COLUMNS - 1) - jj);
                        clearKeys(keycode);
                    }
                }
                else
                {
                    keyState[i] |= ((uint16_t) 1 << jj);

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

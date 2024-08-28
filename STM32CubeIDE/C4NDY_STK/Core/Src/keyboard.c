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
#include "icled.h"

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

bool isMasterGainChanged = false;

bool isUpper = false;

uint8_t countReturnNeutral = 0;
#define MAX_COUNT_RETURN_NEUTRAL 60

int8_t currentStk[2][2] = {0};
int8_t prevStk[2][2]    = {0};

uint8_t keymaps_normal[2][MATRIX_ROWS][MATRIX_COLUMNS] = {
    // clang-format off
    {
	    {SC_Q,    SC_W,    SC_E,      SC_R,    SC_T,      SC_Y,    SC_U,      SC_I,        SC_O,      SC_P},
		{SC_A,    SC_S,    SC_D,      SC_F,    SC_G,      SC_H,    SC_J,      SC_K,        SC_L,      SC_SC},
		{SC_Z,    SC_X,    SC_C,      SC_V,    SC_B,      SC_N,    SC_M,      SC_COMMA,    SC_PERIOD, SC_SLASH},
		{SC_NULL, SC_LNPH, SC_LAYOUT, SC_NULL, SC_LSHIFT, SC_NULL, SC_LEFT,   SC_DOWN,     SC_UP,     SC_RIGHT}
	},
	{
		{SC_APS,  SC_COMMA, SC_O,      SC_U,    SC_Y,      SC_F,    SC_G,    SC_C,    SC_R,  SC_L},
		{SC_P,    SC_I,     SC_E,      SC_A,    SC_PERIOD, SC_D,    SC_S,    SC_T,    SC_H,  SC_Z},
		{SC_J,    SC_Q,     SC_SC,     SC_K,    SC_X,      SC_B,    SC_M,    SC_W,    SC_N,  SC_V},
		{SC_NULL, SC_LNPH,  SC_LAYOUT, SC_NULL, SC_LSHIFT, SC_NULL, SC_LEFT, SC_DOWN, SC_UP, SC_RIGHT}
	}
    // clang-format on
};

uint8_t keymaps_upper[2][MATRIX_ROWS][MATRIX_COLUMNS] = {
    // clang-format off
	{
		{SC_1,    SC_2,        SC_3,      SC_4,    SC_5,      SC_6,    SC_7,     SC_8,     SC_9,    SC_0},
		{SC_Q,    SC_W,        SC_NULL,   SC_LGUI, SC_G,      SC_NULL, SC_MINUS, SC_EQUAL, SC_LSB,  SC_APS},
		{SC_NULL, SC_RCONTROL, SC_NULL,   SC_NULL, SC_NULL,   SC_NULL, SC_RSB,   SC_NULL,  SC_NULL, SC_YEN},
		{SC_NULL, SC_LNPH,     SC_LAYOUT, SC_NULL, SC_LSHIFT, SC_NULL, SC_NULL,  SC_NULL,  SC_NULL, SC_GA}
	},
	{
		{SC_1,    SC_2,        SC_3,      SC_4,    SC_5,      SC_6,    SC_7,     SC_8,    SC_9,     SC_0},
		{SC_APS,  SC_COMMA,    SC_NULL,   SC_LGUI, SC_PERIOD, SC_NULL, SC_LSB,   SC_RSB,  SC_SLASH, SC_MINUS},
		{SC_NULL, SC_CAPSLOCK, SC_NULL,   SC_NULL, SC_NULL,   SC_NULL, SC_EQUAL, SC_NULL, SC_NULL,  SC_BSLASH},
		{SC_NULL, SC_LNPH,     SC_LAYOUT, SC_NULL, SC_LSHIFT, SC_NULL, SC_NULL,  SC_NULL, SC_NULL,  SC_GA}
	}
    // clang-format on
};

uint8_t keymaps_stk[2][2][4] = {
    // clang-format off
    //   left,   right,     down,        up
	{
		{SC_BS,  SC_TAB,    SC_CAPSLOCK, SC_UPPER}, // left stick
		{SC_ESC, SC_HENKAN, SC_SPACE,    SC_RALT}   // right stick
	},
	{
		{SC_BS,  SC_TAB,   SC_LCONTROL, SC_UPPER}, // left stick
		{SC_ESC, SC_SPACE, SC_ENTER,    SC_RALT}   // right stick
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
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x02)
    {
        SEGGER_RTT_printf(0, "write to layout1 upper:\n");

        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            keymaps_upper[0][buffer[0] - 0xF0][j] = buffer[j + 2];
        }
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x03)
    {
        SEGGER_RTT_printf(0, "write to layout1 stick:\n");

        for (int j = 0; j < 4; j++)
        {
            keymaps_stk[0][buffer[0] - 0xF0][j] = buffer[j + 2];
        }
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x04)
    {
        SEGGER_RTT_printf(0, "write to layout2:\n");

        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            keymaps_normal[1][buffer[0] - 0xF0][j] = buffer[j + 2];
        }
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x05)
    {
        SEGGER_RTT_printf(0, "write to layout2 upper:\n");

        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            keymaps_upper[1][buffer[0] - 0xF0][j] = buffer[j + 2];
        }
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x06)
    {
        SEGGER_RTT_printf(0, "write to layout2 stick:\n");

        for (int j = 0; j < 4; j++)
        {
            keymaps_stk[1][buffer[0] - 0xF0][j] = buffer[j + 2];
        }
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x07)
    {
        SEGGER_RTT_printf(0, "read from layout1:\n");
        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            rbuf[j] = keymaps_normal[0][buffer[0] - 0xF0][j];
        }

        tud_hid_n_report(1, 0, rbuf, 16);
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x08)
    {
        SEGGER_RTT_printf(0, "read from layout1 upper:\n");
        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            rbuf[j] = keymaps_upper[0][buffer[0] - 0xF0][j];
        }

        tud_hid_n_report(1, 0, rbuf, 16);
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x09)
    {
        SEGGER_RTT_printf(0, "read from layout1 stick:\n");
        for (int j = 0; j < 4; j++)
        {
            rbuf[j] = keymaps_stk[0][buffer[0] - 0xF0][j];
        }

        tud_hid_n_report(1, 0, rbuf, 16);
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x0A)
    {
        SEGGER_RTT_printf(0, "read from layout2:\n");
        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            rbuf[j] = keymaps_normal[1][buffer[0] - 0xF0][j];
        }

        tud_hid_n_report(1, 0, rbuf, 16);
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x0B)
    {
        SEGGER_RTT_printf(0, "read from layout2 upper:\n");
        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            rbuf[j] = keymaps_upper[1][buffer[0] - 0xF0][j];
        }

        tud_hid_n_report(1, 0, rbuf, 16);
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x0C)
    {
        SEGGER_RTT_printf(0, "read from layout2 stick:\n");
        for (int j = 0; j < 4; j++)
        {
            rbuf[j] = keymaps_stk[1][buffer[0] - 0xF0][j];
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
        rbuf[1]          = 0xF5;
        rbuf[2]          = 0x01;
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
    return keymaps_normal[keymapId][x][y];
}

uint8_t getUpperKeyCode(uint8_t keymapId, uint8_t x, uint8_t y)
{
    return keymaps_upper[keymapId][x][y];
}

uint8_t getStickKeyCode(uint8_t keymapId, uint8_t id, uint8_t direction)
{
    return keymaps_stk[keymapId][id][direction];
}

void setKeyCode(uint8_t keymapId, uint8_t x, uint8_t y, uint8_t code)
{
    keymaps_normal[keymapId][x][y] = code;
}

void setUpperKeyCode(uint8_t keymapId, uint8_t x, uint8_t y, uint8_t code)
{
    keymaps_upper[keymapId][x][y] = code;
}

void setStickKeyCode(uint8_t keymapId, uint8_t id, uint8_t direction, uint8_t code)
{
    keymaps_stk[keymapId][id][direction] = code;
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
    else if (code == SC_MGAIN_UP || code == SC_MGAIN_DOWN)
    {
        isMasterGainChanged = false;
    }
    else if (code == SC_UPPER)
    {
        if (isUpper)
        {
            isUpper = false;

            if (((keyboardHID.modifiers >> (SC_LSHIFT - SC_LCONTROL)) & 0x01) ||
                ((keyboardHID.modifiers >> (SC_RSHIFT - SC_LCONTROL)) & 0x01))
            {
                setAllLedBuf(0x67, 0x10, 0x70);
            }
            else
            {
                setAllLedBuf(0xFE, 0x01, 0x9A);
            }
        }
    }
    else if (code >= SC_LCONTROL && code <= SC_RGUI)
    {
        keyboardHID.modifiers &= ~(1 << (code - SC_LCONTROL));

        if (code == SC_LSHIFT || code == SC_RSHIFT)
        {
            if (isUpper)
            {
                setAllLedBuf(0xFF, 0xFF, 0xFF);
            }
            else
            {
                setAllLedBuf(0xFE, 0x01, 0x9A);
            }
        }
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
    static int master_gain = 0;

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
    else if (code == SC_MGAIN_UP)
    {
        if (!isMasterGainChanged && master_gain < MASTER_GAIN_MAX)
        {
            master_gain += 1;
            if (master_gain >= MASTER_GAIN_MAX)
            {
                master_gain = MASTER_GAIN_MAX;
            }
            send_master_gain_db(master_gain);

            isMasterGainChanged = true;
        }
    }
    else if (code == SC_MGAIN_DOWN)
    {
        if (!isMasterGainChanged && master_gain > MASTER_GAIN_MIN)
        {
            master_gain -= 1;
            if (master_gain <= MASTER_GAIN_MIN)
            {
                master_gain = MASTER_GAIN_MIN;
            }
            send_master_gain_db(master_gain);

            isMasterGainChanged = true;
        }
    }
    else if (code == SC_UPPER)
    {
        if (!isUpper)
        {
            isUpper = true;

            setAllLedBuf(0xFF, 0xFF, 0xFF);
        }
    }
    else if (code >= SC_LCONTROL && code <= SC_RGUI)
    {
        if (!((keyboardHID.modifiers >> (SC_LSHIFT - SC_LCONTROL)) & 0x01) &&
            !((keyboardHID.modifiers >> (SC_RSHIFT - SC_LCONTROL)) & 0x01))
        {
            if (code == SC_LSHIFT || code == SC_RSHIFT)
            {
                setAllLedBuf(0x67, 0x10, 0x70);
            }
        }

        keyboardHID.modifiers |= 1 << (code - SC_LCONTROL);
    }
    else
    {
        for (int k = 0; k < 6; k++)
        {
            if (keyboardHID.key[k] == code)
            {
                // RESET or LGUI + ESC 長押しでDFUモーでリセット
                if (keyboardHID.key[k] == SC_RESET || (keyboardHID.modifiers == 0x08 && keyboardHID.key[k] == SC_ESC))
                {
                    longPressCounter++;
                    SEGGER_RTT_printf(0, "longPressCounter = %d\n", longPressCounter);
                    if (longPressCounter == MAX_LONG_PRESS_COUNT)
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

void controlJoySticks()
{
    for (int i = 1; i < 5; i++)
    {
        int hv = (i == 1 || i == 3) ? H : V;
        int id = (i == 1 || i == 3) ? (i - 1) / 2 : (i - 2) / 2;

        if (pot_value[i] < JOYSTICK_CENTER - JOYSTICK_ON_THRESHOLD)
        {
            currentStk[hv][id] = 1;
        }
        else if (pot_value[i] >= JOYSTICK_CENTER + JOYSTICK_ON_THRESHOLD)
        {
            currentStk[hv][id] = -1;
        }
        else
        {
            currentStk[hv][id] = 0;
        }
    }

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            if (currentStk[j][i] != prevStk[j][i])
            {
                if (currentStk[j][i] == -1 || currentStk[j][i] == 1)
                {
                    int8_t direction = (j == 0) ? ((currentStk[j][i] + 1) / 2) : ((5 - currentStk[j][i]) / 2);
                    setKeys(keymaps_stk[keymapID][i][direction]);
                }
                else if (prevStk[j][i] == -1 || prevStk[j][i] == 1)
                {
                    int8_t direction = (j == 0) ? ((prevStk[j][i] + 1) / 2) : ((5 - prevStk[j][i]) / 2);
                    clearKeys(keymaps_stk[keymapID][i][direction]);
                    resetKeys();
                    countReturnNeutral = MAX_COUNT_RETURN_NEUTRAL;
                }
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

                    keyState[i] &= ~((uint16_t) 1 << jj);

                    if (((keyState[i] >> jj) & 0x0001) != ((prevKeyState[i] >> jj) & 0x0001))
                    {
                        uint8_t keycode = getKeyCode(keymapID, i, (MATRIX_COLUMNS - 1) - jj);

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
                                // clearKeys(SC_LSHIFT);
                            }
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

                    keyState[i] |= ((uint16_t) 1 << jj);

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
                            // setKeys(SC_LSHIFT);
                        }
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
        for (int k = 0; k < MATRIX_ROWS; k++)
        {
            if (keyState[k] != 0x0 || (keyState[k] == 0x0 && keyState[k] != prevKeyState[k]) ||
                currentStk[H][0] != prevStk[H][0] ||
                currentStk[V][0] != prevStk[V][0] ||
                currentStk[H][1] != prevStk[H][1] ||
                currentStk[V][1] != prevStk[V][1])
            {
                if (!tud_hid_ready())
                    return;

                tud_hid_keyboard_report(REPORT_ID_KEYBOARD, keyboardHID.modifiers, keyboardHID.key);
                break;
            }
        }

        for (int k = 0; k < MATRIX_ROWS; k++)
        {
            prevKeyState[k] = keyState[k];
        }

        prevStk[H][0] = currentStk[H][0];
        prevStk[V][0] = currentStk[V][0];

        prevStk[H][1] = currentStk[H][1];
        prevStk[V][1] = currentStk[V][1];

        i = 0;
        break;
    default:
        i = 0;
        break;
    }
}

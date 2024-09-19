/*
 * keyboard.c
 *
 *  Created on: Aug 17, 2023
 *      Author: shun
 */

#include "keyboard.h"

#include "audio_control.h"
#include "adc.h"
#include "icled.h"

#include <math.h>

struct keyboardHID_t
{
    uint8_t modifiers;
    uint8_t reserved;
    uint8_t key[6];
} keyboardHID;

struct mouseHID_t
{
    int8_t x;
    int8_t y;
    uint8_t buttons;
    int8_t vertical;
    int8_t horizontal;
    int8_t vertical_prev;
} mouseHID;

uint16_t keyState[MATRIX_ROWS]     = {0x0};
uint16_t prevKeyState[MATRIX_ROWS] = {0x0};

bool isSwitchDFUMode      = false;
uint32_t longPressCounter = 0;

bool isKeymapIDChanged = false;
uint8_t keymapID       = 0;

bool isLinePhonoSWChanged = false;
uint8_t linePhonoSW       = 0;

bool isMasterGainChanged = false;

bool isUpper   = false;
bool isShift   = false;
bool isClicked = false;
bool isWheel   = false;

uint8_t countReturnNeutral = 0;
#define MAX_COUNT_RETURN_NEUTRAL 60

int8_t currentStk[JOYSTICK_NUMS][JOYSTICK_AXIS] = {0};
int8_t prevStk[JOYSTICK_NUMS][JOYSTICK_AXIS]    = {0};

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
		{SC_1,    SC_2,        SC_3,      SC_4,      SC_5,       SC_6,    SC_7,     SC_8,     SC_9,    SC_0},
		{SC_Q,    SC_W,        SC_NULL,   SC_LGUI,   SC_G,       SC_NULL, SC_MINUS, SC_EQUAL, SC_LSB,  SC_APS},
		{SC_NULL, SC_RCONTROL, SC_M_LBTN, SC_M_RBTN, SC_M_WHEEL, SC_NULL, SC_RSB,   SC_NULL,  SC_NULL, SC_YEN},
		{SC_NULL, SC_LNPH,     SC_LAYOUT, SC_NULL,   SC_LSHIFT,  SC_NULL, SC_NULL,  SC_NULL,  SC_NULL, SC_GA}
	},
	{
		{SC_1,    SC_2,        SC_3,      SC_4,      SC_5,       SC_6,    SC_7,     SC_8,    SC_9,     SC_0},
		{SC_APS,  SC_COMMA,    SC_NULL,   SC_LGUI,   SC_PERIOD,  SC_NULL, SC_LSB,   SC_RSB,  SC_SLASH, SC_MINUS},
		{SC_NULL, SC_CAPSLOCK, SC_M_LBTN, SC_M_RBTN, SC_M_WHEEL, SC_NULL, SC_EQUAL, SC_NULL, SC_NULL,  SC_BSLASH},
		{SC_NULL, SC_LNPH,     SC_LAYOUT, SC_NULL,   SC_LSHIFT,  SC_NULL, SC_NULL,  SC_NULL, SC_NULL,  SC_GA}
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

void writeAllKeyboardSettings(void)
{
    HAL_FLASH_Unlock();

    erase_flash_data();

    write_flash_data(0, 0);
    write_flash_data(1, linePhonoSW);

    for (int i = 0; i < MATRIX_ROWS; i++)
    {
        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            write_flash_data(BASIC_PARAMS_NUM + 0 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, getKeyCode(0, i, j));
            write_flash_data(BASIC_PARAMS_NUM + 1 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, getKeyCode(1, i, j));

            write_flash_data(BASIC_PARAMS_NUM + (2 * MATRIX_ROWS * MATRIX_COLUMNS) + 0 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, getUpperKeyCode(0, i, j));
            write_flash_data(BASIC_PARAMS_NUM + (2 * MATRIX_ROWS * MATRIX_COLUMNS) + 1 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, getUpperKeyCode(1, i, j));
        }
    }

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            write_flash_data(BASIC_PARAMS_NUM + (4 * MATRIX_ROWS * MATRIX_COLUMNS) + 0 * (2 * 4) + i * 4 + j, getStickKeyCode(0, i, j));
            write_flash_data(BASIC_PARAMS_NUM + (4 * MATRIX_ROWS * MATRIX_COLUMNS) + 1 * (2 * 4) + i * 4 + j, getStickKeyCode(1, i, j));
        }
    }

    write_flash_data(BASIC_PARAMS_NUM + (4 * MATRIX_ROWS * MATRIX_COLUMNS) + (2 * 2 * 4) + 0, getNormalColor(0)->r);
    write_flash_data(BASIC_PARAMS_NUM + (4 * MATRIX_ROWS * MATRIX_COLUMNS) + (2 * 2 * 4) + 1, getNormalColor(0)->g);
    write_flash_data(BASIC_PARAMS_NUM + (4 * MATRIX_ROWS * MATRIX_COLUMNS) + (2 * 2 * 4) + 2, getNormalColor(0)->b);
    write_flash_data(BASIC_PARAMS_NUM + (4 * MATRIX_ROWS * MATRIX_COLUMNS) + (2 * 2 * 4) + 3, getUpperColor(0)->r);
    write_flash_data(BASIC_PARAMS_NUM + (4 * MATRIX_ROWS * MATRIX_COLUMNS) + (2 * 2 * 4) + 4, getUpperColor(0)->g);
    write_flash_data(BASIC_PARAMS_NUM + (4 * MATRIX_ROWS * MATRIX_COLUMNS) + (2 * 2 * 4) + 5, getUpperColor(0)->b);
    write_flash_data(BASIC_PARAMS_NUM + (4 * MATRIX_ROWS * MATRIX_COLUMNS) + (2 * 2 * 4) + 6, getShiftColor(0)->r);
    write_flash_data(BASIC_PARAMS_NUM + (4 * MATRIX_ROWS * MATRIX_COLUMNS) + (2 * 2 * 4) + 7, getShiftColor(0)->g);
    write_flash_data(BASIC_PARAMS_NUM + (4 * MATRIX_ROWS * MATRIX_COLUMNS) + (2 * 2 * 4) + 8, getShiftColor(0)->b);
    write_flash_data(BASIC_PARAMS_NUM + (4 * MATRIX_ROWS * MATRIX_COLUMNS) + (2 * 2 * 4) + 9, getNormalColor(1)->r);
    write_flash_data(BASIC_PARAMS_NUM + (4 * MATRIX_ROWS * MATRIX_COLUMNS) + (2 * 2 * 4) + 10, getNormalColor(1)->g);
    write_flash_data(BASIC_PARAMS_NUM + (4 * MATRIX_ROWS * MATRIX_COLUMNS) + (2 * 2 * 4) + 11, getNormalColor(1)->b);
    write_flash_data(BASIC_PARAMS_NUM + (4 * MATRIX_ROWS * MATRIX_COLUMNS) + (2 * 2 * 4) + 12, getUpperColor(1)->r);
    write_flash_data(BASIC_PARAMS_NUM + (4 * MATRIX_ROWS * MATRIX_COLUMNS) + (2 * 2 * 4) + 13, getUpperColor(1)->g);
    write_flash_data(BASIC_PARAMS_NUM + (4 * MATRIX_ROWS * MATRIX_COLUMNS) + (2 * 2 * 4) + 14, getUpperColor(1)->b);
    write_flash_data(BASIC_PARAMS_NUM + (4 * MATRIX_ROWS * MATRIX_COLUMNS) + (2 * 2 * 4) + 15, getShiftColor(1)->r);
    write_flash_data(BASIC_PARAMS_NUM + (4 * MATRIX_ROWS * MATRIX_COLUMNS) + (2 * 2 * 4) + 16, getShiftColor(1)->g);
    write_flash_data(BASIC_PARAMS_NUM + (4 * MATRIX_ROWS * MATRIX_COLUMNS) + (2 * 2 * 4) + 17, getShiftColor(1)->b);

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
        SEGGER_RTT_printf(0, "buf[%d] = %d(%02X)\n", i, buffer[i], buffer[i]);
    }
    SEGGER_RTT_printf(0, "bufsize = %d\n", bufsize);

    uint8_t rbuf[16] = {0x00};
    if (buffer[0] >= 0xF0 && buffer[0] <= 0xF3 && buffer[1] == 0x00)
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
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF3 && buffer[1] == 0x01)
    {
        SEGGER_RTT_printf(0, "write to layout1:\n");

        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            keymaps_normal[0][buffer[0] - 0xF0][j] = buffer[j + 2];
        }
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF3 && buffer[1] == 0x02)
    {
        SEGGER_RTT_printf(0, "write to layout1 upper:\n");

        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            keymaps_upper[0][buffer[0] - 0xF0][j] = buffer[j + 2];
        }
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF1 && buffer[1] == 0x03)
    {
        SEGGER_RTT_printf(0, "write to layout1 stick:\n");

        for (int j = 0; j < 4; j++)
        {
            keymaps_stk[0][buffer[0] - 0xF0][j] = buffer[j + 2];
        }
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF2 && buffer[1] == 0x04)
    {
        SEGGER_RTT_printf(0, "write to layout1 led:\n");

        switch (buffer[0])
        {
        case 0xF0:
            setNormalColor(0, buffer[2], buffer[3], buffer[4]);
            break;
        case 0xF1:
            setUpperColor(0, buffer[2], buffer[3], buffer[4]);
            break;
        case 0xF2:
            setShiftColor(0, buffer[2], buffer[3], buffer[4]);
            break;
        }
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF3 && buffer[1] == 0x09)
    {
        SEGGER_RTT_printf(0, "write to layout2:\n");

        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            keymaps_normal[1][buffer[0] - 0xF0][j] = buffer[j + 2];
        }
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF3 && buffer[1] == 0x0A)
    {
        SEGGER_RTT_printf(0, "write to layout2 upper:\n");

        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            keymaps_upper[1][buffer[0] - 0xF0][j] = buffer[j + 2];
        }
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF1 && buffer[1] == 0x0B)
    {
        SEGGER_RTT_printf(0, "write to layout2 stick:\n");

        for (int j = 0; j < 4; j++)
        {
            keymaps_stk[1][buffer[0] - 0xF0][j] = buffer[j + 2];
        }
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF2 && buffer[1] == 0x0C)
    {
        SEGGER_RTT_printf(0, "write to layout2 led:\n");

        switch (buffer[0])
        {
        case 0xF0:
            setNormalColor(1, buffer[2], buffer[3], buffer[4]);
            break;
        case 0xF1:
            setUpperColor(1, buffer[2], buffer[3], buffer[4]);
            break;
        case 0xF2:
            setShiftColor(1, buffer[2], buffer[3], buffer[4]);
            break;
        }
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF3 && buffer[1] == 0x11)
    {
        SEGGER_RTT_printf(0, "read from layout1:\n");
        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            rbuf[j] = keymaps_normal[0][buffer[0] - 0xF0][j];
        }

        tud_hid_n_report(ITF_NUM_HID_GIO, 0, rbuf, 16);
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF3 && buffer[1] == 0x12)
    {
        SEGGER_RTT_printf(0, "read from layout1 upper:\n");
        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            rbuf[j] = keymaps_upper[0][buffer[0] - 0xF0][j];
        }

        tud_hid_n_report(ITF_NUM_HID_GIO, 0, rbuf, 16);
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF1 && buffer[1] == 0x13)
    {
        SEGGER_RTT_printf(0, "read from layout1 stick:\n");
        for (int j = 0; j < 4; j++)
        {
            rbuf[j] = keymaps_stk[0][buffer[0] - 0xF0][j];
        }

        tud_hid_n_report(ITF_NUM_HID_GIO, 0, rbuf, 16);
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF2 && buffer[1] == 0x14)
    {
        SEGGER_RTT_printf(0, "read from layout1 led:\n");
        switch (buffer[0])
        {
        case 0xF0:
            RGB_Color_t* rgb_normal = getNormalColor(0);
            rbuf[0]                 = rgb_normal->r;
            rbuf[1]                 = rgb_normal->g;
            rbuf[2]                 = rgb_normal->b;
            break;
        case 0xF1:
            RGB_Color_t* rgb_upper = getUpperColor(0);
            rbuf[0]                = rgb_upper->r;
            rbuf[1]                = rgb_upper->g;
            rbuf[2]                = rgb_upper->b;
            break;
        case 0xF2:
            RGB_Color_t* rgb_shift = getShiftColor(0);
            rbuf[0]                = rgb_shift->r;
            rbuf[1]                = rgb_shift->g;
            rbuf[2]                = rgb_shift->b;
            break;
        }

        tud_hid_n_report(ITF_NUM_HID_GIO, 0, rbuf, 16);
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF3 && buffer[1] == 0x19)
    {
        SEGGER_RTT_printf(0, "read from layout2:\n");
        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            rbuf[j] = keymaps_normal[1][buffer[0] - 0xF0][j];
        }

        tud_hid_n_report(ITF_NUM_HID_GIO, 0, rbuf, 16);
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF3 && buffer[1] == 0x1A)
    {
        SEGGER_RTT_printf(0, "read from layout2 upper:\n");
        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            rbuf[j] = keymaps_upper[1][buffer[0] - 0xF0][j];
        }

        tud_hid_n_report(ITF_NUM_HID_GIO, 0, rbuf, 16);
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF1 && buffer[1] == 0x1B)
    {
        SEGGER_RTT_printf(0, "read from layout2 stick:\n");
        for (int j = 0; j < 4; j++)
        {
            rbuf[j] = keymaps_stk[1][buffer[0] - 0xF0][j];
        }

        tud_hid_n_report(ITF_NUM_HID_GIO, 0, rbuf, 16);
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF2 && buffer[1] == 0x1C)
    {
        SEGGER_RTT_printf(0, "read from layout2 led:\n");

        switch (buffer[0])
        {
        case 0xF0:
            RGB_Color_t* rgb_normal = getNormalColor(1);
            rbuf[0]                 = rgb_normal->r;
            rbuf[1]                 = rgb_normal->g;
            rbuf[2]                 = rgb_normal->b;
            break;
        case 0xF1:
            RGB_Color_t* rgb_upper = getUpperColor(1);
            rbuf[0]                = rgb_upper->r;
            rbuf[1]                = rgb_upper->g;
            rbuf[2]                = rgb_upper->b;
            break;
        case 0xF2:
            RGB_Color_t* rgb_shift = getShiftColor(1);
            rbuf[0]                = rgb_shift->r;
            rbuf[1]                = rgb_shift->g;
            rbuf[2]                = rgb_shift->b;
            break;
        }

        tud_hid_n_report(ITF_NUM_HID_GIO, 0, rbuf, 16);
    }
    else if (buffer[0] == 0xF5)
    {
        SEGGER_RTT_printf(0, "erase & write FLASH...\n");
        HAL_FLASH_Unlock();

        writeAllKeyboardSettings();

        uint8_t rbuf[16] = {0x00};
        rbuf[1]          = 0xF5;
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

void switchLEDColorAccordingKeymaps(void)
{
    for (int j = 0; j < MATRIX_ROWS; j++)
    {
        for (int i = 0; i < MATRIX_COLUMNS; i++)
        {
            int index = MATRIX_COLUMNS * j + i;
            if (index < 30)
            {
                if (!isUpper && getKeyCode(keymapID, j, i) != SC_NULL)
                {
                    setLedBuf(index, getShiftColor(keymapID));
                }
                else if (!isUpper && getKeyCode(keymapID, j, i) == SC_NULL)
                {
                    setLedBuf(index, getBlankColor());
                }
                else if (isUpper && getUpperKeyCode(keymapID, j, i) != SC_NULL)
                {
                    setLedBuf(index, getShiftColor(keymapID));
                }
                else if (isUpper && getUpperKeyCode(keymapID, j, i) == SC_NULL)
                {
                    setLedBuf(index, getBlankColor());
                }
            }
            else if (index >= 36)
            {
                if (!isUpper && getKeyCode(keymapID, j, i) != SC_NULL)
                {
                    setLedBuf(index - 6, getShiftColor(keymapID));
                }
                else if (!isUpper && getKeyCode(keymapID, j, i) == SC_NULL)
                {
                    setLedBuf(index - 6, getBlankColor());
                }
                else if (isUpper && getUpperKeyCode(keymapID, j, i) != SC_NULL)
                {
                    setLedBuf(index - 6, getShiftColor(keymapID));
                }
                else if (isUpper && getUpperKeyCode(keymapID, j, i) == SC_NULL)
                {
                    setLedBuf(index - 6, getBlankColor());
                }
            }
        }
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
    if (code == SC_RESET)
    {
        setBootDfuFlag(false);
        SEGGER_RTT_printf(0, "Reboot\n");
        HAL_Delay(100);
        NVIC_SystemReset();
    }
    else if (code == SC_LAYOUT)
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
                setAllLedBuf(getShiftColor(keymapID));
            }
            else
            {
                setAllLedBuf(getNormalColor(keymapID));
            }
        }
    }
    else if (code == SC_M_WHEEL)
    {
        if (isWheel)
        {
            SEGGER_RTT_printf(0, "wheel off.\n");
            isWheel = false;
        }
    }
    else if (code >= SC_LCONTROL && code <= SC_RGUI)
    {
        keyboardHID.modifiers &= ~(1 << (code - SC_LCONTROL));

        if (code == SC_LSHIFT || code == SC_RSHIFT)
        {
            isShift = false;

            if (isUpper)
            {
                for (int j = 0; j < MATRIX_ROWS; j++)
                {
                    for (int i = 0; i < MATRIX_COLUMNS; i++)
                    {
                        int index = MATRIX_COLUMNS * j + i;
                        if (index < 30)
                        {
                            if (getUpperKeyCode(keymapID, j, i) != SC_NULL)
                            {
                                setLedBuf(index, getUpperColor(keymapID));
                            }
                            else if (getUpperKeyCode(keymapID, j, i) == SC_NULL)
                            {
                                setLedBuf(index, getBlankColor());
                            }
                        }
                        else if (index >= 36)
                        {
                            if (getUpperKeyCode(keymapID, j, i) != SC_NULL)
                            {
                                setLedBuf(index - 6, getUpperColor(keymapID));
                            }
                            else if (getUpperKeyCode(keymapID, j, i) == SC_NULL)
                            {
                                setLedBuf(index - 6, getBlankColor());
                            }
                        }
                    }
                }
            }
            else
            {
                setAllLedBuf(getNormalColor(keymapID));
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

            if (isUpper)
            {
                switchLEDColorAccordingKeymaps();
            }
            else
            {
                setAllLedBuf(getNormalColor(keymapID));
            }
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

            for (int j = 0; j < MATRIX_ROWS; j++)
            {
                for (int i = 0; i < MATRIX_COLUMNS; i++)
                {
                    int index = MATRIX_COLUMNS * j + i;
                    if (index < 30)
                    {
                        if ((((keyboardHID.modifiers >> (SC_LSHIFT - SC_LCONTROL)) & 0x01) ||
                             ((keyboardHID.modifiers >> (SC_RSHIFT - SC_LCONTROL)) & 0x01)))
                        {
                            if (getUpperKeyCode(keymapID, j, i) != SC_NULL)
                            {
                                setLedBuf(index, getShiftColor(keymapID));
                            }
                            else
                            {
                                setLedBuf(index, getBlankColor());
                            }
                        }
                        else
                        {
                            if (getUpperKeyCode(keymapID, j, i) != SC_NULL)
                            {
                                setLedBuf(index, getUpperColor(keymapID));
                            }
                            else
                            {
                                setLedBuf(index, getBlankColor());
                            }
                        }
                    }
                    else if (index >= 36)
                    {
                        if ((((keyboardHID.modifiers >> (SC_LSHIFT - SC_LCONTROL)) & 0x01) ||
                             ((keyboardHID.modifiers >> (SC_RSHIFT - SC_LCONTROL)) & 0x01)))
                        {
                            if (getUpperKeyCode(keymapID, j, i) != SC_NULL)
                            {
                                setLedBuf(index - 6, getShiftColor(keymapID));
                            }
                            else
                            {
                                setLedBuf(index - 6, getBlankColor());
                            }
                        }
                        else
                        {
                            if (getUpperKeyCode(keymapID, j, i) != SC_NULL)
                            {
                                setLedBuf(index - 6, getUpperColor(keymapID));
                            }
                            else
                            {
                                setLedBuf(index - 6, getBlankColor());
                            }
                        }
                    }
                }
            }
        }
    }
    else if (code == SC_M_WHEEL)
    {
        if (!isWheel)
        {
            SEGGER_RTT_printf(0, "wheel on.\n");
            isWheel = true;
        }
    }
    else if (code >= SC_LCONTROL && code <= SC_RGUI)
    {
        if (!((keyboardHID.modifiers >> (SC_LSHIFT - SC_LCONTROL)) & 0x01) &&
            !((keyboardHID.modifiers >> (SC_RSHIFT - SC_LCONTROL)) & 0x01))
        {
            if (code == SC_LSHIFT || code == SC_RSHIFT)
            {
                isShift = true;

                switchLEDColorAccordingKeymaps();
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

                if (code == keymaps_stk[keymapID][0][0])  // L JoyStick -> Tilt left
                {
                    setBackspaceFlag();
                }
                else if (code == keymaps_stk[keymapID][1][1])  // R JoyStick -> Tilt right
                {
                    setSpaceFlag();
                }
                else if (code == keymaps_stk[keymapID][1][2])  // R JoyStick -> Tilt down
                {
                    setEnterFlag();
                }

                break;
            }
        }
    }
}

void controlJoySticks()
{
    for (int i = 0; i < JOYSTICK_NUMS; i++)
    {
        double x = (double) (2048 - pot_value[2 * i + 1]) / 2048.0;
        double y = (double) (pot_value[2 * i + 2] - 2048) / 2048.0;
        double r = sqrt(pow(x, 2.0) + pow(y, 2.0));

        if (i == 1)
        {
            mouseHID.x = (int8_t) (x * MAX_MOUSE_SENSITIVITY);
            mouseHID.y = (int8_t) (y * -MAX_MOUSE_SENSITIVITY);

            mouseHID.vertical = (int8_t) (y * MAX_WHEEL_SENSITIVITY);
        }

        if (r > JOYSTICK_ON_RADIUS)
        {
            double theta = (y >= 0.0 ? 1.0 : -1.0) * acos(x / r) / M_PI * 180.0;

            if (theta >= 90 - JOYSTICK_ON_ANGLE && theta < 90 + JOYSTICK_ON_ANGLE)
            {
                // SEGGER_RTT_printf(0, "%d:up (%d)\n", i, (int) theta);
                if (isUpper && i == 1)
                {
                    currentStk[i][JOYSTICK_V] = 0;
                }
                else
                {
                    currentStk[i][JOYSTICK_V] = -1;
                }
            }
#ifdef ENABLE_LEFT_UP
            else if (theta >= 135 - JOYSTICK_ON_ANGLE && theta < 135 + JOYSTICK_ON_ANGLE)
            {
                // SEGGER_RTT_printf(0, "%d:up left (%d)\n", i, (int) theta);
                if (isUpper && i == 1)
                {
                    currentStk[i][JOYSTICK_H] = 0;
                    currentStk[i][JOYSTICK_V] = 0;
                }
                else
                {
                    currentStk[i][JOYSTICK_H] = -1;
                    currentStk[i][JOYSTICK_V] = -1;
                }
            }
#endif
            else if (theta >= -90 - JOYSTICK_ON_ANGLE && theta < -90 + JOYSTICK_ON_ANGLE)
            {
                // SEGGER_RTT_printf(0, "%d:down (%d)\n", i, (int) theta);
                if (isUpper && i == 1)
                {
                    currentStk[i][JOYSTICK_V] = 0;
                }
                else
                {
                    currentStk[i][JOYSTICK_V] = 1;
                }
            }
            else if (theta < -180 + JOYSTICK_ON_ANGLE || theta >= 180 - JOYSTICK_ON_ANGLE)
            {
                // SEGGER_RTT_printf(0, "%d:left (%d)\n", i, (int) theta);
                if (isUpper && i == 1)
                {
                    currentStk[i][JOYSTICK_H] = 0;
                }
                else
                {
                    currentStk[i][JOYSTICK_H] = -1;
                }
            }
            else if (theta >= 0 - JOYSTICK_ON_ANGLE && theta < 0 + JOYSTICK_ON_ANGLE)
            {
                // SEGGER_RTT_printf(0, "%d:right (%d)\n", i, (int) theta);
                if (isUpper && i == 1)
                {
                    currentStk[i][JOYSTICK_H] = 0;
                }
                else
                {
                    currentStk[i][JOYSTICK_H] = 1;
                }
            }
            else
            {
                currentStk[i][JOYSTICK_H] = 0;
                currentStk[i][JOYSTICK_V] = 0;
            }
        }
        else
        {
            currentStk[i][JOYSTICK_H] = 0;
            currentStk[i][JOYSTICK_V] = 0;
        }
    }

    for (int i = 0; i < JOYSTICK_NUMS; i++)
    {
        if (currentStk[i][JOYSTICK_H] != 0 && currentStk[i][JOYSTICK_V] != 0 &&
            (prevStk[i][JOYSTICK_H] == 0 || prevStk[i][JOYSTICK_V] == 0))
        {
            // SEGGER_RTT_printf(0, "currentStk[%d][H] = %d\n", i, currentStk[i][JOYSTICK_H]);
            // SEGGER_RTT_printf(0, "currentStk[%d][V] = %d\n", i, currentStk[i][JOYSTICK_V]);
#ifdef ENABLE_LEFT_UP
            if (currentStk[i][JOYSTICK_H] == -1 && currentStk[i][JOYSTICK_H] == -1)
            {
                setKeys(SC_UPPER);
                setKeys(SC_LSHIFT);
            }
#endif
        }
        else
        {
            for (int j = 0; j < JOYSTICK_AXIS; j++)
            {
#ifdef ENABLE_LEFT_UP
                if ((currentStk[i][JOYSTICK_H] == 0 && currentStk[i][JOYSTICK_V] == 0) && (prevStk[i][JOYSTICK_H] == -1 && prevStk[i][JOYSTICK_V] == -1))
                {
                    clearKeys(SC_UPPER);
                    clearKeys(SC_LSHIFT);
                    resetKeys();
                    countReturnNeutral = MAX_COUNT_RETURN_NEUTRAL;
                }
#endif
                if (currentStk[i][j] != prevStk[i][j])
                {
                    // SEGGER_RTT_printf(0, "currentStk[%d][%d] = %d (%d, %d, %d, %d)\n", i, j, currentStk[i][j], pot_value[1], pot_value[2], pot_value[3], pot_value[4]);

                    if (currentStk[i][j] == -1 || currentStk[i][j] == 1)
                    {
                        int8_t direction = (j == 0) ? ((currentStk[i][j] + 1) / 2) : ((5 - currentStk[i][j]) / 2);
                        setKeys(keymaps_stk[keymapID][i][direction]);
                    }
                    else if (prevStk[i][j] == -1 || prevStk[i][j] == 1)
                    {
                        int8_t direction = (j == 0) ? ((prevStk[i][j] + 1) / 2) : ((5 - prevStk[i][j]) / 2);
                        clearKeys(keymaps_stk[keymapID][i][direction]);
                        resetKeys();
                        countReturnNeutral = MAX_COUNT_RETURN_NEUTRAL;
                    }
                }
            }
        }
    }
}

bool isUpperPressed(void)
{
    return isUpper;
}

bool isShiftPressed(void)
{
    return isShift;
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
                            clearKeys(keycode);
                            resetKeys();
                            countReturnNeutral = MAX_COUNT_RETURN_NEUTRAL;
                        }
                        else
                        {
                            if (isUpper)
                            {
                                keycode = getUpperKeyCode(keymapID, i, (MATRIX_COLUMNS - 1) - jj);
                            }
                            if (keycode == SC_M_LBTN || keycode == SC_M_RBTN)
                            {
                                mouseHID.buttons = 0;
                                isClicked        = true;
                            }
                            else
                            {
                                clearKeys(keycode);
                            }
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
                        setKeys(keycode);
                    }
                    else
                    {
                        if (isUpper)
                        {
                            keycode = getUpperKeyCode(keymapID, i, (MATRIX_COLUMNS - 1) - jj);
                        }
                        if (keycode == SC_M_LBTN)
                        {
                            mouseHID.buttons = MOUSE_LEFT_CLICK;
                            isClicked        = true;
                        }
                        else if (keycode == SC_M_RBTN)
                        {
                            mouseHID.buttons = MOUSE_RIGHT_CLICK;
                            isClicked        = true;
                        }
                        else
                        {
                            setKeys(keycode);
                        }
                    }
                }
            }

            HAL_GPIO_WritePin(HC165_CLK_GPIO_Port, HC165_CLK_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(HC165_CLK_GPIO_Port, HC165_CLK_Pin, GPIO_PIN_RESET);
        }

        i++;
        break;
    case 4:
        controlJoySticks();

        for (int k = 0; k < MATRIX_ROWS; k++)
        {
            if (keyState[k] != 0x0 || (keyState[k] == 0x0 && keyState[k] != prevKeyState[k]) ||
                currentStk[0][JOYSTICK_H] != prevStk[0][JOYSTICK_H] ||
                currentStk[0][JOYSTICK_V] != prevStk[0][JOYSTICK_V] ||
                currentStk[1][JOYSTICK_H] != prevStk[1][JOYSTICK_H] ||
                currentStk[1][JOYSTICK_V] != prevStk[1][JOYSTICK_V])
            {
                if (!tud_hid_ready())
                    return;

                tud_hid_n_keyboard_report(ITF_NUM_HID_KEYBOARD, REPORT_ID_KEYBOARD, keyboardHID.modifiers, keyboardHID.key);
                break;
            }
            else
            {
#if 1
                if (isUpper && (abs(mouseHID.x) > MIN_MOUSE_THRESHOLD || abs(mouseHID.y) > MIN_MOUSE_THRESHOLD || isClicked))
                {
                    SEGGER_RTT_printf(0, "(x, y) = (%d, %d)\n", mouseHID.x, mouseHID.y);

                    if (!tud_hid_ready())
                        return;

                    if (isWheel)
                    {
                        if (mouseHID.vertical != mouseHID.vertical_prev)
                        {
                            tud_hid_n_mouse_report(ITF_NUM_HID_MOUSE, REPORT_ID_MOUSE, 0, 0, 0, mouseHID.vertical, 0);
                        }
                        mouseHID.vertical_prev = mouseHID.vertical;
                    }
                    else
                    {
                        tud_hid_n_mouse_report(ITF_NUM_HID_MOUSE, REPORT_ID_MOUSE, mouseHID.buttons, mouseHID.x, mouseHID.y, 0, 0);
                        isClicked = false;
                    }
                }
#endif
            }
        }

        for (int k = 0; k < MATRIX_ROWS; k++)
        {
            prevKeyState[k] = keyState[k];
        }

        for (int i = 0; i < JOYSTICK_NUMS; i++)
        {
            for (int j = 0; j < JOYSTICK_AXIS; j++)
            {
                prevStk[i][j] = currentStk[i][j];
            }
        }
        i = 0;
        break;
    default:
        i = 0;
        break;
    }
}

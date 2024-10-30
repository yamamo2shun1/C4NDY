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

const uint8_t keymaps_normal_default[2][MATRIX_ROWS][MATRIX_COLUMNS][2] = {
    // clang-format off
	{
        {{KC_ESC, M_NO},  {KC_1, M_NO},    {KC_2, M_NO},      {KC_3, M_NO},    {KC_4, M_NO},  {KC_5, M_NO},      {KC_6, M_NO},     {KC_7, M_NO},     {KC_8, M_NO},        {KC_9, M_NO},      {KC_0, M_NO},     {KC_MINUS, M_NO}, {KC_EQUAL, M_NO}},
        {{KC_TAB, M_NO},  {KC_Q, M_NO},    {KC_W, M_NO},      {KC_E, M_NO},    {KC_R, M_NO},  {KC_T, M_NO},      {KC_Y, M_NO},     {KC_U, M_NO},     {KC_I, M_NO},        {KC_O, M_NO},      {KC_P, M_NO},     {KC_LSB, M_NO},   {KC_RSB, M_NO}},
        {{KC_NULL, M_LC}, {KC_A, M_NO},    {KC_S, M_NO},      {KC_D, M_NO},    {KC_F, M_NO},  {KC_G, M_NO},      {KC_H, M_NO},     {KC_J, M_NO},     {KC_K, M_NO},        {KC_L, M_NO},      {KC_SC, M_NO},    {KC_APS, M_NO},   {KC_YEN, M_NO}},
        {{KC_NULL, M_LS}, {KC_Z, M_NO},    {KC_X, M_NO},      {KC_C, M_NO},    {KC_V, M_NO},  {KC_B, M_NO},      {KC_N, M_NO},     {KC_M, M_NO},     {KC_COMMA, M_NO},    {KC_PERIOD, M_NO}, {KC_SLASH, M_NO}, {KC_NULL, M_RS},  {KC_GA, M_NO}},
        {{KC_NULL, M_LG}, {KC_LNPH, M_NO}, {KC_LAYOUT, M_NO}, {KC_NULL, M_LA}, {KC_BS, M_NO}, {KC_DELETE, M_NO}, {KC_ENTER, M_NO}, {KC_SPACE, M_NO}, {KC_CAPSLOCK, M_NO}, {KC_LEFT, M_NO},   {KC_DOWN, M_NO},  {KC_UP, M_NO},    {KC_RIGHT, M_NO}}
    },
    {
        {{KC_ESC, M_NO},  {KC_1, M_NO},    {KC_2, M_NO},      {KC_3, M_NO},    {KC_4, M_NO},  {KC_5, M_NO},      {KC_6, M_NO},     {KC_7, M_NO},     {KC_8, M_NO},        {KC_9, M_NO},    {KC_0, M_NO},    {KC_LSB, M_NO},   {KC_RSB, M_NO}},
        {{KC_TAB, M_NO},  {KC_APS, M_NO},  {KC_COMMA, M_NO},  {KC_O, M_NO},    {KC_U, M_NO},  {KC_Y, M_NO},      {KC_F, M_NO},     {KC_G, M_NO},     {KC_C, M_NO},        {KC_R, M_NO},    {KC_L, M_NO},    {KC_SLASH, M_NO}, {KC_EQUAL, M_NO}},
        {{KC_NULL, M_LC}, {KC_P, M_NO},    {KC_I, M_NO},      {KC_E, M_NO},    {KC_A, M_NO},  {KC_PERIOD, M_NO}, {KC_D, M_NO},     {KC_S, M_NO},     {KC_T, M_NO},        {KC_H, M_NO},    {KC_Z, M_NO},    {KC_MINUS, M_NO}, {KC_BSLASH, M_NO}},
        {{KC_NULL, M_LS}, {KC_J, M_NO},    {KC_Q, M_NO},      {KC_SC, M_NO},   {KC_K, M_NO},  {KC_X, M_NO},      {KC_B, M_NO},     {KC_M, M_NO},     {KC_W, M_NO},        {KC_N, M_NO},    {KC_V, M_NO},    {KC_NULL, M_RS},  {KC_GA, M_NO}},
        {{KC_NULL, M_LG}, {KC_LNPH, M_NO}, {KC_LAYOUT, M_NO}, {KC_NULL, M_LA}, {KC_BS, M_NO}, {KC_DELETE, M_NO}, {KC_ENTER, M_NO}, {KC_SPACE, M_NO}, {KC_CAPSLOCK, M_NO}, {KC_LEFT, M_NO}, {KC_DOWN, M_NO}, {KC_UP, M_NO},    {KC_RIGHT, M_NO}}
    }
    // clang-format on
};

uint8_t keymaps_normal[2][MATRIX_ROWS][MATRIX_COLUMNS][2] = {
    // clang-format off
    {
        {{KC_ESC, M_NO},  {KC_1, M_NO},    {KC_2, M_NO},      {KC_3, M_NO},    {KC_4, M_NO},  {KC_5, M_NO},      {KC_6, M_NO},     {KC_7, M_NO},     {KC_8, M_NO},        {KC_9, M_NO},      {KC_0, M_NO},     {KC_MINUS, M_NO}, {KC_EQUAL, M_NO}},
        {{KC_TAB, M_NO},  {KC_Q, M_NO},    {KC_W, M_NO},      {KC_E, M_NO},    {KC_R, M_NO},  {KC_T, M_NO},      {KC_Y, M_NO},     {KC_U, M_NO},     {KC_I, M_NO},        {KC_O, M_NO},      {KC_P, M_NO},     {KC_LSB, M_NO},   {KC_RSB, M_NO}},
        {{KC_NULL, M_LC}, {KC_A, M_NO},    {KC_S, M_NO},      {KC_D, M_NO},    {KC_F, M_NO},  {KC_G, M_NO},      {KC_H, M_NO},     {KC_J, M_NO},     {KC_K, M_NO},        {KC_L, M_NO},      {KC_SC, M_NO},    {KC_APS, M_NO},   {KC_YEN, M_NO}},
        {{KC_NULL, M_LS}, {KC_Z, M_NO},    {KC_X, M_NO},      {KC_C, M_NO},    {KC_V, M_NO},  {KC_B, M_NO},      {KC_N, M_NO},     {KC_M, M_NO},     {KC_COMMA, M_NO},    {KC_PERIOD, M_NO}, {KC_SLASH, M_NO}, {KC_NULL, M_RS},  {KC_GA, M_NO}},
        {{KC_NULL, M_LG}, {KC_LNPH, M_NO}, {KC_LAYOUT, M_NO}, {KC_NULL, M_LA}, {KC_BS, M_NO}, {KC_DELETE, M_NO}, {KC_ENTER, M_NO}, {KC_SPACE, M_NO}, {KC_CAPSLOCK, M_NO}, {KC_LEFT, M_NO},   {KC_DOWN, M_NO},  {KC_UP, M_NO},    {KC_RIGHT, M_NO}}
    },
    {
        {{KC_ESC, M_NO},  {KC_1, M_NO},    {KC_2, M_NO},      {KC_3, M_NO},    {KC_4, M_NO},  {KC_5, M_NO},      {KC_6, M_NO},     {KC_7, M_NO},     {KC_8, M_NO},        {KC_9, M_NO},    {KC_0, M_NO},    {KC_LSB, M_NO},   {KC_RSB, M_NO}},
        {{KC_TAB, M_NO},  {KC_APS, M_NO},  {KC_COMMA, M_NO},  {KC_O, M_NO},    {KC_U, M_NO},  {KC_Y, M_NO},      {KC_F, M_NO},     {KC_G, M_NO},     {KC_C, M_NO},        {KC_R, M_NO},    {KC_L, M_NO},    {KC_SLASH, M_NO}, {KC_EQUAL, M_NO}},
        {{KC_NULL, M_LC}, {KC_P, M_NO},    {KC_I, M_NO},      {KC_E, M_NO},    {KC_A, M_NO},  {KC_PERIOD, M_NO}, {KC_D, M_NO},     {KC_S, M_NO},     {KC_T, M_NO},        {KC_H, M_NO},    {KC_Z, M_NO},    {KC_MINUS, M_NO}, {KC_BSLASH, M_NO}},
        {{KC_NULL, M_LS}, {KC_J, M_NO},    {KC_Q, M_NO},      {KC_SC, M_NO},   {KC_K, M_NO},  {KC_X, M_NO},      {KC_B, M_NO},     {KC_M, M_NO},     {KC_W, M_NO},        {KC_N, M_NO},    {KC_V, M_NO},    {KC_NULL, M_RS},  {KC_GA, M_NO}},
        {{KC_NULL, M_LG}, {KC_LNPH, M_NO}, {KC_LAYOUT, M_NO}, {KC_NULL, M_LA}, {KC_BS, M_NO}, {KC_DELETE, M_NO}, {KC_ENTER, M_NO}, {KC_SPACE, M_NO}, {KC_CAPSLOCK, M_NO}, {KC_LEFT, M_NO}, {KC_DOWN, M_NO}, {KC_UP, M_NO},    {KC_RIGHT, M_NO}}
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

void setKeymapID(const uint8_t val)
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

void setLinePhonoSW(const uint8_t val)
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
            setKeyCode(0, i, j, keymaps_normal_default[0][i][j][0]);
            setKeyCode(1, i, j, keymaps_normal_default[1][i][j][0]);
            write_flash_data(BASIC_PARAMS_NUM + 0 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, keymaps_normal_default[0][i][j][0]);
            write_flash_data(BASIC_PARAMS_NUM + 1 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, keymaps_normal_default[1][i][j][0]);

            setModifiers(0, i, j, keymaps_normal_default[0][i][j][1]);
            setModifiers(1, i, j, keymaps_normal_default[1][i][j][1]);
            write_flash_data(BASIC_PARAMS_NUM + 2 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, keymaps_normal_default[0][i][j][1]);
            write_flash_data(BASIC_PARAMS_NUM + 3 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, keymaps_normal_default[0][i][j][1]);
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

            write_flash_data(BASIC_PARAMS_NUM + 2 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, getModifiers(0, i, j));
            write_flash_data(BASIC_PARAMS_NUM + 3 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, getModifiers(1, i, j));
        }
    }

    HAL_FLASH_Lock();
}

void loadKeyboardSettingsFromFlash(void)
{
    if (read_flash_data(1) > 1)
    {
        setLinePhonoSW(0);
    }
    else
    {
        setLinePhonoSW(read_flash_data(1));
    }
    SEGGER_RTT_printf(0, "Phono/Line SW = %u\n", getLinePhonoSW());

    if (read_flash_data(2) > 1)
    {
        setKeymapID(0);
    }
    else
    {
        setKeymapID(read_flash_data(2));
    }
    SEGGER_RTT_printf(0, "keymapID = %u %u\n", getKeymapID(), read_flash_data(2));

    for (int k = 0; k < 2; k++)
    {
        SEGGER_RTT_printf(0, "\n");
        for (int i = 0; i < MATRIX_ROWS; i++)
        {
            SEGGER_RTT_printf(0, "[ ");
            for (int j = 0; j < MATRIX_COLUMNS; j++)
            {
                if (k == 0)
                {
                    setKeyCode(0, i, j, read_flash_data(BASIC_PARAMS_NUM + k * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j));
                }
                else
                {
                    setKeyCode(1, i, j, read_flash_data(BASIC_PARAMS_NUM + k * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j));
                }
                SEGGER_RTT_printf(0, "%02X ", read_flash_data(BASIC_PARAMS_NUM + k * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j));
            }
            SEGGER_RTT_printf(0, "]\n");
        }
    }
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

    uint8_t buffer_sb[CFG_TUD_HID_EP_BUFSIZE] = {0x00};
    if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x00)
    {
        SEGGER_RTT_printf(0, "read:\n");
        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            buffer_sb[2 * j]     = keymaps_normal[keymapID][buffer[0] - 0xF0][j][0];
            buffer_sb[2 * j + 1] = keymaps_normal[keymapID][buffer[0] - 0xF0][j][1];
        }

#if 0
	  for (int i = 0; i < CFG_TUD_HID_EP_BUFSIZE; i++)
	  {
		  SEGGER_RTT_printf(0, "buffer_sb[%d] = %d\n", i, buffer_sb[i]);
	  }
#endif
        tud_hid_n_report(1, 0, buffer_sb, CFG_TUD_HID_EP_BUFSIZE);
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x01)
    {
        SEGGER_RTT_printf(0, "write to layout1:\n");

        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            keymaps_normal[0][buffer[0] - 0xF0][j][0] = buffer[(2 * j) + 2];
            keymaps_normal[0][buffer[0] - 0xF0][j][1] = buffer[(2 * j + 1) + 2];
        }
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x09)
    {
        SEGGER_RTT_printf(0, "write to layout2:\n");

        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            keymaps_normal[1][buffer[0] - 0xF0][j][0] = buffer[(2 * j) + 2];
            keymaps_normal[1][buffer[0] - 0xF0][j][1] = buffer[(2 * j + 1) + 2];
        }
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x11)
    {
        SEGGER_RTT_printf(0, "read from layout1:\n");
        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            buffer_sb[2 * j]     = keymaps_normal[0][buffer[0] - 0xF0][j][0];
            buffer_sb[2 * j + 1] = keymaps_normal[0][buffer[0] - 0xF0][j][1];
        }

        tud_hid_n_report(1, 0, buffer_sb, CFG_TUD_HID_EP_BUFSIZE);
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF4 && buffer[1] == 0x19)
    {
        SEGGER_RTT_printf(0, "read from layout2:\n");
        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            buffer_sb[2 * j]     = keymaps_normal[1][buffer[0] - 0xF0][j][0];
            buffer_sb[2 * j + 1] = keymaps_normal[1][buffer[0] - 0xF0][j][1];
        }

        tud_hid_n_report(1, 0, buffer_sb, CFG_TUD_HID_EP_BUFSIZE);
    }
    else if (buffer[0] == 0xF5)
    {
        SEGGER_RTT_printf(0, "erase & write FLASH...\n");

        writeAllKeyboardSettings();

        uint8_t rbuf[CFG_TUD_HID_EP_BUFSIZE] = {0x00};
        rbuf[1]                              = 0xF5;
        rbuf[2]                              = 0x01;
        tud_hid_n_report(1, 0, rbuf, CFG_TUD_HID_EP_BUFSIZE);
    }
    else if (buffer[0] == 0xF6)
    {
        SEGGER_RTT_printf(0, "reboot...\n");

        uint8_t rbuf[CFG_TUD_HID_EP_BUFSIZE] = {0x00};
        rbuf[1]                              = 0xF6;
        rbuf[2]                              = 0x01;
        tud_hid_n_report(ITF_NUM_HID_GIO, 0, rbuf, CFG_TUD_HID_EP_BUFSIZE);

        setBootDfuFlag(false);
        HAL_Delay(100);
        NVIC_SystemReset();
    }
    else if (buffer[0] == 0xF9)
    {
        SEGGER_RTT_printf(0, "factory reset...\n");

        factoryReset();

        uint8_t rbuf[CFG_TUD_HID_EP_BUFSIZE] = {0x00};
        rbuf[1]                              = 0xF9;
        rbuf[2]                              = 0x01;
        tud_hid_n_report(ITF_NUM_HID_GIO, 0, rbuf, CFG_TUD_HID_EP_BUFSIZE);
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

uint8_t getKeyCode(const uint8_t keymapId, const uint8_t x, const uint8_t y)
{
    return keymaps_normal[keymapId][x][y][0];
}

void setKeyCode(const uint8_t keymapId, const uint8_t x, const uint8_t y, const uint8_t code)
{
    keymaps_normal[keymapId][x][y][0] = code;
}

uint8_t getModifiers(const uint8_t keymapId, const uint8_t x, const uint8_t y)
{
    return keymaps_normal[keymapId][x][y][1];
}

void setModifiers(const uint8_t keymapId, const uint8_t x, const uint8_t y, const uint8_t modifiers)
{
    keymaps_normal[keymapId][x][y][1] = modifiers;
}

void resetKeys(void)
{
    keyboardHID.modifiers = 0;
    for (int k = 0; k < 6; k++)
    {
        keyboardHID.key[k] = 0;
    }
}

void clearKeys(const uint8_t code, const uint8_t modifiers)
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

    if (modifiers > M_NO)
    {
        keyboardHID.modifiers &= ~modifiers;
    }

    longPressCounter = 0;
}

void setKeys(const uint8_t code, const uint8_t modifiers)
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
                if (code != KC_NULL)
                {
                    keyboardHID.key[k] = code;
                }
                break;
            }
        }
    }

    if (modifiers > M_NO)
    {
        keyboardHID.modifiers |= modifiers;
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
                        uint8_t keycode   = getKeyCode(keymapID, i, (MATRIX_COLUMNS - 1) - jj);
                        uint8_t modifiers = getModifiers(keymapID, i, (MATRIX_COLUMNS - 1) - jj);
                        clearKeys(keycode, modifiers);
                    }
                }
                else
                {
                    keyState[i] |= ((uint16_t) 1 << jj);

                    uint8_t keycode   = getKeyCode(keymapID, i, (MATRIX_COLUMNS - 1) - jj);
                    uint8_t modifiers = getModifiers(keymapID, i, (MATRIX_COLUMNS - 1) - jj);
                    setKeys(keycode, modifiers);
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

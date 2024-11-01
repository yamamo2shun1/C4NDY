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

bool isUpper         = false;
bool isShift         = false;
bool isClicked       = false;
bool isWheel         = false;
bool isXFadeCut      = false;
bool isStickReturned = false;

int offset_calibrate_count[JOYSTICK_NUMS] = {0};
double x_offset[JOYSTICK_NUMS]            = {0.0};
double y_offset[JOYSTICK_NUMS]            = {0.0};

uint16_t countReturnNeutral = 0;
#define MAX_COUNT_RETURN_NEUTRAL 600

int8_t currentStk[JOYSTICK_NUMS][JOYSTICK_AXIS] = {0};
int8_t prevStk[JOYSTICK_NUMS][JOYSTICK_AXIS]    = {0};

// clang-format off
const uint8_t keymaps_normal_default[2][MATRIX_ROWS][MATRIX_COLUMNS][2] = {
    {
        {{KC_Q, M_NO},    {KC_W, M_NO},    {KC_E, M_NO},    {KC_R, M_NO},    {KC_T, M_NO},        {KC_Y, M_NO},    {KC_U, M_NO},    {KC_I, M_NO},     {KC_O, M_NO},      {KC_P, M_NO}},
        {{KC_A, M_NO},    {KC_S, M_NO},    {KC_D, M_NO},    {KC_F, M_NO},    {KC_G, M_NO},        {KC_H, M_NO},    {KC_J, M_NO},    {KC_K, M_NO},     {KC_L, M_NO},      {KC_SC, M_NO}},
        {{KC_Z, M_NO},    {KC_X, M_NO},    {KC_C, M_NO},    {KC_V, M_NO},    {KC_B, M_NO},        {KC_N, M_NO},    {KC_M, M_NO},    {KC_COMMA, M_NO}, {KC_PERIOD, M_NO}, {KC_SLASH, M_NO}},
        {{KC_NULL, M_NO}, {KC_LGUI, M_NO}, {KC_LALT, M_NO}, {KC_NULL, M_NO}, {KC_LCONTROL, M_NO}, {KC_NULL, M_NO}, {KC_LEFT, M_NO}, {KC_DOWN, M_NO},  {KC_UP, M_NO},     {KC_RIGHT, M_NO}}
    },
    {
        {{KC_APS, M_NO},  {KC_COMMA, M_NO}, {KC_O, M_NO},    {KC_U, M_NO},    {KC_Y, M_NO},        {KC_F, M_NO},    {KC_G, M_NO},    {KC_C, M_NO},    {KC_R, M_NO},  {KC_L, M_NO}},
        {{KC_P, M_NO},    {KC_I, M_NO},     {KC_E, M_NO},    {KC_A, M_NO},    {KC_PERIOD, M_NO},   {KC_D, M_NO},    {KC_S, M_NO},    {KC_T, M_NO},    {KC_H, M_NO},  {KC_Z, M_NO}},
        {{KC_J, M_NO},    {KC_Q, M_NO},     {KC_SC, M_NO},   {KC_K, M_NO},    {KC_X, M_NO},        {KC_B, M_NO},    {KC_M, M_NO},    {KC_W, M_NO},    {KC_N, M_NO},  {KC_V, M_NO}},
        {{KC_NULL, M_NO}, {KC_LGUI, M_NO},  {KC_LALT, M_NO}, {KC_NULL, M_NO}, {KC_LCONTROL, M_NO}, {KC_NULL, M_NO}, {KC_LEFT, M_NO}, {KC_DOWN, M_NO}, {KC_UP, M_NO}, {KC_RIGHT, M_NO}}
    }
};

const uint8_t keymaps_upper_default[2][MATRIX_ROWS][MATRIX_COLUMNS][2] = {
    {
        {{KC_1, M_NO},    {KC_2, M_NO},        {KC_3, M_NO},      {KC_4, M_NO},      {KC_5, M_NO},        {KC_6, M_NO},    {KC_7, M_NO},    {KC_8, M_NO},          {KC_9, M_NO},        {KC_0, M_NO}},
        {{KC_APS, M_NO},  {KC_GA, M_NO},       {KC_NULL, M_NO},   {KC_LGUI, M_NO},   {KC_NULL, M_NO},     {KC_NULL, M_NO}, {KC_LSB, M_NO},  {KC_RSB, M_NO},        {KC_MINUS, M_NO},    {KC_EQUAL, M_NO}},
        {{KC_NULL, M_NO}, {KC_CAPSLOCK, M_NO}, {KC_M_LBTN, M_NO}, {KC_M_RBTN, M_NO}, {KC_M_WHEEL, M_NO},  {KC_NULL, M_NO}, {KC_NULL, M_NO}, {KC_COMMA, M_NO},      {KC_PERIOD, M_NO},   {KC_BSLASH, M_NO}},
        {{KC_NULL, M_NO}, {KC_LNPH, M_NO},     {KC_LAYOUT, M_NO}, {KC_NULL, M_NO},   {KC_LCONTROL, M_NO}, {KC_NULL, M_NO}, {KC_NULL, M_NO}, {KC_MGAIN_DOWN, M_NO}, {KC_MGAIN_UP, M_NO}, {KC_RESET, M_NO}}
    },
    {
        {{KC_1, M_NO},    {KC_2, M_NO},        {KC_3, M_NO},      {KC_4, M_NO},      {KC_5, M_NO},        {KC_6, M_NO},    {KC_7, M_NO},     {KC_8, M_NO},          {KC_9, M_NO},        {KC_0, M_NO}},
        {{KC_APS, M_NO},  {KC_COMMA, M_NO},    {KC_NULL, M_NO},   {KC_LGUI, M_NO},   {KC_PERIOD, M_NO},   {KC_NULL, M_NO}, {KC_LSB, M_NO},   {KC_RSB, M_NO},        {KC_SLASH, M_NO},    {KC_MINUS, M_NO}},
        {{KC_NULL, M_NO}, {KC_CAPSLOCK, M_NO}, {KC_M_LBTN, M_NO}, {KC_M_RBTN, M_NO}, {KC_M_WHEEL, M_NO},  {KC_NULL, M_NO}, {KC_EQUAL, M_NO}, {KC_NULL, M_NO},       {KC_NULL, M_NO},     {KC_BSLASH, M_NO}},
        {{KC_NULL, M_NO}, {KC_LNPH, M_NO},     {KC_LAYOUT, M_NO}, {KC_NULL, M_NO},   {KC_LCONTROL, M_NO}, {KC_NULL, M_NO}, {KC_NULL, M_NO},  {KC_MGAIN_DOWN, M_NO}, {KC_MGAIN_UP, M_NO}, {KC_RESET, M_NO}}
    }
};

const uint8_t keymaps_stk_default[2][STICK_NUM][STICK_DIRECTION][2] = {
    {
        {
            {KC_NULL, M_NO}, {KC_UPPER, M_NO}, {KC_NULL, M_NO},
            {KC_BS, M_NO},   {KC_NULL, M_NO},  {KC_TAB, M_NO},
            {KC_NULL, M_NO}, {KC_ENTER, M_NO}, {KC_NULL, M_NO}
        }, // left stick
        {
            {KC_NULL, M_NO},   {KC_RSHIFT, M_NO}, {KC_NULL, M_NO},
            {KC_DELETE, M_NO}, {KC_NULL, M_NO},   {KC_SPACE, M_NO},
            {KC_NULL, M_NO},   {KC_ESC, M_NO},    {KC_NULL, M_NO}
        } // right stick
    },
    {
        {
            {KC_NULL, M_NO}, {KC_UPPER, M_NO}, {KC_NULL, M_NO},
            {KC_BS, M_NO},   {KC_NULL, M_NO},  {KC_TAB, M_NO},
            {KC_NULL, M_NO}, {KC_ENTER, M_NO}, {KC_NULL, M_NO}
        }, // left stick
        {
            {KC_NULL, M_NO},   {KC_RSHIFT, M_NO}, {KC_NULL, M_NO},
            {KC_DELETE, M_NO}, {KC_NULL, M_NO},   {KC_SPACE, M_NO},
            {KC_NULL, M_NO},   {KC_ESC, M_NO},    {KC_NULL, M_NO}
        } // right stick
    }
};

uint8_t keymaps_normal[2][MATRIX_ROWS][MATRIX_COLUMNS][2] = {
    {
        {{KC_Q, M_NO},    {KC_W, M_NO},    {KC_E, M_NO},    {KC_R, M_NO},    {KC_T, M_NO},        {KC_Y, M_NO},    {KC_U, M_NO},    {KC_I, M_NO},     {KC_O, M_NO},      {KC_P, M_NO}},
        {{KC_A, M_NO},    {KC_S, M_NO},    {KC_D, M_NO},    {KC_F, M_NO},    {KC_G, M_NO},        {KC_H, M_NO},    {KC_J, M_NO},    {KC_K, M_NO},     {KC_L, M_NO},      {KC_SC, M_NO}},
        {{KC_Z, M_NO},    {KC_X, M_NO},    {KC_C, M_NO},    {KC_V, M_NO},    {KC_B, M_NO},        {KC_N, M_NO},    {KC_M, M_NO},    {KC_COMMA, M_NO}, {KC_PERIOD, M_NO}, {KC_SLASH, M_NO}},
        {{KC_NULL, M_NO}, {KC_LGUI, M_NO}, {KC_LALT, M_NO}, {KC_NULL, M_NO}, {KC_LCONTROL, M_NO}, {KC_NULL, M_NO}, {KC_LEFT, M_NO}, {KC_DOWN, M_NO},  {KC_UP, M_NO},     {KC_RIGHT, M_NO}}
    },
    {
        {{KC_APS, M_NO},  {KC_COMMA, M_NO}, {KC_O, M_NO},    {KC_U, M_NO},    {KC_Y, M_NO},        {KC_F, M_NO},    {KC_G, M_NO},    {KC_C, M_NO},    {KC_R, M_NO},  {KC_L, M_NO}},
        {{KC_P, M_NO},    {KC_I, M_NO},     {KC_E, M_NO},    {KC_A, M_NO},    {KC_PERIOD, M_NO},   {KC_D, M_NO},    {KC_S, M_NO},    {KC_T, M_NO},    {KC_H, M_NO},  {KC_Z, M_NO}},
        {{KC_J, M_NO},    {KC_Q, M_NO},     {KC_SC, M_NO},   {KC_K, M_NO},    {KC_X, M_NO},        {KC_B, M_NO},    {KC_M, M_NO},    {KC_W, M_NO},    {KC_N, M_NO},  {KC_V, M_NO}},
        {{KC_NULL, M_NO}, {KC_LGUI, M_NO},  {KC_LALT, M_NO}, {KC_NULL, M_NO}, {KC_LCONTROL, M_NO}, {KC_NULL, M_NO}, {KC_LEFT, M_NO}, {KC_DOWN, M_NO}, {KC_UP, M_NO}, {KC_RIGHT, M_NO}}
    }
};

uint8_t keymaps_upper[2][MATRIX_ROWS][MATRIX_COLUMNS][2] = {
    {
        {{KC_1, M_NO},    {KC_2, M_NO},        {KC_3, M_NO},      {KC_4, M_NO},      {KC_5, M_NO},        {KC_6, M_NO},    {KC_7, M_NO},    {KC_8, M_NO},          {KC_9, M_NO},        {KC_0, M_NO}},
        {{KC_APS, M_NO},  {KC_GA, M_NO},       {KC_NULL, M_NO},   {KC_LGUI, M_NO},   {KC_NULL, M_NO},     {KC_NULL, M_NO}, {KC_LSB, M_NO},  {KC_RSB, M_NO},        {KC_MINUS, M_NO},    {KC_EQUAL, M_NO}},
        {{KC_NULL, M_NO}, {KC_CAPSLOCK, M_NO}, {KC_M_LBTN, M_NO}, {KC_M_RBTN, M_NO}, {KC_M_WHEEL, M_NO},  {KC_NULL, M_NO}, {KC_NULL, M_NO}, {KC_COMMA, M_NO},      {KC_PERIOD, M_NO},   {KC_BSLASH, M_NO}},
        {{KC_NULL, M_NO}, {KC_LNPH, M_NO},     {KC_LAYOUT, M_NO}, {KC_NULL, M_NO},   {KC_LCONTROL, M_NO}, {KC_NULL, M_NO}, {KC_NULL, M_NO}, {KC_MGAIN_DOWN, M_NO}, {KC_MGAIN_UP, M_NO}, {KC_RESET, M_NO}}
    },
    {
        {{KC_1, M_NO},    {KC_2, M_NO},        {KC_3, M_NO},      {KC_4, M_NO},      {KC_5, M_NO},        {KC_6, M_NO},    {KC_7, M_NO},     {KC_8, M_NO},          {KC_9, M_NO},         {KC_0, M_NO}},
        {{KC_APS, M_NO},  {KC_COMMA, M_NO},    {KC_NULL, M_NO},   {KC_LGUI, M_NO},   {KC_PERIOD, M_NO},   {KC_NULL, M_NO}, {KC_LSB, M_NO},   {KC_RSB, M_NO},        {KC_SLASH, M_NO},     {KC_MINUS, M_NO}},
        {{KC_NULL, M_NO}, {KC_CAPSLOCK, M_NO}, {KC_M_LBTN, M_NO}, {KC_M_RBTN, M_NO}, {KC_M_WHEEL, M_NO},  {KC_NULL, M_NO}, {KC_EQUAL, M_NO}, {KC_NULL, M_NO},       {KC_NULL, M_NO},      {KC_BSLASH, M_NO}},
        {{KC_NULL, M_NO}, {KC_LNPH, M_NO},     {KC_LAYOUT, M_NO}, {KC_NULL, M_NO},   {KC_LCONTROL, M_NO}, {KC_NULL, M_NO}, {KC_NULL, M_NO},  {KC_MGAIN_DOWN, M_NO}, {KC_MGAIN_UP, M_NO},  {KC_RESET, M_NO}}
    }
};

uint8_t keymaps_stk[2][STICK_NUM][STICK_DIRECTION][2] = {
    {
        {
            {KC_NULL, M_NO}, {KC_UPPER, M_NO}, {KC_NULL, M_NO},
            {KC_BS, M_NO},   {KC_NULL, M_NO},  {KC_TAB, M_NO},
            {KC_NULL, M_NO}, {KC_ENTER, M_NO}, {KC_NULL, M_NO}
        }, // left stick
        {
            {KC_NULL, M_NO},   {KC_RSHIFT, M_NO}, {KC_NULL, M_NO},
            {KC_DELETE, M_NO}, {KC_NULL, M_NO},   {KC_SPACE, M_NO},
            {KC_NULL, M_NO},   {KC_ESC, M_NO},    {KC_NULL, M_NO}
        } // right stick
    },
    {
        {
            {KC_NULL, M_NO}, {KC_UPPER, M_NO}, {KC_NULL, M_NO},
            {KC_BS, M_NO},   {KC_NULL, M_NO},  {KC_TAB, M_NO},
            {KC_NULL, M_NO}, {KC_ENTER, M_NO}, {KC_NULL, M_NO}
        }, // left stick
        {
            {KC_NULL, M_NO},   {KC_RSHIFT, M_NO}, {KC_NULL, M_NO},
            {KC_DELETE, M_NO}, {KC_NULL, M_NO},   {KC_SPACE, M_NO},
            {KC_NULL, M_NO},   {KC_ESC, M_NO},    {KC_NULL, M_NO}
        } // right stick
    }
};
// clang-format on

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
    write_flash_data(2, 0);

    setIntensity(0, 255);
    setIntensity(1, 255);
    write_flash_data(3, 255);
    write_flash_data(4, 255);

    for (int i = 0; i < MATRIX_ROWS; i++)
    {
        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            setNormalKeyCode(0, i, j, keymaps_normal_default[0][i][j][0]);
            setNormalKeyCode(1, i, j, keymaps_normal_default[1][i][j][0]);
            write_flash_data(BASIC_PARAMS_NUM + 0 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, keymaps_normal_default[0][i][j][0]);
            write_flash_data(BASIC_PARAMS_NUM + 1 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, keymaps_normal_default[1][i][j][0]);

            setNormalModifiers(0, i, j, keymaps_normal_default[0][i][j][1]);
            setNormalModifiers(1, i, j, keymaps_normal_default[1][i][j][1]);
            write_flash_data(BASIC_PARAMS_NUM + 2 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, keymaps_normal_default[0][i][j][1]);
            write_flash_data(BASIC_PARAMS_NUM + 3 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, keymaps_normal_default[1][i][j][1]);

            setUpperKeyCode(0, i, j, keymaps_upper_default[0][i][j][0]);
            setUpperKeyCode(1, i, j, keymaps_upper_default[1][i][j][0]);
            write_flash_data(BASIC_PARAMS_NUM + (4 * MATRIX_ROWS * MATRIX_COLUMNS) + 0 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, keymaps_upper_default[0][i][j][0]);
            write_flash_data(BASIC_PARAMS_NUM + (4 * MATRIX_ROWS * MATRIX_COLUMNS) + 1 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, keymaps_upper_default[1][i][j][0]);

            setUpperModifiers(0, i, j, keymaps_upper_default[0][i][j][1]);
            setUpperModifiers(1, i, j, keymaps_upper_default[1][i][j][1]);
            write_flash_data(BASIC_PARAMS_NUM + (4 * MATRIX_ROWS * MATRIX_COLUMNS) + 2 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, keymaps_upper_default[0][i][j][1]);
            write_flash_data(BASIC_PARAMS_NUM + (4 * MATRIX_ROWS * MATRIX_COLUMNS) + 3 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, keymaps_upper_default[1][i][j][1]);
        }
    }

    for (int i = 0; i < STICK_NUM; i++)
    {
        for (int j = 0; j < STICK_DIRECTION; j++)
        {
            setStickKeyCode(0, i, j, keymaps_stk_default[0][i][j][0]);
            setStickKeyCode(1, i, j, keymaps_stk_default[1][i][j][0]);
            write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + 0 * (STICK_NUM * STICK_DIRECTION) + i * STICK_DIRECTION + j, keymaps_stk_default[0][i][j][0]);
            write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + 1 * (STICK_NUM * STICK_DIRECTION) + i * STICK_DIRECTION + j, keymaps_stk_default[1][i][j][0]);

            setStickModifiers(0, i, j, keymaps_stk_default[0][i][j][1]);
            setStickModifiers(1, i, j, keymaps_stk_default[1][i][j][1]);
            write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + 2 * (STICK_NUM * STICK_DIRECTION) + i * STICK_DIRECTION + j, keymaps_stk_default[0][i][j][0]);
            write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + 3 * (STICK_NUM * STICK_DIRECTION) + i * STICK_DIRECTION + j, keymaps_stk_default[1][i][j][0]);
        }
    }

    setNormalColor(0, 0xFF, 0xFF, 0xFF);
    setNormalColor(1, 0xFF, 0xFF, 0xFF);
    setUpperColor(0, 0x00, 0xFF, 0xFF);
    setUpperColor(1, 0x00, 0xFF, 0xFF);
    setShiftColor(0, 0xFF, 0x00, 0x8C);
    setShiftColor(1, 0xFF, 0x00, 0x8C);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 0, 0xFF);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 1, 0xFF);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 2, 0xFF);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 3, 0x00);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 4, 0xFF);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 5, 0xFF);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 6, 0xFF);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 7, 0x00);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 8, 0x8C);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 9, 0xFF);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 10, 0xFF);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 11, 0xFF);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 12, 0x00);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 13, 0xFF);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 14, 0xFF);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 15, 0xFF);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 16, 0x00);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 17, 0x8C);

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
    write_flash_data(3, (uint8_t) (getIntensity(0) * 255));
    write_flash_data(4, (uint8_t) (getIntensity(1) * 255));

    for (int i = 0; i < MATRIX_ROWS; i++)
    {
        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            write_flash_data(BASIC_PARAMS_NUM + 0 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, getNormalKeyCode(0, i, j));
            write_flash_data(BASIC_PARAMS_NUM + 1 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, getNormalKeyCode(1, i, j));

            write_flash_data(BASIC_PARAMS_NUM + 2 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, getNormalModifiers(0, i, j));
            write_flash_data(BASIC_PARAMS_NUM + 3 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, getNormalModifiers(1, i, j));

            write_flash_data(BASIC_PARAMS_NUM + (4 * MATRIX_ROWS * MATRIX_COLUMNS) + 0 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, getUpperKeyCode(0, i, j));
            write_flash_data(BASIC_PARAMS_NUM + (4 * MATRIX_ROWS * MATRIX_COLUMNS) + 1 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, getUpperKeyCode(1, i, j));

            write_flash_data(BASIC_PARAMS_NUM + (4 * MATRIX_ROWS * MATRIX_COLUMNS) + 2 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, getUpperModifiers(0, i, j));
            write_flash_data(BASIC_PARAMS_NUM + (4 * MATRIX_ROWS * MATRIX_COLUMNS) + 3 * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j, getUpperModifiers(1, i, j));
        }
    }

    for (int i = 0; i < STICK_NUM; i++)
    {
        for (int j = 0; j < STICK_DIRECTION; j++)
        {
            write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + 0 * (STICK_NUM * STICK_DIRECTION) + i * STICK_DIRECTION + j, getStickKeyCode(0, i, j));
            write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + 1 * (STICK_NUM * STICK_DIRECTION) + i * STICK_DIRECTION + j, getStickKeyCode(1, i, j));

            write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + 2 * (STICK_NUM * STICK_DIRECTION) + i * STICK_DIRECTION + j, getStickModifiers(0, i, j));
            write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + 3 * (STICK_NUM * STICK_DIRECTION) + i * STICK_DIRECTION + j, getStickModifiers(1, i, j));
        }
    }

    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 0, getNormalColor(0)->r);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 1, getNormalColor(0)->g);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 2, getNormalColor(0)->b);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 3, getUpperColor(0)->r);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 4, getUpperColor(0)->g);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 5, getUpperColor(0)->b);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 6, getShiftColor(0)->r);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 7, getShiftColor(0)->g);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 8, getShiftColor(0)->b);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 9, getNormalColor(1)->r);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 10, getNormalColor(1)->g);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 11, getNormalColor(1)->b);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 12, getUpperColor(1)->r);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 13, getUpperColor(1)->g);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 14, getUpperColor(1)->b);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 15, getShiftColor(1)->r);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 16, getShiftColor(1)->g);
    write_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 17, getShiftColor(1)->b);

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
    SEGGER_RTT_printf(0, "keymapID = %u\n", getKeymapID());

    // SEGGER_RTT_printf(0, "// Normal");
    for (int k = 0; k < 2; k++)
    {
        // SEGGER_RTT_printf(0, "\n");
        for (int i = 0; i < MATRIX_ROWS; i++)
        {
            // SEGGER_RTT_printf(0, "[ ");
            for (int j = 0; j < MATRIX_COLUMNS; j++)
            {
                setNormalKeyCode(k, i, j, read_flash_data(BASIC_PARAMS_NUM + k * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j));
                setNormalModifiers(k, i, j, read_flash_data(BASIC_PARAMS_NUM + (k + 2) * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j));
                // SEGGER_RTT_printf(0, "{%02X, %02X} ", getNormalKeyCode(k, i, j), getNormalModifiers(k, i, j));
            }
            // SEGGER_RTT_printf(0, "]\n");
        }
    }

    // SEGGER_RTT_printf(0, "// Upper");
    for (int k = 0; k < 2; k++)
    {
        // SEGGER_RTT_printf(0, "\n");
        for (int i = 0; i < MATRIX_ROWS; i++)
        {
            // SEGGER_RTT_printf(0, "[ ");
            for (int j = 0; j < MATRIX_COLUMNS; j++)
            {
                setUpperKeyCode(k, i, j, read_flash_data(BASIC_PARAMS_NUM + (4 * MATRIX_ROWS * MATRIX_COLUMNS) + k * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j));
                setUpperModifiers(k, i, j, read_flash_data(BASIC_PARAMS_NUM + (4 * MATRIX_ROWS * MATRIX_COLUMNS) + (k + 2) * (MATRIX_ROWS * MATRIX_COLUMNS) + i * MATRIX_COLUMNS + j));
                // SEGGER_RTT_printf(0, "{%02X, %02X} ", getUpperKeyCode(k, i, j), getUpperModifiers(k, i, j));
            }
            // SEGGER_RTT_printf(0, "]\n");
        }
    }

    // SEGGER_RTT_printf(0, "// Stick");
    for (int k = 0; k < 2; k++)
    {
        // SEGGER_RTT_printf(0, "\n");
        for (int i = 0; i < STICK_NUM; i++)
        {
            // SEGGER_RTT_printf(0, "[ ");
            for (int j = 0; j < STICK_DIRECTION; j++)
            {
                setStickKeyCode(k, i, j, read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + k * (STICK_NUM * STICK_DIRECTION) + i * STICK_DIRECTION + j));
                setStickModifiers(k, i, j, read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (k + 2) * (STICK_NUM * STICK_DIRECTION) + i * STICK_DIRECTION + j));
                // SEGGER_RTT_printf(0, "{%02X, %02X} ", getStickKeyCode(k, i, j), getStickModifiers(k, i, j));
            }
            // SEGGER_RTT_printf(0, "]\n");
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
        SEGGER_RTT_printf(0, "buf[%d] = %d(%02X)\n", i, buffer[i], buffer[i]);
    }
    SEGGER_RTT_printf(0, "bufsize = %d\n", bufsize);

    uint8_t buffer_sb[CFG_TUD_HID_EP_BUFSIZE] = {0x00};
    if (buffer[0] >= 0xF0 && buffer[0] <= 0xF3 && buffer[1] == 0x00)
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
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF3 && buffer[1] == 0x01)
    {
        SEGGER_RTT_printf(0, "write to layout1:\n");

        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            keymaps_normal[0][buffer[0] - 0xF0][j][0] = buffer[(2 * j) + 2];
            keymaps_normal[0][buffer[0] - 0xF0][j][1] = buffer[(2 * j + 1) + 2];
        }
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF3 && buffer[1] == 0x02)
    {
        SEGGER_RTT_printf(0, "write to layout1 upper:\n");

        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            keymaps_upper[0][buffer[0] - 0xF0][j][0] = buffer[(2 * j) + 2];
            keymaps_upper[0][buffer[0] - 0xF0][j][1] = buffer[(2 * j + 1) + 2];
        }
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF1 && buffer[1] == 0x03)
    {
        SEGGER_RTT_printf(0, "write to layout1 stick:\n");

        for (int j = 0; j < STICK_DIRECTION; j++)
        {
            keymaps_stk[0][buffer[0] - 0xF0][j][0] = buffer[(2 * j) + 2];
            keymaps_stk[0][buffer[0] - 0xF0][j][1] = buffer[(2 * j + 1) + 2];
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
        default:
            break;
        }
    }
    else if (buffer[0] == 0xF0 && buffer[1] == 0x05)
    {
        SEGGER_RTT_printf(0, "write to layout1 intensity:\n");

        switch (buffer[0])
        {
        case 0xF0:
            setIntensity(0, buffer[2]);
            break;
        default:
            break;
        }
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF3 && buffer[1] == 0x09)
    {
        SEGGER_RTT_printf(0, "write to layout2:\n");

        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            keymaps_normal[1][buffer[0] - 0xF0][j][0] = buffer[(2 * j) + 2];
            keymaps_normal[1][buffer[0] - 0xF0][j][1] = buffer[(2 * j + 1) + 2];
        }
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF3 && buffer[1] == 0x0A)
    {
        SEGGER_RTT_printf(0, "write to layout2 upper:\n");

        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            keymaps_upper[1][buffer[0] - 0xF0][j][0] = buffer[(2 * j) + 2];
            keymaps_upper[1][buffer[0] - 0xF0][j][1] = buffer[(2 * j + 1) + 2];
        }
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF1 && buffer[1] == 0x0B)
    {
        SEGGER_RTT_printf(0, "write to layout2 stick:\n");

        for (int j = 0; j < STICK_DIRECTION; j++)
        {
            keymaps_stk[1][buffer[0] - 0xF0][j][0] = buffer[(2 * j) + 2];
            keymaps_stk[1][buffer[0] - 0xF0][j][1] = buffer[(2 * j + 1) + 2];
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
        default:
            break;
        }
    }
    else if (buffer[0] == 0xF0 && buffer[1] == 0x0D)
    {
        SEGGER_RTT_printf(0, "write to layout2 intensity:\n");

        switch (buffer[0])
        {
        case 0xF0:
            setIntensity(1, buffer[2]);
            break;
        default:
            break;
        }
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF3 && buffer[1] == 0x11)
    {
        SEGGER_RTT_printf(0, "read from layout1:\n");
        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            buffer_sb[2 * j]     = keymaps_normal[0][buffer[0] - 0xF0][j][0];
            buffer_sb[2 * j + 1] = keymaps_normal[0][buffer[0] - 0xF0][j][1];
        }

        tud_hid_n_report(ITF_NUM_HID_GIO, 0, buffer_sb, CFG_TUD_HID_EP_BUFSIZE);
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF3 && buffer[1] == 0x12)
    {
        SEGGER_RTT_printf(0, "read from layout1 upper:\n");
        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            buffer_sb[2 * j]     = keymaps_upper[0][buffer[0] - 0xF0][j][0];
            buffer_sb[2 * j + 1] = keymaps_upper[0][buffer[0] - 0xF0][j][1];
        }

        tud_hid_n_report(ITF_NUM_HID_GIO, 0, buffer_sb, CFG_TUD_HID_EP_BUFSIZE);
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF1 && buffer[1] == 0x13)
    {
        SEGGER_RTT_printf(0, "read from layout1 stick:\n");
        for (int j = 0; j < STICK_DIRECTION; j++)
        {
            buffer_sb[2 * j]     = keymaps_stk[0][buffer[0] - 0xF0][j][0];
            buffer_sb[2 * j + 1] = keymaps_stk[0][buffer[0] - 0xF0][j][1];
        }

        tud_hid_n_report(ITF_NUM_HID_GIO, 0, buffer_sb, CFG_TUD_HID_EP_BUFSIZE);
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF2 && buffer[1] == 0x14)
    {
        SEGGER_RTT_printf(0, "read from layout1 led:\n");
        switch (buffer[0])
        {
        case 0xF0:
            const RGB_Color_t* rgb_normal = getNormalColor(0);
            buffer_sb[0]                  = rgb_normal->r;
            buffer_sb[1]                  = rgb_normal->g;
            buffer_sb[2]                  = rgb_normal->b;
            break;
        case 0xF1:
            const RGB_Color_t* rgb_upper = getUpperColor(0);
            buffer_sb[0]                 = rgb_upper->r;
            buffer_sb[1]                 = rgb_upper->g;
            buffer_sb[2]                 = rgb_upper->b;
            break;
        case 0xF2:
            const RGB_Color_t* rgb_shift = getShiftColor(0);
            buffer_sb[0]                 = rgb_shift->r;
            buffer_sb[1]                 = rgb_shift->g;
            buffer_sb[2]                 = rgb_shift->b;
            break;
        default:
            break;
        }

        tud_hid_n_report(ITF_NUM_HID_GIO, 0, buffer_sb, CFG_TUD_HID_EP_BUFSIZE);
    }
    else if (buffer[0] == 0xF0 && buffer[1] == 0x15)
    {
        SEGGER_RTT_printf(0, "read from layout1 intensity:\n");
        switch (buffer[0])
        {
        case 0xF0:
            buffer_sb[0] = (uint8_t) (getIntensity(0) * 255.0);
            break;
        default:
            break;
        }

        tud_hid_n_report(ITF_NUM_HID_GIO, 0, buffer_sb, CFG_TUD_HID_EP_BUFSIZE);
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF3 && buffer[1] == 0x19)
    {
        SEGGER_RTT_printf(0, "read from layout2:\n");
        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            buffer_sb[2 * j]     = keymaps_normal[1][buffer[0] - 0xF0][j][0];
            buffer_sb[2 * j + 1] = keymaps_normal[1][buffer[0] - 0xF0][j][1];
        }

        tud_hid_n_report(ITF_NUM_HID_GIO, 0, buffer_sb, CFG_TUD_HID_EP_BUFSIZE);
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF3 && buffer[1] == 0x1A)
    {
        SEGGER_RTT_printf(0, "read from layout2 upper:\n");
        for (int j = 0; j < MATRIX_COLUMNS; j++)
        {
            buffer_sb[2 * j]     = keymaps_upper[1][buffer[0] - 0xF0][j][0];
            buffer_sb[2 * j + 1] = keymaps_upper[1][buffer[0] - 0xF0][j][1];
        }

        tud_hid_n_report(ITF_NUM_HID_GIO, 0, buffer_sb, CFG_TUD_HID_EP_BUFSIZE);
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF1 && buffer[1] == 0x1B)
    {
        SEGGER_RTT_printf(0, "read from layout2 stick:\n");
        for (int j = 0; j < STICK_DIRECTION; j++)
        {
            buffer_sb[2 * j]     = keymaps_stk[1][buffer[0] - 0xF0][j][0];
            buffer_sb[2 * j + 1] = keymaps_stk[1][buffer[0] - 0xF0][j][1];
        }

        tud_hid_n_report(ITF_NUM_HID_GIO, 0, buffer_sb, CFG_TUD_HID_EP_BUFSIZE);
    }
    else if (buffer[0] >= 0xF0 && buffer[0] <= 0xF2 && buffer[1] == 0x1C)
    {
        SEGGER_RTT_printf(0, "read from layout2 led:\n");

        switch (buffer[0])
        {
        case 0xF0:
            const RGB_Color_t* rgb_normal = getNormalColor(1);
            buffer_sb[0]                  = rgb_normal->r;
            buffer_sb[1]                  = rgb_normal->g;
            buffer_sb[2]                  = rgb_normal->b;
            break;
        case 0xF1:
            const RGB_Color_t* rgb_upper = getUpperColor(1);
            buffer_sb[0]                 = rgb_upper->r;
            buffer_sb[1]                 = rgb_upper->g;
            buffer_sb[2]                 = rgb_upper->b;
            break;
        case 0xF2:
            const RGB_Color_t* rgb_shift = getShiftColor(1);
            buffer_sb[0]                 = rgb_shift->r;
            buffer_sb[1]                 = rgb_shift->g;
            buffer_sb[2]                 = rgb_shift->b;
            break;
        default:
            break;
        }

        tud_hid_n_report(ITF_NUM_HID_GIO, 0, buffer_sb, CFG_TUD_HID_EP_BUFSIZE);
    }
    else if (buffer[0] == 0xF0 && buffer[1] == 0x1D)
    {
        SEGGER_RTT_printf(0, "read from layout2 intensity:\n");
        switch (buffer[0])
        {
        case 0xF0:
            buffer_sb[0] = (uint8_t) (getIntensity(1) * 255.0);
            break;
        default:
            break;
        }

        tud_hid_n_report(ITF_NUM_HID_GIO, 0, buffer_sb, CFG_TUD_HID_EP_BUFSIZE);
    }
    else if (buffer[0] == 0xF5)
    {
        SEGGER_RTT_printf(0, "erase & write FLASH...\n");

        writeAllKeyboardSettings();

        buffer_sb[1] = 0xF5;
        buffer_sb[2] = 0x01;
        tud_hid_n_report(ITF_NUM_HID_GIO, 0, buffer_sb, CFG_TUD_HID_EP_BUFSIZE);
    }
    else if (buffer[0] == 0xF6)
    {
        SEGGER_RTT_printf(0, "restart...\n");

        buffer_sb[1] = 0xF6;
        buffer_sb[2] = 0x01;
        tud_hid_n_report(ITF_NUM_HID_GIO, 0, buffer_sb, CFG_TUD_HID_EP_BUFSIZE);

        // setBootDfuFlag(false);
        HAL_Delay(100);
        NVIC_SystemReset();
    }
    else if (buffer[0] == 0xF7)
    {
        SEGGER_RTT_printf(0, "check LED color\n");

        checkColor(buffer[1], buffer[2], buffer[3]);
    }
    else if (buffer[0] == 0xF8)
    {
        SEGGER_RTT_printf(0, "change LED intensity = %d\n", buffer[1]);

        setIntensity(keymapID, buffer[1]);
        renew();
    }
    else if (buffer[0] == 0xF9)
    {
        SEGGER_RTT_printf(0, "factory reset...\n");

        factoryReset();

        buffer_sb[1] = 0xF9;
        buffer_sb[2] = 0x01;
        tud_hid_n_report(ITF_NUM_HID_GIO, 0, buffer_sb, CFG_TUD_HID_EP_BUFSIZE);
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

uint8_t getNormalKeyCode(const uint8_t keymapId, const uint8_t x, const uint8_t y)
{
    return keymaps_normal[keymapId][x][y][0];
}

uint8_t getUpperKeyCode(const uint8_t keymapId, const uint8_t x, const uint8_t y)
{
    return keymaps_upper[keymapId][x][y][0];
}

uint8_t getStickKeyCode(const uint8_t keymapId, const uint8_t id, const uint8_t direction)
{
    return keymaps_stk[keymapId][id][direction][0];
}

void setNormalKeyCode(const uint8_t keymapId, const uint8_t x, const uint8_t y, const uint8_t code)
{
    keymaps_normal[keymapId][x][y][0] = code;
}

void setUpperKeyCode(const uint8_t keymapId, const uint8_t x, const uint8_t y, const uint8_t code)
{
    keymaps_upper[keymapId][x][y][0] = code;
}

void setStickKeyCode(const uint8_t keymapId, const uint8_t id, const uint8_t direction, const uint8_t code)
{
    keymaps_stk[keymapId][id][direction][0] = code;
}

uint8_t getNormalModifiers(const uint8_t keymapId, const uint8_t x, const uint8_t y)
{
    return keymaps_normal[keymapId][x][y][1];
}

uint8_t getUpperModifiers(const uint8_t keymapId, const uint8_t x, const uint8_t y)
{
    return keymaps_upper[keymapId][x][y][1];
}

uint8_t getStickModifiers(const uint8_t keymapId, const uint8_t id, const uint8_t direction)
{
    return keymaps_stk[keymapId][id][direction][1];
}

void setNormalModifiers(const uint8_t keymapId, const uint8_t x, const uint8_t y, const uint8_t modifiers)
{
    keymaps_normal[keymapId][x][y][1] = modifiers;
}

void setUpperModifiers(const uint8_t keymapId, const uint8_t x, const uint8_t y, const uint8_t modifiers)
{
    keymaps_upper[keymapId][x][y][1] = modifiers;
}

void setStickModifiers(const uint8_t keymapId, const uint8_t id, const uint8_t direction, const uint8_t modifiers)
{
    keymaps_stk[keymapId][id][direction][1] = modifiers;
}

void switchLEDColorAccordingKeymaps(void)
{
    for (int j = 0; j < MATRIX_ROWS; j++)
    {
        for (int i = 0; i < MATRIX_COLUMNS; i++)
        {
            const int index = MATRIX_COLUMNS * j + i;
            if (index < 30)
            {
                if (isUpper)
                {
                    if (getUpperKeyCode(keymapID, j, i) != KC_NULL)
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
                    if (getNormalKeyCode(keymapID, j, i) != KC_NULL)
                    {
                        setLedBuf(index, getShiftColor(keymapID));
                    }
                    else
                    {
                        setLedBuf(index, getBlankColor());
                    }
                }
            }
            else if (index >= 36)
            {
                if (isUpper)
                {
                    if (getUpperKeyCode(keymapID, j, i) != KC_NULL)
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
                    if (getNormalKeyCode(keymapID, j, i) != KC_NULL)
                    {
                        setLedBuf(index - 6, getShiftColor(keymapID));
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
    if (code == KC_RESET)
    {
        setBootDfuFlag(false);
        SEGGER_RTT_printf(0, "Restart.\n");
        HAL_Delay(100);
        NVIC_SystemReset();
    }
    else if (code == KC_LAYOUT)
    {
        isKeymapIDChanged = false;
    }
    else if (code == KC_LNPH)
    {
        isLinePhonoSWChanged = false;
    }
    else if (code == KC_XF_CUT1 || code == KC_XF_CUT2)
    {
        isXFadeCut = false;
    }
    else if (code == KC_MGAIN_UP || code == KC_MGAIN_DOWN)
    {
        isMasterGainChanged = false;
    }
    else if (code == KC_UPPER)
    {
        if (isUpper)
        {
            resetKeys();

            isUpper         = false;
            isWheel         = false;
            isStickReturned = false;

            countReturnNeutral = MAX_COUNT_RETURN_NEUTRAL;
#if 0
            if (((keyboardHID.modifiers >> (KC_LSHIFT - KC_LCONTROL)) & 0x01) ||
                ((keyboardHID.modifiers >> (KC_RSHIFT - KC_LCONTROL)) & 0x01))
            {
                setAllLedBuf(getShiftColor(keymapID));
            }
            else
            {
                setAllLedBuf(getNormalColor(keymapID));
            }
#endif
        }
    }
    else if (code == KC_M_WHEEL)
    {
        if (isWheel)
        {
            SEGGER_RTT_printf(0, "wheel off.\n");
            isWheel = false;
        }
    }
    else if (code >= KC_LCONTROL && code <= KC_RGUI)
    {
        keyboardHID.modifiers &= ~(1 << (code - KC_LCONTROL));

        if (code == KC_LSHIFT || code == KC_RSHIFT)
        {
            isShift = false;

#if 0
            if (isUpper)
            {
                for (int j = 0; j < MATRIX_ROWS; j++)
                {
                    for (int i = 0; i < MATRIX_COLUMNS; i++)
                    {
                        const int index = MATRIX_COLUMNS * j + i;
                        if (index < 30)
                        {
                            if (getUpperKeyCode(keymapID, j, i) != KC_NULL || getUpperModifiers(keymapID, j, i) != M_NO)
                            {
                                setLedBuf(index, getUpperColor(keymapID));
                            }
                            else if (getUpperKeyCode(keymapID, j, i) == KC_NULL || getUpperModifiers(keymapID, j, i) == M_NO)
                            {
                                setLedBuf(index, getBlankColor());
                            }
                        }
                        else if (index >= 36)
                        {
                            if (getUpperKeyCode(keymapID, j, i) != KC_NULL || getUpperModifiers(keymapID, j, i) != M_NO)
                            {
                                setLedBuf(index - 6, getUpperColor(keymapID));
                            }
                            else if (getUpperKeyCode(keymapID, j, i) == KC_NULL || getUpperModifiers(keymapID, j, i) == M_NO)
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
#endif
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

    if (modifiers > M_NO)
    {
        keyboardHID.modifiers &= ~modifiers;

        if ((((modifiers & M_LS) >> 1) & 0x01) || (((modifiers & M_RS) >> 5) & 0x01))
        {
            isShift = false;

#if 0
            if (isUpper)
            {
                for (int j = 0; j < MATRIX_ROWS; j++)
                {
                    for (int i = 0; i < MATRIX_COLUMNS; i++)
                    {
                        const int index = MATRIX_COLUMNS * j + i;
                        if (index < 30)
                        {
                            if (getUpperKeyCode(keymapID, j, i) != KC_NULL || getUpperModifiers(keymapID, j, i) != M_NO)
                            {
                                setLedBuf(index, getUpperColor(keymapID));
                            }
                            else if (getUpperKeyCode(keymapID, j, i) == KC_NULL || getUpperModifiers(keymapID, j, i) == M_NO)
                            {
                                setLedBuf(index, getBlankColor());
                            }
                        }
                        else if (index >= 36)
                        {
                            if (getUpperKeyCode(keymapID, j, i) != KC_NULL || getUpperModifiers(keymapID, j, i) != M_NO)
                            {
                                setLedBuf(index - 6, getUpperColor(keymapID));
                            }
                            else if (getUpperKeyCode(keymapID, j, i) == KC_NULL || getUpperModifiers(keymapID, j, i) == M_NO)
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
#endif
        }
    }

    longPressCounter = 0;
}

void setKeys(const uint8_t code, const uint8_t modifiers)
{
    static int master_gain = 0;

    if (code == KC_LAYOUT)
    {
        if (!isKeymapIDChanged)
        {
            setKeymapID(!keymapID);
            // writeAllKeyboardSettings();

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
    else if (code == KC_LNPH)
    {
        if (!isLinePhonoSWChanged)
        {
            setLinePhonoSW(!linePhonoSW);
            // writeAllKeyboardSettings();

            isLinePhonoSWChanged = true;
        }
    }
    else if (code == KC_XF_CUT1)
    {
        isXFadeCut = true;
        HAL_Delay(5);
        send_xfade(1023);
    }
    else if (code == KC_XF_CUT2)
    {
        isXFadeCut = true;
        HAL_Delay(5);
        send_xfade(0);
    }
    else if (code == KC_MGAIN_UP)
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
    else if (code == KC_MGAIN_DOWN)
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
    else if (code == KC_UPPER)
    {
        if (!isUpper)
        {
            isUpper = true;
#if 0
            for (int j = 0; j < MATRIX_ROWS; j++)
            {
                for (int i = 0; i < MATRIX_COLUMNS; i++)
                {
                    const int index = MATRIX_COLUMNS * j + i;
                    if (index < 30)
                    {
                        if ((((keyboardHID.modifiers >> (KC_LSHIFT - KC_LCONTROL)) & 0x01) ||
                             ((keyboardHID.modifiers >> (KC_RSHIFT - KC_LCONTROL)) & 0x01)))
                        {
                            if (getUpperKeyCode(keymapID, j, i) != KC_NULL || getUpperModifiers(keymapID, j, i) != M_NO)
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
                            if (getUpperKeyCode(keymapID, j, i) != KC_NULL || getUpperModifiers(keymapID, j, i) != M_NO)
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
                        if ((((keyboardHID.modifiers >> (KC_LSHIFT - KC_LCONTROL)) & 0x01) ||
                             ((keyboardHID.modifiers >> (KC_RSHIFT - KC_LCONTROL)) & 0x01)))
                        {
                            if (getUpperKeyCode(keymapID, j, i) != KC_NULL || getUpperModifiers(keymapID, j, i) != M_NO)
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
                            if (getUpperKeyCode(keymapID, j, i) != KC_NULL || getUpperModifiers(keymapID, j, i) != M_NO)
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
#endif
        }
    }
    else if (code == KC_M_WHEEL)
    {
        if (!isWheel)
        {
            SEGGER_RTT_printf(0, "wheel on.\n");
            isWheel = true;
        }
    }
    else if (code >= KC_LCONTROL && code <= KC_RGUI)
    {
        if (!((keyboardHID.modifiers >> (KC_LSHIFT - KC_LCONTROL)) & 0x01) &&
            !((keyboardHID.modifiers >> (KC_RSHIFT - KC_LCONTROL)) & 0x01))
        {
            if (code == KC_LSHIFT || code == KC_RSHIFT)
            {
                isShift = true;

                // switchLEDColorAccordingKeymaps();
            }
        }

        keyboardHID.modifiers |= 1 << (code - KC_LCONTROL);
    }
    else
    {
        for (int k = 0; k < 6; k++)
        {
            if (keyboardHID.key[k] == code)
            {
                // RESET or LGUI + ESC 長押しでDFUモーでリセット
                if (keyboardHID.key[k] == KC_RESET || (keyboardHID.modifiers == 0x08 && keyboardHID.key[k] == KC_ESC))
                {
                    longPressCounter++;
                    SEGGER_RTT_printf(0, "longPressCounter = %d\n", longPressCounter);
                    if (longPressCounter == MAX_LONG_PRESS_COUNT)
                    {
                        setBootDfuFlag(true);
                        SEGGER_RTT_printf(0, "Reboot in custom DFU...\n");
                        HAL_Delay(100);
                        NVIC_SystemReset();
                    }
                }
                break;
            }
            if (keyboardHID.key[k] == 0x00)
            {
                if (code != KC_NULL)
                {
                    keyboardHID.key[k] = code;
                }

#if 0
                if (code == keymaps_stk[keymapID][0][3][0])  // L JoyStick -> Tilt left
                {
                    setBackspaceFlag();
                }
                else if (code == keymaps_stk[keymapID][1][5][0])  // R JoyStick -> Tilt right
                {
                    setSpaceFlag();
                }
                else if (code == keymaps_stk[keymapID][0][7][0])  // R JoyStick -> Tilt down
                {
                    setEnterFlag();
                }
#endif
                break;
            }
        }
    }

    if (modifiers > M_NO)
    {
        if (!((keyboardHID.modifiers >> (KC_LSHIFT - KC_LCONTROL)) & 0x01) &&
            !((keyboardHID.modifiers >> (KC_RSHIFT - KC_LCONTROL)) & 0x01))
        {
            if ((((modifiers & M_LS) >> 1) & 0x01) || (((modifiers & M_RS) >> 5) & 0x01))
            {
                isShift = true;

                // switchLEDColorAccordingKeymaps();
            }
        }

        keyboardHID.modifiers |= modifiers;
    }
}

void controlJoySticks(void)
{
    for (int i = 0; i < JOYSTICK_NUMS; i++)
    {
        double x = (double) (2048 - pot_value[2 * i + 1]) / 2048.0;
        double y = (double) (pot_value[2 * i + 2] - 2048) / 2048.0;

        if (offset_calibrate_count[i] == 0)
        {
            x_offset[i] = x;
            y_offset[i] = y;
            offset_calibrate_count[i]++;

            continue;
        }
        if (offset_calibrate_count[i] < 100)
        {
            x_offset[i] += x;
            y_offset[i] += y;
            offset_calibrate_count[i]++;

            continue;
        }
        if (offset_calibrate_count[i] == 100)
        {
            x_offset[i] /= (double) offset_calibrate_count[i];
            y_offset[i] /= (double) offset_calibrate_count[i];
            offset_calibrate_count[i]++;

            continue;
        }

        x = x - x_offset[i];
        y = y - y_offset[i];

        const double r = sqrt(pow(x, 2.0) + pow(y, 2.0));

        if (i == 1)
        {
            mouseHID.x = (int8_t) (x * MAX_MOUSE_SENSITIVITY);
            mouseHID.y = (int8_t) (y * -MAX_MOUSE_SENSITIVITY);

            mouseHID.vertical = (int8_t) (y * MAX_WHEEL_SENSITIVITY);
        }

        if (r > JOYSTICK_ON_RADIUS)
        {
            const double theta = (y >= 0.0 ? 1.0 : -1.0) * acos(x / r) / M_PI * 180.0;

            if (theta >= 90 - JOYSTICK_ON_ANGLE2 && theta < 90 + JOYSTICK_ON_ANGLE2)
            {
                // SEGGER_RTT_printf(0, "%d:up (%d)\n", i, (int) theta);
                if (isUpper && i == 1)
                {
                    currentStk[i][JOYSTICK_V] = 0;
                    setMouseMark(1);
                }
                else
                {
                    currentStk[i][JOYSTICK_V] = -1;
                }
            }
            else if (theta >= 135 - JOYSTICK_ON_ANGLE2 && theta < 135 + JOYSTICK_ON_ANGLE2)
            {
                // SEGGER_RTT_printf(0, "%d:up left (%d)\n", i, (int) theta);
                if (isUpper && i == 1)
                {
                    currentStk[i][JOYSTICK_H] = 0;
                    currentStk[i][JOYSTICK_V] = 0;
                    setMouseMark(0);
                }
                else
                {
                    currentStk[i][JOYSTICK_H] = -1;
                    currentStk[i][JOYSTICK_V] = -1;
                }
            }
            else if (theta >= 45 - JOYSTICK_ON_ANGLE2 && theta < 45 + JOYSTICK_ON_ANGLE2)
            {
                // SEGGER_RTT_printf(0, "%d:up right (%d)\n", i, (int) theta);
                if (isUpper && i == 1)
                {
                    currentStk[i][JOYSTICK_H] = 0;
                    currentStk[i][JOYSTICK_V] = 0;
                    setMouseMark(2);
                }
                else
                {
                    currentStk[i][JOYSTICK_H] = 1;
                    currentStk[i][JOYSTICK_V] = -1;
                }
            }
            else if (theta >= -135 - JOYSTICK_ON_ANGLE2 && theta < -135 + JOYSTICK_ON_ANGLE2)
            {
                // SEGGER_RTT_printf(0, "%d:down left (%d)\n", i, (int) theta);
                if (isUpper && i == 1)
                {
                    currentStk[i][JOYSTICK_H] = 0;
                    currentStk[i][JOYSTICK_V] = 0;
                    setMouseMark(6);
                }
                else
                {
                    currentStk[i][JOYSTICK_H] = -1;
                    currentStk[i][JOYSTICK_V] = 1;
                }
            }
            else if (theta >= -45 - JOYSTICK_ON_ANGLE2 && theta < -45 + JOYSTICK_ON_ANGLE2)
            {
                // SEGGER_RTT_printf(0, "%d:down right (%d)\n", i, (int) theta);
                if (isUpper && i == 1)
                {
                    currentStk[i][JOYSTICK_H] = 0;
                    currentStk[i][JOYSTICK_V] = 0;
                    setMouseMark(8);
                }
                else
                {
                    currentStk[i][JOYSTICK_H] = 1;
                    currentStk[i][JOYSTICK_V] = 1;
                }
            }
            else if (theta >= -90 - JOYSTICK_ON_ANGLE && theta < -90 + JOYSTICK_ON_ANGLE)
            {
                // SEGGER_RTT_printf(0, "%d:down (%d)\n", i, (int) theta);
                if (isUpper && i == 1)
                {
                    currentStk[i][JOYSTICK_V] = 0;
                    setMouseMark(7);
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
                    setMouseMark(3);
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
                    setMouseMark(5);
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

            if (i == 1)
            {
                isStickReturned = false;
            }
        }
        else
        {
            currentStk[i][JOYSTICK_H] = 0;
            currentStk[i][JOYSTICK_V] = 0;

            if (isUpper && i == 1 && !isStickReturned)
            {
                SEGGER_RTT_printf(0, "stick returned\n");
                clearMouseMark();

                isStickReturned = true;
            }
        }
    }

    for (int i = 0; i < JOYSTICK_NUMS; i++)
    {
        if (currentStk[i][JOYSTICK_H] != 0 && currentStk[i][JOYSTICK_V] != 0 &&
            (prevStk[i][JOYSTICK_H] == 0 || prevStk[i][JOYSTICK_V] == 0))
        {
            // SEGGER_RTT_printf(0, "currentStk[%d][H] = %d\n", i, currentStk[i][JOYSTICK_H]);
            // SEGGER_RTT_printf(0, "currentStk[%d][V] = %d\n", i, currentStk[i][JOYSTICK_V]);

            if (currentStk[i][JOYSTICK_H] == -1 && currentStk[i][JOYSTICK_V] == -1)
            {
                // SEGGER_RTT_printf(0, "UL: set\n");
                setKeys(keymaps_stk[keymapID][i][0][0], keymaps_stk[keymapID][i][0][1]);
                setMark(i, 0);
            }
            else if (currentStk[i][JOYSTICK_H] == 1 && currentStk[i][JOYSTICK_V] == -1)
            {
                // SEGGER_RTT_printf(0, "UR: set\n");
                setKeys(keymaps_stk[keymapID][i][2][0], keymaps_stk[keymapID][i][2][1]);
                setMark(i, 2);
            }
            else if (currentStk[i][JOYSTICK_H] == -1 && currentStk[i][JOYSTICK_V] == 1)
            {
                // SEGGER_RTT_printf(0, "DL: set\n");
                setKeys(keymaps_stk[keymapID][i][6][0], keymaps_stk[keymapID][i][6][1]);
                setMark(i, 6);
            }
            else if (currentStk[i][JOYSTICK_H] == 1 && currentStk[i][JOYSTICK_V] == 1)
            {
                // SEGGER_RTT_printf(0, "DR: set\n");
                setKeys(keymaps_stk[keymapID][i][8][0], keymaps_stk[keymapID][i][8][1]);
                setMark(i, 8);
            }
        }
        else
        {
            for (int j = 0; j < JOYSTICK_AXIS; j++)
            {
                if ((currentStk[i][JOYSTICK_H] == 0 && currentStk[i][JOYSTICK_V] == 0) && (prevStk[i][JOYSTICK_H] == -1 && prevStk[i][JOYSTICK_V] == -1))
                {
                    SEGGER_RTT_printf(0, "UL: clear\n");
                    clearKeys(keymaps_stk[keymapID][i][0][0], keymaps_stk[keymapID][i][0][1]);
                    clearMark(i, 0);

                    countReturnNeutral = MAX_COUNT_RETURN_NEUTRAL;
                }
                else if ((currentStk[i][JOYSTICK_H] == 0 && currentStk[i][JOYSTICK_V] == 0) && (prevStk[i][JOYSTICK_H] == 1 && prevStk[i][JOYSTICK_V] == -1))
                {
                    SEGGER_RTT_printf(0, "UR: clear\n");
                    clearKeys(keymaps_stk[keymapID][i][2][0], keymaps_stk[keymapID][i][2][1]);
                    clearMark(i, 2);

                    countReturnNeutral = MAX_COUNT_RETURN_NEUTRAL;
                }
                else if ((currentStk[i][JOYSTICK_H] == 0 && currentStk[i][JOYSTICK_V] == 0) && (prevStk[i][JOYSTICK_H] == -1 && prevStk[i][JOYSTICK_V] == 1))
                {
                    SEGGER_RTT_printf(0, "DL: clear\n");
                    clearKeys(keymaps_stk[keymapID][i][6][0], keymaps_stk[keymapID][i][6][1]);
                    clearMark(i, 6);

                    countReturnNeutral = MAX_COUNT_RETURN_NEUTRAL;
                }
                else if ((currentStk[i][JOYSTICK_H] == 0 && currentStk[i][JOYSTICK_V] == 0) && (prevStk[i][JOYSTICK_H] == 1 && prevStk[i][JOYSTICK_V] == 1))
                {
                    SEGGER_RTT_printf(0, "DR: clear\n");
                    clearKeys(keymaps_stk[keymapID][i][8][0], keymaps_stk[keymapID][i][8][1]);
                    clearMark(i, 8);

                    countReturnNeutral = MAX_COUNT_RETURN_NEUTRAL;
                }
                else if (currentStk[i][j] != prevStk[i][j])
                {
                    // SEGGER_RTT_printf(0, "currentStk[%d][%d] = %d (%d, %d, %d, %d)\n", i, j, currentStk[i][j], pot_value[1], pot_value[2], pot_value[3], pot_value[4]);

                    if (currentStk[i][j] == -1 || currentStk[i][j] == 1)
                    {
                        const int direction = (j == JOYSTICK_H) ? (currentStk[i][j] + 4) : (3 * currentStk[i][j] + 4);
                        setKeys(keymaps_stk[keymapID][i][direction][0], keymaps_stk[keymapID][i][direction][1]);
                        setMark(i, direction);
                    }
                    else if (prevStk[i][j] == -1 || prevStk[i][j] == 1)
                    {
                        const int direction = (j == JOYSTICK_H) ? (prevStk[i][j] + 4) : (3 * prevStk[i][j] + 4);
                        clearKeys(keymaps_stk[keymapID][i][direction][0], keymaps_stk[keymapID][i][direction][1]);
                        clearMark(i, direction);

                        if (keymaps_stk[keymapID][i][direction][0] == KC_UPPER ||
                            (keymaps_stk[keymapID][i][direction][0] >= KC_LCONTROL && keymaps_stk[keymapID][i][direction][0] <= KC_RGUI))
                        {
                            countReturnNeutral = MAX_COUNT_RETURN_NEUTRAL;
                        }
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

bool isXFadeCutPressed(void)
{
    return isXFadeCut;
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
            else if (j >= 14)
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
                    }
                    else
                    {
                        keyState[i] &= ~((uint16_t) 1 << jj);

                        if (((keyState[i] >> jj) & 0x0001) != ((prevKeyState[i] >> jj) & 0x0001))
                        {
                            uint8_t keycode   = getNormalKeyCode(keymapID, i, (MATRIX_COLUMNS - 1) - jj);
                            uint8_t modifiers = getNormalModifiers(keymapID, i, (MATRIX_COLUMNS - 1) - jj);

                            if (isUpper && keycode == KC_UPPER)
                            {
                                clearKeys(keycode, modifiers);
                                countReturnNeutral = MAX_COUNT_RETURN_NEUTRAL;
                            }
                            else
                            {
                                if (isUpper)
                                {
                                    keycode   = getUpperKeyCode(keymapID, i, (MATRIX_COLUMNS - 1) - jj);
                                    modifiers = getUpperModifiers(keymapID, i, (MATRIX_COLUMNS - 1) - jj);
                                }
                                if (keycode == KC_M_LBTN || keycode == KC_M_RBTN)
                                {
                                    mouseHID.buttons = 0;
                                    isClicked        = true;
                                }
                                else
                                {
                                    clearKeys(keycode, modifiers);
                                }
                            }
                        }
                    }
                }
                else
                {
                    if (countReturnNeutral > 0)
                    {
                        countReturnNeutral--;
                    }
                    else
                    {

                        keyState[i] |= ((uint16_t) 1 << jj);

                        uint8_t keycode   = getNormalKeyCode(keymapID, i, (MATRIX_COLUMNS - 1) - jj);
                        uint8_t modifiers = getNormalModifiers(keymapID, i, (MATRIX_COLUMNS - 1) - jj);

                        if (keycode == KC_UPPER)
                        {
                            setKeys(keycode, modifiers);
                        }
                        else
                        {
                            if (isUpper)
                            {
                                keycode   = getUpperKeyCode(keymapID, i, (MATRIX_COLUMNS - 1) - jj);
                                modifiers = getUpperModifiers(keymapID, i, (MATRIX_COLUMNS - 1) - jj);
                            }
                            if (keycode == KC_M_LBTN)
                            {
                                mouseHID.buttons = MOUSE_LEFT_CLICK;
                                isClicked        = true;
                            }
                            else if (keycode == KC_M_RBTN)
                            {
                                mouseHID.buttons = MOUSE_RIGHT_CLICK;
                                isClicked        = true;
                            }
                            else
                            {
                                setKeys(keycode, modifiers);
                            }
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
                if (isUpper && !isShift && (abs(mouseHID.x) > MIN_MOUSE_THRESHOLD || abs(mouseHID.y) > MIN_MOUSE_THRESHOLD || isClicked))
                {
                    // SEGGER_RTT_printf(0, "(x, y) = (%d, %d)\n", mouseHID.x, mouseHID.y);

                    if (!tud_hid_ready())
                        return;

                    if (isWheel)
                    {
                        if (mouseHID.vertical != mouseHID.vertical_prev)
                        {
                            // SEGGER_RTT_printf(0, "wheel = %d\n", mouseHID.vertical);
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
            }
        }

        for (int k = 0; k < MATRIX_ROWS; k++)
        {
            prevKeyState[k] = keyState[k];
        }

        for (int j = 0; j < JOYSTICK_NUMS; j++)
        {
            for (int k = 0; k < JOYSTICK_AXIS; k++)
            {
                prevStk[j][k] = currentStk[j][k];
            }
        }
        i = 0;
        break;
    default:
        i = 0;
        break;
    }
}

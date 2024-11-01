/*
 * icled.c
 *
 *  Created on: Aug 7, 2024
 *      Author: shun
 */

#include "icled.h"

#include "sai.h"
#include "keyboard.h"
#include "tim.h"

RGB_Color_t rgb_normal[2] = {
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00}
};
RGB_Color_t rgb_upper[2] = {
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00}
};
RGB_Color_t rgb_shift[2] = {
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00}
};
RGB_Color_t rgb_blank = {0x00, 0x00, 0x00};

double intensity[2] = {1.0, 1.0};

uint8_t grb_prev[LED_NUMS][RGB]    = {0};
uint8_t grb_current[LED_NUMS][RGB] = {0};
uint8_t grb[LED_NUMS][RGB]         = {0};

uint32_t led_buf[LED_NUMS * RGB * COL_BITS + 1]      = {0};
uint32_t led_buf_prev[LED_NUMS * RGB * COL_BITS + 1] = {0};

uint32_t counter = 0;

bool isGradation = false;
int count        = 0;
double g_rate    = 0.0;

bool isLeftMarked     = false;
int countLeftMark     = 0;
double fadeLeftMark   = 0.0;
uint8_t stateLeftMark = 0;

bool isRightMarked     = false;
int countRightMark     = 0;
double fadeRightMark   = 0.0;
uint8_t stateRightMark = 0;

bool isMouseMarked = false;
int countMouseMark     = 0;
double fadeMouseMark   = 0.0;
uint8_t stateMouseMark = 4;

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef* htim)
{
    HAL_TIM_PWM_Stop_DMA(htim, TIM_CHANNEL_1);
}

void setNormalColor(const uint8_t keymapId, const uint8_t r, const uint8_t g, const uint8_t b)
{
    rgb_normal[keymapId].r = r;
    rgb_normal[keymapId].g = g;
    rgb_normal[keymapId].b = b;
}

void setUpperColor(const uint8_t keymapId, const uint8_t r, const uint8_t g, const uint8_t b)
{
    rgb_upper[keymapId].r = r;
    rgb_upper[keymapId].g = g;
    rgb_upper[keymapId].b = b;
}

void setShiftColor(const uint8_t keymapId, const uint8_t r, const uint8_t g, const uint8_t b)
{
    rgb_shift[keymapId].r = r;
    rgb_shift[keymapId].g = g;
    rgb_shift[keymapId].b = b;
}

RGB_Color_t* getNormalColor(const uint8_t keymapId)
{
    return &rgb_normal[keymapId];
}

RGB_Color_t* getUpperColor(const uint8_t keymapId)
{
    return &rgb_upper[keymapId];
}

RGB_Color_t* getShiftColor(const uint8_t keymapId)
{
    return &rgb_shift[keymapId];
}

RGB_Color_t* getBlankColor(void)
{
    return &rgb_blank;
}

void setIntensity(const uint8_t keymapId, const uint8_t value)
{
    intensity[keymapId] = (double) value / 255.0;
}

double getIntensity(const uint8_t keymapId)
{
    return intensity[keymapId];
}

void setMark(const uint8_t index, const uint8_t state)
{
    SEGGER_RTT_printf(0, "setMark(%d, %d)\n", index, state);
    if (index == 0)
    {
        isLeftMarked  = false;
        countLeftMark = 0;
        fadeLeftMark  = 1.0;
        stateLeftMark = state;
        setLedMarkForJoystick(0, stateLeftMark);
    }
    else if (index == 1)
    {
        isRightMarked  = false;
        countRightMark = 0;
        fadeRightMark  = 1.0;
        stateRightMark = state;
        setLedMarkForJoystick(1, stateRightMark);
    }
}

void clearMark(const uint8_t index, const uint8_t state)
{
    SEGGER_RTT_printf(0, "clearMark(%d, %d)\n", index, state);
    if (index == 0)
    {
        isLeftMarked  = true;
        countLeftMark = 0;
        fadeLeftMark  = 1.0;
        stateLeftMark = state;
        setLedMarkForJoystick(0, stateLeftMark);
    }
    else if (index == 1)
    {
        isRightMarked  = true;
        countRightMark = 0;
        fadeRightMark  = 1.0;
        stateRightMark = state;
        setLedMarkForJoystick(1, stateRightMark);
    }
}

void setMouseMark(const uint8_t state)
{
    isMouseMarked  = false;
    countMouseMark = 0;
    fadeMouseMark  = 1.0;
    stateMouseMark = state;
    setLedMouseMarkForJoystick(stateMouseMark);
}

void clearMouseMark(void)
{
    isMouseMarked  = true;
    countMouseMark = 0;
    fadeMouseMark  = 1.0;
    setLedMouseMarkForJoystick(stateMouseMark);
}

void setMouseOrigin(void)
{
    stateMouseMark = 4;
}

void setLedBufDirect(const uint8_t index, const RGB_Color_t* rgb_color)
{
    grb[index][0] = (uint8_t) ((double) rgb_color->g);
    grb[index][1] = (uint8_t) ((double) rgb_color->r);
    grb[index][2] = (uint8_t) ((double) rgb_color->b);
}

void setAllLedBufDirect(const RGB_Color_t* rgb_color)
{
    for (int i = 0; i < LED_NUMS; i++)
    {
        grb[i][0] = (uint8_t) ((double) rgb_color->g);
        grb[i][1] = (uint8_t) ((double) rgb_color->r);
        grb[i][2] = (uint8_t) ((double) rgb_color->b);
    }
}

void setLedBuf(const uint8_t index, const RGB_Color_t* rgb_color)
{
    grb_prev[index][0] = grb_current[index][0];
    grb_prev[index][1] = grb_current[index][1];
    grb_prev[index][2] = grb_current[index][2];

    grb_current[index][0] = (uint8_t) ((double) rgb_color->g);
    grb_current[index][1] = (uint8_t) ((double) rgb_color->r);
    grb_current[index][2] = (uint8_t) ((double) rgb_color->b);

    if (grb_prev[index][0] != grb_current[index][0] ||
        grb_prev[index][1] != grb_current[index][1] ||
        grb_prev[index][2] != grb_current[index][2])
    {
        isGradation = true;
        g_rate      = 0.0;
    }
}

void setAllLedBuf(const RGB_Color_t* rgb_color)
{
    for (int i = 0; i < LED_NUMS; i++)
    {
        grb_prev[i][0] = grb_current[i][0];
        grb_prev[i][1] = grb_current[i][1];
        grb_prev[i][2] = grb_current[i][2];

        grb_current[i][0] = (uint8_t) ((double) rgb_color->g);
        grb_current[i][1] = (uint8_t) ((double) rgb_color->r);
        grb_current[i][2] = (uint8_t) ((double) rgb_color->b);

        if (grb_prev[i][0] != grb_current[i][0] ||
            grb_prev[i][1] != grb_current[i][1] ||
            grb_prev[i][2] != grb_current[i][2])
        {
            isGradation = true;
            g_rate      = 0.0;
        }
    }
}

void setColumnColorLedBuf(const uint8_t row, const uint16_t column, const RGB_Color_t color, const double fade)
{
    const RGB_Color_t c  = {(uint8_t) ((double) color.r * fade), (uint8_t) ((double) color.g * fade), (uint8_t) ((double) color.b * fade)};
    const RGB_Color_t bc = {0x00, 0x00, 0x00};

    // SEGGER_RTT_printf(0, "o(r, g, b) = (%d, %d, %d)\n", color.r, color.g, color.b);
    // SEGGER_RTT_printf(0, "m(r, g, b) = (%d, %d, %d)\n", c.r, c.g, c.b);

    for (int i = 0; i < 10; i++)
    {
        if ((column >> (9 - i)) & 0x01)
        {
            setLedBufDirect(i + row * 10, &c);
        }
        else
        {
            setLedBufDirect(i + row * 10, &bc);
        }
    }
}

void setLeftHalfColumnColorLedBuf(const uint8_t row, const uint16_t column, const RGB_Color_t color, const double fade)
{
    const RGB_Color_t c  = {(uint8_t) ((double) color.r * fade), (uint8_t) ((double) color.g * fade), (uint8_t) ((double) color.b * fade)};
    const RGB_Color_t bc = {0x00, 0x00, 0x00};

    // SEGGER_RTT_printf(0, "o(r, g, b) = (%d, %d, %d)\n", color.r, color.g, color.b);
    // SEGGER_RTT_printf(0, "m(r, g, b) = (%d, %d, %d)\n", c.r, c.g, c.b);

    for (int i = 0; i < 5; i++)
    {
        if ((column >> (9 - i)) & 0x01)
        {
            setLedBufDirect(i + row * 10, &c);
        }
        else
        {
            setLedBufDirect(i + row * 10, &bc);
        }
    }
}

void setRightHalfColumnColorLedBuf(const uint8_t row, const uint16_t column, const RGB_Color_t color, const double fade)
{
    const RGB_Color_t c  = {(uint8_t) ((double) color.r * fade), (uint8_t) ((double) color.g * fade), (uint8_t) ((double) color.b * fade)};
    const RGB_Color_t bc = {0x00, 0x00, 0x00};

    // SEGGER_RTT_printf(0, "o(r, g, b) = (%d, %d, %d)\n", color.r, color.g, color.b);
    // SEGGER_RTT_printf(0, "m(r, g, b) = (%d, %d, %d)\n", c.r, c.g, c.b);

    for (int i = 5; i < 10; i++)
    {
        if ((column >> (9 - i)) & 0x01)
        {
            setLedBufDirect(i + row * 10, &c);
        }
        else
        {
            setLedBufDirect(i + row * 10, &bc);
        }
    }
}

void gradation(const uint8_t index, const double rate)
{
    for (int k = 0; k < RGB; k++)
    {
        grb[index][k] = (uint8_t) ((double) (grb_current[index][k] - grb_prev[index][k]) * rate + (double) grb_prev[index][k]);
    }
}

void gradationAll(const double rate)
{
    for (int i = 0; i < LED_NUMS; i++)
    {
        for (int k = 0; k < RGB; k++)
        {
            grb[i][k] = (uint8_t) ((double) (grb_current[i][k] - grb_prev[i][k]) * rate + (double) grb_prev[i][k]);
        }
    }
}

void setLedMarkForJoystick(const uint8_t index, const uint8_t state)
{
    // SEGGER_RTT_printf(0, "set: %d, %d, %d\n", index, state, isLeftMarked);

    if (index == 0)
    {
        switch (state)
        {
        case 0:
            setLeftHalfColumnColorLedBuf(0, 0b0110000000, rgb_normal[getKeymapID()], fadeLeftMark);
            setLeftHalfColumnColorLedBuf(1, 0b0100000000, rgb_normal[getKeymapID()], fadeLeftMark);
            setLeftHalfColumnColorLedBuf(2, 0b0000000000, rgb_normal[getKeymapID()], fadeLeftMark);
            break;
        case 1:
            setLeftHalfColumnColorLedBuf(0, 0b0010000000, rgb_normal[getKeymapID()], fadeLeftMark);
            setLeftHalfColumnColorLedBuf(1, 0b0101000000, rgb_normal[getKeymapID()], fadeLeftMark);
            setLeftHalfColumnColorLedBuf(2, 0b0000000000, rgb_normal[getKeymapID()], fadeLeftMark);
            break;
        case 2:
            setLeftHalfColumnColorLedBuf(0, 0b0011000000, rgb_normal[getKeymapID()], fadeLeftMark);
            setLeftHalfColumnColorLedBuf(1, 0b0001000000, rgb_normal[getKeymapID()], fadeLeftMark);
            setLeftHalfColumnColorLedBuf(2, 0b0000000000, rgb_normal[getKeymapID()], fadeLeftMark);
            break;
        case 3:
            setLeftHalfColumnColorLedBuf(0, 0b0010000000, rgb_normal[getKeymapID()], fadeLeftMark);
            setLeftHalfColumnColorLedBuf(1, 0b0100000000, rgb_normal[getKeymapID()], fadeLeftMark);
            setLeftHalfColumnColorLedBuf(2, 0b0010000000, rgb_normal[getKeymapID()], fadeLeftMark);
            break;
        case 5:
            setLeftHalfColumnColorLedBuf(0, 0b0010000000, rgb_normal[getKeymapID()], fadeLeftMark);
            setLeftHalfColumnColorLedBuf(1, 0b0001000000, rgb_normal[getKeymapID()], fadeLeftMark);
            setLeftHalfColumnColorLedBuf(2, 0b0010000000, rgb_normal[getKeymapID()], fadeLeftMark);
            break;
        case 6:
            setLeftHalfColumnColorLedBuf(0, 0b0000000000, rgb_normal[getKeymapID()], fadeLeftMark);
            setLeftHalfColumnColorLedBuf(1, 0b0100000000, rgb_normal[getKeymapID()], fadeLeftMark);
            setLeftHalfColumnColorLedBuf(2, 0b0110000000, rgb_normal[getKeymapID()], fadeLeftMark);
            break;
        case 7:
            setLeftHalfColumnColorLedBuf(0, 0b0000000000, rgb_normal[getKeymapID()], fadeLeftMark);
            setLeftHalfColumnColorLedBuf(1, 0b0101000000, rgb_normal[getKeymapID()], fadeLeftMark);
            setLeftHalfColumnColorLedBuf(2, 0b0010000000, rgb_normal[getKeymapID()], fadeLeftMark);
            break;
        case 8:
            setLeftHalfColumnColorLedBuf(0, 0b0000000000, rgb_normal[getKeymapID()], fadeLeftMark);
            setLeftHalfColumnColorLedBuf(1, 0b0001000000, rgb_normal[getKeymapID()], fadeLeftMark);
            setLeftHalfColumnColorLedBuf(2, 0b0011000000, rgb_normal[getKeymapID()], fadeLeftMark);
            break;
        default:
            setLeftHalfColumnColorLedBuf(0, 0b0000000000, rgb_normal[getKeymapID()], fadeLeftMark);
            setLeftHalfColumnColorLedBuf(1, 0b0000000000, rgb_normal[getKeymapID()], fadeLeftMark);
            setLeftHalfColumnColorLedBuf(2, 0b0000000000, rgb_normal[getKeymapID()], fadeLeftMark);
            break;
        }
    }
    else if (index == 1)
    {
        switch (state)
        {
        case 0:
            setRightHalfColumnColorLedBuf(0, 0b0000011000, rgb_normal[getKeymapID()], fadeRightMark);
            setRightHalfColumnColorLedBuf(1, 0b0000010000, rgb_normal[getKeymapID()], fadeRightMark);
            setRightHalfColumnColorLedBuf(2, 0b0000000000, rgb_normal[getKeymapID()], fadeRightMark);
            break;
        case 1:
            setRightHalfColumnColorLedBuf(0, 0b0000001000, rgb_normal[getKeymapID()], fadeRightMark);
            setRightHalfColumnColorLedBuf(1, 0b0000010100, rgb_normal[getKeymapID()], fadeRightMark);
            setRightHalfColumnColorLedBuf(2, 0b0000000000, rgb_normal[getKeymapID()], fadeRightMark);
            break;
        case 2:
            setRightHalfColumnColorLedBuf(0, 0b0000001100, rgb_normal[getKeymapID()], fadeRightMark);
            setRightHalfColumnColorLedBuf(1, 0b0000000100, rgb_normal[getKeymapID()], fadeRightMark);
            setRightHalfColumnColorLedBuf(2, 0b0000000000, rgb_normal[getKeymapID()], fadeRightMark);
            break;
        case 3:
            setRightHalfColumnColorLedBuf(0, 0b0000001000, rgb_normal[getKeymapID()], fadeRightMark);
            setRightHalfColumnColorLedBuf(1, 0b0000010000, rgb_normal[getKeymapID()], fadeRightMark);
            setRightHalfColumnColorLedBuf(2, 0b0000001000, rgb_normal[getKeymapID()], fadeRightMark);
            break;
        case 5:
            setRightHalfColumnColorLedBuf(0, 0b0000001000, rgb_normal[getKeymapID()], fadeRightMark);
            setRightHalfColumnColorLedBuf(1, 0b0000000100, rgb_normal[getKeymapID()], fadeRightMark);
            setRightHalfColumnColorLedBuf(2, 0b0000001000, rgb_normal[getKeymapID()], fadeRightMark);
            break;
        case 6:
            setRightHalfColumnColorLedBuf(0, 0b0000000000, rgb_normal[getKeymapID()], fadeRightMark);
            setRightHalfColumnColorLedBuf(1, 0b0000010000, rgb_normal[getKeymapID()], fadeRightMark);
            setRightHalfColumnColorLedBuf(2, 0b0000011000, rgb_normal[getKeymapID()], fadeRightMark);
            break;
        case 7:
            setRightHalfColumnColorLedBuf(0, 0b0000000000, rgb_normal[getKeymapID()], fadeRightMark);
            setRightHalfColumnColorLedBuf(1, 0b0000010100, rgb_normal[getKeymapID()], fadeRightMark);
            setRightHalfColumnColorLedBuf(2, 0b0000001000, rgb_normal[getKeymapID()], fadeRightMark);
            break;
        case 8:
            setRightHalfColumnColorLedBuf(0, 0b0000000000, rgb_normal[getKeymapID()], fadeRightMark);
            setRightHalfColumnColorLedBuf(1, 0b0000000100, rgb_normal[getKeymapID()], fadeRightMark);
            setRightHalfColumnColorLedBuf(2, 0b0000001100, rgb_normal[getKeymapID()], fadeRightMark);
            break;
        default:
            setRightHalfColumnColorLedBuf(0, 0b0000000000, rgb_normal[getKeymapID()], fadeRightMark);
            setRightHalfColumnColorLedBuf(1, 0b0000000000, rgb_normal[getKeymapID()], fadeRightMark);
            setRightHalfColumnColorLedBuf(2, 0b0000000000, rgb_normal[getKeymapID()], fadeRightMark);
            break;
        }
    }
    renew();
}

void setLedMouseMarkForJoystick(const uint8_t state)
{
    // SEGGER_RTT_printf(0, "set: %d, %d, %d\n", index, state, isLeftMarked);

    switch (state)
    {
    case 0:
        setRightHalfColumnColorLedBuf(0, 0b0000010000, rgb_normal[getKeymapID()], fadeMouseMark);
        setRightHalfColumnColorLedBuf(1, 0b0000000000, rgb_normal[getKeymapID()], fadeMouseMark);
        setRightHalfColumnColorLedBuf(2, 0b0000000000, rgb_normal[getKeymapID()], fadeMouseMark);
        break;
    case 1:
        setRightHalfColumnColorLedBuf(0, 0b0000001000, rgb_normal[getKeymapID()], fadeMouseMark);
        setRightHalfColumnColorLedBuf(1, 0b0000000000, rgb_normal[getKeymapID()], fadeMouseMark);
        setRightHalfColumnColorLedBuf(2, 0b0000000000, rgb_normal[getKeymapID()], fadeMouseMark);
        break;
    case 2:
        setRightHalfColumnColorLedBuf(0, 0b0000000100, rgb_normal[getKeymapID()], fadeMouseMark);
        setRightHalfColumnColorLedBuf(1, 0b0000000000, rgb_normal[getKeymapID()], fadeMouseMark);
        setRightHalfColumnColorLedBuf(2, 0b0000000000, rgb_normal[getKeymapID()], fadeMouseMark);
        break;
    case 3:
        setRightHalfColumnColorLedBuf(0, 0b0000000000, rgb_normal[getKeymapID()], fadeMouseMark);
        setRightHalfColumnColorLedBuf(1, 0b0000010000, rgb_normal[getKeymapID()], fadeMouseMark);
        setRightHalfColumnColorLedBuf(2, 0b0000000000, rgb_normal[getKeymapID()], fadeMouseMark);
        break;
    case 4:
        setRightHalfColumnColorLedBuf(0, 0b0000000000, rgb_normal[getKeymapID()], fadeMouseMark);
        setRightHalfColumnColorLedBuf(1, 0b0000001000, rgb_normal[getKeymapID()], fadeMouseMark);
        setRightHalfColumnColorLedBuf(2, 0b0000000000, rgb_normal[getKeymapID()], fadeMouseMark);
        break;
    case 5:
        setRightHalfColumnColorLedBuf(0, 0b0000000000, rgb_normal[getKeymapID()], fadeMouseMark);
        setRightHalfColumnColorLedBuf(1, 0b0000000100, rgb_normal[getKeymapID()], fadeMouseMark);
        setRightHalfColumnColorLedBuf(2, 0b0000000000, rgb_normal[getKeymapID()], fadeMouseMark);
        break;
    case 6:
        setRightHalfColumnColorLedBuf(0, 0b0000000000, rgb_normal[getKeymapID()], fadeMouseMark);
        setRightHalfColumnColorLedBuf(1, 0b0000000000, rgb_normal[getKeymapID()], fadeMouseMark);
        setRightHalfColumnColorLedBuf(2, 0b0000010000, rgb_normal[getKeymapID()], fadeMouseMark);
        break;
    case 7:
        setRightHalfColumnColorLedBuf(0, 0b0000000000, rgb_normal[getKeymapID()], fadeMouseMark);
        setRightHalfColumnColorLedBuf(1, 0b0000000000, rgb_normal[getKeymapID()], fadeMouseMark);
        setRightHalfColumnColorLedBuf(2, 0b0000001000, rgb_normal[getKeymapID()], fadeMouseMark);
        break;
    case 8:
        setRightHalfColumnColorLedBuf(0, 0b0000000000, rgb_normal[getKeymapID()], fadeMouseMark);
        setRightHalfColumnColorLedBuf(1, 0b0000000000, rgb_normal[getKeymapID()], fadeMouseMark);
        setRightHalfColumnColorLedBuf(2, 0b0000000100, rgb_normal[getKeymapID()], fadeMouseMark);
        break;
    default:
        setRightHalfColumnColorLedBuf(0, 0b0000000000, rgb_normal[getKeymapID()], fadeMouseMark);
        setRightHalfColumnColorLedBuf(1, 0b0000000000, rgb_normal[getKeymapID()], fadeMouseMark);
        setRightHalfColumnColorLedBuf(2, 0b0000000000, rgb_normal[getKeymapID()], fadeMouseMark);
        break;
    }

    renew();
}

void renew(void)
{
    bool isRenewed = false;

    for (int j = 0; j < LED_NUMS; j++)
    {
        for (int i = 0; i < COL_BITS; i++)
        {
            for (int k = 0; k < RGB; k++)
            {
                const uint8_t val = (uint8_t) ((double) grb[j][k] * getIntensity(getKeymapID()));

                led_buf[j * WL_LED_BIT_LEN + i + COL_BITS * k] = ((val >> ((COL_BITS - 1) - i)) & 0x01) ? WL_LED_ONE : WL_LED_ZERO;

                if (led_buf_prev[j * WL_LED_BIT_LEN + i + COL_BITS * k] != led_buf[j * WL_LED_BIT_LEN + i + COL_BITS * k])
                {
                    isRenewed = true;
                }

                led_buf_prev[j * WL_LED_BIT_LEN + i + COL_BITS * k] = led_buf[j * WL_LED_BIT_LEN + i + COL_BITS * k];
            }
        }
    }
    led_buf[DMA_BUF_SIZE - 1] = 0;

    if (isRenewed)
    {
        if (hsai_BlockB1.State != HAL_SAI_STATE_BUSY)
        {
            HAL_TIM_PWM_Start_DMA(&htim8, TIM_CHANNEL_1, (uint32_t*) led_buf, DMA_BUF_SIZE);
        }
    }
}

void checkColor(const uint8_t r, const uint8_t g, const uint8_t b)
{
    RGB_Color_t rgb_check = {r, g, b};

    SEGGER_RTT_printf(0, "(r, g, b) = (%02X, %02X, %02X)\n", r, g, b);

    setAllLedBufDirect(&rgb_check);
    renew();
}

void loadLEDColorsFromFlash(void)
{
    SEGGER_RTT_printf(0, "// LED\n");
    SEGGER_RTT_printf(0, "[ ");
    setNormalColor(0, read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 0), read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 1), read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 2));
    SEGGER_RTT_printf(0, "%02X ", read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 0));
    SEGGER_RTT_printf(0, "%02X ", read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 1));
    SEGGER_RTT_printf(0, "%02X ", read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 2));
    SEGGER_RTT_printf(0, "]\n");

    SEGGER_RTT_printf(0, "[ ");
    setUpperColor(0, read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 3), read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 4), read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 5));
    SEGGER_RTT_printf(0, "%02X ", read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 3));
    SEGGER_RTT_printf(0, "%02X ", read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 4));
    SEGGER_RTT_printf(0, "%02X ", read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 5));
    SEGGER_RTT_printf(0, "]\n");

    SEGGER_RTT_printf(0, "[ ");
    setShiftColor(0, read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 6), read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 7), read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 8));
    SEGGER_RTT_printf(0, "%02X ", read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 6));
    SEGGER_RTT_printf(0, "%02X ", read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 7));
    SEGGER_RTT_printf(0, "%02X ", read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 8));
    SEGGER_RTT_printf(0, "]\n");
    SEGGER_RTT_printf(0, "\n");

    SEGGER_RTT_printf(0, "[ ");
    setNormalColor(1, read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 9), read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 10), read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 11));
    SEGGER_RTT_printf(0, "%02X ", read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 9));
    SEGGER_RTT_printf(0, "%02X ", read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 10));
    SEGGER_RTT_printf(0, "%02X ", read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 11));
    SEGGER_RTT_printf(0, "]\n");

    SEGGER_RTT_printf(0, "[ ");
    setUpperColor(1, read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 12), read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 13), read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 14));
    SEGGER_RTT_printf(0, "%02X ", read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 12));
    SEGGER_RTT_printf(0, "%02X ", read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 13));
    SEGGER_RTT_printf(0, "%02X ", read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 14));
    SEGGER_RTT_printf(0, "]\n");

    SEGGER_RTT_printf(0, "[ ");
    setShiftColor(1, read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 15), read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 16), read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 17));
    SEGGER_RTT_printf(0, "%02X ", read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 15));
    SEGGER_RTT_printf(0, "%02X ", read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 16));
    SEGGER_RTT_printf(0, "%02X ", read_flash_data(BASIC_PARAMS_NUM + (8 * MATRIX_ROWS * MATRIX_COLUMNS) + (4 * STICK_NUM * STICK_DIRECTION) + 17));
    SEGGER_RTT_printf(0, "]\n");

    setAllLedBuf(&rgb_blank);
    renew();

    SEGGER_RTT_printf(0, "// LED\n");
    SEGGER_RTT_printf(0, "[ ");
    setIntensity(0, read_flash_data(3));
    setIntensity(1, read_flash_data(4));
    SEGGER_RTT_printf(0, "%d ", read_flash_data(3));
    SEGGER_RTT_printf(0, "%d ", read_flash_data(4));
    SEGGER_RTT_printf(0, "]\n");
}

void led_control_task(void)
{
    if (isGradation)
    {
        count++;
        if (count > GRADATION_COUNT_MAX)
        {
            gradationAll(g_rate);
            renew();

            count = 0;
            g_rate += GRADATION_RATE_STEP;
            if (g_rate > 1.0)
            {
                isGradation = false;
            }
        }
    }

    if (isLeftMarked)
    {
        countLeftMark++;
        if (countLeftMark > ANIMATION_COUNT_MAX)
        {
            countLeftMark = 0;

            setLedMarkForJoystick(0, stateLeftMark);

            fadeLeftMark -= 0.075;
            if (fadeLeftMark <= 0)
            {
                fadeLeftMark = 0;
                isLeftMarked = false;
            }
        }
    }

    if (isRightMarked)
    {
        countRightMark++;
        if (countRightMark > ANIMATION_COUNT_MAX)
        {
            countRightMark = 0;

            setLedMarkForJoystick(1, stateRightMark);

            fadeRightMark -= 0.075;
            if (fadeRightMark <= 0)
            {
                fadeRightMark  = 0.0;
                isRightMarked  = false;
            }
        }
    }

    if (isMouseMarked)
    {
        countMouseMark++;
        if (countMouseMark > ANIMATION_COUNT_MAX)
        {
            countMouseMark = 0;

            setLedMouseMarkForJoystick(stateMouseMark);

            fadeMouseMark -= 0.075;
            if (fadeMouseMark <= 0)
            {
                fadeMouseMark  = 0.0;
                isMouseMarked  = false;
                stateMouseMark = 4;
            }
        }
    }
}

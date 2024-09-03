/*
 * icled.c
 *
 *  Created on: Aug 7, 2024
 *      Author: shun
 */

#include "main.h"
#include "icled.h"
#include "tim.h"

RGB_Color_t rgb_normal = {0xEC, 0x80, 0x8C};
RGB_Color_t rgb_upper  = {0xEC, 0x00, 0x8C};
RGB_Color_t rgb_shift  = {0x2C, 0x00, 0x8C};
RGB_Color_t rgb_blank  = {0x00, 0x00, 0x00};

uint8_t grb_prev[LED_NUMS][RGB]    = {0};
uint8_t grb_current[LED_NUMS][RGB] = {0};
uint8_t grb[LED_NUMS][RGB]         = {0};

uint32_t led_buf[LED_NUMS * RGB * COL_BITS + 1]      = {0};
uint32_t led_buf_prev[LED_NUMS * RGB * COL_BITS + 1] = {0};

uint32_t counter = 0;

bool isGradation = false;
int count        = 0;
double g_rate    = 0.0;

bool isSpace   = false;
int countSpace = 0;
int stepSpace  = 0;

bool isBackspace   = false;
int countBackspace = 0;
int stepBackspace  = 0;

bool isEnter   = false;
int countEnter = 0;
int stepEnter  = 0;

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef* htim)
{
    HAL_TIM_PWM_Stop_DMA(htim, TIM_CHANNEL_1);
}

void setSpaceFlag(void)
{
    SEGGER_RTT_printf(0, "space on.\n");
    isSpace    = true;
    countSpace = 0;
    stepSpace  = 0;
}

void setBackspaceFlag(void)
{
    SEGGER_RTT_printf(0, "BS on.\n");
    isBackspace    = true;
    countBackspace = 0;
    stepBackspace  = 0;
}

void setEnterFlag(void)
{
    SEGGER_RTT_printf(0, "enter on.\n");
    isEnter    = true;
    countEnter = 0;
    stepEnter  = 0;
}

void setLedBuf(uint8_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    grb_prev[index][0] = grb_current[index][0];
    grb_prev[index][1] = grb_current[index][1];
    grb_prev[index][2] = grb_current[index][2];

    grb_current[index][0] = (uint8_t) ((double) green * LED_INTENSITY_RATE);
    grb_current[index][1] = (uint8_t) ((double) red * LED_INTENSITY_RATE);
    grb_current[index][2] = (uint8_t) ((double) blue * LED_INTENSITY_RATE);

    if (grb_prev[index][0] != grb_current[index][0] ||
        grb_prev[index][1] != grb_current[index][1] ||
        grb_prev[index][2] != grb_current[index][2])
    {
        isGradation = true;
        g_rate      = 0.0;
    }
}

void setAllLedBuf(uint8_t red, uint8_t green, uint8_t blue)
{
    for (int i = 0; i < LED_NUMS; i++)
    {
        grb_prev[i][0] = grb_current[i][0];
        grb_prev[i][1] = grb_current[i][1];
        grb_prev[i][2] = grb_current[i][2];

        grb_current[i][0] = (uint8_t) ((double) green * LED_INTENSITY_RATE);
        grb_current[i][1] = (uint8_t) ((double) red * LED_INTENSITY_RATE);
        grb_current[i][2] = (uint8_t) ((double) blue * LED_INTENSITY_RATE);

        if (grb_prev[i][0] != grb_current[i][0] ||
            grb_prev[i][1] != grb_current[i][1] ||
            grb_prev[i][2] != grb_current[i][2])
        {
            isGradation = true;
            g_rate      = 0.0;
        }
    }
}

void setColumn2ColorLedBuf(uint8_t row, uint16_t column, uint8_t red0, uint8_t green0, uint8_t blue0, uint8_t red1, uint8_t green1, uint8_t blue1)
{
    for (int i = 0; i < 10; i++)
    {
        if ((column >> (9 - i)) & 0x01)
        {
            setLedBuf(i + row * 10, red0, green0, blue0);
        }
        else
        {
            setLedBuf(i + row * 10, red1, green1, blue1);
        }
    }
}

void gradation(uint8_t index, double rate)
{
    for (int k = 0; k < RGB; k++)
    {
        grb[index][k] = (uint8_t) ((double) (grb_current[index][k] - grb_prev[index][k]) * rate + (double) grb_prev[index][k]);
    }
}

void gradationAll(double rate)
{
    for (int i = 0; i < LED_NUMS; i++)
    {
        for (int k = 0; k < RGB; k++)
        {
            grb[i][k] = (uint8_t) ((double) (grb_current[i][k] - grb_prev[i][k]) * rate + (double) grb_prev[i][k]);
        }
    }
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
                led_buf[j * WL_LED_BIT_LEN + i + COL_BITS * k] = ((grb[j][k] >> ((COL_BITS - 1) - i)) & 0x01) ? WL_LED_ONE : WL_LED_ZERO;

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
        if (HAL_TIM_PWM_Start_DMA(&htim8, TIM_CHANNEL_1, (uint32_t*) led_buf, DMA_BUF_SIZE) != HAL_OK)
        {
            Error_Handler();
        }
    }
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

    if (isSpace)
    {
        countSpace++;
        if (countSpace > 20)
        {
            countSpace = 0;

            if (stepSpace == 10)
            {
                if (isShiftPressed())
                {
                    setAllLedBuf(rgb_shift.r, rgb_shift.g, rgb_shift.b);
                }
                else if (isUpperPressed())
                {
                    setAllLedBuf(rgb_upper.r, rgb_upper.g, rgb_upper.b);
                }
                else
                {
                    setAllLedBuf(rgb_normal.r, rgb_normal.g, rgb_normal.b);
                }
                renew();
                isSpace = false;
            }
            else
            {
                if (isShiftPressed())
                {
                    switch (stepSpace)
                    {
                    case 0:
                        setColumn2ColorLedBuf(0, 0b1000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(1, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(2, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        break;
                    case 1:
                        setColumn2ColorLedBuf(0, 0b0100000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(1, 0b1000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(2, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        break;
                    case 2:
                        setColumn2ColorLedBuf(0, 0b0010000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(1, 0b0100000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(2, 0b1000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        break;
                    case 3:
                        setColumn2ColorLedBuf(0, 0b0001000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(1, 0b0010000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(2, 0b1100000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        break;
                    case 4:
                        setColumn2ColorLedBuf(0, 0b0000100000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(1, 0b0001000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(2, 0b1110000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        break;
                    case 5:
                        setColumn2ColorLedBuf(0, 0b0000010000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(1, 0b0000010000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(2, 0b0000100000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        break;
                    case 6:
                        setColumn2ColorLedBuf(0, 0b0000001000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(1, 0b0000001000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(2, 0b0000001000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        break;
                    case 7:
                        setColumn2ColorLedBuf(0, 0b0000000100, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(1, 0b0000000100, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(2, 0b0000000100, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        break;
                    case 8:
                        setColumn2ColorLedBuf(0, 0b0000000010, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(1, 0b0000000010, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(2, 0b0000000010, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        break;
                    case 9:
                        setColumn2ColorLedBuf(0, 0b0000000001, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(1, 0b0000000001, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(2, 0b0000000001, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        break;
                    }
                }
                else if (isUpperPressed())
                {
                    switch (stepSpace)
                    {
                    case 0:
                        setColumn2ColorLedBuf(0, 0b1000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(1, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(2, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        break;
                    case 1:
                        setColumn2ColorLedBuf(0, 0b0100000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(1, 0b1000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(2, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        break;
                    case 2:
                        setColumn2ColorLedBuf(0, 0b0010000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(1, 0b0100000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(2, 0b1000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        break;
                    case 3:
                        setColumn2ColorLedBuf(0, 0b0001000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(1, 0b0010000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(2, 0b1100000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        break;
                    case 4:
                        setColumn2ColorLedBuf(0, 0b0000100000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(1, 0b0001000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(2, 0b1110000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        break;
                    case 5:
                        setColumn2ColorLedBuf(0, 0b0000010000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(1, 0b0000010000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(2, 0b0000100000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        break;
                    case 6:
                        setColumn2ColorLedBuf(0, 0b0000001000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(1, 0b0000001000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(2, 0b0000001000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        break;
                    case 7:
                        setColumn2ColorLedBuf(0, 0b0000000100, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(1, 0b0000000100, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(2, 0b0000000100, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        break;
                    case 8:
                        setColumn2ColorLedBuf(0, 0b0000000010, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(1, 0b0000000010, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(2, 0b0000000010, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        break;
                    case 9:
                        setColumn2ColorLedBuf(0, 0b0000000001, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(1, 0b0000000001, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(2, 0b0000000001, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        break;
                    }
                }
                else
                {
                    switch (stepSpace)
                    {
                    case 0:
                        setColumn2ColorLedBuf(0, 0b1000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(1, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(2, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        break;
                    case 1:
                        setColumn2ColorLedBuf(0, 0b0100000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(1, 0b1000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(2, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        break;
                    case 2:
                        setColumn2ColorLedBuf(0, 0b0010000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(1, 0b0100000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(2, 0b1000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        break;
                    case 3:
                        setColumn2ColorLedBuf(0, 0b0001000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(1, 0b0010000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(2, 0b1100000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        break;
                    case 4:
                        setColumn2ColorLedBuf(0, 0b0000100000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(1, 0b0001000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(2, 0b1110000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        break;
                    case 5:
                        setColumn2ColorLedBuf(0, 0b0000010000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(1, 0b0000010000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(2, 0b0000100000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        break;
                    case 6:
                        setColumn2ColorLedBuf(0, 0b0000001000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(1, 0b0000001000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(2, 0b0000001000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        break;
                    case 7:
                        setColumn2ColorLedBuf(0, 0b0000000100, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(1, 0b0000000100, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(2, 0b0000000100, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        break;
                    case 8:
                        setColumn2ColorLedBuf(0, 0b0000000010, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(1, 0b0000000010, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(2, 0b0000000010, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        break;
                    case 9:
                        setColumn2ColorLedBuf(0, 0b0000000001, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(1, 0b0000000001, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(2, 0b0000000001, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        break;
                    }
                }
                if (isShiftPressed())
                {
                    setLedBuf(30, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                    setLedBuf(31, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                    setLedBuf(32, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                    setLedBuf(33, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                }
                else if (isUpperPressed())
                {
                    setLedBuf(30, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                    setLedBuf(31, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                    setLedBuf(32, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                    setLedBuf(33, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                }
                else
                {
                    setLedBuf(30, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                    setLedBuf(31, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                    setLedBuf(32, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                    setLedBuf(33, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                }
                renew();
            }
            stepSpace++;
        }
    }

    if (isBackspace)
    {
        countBackspace++;
        if (countBackspace > 20)
        {
            countBackspace = 0;

            if (stepBackspace == 11)
            {
                if (isShiftPressed())
                {
                    setAllLedBuf(rgb_shift.r, rgb_shift.g, rgb_shift.b);
                }
                else if (isUpperPressed())
                {
                    setAllLedBuf(rgb_upper.r, rgb_upper.g, rgb_upper.b);
                }
                else
                {
                    setAllLedBuf(rgb_normal.r, rgb_normal.g, rgb_normal.b);
                }
                renew();
                isBackspace = false;
            }
            else
            {
                if (isShiftPressed())
                {
                    switch (stepBackspace)
                    {
                    case 0:
                        setColumn2ColorLedBuf(0, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(1, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(2, 0b0000000001, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        break;
                    case 1:
                        setColumn2ColorLedBuf(0, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(1, 0b0000000001, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(2, 0b0000000010, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        break;
                    case 2:
                        setColumn2ColorLedBuf(0, 0b0000000001, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(1, 0b0000000010, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(2, 0b0000000100, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        break;
                    case 3:
                        setColumn2ColorLedBuf(0, 0b0000000011, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(1, 0b0000000100, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(2, 0b0000001000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        break;
                    case 4:
                        setColumn2ColorLedBuf(0, 0b0000000111, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(1, 0b0000001000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(2, 0b0000010000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        break;
                    case 5:
                        setColumn2ColorLedBuf(0, 0b0000010000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(1, 0b0000100000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(2, 0b0000100000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        break;
                    case 6:
                        setColumn2ColorLedBuf(0, 0b0001000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(1, 0b0001000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(2, 0b0001000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        break;
                    case 7:
                        setColumn2ColorLedBuf(0, 0b0010000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(1, 0b0010000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(2, 0b0010000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        break;
                    case 8:
                        setColumn2ColorLedBuf(0, 0b0100000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(1, 0b0100000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(2, 0b0100000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        break;
                    case 9:
                        setColumn2ColorLedBuf(0, 0b1000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(1, 0b1000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(2, 0b1000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        break;
                    }
                }
                else if (isUpperPressed())
                {
                    switch (stepBackspace)
                    {
                    case 0:
                        setColumn2ColorLedBuf(0, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(1, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(2, 0b0000000001, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        break;
                    case 1:
                        setColumn2ColorLedBuf(0, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(1, 0b0000000001, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(2, 0b0000000010, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        break;
                    case 2:
                        setColumn2ColorLedBuf(0, 0b0000000001, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(1, 0b0000000010, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(2, 0b0000000100, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        break;
                    case 3:
                        setColumn2ColorLedBuf(0, 0b0000000011, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(1, 0b0000000100, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(2, 0b0000001000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        break;
                    case 4:
                        setColumn2ColorLedBuf(0, 0b0000000111, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(1, 0b0000001000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(2, 0b0000010000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        break;
                    case 5:
                        setColumn2ColorLedBuf(0, 0b0000010000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(1, 0b0000100000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(2, 0b0000100000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        break;
                    case 6:
                        setColumn2ColorLedBuf(0, 0b0001000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(1, 0b0001000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(2, 0b0001000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        break;
                    case 7:
                        setColumn2ColorLedBuf(0, 0b0010000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(1, 0b0010000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(2, 0b0010000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        break;
                    case 8:
                        setColumn2ColorLedBuf(0, 0b0100000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(1, 0b0100000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(2, 0b0100000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        break;
                    case 9:
                        setColumn2ColorLedBuf(0, 0b1000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(1, 0b1000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(2, 0b1000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        break;
                    }
                }
                else
                {
                    switch (stepBackspace)
                    {
                    case 0:
                        setColumn2ColorLedBuf(0, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(1, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(2, 0b0000000001, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        break;
                    case 1:
                        setColumn2ColorLedBuf(0, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(1, 0b0000000001, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(2, 0b0000000010, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        break;
                    case 2:
                        setColumn2ColorLedBuf(0, 0b0000000001, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(1, 0b0000000010, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(2, 0b0000000100, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        break;
                    case 3:
                        setColumn2ColorLedBuf(0, 0b0000000011, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(1, 0b0000000100, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(2, 0b0000001000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        break;
                    case 4:
                        setColumn2ColorLedBuf(0, 0b0000000111, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(1, 0b0000001000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(2, 0b0000010000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        break;
                    case 5:
                        setColumn2ColorLedBuf(0, 0b0000010000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(1, 0b0000100000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(2, 0b0000100000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        break;
                    case 6:
                        setColumn2ColorLedBuf(0, 0b0001000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(1, 0b0001000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(2, 0b0001000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        break;
                    case 7:
                        setColumn2ColorLedBuf(0, 0b0010000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(1, 0b0010000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(2, 0b0010000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        break;
                    case 8:
                        setColumn2ColorLedBuf(0, 0b0100000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(1, 0b0100000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(2, 0b0100000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        break;
                    case 9:
                        setColumn2ColorLedBuf(0, 0b1000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(1, 0b1000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(2, 0b1000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        break;
                    }
                }
                if (isShiftPressed())
                {
                    setLedBuf(30, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                    setLedBuf(31, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                    setLedBuf(32, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                    setLedBuf(33, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                }
                else if (isUpperPressed())
                {
                    setLedBuf(30, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                    setLedBuf(31, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                    setLedBuf(32, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                    setLedBuf(33, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                }
                else
                {
                    setLedBuf(30, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                    setLedBuf(31, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                    setLedBuf(32, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                    setLedBuf(33, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                }
                renew();
            }
            stepBackspace++;
        }
    }

    if (isEnter)
    {
        countEnter++;
        if (countEnter > 20)
        {
            countEnter = 0;

            if (stepEnter == 10)
            {
                if (isShiftPressed())
                {
                    setAllLedBuf(rgb_shift.r, rgb_shift.g, rgb_shift.b);
                }
                else if (isUpperPressed())
                {
                    setAllLedBuf(rgb_upper.r, rgb_upper.g, rgb_upper.b);
                }
                else
                {
                    setAllLedBuf(rgb_normal.r, rgb_normal.g, rgb_normal.b);
                }
                renew();
                isEnter = false;
            }
            else
            {
                if (isShiftPressed())
                {
                    switch (stepEnter)
                    {
                    case 0:
                        setColumn2ColorLedBuf(0, 0b0000110000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(1, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(2, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        break;
                    case 1:
                        setColumn2ColorLedBuf(0, 0b0001111000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(1, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(2, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        break;
                    case 2:
                        setColumn2ColorLedBuf(0, 0b0001001000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(1, 0b0000110000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(2, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        break;
                    case 3:
                        setColumn2ColorLedBuf(0, 0b0010000100, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(1, 0b0001111000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(2, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        break;
                    case 4:
                        setColumn2ColorLedBuf(0, 0b0010000100, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(1, 0b0001001000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(2, 0b0000110000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        break;
                    case 5:
                        setColumn2ColorLedBuf(0, 0b0100000010, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(1, 0b0010000100, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(2, 0b0001111000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        break;
                    case 6:
                        setColumn2ColorLedBuf(0, 0b0100000010, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(1, 0b0010000100, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(2, 0b0001001000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        break;
                    case 7:
                        setColumn2ColorLedBuf(0, 0b1000000001, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(1, 0b0100000010, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(2, 0b0010000100, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        break;
                    case 8:
                        setColumn2ColorLedBuf(0, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(1, 0b1000000001, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(2, 0b0100000010, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        break;
                    case 9:
                        setColumn2ColorLedBuf(0, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(1, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        setColumn2ColorLedBuf(2, 0b1000000001, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        break;
                    }
                }
                else if (isUpperPressed())
                {
                    switch (stepEnter)
                    {
                    case 0:
                        setColumn2ColorLedBuf(0, 0b0000110000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(1, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(2, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        break;
                    case 1:
                        setColumn2ColorLedBuf(0, 0b0001111000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(1, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(2, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        break;
                    case 2:
                        setColumn2ColorLedBuf(0, 0b0001001000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(1, 0b0000110000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(2, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        break;
                    case 3:
                        setColumn2ColorLedBuf(0, 0b0010000100, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(1, 0b0001111000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(2, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        break;
                    case 4:
                        setColumn2ColorLedBuf(0, 0b0010000100, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(1, 0b0001001000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(2, 0b0000110000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        break;
                    case 5:
                        setColumn2ColorLedBuf(0, 0b0100000010, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(1, 0b0010000100, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(2, 0b0001111000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        break;
                    case 6:
                        setColumn2ColorLedBuf(0, 0b0100000010, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(1, 0b0010000100, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(2, 0b0001001000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        break;
                    case 7:
                        setColumn2ColorLedBuf(0, 0b1000000001, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(1, 0b0100000010, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(2, 0b0010000100, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        break;
                    case 8:
                        setColumn2ColorLedBuf(0, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(1, 0b1000000001, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(2, 0b0100000010, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        break;
                    case 9:
                        setColumn2ColorLedBuf(0, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(1, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        setColumn2ColorLedBuf(2, 0b1000000001, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        break;
                    }
                }
                else
                {
                    switch (stepEnter)
                    {
                    case 0:
                        setColumn2ColorLedBuf(0, 0b0000110000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(1, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(2, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        break;
                    case 1:
                        setColumn2ColorLedBuf(0, 0b0001111000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(1, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(2, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        break;
                    case 2:
                        setColumn2ColorLedBuf(0, 0b0001001000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(1, 0b0000110000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(2, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        break;
                    case 3:
                        setColumn2ColorLedBuf(0, 0b0010000100, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(1, 0b0001111000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(2, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        break;
                    case 4:
                        setColumn2ColorLedBuf(0, 0b0010000100, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(1, 0b0001001000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(2, 0b0000110000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        break;
                    case 5:
                        setColumn2ColorLedBuf(0, 0b0100000010, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(1, 0b0010000100, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(2, 0b0001111000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        break;
                    case 6:
                        setColumn2ColorLedBuf(0, 0b0100000010, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(1, 0b0010000100, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(2, 0b0001001000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        break;
                    case 7:
                        setColumn2ColorLedBuf(0, 0b1000000001, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(1, 0b0100000010, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(2, 0b0010000100, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        break;
                    case 8:
                        setColumn2ColorLedBuf(0, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(1, 0b1000000001, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(2, 0b0100000010, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        break;
                    case 9:
                        setColumn2ColorLedBuf(0, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(1, 0b0000000000, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        setColumn2ColorLedBuf(2, 0b1000000001, rgb_blank.r, rgb_blank.g, rgb_blank.b, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        break;
                    }
                }

                if (isShiftPressed())
                {
                    setLedBuf(30, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                    setLedBuf(31, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                    setLedBuf(32, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                    setLedBuf(33, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                }
                else if (isUpperPressed())
                {
                    setLedBuf(30, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                    setLedBuf(31, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                    setLedBuf(32, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                    setLedBuf(33, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                }
                else
                {
                    setLedBuf(30, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                    setLedBuf(31, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                    setLedBuf(32, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                    setLedBuf(33, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                }
                renew();
            }
            stepEnter++;
        }
    }
}

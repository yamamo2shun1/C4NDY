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
                for (int i = 0; i < 10; i++)
                {
                    if (i == stepSpace)
                    {
                        setLedBuf(i, 0xFF, 0xFF, 0xFF);
                        setLedBuf(i + 10, 0xFF, 0xFF, 0xFF);
                        setLedBuf(i + 20, 0xFF, 0xFF, 0xFF);
                    }
                    else
                    {
                        if (isShiftPressed())
                        {
                            setLedBuf(i, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                            setLedBuf(i + 10, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                            setLedBuf(i + 20, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        }
                        else if (isUpperPressed())
                        {
                            setLedBuf(i, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                            setLedBuf(i + 10, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                            setLedBuf(i + 20, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        }
                        else
                        {
                            setLedBuf(i, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                            setLedBuf(i + 10, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                            setLedBuf(i + 20, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        }
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
                for (int i = 0; i < 10; i++)
                {
                    if (i == stepBackspace)
                    {
                        setLedBuf(9 - i, rgb_blank.r, rgb_blank.g, rgb_blank.b);
                        setLedBuf(19 - i, rgb_blank.r, rgb_blank.g, rgb_blank.b);
                        setLedBuf(29 - i, rgb_blank.r, rgb_blank.g, rgb_blank.b);
                    }
                    else
                    {
                        if (isShiftPressed())
                        {
                            setLedBuf(9 - i, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                            setLedBuf(19 - i, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                            setLedBuf(29 - i, rgb_shift.r, rgb_shift.g, rgb_shift.b);
                        }
                        else if (isUpperPressed())
                        {
                            setLedBuf(9 - i, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                            setLedBuf(19 - i, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                            setLedBuf(29 - i, rgb_upper.r, rgb_upper.g, rgb_upper.b);
                        }
                        else
                        {
                            setLedBuf(9 - i, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                            setLedBuf(19 - i, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                            setLedBuf(29 - i, rgb_normal.r, rgb_normal.g, rgb_normal.b);
                        }
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
}

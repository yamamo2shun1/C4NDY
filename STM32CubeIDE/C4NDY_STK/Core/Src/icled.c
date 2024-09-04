/*
 * icled.c
 *
 *  Created on: Aug 7, 2024
 *      Author: shun
 */

#include "icled.h"

#include "keyboard.h"
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

void setLedBuf(uint8_t index, RGB_Color_t* rgb_color)
{
    grb_prev[index][0] = grb_current[index][0];
    grb_prev[index][1] = grb_current[index][1];
    grb_prev[index][2] = grb_current[index][2];

    grb_current[index][0] = (uint8_t) ((double) rgb_color->g * LED_INTENSITY_RATE);
    grb_current[index][1] = (uint8_t) ((double) rgb_color->r * LED_INTENSITY_RATE);
    grb_current[index][2] = (uint8_t) ((double) rgb_color->b * LED_INTENSITY_RATE);

    if (grb_prev[index][0] != grb_current[index][0] ||
        grb_prev[index][1] != grb_current[index][1] ||
        grb_prev[index][2] != grb_current[index][2])
    {
        isGradation = true;
        g_rate      = 0.0;
    }
}

void setAllLedBuf(RGB_Color_t* rgb_color)
{
    for (int i = 0; i < LED_NUMS; i++)
    {
        grb_prev[i][0] = grb_current[i][0];
        grb_prev[i][1] = grb_current[i][1];
        grb_prev[i][2] = grb_current[i][2];

        grb_current[i][0] = (uint8_t) ((double) rgb_color->g * LED_INTENSITY_RATE);
        grb_current[i][1] = (uint8_t) ((double) rgb_color->r * LED_INTENSITY_RATE);
        grb_current[i][2] = (uint8_t) ((double) rgb_color->b * LED_INTENSITY_RATE);

        if (grb_prev[i][0] != grb_current[i][0] ||
            grb_prev[i][1] != grb_current[i][1] ||
            grb_prev[i][2] != grb_current[i][2])
        {
            isGradation = true;
            g_rate      = 0.0;
        }
    }
}

void setColumn2ColorLedBuf(uint8_t row, uint16_t column, RGB_Color_t* rgb_color1, RGB_Color_t* rgb_color0)
{
    for (int i = 0; i < 10; i++)
    {
        if ((column >> (9 - i)) & 0x01)
        {
            setLedBuf(i + row * 10, rgb_color1);
        }
        else
        {
            setLedBuf(i + row * 10, rgb_color0);
        }
    }
}

void setColumn3ColorLedBuf(uint8_t row, uint16_t column0, uint16_t column1, RGB_Color_t* rgb_color2, RGB_Color_t* rgb_color1, RGB_Color_t* rgb_color0)
{
    for (int i = 0; i < 10; i++)
    {
        if ((column1 >> (9 - i)) & 0x01)
        {
            setLedBuf(i + row * 10, rgb_color1);
        }
        else if ((column0 >> (9 - i)) & 0x01)
        {
            setLedBuf(i + row * 10, rgb_color2);
        }
        else
        {
            setLedBuf(i + row * 10, rgb_color0);
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
                    setAllLedBuf(&rgb_shift);
                }
                else if (isUpperPressed())
                {
                    setAllLedBuf(&rgb_upper);
                }
                else
                {
                    setAllLedBuf(&rgb_normal);
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
                        setColumn3ColorLedBuf(0, 0b0000000000, 0b1000000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(1, 0b0000000000, 0b0000000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(2, 0b0000000000, 0b0000000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        break;
                    case 1:
                        setColumn3ColorLedBuf(0, 0b1000000000, 0b0100000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(1, 0b0000000000, 0b1000000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(2, 0b0000000000, 0b0000000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        break;
                    case 2:
                        setColumn3ColorLedBuf(0, 0b0100000000, 0b0010000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(1, 0b1000000000, 0b0100000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(2, 0b1000000000, 0b1000000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        break;
                    case 3:
                        setColumn3ColorLedBuf(0, 0b0010000000, 0b0001000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(1, 0b0100000000, 0b0010000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(2, 0b1000000000, 0b1100000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        break;
                    case 4:
                        setColumn3ColorLedBuf(0, 0b0001000000, 0b0000100000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(1, 0b0010000000, 0b0001000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(2, 0b1100000000, 0b1110000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        break;
                    case 5:
                        setColumn3ColorLedBuf(0, 0b0000100000, 0b0000010000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(1, 0b0001000000, 0b0000010000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(2, 0b1110000000, 0b0001100000, &rgb_normal, &rgb_blank, &rgb_shift);
                        break;
                    case 6:
                        setColumn3ColorLedBuf(0, 0b0000010000, 0b0000001000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(1, 0b0000010000, 0b0000001000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(2, 0b0000100000, 0b0000011000, &rgb_normal, &rgb_blank, &rgb_shift);
                        break;
                    case 7:
                        setColumn3ColorLedBuf(0, 0b0000001000, 0b0000000100, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(1, 0b0000001000, 0b0000000100, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(2, 0b0000011000, 0b0000000100, &rgb_normal, &rgb_blank, &rgb_shift);
                        break;
                    case 8:
                        setColumn3ColorLedBuf(0, 0b0000000100, 0b0000000010, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(1, 0b0000000100, 0b0000000010, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(2, 0b0000000100, 0b0000000010, &rgb_normal, &rgb_blank, &rgb_shift);
                        break;
                    case 9:
                        setColumn3ColorLedBuf(0, 0b0000000010, 0b0000000001, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(1, 0b0000000010, 0b0000000001, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(2, 0b0000000010, 0b0000000001, &rgb_normal, &rgb_blank, &rgb_shift);
                        break;
                    }
                }
                else if (isUpperPressed())
                {
                    switch (stepSpace)
                    {
                    case 0:
                        setColumn3ColorLedBuf(0, 0b0000000000, 0b1000000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(1, 0b0000000000, 0b0000000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(2, 0b0000000000, 0b0000000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        break;
                    case 1:
                        setColumn3ColorLedBuf(0, 0b1000000000, 0b0100000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(1, 0b0000000000, 0b1000000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(2, 0b0000000000, 0b0000000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        break;
                    case 2:
                        setColumn3ColorLedBuf(0, 0b0100000000, 0b0010000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(1, 0b1000000000, 0b0100000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(2, 0b1000000000, 0b1000000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        break;
                    case 3:
                        setColumn3ColorLedBuf(0, 0b0010000000, 0b0001000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(1, 0b0100000000, 0b0010000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(2, 0b1000000000, 0b1100000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        break;
                    case 4:
                        setColumn3ColorLedBuf(0, 0b0001000000, 0b0000100000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(1, 0b0010000000, 0b0001000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(2, 0b1100000000, 0b1110000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        break;
                    case 5:
                        setColumn3ColorLedBuf(0, 0b0000100000, 0b0000010000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(1, 0b0001000000, 0b0000010000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(2, 0b1110000000, 0b0001100000, &rgb_shift, &rgb_blank, &rgb_upper);
                        break;
                    case 6:
                        setColumn3ColorLedBuf(0, 0b0000010000, 0b0000001000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(1, 0b0000010000, 0b0000001000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(2, 0b0000100000, 0b0000011000, &rgb_shift, &rgb_blank, &rgb_upper);
                        break;
                    case 7:
                        setColumn3ColorLedBuf(0, 0b0000001000, 0b0000000100, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(1, 0b0000001000, 0b0000000100, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(2, 0b0000011000, 0b0000000100, &rgb_shift, &rgb_blank, &rgb_upper);
                        break;
                    case 8:
                        setColumn3ColorLedBuf(0, 0b0000000100, 0b0000000010, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(1, 0b0000000100, 0b0000000010, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(2, 0b0000000100, 0b0000000010, &rgb_shift, &rgb_blank, &rgb_upper);
                        break;
                    case 9:
                        setColumn3ColorLedBuf(0, 0b0000000010, 0b0000000001, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(1, 0b0000000010, 0b0000000001, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(2, 0b0000000010, 0b0000000001, &rgb_shift, &rgb_blank, &rgb_upper);
                        break;
                    }
                }
                else
                {
                    switch (stepSpace)
                    {
                    case 0:
                        setColumn3ColorLedBuf(0, 0b0000000000, 0b1000000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(1, 0b0000000000, 0b0000000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(2, 0b0000000000, 0b0000000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        break;
                    case 1:
                        setColumn3ColorLedBuf(0, 0b1000000000, 0b0100000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(1, 0b0000000000, 0b1000000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(2, 0b0000000000, 0b0000000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        break;
                    case 2:
                        setColumn3ColorLedBuf(0, 0b0100000000, 0b0010000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(1, 0b1000000000, 0b0100000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(2, 0b1000000000, 0b1000000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        break;
                    case 3:
                        setColumn3ColorLedBuf(0, 0b0010000000, 0b0001000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(1, 0b0100000000, 0b0010000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(2, 0b1000000000, 0b1100000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        break;
                    case 4:
                        setColumn3ColorLedBuf(0, 0b0001000000, 0b0000100000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(1, 0b0010000000, 0b0001000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(2, 0b1100000000, 0b1110000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        break;
                    case 5:
                        setColumn3ColorLedBuf(0, 0b0000100000, 0b0000010000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(1, 0b0001000000, 0b0000010000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(2, 0b1110000000, 0b0001100000, &rgb_upper, &rgb_blank, &rgb_normal);
                        break;
                    case 6:
                        setColumn3ColorLedBuf(0, 0b0000010000, 0b0000001000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(1, 0b0000010000, 0b0000001000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(2, 0b0000100000, 0b0000011000, &rgb_upper, &rgb_blank, &rgb_normal);
                        break;
                    case 7:
                        setColumn3ColorLedBuf(0, 0b0000001000, 0b0000000100, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(1, 0b0000001000, 0b0000000100, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(2, 0b0000011000, 0b0000000100, &rgb_upper, &rgb_blank, &rgb_normal);
                        break;
                    case 8:
                        setColumn3ColorLedBuf(0, 0b0000000100, 0b0000000010, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(1, 0b0000000100, 0b0000000010, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(2, 0b0000000100, 0b0000000010, &rgb_upper, &rgb_blank, &rgb_normal);
                        break;
                    case 9:
                        setColumn3ColorLedBuf(0, 0b0000000010, 0b0000000001, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(1, 0b0000000010, 0b0000000001, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(2, 0b0000000010, 0b0000000001, &rgb_upper, &rgb_blank, &rgb_normal);
                        break;
                    }
                }
                if (isShiftPressed())
                {
                    setLedBuf(30, &rgb_shift);
                    setLedBuf(31, &rgb_shift);
                    setLedBuf(32, &rgb_shift);
                    setLedBuf(33, &rgb_shift);
                }
                else if (isUpperPressed())
                {
                    setLedBuf(30, &rgb_upper);
                    setLedBuf(31, &rgb_upper);
                    setLedBuf(32, &rgb_upper);
                    setLedBuf(33, &rgb_upper);
                }
                else
                {
                    setLedBuf(30, &rgb_normal);
                    setLedBuf(31, &rgb_normal);
                    setLedBuf(32, &rgb_normal);
                    setLedBuf(33, &rgb_normal);
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
                    setAllLedBuf(&rgb_shift);
                }
                else if (isUpperPressed())
                {
                    setAllLedBuf(&rgb_upper);
                }
                else
                {
                    setAllLedBuf(&rgb_normal);
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
                        setColumn3ColorLedBuf(0, 0b0000000000, 0b0000000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(1, 0b0000000000, 0b0000000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(2, 0b0000000000, 0b0000000001, &rgb_normal, &rgb_blank, &rgb_shift);
                        break;
                    case 1:
                        setColumn3ColorLedBuf(0, 0b0000000000, 0b0000000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(1, 0b0000000000, 0b0000000001, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(2, 0b0000000001, 0b0000000010, &rgb_normal, &rgb_blank, &rgb_shift);
                        break;
                    case 2:
                        setColumn3ColorLedBuf(0, 0b0000000000, 0b0000000001, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(1, 0b0000000001, 0b0000000010, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(2, 0b0000000010, 0b0000000100, &rgb_normal, &rgb_blank, &rgb_shift);
                        break;
                    case 3:
                        setColumn3ColorLedBuf(0, 0b0000000001, 0b0000000011, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(1, 0b0000000010, 0b0000000100, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(2, 0b0000000100, 0b0000001000, &rgb_normal, &rgb_blank, &rgb_shift);
                        break;
                    case 4:
                        setColumn3ColorLedBuf(0, 0b0000000011, 0b0000000111, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(1, 0b0000000100, 0b0000001000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(2, 0b0000001000, 0b0000010000, &rgb_normal, &rgb_blank, &rgb_shift);
                        break;
                    case 5:
                        setColumn3ColorLedBuf(0, 0b0000000111, 0b0000011000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(1, 0b0000001000, 0b0000100000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(2, 0b0000010000, 0b0000100000, &rgb_normal, &rgb_blank, &rgb_shift);
                        break;
                    case 6:
                        setColumn3ColorLedBuf(0, 0b0000011000, 0b0000100000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(1, 0b0000100000, 0b0001000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(2, 0b0000100000, 0b0001000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        break;
                    case 7:
                        setColumn3ColorLedBuf(0, 0b0000100000, 0b0010000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(1, 0b0001000000, 0b0010000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(2, 0b0001000000, 0b0010000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        break;
                    case 8:
                        setColumn3ColorLedBuf(0, 0b0010000000, 0b0100000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(1, 0b0010000000, 0b0100000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(2, 0b0010000000, 0b0100000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        break;
                    case 9:
                        setColumn3ColorLedBuf(0, 0b0100000000, 0b1000000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(1, 0b0100000000, 0b1000000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(2, 0b0100000000, 0b1000000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        break;
                    }
                }
                else if (isUpperPressed())
                {
                    switch (stepBackspace)
                    {
                    case 0:
                        setColumn3ColorLedBuf(0, 0b0000000000, 0b0000000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(1, 0b0000000000, 0b0000000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(2, 0b0000000000, 0b0000000001, &rgb_shift, &rgb_blank, &rgb_upper);
                        break;
                    case 1:
                        setColumn3ColorLedBuf(0, 0b0000000000, 0b0000000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(1, 0b0000000000, 0b0000000001, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(2, 0b0000000001, 0b0000000010, &rgb_shift, &rgb_blank, &rgb_upper);
                        break;
                    case 2:
                        setColumn3ColorLedBuf(0, 0b0000000000, 0b0000000001, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(1, 0b0000000001, 0b0000000010, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(2, 0b0000000010, 0b0000000100, &rgb_shift, &rgb_blank, &rgb_upper);
                        break;
                    case 3:
                        setColumn3ColorLedBuf(0, 0b0000000001, 0b0000000011, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(1, 0b0000000010, 0b0000000100, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(2, 0b0000000100, 0b0000001000, &rgb_shift, &rgb_blank, &rgb_upper);
                        break;
                    case 4:
                        setColumn3ColorLedBuf(0, 0b0000000011, 0b0000000111, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(1, 0b0000000100, 0b0000001000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(2, 0b0000001000, 0b0000010000, &rgb_shift, &rgb_blank, &rgb_upper);
                        break;
                    case 5:
                        setColumn3ColorLedBuf(0, 0b0000000111, 0b0000011000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(1, 0b0000001000, 0b0000100000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(2, 0b0000010000, 0b0000100000, &rgb_shift, &rgb_blank, &rgb_upper);
                        break;
                    case 6:
                        setColumn3ColorLedBuf(0, 0b0000011000, 0b0000100000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(1, 0b0000100000, 0b0001000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(2, 0b0000100000, 0b0001000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        break;
                    case 7:
                        setColumn3ColorLedBuf(0, 0b0000100000, 0b0010000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(1, 0b0001000000, 0b0010000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(2, 0b0001000000, 0b0010000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        break;
                    case 8:
                        setColumn3ColorLedBuf(0, 0b0010000000, 0b0100000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(1, 0b0010000000, 0b0100000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(2, 0b0010000000, 0b0100000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        break;
                    case 9:
                        setColumn3ColorLedBuf(0, 0b0100000000, 0b1000000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(1, 0b0100000000, 0b1000000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(2, 0b0100000000, 0b1000000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        break;
                    }
                }
                else
                {
                    switch (stepBackspace)
                    {
                    case 0:
                        setColumn3ColorLedBuf(0, 0b0000000000, 0b0000000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(1, 0b0000000000, 0b0000000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(2, 0b0000000000, 0b0000000001, &rgb_upper, &rgb_blank, &rgb_normal);
                        break;
                    case 1:
                        setColumn3ColorLedBuf(0, 0b0000000000, 0b0000000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(1, 0b0000000000, 0b0000000001, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(2, 0b0000000001, 0b0000000010, &rgb_upper, &rgb_blank, &rgb_normal);
                        break;
                    case 2:
                        setColumn3ColorLedBuf(0, 0b0000000000, 0b0000000001, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(1, 0b0000000001, 0b0000000010, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(2, 0b0000000010, 0b0000000100, &rgb_upper, &rgb_blank, &rgb_normal);
                        break;
                    case 3:
                        setColumn3ColorLedBuf(0, 0b0000000001, 0b0000000011, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(1, 0b0000000010, 0b0000000100, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(2, 0b0000000100, 0b0000001000, &rgb_upper, &rgb_blank, &rgb_normal);
                        break;
                    case 4:
                        setColumn3ColorLedBuf(0, 0b0000000011, 0b0000000111, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(1, 0b0000000100, 0b0000001000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(2, 0b0000001000, 0b0000010000, &rgb_upper, &rgb_blank, &rgb_normal);
                        break;
                    case 5:
                        setColumn3ColorLedBuf(0, 0b0000000111, 0b0000011000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(1, 0b0000001000, 0b0000100000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(2, 0b0000010000, 0b0000100000, &rgb_upper, &rgb_blank, &rgb_normal);
                        break;
                    case 6:
                        setColumn3ColorLedBuf(0, 0b0000011000, 0b0000100000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(1, 0b0000100000, 0b0001000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(2, 0b0000100000, 0b0001000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        break;
                    case 7:
                        setColumn3ColorLedBuf(0, 0b0000100000, 0b0010000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(1, 0b0001000000, 0b0010000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(2, 0b0001000000, 0b0010000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        break;
                    case 8:
                        setColumn3ColorLedBuf(0, 0b0010000000, 0b0100000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(1, 0b0010000000, 0b0100000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(2, 0b0010000000, 0b0100000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        break;
                    case 9:
                        setColumn3ColorLedBuf(0, 0b0100000000, 0b1000000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(1, 0b0100000000, 0b1000000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(2, 0b0100000000, 0b1000000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        break;
                    }
                }
                if (isShiftPressed())
                {
                    setLedBuf(30, &rgb_shift);
                    setLedBuf(31, &rgb_shift);
                    setLedBuf(32, &rgb_shift);
                    setLedBuf(33, &rgb_shift);
                }
                else if (isUpperPressed())
                {
                    setLedBuf(30, &rgb_upper);
                    setLedBuf(31, &rgb_upper);
                    setLedBuf(32, &rgb_upper);
                    setLedBuf(33, &rgb_upper);
                }
                else
                {
                    setLedBuf(30, &rgb_normal);
                    setLedBuf(31, &rgb_normal);
                    setLedBuf(32, &rgb_normal);
                    setLedBuf(33, &rgb_normal);
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
                    setAllLedBuf(&rgb_shift);
                }
                else if (isUpperPressed())
                {
                    setAllLedBuf(&rgb_upper);
                }
                else
                {
                    setAllLedBuf(&rgb_normal);
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
                        setColumn3ColorLedBuf(0, 0b0000000000, 0b0000110000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(1, 0b0000000000, 0b0000000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(2, 0b0000000000, 0b0000000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        break;
                    case 1:
                        setColumn3ColorLedBuf(0, 0b0000110000, 0b0001111000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(1, 0b0000000000, 0b0000000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(2, 0b0000000000, 0b0000000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        break;
                    case 2:
                        setColumn3ColorLedBuf(0, 0b0001111000, 0b0001001000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(1, 0b0000000000, 0b0000110000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(2, 0b0000000000, 0b0000000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        break;
                    case 3:
                        setColumn3ColorLedBuf(0, 0b0001001000, 0b0010000100, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(1, 0b0000110000, 0b0001111000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(2, 0b0000000000, 0b0000000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        break;
                    case 4:
                        setColumn3ColorLedBuf(0, 0b0010000100, 0b0010000100, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(1, 0b0001111000, 0b0001001000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(2, 0b0000110000, 0b0000110000, &rgb_normal, &rgb_blank, &rgb_shift);
                        break;
                    case 5:
                        setColumn3ColorLedBuf(0, 0b0010000100, 0b0100000010, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(1, 0b0001001000, 0b0010000100, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(2, 0b0000110000, 0b0001111000, &rgb_normal, &rgb_blank, &rgb_shift);
                        break;
                    case 6:
                        setColumn3ColorLedBuf(0, 0b0100000010, 0b0100000010, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(1, 0b0010000100, 0b0010000100, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(2, 0b0001111000, 0b0001001000, &rgb_normal, &rgb_blank, &rgb_shift);
                        break;
                    case 7:
                        setColumn3ColorLedBuf(0, 0b0100000010, 0b1000000001, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(1, 0b0010000100, 0b0100000010, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(2, 0b0001001000, 0b0010000100, &rgb_normal, &rgb_blank, &rgb_shift);
                        break;
                    case 8:
                        setColumn3ColorLedBuf(0, 0b1000000001, 0b0000000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(1, 0b0100000010, 0b1000000001, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(2, 0b0010000100, 0b0100000010, &rgb_normal, &rgb_blank, &rgb_shift);
                        break;
                    case 9:
                        setColumn3ColorLedBuf(0, 0b0000000000, 0b0000000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(1, 0b1000000001, 0b0000000000, &rgb_normal, &rgb_blank, &rgb_shift);
                        setColumn3ColorLedBuf(2, 0b0100000010, 0b1000000001, &rgb_normal, &rgb_blank, &rgb_shift);
                        break;
                    }
                }
                else if (isUpperPressed())
                {
                    switch (stepEnter)
                    {
                    case 0:
                        setColumn3ColorLedBuf(0, 0b0000000000, 0b0000110000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(1, 0b0000000000, 0b0000000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(2, 0b0000000000, 0b0000000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        break;
                    case 1:
                        setColumn3ColorLedBuf(0, 0b0000110000, 0b0001111000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(1, 0b0000000000, 0b0000000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(2, 0b0000000000, 0b0000000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        break;
                    case 2:
                        setColumn3ColorLedBuf(0, 0b0001111000, 0b0001001000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(1, 0b0000000000, 0b0000110000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(2, 0b0000000000, 0b0000000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        break;
                    case 3:
                        setColumn3ColorLedBuf(0, 0b0001001000, 0b0010000100, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(1, 0b0000110000, 0b0001111000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(2, 0b0000000000, 0b0000000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        break;
                    case 4:
                        setColumn3ColorLedBuf(0, 0b0010000100, 0b0010000100, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(1, 0b0001111000, 0b0001001000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(2, 0b0000110000, 0b0000110000, &rgb_shift, &rgb_blank, &rgb_upper);
                        break;
                    case 5:
                        setColumn3ColorLedBuf(0, 0b0010000100, 0b0100000010, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(1, 0b0001001000, 0b0010000100, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(2, 0b0000110000, 0b0001111000, &rgb_shift, &rgb_blank, &rgb_upper);
                        break;
                    case 6:
                        setColumn3ColorLedBuf(0, 0b0100000010, 0b0100000010, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(1, 0b0010000100, 0b0010000100, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(2, 0b0001111000, 0b0001001000, &rgb_shift, &rgb_blank, &rgb_upper);
                        break;
                    case 7:
                        setColumn3ColorLedBuf(0, 0b0100000010, 0b1000000001, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(1, 0b0010000100, 0b0100000010, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(2, 0b0001001000, 0b0010000100, &rgb_shift, &rgb_blank, &rgb_upper);
                        break;
                    case 8:
                        setColumn3ColorLedBuf(0, 0b1000000001, 0b0000000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(1, 0b0100000010, 0b1000000001, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(2, 0b0010000100, 0b0100000010, &rgb_shift, &rgb_blank, &rgb_upper);
                        break;
                    case 9:
                        setColumn3ColorLedBuf(0, 0b0000000000, 0b0000000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(1, 0b1000000001, 0b0000000000, &rgb_shift, &rgb_blank, &rgb_upper);
                        setColumn3ColorLedBuf(2, 0b0100000010, 0b1000000001, &rgb_shift, &rgb_blank, &rgb_upper);
                        break;
                    }
                }
                else
                {
                    switch (stepEnter)
                    {
                    case 0:
                        setColumn3ColorLedBuf(0, 0b0000000000, 0b0000110000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(1, 0b0000000000, 0b0000000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(2, 0b0000000000, 0b0000000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        break;
                    case 1:
                        setColumn3ColorLedBuf(0, 0b0000110000, 0b0001111000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(1, 0b0000000000, 0b0000000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(2, 0b0000000000, 0b0000000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        break;
                    case 2:
                        setColumn3ColorLedBuf(0, 0b0001111000, 0b0001001000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(1, 0b0000000000, 0b0000110000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(2, 0b0000000000, 0b0000000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        break;
                    case 3:
                        setColumn3ColorLedBuf(0, 0b0001001000, 0b0010000100, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(1, 0b0000110000, 0b0001111000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(2, 0b0000000000, 0b0000000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        break;
                    case 4:
                        setColumn3ColorLedBuf(0, 0b0010000100, 0b0010000100, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(1, 0b0001111000, 0b0001001000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(2, 0b0000110000, 0b0000110000, &rgb_upper, &rgb_blank, &rgb_normal);
                        break;
                    case 5:
                        setColumn3ColorLedBuf(0, 0b0010000100, 0b0100000010, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(1, 0b0001001000, 0b0010000100, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(2, 0b0000110000, 0b0001111000, &rgb_upper, &rgb_blank, &rgb_normal);
                        break;
                    case 6:
                        setColumn3ColorLedBuf(0, 0b0100000010, 0b0100000010, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(1, 0b0010000100, 0b0010000100, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(2, 0b0001111000, 0b0001001000, &rgb_upper, &rgb_blank, &rgb_normal);
                        break;
                    case 7:
                        setColumn3ColorLedBuf(0, 0b0100000010, 0b1000000001, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(1, 0b0010000100, 0b0100000010, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(2, 0b0001001000, 0b0010000100, &rgb_upper, &rgb_blank, &rgb_normal);
                        break;
                    case 8:
                        setColumn3ColorLedBuf(0, 0b1000000001, 0b0000000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(1, 0b0100000010, 0b1000000001, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(2, 0b0010000100, 0b0100000010, &rgb_upper, &rgb_blank, &rgb_normal);
                        break;
                    case 9:
                        setColumn3ColorLedBuf(0, 0b0000000000, 0b0000000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(1, 0b1000000001, 0b0000000000, &rgb_upper, &rgb_blank, &rgb_normal);
                        setColumn3ColorLedBuf(2, 0b0100000010, 0b1000000001, &rgb_upper, &rgb_blank, &rgb_normal);
                        break;
                    }
                }

                if (isShiftPressed())
                {
                    setLedBuf(30, &rgb_shift);
                    setLedBuf(31, &rgb_shift);
                    setLedBuf(32, &rgb_shift);
                    setLedBuf(33, &rgb_shift);
                }
                else if (isUpperPressed())
                {
                    setLedBuf(30, &rgb_upper);
                    setLedBuf(31, &rgb_upper);
                    setLedBuf(32, &rgb_upper);
                    setLedBuf(33, &rgb_upper);
                }
                else
                {
                    setLedBuf(30, &rgb_normal);
                    setLedBuf(31, &rgb_normal);
                    setLedBuf(32, &rgb_normal);
                    setLedBuf(33, &rgb_normal);
                }
                renew();
            }
            stepEnter++;
        }
    }
}

/*
 * icled.c
 *
 *  Created on: Aug 7, 2024
 *      Author: shun
 */

#include "main.h"
#include "icled.h"
#include "tim.h"

RGB_Color_t rgb_normal = {0xFE, 0x01, 0x9A};
RGB_Color_t rgb_upper  = {0x00, 0xFF, 0xFF};
RGB_Color_t rgb_shift  = {0xFF, 0xFF, 0xFF};
RGB_Color_t rgb_blank  = {0x00, 0x00, 0x00};

uint8_t grb_prev[LED_NUMS][3]    = {0};
uint8_t grb_current[LED_NUMS][3] = {0};
uint8_t grb[LED_NUMS][3]         = {0};

uint32_t led_buf[LED_NUMS * 3 * 8 + 1]      = {0};
uint32_t led_buf_prev[LED_NUMS * 3 * 8 + 1] = {0};

uint32_t counter = 0;

bool isGradation = false;
int count        = 0;
double g_rate    = 0.0;

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef* htim)
{
    HAL_TIM_PWM_Stop_DMA(htim, TIM_CHANNEL_1);
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
}

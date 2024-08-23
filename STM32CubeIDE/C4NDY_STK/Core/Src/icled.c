/*
 * icled.c
 *
 *  Created on: Aug 7, 2024
 *      Author: shun
 */

#include "main.h"
#include "icled.h"
#include "tim.h"

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

    grb_current[index][0] = (uint8_t) ((double) green / 0.5);
    grb_current[index][1] = (uint8_t) ((double) red / 0.5);
    grb_current[index][2] = (uint8_t) ((double) blue / 0.5);

    isGradation = true;
    g_rate      = 0.0;
}

void setAllLedBuf(uint8_t red, uint8_t green, uint8_t blue)
{
    if (!isGradation)
    {
        for (int i = 0; i < LED_NUMS; i++)
        {
            grb_prev[i][0] = grb_current[i][0];
            grb_prev[i][1] = grb_current[i][1];
            grb_prev[i][2] = grb_current[i][2];

            grb_current[i][0] = (uint8_t) ((double) green * 0.5);
            grb_current[i][1] = (uint8_t) ((double) red * 0.5);
            grb_current[i][2] = (uint8_t) ((double) blue * 0.5);

            if (grb_prev[i][0] != grb_current[i][0] ||
                grb_prev[i][1] != grb_current[i][1] ||
                grb_prev[i][2] != grb_current[i][2])
            {
                isGradation = true;
                g_rate      = 0.0;
            }
        }
    }
}

void gradation(uint8_t index, double rate)
{
    grb[index][0] = (uint8_t) ((double) (grb_current[index][0] - grb_prev[index][0]) * rate + (double) grb_prev[index][0]);
    grb[index][1] = (uint8_t) ((double) (grb_current[index][1] - grb_prev[index][1]) * rate + (double) grb_prev[index][1]);
    grb[index][2] = (uint8_t) ((double) (grb_current[index][2] - grb_prev[index][2]) * rate + (double) grb_prev[index][2]);
}

void gradationAll(double rate)
{
    for (int i = 0; i < LED_NUMS; i++)
    {
        grb[i][0] = (uint8_t) ((double) (grb_current[i][0] - grb_prev[i][0]) * rate + (double) grb_prev[i][0]);
        grb[i][1] = (uint8_t) ((double) (grb_current[i][1] - grb_prev[i][1]) * rate + (double) grb_prev[i][1]);
        grb[i][2] = (uint8_t) ((double) (grb_current[i][2] - grb_prev[i][2]) * rate + (double) grb_prev[i][2]);
    }
}

void renew(void)
{
    bool isRenewed = false;

    for (int j = 0; j < LED_NUMS; j++)
    {
        for (int i = 0; i < 8; i++)
        {
            led_buf[j * 24 + i]      = ((grb[j][0] >> (7 - i)) & 0x01) ? 12 : 4;
            led_buf[j * 24 + i + 8]  = ((grb[j][1] >> (7 - i)) & 0x01) ? 12 : 4;
            led_buf[j * 24 + i + 16] = ((grb[j][2] >> (7 - i)) & 0x01) ? 12 : 4;

            if (led_buf_prev[j * 24 + i] != led_buf[j * 24 + i] ||
                led_buf_prev[j * 24 + i + 8] != led_buf[j * 24 + i + 8] ||
                led_buf_prev[j * 24 + i + 16] != led_buf[j * 24 + i + 16])
            {
                isRenewed = true;
            }

            led_buf_prev[j * 24 + i]      = led_buf[j * 24 + i];
            led_buf_prev[j * 24 + i + 8]  = led_buf[j * 24 + i + 8];
            led_buf_prev[j * 24 + i + 16] = led_buf[j * 24 + i + 16];
        }
    }
    led_buf[LED_NUMS * 24] = 0;

#if 1
    if (isRenewed)
    {
        // htim8.State = HAL_TIM_STATE_READY;
        if (HAL_TIM_PWM_Start_DMA(&htim8, TIM_CHANNEL_1, (uint32_t*) led_buf, LED_NUMS * 24 + 1) != HAL_OK)
        {
            Error_Handler();
        }
    }
#endif
}

void led_control_task(void)
{
    if (isGradation)
    {
        count++;
        if (count > 10)
        {
            gradationAll(g_rate);
            renew();

            count = 0;
            g_rate += 0.2;
            if (g_rate > 1.0)
            {
                isGradation = false;
            }
        }
    }
}

/*
 * icled.c
 *
 *  Created on: Aug 7, 2024
 *      Author: shun
 */

#include "main.h"
#include "icled.h"
#include "tim.h"

uint8_t grb[LED_NUMS][3] = {0};

uint8_t led_buf[LED_NUMS * 3 * 8 + 1]      = {0};
uint8_t led_buf_prev[LED_NUMS * 3 * 8 + 1] = {0};

void setLedBuf(uint8_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    grb[index][0] = (uint8_t) ((double) green / 0.5);
    grb[index][1] = (uint8_t) ((double) red / 0.5);
    grb[index][2] = (uint8_t) ((double) blue / 0.5);
}

void renew(void)
{
    bool isRenewed = false;

    for (int j = 0; j < LED_NUMS; j++)
    {
        for (int i = 0; i < 8; i++)
        {
            led_buf[j * 24 + i]      = ((grb[j][0] >> i) & 0x01) ? 12 : 4;
            led_buf[j * 24 + i + 8]  = ((grb[j][1] >> i) & 0x01) ? 12 : 4;
            led_buf[j * 24 + i + 16] = ((grb[j][2] >> i) & 0x01) ? 12 : 4;

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

    if (isRenewed)
    {
        HAL_TIM_PWM_Start_DMA(&htim8, TIM_CHANNEL_1, (uint32_t*) led_buf, LED_NUMS * 24 + 1);
    }
}

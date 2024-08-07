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

uint8_t led_buf[LED_NUMS * 3 * 8 + 1] = {0};

void setLedBuf(uint8_t index, uint8_t red, uint8_t green, uint8_t blue)
{
    grb[index][0] = green;
    grb[index][1] = red;
    grb[index][2] = blue;
}

void renew(void)
{
    for (int j = 0; j < LED_NUMS; j++)
    {
        for (int i = 0; i < 8; i++)
        {
            led_buf[j * 24 + i]      = ((grb[j][0] >> i) & 0x01) ? 12 : 4;
            led_buf[j * 24 + i + 8]  = ((grb[j][1] >> i) & 0x01) ? 12 : 4;
            led_buf[j * 24 + i + 16] = ((grb[j][2] >> i) & 0x01) ? 12 : 4;
        }
    }
    led_buf[LED_NUMS * 24 + 1] = 0;

    HAL_TIM_PWM_Start_DMA(&htim8, TIM_CHANNEL_1, (uint32_t*) led_buf, LED_NUMS * 24 + 1);
}

/*
 * icled.h
 *
 *  Created on: Aug 7, 2024
 *      Author: shun
 */

#ifndef INC_ICLED_H_
#define INC_ICLED_H_

#define RGB      3
#define COL_BITS 8

#define WL_LED_BIT_LEN (RGB * COL_BITS)  // g:8,r:8,b:8
#define WL_LED_ONE     12
#define WL_LED_ZERO    4

#define LED_INTENSITY_RATE 0.5

#define LED_NUMS 34

#define GRADATION_RATE_STEP 0.2
#define GRADATION_COUNT_MAX 10

#define DMA_BUF_SIZE (LED_NUMS * WL_LED_BIT_LEN + 1)

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGB_Color_t;

extern RGB_Color_t rgb_normal;
extern RGB_Color_t rgb_upper;
extern RGB_Color_t rgb_shift;
extern RGB_Color_t rgb_blank;

void setSpaceFlag(void);
void setBackspaceFlag(void);
void setEnterFlag(void);

void setLedBuf(uint8_t index, RGB_Color_t* rgb_color);
void setAllLedBuf(RGB_Color_t* rgb_color);
void setColumn2ColorLedBuf(uint8_t row, uint16_t column, RGB_Color_t* rgb_color1, RGB_Color_t* rgb_color0);
void setColumn3ColorLedBuf(uint8_t row, uint16_t column0, uint16_t column1, RGB_Color_t* rgb_color1, RGB_Color_t* rgb_color2, RGB_Color_t* rgb_color0);
void renew(void);
void led_control_task(void);

#endif /* INC_ICLED_H_ */

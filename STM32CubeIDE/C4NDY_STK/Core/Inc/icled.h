/*
 * icled.h
 *
 *  Created on: Aug 7, 2024
 *      Author: shun
 */

#ifndef INC_ICLED_H_
#define INC_ICLED_H_

#include <stdint.h>
#include <stdbool.h>

#define RGB      3
#define COL_BITS 8

#define WL_LED_BIT_LEN (RGB * COL_BITS)  // g:8,r:8,b:8
#define WL_LED_ONE     12
#define WL_LED_ZERO    4

#define LED_INTENSITY_RATE 1.0

#define LED_NUMS 34

#define GRADATION_RATE_STEP 0.2
#define GRADATION_COUNT_MAX 5

#define ANIMATION_COUNT_MAX 30

#define DMA_BUF_SIZE (LED_NUMS * WL_LED_BIT_LEN + 1)

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGB_Color_t;

void setNormalColor(const uint8_t keymapId, const uint8_t r, const uint8_t g, const uint8_t b);
void setUpperColor(const uint8_t keymapId, const uint8_t r, const uint8_t g, const uint8_t b);
void setShiftColor(const uint8_t keymapId, const uint8_t r, const uint8_t g, const uint8_t b);
RGB_Color_t* getNormalColor(const uint8_t keymapId);
RGB_Color_t* getUpperColor(const uint8_t keymapId);
RGB_Color_t* getShiftColor(const uint8_t keymapId);
RGB_Color_t* getBlankColor(void);

void setSpaceFlag(void);
void setBackspaceFlag(void);
void setEnterFlag(void);

void setLedBuf(const uint8_t index, const RGB_Color_t* rgb_color);
void setAllLedBuf(const RGB_Color_t* rgb_color);
void setColumn2ColorLedBuf(const uint8_t row, const uint16_t column, const RGB_Color_t* rgb_color1, const RGB_Color_t* rgb_color0);
void setColumn3ColorLedBuf(const uint8_t row, const uint16_t column0, const uint16_t column1, const RGB_Color_t* rgb_color1, const RGB_Color_t* rgb_color2, const RGB_Color_t* rgb_color0);
void renew(void);
void checkColor(const uint8_t r, const uint8_t g, const uint8_t b);
void led_control_task(void);

#endif /* INC_ICLED_H_ */

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

void setLedBuf(uint8_t index, uint8_t red, uint8_t green, uint8_t blue);
void setAllLedBuf(uint8_t red, uint8_t green, uint8_t blue);
void renew(void);
void led_control_task(void);

#endif /* INC_ICLED_H_ */

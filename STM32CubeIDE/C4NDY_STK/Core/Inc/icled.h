/*
 * icled.h
 *
 *  Created on: Aug 7, 2024
 *      Author: shun
 */

#ifndef INC_ICLED_H_
#define INC_ICLED_H_

#define LED_NUMS 4

void setLedBuf(uint8_t index, uint8_t red, uint8_t green, uint8_t blue);
void setAllLedBuf(uint8_t red, uint8_t green, uint8_t blue);
void renew(void);
void led_control_task(void);

#endif /* INC_ICLED_H_ */

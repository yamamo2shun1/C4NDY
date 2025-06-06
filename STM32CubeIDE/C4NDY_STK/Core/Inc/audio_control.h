/*
 * audio_control.h
 *
 *  Created on: Aug 17, 2023
 *      Author: shun
 */

#ifndef INC_AUDIO_CONTROL_H_
#define INC_AUDIO_CONTROL_H_

#include "tusb.h"

#define ADAU1761_ADDR 0x70

#define SIGMASTUDIOTYPE_FIXPOINT 0
#define SIGMASTUDIOTYPE_INTEGER  1

#define SIGMA_SAFELOAD_MODULO_RAM_SIZE 0x0000
#define SIGMA_SAFELOAD_DATA_1          0x0001
#define SIGMA_SAFELOAD_DATA_2          0x0002
#define SIGMA_SAFELOAD_DATA_3          0x0003
#define SIGMA_SAFELOAD_DATA_4          0x0004
#define SIGMA_SAFELOAD_DATA_5          0x0005
#define SIGMA_SAFELOAD_TARGET_ADDRESS  0x0006
#define SIGMA_SAFELOAD_TRIGGER         0x0007

#define SAI_RNG_BUF_SIZE   16384
#define SAI_BUF_SIZE       4096
#define N_SAMPLES_PER_1_RX 96

#define MASTER_GAIN_MIN -60
#define MASTER_GAIN_MAX 10

// Speaker data size received in the last frame
extern uint_fast16_t spk_data_size;

// Buffer for speaker data
extern int_fast32_t spk_buf[CFG_TUD_AUDIO_FUNC_1_EP_OUT_SW_BUF_SZ];
extern int_fast32_t hpout_buf[SAI_BUF_SIZE];

void start_adc(void);
void start_sai(void);

void read_audio_data_from_usb(void);
void copybuf_usb2sai(void);
void copybuf_sai2codec(void);

void send_usb_gain_L(const int16_t usb_db);
void send_usb_gain_R(const int16_t usb_db);
void send_xfade(const uint16_t fader_val);
void send_master_gain(const uint16_t master_val);
void send_master_gain_db(int master_db);
void send_switch_to_linein(void);
void send_switch_to_phonoin(void);

void codec_control_task(void);

#endif /* INC_AUDIO_CONTROL_H_ */

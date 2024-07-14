/*
 * audio_control.h
 *
 *  Created on: Aug 17, 2023
 *      Author: shun
 */

#ifndef INC_AUDIO_CONTROL_H_
#define INC_AUDIO_CONTROL_H_

#define ADAU1761_ADDR 0x70

#define SIGMASTUDIOTYPE_FIXPOINT 	0
#define SIGMASTUDIOTYPE_INTEGER 	1

#define SIGMA_SAFELOAD_MODULO_RAM_SIZE 0x0000
#define SIGMA_SAFELOAD_DATA_1 0x0001
#define SIGMA_SAFELOAD_DATA_2 0x0002
#define SIGMA_SAFELOAD_DATA_3 0x0003
#define SIGMA_SAFELOAD_DATA_4 0x0004
#define SIGMA_SAFELOAD_DATA_5 0x0005
#define SIGMA_SAFELOAD_TARGET_ADDRESS 0x0006
#define SIGMA_SAFELOAD_TRIGGER 0x0007

#define SAI_RNG_BUF_SIZE 10240//20480
#define SAI_BUF_SIZE 512

// Speaker data size received in the last frame
extern int spk_data_size;

// Buffer for speaker data
extern int32_t spk_buf[CFG_TUD_AUDIO_FUNC_1_EP_OUT_SW_BUF_SZ];
extern int32_t hpout_buf[SAI_BUF_SIZE];

void start_adc(void);
void start_sai(void);

void clear_usb_audio_buf(void);
void read_audio_data_from_usb(uint16_t n_bytes_received);
void copybuf_usb2sai(void);
void copybuf_sai2codec(void);

void send_xfade(uint16_t fader_val);
void send_master_gain(uint16_t master_val);
void send_switch_to_linein(void);
void send_switch_to_phonoin(void);

void codec_control_task(void);

#endif /* INC_AUDIO_CONTROL_H_ */

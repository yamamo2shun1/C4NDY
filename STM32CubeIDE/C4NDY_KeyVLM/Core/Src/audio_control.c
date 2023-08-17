/*
 * audio_control.c
 *
 *  Created on: Aug 17, 2023
 *      Author: shun
 */

#include "main.h"
#include "sai.h"

#include "audio_control.h"

#include "ADAU1761_IC_1_PARAM.h"
#include "ADAU1761_IC_1_REG.h"
#include "ADAU1761_IC_1.h"
#include "SigmaStudioFW.h"

uint16_t pot_value[2] = {0};
uint8_t xfade_buffer_index = 0;
uint8_t buffer_index = 0;
uint16_t xfade_buffer[16] = {0};
uint16_t xfade = 0;
uint16_t xfade_prev = 0;
uint16_t master_gain_buffer[16] = {0};
uint16_t master_gain = 0;
uint16_t master_gain_prev = 0;

uint64_t sai_buf_index = 0;
uint64_t sai_transmit_index = 0;
int32_t sai_buf[SAI_RNG_BUF_SIZE] = {0};
bool is_dma_pause = false;

// Speaker data size received in the last frame
int spk_data_size = 0;

// Buffer for microphone data
int32_t mic_buf[CFG_TUD_AUDIO_FUNC_1_EP_IN_SW_BUF_SZ / 2] = {0};

// Buffer for speaker data
int32_t spk_buf[CFG_TUD_AUDIO_FUNC_1_EP_OUT_SW_BUF_SZ / 2] = {0};
int32_t hpout_buf[CFG_TUD_AUDIO_FUNC_1_EP_OUT_SW_BUF_SZ / 2] = {0};

void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{
	copybuf_sai2codec();
}

void start_adc(void)
{
	/* Run the ADC calibration in single-ended mode */
	if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
	{
		/* Calibration Error */
		Error_Handler();
	}

	if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)pot_value, 2) != HAL_OK)
	{
		/* ADC conversion start error */
		Error_Handler();
	}
}

void start_sai(void)
{
	if (HAL_SAI_Transmit_DMA(&hsai_BlockB1, (uint8_t *)hpout_buf, SAI_BUF_SIZE / 2) != HAL_OK)
	{
		/* SAI transmit start error */
		Error_Handler();
	}
}

void clear_usb_audio_buf(void)
{
	spk_data_size = 0;
}

void read_audio_data_from_usb(uint16_t n_bytes_received)
{
	spk_data_size = tud_audio_read(spk_buf, n_bytes_received);
	copybuf_usb2sai();
}

void copybuf_usb2sai(void)
{
	for (int i = 0; i < spk_data_size / 4; i++)
	{
		sai_buf[sai_buf_index % SAI_RNG_BUF_SIZE] = spk_buf[i];
		sai_buf_index++;
	}
}

void copybuf_sai2codec(void)
{
	if (sai_buf_index >= (sai_transmit_index + SAI_BUF_SIZE / 4))
	{
		for (int i = 0; i < SAI_BUF_SIZE / 4; i++)
		{
			hpout_buf[i] = sai_buf[sai_transmit_index % SAI_RNG_BUF_SIZE];
			sai_transmit_index++;
		}
	}
}

void send_xfade(uint16_t fader_val)
{
	double xf_rate = (double)fader_val / 1023.0;

	uint8_t dc1_array[4] = {0x00};
	dc1_array[0] = ((uint32_t)((1.0 - xf_rate) * pow(2, 23)) >> 24) & 0x000000FF;
	dc1_array[1] = ((uint32_t)((1.0 - xf_rate) * pow(2, 23)) >> 16) & 0x000000FF;
	dc1_array[2] = ((uint32_t)((1.0 - xf_rate) * pow(2, 23)) >> 8)  & 0x000000FF;
	dc1_array[3] =  (uint32_t)((1.0 - xf_rate) * pow(2, 23))        & 0x000000FF;

	SIGMA_WRITE_REGISTER_BLOCK(DEVICE_ADDR_IC_1, MOD_DC1_DCINPALG1_ADDR, 4, dc1_array);

	uint8_t dc2_array[4] = {0x00};
	dc2_array[0] = ((uint32_t)(xf_rate * pow(2, 23)) >> 24) & 0x000000FF;
	dc2_array[1] = ((uint32_t)(xf_rate * pow(2, 23)) >> 16) & 0x000000FF;
	dc2_array[2] = ((uint32_t)(xf_rate * pow(2, 23)) >> 8)  & 0x000000FF;
	dc2_array[3] =  (uint32_t)(xf_rate * pow(2, 23))        & 0x000000FF;

	SIGMA_WRITE_REGISTER_BLOCK(DEVICE_ADDR_IC_1, MOD_DC2_DCINPALG2_ADDR, 4, dc2_array);
}

void send_master_gain(uint16_t master_val)
{
	double master_db = (135.0 / 1023.0) * (double)master_val - 120.0;

	double master_rate = pow(10.0, master_db / 20);

	uint8_t master_gain_array[8] = {0x00};
	master_gain_array[0] = ((uint32_t)(master_rate * pow(2, 23)) >> 24) & 0x000000FF;
	master_gain_array[1] = ((uint32_t)(master_rate * pow(2, 23)) >> 16) & 0x000000FF;
	master_gain_array[2] = ((uint32_t)(master_rate * pow(2, 23)) >> 8)  & 0x000000FF;
	master_gain_array[3] =  (uint32_t)(master_rate * pow(2, 23))        & 0x000000FF;
	master_gain_array[4] = 0x00;// if_step
	master_gain_array[5] = 0x00;
	master_gain_array[6] = 0x80;
	master_gain_array[7] = 0x00;
#if 0
	SEGGER_RTT_printf(0, "%d -> %02X,%02X,%02X,%02X\n", master_val,
													  master_gain_array[0],
													  master_gain_array[1],
													  master_gain_array[2],
													  master_gain_array[3]);
#endif
	SIGMA_SAFELOAD_WRITE_DATA(DEVICE_ADDR_IC_1, SIGMA_SAFELOAD_DATA_1, 8, master_gain_array);

	uint8_t target_address_count[8] = {0x00, 0x00, 0x00, MOD_MASTERGAIN_ALG0_TARGET_ADDR - 1,
									   0x00, 0x00, 0x00, MOD_MASTERGAIN_COUNT};
	SIGMA_SAFELOAD_WRITE_DATA(DEVICE_ADDR_IC_1, SIGMA_SAFELOAD_TARGET_ADDRESS, 8, target_address_count);
}

void send_switch_to_linein(void)
{
	uint8_t switch0_array[4] = {0x00, 0x80, 0x00, 0x00};
	SIGMA_SAFELOAD_WRITE_DATA(DEVICE_ADDR_IC_1, SIGMA_SAFELOAD_DATA_1, 4, switch0_array);

	uint8_t target_address0_count[8] = {0x00, 0x00, 0x00, MOD_LNPHSW_ALG0_STEREODEMUXSLEW10_ADDR - 1,
									    0x00, 0x00, 0x00, 0x01};
	SIGMA_SAFELOAD_WRITE_DATA(DEVICE_ADDR_IC_1, SIGMA_SAFELOAD_TARGET_ADDRESS, 8, target_address0_count);

	uint8_t switch1_array[4] = {0x00, 0x00, 0x00, 0x00};
	SIGMA_SAFELOAD_WRITE_DATA(DEVICE_ADDR_IC_1, SIGMA_SAFELOAD_DATA_1, 4, switch1_array);

	uint8_t target_address1_count[8] = {0x00, 0x00, 0x00, MOD_LNPHSW_ALG0_STEREODEMUXSLEW11_ADDR - 1,
									    0x00, 0x00, 0x00, 0x01};
	SIGMA_SAFELOAD_WRITE_DATA(DEVICE_ADDR_IC_1, SIGMA_SAFELOAD_TARGET_ADDRESS, 8, target_address1_count);
}

void send_switch_to_phonoin(void)
{
	uint8_t switch0_array[4] = {0x00, 0x00, 0x00, 0x00};
	SIGMA_SAFELOAD_WRITE_DATA(DEVICE_ADDR_IC_1, SIGMA_SAFELOAD_DATA_1, 4, switch0_array);

	uint8_t target_address0_count[8] = {0x00, 0x00, 0x00, MOD_LNPHSW_ALG0_STEREODEMUXSLEW10_ADDR - 1,
									    0x00, 0x00, 0x00, 0x01};
	SIGMA_SAFELOAD_WRITE_DATA(DEVICE_ADDR_IC_1, SIGMA_SAFELOAD_TARGET_ADDRESS, 8, target_address0_count);

	uint8_t switch1_array[4] = {0x00, 0x80, 0x00, 0x00};
	SIGMA_SAFELOAD_WRITE_DATA(DEVICE_ADDR_IC_1, SIGMA_SAFELOAD_DATA_1, 4, switch1_array);

	uint8_t target_address1_count[8] = {0x00, 0x00, 0x00, MOD_LNPHSW_ALG0_STEREODEMUXSLEW11_ADDR - 1,
									    0x00, 0x00, 0x00, 0x01};
	SIGMA_SAFELOAD_WRITE_DATA(DEVICE_ADDR_IC_1, SIGMA_SAFELOAD_TARGET_ADDRESS, 8, target_address1_count);
}

void codec_control_task(void)
{
	xfade_buffer[xfade_buffer_index] = pot_value[1] >> 2;
	xfade_buffer_index = (xfade_buffer_index + 1) & (16 - 1);
	xfade = 0;
	for (int i = 0; i < 16; i++)
	{
		xfade += xfade_buffer[i];
	}
	xfade >>= 4;

	if (abs(xfade - xfade_prev) > 2)
	{
		send_xfade(xfade);
		xfade_prev = xfade;
	}

	master_gain_buffer[buffer_index] = pot_value[0] >> 2;
	buffer_index = (buffer_index + 1) & (16 - 1);
	master_gain = 0;
	for (int i = 0; i < 16; i++)
	{
		master_gain += master_gain_buffer[i];
	}
	master_gain >>= 4;

	if (abs(master_gain - master_gain_prev) > 2)
	{
		send_master_gain(master_gain);
		master_gain_prev = master_gain;
	}
}

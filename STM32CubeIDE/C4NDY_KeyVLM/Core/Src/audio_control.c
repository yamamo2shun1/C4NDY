/*
 * audio_control.c
 *
 *  Created on: Aug 17, 2023
 *      Author: shun
 */

#include <math.h>

#include "main.h"
#include "sai.h"

#include "audio_control.h"

#include "ADAU1761_IC_1_PARAM.h"
#include "ADAU1761_IC_1_REG.h"
#include "ADAU1761_IC_1.h"
#include "SigmaStudioFW.h"

// List of supported sample rates
const uint32_t sample_rates[] = {48000};
uint32_t current_sample_rate  = 48000;

enum
{
  VOLUME_CTRL_0_DB = 0,
  VOLUME_CTRL_10_DB = 2560,
  VOLUME_CTRL_20_DB = 5120,
  VOLUME_CTRL_30_DB = 7680,
  VOLUME_CTRL_40_DB = 10240,
  VOLUME_CTRL_50_DB = 12800,
  VOLUME_CTRL_60_DB = 15360,
  VOLUME_CTRL_70_DB = 17920,
  VOLUME_CTRL_80_DB = 20480,
  VOLUME_CTRL_90_DB = 23040,
  VOLUME_CTRL_100_DB = 25600,
  VOLUME_CTRL_SILENCE = 0x8000,
};

// Audio controls
// Current states
int8_t mute[CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX + 1];       // +1 for master channel 0
int16_t volume[CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX + 1];    // +1 for master channel 0

// Resolution per format
const uint8_t resolutions_per_format[CFG_TUD_AUDIO_FUNC_1_N_FORMATS] = {CFG_TUD_AUDIO_FUNC_1_FORMAT_1_RESOLUTION_RX,
                                                                        CFG_TUD_AUDIO_FUNC_1_FORMAT_2_RESOLUTION_RX};
// Current resolution, update on format change
uint8_t current_resolution = 16;

#define N_SAMPLE_RATES  TU_ARRAY_SIZE(sample_rates)

uint16_t pot_value[2] = {0};
uint8_t xfade_buffer_index = 0;
uint8_t buffer_index = 0;
uint16_t xfade_buffer[16] = {0};
uint16_t xfade = 0;
uint16_t xfade_prev = 255;
uint16_t master_gain_buffer[16] = {0};
uint16_t master_gain = 0;
uint16_t master_gain_prev = 255;

uint32_t sai_buf_index = 0;
uint32_t sai_transmit_index = 0;
int32_t sai_buf[SAI_RNG_BUF_SIZE * 2] = {0};
bool is_dma_pause = false;

// Speaker data size received in the last frame
int spk_data_size = 0;

// Buffer for speaker data
int32_t spk_buf[CFG_TUD_AUDIO_FUNC_1_EP_OUT_SW_BUF_SZ] = {0};
int32_t hpout_buf[SAI_BUF_SIZE] = {0};

// Helper for clock get requests
static bool tud_audio_clock_get_request(uint8_t rhport, audio_control_request_t const *request)
{
  TU_ASSERT(request->bEntityID == UAC2_ENTITY_CLOCK);

  if (request->bControlSelector == AUDIO_CS_CTRL_SAM_FREQ)
  {
    if (request->bRequest == AUDIO_CS_REQ_CUR)
    {
      SEGGER_RTT_printf(0, "Clock get current freq %lu\r\n", current_sample_rate);

      audio_control_cur_4_t curf = { (int32_t) tu_htole32(current_sample_rate) };
      return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &curf, sizeof(curf));
    }
    else if (request->bRequest == AUDIO_CS_REQ_RANGE)
    {
      audio_control_range_4_n_t(N_SAMPLE_RATES) rangef =
      {
        .wNumSubRanges = tu_htole16(N_SAMPLE_RATES)
      };
      SEGGER_RTT_printf(0, "Clock get %d freq ranges\r\n", N_SAMPLE_RATES);
      for(uint8_t i = 0; i < N_SAMPLE_RATES; i++)
      {
        rangef.subrange[i].bMin = (int32_t) sample_rates[i];
        rangef.subrange[i].bMax = (int32_t) sample_rates[i];
        rangef.subrange[i].bRes = 0;
        SEGGER_RTT_printf(0, "Range %d (%d, %d, %d)\r\n", i, (int)rangef.subrange[i].bMin, (int)rangef.subrange[i].bMax, (int)rangef.subrange[i].bRes);
      }

      return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &rangef, sizeof(rangef));
    }
  }
  else if (request->bControlSelector == AUDIO_CS_CTRL_CLK_VALID &&
           request->bRequest == AUDIO_CS_REQ_CUR)
  {
    audio_control_cur_1_t cur_valid = { .bCur = 1 };
    SEGGER_RTT_printf(0, "Clock get is valid %u\r\n", cur_valid.bCur);
    return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &cur_valid, sizeof(cur_valid));
  }
  SEGGER_RTT_printf(0, "Clock get request not supported, entity = %u, selector = %u, request = %u\r\n",
          request->bEntityID, request->bControlSelector, request->bRequest);
  return false;
}

// Helper for clock set requests
static bool tud_audio_clock_set_request(uint8_t rhport, audio_control_request_t const *request, uint8_t const *buf)
{
  (void)rhport;

  TU_ASSERT(request->bEntityID == UAC2_ENTITY_CLOCK);
  TU_VERIFY(request->bRequest == AUDIO_CS_REQ_CUR);

  if (request->bControlSelector == AUDIO_CS_CTRL_SAM_FREQ)
  {
    TU_VERIFY(request->wLength == sizeof(audio_control_cur_4_t));

    current_sample_rate = (uint32_t) ((audio_control_cur_4_t const *)buf)->bCur;

    SEGGER_RTT_printf(0, "Clock set current freq: %ld\r\n", current_sample_rate);

    return true;
  }
  else
  {
	SEGGER_RTT_printf(0, "Clock set request not supported, entity = %u, selector = %u, request = %u\r\n",
            request->bEntityID, request->bControlSelector, request->bRequest);
    return false;
  }
}

// Helper for feature unit get requests
// PC側で音量調整をするとここも呼ばれる
static bool tud_audio_feature_unit_get_request(uint8_t rhport, audio_control_request_t const *request)
{
  TU_ASSERT(request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT);

  if (request->bControlSelector == AUDIO_FU_CTRL_MUTE && request->bRequest == AUDIO_CS_REQ_CUR)
  {
    audio_control_cur_1_t mute1 = { .bCur = mute[request->bChannelNumber] };
    SEGGER_RTT_printf(0, "Get channel %u mute %d\r\n", request->bChannelNumber, mute1.bCur);
    return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &mute1, sizeof(mute1));
  }
  else if (UAC2_ENTITY_SPK_FEATURE_UNIT && request->bControlSelector == AUDIO_FU_CTRL_VOLUME)
  {
    if (request->bRequest == AUDIO_CS_REQ_RANGE)
    {
      audio_control_range_2_n_t(1) range_vol = {
        .wNumSubRanges = tu_htole16(1),
        .subrange[0] = { .bMin = tu_htole16(-VOLUME_CTRL_50_DB), tu_htole16(VOLUME_CTRL_0_DB), tu_htole16(256) }
      };
      SEGGER_RTT_printf(0, "Get channel %u volume range (%d, %d, %u) dB\r\n", request->bChannelNumber,
              range_vol.subrange[0].bMin / 256, range_vol.subrange[0].bMax / 256, range_vol.subrange[0].bRes / 256);
      return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &range_vol, sizeof(range_vol));
    }
    else if (request->bRequest == AUDIO_CS_REQ_CUR)
    {
      audio_control_cur_2_t cur_vol = { .bCur = tu_htole16(volume[request->bChannelNumber]) };
      SEGGER_RTT_printf(0, "Get channel %u volume %d dB\r\n", request->bChannelNumber, cur_vol.bCur / 256);
      return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &cur_vol, sizeof(cur_vol));
    }
  }
  SEGGER_RTT_printf(0, "Feature unit get request not supported, entity = %u, selector = %u, request = %u\r\n",
          request->bEntityID, request->bControlSelector, request->bRequest);

  return false;
}

// Helper for feature unit set requests
// PC側で音量調整をするとこの関数が呼ばれる
static bool tud_audio_feature_unit_set_request(uint8_t rhport, audio_control_request_t const *request, uint8_t const *buf)
{
  (void)rhport;

  TU_ASSERT(request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT);
  TU_VERIFY(request->bRequest == AUDIO_CS_REQ_CUR);

  if (request->bControlSelector == AUDIO_FU_CTRL_MUTE)
  {
    TU_VERIFY(request->wLength == sizeof(audio_control_cur_1_t));

    mute[request->bChannelNumber] = ((audio_control_cur_1_t const *)buf)->bCur;

    SEGGER_RTT_printf(0, "Set channel %d Mute: %d\r\n", request->bChannelNumber, mute[request->bChannelNumber]);

    return true;
  }
  else if (request->bControlSelector == AUDIO_FU_CTRL_VOLUME)
  {
    TU_VERIFY(request->wLength == sizeof(audio_control_cur_2_t));

    volume[request->bChannelNumber] = ((audio_control_cur_2_t const *)buf)->bCur;

    switch (request->bChannelNumber)
    {
    case 1:
    	send_usb_gain_L(volume[request->bChannelNumber] / 256);
    	break;
    case 2:
    	send_usb_gain_R(volume[request->bChannelNumber] / 256);
    	break;
    }

    SEGGER_RTT_printf(0, "Set channel %d volume: %d dB\r\n", request->bChannelNumber, volume[request->bChannelNumber] / 256);

    return true;
  }
  else
  {
	SEGGER_RTT_printf(0, "Feature unit set request not supported, entity = %u, selector = %u, request = %u\r\n",
            request->bEntityID, request->bControlSelector, request->bRequest);
    return false;
  }
}

// Invoked when audio class specific get request received for an entity
bool tud_audio_get_req_entity_cb(uint8_t rhport, tusb_control_request_t const *p_request)
{
  audio_control_request_t const *request = (audio_control_request_t const *)p_request;

  if (request->bEntityID == UAC2_ENTITY_CLOCK)
    return tud_audio_clock_get_request(rhport, request);
  if (request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT)
    return tud_audio_feature_unit_get_request(rhport, request);
  else
  {
	SEGGER_RTT_printf(0, "Get request not handled, entity = %d, selector = %d, request = %d\r\n",
            request->bEntityID, request->bControlSelector, request->bRequest);
  }
  return false;
}

// Invoked when audio class specific set request received for an entity
bool tud_audio_set_req_entity_cb(uint8_t rhport, tusb_control_request_t const *p_request, uint8_t *buf)
{
  audio_control_request_t const *request = (audio_control_request_t const *)p_request;

  if (request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT)
    return tud_audio_feature_unit_set_request(rhport, request, buf);
  if (request->bEntityID == UAC2_ENTITY_CLOCK)
    return tud_audio_clock_set_request(rhport, request, buf);
  SEGGER_RTT_printf(0, "Set request not handled, entity = %d, selector = %d, request = %d\r\n",
          request->bEntityID, request->bControlSelector, request->bRequest);

  return false;
}

bool tud_audio_set_itf_close_EP_cb(uint8_t rhport, tusb_control_request_t const * p_request)
{
  (void)rhport;

  uint8_t const itf = tu_u16_low(tu_le16toh(p_request->wIndex));
  uint8_t const alt = tu_u16_low(tu_le16toh(p_request->wValue));

#if 0
  if (ITF_NUM_AUDIO_STREAMING_SPK == itf && alt == 0)
      blink_interval_ms = BLINK_MOUNTED;
#endif

  return true;
}

bool tud_audio_set_itf_cb(uint8_t rhport, tusb_control_request_t const * p_request)
{
  (void)rhport;
  uint8_t const itf = tu_u16_low(tu_le16toh(p_request->wIndex));
  uint8_t const alt = tu_u16_low(tu_le16toh(p_request->wValue));

#if 0
  TU_LOG2("Set interface %d alt %d\r\n", itf, alt);
  if (ITF_NUM_AUDIO_STREAMING_SPK == itf && alt != 0)
      blink_interval_ms = BLINK_STREAMING;
#endif

  // Clear buffer when streaming format is changed
  clear_usb_audio_buf();
  if(alt != 0)
  {
    current_resolution = resolutions_per_format[alt-1];
  }

  return true;
}

bool tud_audio_rx_done_pre_read_cb(uint8_t rhport, uint16_t n_bytes_received, uint8_t func_id, uint8_t ep_out, uint8_t cur_alt_setting)
{
  (void)rhport;
  (void)func_id;
  (void)ep_out;
  (void)cur_alt_setting;

  read_audio_data_from_usb(n_bytes_received);

  //SEGGER_RTT_printf(0, "sai_buf_index = %d\n", sai_buf_index);

  return true;
}

bool tud_audio_tx_done_pre_load_cb(uint8_t rhport, uint8_t itf, uint8_t ep_in, uint8_t cur_alt_setting)
{
  (void)rhport;
  (void)itf;
  (void)ep_in;
  (void)cur_alt_setting;

  // This callback could be used to fill microphone data separately
  return true;
}

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
	if (HAL_SAI_Transmit_DMA(&hsai_BlockB1, (uint8_t *)hpout_buf, SAI_BUF_SIZE) != HAL_OK)
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
	if (sai_buf_index >= (sai_transmit_index + SAI_BUF_SIZE))
	{
		for (int i = 0; i < SAI_BUF_SIZE; i++)
		{
			//hpout_buf[i] = sai_buf[sai_transmit_index % SAI_RNG_BUF_SIZE];
			int32_t x = sai_buf[sai_transmit_index % SAI_RNG_BUF_SIZE];
			hpout_buf[i] = (0x0000FFFF & x >> 16) | x << 16;
			sai_transmit_index++;
		}
	}
}

void send_usb_gain_L(int16_t usb_db)
{
	double usb_rate = pow(10.0, (double)usb_db / 20.0);

	uint8_t usb_gain_array[8] = {0x00};
	usb_gain_array[0] = ((uint32_t)(usb_rate * pow(2.0, 23.0)) >> 24) & 0x000000FF;
	usb_gain_array[1] = ((uint32_t)(usb_rate * pow(2.0, 23.0)) >> 16) & 0x000000FF;
	usb_gain_array[2] = ((uint32_t)(usb_rate * pow(2.0, 23.0)) >> 8)  & 0x000000FF;
	usb_gain_array[3] =  (uint32_t)(usb_rate * pow(2.0, 23.0))        & 0x000000FF;
	usb_gain_array[4] = 0x00;// if_step
	usb_gain_array[5] = 0x00;
	usb_gain_array[6] = 0x80;
	usb_gain_array[7] = 0x00;

	SIGMA_SAFELOAD_WRITE_DATA(DEVICE_ADDR_IC_1, SIGMA_SAFELOAD_DATA_1, 8, usb_gain_array);

	uint8_t target_address_count[8] = {0x00, 0x00, 0x00, MOD_USBGAINL_ALG0_TARGET_ADDR - 1,
									   0x00, 0x00, 0x00, MOD_USBGAINL_COUNT};
	SIGMA_SAFELOAD_WRITE_DATA(DEVICE_ADDR_IC_1, SIGMA_SAFELOAD_TARGET_ADDRESS, 8, target_address_count);
}

void send_usb_gain_R(int16_t usb_db)
{
	double usb_rate = pow(10.0, (double)usb_db / 20.0);

	uint8_t usb_gain_array[8] = {0x00};
	usb_gain_array[0] = ((uint32_t)(usb_rate * pow(2.0, 23.0)) >> 24) & 0x000000FF;
	usb_gain_array[1] = ((uint32_t)(usb_rate * pow(2.0, 23.0)) >> 16) & 0x000000FF;
	usb_gain_array[2] = ((uint32_t)(usb_rate * pow(2.0, 23.0)) >> 8)  & 0x000000FF;
	usb_gain_array[3] =  (uint32_t)(usb_rate * pow(2.0, 23.0))        & 0x000000FF;
	usb_gain_array[4] = 0x00;// if_step
	usb_gain_array[5] = 0x00;
	usb_gain_array[6] = 0x80;
	usb_gain_array[7] = 0x00;

	SIGMA_SAFELOAD_WRITE_DATA(DEVICE_ADDR_IC_1, SIGMA_SAFELOAD_DATA_1, 8, usb_gain_array);

	uint8_t target_address_count[8] = {0x00, 0x00, 0x00, MOD_USBGAINR_ALG0_TARGET_ADDR - 1,
									   0x00, 0x00, 0x00, MOD_USBGAINR_COUNT};
	SIGMA_SAFELOAD_WRITE_DATA(DEVICE_ADDR_IC_1, SIGMA_SAFELOAD_TARGET_ADDRESS, 8, target_address_count);
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
	double c_curve_val = 1038.0 * tanh((double)master_val / 448.0);
	double master_db = (135.0 / 1023.0) * c_curve_val - 120.0;

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

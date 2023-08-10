/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "quadspi.h"
#include "sai.h"
#include "tim.h"
#include "usb.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include "usb_descriptors.h"

#include "ADAU1761_IC_1_PARAM.h"
#include "ADAU1761_IC_1_REG.h"
#include "ADAU1761_IC_1.h"
#include "SigmaStudioFW.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define AUDIO_SAMPLE_RATE 48000

#define ADAU1761_ADDR 0x70

#define MATRIX_ROWS 5
#define MATRIX_COLUMNS 13
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
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

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint16_t pot_value[2] = {0};
uint8_t buffer_index = 0;
uint16_t master_gain_buffer[16] = {0};
uint16_t master_gain = 0;
uint16_t master_gain_prev = 0;

struct keyboardHID_t {
	uint8_t modifiers;
	uint8_t reserved;
	uint8_t key[6];
} keyboardHID;

uint16_t keyState[MATRIX_ROWS] = {0x0};
uint16_t prevKeyState[MATRIX_ROWS] = {0x0};

bool isKeymapIDChanged = false;
uint8_t keymapID = 0;

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

// Buffer for microphone data
int32_t mic_buf[CFG_TUD_AUDIO_FUNC_1_EP_IN_SW_BUF_SZ / 2];
// Buffer for speaker data
#if 0
int32_t spk_buf[CFG_TUD_AUDIO_FUNC_1_EP_OUT_SW_BUF_SZ / 2];
#else
int32_t spk_buf[192 * 10] = {0};
#endif

// Speaker data size received in the last frame
int spk_data_size;
// Resolution per format
const uint8_t resolutions_per_format[CFG_TUD_AUDIO_FUNC_1_N_FORMATS] = {CFG_TUD_AUDIO_FUNC_1_FORMAT_1_RESOLUTION_RX,
                                                                        CFG_TUD_AUDIO_FUNC_1_FORMAT_2_RESOLUTION_RX};
// Current resolution, update on format change
uint8_t current_resolution = 16;

#define N_SAMPLE_RATES  TU_ARRAY_SIZE(sample_rates)

// deafult QWERTY layout
uint8_t keymaps_default[MATRIX_ROWS][MATRIX_COLUMNS] = {
//       ESC   1     2     3     4     5     6     7     8     9     0     -_    =+
		{0x29, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x2D, 0x2E},
//       TAB   q     w     e     r     t     y     u     i     o     p     [{    ]}
		{0x2B, 0x14, 0x1A, 0x08, 0x15, 0x17, 0x1C, 0x18, 0x0C, 0x12, 0x13, 0x2F, 0x30},
//       CAPS  a     s     d     f     g     h     j     k     l     ;:    '"    \|
		{0x39, 0x04, 0x16, 0x07, 0x09, 0x0A, 0x0B, 0x0D, 0x0E, 0x0F, 0x33, 0x34, 0x31},
//       LSFT  z     x     c     v     b     n     m     ,<    .>    /?    RSFT  `~
		{0xE1, 0x1D, 0x1B, 0x06, 0x19, 0x05, 0x11, 0x10, 0x36, 0x37, 0x38, 0xE5, 0x35},
//       GUI               LALT  BS    ENT   SPC   Int4  RCTRL ◀     ▼     ▲     ►
		{0xE3, 0xFE, 0xFF, 0xE2, 0x2A, 0x28, 0x2C, 0x8A, 0xE4, 0x50, 0x51, 0x52, 0x4F}
};

// Pinky-less Dvorak layout
uint8_t keymaps_pinkyless[MATRIX_ROWS][MATRIX_COLUMNS] = {
//       ESC   1     2     3     4     5     6     7     8     9     0     [{    }]
		{0x29, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x2F, 0x30},
//       TAB   '"    ,<    o     u     y     f     g     c     r     l     /?    +=
		{0x2B, 0x34, 0x36, 0x12, 0x18, 0x1C, 0x09, 0x0A, 0x06, 0x15, 0x0F, 0x38, 0x2E},
//       LCTRL p     i     e     a     .>    d     s     t     h     z     -_    \|
		{0xE0, 0x13, 0x0C, 0x08, 0x04, 0x37, 0x07, 0x16, 0x17, 0x0B, 0x1D, 0x2D, 0x31},
//       LSFT  j     q     ;:    k     x     b     m     w     n     v     RSFT  `~
		{0xE1, 0x0D, 0x14, 0x33, 0x0E, 0x1B, 0x05, 0x10, 0x1A, 0x11, 0x19, 0xE5, 0x35},
//       GUI               LALT  BS    DEL   ENT   SPC   CAPS  ◀     ▼     ▲     ►
		{0xE3, 0xFE, 0xFF, 0xE2, 0x2A, 0x4C, 0x28, 0x2C, 0x39, 0x50, 0x51, 0x52, 0x4F}
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
#if 0
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len)
{
  (void) instance;
  (void) len;

  uint8_t next_report_id = report[0] + 1u;

  if (next_report_id < REPORT_ID_COUNT)
  {
    send_hid_report(next_report_id, board_button_read());
  }
}
#endif

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  // TODO not Implemented
  (void) instance;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  (void) instance;
#if 0
  if (report_type == HID_REPORT_TYPE_OUTPUT)
  {
    // Set keyboard LED e.g Capslock, Numlock etc...
    if (report_id == REPORT_ID_KEYBOARD)
    {
      // bufsize should be (at least) 1
      if ( bufsize < 1 ) return;

      uint8_t const kbd_leds = buffer[0];

      if (kbd_leds & KEYBOARD_LED_CAPSLOCK)
      {
        // Capslock On: disable blink, turn led on
        blink_interval_ms = 0;
        board_led_write(true);
      }else
      {
        // Caplocks Off: back to normal blink
        board_led_write(false);
        blink_interval_ms = BLINK_MOUNTED;
      }
    }
  }
#endif
}

// Helper for clock get requests
static bool tud_audio_clock_get_request(uint8_t rhport, audio_control_request_t const *request)
{
  TU_ASSERT(request->bEntityID == UAC2_ENTITY_CLOCK);

  if (request->bControlSelector == AUDIO_CS_CTRL_SAM_FREQ)
  {
    if (request->bRequest == AUDIO_CS_REQ_CUR)
    {
      TU_LOG1("Clock get current freq %lu\r\n", current_sample_rate);

      audio_control_cur_4_t curf = { (int32_t) tu_htole32(current_sample_rate) };
      return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &curf, sizeof(curf));
    }
    else if (request->bRequest == AUDIO_CS_REQ_RANGE)
    {
      audio_control_range_4_n_t(N_SAMPLE_RATES) rangef =
      {
        .wNumSubRanges = tu_htole16(N_SAMPLE_RATES)
      };
      TU_LOG1("Clock get %d freq ranges\r\n", N_SAMPLE_RATES);
      for(uint8_t i = 0; i < N_SAMPLE_RATES; i++)
      {
        rangef.subrange[i].bMin = (int32_t) sample_rates[i];
        rangef.subrange[i].bMax = (int32_t) sample_rates[i];
        rangef.subrange[i].bRes = 0;
        TU_LOG1("Range %d (%d, %d, %d)\r\n", i, (int)rangef.subrange[i].bMin, (int)rangef.subrange[i].bMax, (int)rangef.subrange[i].bRes);
      }

      return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &rangef, sizeof(rangef));
    }
  }
  else if (request->bControlSelector == AUDIO_CS_CTRL_CLK_VALID &&
           request->bRequest == AUDIO_CS_REQ_CUR)
  {
    audio_control_cur_1_t cur_valid = { .bCur = 1 };
    TU_LOG1("Clock get is valid %u\r\n", cur_valid.bCur);
    return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &cur_valid, sizeof(cur_valid));
  }
  TU_LOG1("Clock get request not supported, entity = %u, selector = %u, request = %u\r\n",
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

    TU_LOG1("Clock set current freq: %ld\r\n", current_sample_rate);

    return true;
  }
  else
  {
    TU_LOG1("Clock set request not supported, entity = %u, selector = %u, request = %u\r\n",
            request->bEntityID, request->bControlSelector, request->bRequest);
    return false;
  }
}

// Helper for feature unit get requests
static bool tud_audio_feature_unit_get_request(uint8_t rhport, audio_control_request_t const *request)
{
  TU_ASSERT(request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT);

  if (request->bControlSelector == AUDIO_FU_CTRL_MUTE && request->bRequest == AUDIO_CS_REQ_CUR)
  {
    audio_control_cur_1_t mute1 = { .bCur = mute[request->bChannelNumber] };
    TU_LOG1("Get channel %u mute %d\r\n", request->bChannelNumber, mute1.bCur);
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
      TU_LOG1("Get channel %u volume range (%d, %d, %u) dB\r\n", request->bChannelNumber,
              range_vol.subrange[0].bMin / 256, range_vol.subrange[0].bMax / 256, range_vol.subrange[0].bRes / 256);
      return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &range_vol, sizeof(range_vol));
    }
    else if (request->bRequest == AUDIO_CS_REQ_CUR)
    {
      audio_control_cur_2_t cur_vol = { .bCur = tu_htole16(volume[request->bChannelNumber]) };
      TU_LOG1("Get channel %u volume %d dB\r\n", request->bChannelNumber, cur_vol.bCur / 256);
      return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &cur_vol, sizeof(cur_vol));
    }
  }
  TU_LOG1("Feature unit get request not supported, entity = %u, selector = %u, request = %u\r\n",
          request->bEntityID, request->bControlSelector, request->bRequest);

  return false;
}

// Helper for feature unit set requests
static bool tud_audio_feature_unit_set_request(uint8_t rhport, audio_control_request_t const *request, uint8_t const *buf)
{
  (void)rhport;

  TU_ASSERT(request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT);
  TU_VERIFY(request->bRequest == AUDIO_CS_REQ_CUR);

  if (request->bControlSelector == AUDIO_FU_CTRL_MUTE)
  {
    TU_VERIFY(request->wLength == sizeof(audio_control_cur_1_t));

    mute[request->bChannelNumber] = ((audio_control_cur_1_t const *)buf)->bCur;

    TU_LOG1("Set channel %d Mute: %d\r\n", request->bChannelNumber, mute[request->bChannelNumber]);

    return true;
  }
  else if (request->bControlSelector == AUDIO_FU_CTRL_VOLUME)
  {
    TU_VERIFY(request->wLength == sizeof(audio_control_cur_2_t));

    volume[request->bChannelNumber] = ((audio_control_cur_2_t const *)buf)->bCur;

    TU_LOG1("Set channel %d volume: %d dB\r\n", request->bChannelNumber, volume[request->bChannelNumber] / 256);

    return true;
  }
  else
  {
    TU_LOG1("Feature unit set request not supported, entity = %u, selector = %u, request = %u\r\n",
            request->bEntityID, request->bControlSelector, request->bRequest);
    return false;
  }
}

//--------------------------------------------------------------------+
// Application Callback API Implementations
//--------------------------------------------------------------------+
// Invoked when device is mounted
void tud_mount_cb(void)
{
  //blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
  //blink_interval_ms = BLINK_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void)remote_wakeup_en;
  //blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
  //blink_interval_ms = BLINK_MOUNTED;
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
    TU_LOG1("Get request not handled, entity = %d, selector = %d, request = %d\r\n",
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
  TU_LOG1("Set request not handled, entity = %d, selector = %d, request = %d\r\n",
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
  spk_data_size = 0;
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

  spk_data_size = tud_audio_read(spk_buf, n_bytes_received);

  //SEGGER_RTT_printf(0, "size = %d %d\n", spk_data_size, n_bytes_received);

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

void audio_task(void)
{
	if (spk_data_size)
	{
		HAL_SAI_Transmit(&hsai_BlockB1, (uint8_t *)spk_buf, spk_data_size / 2, 10000);
		spk_data_size = 0;
	}
#if 0
	else
	{
		//SEGGER_RTT_printf(0, "pot = %d, %d\n", pot_value[0] >> 2, pot_value[1] >> 2);
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
			//SEGGER_RTT_printf(0, "master gain = %d\n", master_gain);
			send_master_gain(master_gain);
			master_gain_prev = master_gain;
		}
	}
#endif
}

uint8_t getKeyCode(uint8_t keymapId, uint8_t x, uint8_t y)
{
	if (keymapId == 0)
	{
		return keymaps_default[x][y];
	}
	else
	{
		return keymaps_pinkyless[x][y];
	}
}

void resetKeys(void)
{
	keyboardHID.modifiers = 0;
	for (int k = 0; k < 6; k++)
	{
		keyboardHID.key[k] = 0;
	}
}

void clearKeys(uint8_t code)
{
	if (code == 0xFF)
	{
		isKeymapIDChanged = false;
	}
	else if (code >= 0xE0 && code <= 0xE7)
	{
		keyboardHID.modifiers &= ~(1 << (code - 0xE0));
	}
	else
	{
		for (int k = 0; k < 6; k++)
		{
			if (keyboardHID.key[k] == code)
			{
				keyboardHID.key[k] = 0;
			}
		}
	}
}

void setKeys(uint8_t code)
{
	if (code == 0xFF)
	{
		if (!isKeymapIDChanged)
		{
			if (keymapID == 0)
			{
				keymapID = 1;
				HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_SET);
			}
			else
			{
				keymapID = 0;
				HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET);
			}
			isKeymapIDChanged = true;
		}
	}
	else if (code >= 0xE0 && code <= 0xE7)
	{
		keyboardHID.modifiers |= 1 << (code - 0xE0);
	}
	else
	{
		for (int k = 0; k < 6; k++)
		{
			if (keyboardHID.key[k] == code)
			{
				break;
			}
			else if (keyboardHID.key[k] == 0x00)
			{
				keyboardHID.key[k] = code;
				break;
			}
		}
	}
}

void test0(void)
{
	HAL_GPIO_WritePin(HC164_CLK_GPIO_Port, HC164_CLK_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(HC164_CLK_GPIO_Port, HC164_CLK_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(HC164_A_GPIO_Port, HC164_A_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(HC165_SL_GPIO_Port, HC165_SL_Pin, GPIO_PIN_RESET);
	asm("NOP");
	HAL_GPIO_WritePin(HC165_SL_GPIO_Port, HC165_SL_Pin, GPIO_PIN_SET);
}

void test(int i, int startPos, int endPos)
{
	for (int j = startPos; j < endPos; j++)
	{
		uint8_t jj = 255;
		if (j < 8)
		{
			jj = j + 5;
		}
		else if (j >= 11 && j < 16)
		{
			jj = j - 11;
		}

		if (jj < MATRIX_COLUMNS)
		{
			if (HAL_GPIO_ReadPin(HC165_QH_GPIO_Port, HC165_QH_Pin))
			{
				keyState[i] &= ~((uint16_t)1 << jj);

				if (keyState[i] != prevKeyState[i])
				{
					uint8_t keycode = getKeyCode(keymapID, i, (MATRIX_COLUMNS - 1) - jj);
					clearKeys(keycode);
				}
			}
			else
			{
				keyState[i] |= ((uint16_t)1 << jj);

				uint8_t keycode = getKeyCode(keymapID, i, (MATRIX_COLUMNS - 1) - jj);
				setKeys(keycode);
			}
		}

		HAL_GPIO_WritePin(HC165_CLK_GPIO_Port, HC165_CLK_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(HC165_CLK_GPIO_Port, HC165_CLK_Pin, GPIO_PIN_RESET);
	}
}

void detectSwitches(void)
{
	static int i = 0;
	static int index = 0;

	switch (i)
	{
	case 0:// 0
		HAL_GPIO_WritePin(HC164_A_GPIO_Port, HC164_A_Pin, GPIO_PIN_RESET);
		test0();

		i++;
		break;
	case 1:
		test(index, 0, 4);

		i++;
		break;
	case 2:
		test(index, 4, 8);

		i++;
		break;
	case 3:
		test(index, 8, 12);

		i++;
		break;
	case 4:
		test(index, 12, 16);
		index++;

		i++;
		break;
	case 5:// 1
		test0();

		i++;
		break;
	case 6:
		test(index, 0, 4);

		i++;
		break;
	case 7:
		test(index, 4, 8);

		i++;
		break;
	case 8:
		test(index, 8, 12);

		i++;
		break;
	case 9:
		test(index, 12, 16);
		index++;

		i++;
		break;
	case 10:// 2
		test0();

		i++;
		break;
	case 11:
		test(index, 0, 4);

		i++;
		break;
	case 12:
		test(index, 4, 8);

		i++;
		break;
	case 13:
		test(index, 8, 12);

		i++;
		break;
	case 14:
		test(index, 12, 16);
		index++;

		i++;
		break;
	case 15:// 3
		test0();

		i++;
		break;
	case 16:
		test(index, 0, 4);

		i++;
		break;
	case 17:
		test(index, 4, 8);

		i++;
		break;
	case 18:
		test(index, 8, 12);

		i++;
		break;
	case 19:
		test(index, 12, 16);
		index++;

		i++;
		break;
	case 20:// 4
		test0();

		i++;
		break;
	case 21:
		test(index, 0, 4);

		i++;
		break;
	case 22:
		test(index, 4, 8);

		i++;
		break;
	case 23:
		test(index, 8, 12);

		i++;
		break;
	case 24:
		test(index, 12, 16);
		index = 0;

		i++;
		break;
	case 25:
		for (int i = 0; i < MATRIX_ROWS; i++)
		{
			if (keyState[i] != 0x0 || (keyState[i] == 0x0 && keyState[i] != prevKeyState[i]))
			{
				if (!tud_hid_ready())
					return;

				tud_hid_keyboard_report(REPORT_ID_KEYBOARD, keyboardHID.modifiers, keyboardHID.key);
				break;
			}
		}

		for (int i = 0; i < MATRIX_ROWS; i++)
		{
			prevKeyState[i] = keyState[i];
		}

		i = 0;
		break;
	default:
		i = 0;
		break;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim6)
	{
		//SEGGER_RTT_printf(0, "pot = %d, %d\n", pot_value[0] >> 2, pot_value[1] >> 2);
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
			SEGGER_RTT_printf(0, "master gain = %d\n", master_gain);
			send_master_gain(master_gain);
			master_gain_prev = master_gain;
		}
	}
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
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SAI1_Init();
  MX_QUADSPI1_Init();
  MX_I2C2_Init();
  MX_ADC1_Init();
  MX_USB_PCD_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  default_download_IC_1();

  tusb_init();

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

  HAL_TIM_Base_Start_IT(&htim6);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  tud_task();

	  audio_task();

	  detectSwitches();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 24;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV6;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

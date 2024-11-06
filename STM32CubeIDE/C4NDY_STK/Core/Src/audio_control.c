/*
 * audio_control.c
 *
 *  Created on: Aug 17, 2023
 *      Author: shun
 */

#include <math.h>

#include "adc.h"
#include "sai.h"

#include "audio_control.h"

#include "ADAU1761_IC_1_PARAM.h"
#include "ADAU1761_IC_1_REG.h"
#include "ADAU1761_IC_1.h"
#include "SigmaStudioFW.h"

#include <icled.h>
#include <keyboard.h>

// List of supported sample rates
const uint32_t sample_rates[] = {48000};
uint32_t current_sample_rate  = 48000;

enum
{
    VOLUME_CTRL_0_DB    = 0,
    VOLUME_CTRL_10_DB   = 2560,
    VOLUME_CTRL_20_DB   = 5120,
    VOLUME_CTRL_30_DB   = 7680,
    VOLUME_CTRL_40_DB   = 10240,
    VOLUME_CTRL_50_DB   = 12800,
    VOLUME_CTRL_60_DB   = 15360,
    VOLUME_CTRL_70_DB   = 17920,
    VOLUME_CTRL_80_DB   = 20480,
    VOLUME_CTRL_90_DB   = 23040,
    VOLUME_CTRL_100_DB  = 25600,
    VOLUME_CTRL_SILENCE = 0x8000,
};

// Audio controls
// Current states
int8_t mute[CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX + 1];     // +1 for master channel 0
int16_t volume[CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX + 1];  // +1 for master channel 0

// Resolution per format
const uint8_t resolutions_per_format[1] = {CFG_TUD_AUDIO_FUNC_1_RESOLUTION_RX};
// Current resolution, update on format change
uint8_t current_resolution = 24;

#define N_SAMPLE_RATES TU_ARRAY_SIZE(sample_rates)

uint8_t xfade_buffer_index      = 0;
uint8_t buffer_index            = 0;
uint16_t xfade_buffer[16]       = {0};
uint16_t xfade                  = 0;
uint16_t xfade_prev             = 255;
uint16_t master_gain_buffer[16] = {0};
uint16_t master_gain            = 0;
uint16_t master_gain_prev       = 255;

uint_fast64_t sai_buf_index            = 0;
uint_fast64_t sai_transmit_index       = 0;
int_fast32_t sai_buf[SAI_RNG_BUF_SIZE] = {0};

// Speaker data size received in the last frame
uint_fast16_t spk_data_size = 0;

// Buffer for speaker data
int_fast32_t spk_buf[CFG_TUD_AUDIO_FUNC_1_EP_OUT_SW_BUF_SZ] = {0};
int_fast32_t hpout_buf[SAI_BUF_SIZE]                        = {0};

volatile int16_t update_pointer    = -1;
volatile int16_t hpout_clear_count = 0;

// Helper for clock get requests
static bool tud_audio_clock_get_request(uint8_t rhport, audio_control_request_t const* request)
{
    TU_ASSERT(request->bEntityID == UAC2_ENTITY_CLOCK);

    if (request->bControlSelector == AUDIO_CS_CTRL_SAM_FREQ)
    {
        if (request->bRequest == AUDIO_CS_REQ_CUR)
        {
            SEGGER_RTT_printf(0, "Clock get current freq %lu\r\n", current_sample_rate);

            audio_control_cur_4_t curf = {(int32_t) tu_htole32(current_sample_rate)};
            return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const*) request, &curf, sizeof(curf));
        }
        else if (request->bRequest == AUDIO_CS_REQ_RANGE)
        {
            audio_control_range_4_n_t(N_SAMPLE_RATES) rangef =
                {
                    .wNumSubRanges = tu_htole16(N_SAMPLE_RATES)};
            SEGGER_RTT_printf(0, "Clock get %d freq ranges\r\n", N_SAMPLE_RATES);
            for (uint8_t i = 0; i < N_SAMPLE_RATES; i++)
            {
                rangef.subrange[i].bMin = (int32_t) sample_rates[i];
                rangef.subrange[i].bMax = (int32_t) sample_rates[i];
                rangef.subrange[i].bRes = 0;
                SEGGER_RTT_printf(0, "Range %d (%d, %d, %d)\r\n", i, (int) rangef.subrange[i].bMin, (int) rangef.subrange[i].bMax, (int) rangef.subrange[i].bRes);
            }

            return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const*) request, &rangef, sizeof(rangef));
        }
    }
    else if (request->bControlSelector == AUDIO_CS_CTRL_CLK_VALID && request->bRequest == AUDIO_CS_REQ_CUR)
    {
        audio_control_cur_1_t cur_valid = {.bCur = 1};
        SEGGER_RTT_printf(0, "Clock get is valid %u\r\n", cur_valid.bCur);
        return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const*) request, &cur_valid, sizeof(cur_valid));
    }
    SEGGER_RTT_printf(0, "Clock get request not supported, entity = %u, selector = %u, request = %u\r\n", request->bEntityID, request->bControlSelector, request->bRequest);
    return false;
}

// Helper for clock set requests
static bool tud_audio_clock_set_request(uint8_t rhport, audio_control_request_t const* request, uint8_t const* buf)
{
    (void) rhport;

    TU_ASSERT(request->bEntityID == UAC2_ENTITY_CLOCK);
    TU_VERIFY(request->bRequest == AUDIO_CS_REQ_CUR);

    if (request->bControlSelector == AUDIO_CS_CTRL_SAM_FREQ)
    {
        TU_VERIFY(request->wLength == sizeof(audio_control_cur_4_t));

        current_sample_rate = (uint32_t) ((audio_control_cur_4_t const*) buf)->bCur;

        SEGGER_RTT_printf(0, "Clock set current freq: %ld\r\n", current_sample_rate);

        return true;
    }
    else
    {
        SEGGER_RTT_printf(0, "Clock set request not supported, entity = %u, selector = %u, request = %u\r\n", request->bEntityID, request->bControlSelector, request->bRequest);
        return false;
    }
}

// Helper for feature unit get requests
// PC側で音量調整をするとここも呼ばれる
static bool tud_audio_feature_unit_get_request(uint8_t rhport, audio_control_request_t const* request)
{
    TU_ASSERT(request->bEntityID == UAC2_ENTITY_FEATURE_UNIT);

    if (request->bControlSelector == AUDIO_FU_CTRL_MUTE && request->bRequest == AUDIO_CS_REQ_CUR)
    {
        audio_control_cur_1_t mute1 = {.bCur = mute[request->bChannelNumber]};
        SEGGER_RTT_printf(0, "Get channel %u mute %d\r\n", request->bChannelNumber, mute1.bCur);
        return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const*) request, &mute1, sizeof(mute1));
    }
    else if (UAC2_ENTITY_FEATURE_UNIT && request->bControlSelector == AUDIO_FU_CTRL_VOLUME)
    {
        if (request->bRequest == AUDIO_CS_REQ_RANGE)
        {
            audio_control_range_2_n_t(1) range_vol = {
                .wNumSubRanges = tu_htole16(1),
                .subrange[0]   = {.bMin = tu_htole16(-VOLUME_CTRL_50_DB), tu_htole16(VOLUME_CTRL_0_DB), tu_htole16(256)}
            };
            SEGGER_RTT_printf(0, "Get channel %u volume range (%d, %d, %u) dB\r\n", request->bChannelNumber, range_vol.subrange[0].bMin / 256, range_vol.subrange[0].bMax / 256, range_vol.subrange[0].bRes / 256);
            return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const*) request, &range_vol, sizeof(range_vol));
        }
        else if (request->bRequest == AUDIO_CS_REQ_CUR)
        {
            audio_control_cur_2_t cur_vol = {.bCur = tu_htole16(volume[request->bChannelNumber])};
            SEGGER_RTT_printf(0, "Get channel %u volume %d dB\r\n", request->bChannelNumber, cur_vol.bCur / 256);
            return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const*) request, &cur_vol, sizeof(cur_vol));
        }
    }
    SEGGER_RTT_printf(0, "Feature unit get request not supported, entity = %u, selector = %u, request = %u\r\n", request->bEntityID, request->bControlSelector, request->bRequest);

    return false;
}

// Helper for feature unit set requests
// PC側で音量調整をするとこの関数が呼ばれる
static bool tud_audio_feature_unit_set_request(uint8_t rhport, audio_control_request_t const* request, uint8_t const* buf)
{
    (void) rhport;

    TU_ASSERT(request->bEntityID == UAC2_ENTITY_FEATURE_UNIT);
    TU_VERIFY(request->bRequest == AUDIO_CS_REQ_CUR);

    if (request->bControlSelector == AUDIO_FU_CTRL_MUTE)
    {
        TU_VERIFY(request->wLength == sizeof(audio_control_cur_1_t));

        mute[request->bChannelNumber] = ((audio_control_cur_1_t const*) buf)->bCur;

        SEGGER_RTT_printf(0, "Set channel %d Mute: %d\r\n", request->bChannelNumber, mute[request->bChannelNumber]);

        return true;
    }
    else if (request->bControlSelector == AUDIO_FU_CTRL_VOLUME)
    {
        TU_VERIFY(request->wLength == sizeof(audio_control_cur_2_t));

        volume[request->bChannelNumber] = ((audio_control_cur_2_t const*) buf)->bCur;

        switch (request->bChannelNumber)
        {
        case 1:
            send_usb_gain_L(volume[request->bChannelNumber] / 256);
            break;
        case 2:
            send_usb_gain_R(volume[request->bChannelNumber] / 256);
            break;
        default:
            break;
        }

        SEGGER_RTT_printf(0, "Set channel %d volume: %d dB\r\n", request->bChannelNumber, volume[request->bChannelNumber] / 256);

        return true;
    }
    else
    {
        SEGGER_RTT_printf(0, "Feature unit set request not supported, entity = %u, selector = %u, request = %u\r\n", request->bEntityID, request->bControlSelector, request->bRequest);
        return false;
    }
}

// Invoked when audio class specific get request received for an entity
bool tud_audio_get_req_entity_cb(uint8_t rhport, tusb_control_request_t const* p_request)
{
    audio_control_request_t const* request = (audio_control_request_t const*) p_request;

    if (request->bEntityID == UAC2_ENTITY_CLOCK)
        return tud_audio_clock_get_request(rhport, request);
    if (request->bEntityID == UAC2_ENTITY_FEATURE_UNIT)
        return tud_audio_feature_unit_get_request(rhport, request);
    else
    {
        SEGGER_RTT_printf(0, "Get request not handled, entity = %d, selector = %d, request = %d\r\n", request->bEntityID, request->bControlSelector, request->bRequest);
    }
    return false;
}

// Invoked when audio class specific set request received for an entity
bool tud_audio_set_req_entity_cb(uint8_t rhport, tusb_control_request_t const* p_request, uint8_t* buf)
{
    audio_control_request_t const* request = (audio_control_request_t const*) p_request;

    if (request->bEntityID == UAC2_ENTITY_FEATURE_UNIT)
        return tud_audio_feature_unit_set_request(rhport, request, buf);
    if (request->bEntityID == UAC2_ENTITY_CLOCK)
        return tud_audio_clock_set_request(rhport, request, buf);
    SEGGER_RTT_printf(0, "Set request not handled, entity = %d, selector = %d, request = %d\r\n", request->bEntityID, request->bControlSelector, request->bRequest);

    return false;
}

bool tud_audio_set_itf_close_EP_cb(uint8_t rhport, tusb_control_request_t const* p_request)
{
    (void) rhport;

    // uint8_t const itf = tu_u16_low(tu_le16toh(p_request->wIndex));
    // uint8_t const alt = tu_u16_low(tu_le16toh(p_request->wValue));

    return true;
}

bool tud_audio_set_itf_cb(uint8_t rhport, tusb_control_request_t const* p_request)
{
    (void) rhport;

    // uint8_t const itf = tu_u16_low(tu_le16toh(p_request->wIndex));
    // uint8_t const alt = tu_u16_low(tu_le16toh(p_request->wValue));

    return true;
}

#if 0
bool tud_audio_rx_done_pre_read_cb(uint8_t rhport, uint16_t n_bytes_received, uint8_t func_id, uint8_t ep_out, uint8_t cur_alt_setting)
{
    (void) rhport;
    (void) func_id;
    (void) ep_out;
    (void) cur_alt_setting;

    read_audio_data_from_usb(n_bytes_received);

    return true;
}
#endif

void tud_audio_feedback_params_cb(uint8_t func_id, uint8_t alt_itf, audio_feedback_params_t* feedback_param)
{
    (void) func_id;
    (void) alt_itf;

    feedback_param->method      = AUDIO_FEEDBACK_METHOD_FIFO_COUNT;
    feedback_param->sample_freq = current_sample_rate;

    SEGGER_RTT_printf(0, "feedback\n");
}

void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef* hsai)
{
    // SEGGER_RTT_printf(0, "tx cplt\n");
    update_pointer = SAI_BUF_SIZE / 2;
}

void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef* hsai)
{
    // SEGGER_RTT_printf(0, "tx half cplt\n");
    update_pointer = 0;
}

void start_adc(void)
{
    /* Run the ADC calibration in single-ended mode */
    if (HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED) != HAL_OK)
    {
        /* Calibration Error */
        Error_Handler();
    }

    if (HAL_ADC_Start_DMA(&hadc2, (uint32_t*) pot_value, 5) != HAL_OK)
    {
        /* ADC conversion start error */
        Error_Handler();
    }
}

void start_sai(void)
{
    if (HAL_SAI_Transmit_DMA(&hsai_BlockB1, (uint8_t*) hpout_buf, SAI_BUF_SIZE) != HAL_OK)
    {
        /* SAI transmit start error */
        Error_Handler();
    }
}

void read_audio_data_from_usb(void)
{
    const uint16_t rcv_buf_size = N_SAMPLES_PER_1_RX * CFG_TUD_AUDIO_FUNC_1_N_BYTES_PER_SAMPLE_RX * CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX;
    spk_data_size = tud_audio_read(spk_buf, rcv_buf_size);
    // SEGGER_RTT_printf(0, "size = %d, %d %d\n", spk_data_size, n_bytes_received, CFG_TUD_AUDIO_FUNC_1_EP_OUT_SZ_MAX);

    if (spk_data_size == 0 && hpout_clear_count < 100)
    {
        hpout_clear_count++;

        if (hpout_clear_count == 100)
        {
            memset(hpout_buf, 0, sizeof(hpout_buf));
            hpout_clear_count = 101;
        }
    }
    else
    {
        hpout_clear_count = 0;
    }

    copybuf_usb2sai();
    copybuf_sai2codec();
}

void copybuf_usb2sai(void)
{
    // SEGGER_RTT_printf(0, "sb_index = %d -> ", sai_buf_index);

    const uint_fast16_t array_size = spk_data_size >> 2;
    for (uint_fast16_t i = 0; i < array_size; i++)
    {
        if (sai_buf_index + array_size != sai_transmit_index)
        {
            const int_fast32_t val = spk_buf[i];

            sai_buf[sai_buf_index & (SAI_RNG_BUF_SIZE - 1)] = val << 16 | val >> 16;
            sai_buf_index++;
        }
    }
    // SEGGER_RTT_printf(0, " %d\n", sai_buf_index);
}

void copybuf_sai2codec(void)
{
    if (sai_buf_index - sai_transmit_index >= SAI_BUF_SIZE / 2)
    {
        while (update_pointer == -1)
        {
        }

        const int16_t index0 = update_pointer;
        update_pointer       = -1;

        // SEGGER_RTT_printf(0, "st_index = %d -> ", sai_transmit_index);

        const uint_fast64_t index1 = sai_transmit_index & (SAI_RNG_BUF_SIZE - 1);
        memcpy(hpout_buf + index0, sai_buf + index1, sizeof(hpout_buf) / 2);
        sai_transmit_index += SAI_BUF_SIZE / 2;

        // SEGGER_RTT_printf(0, " %d\n", sai_transmit_index);

        if (update_pointer != -1)
        {
            SEGGER_RTT_printf(0, "buffer update too long...\n");
        }
    }
}

void send_usb_gain_L(const int16_t usb_db)
{
    const double usb_rate = pow(10.0, (double) usb_db / 20.0);

    uint8_t usb_gain_array[8] = {0x00};
    usb_gain_array[0]         = ((uint32_t) (usb_rate * pow(2.0, 23.0)) >> 24) & 0x000000FF;
    usb_gain_array[1]         = ((uint32_t) (usb_rate * pow(2.0, 23.0)) >> 16) & 0x000000FF;
    usb_gain_array[2]         = ((uint32_t) (usb_rate * pow(2.0, 23.0)) >> 8) & 0x000000FF;
    usb_gain_array[3]         = (uint32_t) (usb_rate * pow(2.0, 23.0)) & 0x000000FF;
    usb_gain_array[4]         = 0x00;  // if_step
    usb_gain_array[5]         = 0x00;
    usb_gain_array[6]         = 0x80;
    usb_gain_array[7]         = 0x00;

    SIGMA_SAFELOAD_WRITE_DATA(DEVICE_ADDR_IC_1, SIGMA_SAFELOAD_DATA_1, 8, usb_gain_array);

    uint8_t target_address_count[8] = {0x00, 0x00, 0x00, MOD_USBGAINL_ALG0_TARGET_ADDR - 1, 0x00, 0x00, 0x00, MOD_USBGAINL_COUNT};
    SIGMA_SAFELOAD_WRITE_DATA(DEVICE_ADDR_IC_1, SIGMA_SAFELOAD_TARGET_ADDRESS, 8, target_address_count);
}

void send_usb_gain_R(const int16_t usb_db)
{
    const double usb_rate = pow(10.0, (double) usb_db / 20.0);

    uint8_t usb_gain_array[8] = {0x00};
    usb_gain_array[0]         = ((uint32_t) (usb_rate * pow(2.0, 23.0)) >> 24) & 0x000000FF;
    usb_gain_array[1]         = ((uint32_t) (usb_rate * pow(2.0, 23.0)) >> 16) & 0x000000FF;
    usb_gain_array[2]         = ((uint32_t) (usb_rate * pow(2.0, 23.0)) >> 8) & 0x000000FF;
    usb_gain_array[3]         = (uint32_t) (usb_rate * pow(2.0, 23.0)) & 0x000000FF;
    usb_gain_array[4]         = 0x00;  // if_step
    usb_gain_array[5]         = 0x00;
    usb_gain_array[6]         = 0x80;
    usb_gain_array[7]         = 0x00;

    SIGMA_SAFELOAD_WRITE_DATA(DEVICE_ADDR_IC_1, SIGMA_SAFELOAD_DATA_1, 8, usb_gain_array);

    uint8_t target_address_count[8] = {0x00, 0x00, 0x00, MOD_USBGAINR_ALG0_TARGET_ADDR - 1, 0x00, 0x00, 0x00, MOD_USBGAINR_COUNT};
    SIGMA_SAFELOAD_WRITE_DATA(DEVICE_ADDR_IC_1, SIGMA_SAFELOAD_TARGET_ADDRESS, 8, target_address_count);
}

void send_xfade(const uint16_t fader_val)
{
    const double xf_rate = (double) fader_val / 1023.0;

    uint8_t dc1_array[4] = {0x00};
    dc1_array[0]         = ((uint32_t) ((1.0 - xf_rate) * pow(2, 23)) >> 24) & 0x000000FF;
    dc1_array[1]         = ((uint32_t) ((1.0 - xf_rate) * pow(2, 23)) >> 16) & 0x000000FF;
    dc1_array[2]         = ((uint32_t) ((1.0 - xf_rate) * pow(2, 23)) >> 8) & 0x000000FF;
    dc1_array[3]         = (uint32_t) ((1.0 - xf_rate) * pow(2, 23)) & 0x000000FF;

    SIGMA_WRITE_REGISTER_BLOCK(DEVICE_ADDR_IC_1, MOD_DC1_DCINPALG1_ADDR, 4, dc1_array);

    uint8_t dc2_array[4] = {0x00};
    dc2_array[0]         = ((uint32_t) (xf_rate * pow(2, 23)) >> 24) & 0x000000FF;
    dc2_array[1]         = ((uint32_t) (xf_rate * pow(2, 23)) >> 16) & 0x000000FF;
    dc2_array[2]         = ((uint32_t) (xf_rate * pow(2, 23)) >> 8) & 0x000000FF;
    dc2_array[3]         = (uint32_t) (xf_rate * pow(2, 23)) & 0x000000FF;

    SIGMA_WRITE_REGISTER_BLOCK(DEVICE_ADDR_IC_1, MOD_DC2_DCINPALG2_ADDR, 4, dc2_array);
}

void send_master_gain(const uint16_t master_val)
{
    const double c_curve_val = 1038.0 * tanh((double) master_val / 448.0);
    const double master_db   = (135.0 / 1023.0) * c_curve_val - 120.0;

    const double master_rate = pow(10.0, master_db / 20.0);

    uint8_t master_gain_array[8] = {0x00};
    master_gain_array[0]         = ((uint32_t) (master_rate * pow(2, 23)) >> 24) & 0x000000FF;
    master_gain_array[1]         = ((uint32_t) (master_rate * pow(2, 23)) >> 16) & 0x000000FF;
    master_gain_array[2]         = ((uint32_t) (master_rate * pow(2, 23)) >> 8) & 0x000000FF;
    master_gain_array[3]         = (uint32_t) (master_rate * pow(2, 23)) & 0x000000FF;
    master_gain_array[4]         = 0x00;  // if_step
    master_gain_array[5]         = 0x00;
    master_gain_array[6]         = 0x80;
    master_gain_array[7]         = 0x00;
#if 1
    SEGGER_RTT_printf(0, "%d -> %02X,%02X,%02X,%02X\n", master_val, master_gain_array[0], master_gain_array[1], master_gain_array[2], master_gain_array[3]);
#endif
    SIGMA_SAFELOAD_WRITE_DATA(DEVICE_ADDR_IC_1, SIGMA_SAFELOAD_DATA_1, 8, master_gain_array);

    uint8_t target_address_count[8] = {0x00, 0x00, 0x00, MOD_MASTERGAIN_ALG0_TARGET_ADDR - 1, 0x00, 0x00, 0x00, MOD_MASTERGAIN_COUNT};
    SIGMA_SAFELOAD_WRITE_DATA(DEVICE_ADDR_IC_1, SIGMA_SAFELOAD_TARGET_ADDRESS, 8, target_address_count);
}

void send_master_gain_db(const int master_db)
{
    const double master_rate = pow(10.0, (double) master_db / 20.0);

    uint8_t master_gain_array[8] = {0x00};
    master_gain_array[0]         = ((uint32_t) (master_rate * pow(2, 23)) >> 24) & 0x000000FF;
    master_gain_array[1]         = ((uint32_t) (master_rate * pow(2, 23)) >> 16) & 0x000000FF;
    master_gain_array[2]         = ((uint32_t) (master_rate * pow(2, 23)) >> 8) & 0x000000FF;
    master_gain_array[3]         = (uint32_t) (master_rate * pow(2, 23)) & 0x000000FF;
    master_gain_array[4]         = 0x00;  // if_step
    master_gain_array[5]         = 0x00;
    master_gain_array[6]         = 0x80;
    master_gain_array[7]         = 0x00;
#if 1
    SEGGER_RTT_printf(0, "%d -> %02X,%02X,%02X,%02X\n", master_db, master_gain_array[0], master_gain_array[1], master_gain_array[2], master_gain_array[3]);
#endif
    SIGMA_SAFELOAD_WRITE_DATA(DEVICE_ADDR_IC_1, SIGMA_SAFELOAD_DATA_1, 8, master_gain_array);

    uint8_t target_address_count[8] = {0x00, 0x00, 0x00, MOD_MASTERGAIN_ALG0_TARGET_ADDR - 1, 0x00, 0x00, 0x00, MOD_MASTERGAIN_COUNT};
    SIGMA_SAFELOAD_WRITE_DATA(DEVICE_ADDR_IC_1, SIGMA_SAFELOAD_TARGET_ADDRESS, 8, target_address_count);
}

void send_switch_to_linein(void)
{
    uint8_t switch0_array[4] = {0x00, 0x80, 0x00, 0x00};
    SIGMA_SAFELOAD_WRITE_DATA(DEVICE_ADDR_IC_1, SIGMA_SAFELOAD_DATA_1, 4, switch0_array);

    uint8_t target_address0_count[8] = {0x00, 0x00, 0x00, MOD_LNPHSW_ALG0_STEREODEMUXSLEW10_ADDR - 1, 0x00, 0x00, 0x00, 0x01};
    SIGMA_SAFELOAD_WRITE_DATA(DEVICE_ADDR_IC_1, SIGMA_SAFELOAD_TARGET_ADDRESS, 8, target_address0_count);

    uint8_t switch1_array[4] = {0x00, 0x00, 0x00, 0x00};
    SIGMA_SAFELOAD_WRITE_DATA(DEVICE_ADDR_IC_1, SIGMA_SAFELOAD_DATA_1, 4, switch1_array);

    uint8_t target_address1_count[8] = {0x00, 0x00, 0x00, MOD_LNPHSW_ALG0_STEREODEMUXSLEW11_ADDR - 1, 0x00, 0x00, 0x00, 0x01};
    SIGMA_SAFELOAD_WRITE_DATA(DEVICE_ADDR_IC_1, SIGMA_SAFELOAD_TARGET_ADDRESS, 8, target_address1_count);
}

void send_switch_to_phonoin(void)
{
    uint8_t switch0_array[4] = {0x00, 0x00, 0x00, 0x00};
    SIGMA_SAFELOAD_WRITE_DATA(DEVICE_ADDR_IC_1, SIGMA_SAFELOAD_DATA_1, 4, switch0_array);

    uint8_t target_address0_count[8] = {0x00, 0x00, 0x00, MOD_LNPHSW_ALG0_STEREODEMUXSLEW10_ADDR - 1, 0x00, 0x00, 0x00, 0x01};
    SIGMA_SAFELOAD_WRITE_DATA(DEVICE_ADDR_IC_1, SIGMA_SAFELOAD_TARGET_ADDRESS, 8, target_address0_count);

    uint8_t switch1_array[4] = {0x00, 0x80, 0x00, 0x00};
    SIGMA_SAFELOAD_WRITE_DATA(DEVICE_ADDR_IC_1, SIGMA_SAFELOAD_DATA_1, 4, switch1_array);

    uint8_t target_address1_count[8] = {0x00, 0x00, 0x00, MOD_LNPHSW_ALG0_STEREODEMUXSLEW11_ADDR - 1, 0x00, 0x00, 0x00, 0x01};
    SIGMA_SAFELOAD_WRITE_DATA(DEVICE_ADDR_IC_1, SIGMA_SAFELOAD_TARGET_ADDRESS, 8, target_address1_count);
}

void codec_control_task(void)
{
    // SEGGER_RTT_printf(0, "pot_val = [%d, %d, %d, %d, %d]\r\n", pot_value[1], pot_value[0], pot_value[2], pot_value[3], pot_value[4]);

    if (isXFadeCutPressed())
    {
        xfade      = 65535;
        xfade_prev = 0;
    }
    else
    {
        xfade_buffer[xfade_buffer_index] = pot_value[0] >> 2;
        xfade_buffer_index               = (xfade_buffer_index + 1) & (16 - 1);
        xfade                            = 0;
        for (int i = 0; i < 16; i++)
        {
            xfade += xfade_buffer[i];
        }
        xfade >>= 4;

        if (abs(xfade - xfade_prev) > 2)
        {
            send_xfade(xfade);
            setMixMark(xfade);
            xfade_prev = xfade;
        }
    }
}

/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef USB_DESCRIPTORS_H_
#define USB_DESCRIPTORS_H_

enum
{
    REPORT_ID_KEYBOARD = 1,
    REPORT_ID_MOUSE,
    REPORT_ID_CONSUMER_CONTROL,
    REPORT_ID_COUNT
};

// Unit numbers are arbitrary selected
#define UAC2_ENTITY_CLOCK           0x04
#define UAC2_ENTITY_INPUT_TERMINAL  0x01
#define UAC2_ENTITY_FEATURE_UNIT    0x02
#define UAC2_ENTITY_OUTPUT_TERMINAL 0x03

enum
{
    ITF_NUM_HID_KEYBOARD = 0,
    ITF_NUM_HID_MOUSE,
    ITF_NUM_HID_GIO,
    ITF_NUM_AUDIO_CONTROL,
    ITF_NUM_AUDIO_STREAMING_SPK,
    ITF_NUM_TOTAL
};

// clang-format off
#define TUD_AUDIO_HEADPHONE_STEREO_DESC_LEN (TUD_AUDIO_DESC_IAD_LEN\
    + TUD_AUDIO_DESC_STD_AC_LEN\
    + TUD_AUDIO_DESC_CS_AC_LEN\
    + TUD_AUDIO_DESC_CLK_SRC_LEN\
    + TUD_AUDIO_DESC_INPUT_TERM_LEN\
    + TUD_AUDIO_DESC_OUTPUT_TERM_LEN\
    + TUD_AUDIO_DESC_FEATURE_UNIT_TWO_CHANNEL_LEN\
    + TUD_AUDIO_DESC_STD_AS_INT_LEN\
    + TUD_AUDIO_DESC_STD_AS_INT_LEN\
    + TUD_AUDIO_DESC_CS_AS_INT_LEN\
    + TUD_AUDIO_DESC_TYPE_I_FORMAT_LEN\
    + TUD_AUDIO_DESC_STD_AS_ISO_EP_LEN\
    + TUD_AUDIO_DESC_CS_AS_ISO_EP_LEN\
    + TUD_AUDIO_DESC_STD_AS_ISO_FB_EP_LEN)

#define TUD_AUDIO_HEADPHONE_STEREO_DESCRIPTOR(_stridx, _epout, _epfb) \
    /* Standard Interface Association Descriptor (IAD) */\
    TUD_AUDIO_DESC_IAD(/*_firstitf*/ ITF_NUM_AUDIO_CONTROL,\
                          /*_nitfs*/ 0x02,\
                         /*_stridx*/ 0x00),\
    /* Standard AC Interface Descriptor(4.7.1) */\
    TUD_AUDIO_DESC_STD_AC(/*_itfnum*/ ITF_NUM_AUDIO_CONTROL,\
                            /*_nEPs*/ 0x00,\
                          /*_stridx*/ _stridx),\
    /* Class-Specific AC Interface Header Descriptor(4.7.2) */\
    TUD_AUDIO_DESC_CS_AC(/*_bcdADC*/ 0x0200,\
                       /*_category*/ AUDIO_FUNC_DESKTOP_SPEAKER,\
                       /*_totallen*/ TUD_AUDIO_DESC_CLK_SRC_LEN+TUD_AUDIO_DESC_FEATURE_UNIT_TWO_CHANNEL_LEN+TUD_AUDIO_DESC_INPUT_TERM_LEN+TUD_AUDIO_DESC_OUTPUT_TERM_LEN,\
                           /*_ctrl*/ AUDIO_CS_AS_INTERFACE_CTRL_LATENCY_POS),\
    /* Clock Source Descriptor(4.7.2.1) */\
    TUD_AUDIO_DESC_CLK_SRC(/*_clkid*/ UAC2_ENTITY_CLOCK,\
                            /*_attr*/ AUDIO_CLOCK_SOURCE_ATT_INT_PRO_CLK,\
                            /*_ctrl*/ (AUDIO_CTRL_RW << AUDIO_CLOCK_SOURCE_CTRL_CLK_FRQ_POS),\
                       /*_assocTerm*/ 0x01,\
                          /*_stridx*/ 0x00),\
    /* Input Terminal Descriptor(4.7.2.4) */\
    TUD_AUDIO_DESC_INPUT_TERM(/*_termid*/ UAC2_ENTITY_INPUT_TERMINAL,\
                            /*_termtype*/ AUDIO_TERM_TYPE_USB_STREAMING,\
                           /*_assocTerm*/ 0x00,\
                               /*_clkid*/ UAC2_ENTITY_CLOCK,\
                    /*_nchannelslogical*/ 0x02,\
                          /*_channelcfg*/ AUDIO_CHANNEL_CONFIG_NON_PREDEFINED,\
                     /*_idxchannelnames*/ 0x00,\
                                /*_ctrl*/ 0 * (AUDIO_CTRL_R << AUDIO_IN_TERM_CTRL_CONNECTOR_POS),\
                              /*_stridx*/ 0x00),\
    /* Output Terminal Descriptor(4.7.2.5) */\
    TUD_AUDIO_DESC_OUTPUT_TERM(/*_termid*/ UAC2_ENTITY_OUTPUT_TERMINAL,\
                             /*_termtype*/ AUDIO_TERM_TYPE_OUT_HEADPHONES,\
                            /*_assocTerm*/ 0x00,\
                                /*_srcid*/ UAC2_ENTITY_FEATURE_UNIT,\
                                /*_clkid*/ UAC2_ENTITY_CLOCK,\
                                 /*_ctrl*/ 0x0000,\
                               /*_stridx*/ 0x00),\
    /* Feature Unit Descriptor(4.7.2.8) */\
    TUD_AUDIO_DESC_FEATURE_UNIT_TWO_CHANNEL(/*_unitid*/ UAC2_ENTITY_FEATURE_UNIT,\
                                             /*_srcid*/ UAC2_ENTITY_INPUT_TERMINAL,\
                                     /*_ctrlch0master*/ (AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_MUTE_POS | AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_VOLUME_POS),\
                                           /*_ctrlch1*/ (AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_MUTE_POS | AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_VOLUME_POS),\
                                           /*_ctrlch2*/ (AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_MUTE_POS | AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_VOLUME_POS),\
                                            /*_stridx*/ 0x00),\
    /* Standard AS Interface Descriptor(4.9.1) */\
    /* Interface 1, Alternate 0 - default alternate setting with 0 bandwidth */\
    TUD_AUDIO_DESC_STD_AS_INT(/*_itfnum*/ (uint8_t)(ITF_NUM_AUDIO_STREAMING_SPK),\
                              /*_altset*/ 0x00,\
                                /*_nEPs*/ 0x00,\
                              /*_stridx*/ 0x00),\
    /* Standard AS Interface Descriptor(4.9.1) */\
    /* Interface 1, Alternate 1 - alternate interface for data streaming */\
    TUD_AUDIO_DESC_STD_AS_INT(/*_itfnum*/ (uint8_t)(ITF_NUM_AUDIO_STREAMING_SPK),\
                              /*_altset*/ 0x01,\
                                /*_nEPs*/ 0x02,\
                              /*_stridx*/ 0x00),\
    /* Class-Specific AS Interface Descriptor(4.9.2) */\
    TUD_AUDIO_DESC_CS_AS_INT(/*_termid*/ UAC2_ENTITY_INPUT_TERMINAL,\
                               /*_ctrl*/ AUDIO_CTRL_NONE,\
                         /*_formattype*/ AUDIO_FORMAT_TYPE_I,\
                            /*_formats*/ AUDIO_DATA_FORMAT_TYPE_I_PCM,\
                  /*_nchannelsphysical*/ CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX,\
                         /*_channelcfg*/ AUDIO_CHANNEL_CONFIG_NON_PREDEFINED,\
                             /*_stridx*/ 0x00),\
    /* Type I Format Type Descriptor(2.3.1.6 - Audio Formats) */\
    TUD_AUDIO_DESC_TYPE_I_FORMAT(CFG_TUD_AUDIO_FUNC_1_N_BYTES_PER_SAMPLE_RX,\
                                 CFG_TUD_AUDIO_FUNC_1_RESOLUTION_RX),\
    /* Standard AS Isochronous Audio Data Endpoint Descriptor(4.10.1.1) */\
    TUD_AUDIO_DESC_STD_AS_ISO_EP(/*_ep*/ _epout,\
                               /*_attr*/ (uint8_t) ((uint8_t) TUSB_XFER_ISOCHRONOUS | (uint8_t) TUSB_ISO_EP_ATT_ASYNCHRONOUS | (uint8_t) TUSB_ISO_EP_ATT_DATA),\
                          /*_maxEPsize*/ CFG_TUD_AUDIO_FUNC_1_EP_OUT_SZ_MAX,\
                           /*_interval*/ 0x01),\
    /* Class-Specific AS Isochronous Audio Data Endpoint Descriptor(4.10.1.2) */\
    TUD_AUDIO_DESC_CS_AS_ISO_EP(/*_attr*/ AUDIO_CS_AS_ISO_DATA_EP_ATT_NON_MAX_PACKETS_OK,\
                                /*_ctrl*/ 0x00,\
                       /*_lockdelayunit*/ AUDIO_CS_AS_ISO_DATA_EP_LOCK_DELAY_UNIT_UNDEFINED,\
                           /*_lockdelay*/ 0x0000),\
    /* Standard AS Isochronous Feedback Endpoint Descriptor(4.10.2.1) */\
    TUD_AUDIO_DESC_STD_AS_ISO_FB_EP(/*_ep*/ _epfb,\
                                /*_epsize*/ 0x04,\
                              /*_interval*/ 0x10)
// clang-format on
#endif /* USB_DESCRIPTORS_H_ */

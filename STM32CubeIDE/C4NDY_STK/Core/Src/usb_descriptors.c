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
 *
 */

#include "tusb.h"
#include "usb_descriptors.h"
#include "stm32g4xx_hal.h"

/* A combination of interfaces must have a unique product id, since PC will save device driver after the first plug.
 * Same VID/PID with different interface e.g MSC (first), then CDC (later) will possibly cause system error on PC.
 *
 * Auto ProductID layout's Bitmap:
 *   [MSB]         HID | MSC | CDC          [LSB]
 */
#define _PID_MAP(itf, n)  ( (CFG_TUD_##itf) << (n) )
#define USB_PID   0x0004

#define USB_VID   0x31BF
#define USB_BCD   0x0200

//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+
tusb_desc_device_t const desc_device =
{
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = USB_BCD,
    .bDeviceClass       = 0xEF,
    .bDeviceSubClass    = 0x02,
    .bDeviceProtocol    = 0x01,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor           = USB_VID,
    .idProduct          = USB_PID,
    .bcdDevice          = 0x0100,

    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,

    .bNumConfigurations = 0x01
};

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const * tud_descriptor_device_cb(void)
{
  return (uint8_t const *) &desc_device;
}

//--------------------------------------------------------------------+
// HID Report Descriptor
//--------------------------------------------------------------------+

uint8_t const desc_hid_report[] =
{
  TUD_HID_REPORT_DESC_KEYBOARD( HID_REPORT_ID(REPORT_ID_KEYBOARD         )),
#if 0
  TUD_HID_REPORT_DESC_MOUSE   ( HID_REPORT_ID(REPORT_ID_MOUSE            )),
  TUD_HID_REPORT_DESC_CONSUMER( HID_REPORT_ID(REPORT_ID_CONSUMER_CONTROL )),
  TUD_HID_REPORT_DESC_GAMEPAD ( HID_REPORT_ID(REPORT_ID_GAMEPAD          ))
#endif
};

uint8_t const desc_hid_report_gio[] =
{
  TUD_HID_REPORT_DESC_GENERIC_INOUT(CFG_TUD_HID_EP_BUFSIZE)
};

// Invoked when received GET HID REPORT DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
//uint8_t const * tud_hid_descriptor_report_cb(uint8_t instance)
uint8_t const * tud_hid_descriptor_report_cb(uint8_t itf)
{
#if 0
	(void) instance;
	return desc_hid_report;
#else
	if (itf == 0)
	{
		return desc_hid_report;
	}
	else if (itf == 1)
	{
		return desc_hid_report_gio;
	}

	return NULL;
#endif
}

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+

#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN + TUD_HID_INOUT_DESC_LEN + TUD_AUDIO_HEADPHONE_STEREO_DESC_LEN)

#define EPNUM_HID       0x01
#define EPNUM_HID_GIO   0x02
#define EPNUM_AUDIO_IN  0x03
#define EPNUM_AUDIO_OUT 0x03

uint8_t const desc_configuration[] =
{
  // Config number, interface count, string index, total length, attribute, power in mA
  TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, 0x00, 500),

  // Interface number, string index, protocol, report descriptor len, EP In address, size & polling interval
  TUD_HID_DESCRIPTOR(ITF_NUM_HID,
		  	  	  	 4,
					 HID_ITF_PROTOCOL_NONE,
					 sizeof(desc_hid_report),
					 EPNUM_HID | 0x80,
					 CFG_TUD_HID_EP_BUFSIZE,
					 5),

  TUD_HID_INOUT_DESCRIPTOR(ITF_NUM_HID_GIO,
		  	  	  	  	   5,
						   HID_ITF_PROTOCOL_NONE,
						   sizeof(desc_hid_report_gio),
						   EPNUM_HID_GIO,
						   0x80 | EPNUM_HID_GIO,
						   CFG_TUD_HID_EP_BUFSIZE,
						   10),

  // Interface number, string index, EP Out & EP In address, EP size
  TUD_AUDIO_HEADPHONE_STEREO_DESCRIPTOR(6,
		  	  	  	  	  	  	  	  EPNUM_AUDIO_OUT,
									  EPNUM_AUDIO_IN | 0x80)
};

// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const * tud_descriptor_configuration_cb(uint8_t index)
{
  (void) index; // for multiple configurations

  // This example use the same configuration for both high and full speed mode
  return desc_configuration;
}

//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+

// array of pointer to string descriptors
char const* string_desc_arr [] =
{
  (const char[]) { 0x09, 0x04 }, // 0: is supported language is English (0x0409)
  "Yamamoto Works Ltd.",         // 1: Manufacturer
  "C4NDY STK",                // 2: Product
  "0",                           // 3: Serials, should use chip ID
  "C4NDY STK(Keyboard)",      // 4: HID Interface
  "C4NDY STK(Setting)",       // 5: HID GIO Interface
  "C4NDY STK(Mixer)",         // 6: UAC Interface
};

static uint16_t _desc_str[32];

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
  (void) langid;

  uint8_t chr_count;

  if ( index == 0)
  {
    memcpy(&_desc_str[1], string_desc_arr[0], 2);
    chr_count = 1;
  }else
  {
    // Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
    // https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors

    if ( !(index < sizeof(string_desc_arr)/sizeof(string_desc_arr[0])) ) return NULL;

    if (index == 3) // serial id
    {
    	char sstr[30] = "";
    	uint16_t upper_id = (uint16_t)((DBGMCU->IDCODE >> 16) & 0x0000FFFF);
    	uint16_t lower_id = (uint16_t)(DBGMCU->IDCODE & 0x0000FFFF);
    	sprintf(sstr, "%x%x", upper_id, lower_id);

    	// Cap at max char
    	chr_count = (uint8_t) strlen(sstr);
    	if ( chr_count > 31 ) chr_count = 31;

    	// Convert ASCII string into UTF-16
    	for(uint8_t i=0; i<chr_count; i++)
    	{
    		_desc_str[1+i] = sstr[i];
    	}
    }
    else
    {
    	const char* str = string_desc_arr[index];

    	// Cap at max char
    	chr_count = (uint8_t) strlen(str);
    	if ( chr_count > 31 ) chr_count = 31;

    	// Convert ASCII string into UTF-16
    	for(uint8_t i=0; i<chr_count; i++)
    	{
    		_desc_str[1+i] = str[i];
    	}
    }
  }

  // first byte is length (including header), second byte is string type
  _desc_str[0] = (uint16_t) ((TUSB_DESC_STRING << 8 ) | (2*chr_count + 2));

  return _desc_str;
}

/*
 * File:           C:\Users\shun\C4NDY\SigmaDSP\ADAU1761_IC_1.h
 *
 * Created:        Wednesday, September 4, 2024 8:57:34 PM
 * Description:    ADAU1761:IC 1 program data.
 *
 * This software is distributed in the hope that it will be useful,
 * but is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * This software may only be used to program products purchased from
 * Analog Devices for incorporation by you into audio products that
 * are intended for resale to audio product end users. This software
 * may not be distributed whole or in any part to third parties.
 *
 * Copyright ©2024 Analog Devices, Inc. All rights reserved.
 */
#ifndef __ADAU1761_IC_1_H__
#define __ADAU1761_IC_1_H__

#include "SigmaStudioFW.h"
#include "ADAU1761_IC_1_REG.h"

#define DEVICE_ARCHITECTURE_IC_1                  "ADAU176x"
#define DEVICE_ADDR_IC_1                          0x70

/* DSP Program Data */
#define PROGRAM_SIZE_IC_1 1705
#define PROGRAM_ADDR_IC_1 2048
ADI_REG_TYPE Program_Data_IC_1[PROGRAM_SIZE_IC_1] = {
0x00, 0x00, 0x00, 0x00, 0x00, 
0xFE, 0xE0, 0x00, 0x00, 0x00, 
0xFF, 0x34, 0x00, 0x00, 0x00, 
0xFF, 0x2C, 0x00, 0x00, 0x00, 
0xFF, 0x54, 0x00, 0x00, 0x00, 
0xFF, 0x5C, 0x00, 0x00, 0x00, 
0xFF, 0xF5, 0x08, 0x20, 0x00, 
0xFF, 0x38, 0x00, 0x00, 0x00, 
0xFF, 0x80, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 
0xFE, 0xE8, 0x0C, 0x00, 0x00, 
0xFE, 0x30, 0x00, 0xE2, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 
0xFF, 0xE8, 0x07, 0x20, 0x08, 
0x00, 0x00, 0x06, 0xA0, 0x00, 
0xFF, 0xE0, 0x00, 0xC0, 0x00, 
0xFF, 0x80, 0x07, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 
0xFF, 0x00, 0x00, 0x00, 0x00, 
0xFE, 0xC0, 0x22, 0x00, 0x27, 
0x00, 0x00, 0x00, 0x00, 0x00, 
0xFE, 0xE8, 0x1E, 0x00, 0x00, 
0xFF, 0xE8, 0x01, 0x20, 0x00, 
0xFF, 0xD8, 0x01, 0x03, 0x00, 
0x00, 0x07, 0xC6, 0x00, 0x00, 
0xFF, 0x08, 0x00, 0x00, 0x00, 
0xFF, 0xF4, 0x00, 0x20, 0x00, 
0xFF, 0xD8, 0x07, 0x02, 0x00, 
0xFD, 0xA5, 0x08, 0x20, 0x00, 
0x00, 0x00, 0x00, 0xE2, 0x00, 
0xFD, 0xAD, 0x08, 0x20, 0x00, 
0x00, 0x08, 0x00, 0xE2, 0x00, 
0xFD, 0x25, 0x08, 0x20, 0x00, 
0x00, 0x10, 0x00, 0xE2, 0x00, 
0xFD, 0x2D, 0x08, 0x20, 0x00, 
0x00, 0x18, 0x00, 0xE2, 0x00, 
0xFF, 0xE8, 0x08, 0x20, 0x00, 
0x00, 0x20, 0x00, 0xE2, 0x00, 
0xFF, 0xE8, 0x09, 0x20, 0x00, 
0x00, 0x28, 0x00, 0xE2, 0x00, 
0x00, 0x55, 0x08, 0x20, 0x00, 
0xFF, 0xE8, 0x0B, 0x34, 0x00, 
0x00, 0x45, 0x08, 0x22, 0x48, 
0x00, 0x50, 0x0A, 0x20, 0x40, 
0x00, 0x40, 0x0A, 0x34, 0x40, 
0x00, 0x50, 0x0A, 0x20, 0x25, 
0x00, 0x40, 0x0A, 0x34, 0x25, 
0x00, 0x68, 0x00, 0xE2, 0x00, 
0x00, 0x60, 0x00, 0xF2, 0x00, 
0x00, 0x55, 0x08, 0x20, 0x00, 
0x00, 0x65, 0x08, 0x22, 0x00, 
0x00, 0x45, 0x08, 0x34, 0x00, 
0x00, 0x6D, 0x08, 0x22, 0x00, 
0x00, 0x48, 0x00, 0xE2, 0x00, 
0x00, 0x58, 0x00, 0xF2, 0x00, 
0x00, 0x4D, 0x08, 0x20, 0x08, 
0xFF, 0xED, 0x1F, 0x20, 0x00, 
0x00, 0x4D, 0x08, 0x20, 0x25, 
0x00, 0x48, 0x00, 0xE2, 0x00, 
0xFF, 0xED, 0x1F, 0x22, 0x40, 
0x00, 0x78, 0x00, 0xE2, 0x00, 
0x00, 0x4D, 0x08, 0x20, 0x00, 
0xFF, 0xE8, 0x0B, 0x22, 0x40, 
0x00, 0x48, 0x0A, 0x30, 0x48, 
0x00, 0x7D, 0x08, 0x20, 0x00, 
0xFF, 0xE8, 0x0B, 0x20, 0x27, 
0x00, 0x78, 0x00, 0xE2, 0x00, 
0x00, 0x78, 0x00, 0xC0, 0x00, 
0x00, 0x17, 0xFF, 0x20, 0x00, 
0x00, 0x30, 0x00, 0xE2, 0x00, 
0x00, 0x1F, 0xFF, 0x20, 0x00, 
0x00, 0x38, 0x00, 0xE2, 0x00, 
0xFF, 0xE8, 0x0C, 0x20, 0x00, 
0x00, 0x85, 0x08, 0x22, 0x40, 
0x00, 0xC0, 0x00, 0xE2, 0x00, 
0x00, 0x95, 0x08, 0x20, 0x00, 
0x00, 0x85, 0x08, 0x34, 0x00, 
0x00, 0xC5, 0x14, 0x22, 0x00, 
0x00, 0x88, 0x00, 0xE2, 0x00, 
0x00, 0x88, 0x00, 0xC0, 0x00, 
0x00, 0x98, 0x00, 0xF2, 0x00, 
0x00, 0x07, 0xFF, 0x20, 0x00, 
0x00, 0xC8, 0x00, 0xE2, 0x00, 
0x00, 0x0F, 0xFF, 0x20, 0x00, 
0x00, 0xD0, 0x00, 0xE2, 0x00, 
0xFF, 0xE8, 0x0D, 0x20, 0x00, 
0x00, 0xA5, 0x08, 0x22, 0x40, 
0x00, 0xC0, 0x00, 0xE2, 0x00, 
0x00, 0xB5, 0x08, 0x20, 0x00, 
0x00, 0xA5, 0x08, 0x34, 0x00, 
0x00, 0xC5, 0x14, 0x22, 0x00, 
0x00, 0xA8, 0x00, 0xE2, 0x00, 
0x00, 0xA8, 0x00, 0xC0, 0x00, 
0x00, 0xB8, 0x00, 0xF2, 0x00, 
0x00, 0x07, 0xFF, 0x20, 0x00, 
0x00, 0xE8, 0x00, 0xE2, 0x00, 
0x00, 0x0F, 0xFF, 0x20, 0x00, 
0x01, 0x90, 0x00, 0xE2, 0x00, 
0x01, 0x10, 0x11, 0x20, 0x00, 
0x01, 0x08, 0x12, 0x22, 0x00, 
0x00, 0xF8, 0x11, 0x34, 0x00, 
0x00, 0xF0, 0x12, 0x22, 0x00, 
0x00, 0xE8, 0x0E, 0x22, 0x00, 
0x00, 0xE0, 0x0F, 0x22, 0x00, 
0x00, 0xD8, 0x10, 0x22, 0x00, 
0x01, 0x00, 0x00, 0xE2, 0x00, 
0x01, 0x18, 0x00, 0xF2, 0x00, 
0x01, 0x40, 0x16, 0x20, 0x00, 
0x01, 0x38, 0x17, 0x22, 0x00, 
0x01, 0x28, 0x16, 0x34, 0x00, 
0x01, 0x20, 0x17, 0x22, 0x00, 
0x01, 0x00, 0x13, 0x22, 0x00, 
0x00, 0xF8, 0x14, 0x22, 0x00, 
0x00, 0xF0, 0x15, 0x22, 0x00, 
0x01, 0x30, 0x00, 0xE2, 0x00, 
0x01, 0x48, 0x00, 0xF2, 0x00, 
0x01, 0x70, 0x1B, 0x20, 0x00, 
0x01, 0x68, 0x1C, 0x22, 0x00, 
0x01, 0x58, 0x1B, 0x34, 0x00, 
0x01, 0x50, 0x1C, 0x22, 0x00, 
0x01, 0x30, 0x18, 0x22, 0x00, 
0x01, 0x28, 0x19, 0x22, 0x00, 
0x01, 0x20, 0x1A, 0x22, 0x00, 
0x01, 0x60, 0x00, 0xE2, 0x00, 
0x01, 0x78, 0x00, 0xF2, 0x00, 
0x01, 0xB8, 0x11, 0x20, 0x00, 
0x01, 0xB0, 0x12, 0x22, 0x00, 
0x01, 0xA0, 0x11, 0x34, 0x00, 
0x01, 0x98, 0x12, 0x22, 0x00, 
0x01, 0x90, 0x0E, 0x22, 0x00, 
0x01, 0x88, 0x0F, 0x22, 0x00, 
0x01, 0x80, 0x10, 0x22, 0x00, 
0x01, 0xA8, 0x00, 0xE2, 0x00, 
0x01, 0xC0, 0x00, 0xF2, 0x00, 
0x01, 0xE8, 0x16, 0x20, 0x00, 
0x01, 0xE0, 0x17, 0x22, 0x00, 
0x01, 0xD0, 0x16, 0x34, 0x00, 
0x01, 0xC8, 0x17, 0x22, 0x00, 
0x01, 0xA8, 0x13, 0x22, 0x00, 
0x01, 0xA0, 0x14, 0x22, 0x00, 
0x01, 0x98, 0x15, 0x22, 0x00, 
0x01, 0xD8, 0x00, 0xE2, 0x00, 
0x01, 0xF0, 0x00, 0xF2, 0x00, 
0x02, 0x18, 0x1B, 0x20, 0x00, 
0x02, 0x10, 0x1C, 0x22, 0x00, 
0x02, 0x00, 0x1B, 0x34, 0x00, 
0x01, 0xF8, 0x1C, 0x22, 0x00, 
0x01, 0xD8, 0x18, 0x22, 0x00, 
0x01, 0xD0, 0x19, 0x22, 0x00, 
0x01, 0xC8, 0x1A, 0x22, 0x00, 
0x02, 0x08, 0x00, 0xE2, 0x00, 
0x02, 0x20, 0x00, 0xF2, 0x00, 
0xFF, 0xE8, 0x1D, 0x20, 0x00, 
0x02, 0x35, 0x08, 0x22, 0x40, 
0x02, 0x50, 0x00, 0xE2, 0x00, 
0x02, 0x45, 0x08, 0x20, 0x00, 
0x02, 0x35, 0x08, 0x34, 0x00, 
0x02, 0x50, 0x1E, 0x22, 0x00, 
0x02, 0x38, 0x00, 0xE2, 0x00, 
0x02, 0x38, 0x00, 0xC0, 0x00, 
0x02, 0x48, 0x00, 0xF2, 0x00, 
0x00, 0x37, 0xFF, 0x20, 0x00, 
0x02, 0x28, 0x00, 0xE2, 0x00, 
0xFF, 0xE8, 0x1F, 0x20, 0x00, 
0x02, 0x65, 0x08, 0x22, 0x40, 
0x02, 0x80, 0x00, 0xE2, 0x00, 
0x02, 0x75, 0x08, 0x20, 0x00, 
0x02, 0x65, 0x08, 0x34, 0x00, 
0x02, 0x80, 0x20, 0x22, 0x00, 
0x02, 0x68, 0x00, 0xE2, 0x00, 
0x02, 0x68, 0x00, 0xC0, 0x00, 
0x02, 0x78, 0x00, 0xF2, 0x00, 
0x00, 0x3F, 0xFF, 0x20, 0x00, 
0x02, 0x58, 0x00, 0xE2, 0x00, 
0x00, 0x2D, 0x08, 0x20, 0x00, 
0x02, 0x9D, 0x08, 0x22, 0x40, 
0x02, 0xB8, 0x00, 0xE2, 0x00, 
0x02, 0xAD, 0x08, 0x20, 0x00, 
0x02, 0x9D, 0x08, 0x34, 0x00, 
0x02, 0xB8, 0x21, 0x22, 0x00, 
0x02, 0xA0, 0x00, 0xE2, 0x00, 
0x02, 0xA0, 0x00, 0xC0, 0x00, 
0x02, 0xB0, 0x00, 0xF2, 0x00, 
0x02, 0x2F, 0xFF, 0x20, 0x00, 
0x02, 0x88, 0x00, 0xE2, 0x00, 
0x02, 0x5F, 0xFF, 0x20, 0x00, 
0x02, 0x90, 0x00, 0xE2, 0x00, 
0xFF, 0xE8, 0x22, 0x20, 0x00, 
0x02, 0xC5, 0x08, 0x22, 0x40, 
0x02, 0xE0, 0x00, 0xE2, 0x00, 
0x02, 0xD5, 0x08, 0x20, 0x00, 
0x02, 0xC5, 0x08, 0x34, 0x00, 
0x02, 0xE0, 0x23, 0x22, 0x00, 
0x02, 0xC8, 0x00, 0xE2, 0x00, 
0x02, 0xC8, 0x00, 0xC0, 0x00, 
0x02, 0xD8, 0x00, 0xF2, 0x00, 
0x01, 0x67, 0xFF, 0x20, 0x00, 
0x02, 0xF8, 0x00, 0xE2, 0x00, 
0x02, 0x0F, 0xFF, 0x20, 0x00, 
0x03, 0x10, 0x00, 0xE2, 0x00, 
0x03, 0x38, 0x27, 0x20, 0x00, 
0x03, 0x30, 0x28, 0x22, 0x00, 
0x03, 0x20, 0x27, 0x34, 0x00, 
0x03, 0x18, 0x28, 0x22, 0x00, 
0x02, 0xF8, 0x24, 0x22, 0x00, 
0x02, 0xF0, 0x25, 0x22, 0x00, 
0x02, 0xE8, 0x26, 0x22, 0x00, 
0x03, 0x28, 0x00, 0xE2, 0x00, 
0x03, 0x40, 0x00, 0xF2, 0x00, 
0x03, 0x68, 0x27, 0x20, 0x00, 
0x03, 0x60, 0x28, 0x22, 0x00, 
0x03, 0x50, 0x27, 0x34, 0x00, 
0x03, 0x48, 0x28, 0x22, 0x00, 
0x03, 0x10, 0x24, 0x22, 0x00, 
0x03, 0x08, 0x25, 0x22, 0x00, 
0x03, 0x00, 0x26, 0x22, 0x00, 
0x03, 0x58, 0x00, 0xE2, 0x00, 
0x03, 0x70, 0x00, 0xF2, 0x00, 
0x03, 0x28, 0x29, 0x20, 0x00, 
0x03, 0x58, 0x29, 0x22, 0x00, 
0x03, 0x78, 0x00, 0xE2, 0x00, 
0xFF, 0xE8, 0x55, 0x40, 0x00, 
0x03, 0x7D, 0x08, 0x20, 0x00, 
0x03, 0xD0, 0x00, 0xE2, 0x00, 
0x03, 0xD5, 0x0C, 0x20, 0x00, 
0x04, 0x20, 0x00, 0xE2, 0x00, 
0x04, 0x20, 0x00, 0xC0, 0x00, 
0x04, 0x27, 0xFF, 0x20, 0x00, 
0x04, 0x28, 0x00, 0xE2, 0x00, 
0x03, 0xD0, 0x00, 0xC0, 0x00, 
0x03, 0xD7, 0xFF, 0x20, 0x00, 
0x03, 0xF0, 0x00, 0xE2, 0x00, 
0x03, 0xF8, 0x00, 0xF2, 0x00, 
0x04, 0x35, 0x08, 0x20, 0x00, 
0x04, 0x30, 0x52, 0x22, 0x40, 
0x04, 0x28, 0x52, 0x22, 0x08, 
0x04, 0x38, 0x00, 0xE2, 0x00, 
0x03, 0xA5, 0x08, 0x20, 0x00, 
0x03, 0xA0, 0x52, 0x22, 0x40, 
0x03, 0xF8, 0x52, 0x22, 0x00, 
0x03, 0x95, 0x08, 0x34, 0x00, 
0x03, 0x90, 0x52, 0x22, 0x40, 
0x03, 0xF0, 0x52, 0x22, 0x00, 
0x03, 0x98, 0x00, 0xE2, 0x00, 
0xFF, 0xF5, 0x08, 0x42, 0x00, 
0x03, 0xA8, 0x00, 0xF2, 0x00, 
0x04, 0x3D, 0x08, 0x20, 0x00, 
0xFF, 0xF5, 0x08, 0x82, 0x00, 
0xFF, 0xF5, 0x08, 0x84, 0x23, 
0xFF, 0xED, 0x0A, 0x20, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 
0x04, 0x18, 0x00, 0xF6, 0x00, 
0x04, 0x1D, 0x08, 0x22, 0x00, 
0x04, 0x00, 0x00, 0xE2, 0x00, 
0x04, 0x00, 0x00, 0xF6, 0x23, 
0x03, 0xC5, 0x08, 0x20, 0x08, 
0x03, 0xC8, 0x00, 0xE2, 0x00, 
0x03, 0xB5, 0x08, 0x20, 0x00, 
0xFF, 0xE8, 0x54, 0x22, 0x67, 
0x03, 0xB8, 0x00, 0xE2, 0x00, 
0x04, 0x05, 0x08, 0x22, 0x48, 
0x04, 0x05, 0x08, 0x20, 0x00, 
0x03, 0xB8, 0x00, 0xE2, 0x27, 
0xFF, 0xE8, 0x53, 0x20, 0x00, 
0x03, 0xC8, 0x00, 0xE2, 0x27, 
0x03, 0xCD, 0x08, 0x20, 0x08, 
0xFF, 0xFD, 0x08, 0x22, 0x40, 
0x03, 0xC8, 0x00, 0xE2, 0x26, 
0x03, 0xBD, 0x1A, 0x20, 0x00, 
0x04, 0x08, 0x00, 0xE2, 0x00, 
0x04, 0x10, 0x00, 0xF2, 0x00, 
0x04, 0x08, 0x00, 0xC0, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x2B, 0xA1, 0x00, 
0xFF, 0xE5, 0x08, 0x20, 0x00, 
0x00, 0x00, 0x2A, 0xA1, 0x00, 
0xFF, 0xE5, 0x08, 0x22, 0x40, 
0x03, 0xD8, 0x00, 0xE2, 0x00, 
0x04, 0x10, 0x00, 0xC0, 0x00, 
0x03, 0xDF, 0xFF, 0x20, 0x00, 
0xFF, 0xE5, 0x08, 0x22, 0x00, 
0x03, 0xE8, 0x00, 0xE2, 0x00, 
0x03, 0xE8, 0x00, 0xC0, 0x00, 
0x03, 0x2F, 0xFF, 0x20, 0x00, 
0x03, 0x80, 0x00, 0xE2, 0x00, 
0x03, 0x5F, 0xFF, 0x20, 0x00, 
0x03, 0x88, 0x00, 0xE2, 0x00, 
0x00, 0xC8, 0x56, 0x20, 0x00, 
0x03, 0x80, 0x56, 0x22, 0x00, 
0x04, 0x40, 0x00, 0xE2, 0x00, 
0x00, 0xD0, 0x57, 0x20, 0x00, 
0x03, 0x88, 0x57, 0x22, 0x00, 
0x04, 0x48, 0x00, 0xE2, 0x00, 
0x00, 0x25, 0x08, 0x20, 0x00, 
0x04, 0x65, 0x08, 0x22, 0x40, 
0x04, 0x80, 0x00, 0xE2, 0x00, 
0x04, 0x75, 0x08, 0x20, 0x00, 
0x04, 0x65, 0x08, 0x34, 0x00, 
0x04, 0x80, 0x58, 0x22, 0x00, 
0x04, 0x68, 0x00, 0xE2, 0x00, 
0x04, 0x68, 0x00, 0xC0, 0x00, 
0x04, 0x78, 0x00, 0xF2, 0x00, 
0x04, 0x47, 0xFF, 0x20, 0x00, 
0x04, 0x50, 0x00, 0xE2, 0x00, 
0x04, 0x4F, 0xFF, 0x20, 0x00, 
0x04, 0x58, 0x00, 0xE2, 0x00, 
0x04, 0x50, 0x59, 0x20, 0x00, 
0x02, 0x88, 0x59, 0x22, 0x00, 
0x04, 0x88, 0x00, 0xE2, 0x00, 
0x04, 0x58, 0x5A, 0x20, 0x00, 
0x02, 0x90, 0x5A, 0x22, 0x00, 
0x04, 0x90, 0x00, 0xE2, 0x00, 
0xFF, 0xE8, 0x5B, 0x20, 0x00, 
0x04, 0xAD, 0x08, 0x22, 0x40, 
0x04, 0xC8, 0x00, 0xE2, 0x00, 
0x04, 0xBD, 0x08, 0x20, 0x00, 
0x04, 0xAD, 0x08, 0x34, 0x00, 
0x04, 0xC8, 0x5C, 0x22, 0x00, 
0x04, 0xB0, 0x00, 0xE2, 0x00, 
0x04, 0xB0, 0x00, 0xC0, 0x00, 
0x04, 0xC0, 0x00, 0xF2, 0x00, 
0x04, 0x8F, 0xFF, 0x20, 0x00, 
0x04, 0x98, 0x00, 0xE2, 0x00, 
0x04, 0x97, 0xFF, 0x20, 0x00, 
0x04, 0xA0, 0x00, 0xE2, 0x00, 
0x04, 0x9D, 0x08, 0x20, 0x00, 
0xFD, 0xB0, 0x00, 0xE2, 0x00, 
0x04, 0xA5, 0x08, 0x20, 0x00, 
0xFD, 0xB8, 0x00, 0xE2, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 
0xFE, 0x30, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 
0xFE, 0xC0, 0x0F, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 
};

/* DSP Parameter (Coefficient) Data */
#define PARAM_SIZE_IC_1 372
#define PARAM_ADDR_IC_1 0
ADI_REG_TYPE Param_Data_IC_1[PARAM_SIZE_IC_1] = {
0x00, 0x00, 0x10, 0x00, 
0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 
0x00, 0x40, 0x00, 0x00, 
0x00, 0x40, 0x00, 0x00, 
0x00, 0x00, 0x10, 0x00, 
0x00, 0x80, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 
0x00, 0x80, 0x00, 0x00, 
0x00, 0x84, 0xBF, 0xBA, 
0x0F, 0x01, 0x0E, 0x63, 
0x00, 0x7A, 0x32, 0x1C, 
0x00, 0xFE, 0xF1, 0x9D, 
0x0F, 0x81, 0x0E, 0x29, 
0x00, 0x7E, 0x88, 0x97, 
0x0F, 0x06, 0xBB, 0xD6, 
0x00, 0x7A, 0xC3, 0xDF, 
0x00, 0xF9, 0x44, 0x2A, 
0x0F, 0x86, 0xB3, 0x8A, 
0x00, 0x33, 0x81, 0x11, 
0x00, 0x4A, 0x7C, 0x78, 
0x00, 0x22, 0x81, 0x4E, 
0x0F, 0xB5, 0x83, 0x88, 
0x00, 0x29, 0xFD, 0xA1, 
0x00, 0x80, 0x00, 0x00, 
0x00, 0x00, 0x80, 0x00, 
0x00, 0x80, 0x00, 0x00, 
0x00, 0x00, 0x80, 0x00, 
0x00, 0x01, 0x00, 0x00, 
0x07, 0xFF, 0xFF, 0xF8, 
0x00, 0x00, 0x80, 0x00, 
0x00, 0x7D, 0xD1, 0xC8, 
0x0F, 0x04, 0x62, 0xB3, 
0x00, 0x7D, 0xCB, 0x85, 
0x00, 0xFB, 0x93, 0xAF, 
0x0F, 0x84, 0x59, 0x15, 
0x00, 0x40, 0x00, 0x00, 
0x00, 0x80, 0x00, 0x00, 
0x00, 0x80, 0x00, 0x00, 
0x00, 0x5B, 0x12, 0xD0, 
0x00, 0x40, 0xCC, 0xC2, 
0x00, 0x2E, 0x1B, 0x18, 
0x00, 0x20, 0xCE, 0x0A, 
0x00, 0x17, 0x57, 0x4D, 
0x00, 0x10, 0x9B, 0x7F, 
0x00, 0x0B, 0xD0, 0xFD, 
0x00, 0x08, 0x68, 0x50, 
0x00, 0x05, 0xFB, 0x66, 
0x00, 0x04, 0x41, 0x9B, 
0x00, 0x03, 0x07, 0x45, 
0x00, 0x02, 0x27, 0x9D, 
0x00, 0x01, 0x88, 0x7A, 
0x00, 0x04, 0x5B, 0x48, 
0x00, 0x10, 0xDF, 0x41, 
0x00, 0x41, 0x57, 0x98, 
0x00, 0xA1, 0x24, 0x78, 
0x00, 0xA1, 0x24, 0x78, 
0x00, 0xA1, 0x24, 0x78, 
0x00, 0xA1, 0x24, 0x78, 
0x00, 0xA1, 0x24, 0x78, 
0x00, 0xA1, 0x24, 0x78, 
0x00, 0xA1, 0x24, 0x78, 
0x00, 0xA1, 0x24, 0x78, 
0x00, 0xA1, 0x24, 0x78, 
0x00, 0xA1, 0x24, 0x78, 
0x00, 0xA1, 0x24, 0x78, 
0x00, 0xA1, 0x24, 0x78, 
0x00, 0xA1, 0x24, 0x78, 
0x00, 0xA1, 0x24, 0x78, 
0x00, 0xA1, 0x24, 0x78, 
0x00, 0xA1, 0x24, 0x78, 
0x00, 0x71, 0xF1, 0xE6, 
0x00, 0x50, 0x92, 0x3C, 
0x00, 0x38, 0xF8, 0xF3, 
0x00, 0x28, 0x49, 0x1E, 
0x00, 0x1C, 0x7C, 0x7A, 
0x00, 0x14, 0x24, 0x8F, 
0x00, 0x00, 0x13, 0x07, 
0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0xB6, 
0x00, 0x00, 0x51, 0xEC, 
0x00, 0x40, 0x00, 0x00, 
0x00, 0x40, 0x00, 0x00, 
0x00, 0x01, 0x00, 0x00, 
0x00, 0x40, 0x00, 0x00, 
0x00, 0x40, 0x00, 0x00, 
0x00, 0x80, 0x00, 0x00, 
0x00, 0x00, 0x08, 0x00, 
};


/* Register Default - IC 1.Sample Rate Setting */
ADI_REG_TYPE R0_SAMPLE_RATE_SETTING_IC_1_Default[REG_SAMPLE_RATE_SETTING_IC_1_BYTE] = {
0x7F
};

/* Register Default - IC 1.DSP Run Register */
ADI_REG_TYPE R1_DSP_RUN_REGISTER_IC_1_Default[REG_DSP_RUN_REGISTER_IC_1_BYTE] = {
0x00
};

/* Register Default - IC 1.Clock Control Register */
ADI_REG_TYPE R2_CLKCTRLREGISTER_IC_1_Default[REG_CLKCTRLREGISTER_IC_1_BYTE] = {
0x0F
};

/* Register Default - IC 1.PLL Control Register */
ADI_REG_TYPE R3_PLLCRLREGISTER_IC_1_Default[REG_PLLCRLREGISTER_IC_1_BYTE] = {
0x00, 0x01, 0x00, 0x00, 0x20, 0x03
};

/* Register Default - IC 1.Delay */
#define R4_DELAY_IC_1_ADDR 0x0
#define R4_DELAY_IC_1_SIZE 2
ADI_REG_TYPE R4_DELAY_IC_1_Default[R4_DELAY_IC_1_SIZE] = {
0x00, 0x64
};

/* Register Default - IC 1.Serial Port Control Registers */
#define R5_SERIAL_PORT_CONTROL_REGISTERS_IC_1_SIZE 2
ADI_REG_TYPE R5_SERIAL_PORT_CONTROL_REGISTERS_IC_1_Default[R5_SERIAL_PORT_CONTROL_REGISTERS_IC_1_SIZE] = {
0x00, 0x02
};

/* Register Default - IC 1.ALC Control Registers */
#define R6_ALC_CONTROL_REGISTERS_IC_1_SIZE 4
ADI_REG_TYPE R6_ALC_CONTROL_REGISTERS_IC_1_Default[R6_ALC_CONTROL_REGISTERS_IC_1_SIZE] = {
0x00, 0x00, 0x00, 0x00
};

/* Register Default - IC 1.Microphone Control Register */
ADI_REG_TYPE R7_MICCTRLREGISTER_IC_1_Default[REG_MICCTRLREGISTER_IC_1_BYTE] = {
0x00
};

/* Register Default - IC 1.Record Input Signal Path Registers */
#define R8_RECORD_INPUT_SIGNAL_PATH_REGISTERS_IC_1_SIZE 8
ADI_REG_TYPE R8_RECORD_INPUT_SIGNAL_PATH_REGISTERS_IC_1_Default[R8_RECORD_INPUT_SIGNAL_PATH_REGISTERS_IC_1_SIZE] = {
0x00, 0x01, 0x05, 0x01, 0x05, 0x00, 0x00, 0x00
};

/* Register Default - IC 1.ADC Control Registers */
#define R9_ADC_CONTROL_REGISTERS_IC_1_SIZE 3
ADI_REG_TYPE R9_ADC_CONTROL_REGISTERS_IC_1_Default[R9_ADC_CONTROL_REGISTERS_IC_1_SIZE] = {
0x13, 0x00, 0x00
};

/* Register Default - IC 1.Playback Output Signal Path Registers */
#define R10_PLAYBACK_OUTPUT_SIGNAL_PATH_REGISTERS_IC_1_SIZE 14
ADI_REG_TYPE R10_PLAYBACK_OUTPUT_SIGNAL_PATH_REGISTERS_IC_1_Default[R10_PLAYBACK_OUTPUT_SIGNAL_PATH_REGISTERS_IC_1_SIZE] = {
0x21, 0x00, 0x41, 0x00, 0x00, 0x00, 0x01, 0xE7, 0xE7, 0x00, 0x00, 0xE5, 0x00, 0x03
};

/* Register Default - IC 1.Converter Control Registers */
#define R11_CONVERTER_CONTROL_REGISTERS_IC_1_SIZE 2
ADI_REG_TYPE R11_CONVERTER_CONTROL_REGISTERS_IC_1_Default[R11_CONVERTER_CONTROL_REGISTERS_IC_1_SIZE] = {
0x00, 0x00
};

/* Register Default - IC 1.DAC Control Registers */
#define R12_DAC_CONTROL_REGISTERS_IC_1_SIZE 3
ADI_REG_TYPE R12_DAC_CONTROL_REGISTERS_IC_1_Default[R12_DAC_CONTROL_REGISTERS_IC_1_SIZE] = {
0x03, 0x00, 0x00
};

/* Register Default - IC 1.Serial Port Pad Control Registers */
#define R13_SERIAL_PORT_PAD_CONTROL_REGISTERS_IC_1_SIZE 1
ADI_REG_TYPE R13_SERIAL_PORT_PAD_CONTROL_REGISTERS_IC_1_Default[R13_SERIAL_PORT_PAD_CONTROL_REGISTERS_IC_1_SIZE] = {
0xAA
};

/* Register Default - IC 1.Communication Port Pad Control Registers */
#define R14_COMMUNICATION_PORT_PAD_CONTROL_REGISTERS_IC_1_SIZE 2
ADI_REG_TYPE R14_COMMUNICATION_PORT_PAD_CONTROL_REGISTERS_IC_1_Default[R14_COMMUNICATION_PORT_PAD_CONTROL_REGISTERS_IC_1_SIZE] = {
0xAA, 0x00
};

/* Register Default - IC 1.Jack Detect Pad Control Register */
ADI_REG_TYPE R15_JACKREGISTER_IC_1_Default[REG_JACKREGISTER_IC_1_BYTE] = {
0x08
};

/* Register Default - IC 1.DSP ON Register */
ADI_REG_TYPE R21_DSP_ENABLE_REGISTER_IC_1_Default[REG_DSP_ENABLE_REGISTER_IC_1_BYTE] = {
0x01
};

/* Register Default - IC 1.CRC Registers */
#define R22_CRC_REGISTERS_IC_1_SIZE 5
ADI_REG_TYPE R22_CRC_REGISTERS_IC_1_Default[R22_CRC_REGISTERS_IC_1_SIZE] = {
0x4A, 0x7F, 0x7F, 0x2C, 0x01
};

/* Register Default - IC 1.GPIO Registers */
#define R23_GPIO_REGISTERS_IC_1_SIZE 4
ADI_REG_TYPE R23_GPIO_REGISTERS_IC_1_Default[R23_GPIO_REGISTERS_IC_1_SIZE] = {
0x00, 0x00, 0x00, 0x00
};

/* Register Default - IC 1.Non Modulo Registers */
#define R24_NON_MODULO_REGISTERS_IC_1_SIZE 2
ADI_REG_TYPE R24_NON_MODULO_REGISTERS_IC_1_Default[R24_NON_MODULO_REGISTERS_IC_1_SIZE] = {
0x10, 0x00
};

/* Register Default - IC 1.Watchdog Registers */
#define R25_WATCHDOG_REGISTERS_IC_1_SIZE 5
ADI_REG_TYPE R25_WATCHDOG_REGISTERS_IC_1_Default[R25_WATCHDOG_REGISTERS_IC_1_SIZE] = {
0x00, 0x04, 0x00, 0x00, 0x00
};

/* Register Default - IC 1.Sampling Rate Setting Register */
ADI_REG_TYPE R26_SAMPLE_RATE_SETTING_IC_1_Default[REG_SAMPLE_RATE_SETTING_IC_1_BYTE] = {
0x7F
};

/* Register Default - IC 1.Routing Matrix Inputs Register */
ADI_REG_TYPE R27_ROUTING_MATRIX_INPUTS_IC_1_Default[REG_ROUTING_MATRIX_INPUTS_IC_1_BYTE] = {
0x00
};

/* Register Default - IC 1.Routing Matrix Outputs Register */
ADI_REG_TYPE R28_ROUTING_MATRIX_OUTPUTS_IC_1_Default[REG_ROUTING_MATRIX_OUTPUTS_IC_1_BYTE] = {
0x00
};

/* Register Default - IC 1.Serial Data Configuration Register */
ADI_REG_TYPE R29_SERIAL_DATAGPIO_PIN_CONFIG_IC_1_Default[REG_SERIAL_DATAGPIO_PIN_CONFIG_IC_1_BYTE] = {
0x00
};

/* Register Default - IC 1.DSP Slew Mode Register */
ADI_REG_TYPE R30_DSP_SLEW_MODES_IC_1_Default[REG_DSP_SLEW_MODES_IC_1_BYTE] = {
0x00
};

/* Register Default - IC 1.Serial Port Sample Rate Register */
ADI_REG_TYPE R31_SERIAL_PORT_SAMPLE_RATE_SETTING_IC_1_Default[REG_SERIAL_PORT_SAMPLE_RATE_SETTING_IC_1_BYTE] = {
0x00
};

/* Register Default - IC 1.Clock Enable Registers */
#define R32_CLOCK_ENABLE_REGISTERS_IC_1_SIZE 2
ADI_REG_TYPE R32_CLOCK_ENABLE_REGISTERS_IC_1_Default[R32_CLOCK_ENABLE_REGISTERS_IC_1_SIZE] = {
0x7F, 0x03
};

/* Register Default - IC 1.Sample Rate Setting */
ADI_REG_TYPE R35_SAMPLE_RATE_SETTING_IC_1_Default[REG_SAMPLE_RATE_SETTING_IC_1_BYTE] = {
0x01
};

/* Register Default - IC 1.DSP Run Register */
ADI_REG_TYPE R36_DSP_RUN_REGISTER_IC_1_Default[REG_DSP_RUN_REGISTER_IC_1_BYTE] = {
0x01
};

/* Register Default - IC 1.Dejitter Register Control */
ADI_REG_TYPE R37_DEJITTER_REGISTER_CONTROL_IC_1_Default[REG_DEJITTER_REGISTER_CONTROL_IC_1_BYTE] = {
0x00
};

/* Register Default - IC 1.Dejitter Register Control */
ADI_REG_TYPE R38_DEJITTER_REGISTER_CONTROL_IC_1_Default[REG_DEJITTER_REGISTER_CONTROL_IC_1_BYTE] = {
0x03
};


/*
 * Default Download
 */
#define DEFAULT_DOWNLOAD_SIZE_IC_1 39

void default_download_IC_1() {
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, REG_SAMPLE_RATE_SETTING_IC_1_ADDR, REG_SAMPLE_RATE_SETTING_IC_1_BYTE, R0_SAMPLE_RATE_SETTING_IC_1_Default );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, REG_DSP_RUN_REGISTER_IC_1_ADDR, REG_DSP_RUN_REGISTER_IC_1_BYTE, R1_DSP_RUN_REGISTER_IC_1_Default );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, REG_CLKCTRLREGISTER_IC_1_ADDR, REG_CLKCTRLREGISTER_IC_1_BYTE, R2_CLKCTRLREGISTER_IC_1_Default );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, REG_PLLCRLREGISTER_IC_1_ADDR, REG_PLLCRLREGISTER_IC_1_BYTE, R3_PLLCRLREGISTER_IC_1_Default );
	SIGMA_WRITE_DELAY( DEVICE_ADDR_IC_1, R4_DELAY_IC_1_SIZE, R4_DELAY_IC_1_Default );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, REG_SERIAL_PORT_CONTROL_0_IC_1_ADDR , R5_SERIAL_PORT_CONTROL_REGISTERS_IC_1_SIZE, R5_SERIAL_PORT_CONTROL_REGISTERS_IC_1_Default );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, REG_ALC_CONTROL_0_IC_1_ADDR , R6_ALC_CONTROL_REGISTERS_IC_1_SIZE, R6_ALC_CONTROL_REGISTERS_IC_1_Default );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, REG_MICCTRLREGISTER_IC_1_ADDR, REG_MICCTRLREGISTER_IC_1_BYTE, R7_MICCTRLREGISTER_IC_1_Default );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, REG_RECORD_PWR_MANAGEMENT_IC_1_ADDR , R8_RECORD_INPUT_SIGNAL_PATH_REGISTERS_IC_1_SIZE, R8_RECORD_INPUT_SIGNAL_PATH_REGISTERS_IC_1_Default );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, REG_ADC_CONTROL_0_IC_1_ADDR , R9_ADC_CONTROL_REGISTERS_IC_1_SIZE, R9_ADC_CONTROL_REGISTERS_IC_1_Default );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, REG_PLAYBACK_MIXER_LEFT_CONTROL_0_IC_1_ADDR , R10_PLAYBACK_OUTPUT_SIGNAL_PATH_REGISTERS_IC_1_SIZE, R10_PLAYBACK_OUTPUT_SIGNAL_PATH_REGISTERS_IC_1_Default );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, REG_CONVERTER_CTRL_0_IC_1_ADDR , R11_CONVERTER_CONTROL_REGISTERS_IC_1_SIZE, R11_CONVERTER_CONTROL_REGISTERS_IC_1_Default );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, REG_DAC_CONTROL_0_IC_1_ADDR , R12_DAC_CONTROL_REGISTERS_IC_1_SIZE, R12_DAC_CONTROL_REGISTERS_IC_1_Default );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, REG_SERIAL_PORT_PAD_CONTROL_0_IC_1_ADDR , R13_SERIAL_PORT_PAD_CONTROL_REGISTERS_IC_1_SIZE, R13_SERIAL_PORT_PAD_CONTROL_REGISTERS_IC_1_Default );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, REG_COMM_PORT_PAD_CTRL_0_IC_1_ADDR , R14_COMMUNICATION_PORT_PAD_CONTROL_REGISTERS_IC_1_SIZE, R14_COMMUNICATION_PORT_PAD_CONTROL_REGISTERS_IC_1_Default );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, REG_JACKREGISTER_IC_1_ADDR, REG_JACKREGISTER_IC_1_BYTE, R15_JACKREGISTER_IC_1_Default );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, PROGRAM_ADDR_IC_1, PROGRAM_SIZE_IC_1, Program_Data_IC_1 );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, PROGRAM_ADDR_IC_1, PROGRAM_SIZE_IC_1, Program_Data_IC_1 );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, PROGRAM_ADDR_IC_1, PROGRAM_SIZE_IC_1, Program_Data_IC_1 );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, PROGRAM_ADDR_IC_1, PROGRAM_SIZE_IC_1, Program_Data_IC_1 );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, PROGRAM_ADDR_IC_1, PROGRAM_SIZE_IC_1, Program_Data_IC_1 );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, REG_DSP_ENABLE_REGISTER_IC_1_ADDR, REG_DSP_ENABLE_REGISTER_IC_1_BYTE, R21_DSP_ENABLE_REGISTER_IC_1_Default );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, REG_CRC_IDEAL_1_IC_1_ADDR , R22_CRC_REGISTERS_IC_1_SIZE, R22_CRC_REGISTERS_IC_1_Default );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, REG_GPIO_0_CONTROL_IC_1_ADDR , R23_GPIO_REGISTERS_IC_1_SIZE, R23_GPIO_REGISTERS_IC_1_Default );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, REG_NON_MODULO_RAM_1_IC_1_ADDR , R24_NON_MODULO_REGISTERS_IC_1_SIZE, R24_NON_MODULO_REGISTERS_IC_1_Default );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, REG_WATCHDOG_ENABLE_IC_1_ADDR , R25_WATCHDOG_REGISTERS_IC_1_SIZE, R25_WATCHDOG_REGISTERS_IC_1_Default );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, REG_SAMPLE_RATE_SETTING_IC_1_ADDR, REG_SAMPLE_RATE_SETTING_IC_1_BYTE, R26_SAMPLE_RATE_SETTING_IC_1_Default );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, REG_ROUTING_MATRIX_INPUTS_IC_1_ADDR, REG_ROUTING_MATRIX_INPUTS_IC_1_BYTE, R27_ROUTING_MATRIX_INPUTS_IC_1_Default );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, REG_ROUTING_MATRIX_OUTPUTS_IC_1_ADDR, REG_ROUTING_MATRIX_OUTPUTS_IC_1_BYTE, R28_ROUTING_MATRIX_OUTPUTS_IC_1_Default );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, REG_SERIAL_DATAGPIO_PIN_CONFIG_IC_1_ADDR, REG_SERIAL_DATAGPIO_PIN_CONFIG_IC_1_BYTE, R29_SERIAL_DATAGPIO_PIN_CONFIG_IC_1_Default );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, REG_DSP_SLEW_MODES_IC_1_ADDR, REG_DSP_SLEW_MODES_IC_1_BYTE, R30_DSP_SLEW_MODES_IC_1_Default );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, REG_SERIAL_PORT_SAMPLE_RATE_SETTING_IC_1_ADDR, REG_SERIAL_PORT_SAMPLE_RATE_SETTING_IC_1_BYTE, R31_SERIAL_PORT_SAMPLE_RATE_SETTING_IC_1_Default );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, REG_CLOCK_ENABLE_REG_0_IC_1_ADDR , R32_CLOCK_ENABLE_REGISTERS_IC_1_SIZE, R32_CLOCK_ENABLE_REGISTERS_IC_1_Default );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, PROGRAM_ADDR_IC_1, PROGRAM_SIZE_IC_1, Program_Data_IC_1 );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, PARAM_ADDR_IC_1, PARAM_SIZE_IC_1, Param_Data_IC_1 );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, REG_SAMPLE_RATE_SETTING_IC_1_ADDR, REG_SAMPLE_RATE_SETTING_IC_1_BYTE, R35_SAMPLE_RATE_SETTING_IC_1_Default );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, REG_DSP_RUN_REGISTER_IC_1_ADDR, REG_DSP_RUN_REGISTER_IC_1_BYTE, R36_DSP_RUN_REGISTER_IC_1_Default );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, REG_DEJITTER_REGISTER_CONTROL_IC_1_ADDR, REG_DEJITTER_REGISTER_CONTROL_IC_1_BYTE, R37_DEJITTER_REGISTER_CONTROL_IC_1_Default );
	SIGMA_WRITE_REGISTER_BLOCK( DEVICE_ADDR_IC_1, REG_DEJITTER_REGISTER_CONTROL_IC_1_ADDR, REG_DEJITTER_REGISTER_CONTROL_IC_1_BYTE, R38_DEJITTER_REGISTER_CONTROL_IC_1_Default );
}

#endif

/*
 * File:           C:\Users\shun\C4NDY\SigmaDSP\ADAU1761_IC_1_REG.h
 *
 * Created:        Friday, December 1, 2023 12:12:22 PM
 * Description:    ADAU1761:IC 1 control register definitions.
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
 * Copyright ©2023 Analog Devices, Inc. All rights reserved.
 */
#ifndef __ADAU1761_IC_1_REG_H__
#define __ADAU1761_IC_1_REG_H__


/* ClkCtrlRegister  - Registers (IC 1) */
#define REG_CLKCTRLREGISTER_IC_1_ADDR             0x4000
#define REG_CLKCTRLREGISTER_IC_1_BYTE             1
#define REG_CLKCTRLREGISTER_IC_1_VALUE            0xF

/* RegPowCtrlRegister  - Registers (IC 1) */
#define REG_REGPOWCTRLREGISTER_IC_1_ADDR          0x4001
#define REG_REGPOWCTRLREGISTER_IC_1_BYTE          1
#define REG_REGPOWCTRLREGISTER_IC_1_VALUE         0x0

/* PLLCrlRegister  - Registers (IC 1) */
#define REG_PLLCRLREGISTER_IC_1_ADDR              0x4002
#define REG_PLLCRLREGISTER_IC_1_BYTE              6
#define REG_PLLCRLREGISTER_IC_1_VALUE             0x00FD000C2001

/* MicCtrlRegister  - Registers (IC 1) */
#define REG_MICCTRLREGISTER_IC_1_ADDR             0x4008
#define REG_MICCTRLREGISTER_IC_1_BYTE             1
#define REG_MICCTRLREGISTER_IC_1_VALUE            0x0

/* Record Pwr Management  - Registers (IC 1) */
#define REG_RECORD_PWR_MANAGEMENT_IC_1_ADDR       0x4009
#define REG_RECORD_PWR_MANAGEMENT_IC_1_BYTE       1
#define REG_RECORD_PWR_MANAGEMENT_IC_1_VALUE      0x0

/* Record Mixer Left Ctrl 0  - Registers (IC 1) */
#define REG_RECORD_MIXER_LEFT_CTRL_0_IC_1_ADDR    0x400A
#define REG_RECORD_MIXER_LEFT_CTRL_0_IC_1_BYTE    1
#define REG_RECORD_MIXER_LEFT_CTRL_0_IC_1_VALUE   0x1

/* Record Mixer Left Ctrl 1  - Registers (IC 1) */
#define REG_RECORD_MIXER_LEFT_CTRL_1_IC_1_ADDR    0x400B
#define REG_RECORD_MIXER_LEFT_CTRL_1_IC_1_BYTE    1
#define REG_RECORD_MIXER_LEFT_CTRL_1_IC_1_VALUE   0x5

/* Record Mixer Right Ctrl 0  - Registers (IC 1) */
#define REG_RECORD_MIXER_RIGHT_CTRL_0_IC_1_ADDR   0x400C
#define REG_RECORD_MIXER_RIGHT_CTRL_0_IC_1_BYTE   1
#define REG_RECORD_MIXER_RIGHT_CTRL_0_IC_1_VALUE  0x1

/* Record Mixer Right Ctrl 1  - Registers (IC 1) */
#define REG_RECORD_MIXER_RIGHT_CTRL_1_IC_1_ADDR   0x400D
#define REG_RECORD_MIXER_RIGHT_CTRL_1_IC_1_BYTE   1
#define REG_RECORD_MIXER_RIGHT_CTRL_1_IC_1_VALUE  0x5

/* Record Volume Ctrl Left  - Registers (IC 1) */
#define REG_RECORD_VOLUME_CTRL_LEFT_IC_1_ADDR     0x400E
#define REG_RECORD_VOLUME_CTRL_LEFT_IC_1_BYTE     1
#define REG_RECORD_VOLUME_CTRL_LEFT_IC_1_VALUE    0x0

/* Record Volume Ctrl Right  - Registers (IC 1) */
#define REG_RECORD_VOLUME_CTRL_RIGHT_IC_1_ADDR    0x400F
#define REG_RECORD_VOLUME_CTRL_RIGHT_IC_1_BYTE    1
#define REG_RECORD_VOLUME_CTRL_RIGHT_IC_1_VALUE   0x0

/* Record Mic Bias Control  - Registers (IC 1) */
#define REG_RECORD_MIC_BIAS_CONTROL_IC_1_ADDR     0x4010
#define REG_RECORD_MIC_BIAS_CONTROL_IC_1_BYTE     1
#define REG_RECORD_MIC_BIAS_CONTROL_IC_1_VALUE    0x0

/* ALC Control 0  - Registers (IC 1) */
#define REG_ALC_CONTROL_0_IC_1_ADDR               0x4011
#define REG_ALC_CONTROL_0_IC_1_BYTE               1
#define REG_ALC_CONTROL_0_IC_1_VALUE              0x0

/* ALC Control 1  - Registers (IC 1) */
#define REG_ALC_CONTROL_1_IC_1_ADDR               0x4012
#define REG_ALC_CONTROL_1_IC_1_BYTE               1
#define REG_ALC_CONTROL_1_IC_1_VALUE              0x0

/* ALC Control 2  - Registers (IC 1) */
#define REG_ALC_CONTROL_2_IC_1_ADDR               0x4013
#define REG_ALC_CONTROL_2_IC_1_BYTE               1
#define REG_ALC_CONTROL_2_IC_1_VALUE              0x0

/* ALC Control 3  - Registers (IC 1) */
#define REG_ALC_CONTROL_3_IC_1_ADDR               0x4014
#define REG_ALC_CONTROL_3_IC_1_BYTE               1
#define REG_ALC_CONTROL_3_IC_1_VALUE              0x0

/* Serial Port Control 0  - Registers (IC 1) */
#define REG_SERIAL_PORT_CONTROL_0_IC_1_ADDR       0x4015
#define REG_SERIAL_PORT_CONTROL_0_IC_1_BYTE       1
#define REG_SERIAL_PORT_CONTROL_0_IC_1_VALUE      0x0

/* Serail Port Control 1  - Registers (IC 1) */
#define REG_SERAIL_PORT_CONTROL_1_IC_1_ADDR       0x4016
#define REG_SERAIL_PORT_CONTROL_1_IC_1_BYTE       1
#define REG_SERAIL_PORT_CONTROL_1_IC_1_VALUE      0x0

/* Converter Ctrl 0  - Registers (IC 1) */
#define REG_CONVERTER_CTRL_0_IC_1_ADDR            0x4017
#define REG_CONVERTER_CTRL_0_IC_1_BYTE            1
#define REG_CONVERTER_CTRL_0_IC_1_VALUE           0x0

/* Converter Ctrl 1  - Registers (IC 1) */
#define REG_CONVERTER_CTRL_1_IC_1_ADDR            0x4018
#define REG_CONVERTER_CTRL_1_IC_1_BYTE            1
#define REG_CONVERTER_CTRL_1_IC_1_VALUE           0x0

/* ADC Control 0  - Registers (IC 1) */
#define REG_ADC_CONTROL_0_IC_1_ADDR               0x4019
#define REG_ADC_CONTROL_0_IC_1_BYTE               1
#define REG_ADC_CONTROL_0_IC_1_VALUE              0x13

/* ADC Control 1  - Registers (IC 1) */
#define REG_ADC_CONTROL_1_IC_1_ADDR               0x401A
#define REG_ADC_CONTROL_1_IC_1_BYTE               1
#define REG_ADC_CONTROL_1_IC_1_VALUE              0x0

/* ADC Control 2  - Registers (IC 1) */
#define REG_ADC_CONTROL_2_IC_1_ADDR               0x401B
#define REG_ADC_CONTROL_2_IC_1_BYTE               1
#define REG_ADC_CONTROL_2_IC_1_VALUE              0x0

/* Playback Mixer Left Control 0  - Registers (IC 1) */
#define REG_PLAYBACK_MIXER_LEFT_CONTROL_0_IC_1_ADDR 0x401C
#define REG_PLAYBACK_MIXER_LEFT_CONTROL_0_IC_1_BYTE 1
#define REG_PLAYBACK_MIXER_LEFT_CONTROL_0_IC_1_VALUE 0x21

/* Plaback Mixer Left Control 1  - Registers (IC 1) */
#define REG_PLABACK_MIXER_LEFT_CONTROL_1_IC_1_ADDR 0x401D
#define REG_PLABACK_MIXER_LEFT_CONTROL_1_IC_1_BYTE 1
#define REG_PLABACK_MIXER_LEFT_CONTROL_1_IC_1_VALUE 0x0

/* Plaback Mixer Right Control 0  - Registers (IC 1) */
#define REG_PLABACK_MIXER_RIGHT_CONTROL_0_IC_1_ADDR 0x401E
#define REG_PLABACK_MIXER_RIGHT_CONTROL_0_IC_1_BYTE 1
#define REG_PLABACK_MIXER_RIGHT_CONTROL_0_IC_1_VALUE 0x41

/* Playback Mixer Right Control 1  - Registers (IC 1) */
#define REG_PLAYBACK_MIXER_RIGHT_CONTROL_1_IC_1_ADDR 0x401F
#define REG_PLAYBACK_MIXER_RIGHT_CONTROL_1_IC_1_BYTE 1
#define REG_PLAYBACK_MIXER_RIGHT_CONTROL_1_IC_1_VALUE 0x0

/* Playback LR Left  - Registers (IC 1) */
#define REG_PLAYBACK_LR_LEFT_IC_1_ADDR            0x4020
#define REG_PLAYBACK_LR_LEFT_IC_1_BYTE            1
#define REG_PLAYBACK_LR_LEFT_IC_1_VALUE           0x0

/* Playback LR Right  - Registers (IC 1) */
#define REG_PLAYBACK_LR_RIGHT_IC_1_ADDR           0x4021
#define REG_PLAYBACK_LR_RIGHT_IC_1_BYTE           1
#define REG_PLAYBACK_LR_RIGHT_IC_1_VALUE          0x0

/* Playback LR Mono Ctrl  - Registers (IC 1) */
#define REG_PLAYBACK_LR_MONO_CTRL_IC_1_ADDR       0x4022
#define REG_PLAYBACK_LR_MONO_CTRL_IC_1_BYTE       1
#define REG_PLAYBACK_LR_MONO_CTRL_IC_1_VALUE      0x1

/* Playback Headphone Left  - Registers (IC 1) */
#define REG_PLAYBACK_HEADPHONE_LEFT_IC_1_ADDR     0x4023
#define REG_PLAYBACK_HEADPHONE_LEFT_IC_1_BYTE     1
#define REG_PLAYBACK_HEADPHONE_LEFT_IC_1_VALUE    0xE7

/* Playback Headphone Right  - Registers (IC 1) */
#define REG_PLAYBACK_HEADPHONE_RIGHT_IC_1_ADDR    0x4024
#define REG_PLAYBACK_HEADPHONE_RIGHT_IC_1_BYTE    1
#define REG_PLAYBACK_HEADPHONE_RIGHT_IC_1_VALUE   0xE7

/* Playback Line Out Left  - Registers (IC 1) */
#define REG_PLAYBACK_LINE_OUT_LEFT_IC_1_ADDR      0x4025
#define REG_PLAYBACK_LINE_OUT_LEFT_IC_1_BYTE      1
#define REG_PLAYBACK_LINE_OUT_LEFT_IC_1_VALUE     0x0

/* Playback Line Out Right  - Registers (IC 1) */
#define REG_PLAYBACK_LINE_OUT_RIGHT_IC_1_ADDR     0x4026
#define REG_PLAYBACK_LINE_OUT_RIGHT_IC_1_BYTE     1
#define REG_PLAYBACK_LINE_OUT_RIGHT_IC_1_VALUE    0x0

/* Playback Line Out Mono  - Registers (IC 1) */
#define REG_PLAYBACK_LINE_OUT_MONO_IC_1_ADDR      0x4027
#define REG_PLAYBACK_LINE_OUT_MONO_IC_1_BYTE      1
#define REG_PLAYBACK_LINE_OUT_MONO_IC_1_VALUE     0xE5

/* Playback Control  - Registers (IC 1) */
#define REG_PLAYBACK_CONTROL_IC_1_ADDR            0x4028
#define REG_PLAYBACK_CONTROL_IC_1_BYTE            1
#define REG_PLAYBACK_CONTROL_IC_1_VALUE           0x0

/* Playback Power Management  - Registers (IC 1) */
#define REG_PLAYBACK_POWER_MANAGEMENT_IC_1_ADDR   0x4029
#define REG_PLAYBACK_POWER_MANAGEMENT_IC_1_BYTE   1
#define REG_PLAYBACK_POWER_MANAGEMENT_IC_1_VALUE  0x3

/* DAC Control 0  - Registers (IC 1) */
#define REG_DAC_CONTROL_0_IC_1_ADDR               0x402A
#define REG_DAC_CONTROL_0_IC_1_BYTE               1
#define REG_DAC_CONTROL_0_IC_1_VALUE              0x3

/* DAC Control 1  - Registers (IC 1) */
#define REG_DAC_CONTROL_1_IC_1_ADDR               0x402B
#define REG_DAC_CONTROL_1_IC_1_BYTE               1
#define REG_DAC_CONTROL_1_IC_1_VALUE              0x0

/* DAC Control 2  - Registers (IC 1) */
#define REG_DAC_CONTROL_2_IC_1_ADDR               0x402C
#define REG_DAC_CONTROL_2_IC_1_BYTE               1
#define REG_DAC_CONTROL_2_IC_1_VALUE              0x0

/* Serial Port Pad Control 0  - Registers (IC 1) */
#define REG_SERIAL_PORT_PAD_CONTROL_0_IC_1_ADDR   0x402D
#define REG_SERIAL_PORT_PAD_CONTROL_0_IC_1_BYTE   1
#define REG_SERIAL_PORT_PAD_CONTROL_0_IC_1_VALUE  0xAA

/* Comm Port Pad Ctrl 0  - Registers (IC 1) */
#define REG_COMM_PORT_PAD_CTRL_0_IC_1_ADDR        0x402F
#define REG_COMM_PORT_PAD_CTRL_0_IC_1_BYTE        1
#define REG_COMM_PORT_PAD_CTRL_0_IC_1_VALUE       0xAA

/* Comm Port Pad Ctrl 1  - Registers (IC 1) */
#define REG_COMM_PORT_PAD_CTRL_1_IC_1_ADDR        0x4030
#define REG_COMM_PORT_PAD_CTRL_1_IC_1_BYTE        1
#define REG_COMM_PORT_PAD_CTRL_1_IC_1_VALUE       0x0

/* JackRegister  - Registers (IC 1) */
#define REG_JACKREGISTER_IC_1_ADDR                0x4031
#define REG_JACKREGISTER_IC_1_BYTE                1
#define REG_JACKREGISTER_IC_1_VALUE               0x8

/* Dejitter Register Control  - Registers (IC 1) */
#define REG_DEJITTER_REGISTER_CONTROL_IC_1_ADDR   0x4036
#define REG_DEJITTER_REGISTER_CONTROL_IC_1_BYTE   1
#define REG_DEJITTER_REGISTER_CONTROL_IC_1_VALUE  0x3

/* CRC Ideal_1  - Registers (IC 1) */
#define REG_CRC_IDEAL_1_IC_1_ADDR                 0x40C0
#define REG_CRC_IDEAL_1_IC_1_BYTE                 1
#define REG_CRC_IDEAL_1_IC_1_VALUE                0x7F

/* CRC Ideal_2  - Registers (IC 1) */
#define REG_CRC_IDEAL_2_IC_1_ADDR                 0x40C1
#define REG_CRC_IDEAL_2_IC_1_BYTE                 1
#define REG_CRC_IDEAL_2_IC_1_VALUE                0x7F

/* CRC Ideal_3  - Registers (IC 1) */
#define REG_CRC_IDEAL_3_IC_1_ADDR                 0x40C2
#define REG_CRC_IDEAL_3_IC_1_BYTE                 1
#define REG_CRC_IDEAL_3_IC_1_VALUE                0x67

/* CRC Ideal_4  - Registers (IC 1) */
#define REG_CRC_IDEAL_4_IC_1_ADDR                 0x40C3
#define REG_CRC_IDEAL_4_IC_1_BYTE                 1
#define REG_CRC_IDEAL_4_IC_1_VALUE                0x61

/* CRC Enable  - Registers (IC 1) */
#define REG_CRC_ENABLE_IC_1_ADDR                  0x40C4
#define REG_CRC_ENABLE_IC_1_BYTE                  1
#define REG_CRC_ENABLE_IC_1_VALUE                 0x1

/* GPIO 0 Control  - Registers (IC 1) */
#define REG_GPIO_0_CONTROL_IC_1_ADDR              0x40C6
#define REG_GPIO_0_CONTROL_IC_1_BYTE              1
#define REG_GPIO_0_CONTROL_IC_1_VALUE             0x0

/* GPIO 1 Control  - Registers (IC 1) */
#define REG_GPIO_1_CONTROL_IC_1_ADDR              0x40C7
#define REG_GPIO_1_CONTROL_IC_1_BYTE              1
#define REG_GPIO_1_CONTROL_IC_1_VALUE             0x0

/* GPIO 2 Control  - Registers (IC 1) */
#define REG_GPIO_2_CONTROL_IC_1_ADDR              0x40C8
#define REG_GPIO_2_CONTROL_IC_1_BYTE              1
#define REG_GPIO_2_CONTROL_IC_1_VALUE             0x0

/* GPIO 3 Control  - Registers (IC 1) */
#define REG_GPIO_3_CONTROL_IC_1_ADDR              0x40C9
#define REG_GPIO_3_CONTROL_IC_1_BYTE              1
#define REG_GPIO_3_CONTROL_IC_1_VALUE             0x0

/* Watchdog_Enable  - Registers (IC 1) */
#define REG_WATCHDOG_ENABLE_IC_1_ADDR             0x40D0
#define REG_WATCHDOG_ENABLE_IC_1_BYTE             1
#define REG_WATCHDOG_ENABLE_IC_1_VALUE            0x0

/* Watchdog Register Value 1  - Registers (IC 1) */
#define REG_WATCHDOG_REGISTER_VALUE_1_IC_1_ADDR   0x40D1
#define REG_WATCHDOG_REGISTER_VALUE_1_IC_1_BYTE   1
#define REG_WATCHDOG_REGISTER_VALUE_1_IC_1_VALUE  0x4

/* Watchdog Register Value 2  - Registers (IC 1) */
#define REG_WATCHDOG_REGISTER_VALUE_2_IC_1_ADDR   0x40D2
#define REG_WATCHDOG_REGISTER_VALUE_2_IC_1_BYTE   1
#define REG_WATCHDOG_REGISTER_VALUE_2_IC_1_VALUE  0x0

/* Watchdog Register Value 3  - Registers (IC 1) */
#define REG_WATCHDOG_REGISTER_VALUE_3_IC_1_ADDR   0x40D3
#define REG_WATCHDOG_REGISTER_VALUE_3_IC_1_BYTE   1
#define REG_WATCHDOG_REGISTER_VALUE_3_IC_1_VALUE  0x0

/* Watchdog Error  - Registers (IC 1) */
#define REG_WATCHDOG_ERROR_IC_1_ADDR              0x40D4
#define REG_WATCHDOG_ERROR_IC_1_BYTE              1
#define REG_WATCHDOG_ERROR_IC_1_VALUE             0x0

/* Non Modulo RAM 1  - Registers (IC 1) */
#define REG_NON_MODULO_RAM_1_IC_1_ADDR            0x40E9
#define REG_NON_MODULO_RAM_1_IC_1_BYTE            1
#define REG_NON_MODULO_RAM_1_IC_1_VALUE           0x10

/* Non Modulo RAM 2  - Registers (IC 1) */
#define REG_NON_MODULO_RAM_2_IC_1_ADDR            0x40EA
#define REG_NON_MODULO_RAM_2_IC_1_BYTE            1
#define REG_NON_MODULO_RAM_2_IC_1_VALUE           0x0

/* Sample Rate Setting  - Registers (IC 1) */
#define REG_SAMPLE_RATE_SETTING_IC_1_ADDR         0x40EB
#define REG_SAMPLE_RATE_SETTING_IC_1_BYTE         1
#define REG_SAMPLE_RATE_SETTING_IC_1_VALUE        0x1

/* Routing Matrix Inputs  - Registers (IC 1) */
#define REG_ROUTING_MATRIX_INPUTS_IC_1_ADDR       0x40F2
#define REG_ROUTING_MATRIX_INPUTS_IC_1_BYTE       1
#define REG_ROUTING_MATRIX_INPUTS_IC_1_VALUE      0x0

/* Routing Matrix Outputs  - Registers (IC 1) */
#define REG_ROUTING_MATRIX_OUTPUTS_IC_1_ADDR      0x40F3
#define REG_ROUTING_MATRIX_OUTPUTS_IC_1_BYTE      1
#define REG_ROUTING_MATRIX_OUTPUTS_IC_1_VALUE     0x0

/* Serial Data/GPIO Pin Config  - Registers (IC 1) */
#define REG_SERIAL_DATAGPIO_PIN_CONFIG_IC_1_ADDR  0x40F4
#define REG_SERIAL_DATAGPIO_PIN_CONFIG_IC_1_BYTE  1
#define REG_SERIAL_DATAGPIO_PIN_CONFIG_IC_1_VALUE 0x0

/* DSP Enable Register  - Registers (IC 1) */
#define REG_DSP_ENABLE_REGISTER_IC_1_ADDR         0x40F5
#define REG_DSP_ENABLE_REGISTER_IC_1_BYTE         1
#define REG_DSP_ENABLE_REGISTER_IC_1_VALUE        0x1

/* DSP Run Register  - Registers (IC 1) */
#define REG_DSP_RUN_REGISTER_IC_1_ADDR            0x40F6
#define REG_DSP_RUN_REGISTER_IC_1_BYTE            1
#define REG_DSP_RUN_REGISTER_IC_1_VALUE           0x1

/* DSP Slew Modes  - Registers (IC 1) */
#define REG_DSP_SLEW_MODES_IC_1_ADDR              0x40F7
#define REG_DSP_SLEW_MODES_IC_1_BYTE              1
#define REG_DSP_SLEW_MODES_IC_1_VALUE             0x0

/* Serial Port Sample Rate Setting  - Registers (IC 1) */
#define REG_SERIAL_PORT_SAMPLE_RATE_SETTING_IC_1_ADDR 0x40F8
#define REG_SERIAL_PORT_SAMPLE_RATE_SETTING_IC_1_BYTE 1
#define REG_SERIAL_PORT_SAMPLE_RATE_SETTING_IC_1_VALUE 0x0

/* Clock Enable Reg 0  - Registers (IC 1) */
#define REG_CLOCK_ENABLE_REG_0_IC_1_ADDR          0x40F9
#define REG_CLOCK_ENABLE_REG_0_IC_1_BYTE          1
#define REG_CLOCK_ENABLE_REG_0_IC_1_VALUE         0x7F

/* Clock Enable Reg 1  - Registers (IC 1) */
#define REG_CLOCK_ENABLE_REG_1_IC_1_ADDR          0x40FA
#define REG_CLOCK_ENABLE_REG_1_IC_1_BYTE          1
#define REG_CLOCK_ENABLE_REG_1_IC_1_VALUE         0x3


/*
 *
 * Control register's field descriptions
 *
 */

/* ClkCtrlRegister (IC 1) */
#define R0_MCLK_ENA_IC_1                          0x1    /* 1b	[0] */
#define R0_INPUT_MCLK_FREQ_IC_1                   0x3    /* 11b	[2:1] */
#define R0_CLK_SOURCE_SEL_IC_1                    0x1    /* 1b	[3] */
#define R0_MCLK_ENA_IC_1_MASK                     0x1
#define R0_MCLK_ENA_IC_1_SHIFT                    0
#define R0_INPUT_MCLK_FREQ_IC_1_MASK              0x6
#define R0_INPUT_MCLK_FREQ_IC_1_SHIFT             1
#define R0_CLK_SOURCE_SEL_IC_1_MASK               0x8
#define R0_CLK_SOURCE_SEL_IC_1_SHIFT              3

/* RegPowCtrlRegister (IC 1) */
#define R1_REGPOWCTRL_IC_1                        0x0    /* 0b	[0] */
#define R1_REGPOWCTRL_IC_1_MASK                   0x1
#define R1_REGPOWCTRL_IC_1_SHIFT                  0

/* PLLCrlRegister (IC 1) */
#define R2_PLL_POWER_DOWN_IC_1                    0x1    /* 1b	[0] */
#define R2_PLL_LOCK_IC_1                          0x0    /* 0b	[1] */
#define R2_PLL_TYPE_IC_1                          0x0    /* 0b	[8] */
#define R2_INPUT_DIVIDER_IC_1                     0x0    /* 00b	[10:9] */
#define R2_R_FEEDBACK_IC_1                        0x4    /* 0100b	[14:11] */
#define R2_N_NUMERATOR_IC_1                       0x000C /* 0000000000001100b	[31:16] */
#define R2_M_DENOMINATOR_IC_1                     0x00FD /* 0000000011111101b	[47:32] */
#define R2_PLL_POWER_DOWN_IC_1_MASK               0x1
#define R2_PLL_POWER_DOWN_IC_1_SHIFT              0
#define R2_PLL_LOCK_IC_1_MASK                     0x2
#define R2_PLL_LOCK_IC_1_SHIFT                    1
#define R2_PLL_TYPE_IC_1_MASK                     0x100
#define R2_PLL_TYPE_IC_1_SHIFT                    8
#define R2_INPUT_DIVIDER_IC_1_MASK                0x600
#define R2_INPUT_DIVIDER_IC_1_SHIFT               9
#define R2_R_FEEDBACK_IC_1_MASK                   0x7800
#define R2_R_FEEDBACK_IC_1_SHIFT                  11
#define R2_N_NUMERATOR_IC_1_MASK                  0xFFFF0000
#define R2_N_NUMERATOR_IC_1_SHIFT                 16
#define R2_M_DENOMINATOR_IC_1_MASK                0xFFFF00000000
#define R2_M_DENOMINATOR_IC_1_SHIFT               32

/* MicCtrlRegister (IC 1) */
#define R3_JACK_POLARITY_IC_1                     0x0    /* 0b	[0] */
#define R3_JACK_DETECT_MIC_IN_IC_1                0x0    /* 00b	[5:4] */
#define R3_JACK_DEBOUNCE_TIME_IC_1                0x0    /* 00b	[7:6] */
#define R3_JACK_POLARITY_IC_1_MASK                0x1
#define R3_JACK_POLARITY_IC_1_SHIFT               0
#define R3_JACK_DETECT_MIC_IN_IC_1_MASK           0x30
#define R3_JACK_DETECT_MIC_IN_IC_1_SHIFT          4
#define R3_JACK_DEBOUNCE_TIME_IC_1_MASK           0xC0
#define R3_JACK_DEBOUNCE_TIME_IC_1_SHIFT          6

/* Record Pwr Management (IC 1) */
#define R4_FRONT_END_BIAS_IC_1                    0x0    /* 00b	[2:1] */
#define R4_ADC_BIAS_CONTROL_IC_1                  0x0    /* 00b	[4:3] */
#define R4_MIXER_AMP_BOOST_IC_1                   0x0    /* 00b	[6:5] */
#define R4_FRONT_END_BIAS_IC_1_MASK               0x6
#define R4_FRONT_END_BIAS_IC_1_SHIFT              1
#define R4_ADC_BIAS_CONTROL_IC_1_MASK             0x18
#define R4_ADC_BIAS_CONTROL_IC_1_SHIFT            3
#define R4_MIXER_AMP_BOOST_IC_1_MASK              0x60
#define R4_MIXER_AMP_BOOST_IC_1_SHIFT             5

/* Record Mixer Left Ctrl 0 (IC 1) */
#define R5_ENABLE_MIXER_IC_1                      0x1    /* 1b	[0] */
#define R5_LINE_IN_N_GAIN_IC_1                    0x0    /* 000b	[3:1] */
#define R5_LINE_IN_P_GAIN_IC_1                    0x0    /* 000b	[6:4] */
#define R5_ENABLE_MIXER_IC_1_MASK                 0x1
#define R5_ENABLE_MIXER_IC_1_SHIFT                0
#define R5_LINE_IN_N_GAIN_IC_1_MASK               0xE
#define R5_LINE_IN_N_GAIN_IC_1_SHIFT              1
#define R5_LINE_IN_P_GAIN_IC_1_MASK               0x70
#define R5_LINE_IN_P_GAIN_IC_1_SHIFT              4

/* Record Mixer Left Ctrl 1 (IC 1) */
#define R6_REC_MIX_LFT_CTRL1_AUXING_IC_1          0x5    /* 101b	[2:0] */
#define R6_REC_MIX_LFT_CTRL1_PGABOOST_IC_1        0x0    /* 00b	[4:3] */
#define R6_REC_MIX_LFT_CTRL1_AUXING_IC_1_MASK     0x7
#define R6_REC_MIX_LFT_CTRL1_AUXING_IC_1_SHIFT    0
#define R6_REC_MIX_LFT_CTRL1_PGABOOST_IC_1_MASK   0x18
#define R6_REC_MIX_LFT_CTRL1_PGABOOST_IC_1_SHIFT  3

/* Record Mixer Right Ctrl 0 (IC 1) */
#define R7_RECMIX_RIGHT_CTRL0_ENABLE_IC_1         0x1    /* 1b	[0] */
#define R7_RECMIXRIGHT_CTRL0_NGAIN_IC_1           0x0    /* 000b	[3:1] */
#define R7_RECMIXRIGHT_CTRL0_PGAIN_IC_1           0x0    /* 000b	[6:4] */
#define R7_RECMIX_RIGHT_CTRL0_ENABLE_IC_1_MASK    0x1
#define R7_RECMIX_RIGHT_CTRL0_ENABLE_IC_1_SHIFT   0
#define R7_RECMIXRIGHT_CTRL0_NGAIN_IC_1_MASK      0xE
#define R7_RECMIXRIGHT_CTRL0_NGAIN_IC_1_SHIFT     1
#define R7_RECMIXRIGHT_CTRL0_PGAIN_IC_1_MASK      0x70
#define R7_RECMIXRIGHT_CTRL0_PGAIN_IC_1_SHIFT     4

/* Record Mixer Right Ctrl 1 (IC 1) */
#define R8_RECMIXRIGHT_CTRL1_AUXING_IC_1          0x5    /* 101b	[2:0] */
#define R8_RECMIXRIGHT_CTRL1_PGABOOST_IC_1        0x0    /* 00b	[4:3] */
#define R8_RECMIXRIGHT_CTRL1_AUXING_IC_1_MASK     0x7
#define R8_RECMIXRIGHT_CTRL1_AUXING_IC_1_SHIFT    0
#define R8_RECMIXRIGHT_CTRL1_PGABOOST_IC_1_MASK   0x18
#define R8_RECMIXRIGHT_CTRL1_PGABOOST_IC_1_SHIFT  3

/* Record Volume Ctrl Left (IC 1) */
#define R9_REC_VOL_CTRL_LFT_DIFPATH_IC_1          0x0    /* 0b	[0] */
#define R9_REC_VOL_CTRL_LFT_MUTE_IC_1             0x0    /* 0b	[1] */
#define R9_REC_VOL_CTRL_LFT_IC_1                  0x0    /* 000000b	[7:2] */
#define R9_REC_VOL_CTRL_LFT_DIFPATH_IC_1_MASK     0x1
#define R9_REC_VOL_CTRL_LFT_DIFPATH_IC_1_SHIFT    0
#define R9_REC_VOL_CTRL_LFT_MUTE_IC_1_MASK        0x2
#define R9_REC_VOL_CTRL_LFT_MUTE_IC_1_SHIFT       1
#define R9_REC_VOL_CTRL_LFT_IC_1_MASK             0xFC
#define R9_REC_VOL_CTRL_LFT_IC_1_SHIFT            2

/* Record Volume Ctrl Right (IC 1) */
#define R10_REC_VOL_CTRL_RGT_DIFPATH_IC_1         0x0    /* 0b	[0] */
#define R10_REC_VOL_CTRL_RGT_MUTE_IC_1            0x0    /* 0b	[1] */
#define R10_REC_VOL_CTRL_RGT_IC_1                 0x0    /* 000000b	[7:2] */
#define R10_REC_VOL_CTRL_RGT_DIFPATH_IC_1_MASK    0x1
#define R10_REC_VOL_CTRL_RGT_DIFPATH_IC_1_SHIFT   0
#define R10_REC_VOL_CTRL_RGT_MUTE_IC_1_MASK       0x2
#define R10_REC_VOL_CTRL_RGT_MUTE_IC_1_SHIFT      1
#define R10_REC_VOL_CTRL_RGT_IC_1_MASK            0xFC
#define R10_REC_VOL_CTRL_RGT_IC_1_SHIFT           2

/* Record Mic Bias Control (IC 1) */
#define R11_MIC_ENABLE_IC_1                       0x0    /* 0b	[0] */
#define R11_MIC_GAIN_IC_1                         0x0    /* 0b	[2] */
#define R11_MIC_HI_PERFORM_IC_1                   0x0    /* 0b	[3] */
#define R11_MIC_ENABLE_IC_1_MASK                  0x1
#define R11_MIC_ENABLE_IC_1_SHIFT                 0
#define R11_MIC_GAIN_IC_1_MASK                    0x4
#define R11_MIC_GAIN_IC_1_SHIFT                   2
#define R11_MIC_HI_PERFORM_IC_1_MASK              0x8
#define R11_MIC_HI_PERFORM_IC_1_SHIFT             3

/* ALC Control 0 (IC 1) */
#define R12_ALC_SELECT_IC_1                       0x0    /* 000b	[2:0] */
#define R12_MAX_ALC_VOLUME_GAIN_IC_1              0x0    /* 000b	[5:3] */
#define R12_ALC_VOLUME_SLEW_TIME_IC_1             0x0    /* 00b	[7:6] */
#define R12_ALC_SELECT_IC_1_MASK                  0x7
#define R12_ALC_SELECT_IC_1_SHIFT                 0
#define R12_MAX_ALC_VOLUME_GAIN_IC_1_MASK         0x38
#define R12_MAX_ALC_VOLUME_GAIN_IC_1_SHIFT        3
#define R12_ALC_VOLUME_SLEW_TIME_IC_1_MASK        0xC0
#define R12_ALC_VOLUME_SLEW_TIME_IC_1_SHIFT       6

/* ALC Control 1 (IC 1) */
#define R13_ALC_TARGET_IC_1                       0x0    /* 0000b	[3:0] */
#define R13_ALC_COMPRESSOR_HOLD_TIME_IC_1         0x0    /* 0000b	[7:4] */
#define R13_ALC_TARGET_IC_1_MASK                  0xF
#define R13_ALC_TARGET_IC_1_SHIFT                 0
#define R13_ALC_COMPRESSOR_HOLD_TIME_IC_1_MASK    0xF0
#define R13_ALC_COMPRESSOR_HOLD_TIME_IC_1_SHIFT   4

/* ALC Control 2 (IC 1) */
#define R14_ALC_DECAY_TIME_IC_1                   0x0    /* 0000b	[3:0] */
#define R14_ALC_COMPRESSOR_ATTACK_TIME_IC_1       0x0    /* 0000b	[7:4] */
#define R14_ALC_DECAY_TIME_IC_1_MASK              0xF
#define R14_ALC_DECAY_TIME_IC_1_SHIFT             0
#define R14_ALC_COMPRESSOR_ATTACK_TIME_IC_1_MASK  0xF0
#define R14_ALC_COMPRESSOR_ATTACK_TIME_IC_1_SHIFT 4

/* ALC Control 3 (IC 1) */
#define R15_NOISE_GATE_THRESHOLD_IC_1             0x0    /* 00000b	[4:0] */
#define R15_NOISE_GATE_FUNCTION_IC_1              0x0    /* 0b	[5] */
#define R15_NOISE_GATE_TYPE_IC_1                  0x0    /* 00b	[7:6] */
#define R15_NOISE_GATE_THRESHOLD_IC_1_MASK        0x1F
#define R15_NOISE_GATE_THRESHOLD_IC_1_SHIFT       0
#define R15_NOISE_GATE_FUNCTION_IC_1_MASK         0x20
#define R15_NOISE_GATE_FUNCTION_IC_1_SHIFT        5
#define R15_NOISE_GATE_TYPE_IC_1_MASK             0xC0
#define R15_NOISE_GATE_TYPE_IC_1_SHIFT            6

/* Serial Port Control 0 (IC 1) */
#define R16_SP_CTRL0_SDBUSMSTRMD_IC_1             0x0    /* 0b	[0] */
#define R16_SP_CTRL0_CHANFRM_IC_1                 0x0    /* 00b	[2:1] */
#define R16_SP_CTRL0_LRCLK_POL_IC_1               0x0    /* 0b	[3] */
#define R16_SP_CTRL0_BCLK_IC_1                    0x0    /* 0b	[4] */
#define R16_SP_CTRL0_LRCLK_IC_1                   0x0    /* 0b	[5] */
#define R16_DITHER_ENABLE_IC_1                    0x0    /* 0b	[7] */
#define R16_SP_CTRL0_SDBUSMSTRMD_IC_1_MASK        0x1
#define R16_SP_CTRL0_SDBUSMSTRMD_IC_1_SHIFT       0
#define R16_SP_CTRL0_CHANFRM_IC_1_MASK            0x6
#define R16_SP_CTRL0_CHANFRM_IC_1_SHIFT           1
#define R16_SP_CTRL0_LRCLK_POL_IC_1_MASK          0x8
#define R16_SP_CTRL0_LRCLK_POL_IC_1_SHIFT         3
#define R16_SP_CTRL0_BCLK_IC_1_MASK               0x10
#define R16_SP_CTRL0_BCLK_IC_1_SHIFT              4
#define R16_SP_CTRL0_LRCLK_IC_1_MASK              0x20
#define R16_SP_CTRL0_LRCLK_IC_1_SHIFT             5
#define R16_DITHER_ENABLE_IC_1_MASK               0x80
#define R16_DITHER_ENABLE_IC_1_SHIFT              7

/* Serail Port Control 1 (IC 1) */
#define R17_DATA_DELAY_FROM_LRCLK_IC_1            0x0    /* 00b	[1:0] */
#define R17_SP_CTRL1_MSB_POSITION_IC_1            0x0    /* 0b	[2] */
#define R17_SP_CTRL1_DAC_CHAN_POSITION_IC_1       0x0    /* 0b	[3] */
#define R17_SP_CTRL1_ADC_CHAN_POSITION_IC_1       0x0    /* 0b	[4] */
#define R17_SP_CTRL1_NUMBER_OF_BIT_CLK_CYCLES_IC_1 0x0   /* 000b	[7:5] */
#define R17_DATA_DELAY_FROM_LRCLK_IC_1_MASK       0x3
#define R17_DATA_DELAY_FROM_LRCLK_IC_1_SHIFT      0
#define R17_SP_CTRL1_MSB_POSITION_IC_1_MASK       0x4
#define R17_SP_CTRL1_MSB_POSITION_IC_1_SHIFT      2
#define R17_SP_CTRL1_DAC_CHAN_POSITION_IC_1_MASK  0x8
#define R17_SP_CTRL1_DAC_CHAN_POSITION_IC_1_SHIFT 3
#define R17_SP_CTRL1_ADC_CHAN_POSITION_IC_1_MASK  0x10
#define R17_SP_CTRL1_ADC_CHAN_POSITION_IC_1_SHIFT 4
#define R17_SP_CTRL1_NUMBER_OF_BIT_CLK_CYCLES_IC_1_MASK 0xE0
#define R17_SP_CTRL1_NUMBER_OF_BIT_CLK_CYCLES_IC_1_SHIFT 5

/* Converter Ctrl 0 (IC 1) */
#define R18_CONV_CTRL0_SAMPLE_RATE_IC_1           0x0    /* 000b	[2:0] */
#define R18_ADC_OVERSAMPLING_IC_1                 0x0    /* 0b	[3] */
#define R18_DAC_OVERSAMPLING_IC_1                 0x0    /* 0b	[4] */
#define R18_ON_CHIP_DAC_IC_1                      0x0    /* 00b	[6:5] */
#define R18_CONV_CTRL0_SAMPLE_RATE_IC_1_MASK      0x7
#define R18_CONV_CTRL0_SAMPLE_RATE_IC_1_SHIFT     0
#define R18_ADC_OVERSAMPLING_IC_1_MASK            0x8
#define R18_ADC_OVERSAMPLING_IC_1_SHIFT           3
#define R18_DAC_OVERSAMPLING_IC_1_MASK            0x10
#define R18_DAC_OVERSAMPLING_IC_1_SHIFT           4
#define R18_ON_CHIP_DAC_IC_1_MASK                 0x60
#define R18_ON_CHIP_DAC_IC_1_SHIFT                5

/* Converter Ctrl 1 (IC 1) */
#define R19_ON_CHIP_ADC_IC_1                      0x0    /* 00b	[1:0] */
#define R19_ON_CHIP_ADC_IC_1_MASK                 0x3
#define R19_ON_CHIP_ADC_IC_1_SHIFT                0

/* ADC Control 0 (IC 1) */
#define R20_ADC_ENABLE_IC_1                       0x3    /* 11b	[1:0] */
#define R20_DIGITAL_MIC_INPUT_IC_1                0x0    /* 0b	[2] */
#define R20_DIGITAL_MIC_SWAP_IC_1                 0x0    /* 0b	[3] */
#define R20_DIGITAL_MIC_POLARITY_IC_1             0x1    /* 1b	[4] */
#define R20_HIGH_PASS_SELECT_IC_1                 0x0    /* 0b	[5] */
#define R20_INVERT_IN_POLARITY_IC_1               0x0    /* 0b	[6] */
#define R20_ADC_ENABLE_IC_1_MASK                  0x3
#define R20_ADC_ENABLE_IC_1_SHIFT                 0
#define R20_DIGITAL_MIC_INPUT_IC_1_MASK           0x4
#define R20_DIGITAL_MIC_INPUT_IC_1_SHIFT          2
#define R20_DIGITAL_MIC_SWAP_IC_1_MASK            0x8
#define R20_DIGITAL_MIC_SWAP_IC_1_SHIFT           3
#define R20_DIGITAL_MIC_POLARITY_IC_1_MASK        0x10
#define R20_DIGITAL_MIC_POLARITY_IC_1_SHIFT       4
#define R20_HIGH_PASS_SELECT_IC_1_MASK            0x20
#define R20_HIGH_PASS_SELECT_IC_1_SHIFT           5
#define R20_INVERT_IN_POLARITY_IC_1_MASK          0x40
#define R20_INVERT_IN_POLARITY_IC_1_SHIFT         6

/* ADC Control 1 (IC 1) */
#define R21_ADC_LEFT_DIGITAL_ATTENUATOR_IC_1      0x00   /* 00000000b	[7:0] */
#define R21_ADC_LEFT_DIGITAL_ATTENUATOR_IC_1_MASK 0xFF
#define R21_ADC_LEFT_DIGITAL_ATTENUATOR_IC_1_SHIFT 0

/* ADC Control 2 (IC 1) */
#define R22_ADC_RIGHT_DIGITAL_ATTENUATOR_IC_1     0x00   /* 00000000b	[7:0] */
#define R22_ADC_RIGHT_DIGITAL_ATTENUATOR_IC_1_MASK 0xFF
#define R22_ADC_RIGHT_DIGITAL_ATTENUATOR_IC_1_SHIFT 0

/* Playback Mixer Left Control 0 (IC 1) */
#define R23_PBC0_MIX_ENABLE_IC_1                  0x1    /* 1b	[0] */
#define R23_PBC0_AUX_OUT_GAIN_IC_1                0x0    /* 0000b	[4:1] */
#define R23_PBC0_LEFT_DAC_MUTE_IC_1               0x1    /* 1b	[5] */
#define R23_PBC0_RIGHT_DAC_MUTE_IC_1              0x0    /* 0b	[6] */
#define R23_PBC0_MIX_ENABLE_IC_1_MASK             0x1
#define R23_PBC0_MIX_ENABLE_IC_1_SHIFT            0
#define R23_PBC0_AUX_OUT_GAIN_IC_1_MASK           0x1E
#define R23_PBC0_AUX_OUT_GAIN_IC_1_SHIFT          1
#define R23_PBC0_LEFT_DAC_MUTE_IC_1_MASK          0x20
#define R23_PBC0_LEFT_DAC_MUTE_IC_1_SHIFT         5
#define R23_PBC0_RIGHT_DAC_MUTE_IC_1_MASK         0x40
#define R23_PBC0_RIGHT_DAC_MUTE_IC_1_SHIFT        6

/* Plaback Mixer Left Control 1 (IC 1) */
#define R24_PBC1_LEFT_PGA_GAIN_IC_1               0x0    /* 0000b	[3:0] */
#define R24_PBC1_RIGHT_PGA_GAIN_IC_1              0x0    /* 0000b	[7:4] */
#define R24_PBC1_LEFT_PGA_GAIN_IC_1_MASK          0xF
#define R24_PBC1_LEFT_PGA_GAIN_IC_1_SHIFT         0
#define R24_PBC1_RIGHT_PGA_GAIN_IC_1_MASK         0xF0
#define R24_PBC1_RIGHT_PGA_GAIN_IC_1_SHIFT        4

/* Plaback Mixer Right Control 0 (IC 1) */
#define R25_PBMRC0_ENABLE_IC_1                    0x1    /* 1b	[0] */
#define R25_PBMRC0_AUX_GAIN_IC_1                  0x0    /* 0000b	[4:1] */
#define R25_PBMRC0_L_DAC_MUTE_IC_1                0x0    /* 0b	[5] */
#define R25_PBMRC0_R_DAC_MUTE_IC_1                0x1    /* 1b	[6] */
#define R25_PBMRC0_ENABLE_IC_1_MASK               0x1
#define R25_PBMRC0_ENABLE_IC_1_SHIFT              0
#define R25_PBMRC0_AUX_GAIN_IC_1_MASK             0x1E
#define R25_PBMRC0_AUX_GAIN_IC_1_SHIFT            1
#define R25_PBMRC0_L_DAC_MUTE_IC_1_MASK           0x20
#define R25_PBMRC0_L_DAC_MUTE_IC_1_SHIFT          5
#define R25_PBMRC0_R_DAC_MUTE_IC_1_MASK           0x40
#define R25_PBMRC0_R_DAC_MUTE_IC_1_SHIFT          6

/* Playback Mixer Right Control 1 (IC 1) */
#define R26_PBMRC1_L_PGA_OUT_GAIN_IC_1            0x0    /* 0000b	[3:0] */
#define R26_PBMRC1_R_PGA_OUT_GAIN_IC_1            0x0    /* 0000b	[7:4] */
#define R26_PBMRC1_L_PGA_OUT_GAIN_IC_1_MASK       0xF
#define R26_PBMRC1_L_PGA_OUT_GAIN_IC_1_SHIFT      0
#define R26_PBMRC1_R_PGA_OUT_GAIN_IC_1_MASK       0xF0
#define R26_PBMRC1_R_PGA_OUT_GAIN_IC_1_SHIFT      4

/* Playback LR Left (IC 1) */
#define R27_PBLR_LEFT_MIX_ENABLE_IC_1             0x0    /* 0b	[0] */
#define R27_PBLR_LEFT_LEFT_MIXER_IC_1             0x0    /* 00b	[2:1] */
#define R27_PBLR_LEFT_RIGHT_MIXER_IC_1            0x0    /* 00b	[4:3] */
#define R27_PBLR_LEFT_MIX_ENABLE_IC_1_MASK        0x1
#define R27_PBLR_LEFT_MIX_ENABLE_IC_1_SHIFT       0
#define R27_PBLR_LEFT_LEFT_MIXER_IC_1_MASK        0x6
#define R27_PBLR_LEFT_LEFT_MIXER_IC_1_SHIFT       1
#define R27_PBLR_LEFT_RIGHT_MIXER_IC_1_MASK       0x18
#define R27_PBLR_LEFT_RIGHT_MIXER_IC_1_SHIFT      3

/* Playback LR Right (IC 1) */
#define R28_PBLR_RIGHT_MIX_ENABLE_IC_1            0x0    /* 0b	[0] */
#define R28_PBLR_RIGHT_LEFT_MIXER_IC_1            0x0    /* 00b	[2:1] */
#define R28_PBLR_RIGHT_RIGHT_MIXER_IC_1           0x0    /* 00b	[4:3] */
#define R28_PBLR_RIGHT_MIX_ENABLE_IC_1_MASK       0x1
#define R28_PBLR_RIGHT_MIX_ENABLE_IC_1_SHIFT      0
#define R28_PBLR_RIGHT_LEFT_MIXER_IC_1_MASK       0x6
#define R28_PBLR_RIGHT_LEFT_MIXER_IC_1_SHIFT      1
#define R28_PBLR_RIGHT_RIGHT_MIXER_IC_1_MASK      0x18
#define R28_PBLR_RIGHT_RIGHT_MIXER_IC_1_SHIFT     3

/* Playback LR Mono Ctrl (IC 1) */
#define R29_PBMONO_MIX_ENABLE_IC_1                0x1    /* 1b	[0] */
#define R29_PBMONO_LEFT_AND_RIGHT_MIXER_IC_1      0x0    /* 00b	[2:1] */
#define R29_PBMONO_MIX_ENABLE_IC_1_MASK           0x1
#define R29_PBMONO_MIX_ENABLE_IC_1_SHIFT          0
#define R29_PBMONO_LEFT_AND_RIGHT_MIXER_IC_1_MASK 0x6
#define R29_PBMONO_LEFT_AND_RIGHT_MIXER_IC_1_SHIFT 1

/* Playback Headphone Left (IC 1) */
#define R30_PB_HEADPHONE_LEFT_PWRUP_IC_1          0x1    /* 1b	[0] */
#define R30_PB_HEADPHONE_LEFT_MUTE_IC_1           0x1    /* 1b	[1] */
#define R30_PB_HEADPHONE_LEFT_VOL_IC_1            0x39   /* 111001b	[7:2] */
#define R30_PB_HEADPHONE_LEFT_PWRUP_IC_1_MASK     0x1
#define R30_PB_HEADPHONE_LEFT_PWRUP_IC_1_SHIFT    0
#define R30_PB_HEADPHONE_LEFT_MUTE_IC_1_MASK      0x2
#define R30_PB_HEADPHONE_LEFT_MUTE_IC_1_SHIFT     1
#define R30_PB_HEADPHONE_LEFT_VOL_IC_1_MASK       0xFC
#define R30_PB_HEADPHONE_LEFT_VOL_IC_1_SHIFT      2

/* Playback Headphone Right (IC 1) */
#define R31_PB_HEADPHONE_RIGHT_PWRUP_IC_1         0x1    /* 1b	[0] */
#define R31_PB_HEADPHONE_RIGHT_MUTE_IC_1          0x1    /* 1b	[1] */
#define R31_PB_HEADPHONE_RIGHT_VOL_IC_1           0x39   /* 111001b	[7:2] */
#define R31_PB_HEADPHONE_RIGHT_PWRUP_IC_1_MASK    0x1
#define R31_PB_HEADPHONE_RIGHT_PWRUP_IC_1_SHIFT   0
#define R31_PB_HEADPHONE_RIGHT_MUTE_IC_1_MASK     0x2
#define R31_PB_HEADPHONE_RIGHT_MUTE_IC_1_SHIFT    1
#define R31_PB_HEADPHONE_RIGHT_VOL_IC_1_MASK      0xFC
#define R31_PB_HEADPHONE_RIGHT_VOL_IC_1_SHIFT     2

/* Playback Line Out Left (IC 1) */
#define R32_PB_LEFT_PWRUP_IC_1                    0x0    /* 0b	[0] */
#define R32_PB_LEFT_MUTE_IC_1                     0x0    /* 0b	[1] */
#define R32_PB_LEFT_VOL_IC_1                      0x0    /* 000000b	[7:2] */
#define R32_PB_LEFT_PWRUP_IC_1_MASK               0x1
#define R32_PB_LEFT_PWRUP_IC_1_SHIFT              0
#define R32_PB_LEFT_MUTE_IC_1_MASK                0x2
#define R32_PB_LEFT_MUTE_IC_1_SHIFT               1
#define R32_PB_LEFT_VOL_IC_1_MASK                 0xFC
#define R32_PB_LEFT_VOL_IC_1_SHIFT                2

/* Playback Line Out Right (IC 1) */
#define R33_PB_RIGHT_PWRUP_IC_1                   0x0    /* 0b	[0] */
#define R33_PB_RIGHT_MUTE_IC_1                    0x0    /* 0b	[1] */
#define R33_PB_RIGHT_VOL_IC_1                     0x0    /* 000000b	[7:2] */
#define R33_PB_RIGHT_PWRUP_IC_1_MASK              0x1
#define R33_PB_RIGHT_PWRUP_IC_1_SHIFT             0
#define R33_PB_RIGHT_MUTE_IC_1_MASK               0x2
#define R33_PB_RIGHT_MUTE_IC_1_SHIFT              1
#define R33_PB_RIGHT_VOL_IC_1_MASK                0xFC
#define R33_PB_RIGHT_VOL_IC_1_SHIFT               2

/* Playback Line Out Mono (IC 1) */
#define R34_PB_MONO_PWRUP_IC_1                    0x1    /* 1b	[0] */
#define R34_PB_MONO_MUTE_IC_1                     0x0    /* 0b	[1] */
#define R34_PB_MONO_VOL_IC_1                      0x39   /* 111001b	[7:2] */
#define R34_PB_MONO_PWRUP_IC_1_MASK               0x1
#define R34_PB_MONO_PWRUP_IC_1_SHIFT              0
#define R34_PB_MONO_MUTE_IC_1_MASK                0x2
#define R34_PB_MONO_MUTE_IC_1_SHIFT               1
#define R34_PB_MONO_VOL_IC_1_MASK                 0xFC
#define R34_PB_MONO_VOL_IC_1_SHIFT                2

/* Playback Control (IC 1) */
#define R35_OBSOLETE_BIT_IC_1                     0x0    /* 0b	[0] */
#define R35_PLAYBACK_ANALOG_VOL_SLEW_RATE_IC_1    0x0    /* 00b	[2:1] */
#define R35_POPLESS_IC_1                          0x0    /* 0b	[3] */
#define R35_POP_MODE_IC_1                         0x0    /* 0b	[4] */
#define R35_OBSOLETE_BIT_IC_1_MASK                0x1
#define R35_OBSOLETE_BIT_IC_1_SHIFT               0
#define R35_PLAYBACK_ANALOG_VOL_SLEW_RATE_IC_1_MASK 0x6
#define R35_PLAYBACK_ANALOG_VOL_SLEW_RATE_IC_1_SHIFT 1
#define R35_POPLESS_IC_1_MASK                     0x8
#define R35_POPLESS_IC_1_SHIFT                    3
#define R35_POP_MODE_IC_1_MASK                    0x10
#define R35_POP_MODE_IC_1_SHIFT                   4

/* Playback Power Management (IC 1) */
#define R36_PB_BCKEND_LEFT_ENABLE_IC_1            0x1    /* 1b	[0] */
#define R36_PB_BCKEND_RIGHT_ENABLE_IC_1           0x1    /* 1b	[1] */
#define R36_PB_BACK_CTRL_IC_1                     0x0    /* 00b	[3:2] */
#define R36_PB_DAC_BIAS_IC_1                      0x0    /* 00b	[5:4] */
#define R36_PB_HP_BIAS_IC_1                       0x0    /* 00b	[7:6] */
#define R36_PB_BCKEND_LEFT_ENABLE_IC_1_MASK       0x1
#define R36_PB_BCKEND_LEFT_ENABLE_IC_1_SHIFT      0
#define R36_PB_BCKEND_RIGHT_ENABLE_IC_1_MASK      0x2
#define R36_PB_BCKEND_RIGHT_ENABLE_IC_1_SHIFT     1
#define R36_PB_BACK_CTRL_IC_1_MASK                0xC
#define R36_PB_BACK_CTRL_IC_1_SHIFT               2
#define R36_PB_DAC_BIAS_IC_1_MASK                 0x30
#define R36_PB_DAC_BIAS_IC_1_SHIFT                4
#define R36_PB_HP_BIAS_IC_1_MASK                  0xC0
#define R36_PB_HP_BIAS_IC_1_SHIFT                 6

/* DAC Control 0 (IC 1) */
#define R37_DACC0_ENABLE_IC_1                     0x3    /* 11b	[1:0] */
#define R37_DACC0_DE_EMPH_FILTER_ENA_IC_1         0x0    /* 0b	[2] */
#define R37_DACC0_LEFT_DIGITAL_MUTE_IC_1          0x0    /* 0b	[3] */
#define R37_DACC0_RIGHT_DIGITAL_MUTE_IC_1         0x0    /* 0b	[4] */
#define R37_DACC0_INV_INPUT_POL_IC_1              0x0    /* 0b	[5] */
#define R37_DACC0_MONO_MODE_IC_1                  0x0    /* 00b	[7:6] */
#define R37_DACC0_ENABLE_IC_1_MASK                0x3
#define R37_DACC0_ENABLE_IC_1_SHIFT               0
#define R37_DACC0_DE_EMPH_FILTER_ENA_IC_1_MASK    0x4
#define R37_DACC0_DE_EMPH_FILTER_ENA_IC_1_SHIFT   2
#define R37_DACC0_LEFT_DIGITAL_MUTE_IC_1_MASK     0x8
#define R37_DACC0_LEFT_DIGITAL_MUTE_IC_1_SHIFT    3
#define R37_DACC0_RIGHT_DIGITAL_MUTE_IC_1_MASK    0x10
#define R37_DACC0_RIGHT_DIGITAL_MUTE_IC_1_SHIFT   4
#define R37_DACC0_INV_INPUT_POL_IC_1_MASK         0x20
#define R37_DACC0_INV_INPUT_POL_IC_1_SHIFT        5
#define R37_DACC0_MONO_MODE_IC_1_MASK             0xC0
#define R37_DACC0_MONO_MODE_IC_1_SHIFT            6

/* DAC Control 1 (IC 1) */
#define R38_DACC1_ADC_LEFT_CHANNEL_IC_1           0x00   /* 00000000b	[7:0] */
#define R38_DACC1_ADC_LEFT_CHANNEL_IC_1_MASK      0xFF
#define R38_DACC1_ADC_LEFT_CHANNEL_IC_1_SHIFT     0

/* DAC Control 2 (IC 1) */
#define R39_DACC2_ADC_RIGHT_CHANNEL_IC_1          0x00   /* 00000000b	[7:0] */
#define R39_DACC2_ADC_RIGHT_CHANNEL_IC_1_MASK     0xFF
#define R39_DACC2_ADC_RIGHT_CHANNEL_IC_1_SHIFT    0

/* Serial Port Pad Control 0 (IC 1) */
#define R40_BCLKP_IC_1                            0x2    /* 10b	[1:0] */
#define R40_LRCLKP_IC_1                           0x2    /* 10b	[3:2] */
#define R40_DACSDP_IC_1                           0x2    /* 10b	[5:4] */
#define R40_ADCSDP_IC_1                           0x2    /* 10b	[7:6] */
#define R40_BCLKP_IC_1_MASK                       0x3
#define R40_BCLKP_IC_1_SHIFT                      0
#define R40_LRCLKP_IC_1_MASK                      0xC
#define R40_LRCLKP_IC_1_SHIFT                     2
#define R40_DACSDP_IC_1_MASK                      0x30
#define R40_DACSDP_IC_1_SHIFT                     4
#define R40_ADCSDP_IC_1_MASK                      0xC0
#define R40_ADCSDP_IC_1_SHIFT                     6

/* Comm Port Pad Ctrl 0 (IC 1) */
#define R41_SDAP_PULL_UP_DOWN_IC_1                0x2    /* 10b	[1:0] */
#define R41_SCLP_PULL_UP_DOWN_IC_1                0x2    /* 10b	[3:2] */
#define R41_CLATCH_PULL_UP_DOWN_IC_1              0x2    /* 10b	[5:4] */
#define R41_CDATA_PULL_UP_DOWN_IC_1               0x2    /* 10b	[7:6] */
#define R41_SDAP_PULL_UP_DOWN_IC_1_MASK           0x3
#define R41_SDAP_PULL_UP_DOWN_IC_1_SHIFT          0
#define R41_SCLP_PULL_UP_DOWN_IC_1_MASK           0xC
#define R41_SCLP_PULL_UP_DOWN_IC_1_SHIFT          2
#define R41_CLATCH_PULL_UP_DOWN_IC_1_MASK         0x30
#define R41_CLATCH_PULL_UP_DOWN_IC_1_SHIFT        4
#define R41_CDATA_PULL_UP_DOWN_IC_1_MASK          0xC0
#define R41_CDATA_PULL_UP_DOWN_IC_1_SHIFT         6

/* Comm Port Pad Ctrl 1 (IC 1) */
#define R42_CPPC1_SDACOUT_IC_1                    0x0    /* 0b	[0] */
#define R42_CPPC1_SDACOUT_IC_1_MASK               0x1
#define R42_CPPC1_SDACOUT_IC_1_SHIFT              0

/* JackRegister (IC 1) */
#define R43_MCLK_JACK_PULLUP_IC_1                 0x2    /* 10b	[3:2] */
#define R43_MCLK_JACK_STRENGTH_IC_1               0x0    /* 0b	[5] */
#define R43_MCLK_JACK_PULLUP_IC_1_MASK            0xC
#define R43_MCLK_JACK_PULLUP_IC_1_SHIFT           2
#define R43_MCLK_JACK_STRENGTH_IC_1_MASK          0x20
#define R43_MCLK_JACK_STRENGTH_IC_1_SHIFT         5

/* Dejitter Register Control (IC 1) */
#define R44_DEJITTER_IC_1                         0x03   /* 00000011b	[7:0] */
#define R44_DEJITTER_IC_1_MASK                    0xFF
#define R44_DEJITTER_IC_1_SHIFT                   0

/* CRC Ideal_1 (IC 1) */
#define R45_CRC_IDEAL_1_IC_1                      0x7F   /* 01111111b	[7:0] */
#define R45_CRC_IDEAL_1_IC_1_MASK                 0xFF
#define R45_CRC_IDEAL_1_IC_1_SHIFT                0

/* CRC Ideal_2 (IC 1) */
#define R46_CRC_IDEAL_2_IC_1                      0x7F   /* 01111111b	[7:0] */
#define R46_CRC_IDEAL_2_IC_1_MASK                 0xFF
#define R46_CRC_IDEAL_2_IC_1_SHIFT                0

/* CRC Ideal_3 (IC 1) */
#define R47_CRC_IDEAL_3_IC_1                      0x67   /* 01100111b	[7:0] */
#define R47_CRC_IDEAL_3_IC_1_MASK                 0xFF
#define R47_CRC_IDEAL_3_IC_1_SHIFT                0

/* CRC Ideal_4 (IC 1) */
#define R48_CRC_IDEAL_4_IC_1                      0x61   /* 01100001b	[7:0] */
#define R48_CRC_IDEAL_4_IC_1_MASK                 0xFF
#define R48_CRC_IDEAL_4_IC_1_SHIFT                0

/* CRC Enable (IC 1) */
#define R49_CRC_ENABLE_IC_1                       0x1    /* 1b	[0] */
#define R49_CRC_ENABLE_IC_1_MASK                  0x1
#define R49_CRC_ENABLE_IC_1_SHIFT                 0

/* GPIO 0 Control (IC 1) */
#define R50_GPIO_0_PIN_FUNCTION_IC_1              0x0    /* 0000b	[3:0] */
#define R50_GPIO_0_PIN_FUNCTION_IC_1_MASK         0xF
#define R50_GPIO_0_PIN_FUNCTION_IC_1_SHIFT        0

/* GPIO 1 Control (IC 1) */
#define R51_GPIO_1_PIN_FUNCTION_IC_1              0x0    /* 0000b	[3:0] */
#define R51_GPIO_1_PIN_FUNCTION_IC_1_MASK         0xF
#define R51_GPIO_1_PIN_FUNCTION_IC_1_SHIFT        0

/* GPIO 2 Control (IC 1) */
#define R52_GPIO_2_PIN_FUNCTION_IC_1              0x0    /* 0000b	[3:0] */
#define R52_GPIO_2_PIN_FUNCTION_IC_1_MASK         0xF
#define R52_GPIO_2_PIN_FUNCTION_IC_1_SHIFT        0

/* GPIO 3 Control (IC 1) */
#define R53_GPIO_3_PIN_FUNCTION_IC_1              0x0    /* 0000b	[3:0] */
#define R53_GPIO_3_PIN_FUNCTION_IC_1_MASK         0xF
#define R53_GPIO_3_PIN_FUNCTION_IC_1_SHIFT        0

/* Watchdog_Enable (IC 1) */
#define R54_WATCHDOG_ENABLE_IC_1                  0x0    /* 0b	[0] */
#define R54_WATCHDOG_ENABLE_IC_1_MASK             0x1
#define R54_WATCHDOG_ENABLE_IC_1_SHIFT            0

/* Watchdog Register Value 1 (IC 1) */
#define R55_WATCHDOG_VALUE_1_IC_1                 0x04   /* 00000100b	[7:0] */
#define R55_WATCHDOG_VALUE_1_IC_1_MASK            0xFF
#define R55_WATCHDOG_VALUE_1_IC_1_SHIFT           0

/* Watchdog Register Value 2 (IC 1) */
#define R56_WATCHDOG_VALUE_2_IC_1                 0x00   /* 00000000b	[7:0] */
#define R56_WATCHDOG_VALUE_2_IC_1_MASK            0xFF
#define R56_WATCHDOG_VALUE_2_IC_1_SHIFT           0

/* Watchdog Register Value 3 (IC 1) */
#define R57_WATCHDOG_VALUE_3_IC_1                 0x00   /* 00000000b	[7:0] */
#define R57_WATCHDOG_VALUE_3_IC_1_MASK            0xFF
#define R57_WATCHDOG_VALUE_3_IC_1_SHIFT           0

/* Watchdog Error (IC 1) */
#define R58_WATCHDOG_ERROR_STICKY_IC_1            0x0    /* 0b	[0] */
#define R58_WATCHDOG_ERROR_STICKY_IC_1_MASK       0x1
#define R58_WATCHDOG_ERROR_STICKY_IC_1_SHIFT      0

/* Non Modulo RAM 1 (IC 1) */
#define R59_NON_MODULO_1_IC_1                     0x10   /* 00010000b	[7:0] */
#define R59_NON_MODULO_1_IC_1_MASK                0xFF
#define R59_NON_MODULO_1_IC_1_SHIFT               0

/* Non Modulo RAM 2 (IC 1) */
#define R60_NON_MODULO_2_IC_1                     0x00   /* 00000000b	[7:0] */
#define R60_NON_MODULO_2_IC_1_MASK                0xFF
#define R60_NON_MODULO_2_IC_1_SHIFT               0

/* Sample Rate Setting (IC 1) */
#define R61_CORE_DSP_SAMPLING_RATE_IC_1           0x1    /* 0001b	[3:0] */
#define R61_CORE_DSP_SAMPLING_RATE_IC_1_MASK      0xF
#define R61_CORE_DSP_SAMPLING_RATE_IC_1_SHIFT     0

/* Routing Matrix Inputs (IC 1) */
#define R62_ROUTING_MATRIX_IN_IC_1                0x0    /* 0000b	[3:0] */
#define R62_ROUTING_MATRIX_IN_IC_1_MASK           0xF
#define R62_ROUTING_MATRIX_IN_IC_1_SHIFT          0

/* Routing Matrix Outputs (IC 1) */
#define R63_ROUTING_MATRIX_OUT_IC_1               0x0    /* 0000b	[3:0] */
#define R63_ROUTING_MATRIX_OUT_IC_1_MASK          0xF
#define R63_ROUTING_MATRIX_OUT_IC_1_SHIFT         0

/* Serial Data/GPIO Pin Config (IC 1) */
#define R64_GPIO_1_ENABLE_IC_1                    0x0    /* 0b	[0] */
#define R64_GPIO_0_ENABLE_IC_1                    0x0    /* 0b	[1] */
#define R64_GPIO_2_ENABLE_IC_1                    0x0    /* 0b	[2] */
#define R64_GPIO_3_ENABLE_IC_1                    0x0    /* 0b	[3] */
#define R64_GPIO_1_ENABLE_IC_1_MASK               0x1
#define R64_GPIO_1_ENABLE_IC_1_SHIFT              0
#define R64_GPIO_0_ENABLE_IC_1_MASK               0x2
#define R64_GPIO_0_ENABLE_IC_1_SHIFT              1
#define R64_GPIO_2_ENABLE_IC_1_MASK               0x4
#define R64_GPIO_2_ENABLE_IC_1_SHIFT              2
#define R64_GPIO_3_ENABLE_IC_1_MASK               0x8
#define R64_GPIO_3_ENABLE_IC_1_SHIFT              3

/* DSP Enable Register (IC 1) */
#define R65_DSP_ENABLE_IC_1                       0x1    /* 1b	[0] */
#define R65_DSP_ENABLE_IC_1_MASK                  0x1
#define R65_DSP_ENABLE_IC_1_SHIFT                 0

/* DSP Run Register (IC 1) */
#define R66_DSP_RUN_IC_1                          0x1    /* 1b	[0] */
#define R66_DSP_RUN_IC_1_MASK                     0x1
#define R66_DSP_RUN_IC_1_SHIFT                    0

/* DSP Slew Modes (IC 1) */
#define R67_HEADPHONE_LEFT_IC_1                   0x0    /* 0b	[0] */
#define R67_HEADPHONE_RIGHT_IC_1                  0x0    /* 0b	[1] */
#define R67_LINE_OUT_LEFT_IC_1                    0x0    /* 0b	[2] */
#define R67_LINE_OUT_RIGHT_IC_1                   0x0    /* 0b	[3] */
#define R67_MONO_IC_1                             0x0    /* 0b	[4] */
#define R67_HEADPHONE_LEFT_IC_1_MASK              0x1
#define R67_HEADPHONE_LEFT_IC_1_SHIFT             0
#define R67_HEADPHONE_RIGHT_IC_1_MASK             0x2
#define R67_HEADPHONE_RIGHT_IC_1_SHIFT            1
#define R67_LINE_OUT_LEFT_IC_1_MASK               0x4
#define R67_LINE_OUT_LEFT_IC_1_SHIFT              2
#define R67_LINE_OUT_RIGHT_IC_1_MASK              0x8
#define R67_LINE_OUT_RIGHT_IC_1_SHIFT             3
#define R67_MONO_IC_1_MASK                        0x10
#define R67_MONO_IC_1_SHIFT                       4

/* Serial Port Sample Rate Setting (IC 1) */
#define R68_SERIAL_PORT_SMPL_RTE_IC_1             0x0    /* 000b	[2:0] */
#define R68_SERIAL_PORT_SMPL_RTE_IC_1_MASK        0x7
#define R68_SERIAL_PORT_SMPL_RTE_IC_1_SHIFT       0

/* Clock Enable Reg 0 (IC 1) */
#define R69_SERIAL_PORT_IC_1                      0x1    /* 1b	[0] */
#define R69_ROUTING_MATRIX_IN_IC_1                0x1    /* 1b	[1] */
#define R69_INTERPOLATOR_RESYNC_IC_1              0x1    /* 1b	[2] */
#define R69_ROUTING_MATRIX_OUT_IC_1               0x1    /* 1b	[3] */
#define R69_DECIMATOR_RESYNC_IC_1                 0x1    /* 1b	[4] */
#define R69_ALC_ON_IC_1                           0x1    /* 1b	[5] */
#define R69_SLEW_ON_IC_1                          0x1    /* 1b	[6] */
#define R69_SERIAL_PORT_IC_1_MASK                 0x1
#define R69_SERIAL_PORT_IC_1_SHIFT                0
#define R69_ROUTING_MATRIX_IN_IC_1_MASK           0x2
#define R69_ROUTING_MATRIX_IN_IC_1_SHIFT          1
#define R69_INTERPOLATOR_RESYNC_IC_1_MASK         0x4
#define R69_INTERPOLATOR_RESYNC_IC_1_SHIFT        2
#define R69_ROUTING_MATRIX_OUT_IC_1_MASK          0x8
#define R69_ROUTING_MATRIX_OUT_IC_1_SHIFT         3
#define R69_DECIMATOR_RESYNC_IC_1_MASK            0x10
#define R69_DECIMATOR_RESYNC_IC_1_SHIFT           4
#define R69_ALC_ON_IC_1_MASK                      0x20
#define R69_ALC_ON_IC_1_SHIFT                     5
#define R69_SLEW_ON_IC_1_MASK                     0x40
#define R69_SLEW_ON_IC_1_SHIFT                    6

/* Clock Enable Reg 1 (IC 1) */
#define R70_CLOCK_GENERATOR_0_IC_1                0x1    /* 1b	[0] */
#define R70_CLOCK_GENERATOR_1_IC_1                0x1    /* 1b	[1] */
#define R70_CLOCK_GENERATOR_0_IC_1_MASK           0x1
#define R70_CLOCK_GENERATOR_0_IC_1_SHIFT          0
#define R70_CLOCK_GENERATOR_1_IC_1_MASK           0x2
#define R70_CLOCK_GENERATOR_1_IC_1_SHIFT          1

#endif

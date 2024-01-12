/*
 * File:           C:\Users\shun\C4NDY\SigmaDSP\ADAU1761_IC_1_PARAM.h
 *
 * Created:        Friday, January 12, 2024 2:43:02 PM
 * Description:    ADAU1761:IC 1 parameter RAM definitions.
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
#ifndef __ADAU1761_IC_1_PARAM_H__
#define __ADAU1761_IC_1_PARAM_H__


/* Module Modulo Size - Modulo Size*/
#define MOD_MODULOSIZE_COUNT                           1
#define MOD_MODULOSIZE_DEVICE                          "IC1"
#define MOD_MODULOSIZE_MODULO_SIZE_ADDR                0
#define MOD_MODULOSIZE_MODULO_SIZE_FIXPT               0x00001000
#define MOD_MODULOSIZE_MODULO_SIZE_VALUE               SIGMASTUDIOTYPE_INTEGER_CONVERT(4096)
#define MOD_MODULOSIZE_MODULO_SIZE_TYPE                SIGMASTUDIOTYPE_INTEGER

/* Module DC1 - DC Input Entry*/
#define MOD_DC1_COUNT                                  1
#define MOD_DC1_DEVICE                                 "IC1"
#define MOD_DC1_DCINPALG1_ADDR                         8
#define MOD_DC1_DCINPALG1_FIXPT                        0x00400000
#define MOD_DC1_DCINPALG1_VALUE                        SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.5)
#define MOD_DC1_DCINPALG1_TYPE                         SIGMASTUDIOTYPE_FIXPOINT

/* Module DC2 - DC Input Entry*/
#define MOD_DC2_COUNT                                  1
#define MOD_DC2_DEVICE                                 "IC1"
#define MOD_DC2_DCINPALG2_ADDR                         9
#define MOD_DC2_DCINPALG2_FIXPT                        0x00400000
#define MOD_DC2_DCINPALG2_VALUE                        SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.5)
#define MOD_DC2_DCINPALG2_TYPE                         SIGMASTUDIOTYPE_FIXPOINT

/* Module Single 1 - Single Volume Shared*/
#define MOD_SINGLE1_COUNT                              2
#define MOD_SINGLE1_DEVICE                             "IC1"
#define MOD_SINGLE1_ALG0_APPROXFACTORGAINS200ALGGROWDBSTEP1_ADDR 10
#define MOD_SINGLE1_ALG0_APPROXFACTORGAINS200ALGGROWDBSTEP1_FIXPT 0x00001000
#define MOD_SINGLE1_ALG0_APPROXFACTORGAINS200ALGGROWDBSTEP1_VALUE SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.00048828125)
#define MOD_SINGLE1_ALG0_APPROXFACTORGAINS200ALGGROWDBSTEP1_TYPE SIGMASTUDIOTYPE_FIXPOINT
#define MOD_SINGLE1_ALG0_TARGETGAINS200ALGGROWDBSTEP1_ADDR 11
#define MOD_SINGLE1_ALG0_TARGETGAINS200ALGGROWDBSTEP1_FIXPT 0x0050C335
#define MOD_SINGLE1_ALG0_TARGETGAINS200ALGGROWDBSTEP1_VALUE SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.630957344480193)
#define MOD_SINGLE1_ALG0_TARGETGAINS200ALGGROWDBSTEP1_TYPE SIGMASTUDIOTYPE_FIXPOINT

/* Module ln ph sw - Stereo Switch 2xN*/
#define MOD_LNPHSW_COUNT                               2
#define MOD_LNPHSW_DEVICE                              "IC1"
#define MOD_LNPHSW_ALG0_STEREODEMUXSLEW10_ADDR         12
#define MOD_LNPHSW_ALG0_STEREODEMUXSLEW10_FIXPT        0x00000000
#define MOD_LNPHSW_ALG0_STEREODEMUXSLEW10_VALUE        SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0)
#define MOD_LNPHSW_ALG0_STEREODEMUXSLEW10_TYPE         SIGMASTUDIOTYPE_FIXPOINT
#define MOD_LNPHSW_ALG0_STEREODEMUXSLEW11_ADDR         13
#define MOD_LNPHSW_ALG0_STEREODEMUXSLEW11_FIXPT        0x00800000
#define MOD_LNPHSW_ALG0_STEREODEMUXSLEW11_VALUE        SIGMASTUDIOTYPE_FIXPOINT_CONVERT(1)
#define MOD_LNPHSW_ALG0_STEREODEMUXSLEW11_TYPE         SIGMASTUDIOTYPE_FIXPOINT

/* Module phono eq - Parametric EQ*/
#define MOD_PHONOEQ_COUNT                              30
#define MOD_PHONOEQ_DEVICE                             "IC1"
#define MOD_PHONOEQ_ALG0_STAGE0_B0_ADDR                14
#define MOD_PHONOEQ_ALG0_STAGE0_B0_FIXPT               0x0084BFBA
#define MOD_PHONOEQ_ALG0_STAGE0_B0_VALUE               SIGMASTUDIOTYPE_FIXPOINT_CONVERT(1.03710108693933)
#define MOD_PHONOEQ_ALG0_STAGE0_B0_TYPE                SIGMASTUDIOTYPE_FIXPOINT
#define MOD_PHONOEQ_ALG0_STAGE0_B0_ADDR                14
#define MOD_PHONOEQ_ALG0_STAGE0_B0_FIXPT               0x0084BFBA
#define MOD_PHONOEQ_ALG0_STAGE0_B0_VALUE               SIGMASTUDIOTYPE_FIXPOINT_CONVERT(1.03710108693933)
#define MOD_PHONOEQ_ALG0_STAGE0_B0_TYPE                SIGMASTUDIOTYPE_FIXPOINT
#define MOD_PHONOEQ_ALG0_STAGE0_B1_ADDR                15
#define MOD_PHONOEQ_ALG0_STAGE0_B1_FIXPT               0xFF010E63
#define MOD_PHONOEQ_ALG0_STAGE0_B1_VALUE               SIGMASTUDIOTYPE_FIXPOINT_CONVERT(-1.99174848837953)
#define MOD_PHONOEQ_ALG0_STAGE0_B1_TYPE                SIGMASTUDIOTYPE_FIXPOINT
#define MOD_PHONOEQ_ALG0_STAGE0_B1_ADDR                15
#define MOD_PHONOEQ_ALG0_STAGE0_B1_FIXPT               0xFF010E63
#define MOD_PHONOEQ_ALG0_STAGE0_B1_VALUE               SIGMASTUDIOTYPE_FIXPOINT_CONVERT(-1.99174848837953)
#define MOD_PHONOEQ_ALG0_STAGE0_B1_TYPE                SIGMASTUDIOTYPE_FIXPOINT
#define MOD_PHONOEQ_ALG0_STAGE0_B2_ADDR                16
#define MOD_PHONOEQ_ALG0_STAGE0_B2_FIXPT               0x007A321C
#define MOD_PHONOEQ_ALG0_STAGE0_B2_VALUE               SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.954654227074152)
#define MOD_PHONOEQ_ALG0_STAGE0_B2_TYPE                SIGMASTUDIOTYPE_FIXPOINT
#define MOD_PHONOEQ_ALG0_STAGE0_B2_ADDR                16
#define MOD_PHONOEQ_ALG0_STAGE0_B2_FIXPT               0x007A321C
#define MOD_PHONOEQ_ALG0_STAGE0_B2_VALUE               SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.954654227074152)
#define MOD_PHONOEQ_ALG0_STAGE0_B2_TYPE                SIGMASTUDIOTYPE_FIXPOINT
#define MOD_PHONOEQ_ALG0_STAGE0_A0_ADDR                17
#define MOD_PHONOEQ_ALG0_STAGE0_A0_FIXPT               0x00FEF19D
#define MOD_PHONOEQ_ALG0_STAGE0_A0_VALUE               SIGMASTUDIOTYPE_FIXPOINT_CONVERT(1.99174848837953)
#define MOD_PHONOEQ_ALG0_STAGE0_A0_TYPE                SIGMASTUDIOTYPE_FIXPOINT
#define MOD_PHONOEQ_ALG0_STAGE0_A0_ADDR                17
#define MOD_PHONOEQ_ALG0_STAGE0_A0_FIXPT               0x00FEF19D
#define MOD_PHONOEQ_ALG0_STAGE0_A0_VALUE               SIGMASTUDIOTYPE_FIXPOINT_CONVERT(1.99174848837953)
#define MOD_PHONOEQ_ALG0_STAGE0_A0_TYPE                SIGMASTUDIOTYPE_FIXPOINT
#define MOD_PHONOEQ_ALG0_STAGE0_A1_ADDR                18
#define MOD_PHONOEQ_ALG0_STAGE0_A1_FIXPT               0xFF810E2A
#define MOD_PHONOEQ_ALG0_STAGE0_A1_VALUE               SIGMASTUDIOTYPE_FIXPOINT_CONVERT(-0.991755314013482)
#define MOD_PHONOEQ_ALG0_STAGE0_A1_TYPE                SIGMASTUDIOTYPE_FIXPOINT
#define MOD_PHONOEQ_ALG0_STAGE0_A1_ADDR                18
#define MOD_PHONOEQ_ALG0_STAGE0_A1_FIXPT               0xFF810E2A
#define MOD_PHONOEQ_ALG0_STAGE0_A1_VALUE               SIGMASTUDIOTYPE_FIXPOINT_CONVERT(-0.991755314013482)
#define MOD_PHONOEQ_ALG0_STAGE0_A1_TYPE                SIGMASTUDIOTYPE_FIXPOINT
#define MOD_PHONOEQ_ALG0_STAGE1_B0_ADDR                19
#define MOD_PHONOEQ_ALG0_STAGE1_B0_FIXPT               0x007E8897
#define MOD_PHONOEQ_ALG0_STAGE1_B0_VALUE               SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.988543391364656)
#define MOD_PHONOEQ_ALG0_STAGE1_B0_TYPE                SIGMASTUDIOTYPE_FIXPOINT
#define MOD_PHONOEQ_ALG0_STAGE1_B0_ADDR                19
#define MOD_PHONOEQ_ALG0_STAGE1_B0_FIXPT               0x007E8897
#define MOD_PHONOEQ_ALG0_STAGE1_B0_VALUE               SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.988543391364656)
#define MOD_PHONOEQ_ALG0_STAGE1_B0_TYPE                SIGMASTUDIOTYPE_FIXPOINT
#define MOD_PHONOEQ_ALG0_STAGE1_B1_ADDR                20
#define MOD_PHONOEQ_ALG0_STAGE1_B1_FIXPT               0xFF06BBD7
#define MOD_PHONOEQ_ALG0_STAGE1_B1_VALUE               SIGMASTUDIOTYPE_FIXPOINT_CONVERT(-1.94739265844841)
#define MOD_PHONOEQ_ALG0_STAGE1_B1_TYPE                SIGMASTUDIOTYPE_FIXPOINT
#define MOD_PHONOEQ_ALG0_STAGE1_B1_ADDR                20
#define MOD_PHONOEQ_ALG0_STAGE1_B1_FIXPT               0xFF06BBD7
#define MOD_PHONOEQ_ALG0_STAGE1_B1_VALUE               SIGMASTUDIOTYPE_FIXPOINT_CONVERT(-1.94739265844841)
#define MOD_PHONOEQ_ALG0_STAGE1_B1_TYPE                SIGMASTUDIOTYPE_FIXPOINT
#define MOD_PHONOEQ_ALG0_STAGE1_B2_ADDR                21
#define MOD_PHONOEQ_ALG0_STAGE1_B2_FIXPT               0x007AC3DF
#define MOD_PHONOEQ_ALG0_STAGE1_B2_VALUE               SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.959102528642176)
#define MOD_PHONOEQ_ALG0_STAGE1_B2_TYPE                SIGMASTUDIOTYPE_FIXPOINT
#define MOD_PHONOEQ_ALG0_STAGE1_B2_ADDR                21
#define MOD_PHONOEQ_ALG0_STAGE1_B2_FIXPT               0x007AC3DF
#define MOD_PHONOEQ_ALG0_STAGE1_B2_VALUE               SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.959102528642176)
#define MOD_PHONOEQ_ALG0_STAGE1_B2_TYPE                SIGMASTUDIOTYPE_FIXPOINT
#define MOD_PHONOEQ_ALG0_STAGE1_A0_ADDR                22
#define MOD_PHONOEQ_ALG0_STAGE1_A0_FIXPT               0x00F94429
#define MOD_PHONOEQ_ALG0_STAGE1_A0_VALUE               SIGMASTUDIOTYPE_FIXPOINT_CONVERT(1.94739265844841)
#define MOD_PHONOEQ_ALG0_STAGE1_A0_TYPE                SIGMASTUDIOTYPE_FIXPOINT
#define MOD_PHONOEQ_ALG0_STAGE1_A0_ADDR                22
#define MOD_PHONOEQ_ALG0_STAGE1_A0_FIXPT               0x00F94429
#define MOD_PHONOEQ_ALG0_STAGE1_A0_VALUE               SIGMASTUDIOTYPE_FIXPOINT_CONVERT(1.94739265844841)
#define MOD_PHONOEQ_ALG0_STAGE1_A0_TYPE                SIGMASTUDIOTYPE_FIXPOINT
#define MOD_PHONOEQ_ALG0_STAGE1_A1_ADDR                23
#define MOD_PHONOEQ_ALG0_STAGE1_A1_FIXPT               0xFF86B38A
#define MOD_PHONOEQ_ALG0_STAGE1_A1_VALUE               SIGMASTUDIOTYPE_FIXPOINT_CONVERT(-0.947645920006832)
#define MOD_PHONOEQ_ALG0_STAGE1_A1_TYPE                SIGMASTUDIOTYPE_FIXPOINT
#define MOD_PHONOEQ_ALG0_STAGE1_A1_ADDR                23
#define MOD_PHONOEQ_ALG0_STAGE1_A1_FIXPT               0xFF86B38A
#define MOD_PHONOEQ_ALG0_STAGE1_A1_VALUE               SIGMASTUDIOTYPE_FIXPOINT_CONVERT(-0.947645920006832)
#define MOD_PHONOEQ_ALG0_STAGE1_A1_TYPE                SIGMASTUDIOTYPE_FIXPOINT
#define MOD_PHONOEQ_ALG0_STAGE2_B0_ADDR                24
#define MOD_PHONOEQ_ALG0_STAGE2_B0_FIXPT               0x00338111
#define MOD_PHONOEQ_ALG0_STAGE2_B0_VALUE               SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.402376321322912)
#define MOD_PHONOEQ_ALG0_STAGE2_B0_TYPE                SIGMASTUDIOTYPE_FIXPOINT
#define MOD_PHONOEQ_ALG0_STAGE2_B0_ADDR                24
#define MOD_PHONOEQ_ALG0_STAGE2_B0_FIXPT               0x00338111
#define MOD_PHONOEQ_ALG0_STAGE2_B0_VALUE               SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.402376321322912)
#define MOD_PHONOEQ_ALG0_STAGE2_B0_TYPE                SIGMASTUDIOTYPE_FIXPOINT
#define MOD_PHONOEQ_ALG0_STAGE2_B1_ADDR                25
#define MOD_PHONOEQ_ALG0_STAGE2_B1_FIXPT               0x004A7C78
#define MOD_PHONOEQ_ALG0_STAGE2_B1_VALUE               SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.58192350170784)
#define MOD_PHONOEQ_ALG0_STAGE2_B1_TYPE                SIGMASTUDIOTYPE_FIXPOINT
#define MOD_PHONOEQ_ALG0_STAGE2_B1_ADDR                25
#define MOD_PHONOEQ_ALG0_STAGE2_B1_FIXPT               0x004A7C78
#define MOD_PHONOEQ_ALG0_STAGE2_B1_VALUE               SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.58192350170784)
#define MOD_PHONOEQ_ALG0_STAGE2_B1_TYPE                SIGMASTUDIOTYPE_FIXPOINT
#define MOD_PHONOEQ_ALG0_STAGE2_B2_ADDR                26
#define MOD_PHONOEQ_ALG0_STAGE2_B2_FIXPT               0x0022814D
#define MOD_PHONOEQ_ALG0_STAGE2_B2_VALUE               SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.26957105939467)
#define MOD_PHONOEQ_ALG0_STAGE2_B2_TYPE                SIGMASTUDIOTYPE_FIXPOINT
#define MOD_PHONOEQ_ALG0_STAGE2_B2_ADDR                26
#define MOD_PHONOEQ_ALG0_STAGE2_B2_FIXPT               0x0022814D
#define MOD_PHONOEQ_ALG0_STAGE2_B2_VALUE               SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.26957105939467)
#define MOD_PHONOEQ_ALG0_STAGE2_B2_TYPE                SIGMASTUDIOTYPE_FIXPOINT
#define MOD_PHONOEQ_ALG0_STAGE2_A0_ADDR                27
#define MOD_PHONOEQ_ALG0_STAGE2_A0_FIXPT               0xFFB58388
#define MOD_PHONOEQ_ALG0_STAGE2_A0_VALUE               SIGMASTUDIOTYPE_FIXPOINT_CONVERT(-0.58192350170784)
#define MOD_PHONOEQ_ALG0_STAGE2_A0_TYPE                SIGMASTUDIOTYPE_FIXPOINT
#define MOD_PHONOEQ_ALG0_STAGE2_A0_ADDR                27
#define MOD_PHONOEQ_ALG0_STAGE2_A0_FIXPT               0xFFB58388
#define MOD_PHONOEQ_ALG0_STAGE2_A0_VALUE               SIGMASTUDIOTYPE_FIXPOINT_CONVERT(-0.58192350170784)
#define MOD_PHONOEQ_ALG0_STAGE2_A0_TYPE                SIGMASTUDIOTYPE_FIXPOINT
#define MOD_PHONOEQ_ALG0_STAGE2_A1_ADDR                28
#define MOD_PHONOEQ_ALG0_STAGE2_A1_FIXPT               0x0029FDA0
#define MOD_PHONOEQ_ALG0_STAGE2_A1_VALUE               SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.328052619282417)
#define MOD_PHONOEQ_ALG0_STAGE2_A1_TYPE                SIGMASTUDIOTYPE_FIXPOINT
#define MOD_PHONOEQ_ALG0_STAGE2_A1_ADDR                28
#define MOD_PHONOEQ_ALG0_STAGE2_A1_FIXPT               0x0029FDA0
#define MOD_PHONOEQ_ALG0_STAGE2_A1_VALUE               SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.328052619282417)
#define MOD_PHONOEQ_ALG0_STAGE2_A1_TYPE                SIGMASTUDIOTYPE_FIXPOINT

/* Module usb gain L - Single SW slew vol (adjustable)*/
#define MOD_USBGAINL_COUNT                             2
#define MOD_USBGAINL_DEVICE                            "IC1"
#define MOD_USBGAINL_ALG0_TARGET_ADDR                  29
#define MOD_USBGAINL_ALG0_TARGET_FIXPT                 0x00800000
#define MOD_USBGAINL_ALG0_TARGET_VALUE                 SIGMASTUDIOTYPE_FIXPOINT_CONVERT(1)
#define MOD_USBGAINL_ALG0_TARGET_TYPE                  SIGMASTUDIOTYPE_FIXPOINT
#define MOD_USBGAINL_ALG0_STEP_ADDR                    30
#define MOD_USBGAINL_ALG0_STEP_FIXPT                   0x00008000
#define MOD_USBGAINL_ALG0_STEP_VALUE                   SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.00390625)
#define MOD_USBGAINL_ALG0_STEP_TYPE                    SIGMASTUDIOTYPE_FIXPOINT

/* Module usb gain R - Single SW slew vol (adjustable)*/
#define MOD_USBGAINR_COUNT                             2
#define MOD_USBGAINR_DEVICE                            "IC1"
#define MOD_USBGAINR_ALG0_TARGET_ADDR                  31
#define MOD_USBGAINR_ALG0_TARGET_FIXPT                 0x00800000
#define MOD_USBGAINR_ALG0_TARGET_VALUE                 SIGMASTUDIOTYPE_FIXPOINT_CONVERT(1)
#define MOD_USBGAINR_ALG0_TARGET_TYPE                  SIGMASTUDIOTYPE_FIXPOINT
#define MOD_USBGAINR_ALG0_STEP_ADDR                    32
#define MOD_USBGAINR_ALG0_STEP_FIXPT                   0x00008000
#define MOD_USBGAINR_ALG0_STEP_VALUE                   SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.00390625)
#define MOD_USBGAINR_ALG0_STEP_TYPE                    SIGMASTUDIOTYPE_FIXPOINT

/* Module SW vol 2 - Single slew ext vol*/
#define MOD_SWVOL2_COUNT                               1
#define MOD_SWVOL2_DEVICE                              "IC1"
#define MOD_SWVOL2_EXTSWGAINDB2STEP_ADDR               33
#define MOD_SWVOL2_EXTSWGAINDB2STEP_FIXPT              0x00010000
#define MOD_SWVOL2_EXTSWGAINDB2STEP_VALUE              SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.0078125)
#define MOD_SWVOL2_EXTSWGAINDB2STEP_TYPE               SIGMASTUDIOTYPE_FIXPOINT

/* Module phono gain - Single SW slew vol (adjustable)*/
#define MOD_PHONOGAIN_COUNT                            2
#define MOD_PHONOGAIN_DEVICE                           "IC1"
#define MOD_PHONOGAIN_ALG0_TARGET_ADDR                 34
#define MOD_PHONOGAIN_ALG0_TARGET_FIXPT                0x07ECA9CD
#define MOD_PHONOGAIN_ALG0_TARGET_VALUE                SIGMASTUDIOTYPE_FIXPOINT_CONVERT(15.8489319246111)
#define MOD_PHONOGAIN_ALG0_TARGET_TYPE                 SIGMASTUDIOTYPE_FIXPOINT
#define MOD_PHONOGAIN_ALG0_STEP_ADDR                   35
#define MOD_PHONOGAIN_ALG0_STEP_FIXPT                  0x00008000
#define MOD_PHONOGAIN_ALG0_STEP_VALUE                  SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.00390625)
#define MOD_PHONOGAIN_ALG0_STEP_TYPE                   SIGMASTUDIOTYPE_FIXPOINT

/* Module Mid EQ1 - Medium Size Eq*/
#define MOD_MIDEQ1_COUNT                               5
#define MOD_MIDEQ1_DEVICE                              "IC1"
#define MOD_MIDEQ1_ALG0_STAGE0_B0_ADDR                 36
#define MOD_MIDEQ1_ALG0_STAGE0_B0_FIXPT                0x007DD1C7
#define MOD_MIDEQ1_ALG0_STAGE0_B0_VALUE                SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.982964476761079)
#define MOD_MIDEQ1_ALG0_STAGE0_B0_TYPE                 SIGMASTUDIOTYPE_FIXPOINT
#define MOD_MIDEQ1_ALG0_STAGE0_B1_ADDR                 37
#define MOD_MIDEQ1_ALG0_STAGE0_B1_FIXPT                0xFF0462B4
#define MOD_MIDEQ1_ALG0_STAGE0_B1_VALUE                SIGMASTUDIOTYPE_FIXPOINT_CONVERT(-1.96573791559061)
#define MOD_MIDEQ1_ALG0_STAGE0_B1_TYPE                 SIGMASTUDIOTYPE_FIXPOINT
#define MOD_MIDEQ1_ALG0_STAGE0_B2_ADDR                 38
#define MOD_MIDEQ1_ALG0_STAGE0_B2_FIXPT                0x007DCB85
#define MOD_MIDEQ1_ALG0_STAGE0_B2_VALUE                SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.982773457391717)
#define MOD_MIDEQ1_ALG0_STAGE0_B2_TYPE                 SIGMASTUDIOTYPE_FIXPOINT
#define MOD_MIDEQ1_ALG0_STAGE0_A1_ADDR                 39
#define MOD_MIDEQ1_ALG0_STAGE0_A1_FIXPT                0x00FB93AE
#define MOD_MIDEQ1_ALG0_STAGE0_A1_VALUE                SIGMASTUDIOTYPE_FIXPOINT_CONVERT(1.96544443088901)
#define MOD_MIDEQ1_ALG0_STAGE0_A1_TYPE                 SIGMASTUDIOTYPE_FIXPOINT
#define MOD_MIDEQ1_ALG0_STAGE0_A2_ADDR                 40
#define MOD_MIDEQ1_ALG0_STAGE0_A2_FIXPT                0xFF845916
#define MOD_MIDEQ1_ALG0_STAGE0_A2_VALUE                SIGMASTUDIOTYPE_FIXPOINT_CONVERT(-0.966031418854412)
#define MOD_MIDEQ1_ALG0_STAGE0_A2_TYPE                 SIGMASTUDIOTYPE_FIXPOINT

/* Module Mg1 - Signal Merger*/
#define MOD_MG1_COUNT                                  1
#define MOD_MG1_DEVICE                                 "IC1"
#define MOD_MG1_SINGLECTRLMIXER19401_ADDR              41
#define MOD_MG1_SINGLECTRLMIXER19401_FIXPT             0x00400000
#define MOD_MG1_SINGLECTRLMIXER19401_VALUE             SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.5)
#define MOD_MG1_SINGLECTRLMIXER19401_TYPE              SIGMASTUDIOTYPE_FIXPOINT

/* Module Mg2 - Signal Merger*/
#define MOD_MG2_COUNT                                  1
#define MOD_MG2_DEVICE                                 "IC1"
#define MOD_MG2_SINGLECTRLMIXER19403_ADDR              42
#define MOD_MG2_SINGLECTRLMIXER19403_FIXPT             0x00400000
#define MOD_MG2_SINGLECTRLMIXER19403_VALUE             SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.5)
#define MOD_MG2_SINGLECTRLMIXER19403_TYPE              SIGMASTUDIOTYPE_FIXPOINT

/* Module SW vol 1 - Single slew ext vol*/
#define MOD_SWVOL1_COUNT                               1
#define MOD_SWVOL1_DEVICE                              "IC1"
#define MOD_SWVOL1_EXTSWGAINDB1STEP_ADDR               43
#define MOD_SWVOL1_EXTSWGAINDB1STEP_FIXPT              0x00010000
#define MOD_SWVOL1_EXTSWGAINDB1STEP_VALUE              SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.0078125)
#define MOD_SWVOL1_EXTSWGAINDB1STEP_TYPE               SIGMASTUDIOTYPE_FIXPOINT

/* Module Mg3 - Signal Merger*/
#define MOD_MG3_COUNT                                  1
#define MOD_MG3_DEVICE                                 "IC1"
#define MOD_MG3_SINGLECTRLMIXER19404_ADDR              44
#define MOD_MG3_SINGLECTRLMIXER19404_FIXPT             0x00400000
#define MOD_MG3_SINGLECTRLMIXER19404_VALUE             SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.5)
#define MOD_MG3_SINGLECTRLMIXER19404_TYPE              SIGMASTUDIOTYPE_FIXPOINT

/* Module Mg4 - Signal Merger*/
#define MOD_MG4_COUNT                                  1
#define MOD_MG4_DEVICE                                 "IC1"
#define MOD_MG4_SINGLECTRLMIXER19405_ADDR              45
#define MOD_MG4_SINGLECTRLMIXER19405_FIXPT             0x00400000
#define MOD_MG4_SINGLECTRLMIXER19405_VALUE             SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.5)
#define MOD_MG4_SINGLECTRLMIXER19405_TYPE              SIGMASTUDIOTYPE_FIXPOINT

/* Module master gain - Single SW slew vol (adjustable)*/
#define MOD_MASTERGAIN_COUNT                           2
#define MOD_MASTERGAIN_DEVICE                          "IC1"
#define MOD_MASTERGAIN_ALG0_TARGET_ADDR                46
#define MOD_MASTERGAIN_ALG0_TARGET_FIXPT               0x007C1CEA
#define MOD_MASTERGAIN_ALG0_TARGET_VALUE               SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.969632453498556)
#define MOD_MASTERGAIN_ALG0_TARGET_TYPE                SIGMASTUDIOTYPE_FIXPOINT
#define MOD_MASTERGAIN_ALG0_STEP_ADDR                  47
#define MOD_MASTERGAIN_ALG0_STEP_FIXPT                 0x00008000
#define MOD_MASTERGAIN_ALG0_STEP_VALUE                 SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.00390625)
#define MOD_MASTERGAIN_ALG0_STEP_TYPE                  SIGMASTUDIOTYPE_FIXPOINT

#endif

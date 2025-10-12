/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : tusb_config.h
 * @brief          : Provides the configuration options for TinyUSB
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 Antti Keskinen
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

#ifndef __TUSB_CONFIG_H__
#define __TUSB_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "usb_descriptors.h"

//--------------------------------------------------------------------
// COMMON CONFIGURATION
//--------------------------------------------------------------------

// Define MCU and OS
#define CFG_TUSB_MCU                  OPT_MCU_STM32F0
#define CFG_TUSB_OS                   OPT_OS_NONE

// Device mode
#define CFG_TUD_ENABLED               1

// Device supports full speed max
#define CFG_TUD_MAX_SPEED             OPT_MODE_FULL_SPEED

// Define speed and port settings
#define BOARD_DEVICE_RHPORT_SPEED     OPT_MODE_FULL_SPEED
#define BOARD_DEVICE_RHPORT_NUM       0
#define CFG_TUSB_RHPORT0_MODE         (OPT_MODE_DEVICE | BOARD_DEVICE_RHPORT_SPEED)

// Debug is off
#define CFG_TUSB_DEBUG                0

/* USB DMA on some MCUs can only access a specific SRAM region with restriction on alignment.
 * Tinyusb use follows macros to declare transferring memory so that they can be put
 * into those specific section.
 * e.g
 * - CFG_TUSB_MEM SECTION : __attribute__ (( section(".usb_ram") ))
 * - CFG_TUSB_MEM_ALIGN   : __attribute__ ((aligned(4)))
 */
#ifndef CFG_TUSB_MEM_SECTION
#define CFG_TUSB_MEM_SECTION
#endif

#ifndef CFG_TUSB_MEM_ALIGN
#define CFG_TUSB_MEM_ALIGN        __attribute__ ((aligned(4)))
#endif

//--------------------------------------------------------------------
// DEVICE CONFIGURATION
//--------------------------------------------------------------------

// Double-buffered isochronous endpoints mode
//#define CFG_TUD_FSDEV_DOUBLE_BUFFERED_ISO_EP 1

#define CFG_TUD_ENDPOINT0_SIZE    64

//------------- CLASS -------------//
#define CFG_TUD_AUDIO             1
#define CFG_TUD_CDC               0
#define CFG_TUD_MSC               0
#define CFG_TUD_HID               0
#define CFG_TUD_MIDI              0
#define CFG_TUD_VENDOR            0

//--------------------------------------------------------------------
// AUDIO CLASS DRIVER CONFIGURATION
//--------------------------------------------------------------------

// Sample rate and sample bit depth (not really part of the driver but listed here for clarity)
#define CFG_TUD_AUDIO_FUNC_1_SAMPLE_RATE                       44100
#define CFG_TUD_AUDIO_FUNC_1_SAMPLE_BIT_RESOLUTION             16

// Descriptor length and audio function control buffer size
#define CFG_TUD_AUDIO_FUNC_1_DESC_LEN                          TUD_AUDIO_RADIO_RECEIVER_TWO_CH_1_FORMAT_DESC_LEN
#define CFG_TUD_AUDIO_FUNC_1_CTRL_BUF_SZ                       CFG_TUD_ENDPOINT0_SIZE

// Number of EP IN endpoints to enable
#define CFG_TUD_AUDIO_ENABLE_EP_IN                             1

// Number of bytes per sample
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_1_N_BYTES_PER_SAMPLE_TX    CFG_TUD_AUDIO_FUNC_1_SAMPLE_BIT_RESOLUTION / 8

// Number of channels
#define CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX                     2

// EP IN maximum packet size
#define CFG_TUD_AUDIO_FUNC_1_EP_IN_SZ_MAX                      TUD_AUDIO_EP_SIZE(CFG_TUD_AUDIO_FUNC_1_SAMPLE_RATE, CFG_TUD_AUDIO_FUNC_1_FORMAT_1_N_BYTES_PER_SAMPLE_TX, CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX)

// Enable endpoint flow control
#define CFG_TUD_AUDIO_EP_IN_FLOW_CONTROL                       1

// FIFO buffer size (four one-millisecond frames)
#define CFG_TUD_AUDIO_FUNC_1_EP_IN_SW_BUF_SZ                   4U * CFG_TUD_AUDIO_FUNC_1_EP_IN_SZ_MAX

#ifdef __cplusplus
}
#endif

#endif /* __TUSB_CONFIG_H__ */

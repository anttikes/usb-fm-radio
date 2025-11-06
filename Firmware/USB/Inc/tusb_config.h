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

// Specify control endpoint data packet size
#define CFG_TUD_ENDPOINT0_SIZE    64

//------------- CLASS -------------//
#define CFG_TUD_AUDIO             1
#define CFG_TUD_HID               1

//--------------------------------------------------------------------
// AUDIO CLASS DRIVER CONFIGURATION
//--------------------------------------------------------------------
#include "audio_config.h"

//--------------------------------------------------------------------
// HID CLASS DRIVER CONFIGURATION
//--------------------------------------------------------------------
#include "hid_config.h"

#ifdef __cplusplus
}
#endif

#endif /* __TUSB_CONFIG_H__ */

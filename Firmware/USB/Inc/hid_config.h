/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : hid_config.h
 * @brief          : Header file for settings related to USB HID
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

#ifndef __HID_CONFIG_H__
#define __HID_CONFIG_H__

//--------------------------------------------------------------------
// HID CLASS DRIVER CONFIGURATION
//--------------------------------------------------------------------

// HID buffer size
#define CFG_TUD_HID_EP_BUFSIZE            16

// Interface numbers
#define ITF_NUM_HID                       0x02
#define ITF_NUM_HID_TOTAL                 0x01

// Endpoint numbers
#define EPNUM_HID                         0x02

#endif /* __HID_CONFIG_H__ */

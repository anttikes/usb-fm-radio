/**
 ******************************************************************************
 * @file    hid_config.h
 * @brief   Header file for settings related to USB HID Class Driver
 ******************************************************************************
 * @attention
 *
 * Copyright (c) Antti Keskinen
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 *
 ******************************************************************************
 */

/* Header guard --------------------------------------------------------------*/
#ifndef __HID_CONFIG_H__
#define __HID_CONFIG_H__

/* Includes ------------------------------------------------------------------*/
#include "reports.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

// HID buffer size; link to the report size defined in the shared header
#define CFG_TUD_HID_EP_BUFSIZE MAX_REPORT_SIZE

// Interface numbers
#define ITF_NUM_HID 0x02
#define ITF_NUM_HID_TOTAL 0x01

// Endpoint numbers
#define EPNUM_HID 0x02

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

#endif /* __HID_CONFIG_H__ */

/**
 ******************************************************************************
 * @file    usbd_desc.h
 * @author  MCD Application Team
 * @brief   Header for usbd_desc.c module
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2015 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_DESC_H
#define __USBD_DESC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbd_def.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/*
 * User to provide a unique ID to define the USB device serial number
 * The use of UID_BASE register can be considered as an example
 */
#define         DEVICE_ID1          (UID_BASE)
#define         DEVICE_ID2          (UID_BASE + 0x4U)
#define         DEVICE_ID3          (UID_BASE + 0x8U)

#define         USB_SIZ_STRING_SERIAL       0x1AU

/* Exported macro ------------------------------------------------------------*/
#define         USBD_WIDE_CHAR(wide_char)   \
    (uint8_t)(wide_char & 0x00FF), (uint8_t)((wide_char & 0xFF00) >> 8)

/* Exported functions ------------------------------------------------------- */
extern USBD_DescriptorsTypeDef FS_Desc;

#ifdef __cplusplus
}
#endif

#endif /* __USBD_DESC_H*/


/**
 ******************************************************************************
 * @file           : usb_device.c
 * @version        : v2.0_Cube
 * @brief          : This file implements the USB Device and library initialization
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <usb_device.h>
#include "main.h"
#include "usbd_core.h"
#include "usbd_desc.h"

#include "usbd_radio.h"
//#include "usbd_customhid.h"

/* Private variables ---------------------------------------------------------*/
USBD_HandleTypeDef hUsbDeviceFS;
extern USBD_RADIO_ItfTypeDef USBD_AUDIO_fops_FS;

/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/

/**
 * Init the STM32 USB Device Library, add supported classes, and start it
 * @retval None
 */
void MX_USB_DEVICE_Init(void)
{
	if (USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS) != USBD_OK)
	{
		Error_Handler();
	}

	if (USBD_RegisterClass(&hUsbDeviceFS, &USBD_RADIO) != USBD_OK)
	{
		Error_Handler();
	}

	if (USBD_RADIO_RegisterInterface(&hUsbDeviceFS, &USBD_AUDIO_fops_FS) != USBD_OK) {
		Error_Handler();
	}

	if (USBD_Start(&hUsbDeviceFS) != USBD_OK)
	{
		Error_Handler();
	}
}

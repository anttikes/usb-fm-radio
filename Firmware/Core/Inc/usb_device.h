/**
 ******************************************************************************
 * @file           : usb_device.h
 * @version        : v2.0_Cube
 * @brief          : Header for usb_device.c file.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_DEVICE__H__
#define __USB_DEVICE__H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "stm32f0xx_hal.h"
#include "usbd_def.h"

/* Private variables ---------------------------------------------------------*/
extern USBD_HandleTypeDef hUsbDeviceFS;

/* Private function prototypes -----------------------------------------------*/

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN VARIABLES */

/* USER CODE END VARIABLES */
/**
 * @}
 */

/** @defgroup USBD_DEVICE_Exported_FunctionsPrototype USBD_DEVICE_Exported_FunctionsPrototype
 * @brief Declaration of public functions for Usb device.
 * @{
 */

/** USB Device initialization function. */
void MX_USB_DEVICE_Init(void);

/*
 * -- Insert functions declaration here --
 */
/* USER CODE BEGIN FD */

/* USER CODE END FD */
/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __USB_DEVICE__H__ */

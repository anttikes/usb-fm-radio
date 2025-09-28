/**
 ******************************************************************************
 * @file    usbd_conf.h
 * @author  Antti Keskinen
 * @brief   Header file for the usbd_conf.c file
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
#ifndef __USBD_CONF_H
#define __USBD_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "stm32f0xx_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define USBD_MAX_NUM_INTERFACES                     2U
#define USBD_MAX_NUM_CONFIGURATION                  1U
#define USBD_MAX_STR_DESC_SIZ             (uint16_t)0x100
#define USBD_SELF_POWERED                           0U
#define USBD_DEBUG_LEVEL                            0U

/* Device Core identification */
#define DEVICE_FS                                   0

/* RADIO Class Config */
#define USBD_AUDIO_FREQ                             I2S_AUDIOFREQ_44K		// Linked to the current I2S frequency selection
#define AUDIO_REAL_FREQ								44117U					// This comes from the IOC visualizer
#define AUDIO_BITDEPTH								16U
#define AUDIO_CHANNELS								2U
#define CUSTOM_HID_HS_BINTERVAL                     0x05U
#define CUSTOM_HID_FS_BINTERVAL                     0x05U
#define USBD_CUSTOMHID_OUTREPORT_BUF_SIZE           0x02U
#define USBD_CUSTOM_HID_REPORT_DESC_SIZE            163U

/* #define USBD_CUSTOMHID_REPORT_DESC_SIZE_ENABLED */
/* #define USBD_CUSTOMHID_CTRL_REQ_GET_REPORT_ENABLED */
/* #define USBD_CUSTOMHID_OUT_PREPARE_RECEIVE_DISABLED */
/* #define USBD_CUSTOMHID_EP0_OUT_PREPARE_RECEIVE_DISABLED */
/* #define USBD_CUSTOMHID_CTRL_REQ_COMPLETE_CALLBACK_ENABLED */
/* #define USBD_CUSTOMHID_REPORT_BUFFER_EVENT_ENABLED */

/** Alias for memory allocation. */
#define USBD_malloc         (void *)malloc

/** Alias for memory release. */
#define USBD_free           free

/** Alias for memory set. */
#define USBD_memset         memset

/** Alias for memory copy. */
#define USBD_memcpy         memcpy

/** Alias for delay. */
#define USBD_Delay          HAL_Delay

/* DEBUG macros */
#if (USBD_DEBUG_LEVEL > 0U)
#define  USBD_UsrLog(...)   do { \
                                 printf(__VA_ARGS__); \
                                 printf("\n"); \
                               } while (0)
#else
#define USBD_UsrLog(...) do {} while (0)
#endif /* (USBD_DEBUG_LEVEL > 0U) */

#if (USBD_DEBUG_LEVEL > 1U)
#define  USBD_ErrLog(...) do { \
                               printf("ERROR: ") ; \
                               printf(__VA_ARGS__); \
                               printf("\n"); \
                             } while (0)
#else
#define USBD_ErrLog(...) do {} while (0)
#endif /* (USBD_DEBUG_LEVEL > 1U) */

#if (USBD_DEBUG_LEVEL > 2U)
#define  USBD_DbgLog(...)   do { \
                                 printf("DEBUG : ") ; \
                                 printf(__VA_ARGS__); \
                                 printf("\n"); \
                               } while (0)
#else
#define USBD_DbgLog(...) do {} while (0)
#endif /* (USBD_DEBUG_LEVEL > 2U) */

/* Exported functions -------------------------------------------------------*/
//void* USBD_static_malloc(uint32_t size);
//void USBD_static_free(void *p);

#ifdef __cplusplus
}
#endif

#endif /* __USBD_CONF_H */

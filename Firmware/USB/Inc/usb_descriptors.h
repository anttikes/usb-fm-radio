/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : usb_descriptors.h
 * @brief          : Header file for usb_descriptors.c
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

#ifndef __USB_DESCRIPTORS_H__
#define __USB_DESCRIPTORS_H__

// Unit numbers are arbitrary selected
#define ENTITY_ID_INPUT_TERMINAL          0x01
#define ENTITY_ID_FEATURE_UNIT            0x02
#define ENTITY_ID_OUTPUT_TERMINAL         0x03
#define ENTITY_ID_CLOCK_SOURCE            0x04

// Endpoint numbers
#define EPNUM_AUDIO                       0x01

#define TUD_AUDIO_RADIO_RECEIVER_TWO_CH_1_FORMAT_DESC_LEN (TUD_AUDIO_DESC_IAD_LEN\
    + TUD_AUDIO_DESC_STD_AC_LEN\
    + TUD_AUDIO_DESC_CS_AC_LEN\
    + TUD_AUDIO_DESC_CLK_SRC_LEN\
    + TUD_AUDIO_DESC_INPUT_TERM_LEN\
	+ TUD_AUDIO_DESC_FEATURE_UNIT_TWO_CHANNEL_LEN \
    + TUD_AUDIO_DESC_OUTPUT_TERM_LEN\
    /* Interface 1, Alternate 0 */\
    + TUD_AUDIO_DESC_STD_AS_INT_LEN\
    /* Interface 1, Alternate 1 */\
    + TUD_AUDIO_DESC_STD_AS_INT_LEN\
    + TUD_AUDIO_DESC_CS_AS_INT_LEN\
    + TUD_AUDIO_DESC_TYPE_I_FORMAT_LEN\
    /* Endpoint 1 */\
    + TUD_AUDIO_DESC_STD_AS_ISO_EP_LEN\
    + TUD_AUDIO_DESC_CS_AS_ISO_EP_LEN)

#endif /* __USB_DESCRIPTORS_H__ */

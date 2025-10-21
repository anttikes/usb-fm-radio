/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : audio_config.h
 * @brief          : Header file for settings related to USB Audio
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

#ifndef __AUDIO_CONFIG_H__
#define __AUDIO_CONFIG_H__

//--------------------------------------------------------------------
// AUDIO CLASS DRIVER CONFIGURATION
//--------------------------------------------------------------------

// Sample rate and sample bit depth (not really part of the driver but listed here for clarity)
#define CFG_TUD_AUDIO_FUNC_1_SAMPLE_RATE                       48000
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

// Interface numbers
#define ITF_NUM_AUDIO_CONTROL             0x00
#define ITF_NUM_AUDIO_STREAMING	          0x01
#define ITF_NUM_AUDIO_TOTAL               0x02

// Alternative settings
#define ALTERNATIVE_SETTING_DISABLE       0x00
#define ALTERNATIVE_SETTING_ENABLE        0x01

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

#endif /* __AUDIO_CONFIG_H__ */

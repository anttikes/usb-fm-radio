/**
 ******************************************************************************
 * @file    common.h
 * @brief   Header for common definitions
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 Antti Keskinen
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 *
 ******************************************************************************
 */

/* Header guard --------------------------------------------------------------*/
#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/
typedef struct _GetRevisionResponse_t
{
    /* Final 2 digits of part number, in HEX */
    uint8_t productNumber;

    /* Firmware Major Revision, in ASCII */
    uint8_t firmwareMajor;

    /* Firmware Minor Revision, in ASCII */
    uint8_t firmwareMinor;

    /* Patch ID, in hex */
    uint16_t patchId;

    /* Component Major Revision, in ASCII */
    uint8_t componentMajor;

    /* Component Minor Revision, in ASCII */
    uint8_t componentMinor;

    /* Chip Revision, in ASCII */
    uint8_t chipRevision;

    /* CID (Si4705/06 only) */
    uint8_t cid;
} GetRevisionResponse_t;

typedef struct _GetTuneStatusResponse_t
{
    /* When set, the seek has hit the band limit, or has wrapped back to the original frequency */
    bool bandLimit;

    /* Set if the AFC rails */
    bool AFCRail;

    /* When set, the current channel is considered valid according to the seek/tune properties */
    bool validChannel;

    /* Frequency we are currently tuned to */
    uint16_t frequency;

    /* After tune has been completed, holds the Received Signal Strength Indicator, in dbµV */
    uint8_t rssi;

    /* After tune has been completed, holds the Signal to Noise Ratio, in dB */
    uint8_t snr;

    /* After tune has been completed, holds the multipath metric */
    uint8_t multipath;

    /* Holds the current antenna tuning capacitor's value */
    uint8_t antennaTuningCapacitor;
} GetTuneStatusResponse_t;

typedef enum _StatusFlags_t : uint8_t
{
    /* No status bits are set */
    STATUS_NONE = 0x00,

    /* Seek/Tune Complete Interrupt is active */
    STATUS_STCINT = 0x01,

    /* Radio Data System (RDS) Interrupt is active */
    STATUS_RDSINT = 0x04,

    /* Received Signal Quality (RSQ) Interrupt is active */
    STATUS_RSQINT = 0x08,

    /* Error */
    STATUS_ERROR = 0x40,

    /* Clear to Send (CTS) is active */
    STATUS_CLEAR_TO_SEND = 0x80,
} StatusFlags_t;

typedef enum _CommandIdentifiers_t : uint8_t
{
    CMD_ID_POWER_UP = 0x01,
    CMD_ID_GET_REV = 0x10,
    CMD_ID_POWER_DOWN = 0x11,
    CMD_ID_SET_PROPERTY = 0x12,
    CMD_ID_GET_PROPERTY = 0x13,
    CMD_ID_GET_INT_STATUS = 0x14,
    CMD_ID_PATCH_ARGS = 0x15,
    CMD_ID_PATCH_DATA = 0x16,
    CMD_ID_FM_TUNE_FREQ = 0x20,
    CMD_ID_FM_SEEK_START = 0x21,
    CMD_ID_FM_TUNE_STATUS = 0x22,
    CMD_ID_FM_RSQ_STATUS = 0x23,
    CMD_ID_FM_RDS_STATUS = 0x24,
    CMD_ID_FM_AGC_STATUS = 0x27,
    CMD_ID_FM_AGC_OVERRIDE = 0x28,
    CMD_ID_GPIO_CTL = 0x80,
    CMD_ID_GPIO_SET = 0x81,
} CommandIdentifiers_t;

typedef enum _PropertyIdentifiers_t : uint16_t
{
    PROP_ID_GPO_IEN = 0x0001,
    PROP_ID_DIGITAL_OUTPUT_FORMAT = 0x0102,
    PROP_ID_DIGITAL_OUTPUT_SAMPLE_RATE = 0x0104,
    PROP_ID_REFCLK_FREQ = 0x0201,
    PROP_ID_REFCLK_PRESCALE = 0x0202,
    PROP_ID_FM_DEEMPHASIS = 0x1100,
    PROP_ID_FM_CHANNEL_FILTER = 0x1102,
    PROP_ID_FM_BLEND_STEREO_THRESHOLD = 0x1105,
    PROP_ID_FM_BLEND_MONO_THRESHOLD = 0x1106,
    PROP_ID_FM_ANTENNA_INPUT = 0x1107,
    PROP_ID_FM_MAX_TUNE_ERROR = 0x1108,
    PROP_ID_FM_RSQ_INT_SOURCE = 0x1200,
    PROP_ID_FM_RSQ_SNR_HI_THRESHOLD = 0x1201,
    PROP_ID_FM_RSQ_SNR_LO_THRESHOLD = 0x1202,
    PROP_ID_FM_RSQ_RSSI_HI_THRESHOLD = 0x1203,
    PROP_ID_FM_RSQ_RSSI_LO_THRESHOLD = 0x1204,
    PROP_ID_FM_RSQ_MULTIPATH_HI_THRESHOLD = 0x1205,
    PROP_ID_FM_RSQ_MULTIPATH_LO_THRESHOLD = 0x1206,
    PROP_ID_FM_RSQ_BLEND_THRESHOLD = 0x1207,
    PROP_ID_FM_SOFT_MUTE_RATE = 0x1300,
    PROP_ID_FM_SOFT_MUTE_SLOPE = 0x1301,
    PROP_ID_FM_SOFT_MUTE_MAX_ATTENUATION = 0x1302,
    PROP_ID_FM_SOFT_MUTE_SNR_THRESHOLD = 0x1303,
    PROP_ID_FM_SOFT_MUTE_RELEASE_RATE = 0x1304,
    PROP_ID_FM_SOFT_MUTE_ATTACK_RATE = 0x1305,
    PROP_ID_FM_SEEK_BAND_BOTTOM = 0x1400,
    PROP_ID_FM_SEEK_BAND_TOP = 0x1401,
    PROP_ID_FM_SEEK_FREQ_SPACING = 0x1402,
    PROP_ID_FM_SEEK_TUNE_THRESHOLD = 0x1403,
    PROP_ID_FM_SEEK_TUNE_RSSI_THRESHOLD = 0x1404,
    PROP_ID_FM_RDS_INT_SOURCE = 0x1500,
    PROP_ID_FM_RDS_INT_FIFO_COUNT = 0x1501,
    PROP_ID_FM_RDS_CONFIG = 0x1502,
    PROP_ID_FM_RDS_CONFIDENCE = 0x1503,
    PROP_ID_FM_BLEND_RSSI_STEREO_THRESHOLD = 0x1800,
    PROP_ID_FM_BLEND_RSSI_MONO_THRESHOLD = 0x1801,
    PROP_ID_FM_BLEND_RSSI_ATTACK_RATE = 0x1802,
    PROP_ID_FM_BLEND_RSSI_RELEASE_RATE = 0x1803,
    PROP_ID_FM_BLEND_SNR_STEREO_THRESHOLD = 0x1804,
    PROP_ID_FM_BLEND_SNR_MONO_THRESHOLD = 0x1805,
    PROP_ID_FM_BLEND_SNR_ATTACK_RATE = 0x1806,
    PROP_ID_FM_BLEND_SNR_RELEASE_RATE = 0x1807,
    PROP_ID_FM_BLEND_MULTIPATH_STEREO_THRESHOLD = 0x1808,
    PROP_ID_FM_BLEND_MULTIPATH_MONO_THRESHOLD = 0x1809,
    PROP_ID_FM_BLEND_MULTIPATH_ATTACK_RATE = 0x180A,
    PROP_ID_FM_BLEND_MULTIPATH_RELEASE_RATE = 0x180B,
    PROP_ID_FM_HICUT_SNR_HIGH_THRESHOLD = 0x1A00,
    PROP_ID_FM_HICUT_SNR_LOW_THRESHOLD = 0x1A01,
    PROP_ID_FM_HICUT_ATTACK_RATE = 0x1A02,
    PROP_ID_FM_HICUT_RELEASE_RATE = 0x1A03,
    PROP_ID_FM_HICUT_MULTIPATH_TRIGGER_THRESHOLD = 0x1A04,
    PROP_ID_FM_HICUT_MULTIPATH_END_THRESHOLD = 0x1A05,
    PROP_ID_FM_HICUT_CUTOFF_FREQUENCY = 0x1A06,
    PROP_ID_RX_VOLUME = 0x4000,
    PROP_ID_RX_HARD_MUTE = 0x4001,
} PropertyIdentifiers_t;

/* Exported constants --------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __COMMON_H__ */

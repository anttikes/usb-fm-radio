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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

typedef enum _CMD_POWER_UP_ARGS_1
{
    /* Execute the "FM Receive" operation */
    POWER_UP_ARGS_1_FUNCTION_FM = 0x00,

    /* Use the crystal oscillator via GPO3 and DCLK pins */
    POWER_UP_ARGS_1_CRYSTAL_OSCILLATOR_ENABLE = 0x10,

    /* Copy NVM to RAM, but do not boot */
    POWER_UP_ARGS_1_PATCH_ENABLE = 0x20,

    /* GPO2 Output Enable */
    POWER_UP_ARGS_1_GPO2_OUTPUT_ENABLE = 0x40,

    /* Trigger the GPO2 interrupt when Clear-to-Send is raised */
    POWER_UP_ARGS_1_CTS_INTERRUPT_ENABLE = 0x80,
} CMD_POWER_UP_ARGS_1;

typedef enum _CMD_POWER_UP_ARGS_2
{
    /* Analog audio output (LOUT and ROUT pins) */
    POWER_UP_ARGS_2_ANALOG_OUTPUT = 0x05,

    /* Digital audio output (DCLK, LOUT as DFS and ROUT as DOUT) */
    POWER_UP_ARGS_2_DIGITAL_OUTPUT_1 = 0x0B,

    /* Digital audio output (DCLK, DFS and DOUT through dedicated pins) */
    POWER_UP_ARGS_2_DIGITAL_OUTPUT_2 = 0xB0,

    /* Analog and digital audio output (LOUT/ROUT, DCLK, DFS and DOUT through dedicated pins) */
    POWER_UP_ARGS_2_ANALOG_AND_DIGITAL_OUTPUT = 0xB5,
} CMD_POWER_UP_ARGS_2;

typedef enum _CMD_FM_TUNE_FREQ_ARGS
{
    /* No arguments */
    FM_TUNE_FREQ_ARGS_NONE = 0x00,

    /* When set, executes a fast and invalidated tune. The tune status will not be accurate */
    FM_TUNE_FREQ_ARGS_FAST = 0x01,

    /* When set, the blend, hicut and softmute transition as a function of the associated attack/release parameters
     * rather than instantaneously when tuning to an alternate frequency */
    FM_TUNE_FREQ_ARGS_FREEZE = 0x02,
} CMD_FM_TUNE_FREQ_ARGS;

typedef enum _CMD_FM_SEEK_START_ARGS
{
    /* No arguments */
    SEEK_START_ARGS_NONE = 0x00,

    /* When set, the seek will wrap around at the end of band */
    SEEK_START_ARGS_WRAP = 0x04,

    /* When set, the seek direction is up; otherwise it's down */
    SEEK_START_ARGS_UP = 0x08
} CMD_FM_SEEK_START_ARGS;

typedef enum _CMD_GET_TUNE_STATUS_ARGS
{
    /* No arguments */
    GET_TUNE_STATUS_ARGS_NONE = 0x00,

    /* When set, clears the seek/tune complete interrupt status indicator bit */
    GET_TUNE_STATUS_ARGS_INTACK = 0x01,

    /* When set, aborts the seek that is currently in progress */
    GET_TUNE_STATUS_ARGS_CANCEL = 0x02
} CMD_GET_TUNE_STATUS_ARGS;

typedef enum _CMD_FM_RSQ_STATUS_ARGS
{
    /* No arguments */
    FM_RSQ_STATUS_ARGS_NONE = 0x00,

    /* When set, clears the RSQINT, BLENDINT, SNRHINT, SNRLINT,
     * RSSIHINT, RSSILINT, MULTHINT and MULTLINT interrupt bits
     */
    FM_RSQ_STATUS_ARGS_INTACK = 0x01,
} CMD_FM_RSQ_STATUS_ARGS;

typedef enum _CMD_GPIO_CTL_ARGS
{
    /* When set, GPO3 Output is enabled. When unset, the output is high impedance (floating) */
    GPIO_CTL_GPO3_OUTPUT_ENABLE = 0x80,

    /* When set, GPO2 Output is enabled. When unset, the output is high impedance (floating) */
    GPIO_CTL_GPO2_OUTPUT_ENABLE = 0x40,

    /* When set, GPO1 Output is enabled. When unset, the output is high impedance (floating) */
    GPIO_CTL_GPO1_OUTPUT_ENABLE = 0x20,
} CMD_GPIO_CTL_ARGS;

typedef enum _CMD_GPIO_SET_ARGS
{
    /* When set, GPO3 Output is driven high. When unset, it is driven low. */
    GPIO_SET_GPO3_OUTPUT_HIGH = 0x80,

    /* When set, GPO2 Output is driven high. When unset, it is driven low. */
    GPIO_SET_GPO2_OUTPUT_HIGH = 0x40,

    /* When set, GPO1 Output is driven high. When unset, it is driven low. */
    GPIO_SET_GPO1_OUTPUT_HIGH = 0x20,
} CMD_GPIO_SET_ARGS;

typedef struct _GetRevisionResponse_t
{
    /* Final 2 digits of part number, in hex */
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

typedef struct _RSQStatusResponse_t
{
    /* When set, the blend goes above or below the blend threshold settings */
    bool blendInt;

    /* When set, the detected multipath value has exceeded above the high threshold setting */
    bool multHInt;

    /* When set, the detected multipath value has fallen below the low threshold setting */
    bool multLInt;

    /* When set, the Signal to Noise ratio has exceeded the high threshold setting */
    bool snrHInt;

    /* When set, the Signal to Noise ratio has fallen below the low threshold setting  */
    bool snrLInt;

    /* When set, the Received Signal Strength Indicator has exceeded the high threshold setting */
    bool rssiHInt;

    /* When set, the Received Signal Strength Indicator has fallen below the low threshold setting */
    bool rssiLInt;

    /* When set, soft mute is engaged */
    bool softMute;

    /* Set if the AFC rails */
    bool AFCRail;

    /* When set, the current channel is considered valid according to the seek/tune properties */
    bool validChannel;

    /* Indicates stereo pilot presence */
    bool pilot;

    /* Indicates the amount of stereo blend, in % (100 = full stereo, 0 = full mono) */
    uint8_t stereoBlend;

    /* Contains the current Received Signal Strength Indicator (0-127 dbµV */
    uint8_t rssi;

    /* Contains the current Signal to Noise Ratio (0-127 dB) */
    uint8_t snr;

    /* Contains the current multipath metric (0 = no multipath, 100 = full multipath) */
    uint8_t multipath;

    /* Contains the signed frequency offset, in kHz */
    int8_t frequencyOffset;
} RSQStatusResponse_t;

typedef enum _PROP_FM_SEEK_FREQ_SPACING_ARGS
{
    /* Seek is performed in 50 kHz increments */
    FM_SEEK_FREQ_SPACING_ARGS_50_KHZ = 5U,

    /* Seek is performed in 100 kHz increments */
    FM_SEEK_FREQ_SPACING_ARGS_100_KHZ = 10U,

    /* Seek is performed in 200 kHz increments */
    FM_SEEK_FREQ_SPACING_ARGS_200_KHZ = 20U
} PROP_FM_SEEK_FREQ_SPACING_ARGS;

typedef enum _StatusFlags_t
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

typedef enum _InterruptSources_t
{
    /* Do not generate interrupts when status bits become set */
    INTERRUPT_SOURCES_NONE = 0x0000,

    /* Generate interrupt when Seek/Tune Completed status bit becomes set */
    INTERRUPT_SOURCES_STCIEN = 0x0001,

    /* Generate interrupt when Radio Data System status status bit becomes set */
    INTERRUPT_SOURCES_RDSIEN = 0x0004,

    /* Generate interrupt when Received Signal Quality status bit becomes set */
    INTERRUPT_SOURCES_RSQIEN = 0x0008,

    /* Generate interrupt when Error status bit bit becomes set */
    INTERRUPT_SOURCES_ERRIEN = 0x0040,

    /* Generate interrupt when Clear-to-Send status bit becomes set (see also PowerUp command) */
    INTERRUPT_SOURCES_CTSIEN = 0x0080,

    /* Helper value to enable all interrupts */
    INTERRUPT_SOURCES_ALLIEN = 0x00CD,

    /* Generate interrupt even if Seek/Tune Completed status bit was already set */
    INTERRUPT_SOURCES_STCREP = 0x0100,

    /* Generate interrupt even if Radio Data System status bit was already set  */
    INTERRUPT_SOURCES_RDSREP = 0x0400,

    /* Generate interrupt even if Received Signal Quality status bit was already set  */
    INTERRUPT_SOURCES_RSQREP = 0x0800,

    /* Helper value to enable all repeating interrupts */
    INTERRUPT_SOURCES_ALLREP = 0x0D00,

    /* Helper value to enable all interrupts, repeating ones included */
    INTERRUPT_SOURCES_ALL = INTERRUPT_SOURCES_ALLIEN | INTERRUPT_SOURCES_ALLREP,

    /* Sentinel value, to encourage uint16_t */
    INTERRUPT_SOURCES_SENTINEL = 0xFFFF
} __attribute__((__packed__)) InterruptSources_t;

typedef enum _CommandIdentifiers_t
{
    // This is not really a command
    CMD_ID_NONE = 0x00,

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

    /* Sentinel value, to encourage uint8_t */
    CMD_ID_SENTINEL = 0xFF
} __attribute__((__packed__)) CommandIdentifiers_t;

typedef enum _PropertyIdentifiers_t
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

    /* Sentinel value, to encourage uint16_t */
    PROP_ID_SENTINEL = 0xFFFF
} __attribute__((__packed__)) PropertyIdentifiers_t;

/* Exported functions prototypes ---------------------------------------------*/

/* Private defines -----------------------------------------------------------*/
#define SI4705_I2C_ADDRESS                                                                                             \
    (uint16_t)(0x11 << 1) /* Hard-coded address for the Si4705 device, left-shifted for HAL-compatibility */
#define SI4705_I2C_ALT_ADDRESS                                                                                         \
    (uint16_t)(0x63 << 1) /* Hard-coded alternative address for the Si4705 device, left-shifted for HAL-compatibility  \
                           */
#define SI4705_VOLUME_MAX_SETTING 63
#define SI4705_VOLUME_MIN_SETTING 0

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __COMMON_H__ */

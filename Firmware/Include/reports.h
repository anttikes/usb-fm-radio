/**
 ******************************************************************************
 * @file    reports.h
 * @brief   Header for the HID report API exposed by the device.
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
#ifndef __REPORTS_H__
#define __REPORTS_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

/* Exported types */
typedef enum _REPORT_IDENTIFIERS : uint8_t
{
    REPORT_IDENTIFIER_RADIO_STATUS = 0x01,
    REPORT_IDENTIFIER_RSQ_STATUS = 0x02,
    REPORT_IDENTIFIER_TUNE_FREQ = 0x03,
    REPORT_IDENTIFIER_SEEK_START = 0x04,
} REPORT_IDENTIFIERS;

typedef enum _RadioState_t : uint8_t
{
    /* Radio is in power-down state; send PowerUp command to wake it */
    RADIOSTATE_POWERDOWN = 0x00,

    /* Radio is in power-up state; set properties, tune to station, begin seek or send power-down to power it down */
    RADIOSTATE_POWERUP = 0x01,

    /* Radio is tuning to a station */
    RADIOSTATE_TUNE_IN_PROGRESS = 0x02,

    /* Radio is tuned to a station, and is receiving a signal */
    RADIOSTATE_TUNED_TO_STATION = 0x03,

    /* Radio is sampling and sending digital output of the received audio signal */
    RADIOSTATE_DIGITAL_OUTPUT_ENABLED = 0x04
} RadioState_t;

typedef struct _RadioStatus_t
{
    /* Holds the current state of the device */
    RadioState_t currentState;

    /* Holds the current frequency of the device, when tuned to a station */
    uint16_t currentFrequency;

    /* Holds the current volume level of the device */
    uint8_t currentVolume;

    /* Holds the mute status of the device */
    bool isMuted;
} RadioStatus_t;

typedef struct _RSQStatus_t
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
} RSQStatus_t;

typedef struct _TuneFreqRequest_t
{
    /* Frequency to which the radio should tune itself, in 10 kHz increments */
    uint16_t frequency;
} TuneFreqRequest_t;

typedef struct _SeekStartRequest_t
{
    /* When set, the seek will wrap around at the end of band */
    bool wrap;

    /* When set, the seek direction is up; otherwise it's down */
    bool seekUp;
} SeekStartRequest_t;

/* Exported constants --------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __REPORTS_H__ */
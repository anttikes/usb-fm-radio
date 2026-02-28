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

/* Includes ------------------------------------------------------------------*/
#if defined __cplusplus
#include <QtQml/qqmlregistration.h>
#endif /* __cplusplus */

#include <stdbool.h>
#include <stdint.h>

/* The HID report size includes the identifier and the struct bytes */
#define MAX_REPORT_SIZE 17
#define MAX_STRUCT_SIZE MAX_REPORT_SIZE - 1

/* Exported types */
typedef enum _ReportIdentifier_t : uint8_t
{
    /* Indicates that the command queue was reset due to overflow */
    REPORT_IDENTIFIER_QUEUE_RESET = 0x01,

    /* Indentifies a radio status report */
    REPORT_IDENTIFIER_RADIO_STATUS = 0x02,

    /* Identifies an interrupt status report */
    REPORT_IDENTIFIER_INTERRUPT_STATUS = 0x03,

    /* Identifies an RSQ status report */
    REPORT_IDENTIFIER_RSQ_STATUS = 0x04,

    /* Indicates a request to tune to a new frequency */
    REPORT_IDENTIFIER_TUNE_FREQ = 0x20,

    /* Indicates a request to begin seeking the next station */
    REPORT_IDENTIFIER_SEEK_START = 0x21,
} ReportIdentifier_t;

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

typedef struct _RadioStatusResponse_t
{
#if defined __cplusplus
    Q_GADGET

    Q_PROPERTY(RadioState_t currentState MEMBER currentState)
    Q_PROPERTY(uint16_t currentFrequency MEMBER currentFrequency)
    Q_PROPERTY(uint8_t currentVolume MEMBER currentVolume)
    Q_PROPERTY(bool isMuted MEMBER isMuted)
#endif /* __cplusplus */

    /* Holds the current state of the device */
    RadioState_t currentState;

    /* Holds the current frequency of the device when tuned to a station, in 10 kHz increments */
    uint16_t currentFrequency;

    /* Holds the current volume level of the device */
    uint8_t currentVolume;

    /* Holds the mute status of the device */
    bool isMuted;
} RadioStatusResponse_t;

static_assert(sizeof(RadioStatusResponse_t) <= MAX_STRUCT_SIZE);

typedef struct _GetIntStatusResponse_t
{
#if defined __cplusplus
    Q_GADGET

    Q_PROPERTY(bool clearToSend MEMBER clearToSend)
    Q_PROPERTY(bool error MEMBER error)
    Q_PROPERTY(bool rsqInterrupt MEMBER rsqInterrupt)
    Q_PROPERTY(bool rdsInterrupt MEMBER rdsInterrupt)
    Q_PROPERTY(bool seekTuneCompletedInterrupt MEMBER seekTuneCompletedInterrupt)
#endif /* __cplusplus */

    /* When set, the device is ready to receive the next command */
    bool clearToSend;

    /* When set, the device has encountered an error */
    bool error;

    /* When set, a Received Signal Quality interrupt has been triggered */
    bool rsqInterrupt;

    /* When set, an Radio Data System interrupt has been triggered */
    bool rdsInterrupt;

    /* When set, the Seek/Tune Complete interrupt has been triggered */
    bool seekTuneCompletedInterrupt;
} GetIntStatusResponse_t;

static_assert(sizeof(GetIntStatusResponse_t) <= MAX_STRUCT_SIZE);

typedef struct _RSQStatusResponse_t
{
#if defined __cplusplus
    Q_GADGET

    Q_PROPERTY(bool blendInt MEMBER blendInt)
    Q_PROPERTY(bool multHInt MEMBER multHInt)
    Q_PROPERTY(bool multLInt MEMBER multLInt)
    Q_PROPERTY(bool snrHInt MEMBER snrHInt)
    Q_PROPERTY(bool snrLInt MEMBER snrLInt)
    Q_PROPERTY(bool rssiHInt MEMBER rssiHInt)
    Q_PROPERTY(bool rssiLInt MEMBER rssiLInt)
    Q_PROPERTY(bool softMute MEMBER softMute)
    Q_PROPERTY(bool AFCRail MEMBER AFCRail)
    Q_PROPERTY(bool validChannel MEMBER validChannel)
    Q_PROPERTY(bool pilot MEMBER pilot)
    Q_PROPERTY(uint8_t stereoBlend MEMBER stereoBlend)
    Q_PROPERTY(uint8_t rssi MEMBER rssi)
    Q_PROPERTY(uint8_t snr MEMBER snr)
    Q_PROPERTY(uint8_t multipath MEMBER multipath)
    Q_PROPERTY(int8_t frequencyOffset MEMBER frequencyOffset)
#endif /* __cplusplus */

    /* When set, the blend goes above or below the blend threshold settings */
    bool blendInt;

    /* When set, the detected multipath value has exceeded above the high threshold setting */
    bool multHInt;

    /* When set, the detected multipath value has fallen below the low threshold setting */
    bool multLInt;

    /* When set, the Signal to Noise ratio has exceeded the high threshold setting */
    bool snrHInt;

    /* When set, the Signal to Noise ratio has fallen below the low threshold setting */
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

    /* Contains the current Received Signal Strength Indicator (0-127 dbµV) */
    uint8_t rssi;

    /* Contains the current Signal to Noise Ratio (0-127 dB) */
    uint8_t snr;

    /* Contains the current multipath metric (0 = no multipath, 100 = full multipath) */
    uint8_t multipath;

    /* Contains the signed frequency offset, in kHz */
    int8_t frequencyOffset;
} RSQStatusResponse_t;

static_assert(sizeof(RSQStatusResponse_t) <= MAX_STRUCT_SIZE);

typedef struct _TuneFreqRequest_t
{
    /* Frequency to which the radio should tune itself, in 10 kHz increments */
    uint16_t frequency;
} TuneFreqRequest_t;

static_assert(sizeof(TuneFreqRequest_t) <= MAX_STRUCT_SIZE);

typedef struct _SeekStartRequest_t
{
    /* When set, the seek will wrap around at the end of band */
    bool wrap;

    /* When set, the seek direction is up; otherwise it's down */
    bool seekUp;
} SeekStartRequest_t;

static_assert(sizeof(SeekStartRequest_t) <= MAX_STRUCT_SIZE);

typedef struct _Report_t
{
    /* Identifier of the report */
    ReportIdentifier_t identifier;

    /* Report bytes */
    union ReportBytes {
        RadioStatusResponse_t radioStatus;
        GetIntStatusResponse_t interruptStatus;
        RSQStatusResponse_t rsqStatus;
        TuneFreqRequest_t tuneFreqRequest;
        SeekStartRequest_t seekStartRequest;

        // This ensures any "sizeof(bytes)" will return the proper size
        uint8_t raw[MAX_STRUCT_SIZE];
    } bytes;
} Report_t;

/* Exported constants --------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

#endif /* __REPORTS_H__ */
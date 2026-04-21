/**
 ******************************************************************************
 * @file    properties.h
 * @brief   Header for properties.c
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
#ifndef __PROPERTIES_H__
#define __PROPERTIES_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "device.h"

/* Exported types */
typedef enum _PROP_GPO_IEN_ARGS : uint16_t
{
    /* Do not generate interrupts when status bits become set */
    GPO_IEN_ARGS_NONE = 0x0000,

    /* Generate interrupt when Seek/Tune Completed status bit becomes set */
    GPO_IEN_ARGS_STCIEN = 0x0001,

    /* Generate interrupt when Radio Data System status status bit becomes set */
    GPO_IEN_ARGS_RDSIEN = 0x0004,

    /* Generate interrupt when Received Signal Quality status bit becomes set */
    GPO_IEN_ARGS_RSQIEN = 0x0008,

    /* Generate interrupt when Error status bit bit becomes set */
    GPO_IEN_ARGS_ERRIEN = 0x0040,

    /* Generate interrupt when Clear-to-Send status bit becomes set (see also PowerUp command) */
    GPO_IEN_ARGS_CTSIEN = 0x0080,

    /* Helper value to enable all interrupts */
    GPO_IEN_ARGS_ALLIEN = 0x00CD,

    /* Generate interrupt even if Seek/Tune Completed status bit was already set */
    GPO_IEN_ARGS_STCREP = 0x0100,

    /* Generate interrupt even if Radio Data System status bit was already set  */
    GPO_IEN_ARGS_RDSREP = 0x0400,

    /* Generate interrupt even if Received Signal Quality status bit was already set  */
    GPO_IEN_ARGS_RSQREP = 0x0800,

    /* Helper value to enable all repeating interrupts */
    GPO_IEN_ARGS_ALLREP = 0x0D00,

    /* Helper value to enable all interrupts, repeating ones included */
    GPO_IEN_ARGS_ALL = GPO_IEN_ARGS_ALLIEN | GPO_IEN_ARGS_ALLREP,
} PROP_GPO_IEN_ARGS;

typedef enum _PROP_FM_SEEK_FREQ_SPACING_ARGS : uint16_t
{
    /* Seek is performed in 50 kHz increments */
    FM_SEEK_FREQ_SPACING_ARGS_50_KHZ = 5U,

    /* Seek is performed in 100 kHz increments */
    FM_SEEK_FREQ_SPACING_ARGS_100_KHZ = 10U,

    /* Seek is performed in 200 kHz increments */
    FM_SEEK_FREQ_SPACING_ARGS_200_KHZ = 20U
} PROP_FM_SEEK_FREQ_SPACING_ARGS;

typedef enum _PROP_FM_DEEMPHASIS_ARGS : uint16_t
{
    /* 75 µs; used in USA */
    FM_DEEMPHASIS_ARGS_75_MICROSECONDS = 0x0002,

    /* 50 µs; used in Europe, Australia and Japan */
    FM_DEEMPHASIS_ARGS_50_MICROSECONDS = 0x0001,
} PROP_FM_DEEMPHASIS_ARGS;

typedef enum _PROP_RX_HARD_MUTE_ARGS : uint16_t
{
    /* Unmute both channels*/
    RX_HARD_MUTE_ARGS_NONE = 0x0000,

    /* Mute left channel */
    RX_HARD_MUTE_ARGS_LEFT = 0x0002,

    /* Mute right channel */
    RX_HARD_MUTE_ARGS_RIGHT = 0x0001,

    /* Helper value to mute both */
    RX_HARD_MUTE_ARGS_BOTH = RX_HARD_MUTE_ARGS_LEFT | RX_HARD_MUTE_ARGS_RIGHT,
} PROP_RX_HARD_MUTE_ARGS;

typedef enum _PROP_REFCLK_PRESCALE_ARGS : uint16_t
{
    /* Reference clock source is the RCLK pin */
    REFCLK_PRESCALE_ARGS_RCLK = 0x0000,

    /* Reference clock source is the DCLK pin */
    REFCLK_PRESCALE_ARGS_DCLK = 0x1000,
} PROP_REFCLK_PRESCALE_ARGS;

typedef enum _PROP_FM_RDS_INT_SOURCE_ARGS : uint16_t
{
    /* If set, generate an RDS interrupt when Block B data is found or subsequently changed */
    FM_RDS_INT_SOURCE_ARGS_RDSNEWBLOCKB = 0x0020,

    /* If set, generate an RDS interrupt when Block A data is found or subsequently changed */
    FM_RDS_INT_SOURCE_ARGS_RDSNEWBLOCKA = 0x0010,

    /* If set, generate an RDS interrupt when RDS gains synchronization */
    FM_RDS_INT_SOURCE_ARGS_RDSSYNCFOUND = 0x0004,

    /* If set, generate an RDS interrupt when RDS loses synchronization */
    FM_RDS_INT_SOURCE_ARGS_RDSSYNCLOST = 0x0002,

    /* If set, generate an RDS interrupt when RDS FIFO has at least FM_RDS_INT_FIFO_COUNT entries are available */
    FM_RDS_INT_SOURCE_ARGS_RDSRECV = 0x0001,
} PROP_FM_RDS_INT_SOURCE_ARGS;

typedef enum _PROP_FM_RDS_CONFIG_ARGS : uint16_t
{
    /* Block Error Threshold BLOCKA: Accept even when the block cannot be error-corrected */
    FM_RDS_CONFIG_ARGS_BLETHA_ERROR_MAX_UNCORRECTABLE = 0xC000,

    /* Block Error Threshold BLOCKA: 3-5 bit errors detected and corrected */
    FM_RDS_CONFIG_ARGS_BLETHA_ERROR_MAX_5_BITS = 0x8000,

    /* Block Error Threshold BLOCKA: 1-2 bit errors detected and corrected */
    FM_RDS_CONFIG_ARGS_BLETHA_ERROR_MAX_2_BITS = 0x4000,

    /* Block Error Threshold BLOCKB: Accept even when the block cannot be error-corrected */
    FM_RDS_CONFIG_ARGS_BLETHB_ERROR_MAX_UNCORRECTABLE = 0x3000,

    /* Block Error Threshold BLOCKB: 3-5 bit errors detected and corrected */
    FM_RDS_CONFIG_ARGS_BLETHB_ERROR_MAX_5_BITS = 0x2000,

    /* Block Error Threshold BLOCKB: 1-2 bit errors detected and corrected */
    FM_RDS_CONFIG_ARGS_BLETHB_ERROR_MAX_2_BITS = 0x1000,

    /* Block Error Threshold BLOCKC: Accept even when the block cannot be error-corrected */
    FM_RDS_CONFIG_ARGS_BLETHC_ERROR_MAX_UNCORRECTABLE = 0x0C00,

    /* Block Error Threshold BLOCKC: 3-5 bit errors detected and corrected */
    FM_RDS_CONFIG_ARGS_BLETHC_ERROR_MAX_5_BITS = 0x0800,

    /* Block Error Threshold BLOCKC: 1-2 bit errors detected and corrected */
    FM_RDS_CONFIG_ARGS_BLETHC_ERROR_MAX_2_BITS = 0x0400,

    /* Block Error Threshold BLOCKD: Accept even when the block cannot be error-corrected */
    FM_RDS_CONFIG_ARGS_BLETHD_ERROR_MAX_UNCORRECTABLE = 0x0300,

    /* Block Error Threshold BLOCKD: 3-5 bit errors detected and corrected */
    FM_RDS_CONFIG_ARGS_BLETHD_ERROR_MAX_5_BITS = 0x0200,

    /* Block Error Threshold BLOCKD: 1-2 bit errors detected and corrected */
    FM_RDS_CONFIG_ARGS_BLETHD_ERROR_MAX_2_BITS = 0x0100,

    /* If set, RDS processing is enabled */
    FM_RDS_CONFIG_ARGS_RDS_ENABLE = 0x0001,
} PROP_FM_RDS_CONFIG_ARGS;

/* Exported constants --------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Enqueues the "SET PROPERTY" command with "GPO_IEN" to configure which
 *         interrupt sources are enabled for the GPO2 pin.
 * @param  device Pointer to the radio device structure
 * @param  args Additional interrupt sources to enable
 *
 * @retval True if the command was enqueued; false otherwise
 */
static inline bool SetInterruptSources(RadioDevice_t *device, PROP_GPO_IEN_ARGS args)
{
    return SetProperty(device, PROP_ID_GPO_IEN, args);
}

/**
 * @brief  Enqueues the "SET PROPERTY" command with "FM_SEEK_FREQ_SPACING" to configure
 *         the desired spacing between FM channels.
 * @param  device Pointer to the radio device structure
 * @param  args Desired spacing between FM channels
 *
 * @retval True if the command was enqueued; false otherwise
 */
static inline bool SetFMSeekFrequencySpacing(RadioDevice_t *device, PROP_FM_SEEK_FREQ_SPACING_ARGS args)
{
    return SetProperty(device, PROP_ID_FM_SEEK_FREQ_SPACING, args);
}

/**
 * @brief  Enqueues the "SET PROPERTY" command with "FM_DEEMPHASIS" to configure the
 *         FM de-emphasis level.
 * @param  device Pointer to the radio device structure
 * @param  args Desired FM de-emphasis level
 *
 * @retval True if the command was enqueued; false otherwise
 */
static inline bool SetFMDeemphasis(RadioDevice_t *device, PROP_FM_DEEMPHASIS_ARGS args)
{
    return SetProperty(device, PROP_ID_FM_DEEMPHASIS, args);
}

/**
 * @brief  Enqueues the "SET PROPERTY" command with "RX_VOLUME" to configure the volume level.
 * @param  device Pointer to the radio device structure
 * @param  args Desired volume level, between 0 and 63 inclusive
 *
 * @retval True if the command was enqueued; false otherwise
 */
static inline bool SetVolume(RadioDevice_t *device, uint8_t args)
{
    if (args < SI4705_VOLUME_MIN_SETTING || args > SI4705_VOLUME_MAX_SETTING)
    {
        return false;
    }

    return SetProperty(device, PROP_ID_RX_VOLUME, args);
}

/**
 * @brief  Enqueues the "SET PROPERTY" command with "RX_HARD_MUTE" to mute the radio
 * @param  device Pointer to the radio device structure
 * @param  args Desired mute option
 *
 * @retval True if the command was enqueued; false otherwise
 */
static inline bool SetMute(RadioDevice_t *device, PROP_RX_HARD_MUTE_ARGS args)
{
    return SetProperty(device, PROP_ID_RX_HARD_MUTE, args);
}

/**
 * @brief  Enqueues the "SET PROPERTY" command with "REFCLK_PRESCALE" to configure the
 *         reference clock pin, and the prescaler value for the radio
 * @param  device Pointer to the radio device structure
 * @param  args1 Clock source Additional interrupt sources to enable
 * @param  args2 Prescaler value, between 1 and 4095 inclusive
 *
 * @retval True if the command was enqueued; false otherwise
 */
static inline bool SetReferenceClockPrescaler(RadioDevice_t *device, PROP_REFCLK_PRESCALE_ARGS args1, uint16_t args2)
{
    if (args2 < SI4705_REFCLK_PRESCALE_MIN_SETTING || args2 > SI4705_REFCLK_PRESCALE_MAX_SETTING)
    {
        return false;
    }

    return SetProperty(device, PROP_ID_REFCLK_PRESCALE, args1 | args2);
}

/**
 * @brief  Enqueues the "SET PROPERTY" command with "FM_RDS_INT_SOURCE" to configure
 *         which events cause the RDSRECV bit (and interrupt) to get set.
 * @param  device Pointer to the radio device structure
 * @param  args RDS interrupt sources to enable
 *
 * @retval True if the command was enqueued; false otherwise
 */
static inline bool SetRDSInterruptSources(RadioDevice_t *device, PROP_FM_RDS_INT_SOURCE_ARGS args)
{
    return SetProperty(device, PROP_ID_FM_RDS_INT_SOURCE, args);
}

/**
 * @brief  Enqueues the "SET PROPERTY" command with "FM_RDS_INT_FIFO_COUNT" to configure the
 *         minimum number of RDS group stored in the RDS FIFO before RDSRECV bit (and interrupt) is set.
 * @param  device Pointer to the radio device structure
 * @param  args Minimum number of RDS groups, between 0 and 25 inclusive
 *
 * @retval True if the command was enqueued; false otherwise
 */
static inline bool SetRDSFIFOCount(RadioDevice_t *device, uint8_t args)
{
    if (args > 25)
    {
        return false;
    }

    return SetProperty(device, PROP_ID_FM_RDS_INT_FIFO_COUNT, args);
}

/**
 * @brief  Enqueues the "SET PROPERTY" command with "FM_RDS_CONFIG" to configure the
 *         RDS error thresholds and to enable the RDS processing.
 * @param  device Pointer to the radio device structure
 * @param  args RDS error thresholds and enable or disable bit
 *
 * @retval True if the command was enqueued; false otherwise
 */
static inline bool SetRDSConfig(RadioDevice_t *device, PROP_FM_RDS_CONFIG_ARGS args)
{
    return SetProperty(device, PROP_ID_FM_RDS_CONFIG, args);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PROPERTIES_H__ */

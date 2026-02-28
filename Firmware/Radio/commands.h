/**
 ******************************************************************************
 * @file    commands.h
 * @brief   Header for commands.c
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
#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "common.h"
#include "device.h"

/* Exported types */
typedef enum _CMD_POWER_UP_ARGS_1 : uint8_t
{
    /* Execute the "FM Receive" operation */
    POWER_UP_ARGS_1_FUNCTION_FM = 0x00,

    /* Execute the "Query library ID" operation */
    POWER_UP_ARGS_1_FUNCTION_QUERY_LIBRARY_ID = 0x0F,

    /* Use the crystal oscillator via GPO3 and DCLK pins */
    POWER_UP_ARGS_1_CRYSTAL_OSCILLATOR_ENABLE = 0x10,

    /* Copy NVM to RAM, but do not boot */
    POWER_UP_ARGS_1_PATCH_ENABLE = 0x20,

    /* GPO2 Output Enable */
    POWER_UP_ARGS_1_GPO2_OUTPUT_ENABLE = 0x40,

    /* Trigger the GPO2 interrupt when Clear-to-Send is raised */
    POWER_UP_ARGS_1_CTS_INTERRUPT_ENABLE = 0x80,
} CMD_POWER_UP_ARGS_1;

typedef enum _CMD_POWER_UP_ARGS_2 : uint8_t
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

typedef enum _CMD_FM_TUNE_FREQ_ARGS : uint8_t
{
    /* No arguments */
    FM_TUNE_FREQ_ARGS_NONE = 0x00,

    /* When set, executes a fast and invalidated tune. The tune status will not be accurate */
    FM_TUNE_FREQ_ARGS_FAST = 0x01,

    /* When set, the blend, hicut and softmute transition as a function of the associated attack/release parameters
     * rather than instantaneously when tuning to an alternate frequency */
    FM_TUNE_FREQ_ARGS_FREEZE = 0x02,
} CMD_FM_TUNE_FREQ_ARGS;

typedef enum _CMD_FM_SEEK_START_ARGS : uint8_t
{
    /* No arguments */
    SEEK_START_ARGS_NONE = 0x00,

    /* When set, the seek will wrap around at the end of band */
    SEEK_START_ARGS_WRAP = 0x04,

    /* When set, the seek direction is up; otherwise it's down */
    SEEK_START_ARGS_UP = 0x08
} CMD_FM_SEEK_START_ARGS;

typedef enum _CMD_GET_TUNE_STATUS_ARGS : uint8_t
{
    /* No arguments */
    GET_TUNE_STATUS_ARGS_NONE = 0x00,

    /* When set, clears the seek/tune complete interrupt status indicator bit */
    GET_TUNE_STATUS_ARGS_INTACK = 0x01,

    /* When set, aborts the seek that is currently in progress */
    GET_TUNE_STATUS_ARGS_CANCEL = 0x02
} CMD_GET_TUNE_STATUS_ARGS;

typedef enum _CMD_FM_RSQ_STATUS_ARGS : uint8_t
{
    /* No arguments */
    FM_RSQ_STATUS_ARGS_NONE = 0x00,

    /* When set, clears the RSQINT, BLENDINT, SNRHINT, SNRLINT,
     * RSSIHINT, RSSILINT, MULTHINT and MULTLINT interrupt bits
     */
    FM_RSQ_STATUS_ARGS_INTACK = 0x01,
} CMD_FM_RSQ_STATUS_ARGS;

typedef enum _CMD_GPIO_CTL_ARGS : uint8_t
{
    /* All GPOs are set to high impedance (floating) */
    GPIO_CTL_GPO_OUTPUT_DISABLE = 0x00,

    /* When set, GPO1 Output is enabled. When unset, the output is high impedance (floating) */
    GPIO_CTL_GPO1_OUTPUT_ENABLE = 0x20,

    /* When set, GPO2 Output is enabled. When unset, the output is high impedance (floating) */
    GPIO_CTL_GPO2_OUTPUT_ENABLE = 0x40,

    /* When set, GPO3 Output is enabled. When unset, the output is high impedance (floating) */
    GPIO_CTL_GPO3_OUTPUT_ENABLE = 0x80,
} CMD_GPIO_CTL_ARGS;

typedef enum _CMD_GPIO_SET_ARGS : uint8_t
{
    /* All GPOs are driven low */
    GPIO_SET_GPO_OUTPUT_NONE = 0x00,

    /* When set, GPO1 Output is driven high. When unset, it is driven low. */
    GPIO_SET_GPO1_OUTPUT_HIGH = 0x20,

    /* When set, GPO2 Output is driven high. When unset, it is driven low. */
    GPIO_SET_GPO2_OUTPUT_HIGH = 0x40,

    /* When set, GPO3 Output is driven high. When unset, it is driven low. */
    GPIO_SET_GPO3_OUTPUT_HIGH = 0x80,
} CMD_GPIO_SET_ARGS;

/* Exported constants --------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
extern bool PowerUp(RadioDevice_t *device, CMD_POWER_UP_ARGS_1 arg1, CMD_POWER_UP_ARGS_2 arg2);
extern bool PowerDown(RadioDevice_t *device);
extern bool GetRevision(RadioDevice_t *device);
extern bool SetProperty(RadioDevice_t *device, PropertyIdentifiers_t property, uint16_t value);
extern bool GetProperty(RadioDevice_t *device, PropertyIdentifiers_t property);
extern bool GetIntStatus(RadioDevice_t *device);
extern bool TuneFreq(RadioDevice_t *device, CMD_FM_TUNE_FREQ_ARGS args, uint16_t frequency);
extern bool SeekStart(RadioDevice_t *device, CMD_FM_SEEK_START_ARGS args);
extern bool TuneStatus(RadioDevice_t *device, CMD_GET_TUNE_STATUS_ARGS args);
extern bool RSQStatus(RadioDevice_t *device, CMD_FM_RSQ_STATUS_ARGS args);
extern bool GPIOCtl(RadioDevice_t *device, CMD_GPIO_CTL_ARGS args);
extern bool GPIOSet(RadioDevice_t *device, CMD_GPIO_SET_ARGS args);

extern bool ProcessIntStatus(RadioDevice_t *device, Command_t *command);
extern bool ProcessRSQStatus(RadioDevice_t *device, Command_t *command);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __COMMANDS_H__ */

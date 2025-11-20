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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "common.h"
#include "device.h"

/* Exported functions prototypes ---------------------------------------------*/
void WaitForStatus(volatile RadioDevice_t *pRadioDevice, StatusFlags_t statusToWait);

bool PowerUp(volatile RadioDevice_t *pRadioDevice, CMD_POWER_UP_ARGS_1 arg1, CMD_POWER_UP_ARGS_2 arg2);
bool SetInterruptSources(volatile RadioDevice_t *pRadioDevice, InterruptSources_t sources);
bool PowerDown(volatile RadioDevice_t *pRadioDevice);
bool GetRevision(volatile RadioDevice_t *pRadioDevice, GetRevisionResponse_t *pResponse);
bool SetProperty(volatile RadioDevice_t *pRadioDevice, PropertyIdentifiers_t property, uint16_t value);
bool GetProperty(volatile RadioDevice_t *pRadioDevice, PropertyIdentifiers_t property, uint16_t *pValue);
bool GetIntStatus(volatile RadioDevice_t *pRadioDevice, StatusFlags_t *pValue);
bool TuneFreq(volatile RadioDevice_t *pRadioDevice, CMD_FM_TUNE_FREQ_ARGS args, uint16_t frequency);
bool SeekStart(volatile RadioDevice_t *pRadioDevice, CMD_FM_SEEK_START_ARGS args);
bool GetTuneStatus(volatile RadioDevice_t *pRadioDevice, CMD_GET_TUNE_STATUS_ARGS args,
                   GetTuneStatusResponse_t *pResponse);
bool RSQStatus(volatile RadioDevice_t *pRadioDevice, CMD_FM_RSQ_STATUS_ARGS args, RSQStatusResponse_t *pResponse);

/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __COMMANDS_H__ */

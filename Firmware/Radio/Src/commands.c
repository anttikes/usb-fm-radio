/**
 ******************************************************************************
 * @file    commands.c
 * @brief   Implements command wrapper calls
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
/* Includes ------------------------------------------------------------------*/
#include "commands.h"
#include "common.h"
#include "i2c.h"
#include "stm32f0xx_hal_def.h"
#include "stm32f0xx_hal_i2c.h"
#include <stdint.h>

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/

/**
 * @brief  Synchronously waits for the specified status bit to be set via I2C polling
 * @param  pRadioDevice Pointer to the radio device structure
 * @param  statusToWait Status bit to wait for
 *
 * @retval None
 */
void WaitForStatus(volatile RadioDevice_t *pRadioDevice, StatusFlags_t statusToWait)
{
    while (true)
    {
        HAL_StatusTypeDef status;
        StatusFlags_t latestStatus = STATUS_NONE;

        // When waiting for any other status except CTS we must use GetIntStatus command to update the status byte
        if (statusToWait != STATUS_CLEAR_TO_SEND)
        {
            status = GetIntStatus(pRadioDevice, &latestStatus);
        }
        else
        {
            status =
                HAL_I2C_Master_Receive(&hi2c1, radioDevice.deviceAddress, (uint8_t *)&latestStatus, 1, HAL_MAX_DELAY);
        }

        if (status != HAL_OK)
        {
            // TODO: Break out, and raise error?
        }

        if (latestStatus & statusToWait)
        {
            break;
        }

        HAL_Delay(1);
    }
}

/**
 * @brief  Sends the "Power up" command to the Si4705 chip and waits until CTS becomes set
 * @param  pRadioDevice Pointer to the radio device structure
 * @param  arg1 First set of arguments for the command
 * @param  arg2 Second set of arguments for the command
 * @param  pResponse In case of "QueryLibraryId" variant, points to the structure where return data should be placed
 *
 * @retval True if the command succeeded; false otherwise
 */
bool PowerUp(volatile RadioDevice_t *pRadioDevice, CMD_POWER_UP_ARGS_1 arg1, CMD_POWER_UP_ARGS_2 arg2)
{
    if (pRadioDevice->currentState != RADIOSTATE_POWERDOWN)
    {
        return false;
    }

    while (pRadioDevice->currentCommand.state != COMMANDSTATE_IDLE)
    {
    }

    pRadioDevice->currentCommand.args.opCode = CMD_ID_POWER_UP;
    pRadioDevice->currentCommand.args.bytes[1] = arg1;
    pRadioDevice->currentCommand.args.bytes[2] = arg2;
    pRadioDevice->currentCommand.argLength = 3;
    pRadioDevice->currentCommand.responseLength = 0;

    pRadioDevice->currentCommand.state = COMMANDSTATE_SENDING;

    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit_IT(&hi2c1, pRadioDevice->deviceAddress,
                                                          (uint8_t *)&pRadioDevice->currentCommand.args.bytes[0],
                                                          pRadioDevice->currentCommand.argLength);

    if (status != HAL_OK)
    {
        return false;
    }

    while (pRadioDevice->currentCommand.state != COMMANDSTATE_READY)
    {
    }

    pRadioDevice->currentState = RADIOSTATE_POWERUP;

    pRadioDevice->currentCommand.args.bytes[0] = 0;
    pRadioDevice->currentCommand.args.bytes[1] = 0;
    pRadioDevice->currentCommand.args.bytes[2] = 0;
    pRadioDevice->currentCommand.args.bytes[3] = 0;
    pRadioDevice->currentCommand.args.bytes[4] = 0;
    pRadioDevice->currentCommand.args.bytes[5] = 0;
    pRadioDevice->currentCommand.args.bytes[6] = 0;
    pRadioDevice->currentCommand.args.bytes[7] = 0;
    pRadioDevice->currentCommand.state = COMMANDSTATE_IDLE;
    pRadioDevice->currentCommand.argLength = 0;
    pRadioDevice->currentCommand.responseLength = 0;

    return true;
}

/**
 * @brief  Uses the "GPO_IEN" property to configure the interrupt sources for status bits.
 * @param  pRadioDevice Pointer to the radio device structure
 * @param  sources Additional interrupt sources to enable
 *
 * @retval True if the command succeeded; false otherwise
 */
bool SetInterruptSources(volatile RadioDevice_t *pRadioDevice, InterruptSources_t sources)
{
    return SetProperty(pRadioDevice, PROP_ID_GPO_IEN, sources);
}

/**
 * @brief  Sends the "Power down" command to the Si4705 chip and waits until CTS becomes set
 * @param  pRadioDevice Pointer to the radio device structure
 *
 * @retval True if the command succeeded; false otherwise
 */
bool PowerDown(volatile RadioDevice_t *pRadioDevice)
{
    if (pRadioDevice->currentState == RADIOSTATE_POWERDOWN)
    {
        return true;
    }

    while (pRadioDevice->currentCommand.state != COMMANDSTATE_IDLE)
    {
    }

    pRadioDevice->currentCommand.args.opCode = CMD_ID_POWER_DOWN;
    pRadioDevice->currentCommand.argLength = 1;
    pRadioDevice->currentCommand.responseLength = 0;

    pRadioDevice->currentCommand.state = COMMANDSTATE_SENDING;

    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit_IT(&hi2c1, pRadioDevice->deviceAddress,
                                                          (uint8_t *)&pRadioDevice->currentCommand.args.bytes[0],
                                                          pRadioDevice->currentCommand.argLength);

    if (status != HAL_OK)
    {
        return false;
    }

    while (pRadioDevice->currentCommand.state != COMMANDSTATE_READY)
    {
    }

    pRadioDevice->currentState = RADIOSTATE_POWERDOWN;

    pRadioDevice->currentCommand.args.bytes[0] = 0;
    pRadioDevice->currentCommand.args.bytes[1] = 0;
    pRadioDevice->currentCommand.args.bytes[2] = 0;
    pRadioDevice->currentCommand.args.bytes[3] = 0;
    pRadioDevice->currentCommand.args.bytes[4] = 0;
    pRadioDevice->currentCommand.args.bytes[5] = 0;
    pRadioDevice->currentCommand.args.bytes[6] = 0;
    pRadioDevice->currentCommand.args.bytes[7] = 0;
    pRadioDevice->currentCommand.state = COMMANDSTATE_IDLE;
    pRadioDevice->currentCommand.argLength = 0;
    pRadioDevice->currentCommand.responseLength = 0;

    return true;
}

/**
 * @brief  Sends the "Get Rev" command to the Si4705 chip and waits until CTS becomes set
 * @param  pRadioDevice Pointer to the radio device structure
 * @param  pResponse Pointer to the structure representing the data returned by the chip
 *
 * @retval True if the command succeeded; false otherwise
 */
bool GetRevision(volatile RadioDevice_t *pRadioDevice, GetRevisionResponse_t *pResponse)
{
    volatile HAL_StatusTypeDef status;
    uint8_t tx_buffer[1] = {0};
    uint8_t rx_buffer[16] = {0};

    if (pResponse == NULL)
    {
        return HAL_ERROR;
    }

    WaitForStatus(pRadioDevice, STATUS_CLEAR_TO_SEND);

    tx_buffer[0] = CMD_ID_GET_REV;

    status = HAL_I2C_Master_Transmit(&hi2c1, pRadioDevice->deviceAddress, tx_buffer, sizeof(tx_buffer), HAL_MAX_DELAY);

    if (status != HAL_OK)
    {
        return false;
    }

    WaitForStatus(pRadioDevice, STATUS_CLEAR_TO_SEND);

    status = HAL_I2C_Master_Receive(&hi2c1, pRadioDevice->deviceAddress, rx_buffer, sizeof(rx_buffer), HAL_MAX_DELAY);

    pResponse->productNumber = rx_buffer[1];
    pResponse->firmwareMajor = rx_buffer[2];
    pResponse->firmwareMinor = rx_buffer[3];
    pResponse->patchId = (uint16_t)(rx_buffer[4] << 8 | rx_buffer[5]);
    pResponse->componentMajor = rx_buffer[6];
    pResponse->componentMinor = rx_buffer[7];
    pResponse->chipRevision = rx_buffer[8];
    pResponse->cid = rx_buffer[15];

    return true;
}

/**
 * @brief  Sends the "Set Property" command to the Si4705 chip and waits until CTS becomes set
 * @param  pRadioDevice Pointer to the radio device structure
 * @param  property Identifier of the property to set
 * @param  value New value of the property
 *
 * @retval True if the command succeeded; false otherwise
 */
bool SetProperty(volatile RadioDevice_t *pRadioDevice, PropertyIdentifiers_t property, uint16_t value)
{
    if (pRadioDevice->currentState == RADIOSTATE_POWERDOWN)
    {
        return false;
    }

    while (pRadioDevice->currentCommand.state != COMMANDSTATE_IDLE)
    {
    }

    pRadioDevice->currentCommand.args.opCode = CMD_ID_SET_PROPERTY;
    pRadioDevice->currentCommand.args.bytes[1] = 0x00;
    pRadioDevice->currentCommand.args.bytes[2] = (uint8_t)((property & 0xFF00) >> 8);
    pRadioDevice->currentCommand.args.bytes[3] = (uint8_t)((property & 0x00FF) >> 0);
    pRadioDevice->currentCommand.args.bytes[4] = (uint8_t)((value & 0xFF00) >> 8);
    pRadioDevice->currentCommand.args.bytes[5] = (uint8_t)((value & 0x00FF) >> 0);
    pRadioDevice->currentCommand.argLength = 6;
    pRadioDevice->currentCommand.responseLength = 0;

    pRadioDevice->currentCommand.state = COMMANDSTATE_SENDING;

    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit_IT(&hi2c1, pRadioDevice->deviceAddress,
                                                          (uint8_t *)&pRadioDevice->currentCommand.args.bytes[0],
                                                          pRadioDevice->currentCommand.argLength);

    if (status != HAL_OK)
    {
        return false;
    }

    while (pRadioDevice->currentCommand.state != COMMANDSTATE_READY)
    {
    }

    // Programming guide guarantees that it takes 10ms to complete the operation
    HAL_Delay(10);

    pRadioDevice->currentCommand.args.bytes[0] = 0;
    pRadioDevice->currentCommand.args.bytes[1] = 0;
    pRadioDevice->currentCommand.args.bytes[2] = 0;
    pRadioDevice->currentCommand.args.bytes[3] = 0;
    pRadioDevice->currentCommand.args.bytes[4] = 0;
    pRadioDevice->currentCommand.args.bytes[5] = 0;
    pRadioDevice->currentCommand.args.bytes[6] = 0;
    pRadioDevice->currentCommand.args.bytes[7] = 0;
    pRadioDevice->currentCommand.state = COMMANDSTATE_IDLE;
    pRadioDevice->currentCommand.argLength = 0;
    pRadioDevice->currentCommand.responseLength = 0;

    return true;
}

/**
 * @brief  Sends the "Get Property" command to the Si4705 chip and waits until CTS becomes set
 * @param  pRadioDevice Pointer to the radio device structure
 * @param  property Identifier of the property to set
 * @param  pValue Pointer to the variable that will receive the value of that the property has
 *
 * @retval True if the command succeeded; false otherwise
 */
bool GetProperty(volatile RadioDevice_t *pRadioDevice, PropertyIdentifiers_t property, uint16_t *pValue)
{
    volatile HAL_StatusTypeDef status;
    uint8_t tx_buffer[4] = {0};
    uint8_t rx_buffer[4] = {0};

    if (pValue == NULL)
    {
        return false;
    }

    WaitForStatus(pRadioDevice, STATUS_CLEAR_TO_SEND);

    tx_buffer[0] = CMD_ID_GET_PROPERTY;
    tx_buffer[1] = 0x00;
    tx_buffer[2] = (uint8_t)((property & 0xFF00) >> 8);
    tx_buffer[3] = (uint8_t)((property & 0x00FF) >> 0);

    status = HAL_I2C_Master_Transmit(&hi2c1, pRadioDevice->deviceAddress, tx_buffer, sizeof(tx_buffer), HAL_MAX_DELAY);

    if (status != HAL_OK)
    {
        return false;
    }

    WaitForStatus(pRadioDevice, STATUS_CLEAR_TO_SEND);

    status = HAL_I2C_Master_Receive(&hi2c1, pRadioDevice->deviceAddress, rx_buffer, sizeof(rx_buffer), HAL_MAX_DELAY);

    // Third and fourth bytes are the high and low bytes of the return value
    (*pValue) = (uint16_t)((rx_buffer[2] << 8) | rx_buffer[3]);

    return true;
}

/**
 * @brief  Sends the "Get Int Status" command to the Si4705 chip and waits until CTS becomes set
 * @param  pRadioDevice Pointer to the radio device structure
 * @param  property Identifier of the property to set
 *
 * @retval True if the command succeeded; false otherwise
 */
bool GetIntStatus(volatile RadioDevice_t *pRadioDevice, StatusFlags_t *pValue)
{
    volatile HAL_StatusTypeDef status;
    uint8_t tx_buffer[1] = {0};
    uint8_t rx_buffer[1] = {0};

    if (pValue == NULL)
    {
        return false;
    }

    WaitForStatus(pRadioDevice, STATUS_CLEAR_TO_SEND);

    tx_buffer[0] = CMD_ID_GET_INT_STATUS;

    status = HAL_I2C_Master_Transmit(&hi2c1, pRadioDevice->deviceAddress, tx_buffer, sizeof(tx_buffer), HAL_MAX_DELAY);

    if (status != HAL_OK)
    {
        return false;
    }

    WaitForStatus(pRadioDevice, STATUS_CLEAR_TO_SEND);

    status = HAL_I2C_Master_Receive(&hi2c1, pRadioDevice->deviceAddress, rx_buffer, sizeof(rx_buffer), HAL_MAX_DELAY);

    (*pValue) = rx_buffer[0];

    return true;
}

/**
 * @brief  Sends the "FM Tune" command to the Si4705 chip and waits until CTS becomes set
 * @param  pRadioDevice Pointer to the radio device structure
 * @param  args Arguments for the command
 * @param  frequency Frequency to which the radio should tune itself, in 10 kHz increments
 *
 * @retval True if the command succeeded; false otherwise
 */
bool TuneFreq(volatile RadioDevice_t *pRadioDevice, CMD_FM_TUNE_FREQ_ARGS args, uint16_t frequency)
{
    while (pRadioDevice->currentCommand.state != COMMANDSTATE_IDLE)
    {
    }

    pRadioDevice->currentCommand.args.opCode = CMD_ID_FM_TUNE_FREQ;
    pRadioDevice->currentCommand.args.bytes[1] = args;
    pRadioDevice->currentCommand.args.bytes[2] = (uint8_t)((frequency & 0xFF00) >> 8);
    pRadioDevice->currentCommand.args.bytes[3] = (uint8_t)((frequency & 0x00FF) >> 0);
    pRadioDevice->currentCommand.args.bytes[4] = 0; // Manual ANTCAP tuning is not allowed at this point
    pRadioDevice->currentCommand.argLength = 5;
    pRadioDevice->currentCommand.responseLength = 0;

    pRadioDevice->currentCommand.state = COMMANDSTATE_SENDING;

    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit_IT(&hi2c1, pRadioDevice->deviceAddress,
                                                          (uint8_t *)&pRadioDevice->currentCommand.args.bytes[0],
                                                          pRadioDevice->currentCommand.argLength);

    if (status != HAL_OK)
    {
        return false;
    }

    while (pRadioDevice->currentCommand.state != COMMANDSTATE_READY)
    {
    }

    pRadioDevice->currentState = RADIOSTATE_TUNED_TO_STATION;

    pRadioDevice->currentCommand.args.bytes[0] = 0;
    pRadioDevice->currentCommand.args.bytes[1] = 0;
    pRadioDevice->currentCommand.args.bytes[2] = 0;
    pRadioDevice->currentCommand.args.bytes[3] = 0;
    pRadioDevice->currentCommand.args.bytes[4] = 0;
    pRadioDevice->currentCommand.args.bytes[5] = 0;
    pRadioDevice->currentCommand.args.bytes[6] = 0;
    pRadioDevice->currentCommand.args.bytes[7] = 0;
    pRadioDevice->currentCommand.state = COMMANDSTATE_IDLE;
    pRadioDevice->currentCommand.argLength = 0;
    pRadioDevice->currentCommand.responseLength = 0;

    return true;
}

/**
 * @brief  Sends the "Seek Start" command to the Si4705 chip and waits until CTS becomes set
 * @param  pRadioDevice Pointer to the radio device structure
 * @param  args Arguments for the command
 *
 * @retval True if the command succeeded; false otherwise
 */
bool SeekStart(volatile RadioDevice_t *pRadioDevice, CMD_FM_SEEK_START_ARGS args)
{
    volatile HAL_StatusTypeDef status;
    uint8_t tx_buffer[2] = {0};

    WaitForStatus(pRadioDevice, STATUS_CLEAR_TO_SEND);

    tx_buffer[0] = CMD_ID_FM_SEEK_START;
    tx_buffer[1] = args;

    status = HAL_I2C_Master_Transmit(&hi2c1, pRadioDevice->deviceAddress, tx_buffer, sizeof(tx_buffer), HAL_MAX_DELAY);

    if (status != HAL_OK)
    {
        return false;
    }

    WaitForStatus(pRadioDevice, STATUS_CLEAR_TO_SEND);

    return true;
}

/**
 * @brief  Sends the "Get Tune Status" command to the Si4705 chip and waits until CTS becomes set
 * @param  pRadioDevice Pointer to the radio device structure
 * @param  args Arguments to the command
 * @param  pResponse Pointer to a variable that will hold the response to the command
 *
 * @retval True if the command succeeded; false otherwise
 */
bool GetTuneStatus(volatile RadioDevice_t *pRadioDevice, CMD_GET_TUNE_STATUS_ARGS args,
                   GetTuneStatusResponse_t *pResponse)
{
    volatile HAL_StatusTypeDef status;
    uint8_t tx_buffer[2] = {0};
    uint8_t rx_buffer[8] = {0};

    if (pResponse == NULL)
    {
        return false;
    }

    WaitForStatus(pRadioDevice, STATUS_CLEAR_TO_SEND);

    tx_buffer[0] = CMD_ID_FM_TUNE_STATUS;
    tx_buffer[1] = args;

    status = HAL_I2C_Master_Transmit(&hi2c1, pRadioDevice->deviceAddress, tx_buffer, sizeof(tx_buffer), HAL_MAX_DELAY);

    if (status != HAL_OK)
    {
        return false;
    }

    WaitForStatus(pRadioDevice, STATUS_CLEAR_TO_SEND);

    status = HAL_I2C_Master_Receive(&hi2c1, pRadioDevice->deviceAddress, rx_buffer, sizeof(rx_buffer), HAL_MAX_DELAY);

    pResponse->bandLimit = rx_buffer[1] & 0x80;
    pResponse->AFCRail = rx_buffer[1] & 0x02;
    pResponse->validChannel = rx_buffer[1] & 0x01;

    pResponse->frequency = (uint16_t)(rx_buffer[2] << 8 | rx_buffer[3]);
    pResponse->rssi = rx_buffer[4];
    pResponse->snr = rx_buffer[5];
    pResponse->multipath = rx_buffer[6];
    pResponse->antennaTuningCapacitor = rx_buffer[7];

    return true;
}

/**
 * @brief  Sends the "FM RSQ Status" command to the Si4705 chip and waits until CTS becomes set
 * @param  pRadioDevice Pointer to the radio device structure
 * @param  args Arguments to the command
 * @param  pResponse Pointer to a variable that will hold the response to the command
 *
 * @retval True if the command succeeded; false otherwise
 */
bool RSQStatus(volatile RadioDevice_t *pRadioDevice, CMD_FM_RSQ_STATUS_ARGS args, RSQStatusResponse_t *pResponse)
{
    volatile HAL_StatusTypeDef status;
    uint8_t tx_buffer[2] = {0};
    uint8_t rx_buffer[8] = {0};

    if (pResponse == NULL)
    {
        return false;
    }

    WaitForStatus(pRadioDevice, STATUS_CLEAR_TO_SEND);

    tx_buffer[0] = CMD_ID_FM_RSQ_STATUS;
    tx_buffer[1] = args;

    status = HAL_I2C_Master_Transmit(&hi2c1, pRadioDevice->deviceAddress, tx_buffer, sizeof(tx_buffer), HAL_MAX_DELAY);

    if (status != HAL_OK)
    {
        return false;
    }

    WaitForStatus(pRadioDevice, STATUS_CLEAR_TO_SEND);

    status = HAL_I2C_Master_Receive(&hi2c1, pRadioDevice->deviceAddress, rx_buffer, sizeof(rx_buffer), HAL_MAX_DELAY);

    pResponse->blendInt = rx_buffer[1] & 0x80;
    pResponse->multHInt = rx_buffer[1] & 0x20;
    pResponse->multLInt = rx_buffer[1] & 0x10;
    pResponse->snrHInt = rx_buffer[1] & 0x08;
    pResponse->snrLInt = rx_buffer[1] & 0x04;
    pResponse->rssiHInt = rx_buffer[1] & 0x02;
    pResponse->rssiLInt = rx_buffer[1] & 0x01;

    pResponse->softMute = rx_buffer[2] & 0x08;
    pResponse->AFCRail = rx_buffer[2] & 0x02;
    pResponse->validChannel = rx_buffer[2] & 0x01;

    pResponse->pilot = rx_buffer[3] & 0x80;
    pResponse->stereoBlend = (rx_buffer[3] & 0x7F);

    pResponse->rssi = rx_buffer[4];
    pResponse->snr = rx_buffer[5];
    pResponse->multipath = rx_buffer[6];
    pResponse->frequencyOffset = (int8_t)rx_buffer[7];

    return true;
}

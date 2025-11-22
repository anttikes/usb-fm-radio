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
#include "device.h"
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
 * @brief  Enqueues the "Power up" command
 * @param  device Pointer to the radio device structure
 * @param  arg1 First set of arguments for the command
 * @param  arg2 Second set of arguments for the command
 * @param  pResponse In case of "QueryLibraryId" variant, points to the structure where return data should be placed
 *
 * @retval True if the command was enqueued; false otherwise
 */
bool PowerUp(RadioDevice_t *device, CMD_POWER_UP_ARGS_1 arg1, CMD_POWER_UP_ARGS_2 arg2)
{
    Command_t powerUp = {0};

    powerUp.args.opCode = CMD_ID_POWER_UP;
    powerUp.args.bytes[1] = arg1;
    powerUp.args.bytes[2] = arg2;
    powerUp.argLength = 3;
    powerUp.responseLength = 0;

    return EnqueueCommand(device, &powerUp);
}

/**
 * @brief  Enqueues the "SET PROPERTY" command with "GPO_IEN" property identifier to configure the interrupt sources for
 * status bits.
 * @param  device Pointer to the radio device structure
 * @param  sources Additional interrupt sources to enable
 *
 * @retval True if the command was enqueued; false otherwise
 */
bool SetInterruptSources(RadioDevice_t *device, InterruptSources_t sources)
{
    return SetProperty(device, PROP_ID_GPO_IEN, sources);
}

/**
 * @brief  Enqueues the "Power down" command
 * @param  device Pointer to the radio device structure
 *
 * @retval True if the command was enqueued; false otherwise
 */
bool PowerDown(RadioDevice_t *device)
{
    Command_t powerDown = {0};

    powerDown.args.opCode = CMD_ID_POWER_DOWN;
    powerDown.argLength = 1;
    powerDown.responseLength = 0;

    return EnqueueCommand(device, &powerDown);
}

/**
 * @brief  Enqueues the "Get Rev" command
 * @param  device Pointer to the radio device structure
 *
 * @retval True if the command was enqueued; false otherwise
 */
bool GetRevision(RadioDevice_t *device)
{
    Command_t getRevision = {0};

    getRevision.args.opCode = CMD_ID_GET_REV;
    getRevision.argLength = 1;
    getRevision.responseLength = 16;

    return EnqueueCommand(device, &getRevision);

    // status = HAL_I2C_Master_Receive(&hi2c1, pRadioDevice->deviceAddress, rx_buffer, sizeof(rx_buffer),
    // HAL_MAX_DELAY);

    // pResponse->productNumber = rx_buffer[1];
    // pResponse->firmwareMajor = rx_buffer[2];
    // pResponse->firmwareMinor = rx_buffer[3];
    // pResponse->patchId = (uint16_t)(rx_buffer[4] << 8 | rx_buffer[5]);
    // pResponse->componentMajor = rx_buffer[6];
    // pResponse->componentMinor = rx_buffer[7];
    // pResponse->chipRevision = rx_buffer[8];
    // pResponse->cid = rx_buffer[15];

    // return true;
}

/**
 * @brief  Enqueues the "Set Property" command
 * @param  device Pointer to the radio device structure
 * @param  property Identifier of the property to set
 * @param  value New value of the property
 *
 * @retval True if the command was enqueued; false otherwise
 */
bool SetProperty(RadioDevice_t *device, PropertyIdentifiers_t property, uint16_t value)
{
    Command_t setProperty = {0};

    setProperty.args.opCode = CMD_ID_SET_PROPERTY;
    setProperty.args.bytes[1] = 0x00;
    setProperty.args.bytes[2] = (uint8_t)((property & 0xFF00) >> 8);
    setProperty.args.bytes[3] = (uint8_t)((property & 0x00FF) >> 0);
    setProperty.args.bytes[4] = (uint8_t)((value & 0xFF00) >> 8);
    setProperty.args.bytes[5] = (uint8_t)((value & 0x00FF) >> 0);
    setProperty.argLength = 6;
    setProperty.responseLength = 0;

    return EnqueueCommand(device, &setProperty);
}

/**
 * @brief  Enqueues the "Get Property" command
 * @param  device Pointer to the radio device structure
 * @param  property Identifier of the property to get
 *
 * @retval True if the command was enqueued; false otherwise
 */
bool GetProperty(RadioDevice_t *device, PropertyIdentifiers_t property)
{
    Command_t getProperty = {0};

    getProperty.args.opCode = CMD_ID_GET_PROPERTY;
    getProperty.args.bytes[1] = 0x00;
    getProperty.args.bytes[2] = (uint8_t)((property & 0xFF00) >> 8);
    getProperty.args.bytes[3] = (uint8_t)((property & 0x00FF) >> 0);
    getProperty.argLength = 4;
    getProperty.responseLength = 4;

    return EnqueueCommand(device, &getProperty);

    // status = HAL_I2C_Master_Receive(&hi2c1, pRadioDevice->deviceAddress, rx_buffer, sizeof(rx_buffer),
    // HAL_MAX_DELAY);

    // // Third and fourth bytes are the high and low bytes of the return value
    // (*pValue) = (uint16_t)((rx_buffer[2] << 8) | rx_buffer[3]);

    // return true;
}

/**
 * @brief  Enqueues the "Get Int Status" command
 * @param  device Pointer to the radio device structure
 *
 * @retval True if the command was enqueued; false otherwise
 */
bool GetIntStatus(RadioDevice_t *device)
{
    Command_t getIntStatus = {0};

    getIntStatus.args.opCode = CMD_ID_GET_INT_STATUS;
    getIntStatus.argLength = 1;
    getIntStatus.responseLength = 1;

    return EnqueueCommand(device, &getIntStatus);

    // status = HAL_I2C_Master_Receive(&hi2c1, pRadioDevice->deviceAddress, rx_buffer, sizeof(rx_buffer),
    // HAL_MAX_DELAY);

    // (*pValue) = rx_buffer[0];

    // return true;
}

/**
 * @brief  Enqueues the "FM Tune" command
 * @param  device Pointer to the radio device structure
 * @param  args Arguments for the command
 * @param  frequency Frequency to which the radio should tune itself, in 10 kHz increments
 *
 * @retval True if the command was enqueued; false otherwise
 */
bool TuneFreq(RadioDevice_t *device, CMD_FM_TUNE_FREQ_ARGS args, uint16_t frequency)
{
    Command_t tuneFreq = {0};

    tuneFreq.args.opCode = CMD_ID_FM_TUNE_FREQ;
    tuneFreq.args.bytes[1] = args;
    tuneFreq.args.bytes[2] = (uint8_t)((frequency & 0xFF00) >> 8);
    tuneFreq.args.bytes[3] = (uint8_t)((frequency & 0x00FF) >> 0);
    tuneFreq.args.bytes[4] = 0; // Manual ANTCAP tuning is not allowed at this point
    tuneFreq.argLength = 5;
    tuneFreq.responseLength = 0;

    return EnqueueCommand(device, &tuneFreq);
}

/**
 * @brief  Enqueues the "Seek Start" command
 * @param  device Pointer to the radio device structure
 * @param  args Arguments for the command
 *
 * @retval True if the command was enqueued; false otherwise
 */
bool SeekStart(RadioDevice_t *device, CMD_FM_SEEK_START_ARGS args)
{
    Command_t seekStart = {0};

    seekStart.args.opCode = CMD_ID_FM_SEEK_START;
    seekStart.args.bytes[1] = args;
    seekStart.argLength = 2;
    seekStart.responseLength = 0;

    return EnqueueCommand(device, &seekStart);
}

/**
 * @brief  Enqueues the "Get Tune Status" command
 * @param  device Pointer to the radio device structure
 * @param  args Arguments to the command
 *
 * @retval True if the command was enqueued; false otherwise
 */
bool GetTuneStatus(RadioDevice_t *device, CMD_GET_TUNE_STATUS_ARGS args)
{
    Command_t getTuneStatus = {0};

    getTuneStatus.args.opCode = CMD_ID_FM_TUNE_STATUS;
    getTuneStatus.args.bytes[1] = args;
    getTuneStatus.argLength = 2;
    getTuneStatus.responseLength = 8;

    return EnqueueCommand(device, &getTuneStatus);

    // status = HAL_I2C_Master_Receive(&hi2c1, pRadioDevice->deviceAddress, rx_buffer, sizeof(rx_buffer),
    // HAL_MAX_DELAY);

    // pResponse->bandLimit = rx_buffer[1] & 0x80;
    // pResponse->AFCRail = rx_buffer[1] & 0x02;
    // pResponse->validChannel = rx_buffer[1] & 0x01;

    // pResponse->frequency = (uint16_t)(rx_buffer[2] << 8 | rx_buffer[3]);
    // pResponse->rssi = rx_buffer[4];
    // pResponse->snr = rx_buffer[5];
    // pResponse->multipath = rx_buffer[6];
    // pResponse->antennaTuningCapacitor = rx_buffer[7];

    // return true;
}

/**
 * @brief  Enqueues the "FM RSQ Status" command
 * @param  device Pointer to the radio device structure
 * @param  args Arguments to the command
 *
 * @retval True if the command was enqueued; false otherwise
 */
bool RSQStatus(RadioDevice_t *device, CMD_FM_RSQ_STATUS_ARGS args)
{
    Command_t rsqStatus = {0};

    rsqStatus.args.opCode = CMD_ID_FM_RSQ_STATUS;
    rsqStatus.args.bytes[1] = args;
    rsqStatus.argLength = 2;
    rsqStatus.responseLength = 8;

    return EnqueueCommand(device, &rsqStatus);

    // status = HAL_I2C_Master_Receive(&hi2c1, pRadioDevice->deviceAddress, rx_buffer, sizeof(rx_buffer),
    // HAL_MAX_DELAY);

    // pResponse->blendInt = rx_buffer[1] & 0x80;
    // pResponse->multHInt = rx_buffer[1] & 0x20;
    // pResponse->multLInt = rx_buffer[1] & 0x10;
    // pResponse->snrHInt = rx_buffer[1] & 0x08;
    // pResponse->snrLInt = rx_buffer[1] & 0x04;
    // pResponse->rssiHInt = rx_buffer[1] & 0x02;
    // pResponse->rssiLInt = rx_buffer[1] & 0x01;

    // pResponse->softMute = rx_buffer[2] & 0x08;
    // pResponse->AFCRail = rx_buffer[2] & 0x02;
    // pResponse->validChannel = rx_buffer[2] & 0x01;

    // pResponse->pilot = rx_buffer[3] & 0x80;
    // pResponse->stereoBlend = (rx_buffer[3] & 0x7F);

    // pResponse->rssi = rx_buffer[4];
    // pResponse->snr = rx_buffer[5];
    // pResponse->multipath = rx_buffer[6];
    // pResponse->frequencyOffset = (int8_t)rx_buffer[7];

    // return true;
}

/**
 * @brief  Enqueues the "GPIO_CTL" command
 * @param  device Pointer to the radio device structure
 * @param  args Arguments to the command
 *
 * @retval True if the command was enqueued; false otherwise
 */
bool GPIOCtl(RadioDevice_t *device, CMD_GPIO_CTL_ARGS args)
{
    Command_t gpioCtl = {0};

    gpioCtl.args.opCode = CMD_ID_GPIO_CTL;
    gpioCtl.args.bytes[1] = args;
    gpioCtl.argLength = 2;
    gpioCtl.responseLength = 0;

    return EnqueueCommand(device, &gpioCtl);
}

/**
 * @brief  Enqueues the "GPIO_SET" command
 * @param  device Pointer to the radio device structure
 * @param  args Arguments to the command
 *
 * @retval True if the command was enqueued; false otherwise
 */
bool GPIOSet(RadioDevice_t *device, CMD_GPIO_SET_ARGS args)
{
    Command_t gpioSet = {0};

    gpioSet.args.opCode = CMD_ID_GPIO_SET;
    gpioSet.args.bytes[1] = args;
    gpioSet.argLength = 2;
    gpioSet.responseLength = 0;

    return EnqueueCommand(device, &gpioSet);
}

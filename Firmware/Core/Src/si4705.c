/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    si4705.c
 * @brief   Implements wrapper calls for executing commands on the Si4705.
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "si4705.h"
#include "i2c.h"
//#include "usbd_def.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
volatile RadioDevice_t radioDevice = {
	.deviceAddress = SI4705_I2C_ADDRESS,
	.currentState = RADIOSTATE_POWERDOWN,
};

/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/

/**
 * @brief  Invoked when an EXTI event has occurred
 * @param  GPIO_Pin Specifies the port pin connected to corresponding EXTI line.
 *
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	UNUSED(GPIO_Pin);
}

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
			status = HAL_I2C_Master_Receive(&hi2c1, radioDevice.deviceAddress, (uint8_t*) &latestStatus, 1, HAL_MAX_DELAY);
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
 * @retval HAL status
 */
HAL_StatusTypeDef PowerUp(volatile RadioDevice_t *pRadioDevice, CMD_POWER_UP_ARGS_1 arg1, CMD_POWER_UP_ARGS_2 arg2)
{
	HAL_StatusTypeDef status;
	uint8_t tx_buffer[3] = {0};

	if (pRadioDevice->currentState != RADIOSTATE_POWERDOWN)
	{
		return HAL_ERROR;
	}

	tx_buffer[0] = CMD_POWER_UP;
	tx_buffer[1] = arg1;
	tx_buffer[2] = arg2;

	status = HAL_I2C_Master_Transmit(&hi2c1, pRadioDevice->deviceAddress, tx_buffer, sizeof(tx_buffer),	HAL_MAX_DELAY);

	if (status != HAL_OK)
	{
		return status;
	}

	WaitForStatus(pRadioDevice, STATUS_CLEAR_TO_SEND);

	pRadioDevice->currentState = RADIOSTATE_POWERUP;

	return status;
}

/**
 * @brief  Uses the "GPO_IEN" property to configure the interrupt sources for status bits.
 * @param  pRadioDevice Pointer to the radio device structure
 * @param  sources Additional interrupt sources to enable
 *
 * @retval HAL status
 */
HAL_StatusTypeDef SetInterruptSources(volatile RadioDevice_t *pRadioDevice, InterruptSources_t sources)
{
	HAL_StatusTypeDef status = SetProperty(pRadioDevice, PROP_GPO_IEN, sources);

	if (status != HAL_OK)
	{
		return status;
	}

	return status;
}

/**
 * @brief  Sends the "Power down" command to the Si4705 chip and waits until CTS becomes set
 * @param  pRadioDevice Pointer to the radio device structure
 *
 * @retval None
 */
HAL_StatusTypeDef PowerDown(volatile RadioDevice_t *pRadioDevice)
{
	HAL_StatusTypeDef status;
	uint8_t tx_buffer[1] = {0};

	if (pRadioDevice->currentState == RADIOSTATE_POWERDOWN)
	{
		return HAL_OK;
	}

	WaitForStatus(pRadioDevice, STATUS_CLEAR_TO_SEND);

	tx_buffer[0] = CMD_POWER_DOWN;

	status = HAL_I2C_Master_Transmit(&hi2c1, pRadioDevice->deviceAddress, tx_buffer, sizeof(tx_buffer),	HAL_MAX_DELAY);

	if (status != HAL_OK)
	{
		return status;
	}

	WaitForStatus(pRadioDevice, STATUS_CLEAR_TO_SEND);

	pRadioDevice->currentState = RADIOSTATE_POWERDOWN;

	return status;
}

/**
 * @brief  Sends the "Get Rev" command to the Si4705 chip and waits until CTS becomes set
 * @param  pRadioDevice Pointer to the radio device structure
 * @param  pResponse Pointer to the structure representing the data returned by the chip
 *
 * @retval HAL status
 */
HAL_StatusTypeDef GetRevision(volatile RadioDevice_t *pRadioDevice,	GetRevisionResponse_t *pResponse)
{
	HAL_StatusTypeDef status;
	uint8_t tx_buffer[1] = {0};
	uint8_t rx_buffer[16] = {0};

	if (pResponse == NULL)
	{
		return HAL_ERROR;
	}

	WaitForStatus(pRadioDevice, STATUS_CLEAR_TO_SEND);

	tx_buffer[0] = CMD_GET_REV;

	status = HAL_I2C_Master_Transmit(&hi2c1, pRadioDevice->deviceAddress, tx_buffer, sizeof(tx_buffer),	HAL_MAX_DELAY);

	if (status != HAL_OK)
	{
		return status;
	}

	WaitForStatus(pRadioDevice, STATUS_CLEAR_TO_SEND);

	status = HAL_I2C_Master_Receive(&hi2c1, pRadioDevice->deviceAddress, rx_buffer, sizeof(rx_buffer), HAL_MAX_DELAY);

	pResponse->productNumber = rx_buffer[1];
	pResponse->firmwareMajor = rx_buffer[2];
	pResponse->firmwareMinor = rx_buffer[3];
	pResponse->patchId = (uint16_t) (rx_buffer[4] << 8 | rx_buffer[5]);
	pResponse->componentMajor = rx_buffer[6];
	pResponse->componentMinor = rx_buffer[7];
	pResponse->chipRevision = rx_buffer[8];
	pResponse->cid = rx_buffer[15];

	return status;
}

/**
 * @brief  Sends the "Set Property" command to the Si4705 chip and waits until CTS becomes set
 * @param  pRadioDevice Pointer to the radio device structure
 * @param  property Identifier of the property to set
 * @param  value New value of the property
 *
 * @retval HAL status
 */
HAL_StatusTypeDef SetProperty(volatile RadioDevice_t *pRadioDevice, uint16_t property, uint16_t value)
{
	HAL_StatusTypeDef status;
	uint8_t tx_buffer[6] = {0};

	WaitForStatus(pRadioDevice, STATUS_CLEAR_TO_SEND);

	tx_buffer[0] = CMD_SET_PROPERTY;
	tx_buffer[1] = 0x00;
	tx_buffer[2] = (uint8_t) ((property & 0xFF00) >> 8);
	tx_buffer[3] = (uint8_t) ((property & 0x00FF) >> 0);
	tx_buffer[4] = (uint8_t) ((value & 0xFF00) >> 8);
	tx_buffer[5] = (uint8_t) ((value & 0x00FF) >> 0);

	status = HAL_I2C_Master_Transmit(&hi2c1, pRadioDevice->deviceAddress, tx_buffer, sizeof(tx_buffer), HAL_MAX_DELAY);

	if (status != HAL_OK)
	{
		return status;
	}

	// The programming guide says Set Property has a guaranteed timing of 10 ms, after which it is always completed
	HAL_Delay(10);

	return status;
}

/**
 * @brief  Sends the "Get Property" command to the Si4705 chip and waits until CTS becomes set
 * @param  pRadioDevice Pointer to the radio device structure
 * @param  property Identifier of the property to set
 * @param  pValue Pointer to the variable that will receive the value of that the property has
 *
 * @retval HAL status
 */
HAL_StatusTypeDef GetProperty(volatile RadioDevice_t *pRadioDevice, uint16_t property, uint16_t *pValue)
{
	HAL_StatusTypeDef status;
	uint8_t tx_buffer[4] = {0};
	uint8_t rx_buffer[4] = {0};

	if (pValue == NULL)
	{
		return HAL_ERROR;
	}

	WaitForStatus(pRadioDevice, STATUS_CLEAR_TO_SEND);

	tx_buffer[0] = CMD_GET_PROPERTY;
	tx_buffer[1] = 0x00;
	tx_buffer[2] = (uint8_t) ((property & 0xFF00) >> 8);
	tx_buffer[3] = (uint8_t) ((property & 0x00FF) >> 0);

	status = HAL_I2C_Master_Transmit(&hi2c1, pRadioDevice->deviceAddress, tx_buffer, sizeof(tx_buffer),	HAL_MAX_DELAY);

	if (status != HAL_OK)
	{
		return status;
	}

	WaitForStatus(pRadioDevice, STATUS_CLEAR_TO_SEND);

	status = HAL_I2C_Master_Receive(&hi2c1, pRadioDevice->deviceAddress, rx_buffer, sizeof(rx_buffer), HAL_MAX_DELAY);

	// Third and fourth bytes are the high and low bytes of the return value
	(*pValue) = (uint16_t) ((rx_buffer[2] << 8) | rx_buffer[3]);

	return status;
}

/**
 * @brief  Sends the "Get Int Status" command to the Si4705 chip and waits until CTS becomes set
 * @param  pRadioDevice Pointer to the radio device structure
 * @param  property Identifier of the property to set
 *
 * @retval HAL status
 */
HAL_StatusTypeDef GetIntStatus(volatile RadioDevice_t *pRadioDevice, StatusFlags_t *pValue)
{
	HAL_StatusTypeDef status;
	uint8_t tx_buffer[1] = {0};
	uint8_t rx_buffer[1] = {0};

	if (pValue == NULL)
	{
		return HAL_ERROR;
	}

	WaitForStatus(pRadioDevice, STATUS_CLEAR_TO_SEND);

	tx_buffer[0] = CMD_GET_INT_STATUS;

	status = HAL_I2C_Master_Transmit(&hi2c1, pRadioDevice->deviceAddress, tx_buffer, sizeof(tx_buffer),	HAL_MAX_DELAY);

	if (status != HAL_OK)
	{
		return status;
	}

	WaitForStatus(pRadioDevice, STATUS_CLEAR_TO_SEND);

	status = HAL_I2C_Master_Receive(&hi2c1, pRadioDevice->deviceAddress, rx_buffer, sizeof(rx_buffer), HAL_MAX_DELAY);

	(*pValue) = rx_buffer[0];

	return status;
}

/**
 * @brief  Sends the "FM Tune" command to the Si4705 chip and waits until CTS becomes set
 * @param  pRadioDevice Pointer to the radio device structure
 * @param  args Arguments for the command
 * @param  frequency Frequency to which the radio should tune itself, in 10 kHz increments
 *
 * @retval HAL status
 */
HAL_StatusTypeDef TuneFreq(volatile RadioDevice_t *pRadioDevice,
		CMD_FM_TUNE_FREQ_ARGS args, uint16_t frequency)
{
	HAL_StatusTypeDef status;
	uint8_t tx_buffer[5] = {0};

	WaitForStatus(pRadioDevice, STATUS_CLEAR_TO_SEND);

	tx_buffer[0] = CMD_FM_TUNE_FREQ;
	tx_buffer[1] = args;
	tx_buffer[2] = (uint8_t) ((frequency & 0xFF00) >> 8);
	tx_buffer[3] = (uint8_t) ((frequency & 0x00FF) >> 0);
	tx_buffer[4] = 0;  // Manual ANTCAP tuning is not allowed at this point

	status = HAL_I2C_Master_Transmit(&hi2c1, pRadioDevice->deviceAddress, tx_buffer, sizeof(tx_buffer), HAL_MAX_DELAY);

	if (status != HAL_OK)
	{
		return status;
	}

	WaitForStatus(pRadioDevice, STATUS_CLEAR_TO_SEND);

	return status;
}

/**
 * @brief  Sends the "Seek Start" command to the Si4705 chip and waits until CTS becomes set
 * @param  pRadioDevice Pointer to the radio device structure
 * @param  args Arguments for the command
 *
 * @retval HAL status
 */
HAL_StatusTypeDef SeekStart(volatile RadioDevice_t *pRadioDevice,
		CMD_FM_SEEK_START_ARGS args)
{
	HAL_StatusTypeDef status;
	uint8_t tx_buffer[2] = {0};

	WaitForStatus(pRadioDevice, STATUS_CLEAR_TO_SEND);

	tx_buffer[0] = CMD_FM_SEEK_START;
	tx_buffer[1] = args;

	status = HAL_I2C_Master_Transmit(&hi2c1, pRadioDevice->deviceAddress, tx_buffer, sizeof(tx_buffer), HAL_MAX_DELAY);

	if (status != HAL_OK)
	{
		return status;
	}

	WaitForStatus(pRadioDevice, STATUS_CLEAR_TO_SEND);

	return status;
}

/**
 * @brief  Sends the "Get Tune Status" command to the Si4705 chip and waits until CTS becomes set
 * @param  pRadioDevice Pointer to the radio device structure
 * @param  args Arguments to the command
 * @param  pResponse Pointer to a variable that will hold the response to the command
 *
 * @retval HAL status
 */
HAL_StatusTypeDef GetTuneStatus(volatile RadioDevice_t *pRadioDevice, CMD_GET_TUNE_STATUS_ARGS args, GetTuneStatusResponse_t *pResponse)
{
	HAL_StatusTypeDef status;
	uint8_t tx_buffer[2] = {0};
	uint8_t rx_buffer[8] = {0};

	if (pResponse == NULL)
	{
		return HAL_ERROR;
	}

	WaitForStatus(pRadioDevice, STATUS_CLEAR_TO_SEND);

	tx_buffer[0] = CMD_FM_TUNE_STATUS;
	tx_buffer[1] = args;

	status = HAL_I2C_Master_Transmit(&hi2c1, pRadioDevice->deviceAddress, tx_buffer, sizeof(tx_buffer), HAL_MAX_DELAY);

	if (status != HAL_OK)
	{
		return status;
	}

	WaitForStatus(pRadioDevice, STATUS_CLEAR_TO_SEND);

	status = HAL_I2C_Master_Receive(&hi2c1, pRadioDevice->deviceAddress, rx_buffer, sizeof(rx_buffer), HAL_MAX_DELAY);

	pResponse->bandLimit = rx_buffer[1] & 0x80;
	pResponse->AFCRail = rx_buffer[1] & 0x02;
	pResponse->validChannel = rx_buffer[1] & 0x01;

	pResponse->frequency = (uint16_t) (rx_buffer[2] << 8 | rx_buffer[3]);
	pResponse->rssi = rx_buffer[4];
	pResponse->snr = rx_buffer[5];
	pResponse->multipath = rx_buffer[6];
	pResponse->antennaTuningCapacitor = rx_buffer[7];

	return status;
}

/**
 * @brief  Sends the "FM RSQ Status" command to the Si4705 chip and waits until CTS becomes set
 * @param  pRadioDevice Pointer to the radio device structure
 * @param  args Arguments to the command
 * @param  pResponse Pointer to a variable that will hold the response to the command
 *
 * @retval HAL status
 */
HAL_StatusTypeDef RSQStatus(volatile RadioDevice_t *pRadioDevice, CMD_FM_RSQ_STATUS_ARGS args, RSQStatusResponse_t *pResponse)
{
	HAL_StatusTypeDef status;
	uint8_t tx_buffer[2] = {0};
	uint8_t rx_buffer[8] = {0};

	if (pResponse == NULL)
	{
		return HAL_ERROR;
	}

	WaitForStatus(pRadioDevice, STATUS_CLEAR_TO_SEND);

	tx_buffer[0] = CMD_FM_RSQ_STATUS;
	tx_buffer[1] = args;

	status = HAL_I2C_Master_Transmit(&hi2c1, pRadioDevice->deviceAddress, tx_buffer, sizeof(tx_buffer), HAL_MAX_DELAY);

	if (status != HAL_OK)
	{
		return status;
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
	pResponse->frequencyOffset = (int8_t) rx_buffer[7];

	return status;
}

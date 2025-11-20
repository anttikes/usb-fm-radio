/**
 ******************************************************************************
 * @file    device.h
 * @brief   Implements functions to manage radio device state and
 *          command processing.
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
#include "device.h"
#include "i2c.h"
#include "main.h"
#include "stm32f0xx_hal.h"

/* Exported variables --------------------------------------------------------*/
// clang-format off
volatile RadioDevice_t radioDevice = {
    .deviceAddress = SI4705_I2C_ADDRESS,
    .currentState = RADIOSTATE_POWERDOWN,
    .currentCommand = {
        .opcode = CMD_ID_NONE,
        .state = COMMANDSTATE_IDLE,
        .args = {0},
        .argLength = 0,
        .response = {0},
        .responseLength = 0
    }    
};
// clang-format on

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/* External callbacks --------------------------------------------------------*/

/**
 * @brief  EXTI line detection callback.
 * @param  GPIO_Pin Specifies the port pin connected to corresponding EXTI line.
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == RADIO_NIRQ_Pin)
    {
        if (radioDevice.currentCommand.state == COMMANDSTATE_WAITING_FOR_INT)
        {
            radioDevice.currentCommand.state = COMMANDSTATE_READY;
        }
    }
}

/**
 * @brief  Master Tx Transfer completed callback.
 * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
 *                the configuration information for the specified I2C.
 * @retval None
 */
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == hi2c1.Instance)
    {
        if (radioDevice.currentCommand.responseLength > 0)
        {
            radioDevice.currentCommand.state = COMMANDSTATE_RECEIVING_RESPONSE;

            if (HAL_I2C_Master_Receive_IT(&hi2c1, radioDevice.deviceAddress,
                                          (uint8_t *)&radioDevice.currentCommand.response[0],
                                          radioDevice.currentCommand.responseLength) != HAL_OK)
            {
                // Handle error
            }
        }
        else
        {
            radioDevice.currentCommand.state = COMMANDSTATE_WAITING_FOR_INT;
        }
    }
}

/**
 * @brief  Master Rx Transfer completed callback.
 * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
 *                the configuration information for the specified I2C.
 * @retval None
 */
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == hi2c1.Instance)
    {
        // Handle I2C reception complete
    }
}

/* Private functions ---------------------------------------------------------*/

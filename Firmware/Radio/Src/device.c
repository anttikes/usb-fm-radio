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
#include "common.h"
#include "i2c.h"
#include "main.h"
#include "stm32f0xx_hal.h"

/* Exported variables --------------------------------------------------------*/
// clang-format off
RadioDevice_t radioDevice = {
    .deviceAddress = SI4705_I2C_ADDRESS,
    .currentState = RADIOSTATE_POWERDOWN,
    .commandQueue = {
        .commands = {{0}},
        .count = 0,
        .front = 0,
        .back = 0
    }    
};
// clang-format on

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
bool IsQueueEmpty(CommandQueue_t *queue);
Command_t *PeekQueue(CommandQueue_t *queue);
Command_t *PopQueue(CommandQueue_t *queue);

/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Processes the next command from the queue, if there are any
 * @param  device Pointer to the radio device structure
 *
 * @retval True if the queue is empty; false otherwise
 */
bool ProcessCommand(RadioDevice_t *device)
{
    if (IsQueueEmpty(&device->commandQueue))
    {
        return true;
    }

    Command_t *currentCommand = PeekQueue(&device->commandQueue);

    if (currentCommand == NULL)
    {
        return false;
    }

    if (currentCommand->state == COMMANDSTATE_READY)
    {
        if (currentCommand->args.opCode == CMD_ID_POWER_UP)
        {
            device->currentState = RADIOSTATE_POWERUP;
        }
        else if (currentCommand->args.opCode == CMD_ID_POWER_DOWN)
        {
            device->currentState = RADIOSTATE_POWERDOWN;
        }
        else if (currentCommand->args.opCode == CMD_ID_FM_TUNE_FREQ)
        {
            device->currentState = RADIOSTATE_TUNED_TO_STATION;
        }
        else if (currentCommand->args.opCode == CMD_ID_SET_PROPERTY)
        {
            PropertyIdentifiers_t property =
                (PropertyIdentifiers_t)((currentCommand->args.bytes[2] << 8) | (currentCommand->args.bytes[3] << 0));

            uint16_t value = (uint16_t)((currentCommand->args.bytes[4] << 8) | (currentCommand->args.bytes[5] << 0));

            if (property == PROP_ID_DIGITAL_OUTPUT_SAMPLE_RATE)
            {
                if (value != 0)
                {
                    device->currentState = RADIOSTATE_DIGITAL_OUTPUT_ENABLED;
                }
                else
                {
                    device->currentState = RADIOSTATE_TUNED_TO_STATION;
                }
            }

            // Programming guide guarantees that it takes 10ms to complete the operation
            HAL_Delay(10);
        }

        PopQueue(&device->commandQueue);

        currentCommand = PeekQueue(&device->commandQueue);

        if (currentCommand == NULL)
        {
            return true;
        }
    }
    else if (currentCommand->state != COMMANDSTATE_IDLE)
    {
        return true;
    }

    // All radio commands are I2C at the moment
    currentCommand->state = COMMANDSTATE_SENDING;

    return HAL_I2C_Master_Transmit_IT(&hi2c1, device->deviceAddress, (uint8_t *)&currentCommand->args,
                                      currentCommand->argLength) == HAL_OK;
}

/**
 * @brief  Enqueues the give command into the command queue of the radio device
 * @param  device Pointer to the radio device structure
 * @param  command Pointer to the command
 *
 * @retval True if the command was enqueued; false otherwise
 */
bool EnqueueCommand(RadioDevice_t *device, Command_t *command)
{
    if (device == NULL || command == NULL)
    {
        return false;
    }

    volatile CommandQueue_t *queue = &device->commandQueue;

    const uint8_t capacity = (uint8_t)(sizeof(queue->commands) / sizeof(queue->commands[0]));
    if (queue->count >= capacity)
    {
        /* Queue full */
        return false;
    }

    queue->commands[queue->back] = *command;
    queue->back = (uint8_t)((queue->back + 1) % capacity);
    queue->count++;

    return true;
}

/* External callbacks --------------------------------------------------------*/

/**
 * @brief  Invoked by HAL when a EXTI interrupt has occurred.
 * @param  GPIO_Pin Specifies the port pin connected to corresponding EXTI line.
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == RADIO_NIRQ_Pin)
    {
        Command_t *currentCommand = PeekQueue(&radioDevice.commandQueue);

        if (currentCommand->state == COMMANDSTATE_WAITING_FOR_STC)
        {
            currentCommand->state = COMMANDSTATE_WAITING_FOR_CTS;
        }
        else if (currentCommand->state == COMMANDSTATE_WAITING_FOR_CTS)
        {
            if (currentCommand->responseLength > 0)
            {
                currentCommand->state = COMMANDSTATE_RECEIVING_RESPONSE;

                HAL_I2C_Master_Receive_IT(&hi2c1, radioDevice.deviceAddress, currentCommand->response,
                                          currentCommand->responseLength);
            }
            else
            {
                currentCommand->state = COMMANDSTATE_READY;
            }
        }
    }
}

/**
 * @brief  Invoked by HAL when I2C Master transmit has completed.
 * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
 *                the configuration information for the specified I2C.
 * @retval None
 */
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == hi2c1.Instance)
    {
        Command_t *currentCommand = PeekQueue(&radioDevice.commandQueue);

        if (currentCommand->state == COMMANDSTATE_SENDING)
        {
            if (currentCommand->args.opCode == CMD_ID_FM_TUNE_FREQ ||
                currentCommand->args.opCode == CMD_ID_FM_SEEK_START)
            {
                currentCommand->state = COMMANDSTATE_WAITING_FOR_STC;
            }
            else
            {
                currentCommand->state = COMMANDSTATE_WAITING_FOR_CTS;
            }
        }
    }
}

/**
 * @brief  Invoked by HAL when I2C Master receive has completed.
 * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
 *                the configuration information for the specified I2C.
 * @retval None
 */
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == hi2c1.Instance)
    {
        Command_t *currentCommand = PeekQueue(&radioDevice.commandQueue);

        if (currentCommand->state == COMMANDSTATE_RECEIVING_RESPONSE)
        {
            // TODO: Process the response in some way

            currentCommand->state = COMMANDSTATE_READY;
        }
    }
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Determines if the queue is empty
 * @param  queue Pointer to the queue
 *
 * @retval True if the queue is empty; false otherwise
 */
bool IsQueueEmpty(CommandQueue_t *queue)
{
    if (queue == NULL)
    {
        return true;
    }

    return (queue->count == 0);
}

/**
 * @brief  Peeks the first command from the queue without removing it
 * @param  queue Pointer to the queue
 *
 * @retval Pointer to the first command in the queue or NULL if the queue is empty
 */
Command_t *PeekQueue(CommandQueue_t *queue)
{
    if (queue == NULL || queue->count == 0)
    {
        return NULL;
    }

    return &queue->commands[queue->front];
}

/**
 * @brief  Pops the first command from the queue, removing it
 * @param  queue Pointer to the queue
 *
 * @retval Pointer to the first command in the queue or NULL if the queue is empty
 */
Command_t *PopQueue(CommandQueue_t *queue)
{
    if (queue == NULL || queue->count == 0)
    {
        return NULL;
    }

    Command_t *cmd = &queue->commands[queue->front];
    const uint8_t capacity = (uint8_t)(sizeof(queue->commands) / sizeof(queue->commands[0]));
    queue->front = (uint8_t)((queue->front + 1) % capacity);
    queue->count--;

    return cmd;
}

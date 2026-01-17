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
#include "commands.h"
#include "common.h"
#include "i2c.h"
#include "main.h"
#include "stm32f0xx_hal.h"
#include "tim.h"
#include "tusb.h"

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

            HAL_TIM_Base_Stop(&htim14);
        }
        else if (currentCommand->args.opCode == CMD_ID_FM_TUNE_FREQ ||
                 currentCommand->args.opCode == CMD_ID_FM_SEEK_START)
        {
            device->currentState = RADIOSTATE_TUNED_TO_STATION;

            HAL_TIM_Base_Start_IT(&htim14);

            TuneStatus(device, GET_TUNE_STATUS_ARGS_NONE);
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

            // Tuner programming guide outlines that a property
            // set operation always completes in 10 ms
            HAL_Delay(10);
        }

        PopQueue(&device->commandQueue);

        currentCommand = PeekQueue(&device->commandQueue);

        if (currentCommand == NULL)
        {
            return true;
        }
    }
    else if (currentCommand->state == COMMANDSTATE_RESPONSE_RECEIVED)
    {
        uint8_t report[CFG_TUD_HID_EP_BUFSIZE - 1] = {0};

        report[0] = currentCommand->responseLength;

        for (uint8_t i = 0; i < currentCommand->responseLength; i++)
        {
            report[i + 1] = currentCommand->response[i];
        }

        currentCommand->state = COMMANDSTATE_READY;

        return tud_hid_report(currentCommand->args.opCode, report, CFG_TUD_HID_EP_BUFSIZE - 1);
    }
    else if (currentCommand->state == COMMANDSTATE_WAITING_FOR_RESPONSE_RETRIEVAL)
    {
        currentCommand->state = COMMANDSTATE_RECEIVING_RESPONSE;

        return HAL_I2C_Master_Receive_IT(&hi2c1, device->deviceAddress, (uint8_t *)&currentCommand->response,
                                         currentCommand->responseLength) == HAL_OK;
    }
    else if (currentCommand->state == COMMANDSTATE_IDLE)
    {
        currentCommand->state = COMMANDSTATE_SENDING;

        return HAL_I2C_Master_Transmit_IT(&hi2c1, device->deviceAddress, (uint8_t *)&currentCommand->args,
                                          currentCommand->argLength) == HAL_OK;
    }

    // Current command is executing; nothing to do at this time
    return true;
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

        if (currentCommand == NULL)
        {
            // No command to process; this usually indicates e.g. RDS or RSQ interrupt
            // so schedule the GetIntStatus command
            GetIntStatus(&radioDevice);
        }
        else if (currentCommand->state == COMMANDSTATE_WAITING_FOR_STC)
        {
            currentCommand->state = COMMANDSTATE_WAITING_FOR_CTS;
        }
        else if (currentCommand->state == COMMANDSTATE_WAITING_FOR_CTS)
        {
            if (currentCommand->responseLength > 0)
            {
                currentCommand->state = COMMANDSTATE_WAITING_FOR_RESPONSE_RETRIEVAL;
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
            currentCommand->state = COMMANDSTATE_RESPONSE_RECEIVED;
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

/**
 * @brief  Period elapsed callback in non-blocking mode
 * @param  htim TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (radioDevice.currentState == RADIOSTATE_TUNED_TO_STATION ||
        radioDevice.currentState == RADIOSTATE_DIGITAL_OUTPUT_ENABLED)
    {
        bool succeeded = RSQStatus(&radioDevice, FM_RSQ_STATUS_ARGS_NONE);

        if (!succeeded)
        {
            // Queue is full; when the device is streaming data there's a noise
            // problem which sometimes causes the CTS signal to be missed,
            // and thus the current command never completes

            // To recover, we clear the queue, and send a fault report
            memset(radioDevice.commandQueue.commands, 0, 10 * sizeof(Command_t));
            radioDevice.commandQueue.front = 0;
            radioDevice.commandQueue.back = 0;
            radioDevice.commandQueue.count = 0;

            Command_t fault = {0};

            fault.state = COMMANDSTATE_RESPONSE_RECEIVED;
            fault.args.bytes[0] = 0xAA; // This isn't a real command identifier, so can't use 'opCode'

            EnqueueCommand(&radioDevice, &fault);
        }
    }
}

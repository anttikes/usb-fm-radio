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

/* Global variables ----------------------------------------------------------*/

// clang-format off
RadioDevice_t radioDevice = {
    .deviceAddress = SI4705_I2C_ADDRESS,
    .currentState = RADIOSTATE_POWERDOWN,
    .currentFrequency = 0,
    .currentVolume = SI4705_VOLUME_MAX_SETTING / 2,
    .isMuted = false,
    .commandQueue = {
        .commands = {{0}},
        .count = 0,
        .front = 0,
        .back = 0
    }    
};
// clang-format on

/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
bool IsCommandQueueEmpty(CommandQueue_t *queue);
Command_t *PeekCommand(CommandQueue_t *queue);
Command_t *PopCommand(CommandQueue_t *queue);
Report_t *PopReport(ReportQueue_t *queue);

/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Processes the commands in the queue, if there are any
 * @param  device Pointer to the radio device structure
 *
 * @retval True if a command was somehow processed; false otherwise
 */
bool ProcessCommand(RadioDevice_t *device)
{
    if (IsCommandQueueEmpty(&device->commandQueue))
    {
        return false;
    }

    volatile Command_t *currentCommand = PeekCommand(&device->commandQueue);

    if (currentCommand == NULL)
    {
        return false;
    }

    if (currentCommand->state == COMMANDSTATE_READY)
    {
        if (currentCommand->args.opCode == CMD_ID_POWER_UP && currentCommand->responseLength == 0)
        {
            device->currentState = RADIOSTATE_POWERUP;
        }
        else if (currentCommand->args.opCode == CMD_ID_POWER_DOWN)
        {
            device->currentState = RADIOSTATE_POWERDOWN;

            HAL_TIM_Base_Stop(&htim16);
        }
        else if (currentCommand->args.opCode == CMD_ID_FM_TUNE_FREQ ||
                 currentCommand->args.opCode == CMD_ID_FM_SEEK_START)
        {
            device->currentState = RADIOSTATE_TUNED_TO_STATION;

            HAL_TIM_Base_Start_IT(&htim16);

            // After tuning or seek has completed, set the sample rate so the chip begins sending audio samples
            SetProperty(&radioDevice, PROP_ID_DIGITAL_OUTPUT_SAMPLE_RATE, CFG_TUD_AUDIO_FUNC_1_SAMPLE_RATE);

            // Schedule a GetTuneStatus to update the current frequency reading
            TuneStatus(device, GET_TUNE_STATUS_ARGS_NONE);
        }
        else if (currentCommand->args.opCode == CMD_ID_FM_TUNE_STATUS)
        {
            // If the channel is valid, update the frequency reading; otherwise reset it to zero
            if (currentCommand->response[1] & 0x01)
            {
                device->currentFrequency =
                    (uint16_t)((currentCommand->response[2] << 8) | (currentCommand->response[3] << 0));
            }
            else
            {
                device->currentFrequency = 0;
            }
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
            else if (property == PROP_ID_RX_VOLUME)
            {
                device->currentVolume = value;
            }
            else if (property == PROP_ID_RX_HARD_MUTE)
            {
                if (value == 0)
                {
                    device->isMuted = false;
                }
                else if (value & 0b11)
                {
                    device->isMuted = true;
                }

                // Per-channel mute status is not supported
            }

            // Tuner programming guide outlines that a property
            // set operation always completes in 10 ms
            HAL_Delay(10);
        }

        PopCommand(&device->commandQueue);

        currentCommand = PeekCommand(&device->commandQueue);

        if (currentCommand == NULL)
        {
            return false;
        }
    }
    else if (currentCommand->state == COMMANDSTATE_RESPONSE_RECEIVED)
    {
        switch (currentCommand->args.opCode)
        {
        case CMD_ID_GET_INT_STATUS:
            ProcessIntStatus(device, (Command_t *)currentCommand);
            break;

        case CMD_ID_FM_RSQ_STATUS:
            ProcessRSQStatus(device, (Command_t *)currentCommand);
            break;

        default:
            break;
        }

        currentCommand->state = COMMANDSTATE_READY;
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
 * @brief  Processes the next report from the queue, if there are any
 * @param  device Pointer to the radio device structure
 *
 * @retval True if a report was processed; false otherwise
 */
bool ProcessReport(RadioDevice_t *device)
{
    Report_t *currentReport = PopReport(&device->reportQueue);

    if (currentReport == NULL)
    {
        return false;
    }

    return tud_hid_report(currentReport->identifier, currentReport->bytes.raw, MAX_STRUCT_SIZE);
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

    if (queue->count >= MAX_COMMAND_QUEUE_CAPACITY)
    {
        /* Queue full */
        return false;
    }

    queue->commands[queue->back] = *command;
    queue->back = (uint8_t)((queue->back + 1) % MAX_COMMAND_QUEUE_CAPACITY);
    queue->count++;

    return true;
}

/**
 * @brief  Enqueues the given report into the queue
 * @param  device Pointer to the radio device structure
 * @param  command Pointer to the report
 *
 * @retval True if the report was enqueued; false otherwise
 */
bool EnqueueReport(RadioDevice_t *device, Report_t *report)
{
    if (device == NULL || report == NULL)
    {
        return false;
    }

    volatile ReportQueue_t *queue = &device->reportQueue;

    if (queue->count >= MAX_REPORT_QUEUE_CAPACITY)
    {
        /* Queue full */
        return false;
    }

    queue->reports[queue->back] = *report;
    queue->back = (uint8_t)((queue->back + 1) % MAX_REPORT_QUEUE_CAPACITY);
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
        Command_t *currentCommand = PeekCommand(&radioDevice.commandQueue);

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
        Command_t *currentCommand = PeekCommand(&radioDevice.commandQueue);

        if (currentCommand->state == COMMANDSTATE_SENDING)
        {
            if (currentCommand->args.opCode == CMD_ID_FM_TUNE_FREQ ||
                currentCommand->args.opCode == CMD_ID_FM_SEEK_START)
            {
                radioDevice.currentState = RADIOSTATE_TUNE_IN_PROGRESS;

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
        Command_t *currentCommand = PeekCommand(&radioDevice.commandQueue);

        if (currentCommand->state == COMMANDSTATE_RECEIVING_RESPONSE)
        {
            currentCommand->state = COMMANDSTATE_RESPONSE_RECEIVED;
        }
    }
}

/**
 * @brief  Invoked by HAL when an I2C transaction encounters an error.
 * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
 *                the configuration information for the specified I2C.
 * @retval None
 */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hi2c);
}

/**
 * @brief  Period elapsed callback in non-blocking mode
 * @param  htim TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM16)
    {
        // Timer 16 is used to periodically query RSQ status when tuned to a station
        if (radioDevice.currentState != RADIOSTATE_POWERDOWN)
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

                Report_t fault = {0};

                fault.identifier = REPORT_IDENTIFIER_QUEUE_RESET;

                EnqueueReport(&radioDevice, &fault);
            }
        }
    }
    else if (htim->Instance == TIM17)
    {
        // Timer 17 is used to periodically report device state
        Report_t report = {0};

        report.identifier = REPORT_IDENTIFIER_RADIO_STATUS;

        report.bytes.radioStatus.currentState = radioDevice.currentState;
        report.bytes.radioStatus.currentFrequency = radioDevice.currentFrequency;
        report.bytes.radioStatus.currentVolume = radioDevice.currentVolume;
        report.bytes.radioStatus.commandQueueCount = radioDevice.commandQueue.count;
        report.bytes.radioStatus.reportQueueCount = radioDevice.reportQueue.count;
        report.bytes.radioStatus.isMuted = radioDevice.isMuted;

        EnqueueReport(&radioDevice, &report);
    }
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Determines if the queue is empty
 * @param  queue Pointer to the queue
 *
 * @retval True if the queue is empty; false otherwise
 */
bool IsCommandQueueEmpty(CommandQueue_t *queue)
{
    if (queue == NULL)
    {
        return true;
    }

    return (queue->count == 0);
}

/**
 * @brief  Determines if the queue is empty
 * @param  queue Pointer to the queue
 *
 * @retval True if the queue is empty; false otherwise
 */
bool IsReportQueueEmpty(ReportQueue_t *queue)
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
Command_t *PeekCommand(CommandQueue_t *queue)
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
Command_t *PopCommand(CommandQueue_t *queue)
{
    if (queue == NULL || queue->count == 0)
    {
        return NULL;
    }

    Command_t *command = &queue->commands[queue->front];
    const uint8_t capacity = (uint8_t)(sizeof(queue->commands) / sizeof(queue->commands[0]));
    queue->front = (uint8_t)((queue->front + 1) % capacity);
    queue->count--;

    return command;
}

/**
 * @brief  Pops the first report from the queue, removing it
 * @param  queue Pointer to the queue
 *
 * @retval Pointer to the first report in the queue or NULL if the queue is empty
 */
Report_t *PopReport(ReportQueue_t *queue)
{
    if (queue == NULL || queue->count == 0)
    {
        return NULL;
    }

    Report_t *report = &queue->reports[queue->front];
    const uint8_t capacity = (uint8_t)(sizeof(queue->reports) / sizeof(queue->reports[0]));
    queue->front = (uint8_t)((queue->front + 1) % capacity);
    queue->count--;

    return report;
}

/**
 ******************************************************************************
 * @file    device.h
 * @brief   Header for device.c file.
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
#ifndef __DEVICE_H__
#define __DEVICE_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "common.h"
#include "reports.h"
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/
typedef enum _CommandState_t : uint8_t
{
    /* Idle; the command is not being processed yet */
    COMMANDSTATE_IDLE = 0x00,

    /* The command is currently being sent */
    COMMANDSTATE_SENDING = 0x01,

    /* Command was sent, and is waiting for CTS interrupt */
    COMMANDSTATE_WAITING_FOR_CTS = 0x02,

    /* Command was sent, and is waiting for STC interrupt */
    COMMANDSTATE_WAITING_FOR_STC = 0x03,

    /* Command is waiting for a response retrieval to begin */
    COMMANDSTATE_WAITING_FOR_RESPONSE_RETRIEVAL = 0x04,

    /* Receiving a response to a command */
    COMMANDSTATE_RECEIVING_RESPONSE = 0x05,

    /* Response received, waiting for response to be processed */
    COMMANDSTATE_RESPONSE_RECEIVED = 0x06,

    /* The command is complete */
    COMMANDSTATE_READY = 0x07,
} CommandState_t;

typedef struct _Command_t
{
    /* Status of the command */
    CommandState_t state;

    /* Command arguments; first one is the opcode, and up to seven other arguments */
    union {
        CommandIdentifiers_t opCode;
        uint8_t bytes[8];
    } args;

    /* Number of arguments used */
    uint8_t argLength;

    /* Response buffer; first one is the status byte, and up to 15 other bytes */
    uint8_t response[16];

    /* Number of expected response bytes */
    uint8_t responseLength;
} Command_t;

#define MAX_COMMAND_QUEUE_CAPACITY 20

typedef struct _CommandQueue_t
{
    /* Array of commands in the queue */
    Command_t commands[MAX_COMMAND_QUEUE_CAPACITY];

    /* Number of commands in the queue */
    uint8_t count;

    /* Index of the front of the queue */
    uint8_t front;

    /* Index of the back of the queue */
    uint8_t back;
} CommandQueue_t;

#define MAX_REPORT_QUEUE_CAPACITY 5

typedef struct _ReportQueue_t
{
    /* Array of reports in the queue */
    Report_t reports[MAX_REPORT_QUEUE_CAPACITY];

    /* Number of reports in the queue */
    uint8_t count;

    /* Index of the front of the queue */
    uint8_t front;

    /* Index of the back of the queue */
    uint8_t back;
} ReportQueue_t;

typedef struct _RadioDevice_t
{
    /* I2C address of the device */
    uint16_t deviceAddress;

    /* Holds the current state of the device */
    RadioState_t currentState;

    /* Holds the current frequency of the device, when tuned to a station */
    uint16_t currentFrequency;

    /* Holds the current volume level of the device */
    uint8_t currentVolume;

    /* Holds the mute status of the device */
    bool isMuted;

    /* Holds the command queue */
    CommandQueue_t commandQueue;

    /* Holds the report queue */
    ReportQueue_t reportQueue;
} RadioDevice_t;

/* Exported constants --------------------------------------------------------*/

// Hard-coded addresses recognized by the Si4705 device, left-shifted for HAL-compatibility
#define SI4705_I2C_ADDRESS (uint16_t)(0x11 << 1)
#define SI4705_I2C_ALT_ADDRESS (uint16_t)(0x63 << 1)

// Minimum and maximum volume
#define SI4705_VOLUME_MAX_SETTING 63
#define SI4705_VOLUME_MIN_SETTING 0

// Minimum and maximum reference clock prescaler values
#define SI4705_REFCLK_PRESCALE_MIN_SETTING 1
#define SI4705_REFCLK_PRESCALE_MAX_SETTING 4095

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/
extern RadioDevice_t radioDevice;

/* Exported functions --------------------------------------------------------*/
extern bool EnqueueCommand(RadioDevice_t *device, Command_t *command);
extern bool ProcessCommand(RadioDevice_t *device);
extern bool EnqueueReport(RadioDevice_t *device, Report_t *report);
extern bool ProcessReport(RadioDevice_t *device);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __DEVICE_H__ */

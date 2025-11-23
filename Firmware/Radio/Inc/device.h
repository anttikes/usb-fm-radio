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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEVICE_H__
#define __DEVICE_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "common.h"
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/
typedef enum _RadioState_t
{
    /* Radio is in power-down state; send PowerUp command to wake it */
    RADIOSTATE_POWERDOWN = 0x00,

    /* Radio is in power-up state; set properties, tune to station, begin seek or send power-down to power it down */
    RADIOSTATE_POWERUP = 0x01,

    /* Radio is tuned to a station, and is receiving a signal */
    RADIOSTATE_TUNED_TO_STATION = 0x02,

    /* Radio is sampling and sending digital output of the received audio signal */
    RADIOSTATE_DIGITAL_OUTPUT_ENABLED = 0x03
} RadioState_t;

typedef enum _CommandState_t
{
    /* Idle; the command is not being processed yet */
    COMMANDSTATE_IDLE = 0x00,

    /* The command is currently being sent */
    COMMANDSTATE_SENDING = 0x01,

    /* Command was sent, and we are waiting for CTS interrupt */
    COMMANDSTATE_WAITING_FOR_CTS = 0x02,

    /* Command was sent, and we are waiting for STC interrupt */
    COMMANDSTATE_WAITING_FOR_STC = 0x03,

    /* Receiving a response to a command */
    COMMANDSTATE_RECEIVING_RESPONSE = 0x04,

    /* The command is complete */
    COMMANDSTATE_READY = 0x05,
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

    /* Response buffer; maximum of 15 */
    uint8_t response[15];

    /* Number of expected response bytes */
    uint8_t responseLength;
} Command_t;

typedef struct _CommandQueue_t
{
    /* Array of commands in the queue */
    Command_t commands[10];

    /* Number of commands in the queue */
    uint8_t count;

    /* Index of the front of the queue */
    uint8_t front;

    /* Index of the back of the queue */
    uint8_t back;
} CommandQueue_t;

typedef struct _RadioDevice_t
{
    /* I2C address of the device */
    uint16_t deviceAddress;

    /* Holds the current state of the radio device */
    RadioState_t currentState;

    /* Holds the command queue */
    CommandQueue_t commandQueue;
} RadioDevice_t;

/* Exported constants --------------------------------------------------------*/

// Hard-coded addresses for the Si4705 device, left-shifted for HAL-compatibility
#define SI4705_I2C_ADDRESS (uint16_t)(0x11 << 1)
#define SI4705_I2C_ALT_ADDRESS (uint16_t)(0x63 << 1)

/* Exported macro ------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/
extern RadioDevice_t radioDevice;

/* Exported functions --------------------------------------------------------*/
bool InitializeQueue(CommandQueue_t *queue);
bool EnqueueCommand(RadioDevice_t *device, Command_t *command);
bool ProcessCommand(RadioDevice_t *device);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __DEVICE_H__ */

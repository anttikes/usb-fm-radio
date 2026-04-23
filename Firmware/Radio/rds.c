/**
 ******************************************************************************
 * @file    rds.c
 * @brief   Implements the librdsparser callback functions, and holds the
 *          global state of the library
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
#include "rds.h"

/* Global variables ----------------------------------------------------------*/
rdsparser_t rdsParser;

/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
bool RDSInit()
{
    rdsparser_init(&rdsParser);

    // TODO: Register callbacks here

    return true;
}

/* External callbacks --------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

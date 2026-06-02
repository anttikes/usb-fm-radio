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
#include "device.h"
#include "reports.h"
#include <string.h>

/* Global variables ----------------------------------------------------------*/
rdsparser_t rdsParser;

/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void callback_ps(rdsparser_t *rds, void *user_data);
void callback_rt(rdsparser_t *rds, rdsparser_rt_flag_t flag, void *user_data);

/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Initializes the RDS parser, and registers the callback methods
 *
 * @retval True if initialization succeeded; false otherwise
 */
bool RDSInit()
{
    rdsparser_init(&rdsParser);

    rdsparser_set_text_correction(&rdsParser, RDSPARSER_TEXT_PS, RDSPARSER_BLOCK_TYPE_INFO,
                                  RDSPARSER_BLOCK_ERROR_LARGE);
    rdsparser_set_text_correction(&rdsParser, RDSPARSER_TEXT_PS, RDSPARSER_BLOCK_TYPE_DATA,
                                  RDSPARSER_BLOCK_ERROR_LARGE);
    rdsparser_set_text_correction(&rdsParser, RDSPARSER_TEXT_RT, RDSPARSER_BLOCK_TYPE_INFO,
                                  RDSPARSER_BLOCK_ERROR_LARGE);
    rdsparser_set_text_correction(&rdsParser, RDSPARSER_TEXT_RT, RDSPARSER_BLOCK_TYPE_DATA,
                                  RDSPARSER_BLOCK_ERROR_LARGE);

    rdsparser_set_text_progressive(&rdsParser, RDSPARSER_TEXT_PS, RDSPARSER_PROGRESSIVE_AUTO);
    rdsparser_set_text_progressive(&rdsParser, RDSPARSER_TEXT_RT, RDSPARSER_PROGRESSIVE_AUTO);

    rdsparser_register_ps(&rdsParser, callback_ps);
    rdsparser_register_rt(&rdsParser, callback_rt);

    return true;
}

/**
 * @brief  Processes a new RDS group received from the radio device
 * @param  blockA Block A of the group
 * @param  blockB Block B of the group
 * @param  blockC Block C of the group
 * @param  blockD Block D of the group
 * @param  blockAErrors Errors in Block A
 * @param  blockBErrors Errors in Block B
 * @param  blockCErrors Errors in Block C
 * @param  blockDErrors Errors in Block D
 */
void ProcessRDSData(uint16_t blockA, uint16_t blockB, uint16_t blockC, uint16_t blockD, uint8_t blockAErrors,
                    uint8_t blockBErrors, uint8_t blockCErrors, uint8_t blockDErrors)
{
    rdsparser_parse(&rdsParser, (rdsparser_data_t){blockA, blockB, blockC, blockD},
                    (rdsparser_error_t){blockAErrors, blockBErrors, blockCErrors, blockDErrors});
}

/**
 * @brief  Resets the RDS parser's state
 */
void RDSReset()
{
    rdsparser_clear(&rdsParser);
    rdsDataStable = false;
}

/* External callbacks --------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/**
 * @brief  Invoked by librdsparser when Programme Service (PS) has been processed
 * @param  rds Pointer to the parser instance
 * @param  user_data Pointer to user data; NULL if no data was provided
 */
void callback_ps(rdsparser_t *rds, void *user_data)
{
    // const rdsparser_string_t *ps = rdsparser_get_ps(rds);
    // const rdsparser_string_char_t *ps_content = rdsparser_string_get_content(ps);
    // uint8_t length = rdsparser_string_get_length(ps);

    // Report_t report = {0};

    // report.identifier = REPORT_IDENTIFIER_RDS_PROGRAMME_SERVICE;

    // memcpy(report.bytes.programmeService.programmeService, ps_content, length);

    // EnqueueReport(&radioDevice, &report);
}

/**
 * @brief  Invoked by librdsparser when Radio Text (RT) has been processed
 * @param  rds Pointer to the parser instance
 * @param  flag RT flag
 * @param  user_data Pointer to user data; NULL if no data was provided
 */
void callback_rt(rdsparser_t *rds, rdsparser_rt_flag_t flag, void *user_data)
{
    // const rdsparser_string_t *rt = rdsparser_get_rt(rds, flag);
    // const rdsparser_string_char_t *rt_content = rdsparser_string_get_content(rt);
    // const uint8_t length = rdsparser_string_get_length(rt);

    // Report_t report = {0};

    // report.identifier = REPORT_IDENTIFIER_RDS_RADIO_TEXT;

    // memcpy(report.bytes.radioText.radioText, rt_content, length);

    // EnqueueReport(&radioDevice, &report);
}

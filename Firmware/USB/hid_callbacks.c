/**
 ******************************************************************************
 * @file           : hid_callbacks.c
 * @brief          : Implements the HID callback functions
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
#include "commands.h"
#include "device.h"
#include "hid_config.h"
#include "tusb.h"

extern uint8_t desc_hid_report[];

/**
 * @brief  Invoked when the GET HID REPORT DESCRIPTOR request is received
 * @param  itf Interface number
 *
 * @retval Pointer to the HID descriptor structure
 */
uint8_t const *tud_hid_descriptor_report_cb(uint8_t itf)
{
    (void)itf;

    return desc_hid_report;
}

/**
 * @brief  Invoked when the GET REPORT request is received
 * @param  itf Interface number
 *
 * @retval Amount of bytes to report back to the host; retuning zero will STALL the endpoint
 */
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer,
                               uint16_t reqlen)
{
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)reqlen;

    return 0;
}

/**
 * @brief  Invoked when received the SET REPORT request is received or when
 *         OUT data has been received on the endpoint
 *
 * @remark For OUT data, report ID is zero and type equals OUTPUT
 */
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer,
                           uint16_t bufsize)
{
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)bufsize;

    ReportIdentifier_t reportId = (ReportIdentifier_t)buffer[0];

    switch (reportId)
    {
    case REPORT_IDENTIFIER_TUNE_FREQ:
        TuneFreqRequest_t tuneFreqRequest = {0};
        memcpy(&tuneFreqRequest, &buffer[1], sizeof(TuneFreqRequest_t));

        TuneFreq(&radioDevice, FM_TUNE_FREQ_ARGS_NONE, tuneFreqRequest.frequency);

        break;

    case REPORT_IDENTIFIER_SEEK_START:
        SeekStartRequest_t seekStartRequest = {0};
        memcpy(&seekStartRequest, &buffer[1], sizeof(SeekStartRequest_t));

        CMD_FM_SEEK_START_ARGS seekStartArgs = SEEK_START_ARGS_NONE;

        if (seekStartRequest.wrap)
        {
            seekStartArgs |= SEEK_START_ARGS_WRAP;
        }

        if (seekStartRequest.seekUp)
        {
            seekStartArgs |= SEEK_START_ARGS_UP;
        }

        SeekStart(&radioDevice, seekStartArgs);

        break;

    default:
        // Unrecognized report ID; ignore
        break;
    }

    return;
}

/**
 * @brief  Invoked when a report has successfully been sent to host
 * @param  itf Interface number
 * @remark For composite reports, report[0] is report ID
 */
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report, uint16_t len)
{
    (void)instance;
    (void)report;
    (void)len;

    return;
}

/**
 * @brief  Invoked when a report transfer has failed
 * @param  itf Interface number
 */
void tud_hid_report_failed_cb(uint8_t instance, hid_report_type_t report_type, uint8_t const *report,
                              uint16_t xferred_bytes)
{
    (void)instance;
    (void)report_type;
    (void)report;
    (void)xferred_bytes;

    return;
}

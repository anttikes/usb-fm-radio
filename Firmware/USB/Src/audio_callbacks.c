/**
 ******************************************************************************
 * @file           : audio_callbacks.c
 * @brief          : Implements the Audio callback functions
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 Antti Keskinen
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
#include "audio_config.h"
#include "i2s.h"
#include "tusb.h"
#include <stdbool.h>

// TODO: Move these to the radio struct
bool mute = false;
uint16_t volume = 31;

/**
 * @brief  Invoked to handle the SET INTERFACE request
 * @param  rhport Root hub port to which the request came
 * @param  p_request Pointer to request data
 *
 * @retval True if the call succeeded; false otherwise. Returning false will stall EP0
 */
bool tud_audio_set_itf_cb(uint8_t rhport, tusb_control_request_t const *p_request)
{
    (void)rhport;

    // uint8_t const itf = tu_u16_low(tu_le16toh(p_request->wIndex));
    uint8_t const alt = tu_u16_low(tu_le16toh(p_request->wValue));

    // Clear buffer when streaming format is changed
    if (alt == 1)
    {
        TU_BREAKPOINT();
    }
    else
    {
        TU_BREAKPOINT();
    }

    return true;
}

/**
 * @brief  Invoked to handle a SET INTERFACE request to close an endpoint
 * @param  rhport Root hub port to which the request came
 * @param  p_request Pointer to request data
 *
 * @retval True if the call succeeded; false otherwise. Returning false will stall EP0
 */
bool tud_audio_set_itf_close_ep_cb(uint8_t rhport, tusb_control_request_t const *p_request)
{
    (void)rhport;
    (void)p_request;

    return true;
}

/**
 * @brief  Invoked to handle an audio class-specific SET request for an endpoint
 * @param  rhport Root hub port to which the request came
 * @param  p_request Pointer to request data
 * @param  pBuff Pointer to data buffer
 *
 * @retval True if the call succeeded; false otherwise. Returning false will stall EP0
 */
bool tud_audio_set_req_ep_cb(uint8_t rhport, tusb_control_request_t const *p_request, uint8_t *pBuff)
{
    (void)rhport;
    (void)p_request;
    (void)pBuff;

    return false;
}

/**
 * @brief  Invoked to handle an audio class-specific SET request for an interface
 * @param  rhport Root hub port to which the request came
 * @param  p_request Pointer to request data
 *
 * @retval True if the call succeeded; false otherwise. Returning false will stall EP0
 */
bool tud_audio_set_req_itf_cb(uint8_t rhport, tusb_control_request_t const *p_request, uint8_t *pBuff)
{
    (void)rhport;
    (void)p_request;
    (void)pBuff;

    return false;
}

/**
 * @brief  Invoked to handle an audio class-specific SET request for an entity
 * @param  rhport Root hub port to which the request came
 * @param  p_request Pointer to request data
 *
 * @retval True if the call succeeded; false otherwise. Returning false will stall EP0
 */
bool tud_audio_set_req_entity_cb(uint8_t rhport, tusb_control_request_t const *p_request, uint8_t *pBuff)
{
    (void)rhport;

    // uint8_t channelNum = TU_U16_LOW(p_request->wValue);       // We have only one channel so this doesn't make a
    // difference
    uint8_t ctrlSel = TU_U16_HIGH(p_request->wValue);
    // uint8_t itf = TU_U16_LOW(p_request->wIndex); 			// Since we have only one audio function implemented, we
    // do not need the itf value
    uint8_t entityID = TU_U16_HIGH(p_request->wIndex);

    if (entityID == ENTITY_ID_FEATURE_UNIT)
    {
        // The feature unit has master channel mute and volume
        switch (ctrlSel)
        {
        case AUDIO10_FU_CTRL_VOLUME:
            if (p_request->bRequest != AUDIO10_CS_REQ_SET_CUR)
            {
                TU_BREAKPOINT();
                return false;
            }

            // Only first form is supported
            TU_VERIFY(p_request->wLength == 2);

            volume = tu_unaligned_read16(pBuff);
            return true;

        case AUDIO10_FU_CTRL_MUTE:
            if (p_request->bRequest != AUDIO10_CS_REQ_SET_CUR)
            {
                TU_BREAKPOINT();
                return false;
            }

            // Only first form is supported
            TU_VERIFY(p_request->wLength == 1);

            mute = pBuff[0];
            return true;

        default:
            TU_BREAKPOINT();
            return false;
        }
    }

    return false;
}

/**
 * @brief  Invoked to handle an audio class-specific GET request for an endpoint
 * @param  rhport Root hub port to which the request came
 * @param  p_request Pointer to request data
 *
 * @retval True if the call succeeded; false otherwise. Returning false will stall EP0
 */
bool tud_audio_get_req_ep_cb(uint8_t rhport, tusb_control_request_t const *p_request)
{
    (void)rhport;
    (void)p_request;

    return false;
}

/**
 * @brief  Invoked to handle an audio class-specific GET request for an interface
 * @param  rhport Root hub port to which the request came
 * @param  p_request Pointer to request data
 *
 * @retval True if the call succeeded; false otherwise. Returning false will stall EP0
 */
bool tud_audio_get_req_itf_cb(uint8_t rhport, tusb_control_request_t const *p_request)
{
    (void)rhport;
    (void)p_request;

    return false;
}

/**
 * @brief  Invoked to handle an audio class-specific GET request for an entity
 * @param  rhport Root hub port to which the request came
 * @param  p_request Pointer to request data
 *
 * @retval True if the call succeeded; false otherwise. Returning false will stall EP0
 */
bool tud_audio_get_req_entity_cb(uint8_t rhport, tusb_control_request_t const *p_request)
{
    // uint8_t channelNum = TU_U16_LOW(p_request->wValue);       // We have only one channel so this doesn't make a
    // difference
    uint8_t ctrlSel = TU_U16_HIGH(p_request->wValue);
    // uint8_t itf = TU_U16_LOW(p_request->wIndex); 			// Since we have only one audio function implemented, we
    // do not need the itf value
    uint8_t entityID = TU_U16_HIGH(p_request->wIndex);

    if (entityID == ENTITY_ID_FEATURE_UNIT)
    {
        // The feature unit has master channel mute and volume
        switch (ctrlSel)
        {
        case AUDIO10_FU_CTRL_VOLUME:
            switch (p_request->bRequest)
            {
            case AUDIO10_CS_REQ_GET_CUR:
                return tud_audio_buffer_and_schedule_control_xfer(rhport, p_request, &volume, sizeof(volume));

            case AUDIO10_CS_REQ_GET_MIN: {
                uint16_t minVolume = 0;
                return tud_audio_buffer_and_schedule_control_xfer(rhport, p_request, &minVolume, sizeof(minVolume));
            }

            case AUDIO10_CS_REQ_GET_MAX: {
                uint16_t maxVolume = 64;
                return tud_audio_buffer_and_schedule_control_xfer(rhport, p_request, &maxVolume, sizeof(maxVolume));
            }

            case AUDIO10_CS_REQ_GET_RES: {
                uint16_t res = 1;
                return tud_audio_buffer_and_schedule_control_xfer(rhport, p_request, &res, sizeof(res));
            }

            default:
                TU_BREAKPOINT();
                return false;
            }

        case AUDIO20_FU_CTRL_MUTE:
            return tud_audio_buffer_and_schedule_control_xfer(rhport, p_request, &mute, 1);

        default:
            TU_BREAKPOINT();
            return false;
        }
    }

    return false;
}

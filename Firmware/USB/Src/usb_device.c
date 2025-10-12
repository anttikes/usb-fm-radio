/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : usb_device.c
 * @brief          : Implements the callback functions for TinyUSB
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
/* USER CODE END Header */

#include <stdlib.h>
#include "tusb.h"

uint32_t samplingFrequency = CFG_TUD_AUDIO_FUNC_1_SAMPLE_RATE;
audio_control_range_4_n_t(1) samplingFrequencyRange =
{
	.wNumSubRanges = 1,
	.subrange[0].bMin = CFG_TUD_AUDIO_FUNC_1_SAMPLE_RATE,
	.subrange[0].bMax = CFG_TUD_AUDIO_FUNC_1_SAMPLE_RATE,
	.subrange[0].bRes = 0,
};

bool clockValid = true;

bool mute = false;

uint16_t volume = 31;
audio_control_range_2_n_t(1) volumeRange =
{
	.wNumSubRanges = 1,
	.subrange[0].bMin = 0,
	.subrange[0].bMax = 64,
	.subrange[0].bRes = 1
};

/**
 * @brief  Invoked when audio set interface request received
 * @param  rhport Root hub port to which the request came
 * @param  p_request Pointer to request data
 *
 * @retval True if the call succeeded; false otherwise. Returning false will stall EP0
 */
bool tud_audio_set_itf_cb(uint8_t rhport, tusb_control_request_t const * p_request)
{
	(void) rhport;
	(void) p_request;

	return true;
}

/**
 * @brief  Invoked when audio set interface request received which closes an endpoint
 * @param  rhport Root hub port to which the request came
 * @param  p_request Pointer to request data
 *
 * @retval True if the call succeeded; false otherwise. Returning false will stall EP0
 */
bool tud_audio_set_itf_close_ep_cb(uint8_t rhport, tusb_control_request_t const * p_request)
{
	(void) rhport;
	(void) p_request;

	return true;
}

/**
 * @brief  Invoked when audio class specific set request received for an endpoint
 * @param  rhport Root hub port to which the request came
 * @param  p_request Pointer to request data
 * @param  pBuff Pointer to data buffer
 *
 * @retval True if the call succeeded; false otherwise. Returning false will stall EP0
 */
bool tud_audio_set_req_ep_cb(uint8_t rhport, tusb_control_request_t const * p_request, uint8_t *pBuff)
{
	(void) rhport;
	(void) p_request;
	(void) pBuff;

	return false;
}

/**
 * @brief  Invoked when audio class specific set request received for an interface
 * @param  rhport Root hub port to which the request came
 * @param  p_request Pointer to request data
 *
 * @retval True if the call succeeded; false otherwise. Returning false will stall EP0
 */
bool tud_audio_set_req_itf_cb(uint8_t rhport, tusb_control_request_t const * p_request, uint8_t *pBuff)
{
	(void) rhport;
	(void) p_request;
	(void) pBuff;

	return false;
}

/**
 * @brief  Invoked when audio class specific set request received for an entity
 * @param  rhport Root hub port to which the request came
 * @param  p_request Pointer to request data
 *
 * @retval True if the call succeeded; false otherwise. Returning false will stall EP0
 */
bool tud_audio_set_req_entity_cb(uint8_t rhport, tusb_control_request_t const * p_request, uint8_t *pBuff)
{
	(void) rhport;

	//uint8_t channelNum = TU_U16_LOW(p_request->wValue);       // We have only one channel so this doesn't make a difference
	uint8_t ctrlSel = TU_U16_HIGH(p_request->wValue);
	// uint8_t itf = TU_U16_LOW(p_request->wIndex); 			// Since we have only one audio function implemented, we do not need the itf value
	uint8_t entityID = TU_U16_HIGH(p_request->wIndex);

	if (entityID == ENTITY_ID_FEATURE_UNIT) {
		// The feature unit has master channel mute and volume
		switch(ctrlSel)
		{
			case AUDIO_FU_CTRL_VOLUME:
				TU_VERIFY(p_request->wLength == sizeof(audio_control_cur_2_t));

				volume = (uint16_t) ((audio_control_cur_2_t *) pBuff)->bCur;
				return true;

			case AUDIO_FU_CTRL_MUTE:
				TU_VERIFY(p_request->wLength == sizeof(audio_control_cur_1_t));

				mute = ((audio_control_cur_1_t*) pBuff)->bCur;
				return true;

			default:
				TU_BREAKPOINT();
				return false;
		}
	}

	return false;
}

/**
 * @brief  Invoked when audio class specific get request received for an EP
 * @param  rhport Root hub port to which the request came
 * @param  p_request Pointer to request data
 *
 * @retval True if the call succeeded; false otherwise. Returning false will stall EP0
 */
bool tud_audio_get_req_ep_cb(uint8_t rhport, tusb_control_request_t const * p_request)
{
	(void) rhport;
	(void) p_request;

	return false;
}

/**
 * @brief  Invoked when audio class specific get request received for an interface
 * @param  rhport Root hub port to which the request came
 * @param  p_request Pointer to request data
 *
 * @retval True if the call succeeded; false otherwise. Returning false will stall EP0
 */
bool tud_audio_get_req_itf_cb(uint8_t rhport, tusb_control_request_t const * p_request)
{
	(void) rhport;
	(void) p_request;

	return false;
}

/**
 * @brief  Invoked when audio class specific get request received for an entity
 * @param  rhport Root hub port to which the request came
 * @param  p_request Pointer to request data
 *
 * @retval True if the call succeeded; false otherwise. Returning false will stall EP0
 */
bool tud_audio_get_req_entity_cb(uint8_t rhport, tusb_control_request_t const * p_request)
{
	//uint8_t channelNum = TU_U16_LOW(p_request->wValue);       // We have only one channel so this doesn't make a difference
	uint8_t ctrlSel = TU_U16_HIGH(p_request->wValue);
	// uint8_t itf = TU_U16_LOW(p_request->wIndex); 			// Since we have only one audio function implemented, we do not need the itf value
	uint8_t entityID = TU_U16_HIGH(p_request->wIndex);

	if (entityID == ENTITY_ID_FEATURE_UNIT) {
		// The feature unit has master channel mute and volume
		switch(ctrlSel)
		{
			case AUDIO_FU_CTRL_VOLUME:
				switch(p_request->bRequest)
				{
					case AUDIO_CS_REQ_CUR:
						return tud_control_xfer(rhport, p_request, &volume, sizeof(volume));

					case AUDIO_CS_REQ_RANGE:
						return tud_audio_buffer_and_schedule_control_xfer(rhport, p_request, (void*) &volumeRange, sizeof(volumeRange));

					default:
						TU_BREAKPOINT();
						return false;
				}

			case AUDIO_FU_CTRL_MUTE:
				return tud_control_xfer(rhport, p_request, &mute, 1);

			default:
				TU_BREAKPOINT();
				return false;
		}
	}

	if (entityID == ENTITY_ID_CLOCK_SOURCE) {
		switch(ctrlSel)
		{
			case AUDIO_CS_CTRL_SAM_FREQ:
				switch(p_request->bRequest)
				{
					case AUDIO_CS_REQ_CUR:
						return tud_audio_buffer_and_schedule_control_xfer(rhport, p_request, &samplingFrequency, sizeof(samplingFrequency));

					case AUDIO_CS_REQ_RANGE:
						return tud_control_xfer(rhport, p_request, &samplingFrequencyRange, sizeof(samplingFrequencyRange));

					default:
						TU_BREAKPOINT();
						return false;
				}

				case AUDIO_CS_CTRL_CLK_VALID:
					return tud_control_xfer(rhport, p_request, &clockValid, sizeof(clockValid));

			default:
				TU_BREAKPOINT();
				return false;
		}
	}

	return false;
}

// // /**
//  ******************************************************************************
//  * @file           : hid_callbacks.c
//  * @brief          : Implements the HID callback functions
//  ******************************************************************************
//  * @attention
//  *
//  * Copyright (c) 2025 Antti Keskinen
//  * All rights reserved.
//  *
//  * This software is licensed under terms that can be found in the LICENSE file
//  * in the root directory of this software component.
//  * If no LICENSE file comes with this software, it is provided AS-IS.
//  *
//  ******************************************************************************
//  */
// #include "hid_config.h"
// #include "tusb.h"

// extern uint8_t desc_hid_report[];

// /**
//  * @brief  Invoked when the GET HID REPORT DESCRIPTOR request is received
//  * @param  itf Interface number
//  *
//  * @retval Pointer to the HID descriptor structure
//  */
// uint8_t const* tud_hid_descriptor_report_cb(uint8_t itf)
// {
//   (void) itf;

//   return desc_hid_report;
// }

// /**
//  * @brief  Invoked when the GET REPORT request is received
//  * @param  itf Interface number
//  *
//  * @retval Amount of bytes to report back to the host; retuning zero sends a status packet
//  */
// uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer,
// uint16_t reqlen)
// {
// 	(void) instance;
// 	(void) report_id;
// 	(void) report_type;
// 	(void) buffer;
// 	(void) reqlen;

// 	return 0;
// }

// /**
//  * @brief  Invoked when received the SET REPORT request is received or when OUT data has been received on the
//  endpoint
//  * @remark For OUT data, report ID is zero and type equals OUTPUT
//  */
// void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer,
// uint16_t bufsize)
// {
// 	(void) instance;
// 	(void) report_id;
// 	(void) report_type;
// 	(void) buffer;
// 	(void) bufsize;

// 	tud_hid_report(0, buffer, bufsize);

// 	return;
// }

// /**
//  * @brief  Invoked when a report has successfully been sent to host
//  * @param  itf Interface number
//  * @remark For composite reports, report[0] is report ID
//  */
// void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len)
// {
// 	(void) instance;
// 	(void) report;
// 	(void) len;

// 	return;
// }

// /**
//  * @brief  Invoked when a report transfer has failed
//  * @param  itf Interface number
//  */
// void tud_hid_report_failed_cb(uint8_t instance, hid_report_type_t report_type, uint8_t const* report, uint16_t
// xferred_bytes)
// {
// 	(void) instance;
//     (void) report_type;
//     (void) report;
//     (void) xferred_bytes;

// 	return;
// }

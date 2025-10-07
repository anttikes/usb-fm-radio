/**
 ******************************************************************************
 * @file    usbd_radio.c
 * @author  Antti Keskinen
 * @brief   This file provides an AM/FM Radio device that adheres
 *          to USB Audio and USB HID specifications.
 *
 *
 ******************************************************************************
 * @attention
 *
 * Copyright (c) Antti Keskinen
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 * @verbatim
 *
 *          ===================================================================
 *                           AM/FM Radio Class Description
 *          ===================================================================
 *           This driver manages two distinct USB devices
 *             - Audio Class 1.0 following the "USB Device Class Definition for Audio Devices V1.0 Mar 18, 1998"
 *             - HID Class 1.11 following the "USB DEvice Class Definition for Human Interface Devices V1.11 Jun 27th, 2001"

 *           This driver implements the following aspects of these two specifications:
 *             - Configuration descriptor
 *             - Audio Control interface (i.e. terminals, feature units like volume control, mute etc.)
 *			   - Audio Streaming Interfaces (without any endpoints, with mono and stereo, varying bit depths and sampling frequencies)
 *             - Audio Streaming IN Endpoint (isochronous, with adaptive sync enpoint)
 *             - Audio Sync IN Endpoint (isochronous)
 *             - Audio Synchronization type: Asynchronous (i.e. adaptive source)
 *             - Interfaces necessary to implement HID control (i.e. tuning to a station, device state)
 *
 * @note     In HS mode and when the DMA is used, all variables and data structures
 *           dealing with the DMA during the transaction process should be 32-bit aligned.
 *
 *           This device does not work with the USB Device Library's
 *           composite device scheme as it, in itself, is a composite device
 *
 *
 *  @endverbatim
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "usbd_radio.h"
#include "usbd_ctlreq.h"
#include "si4705.h"

/* Private defines -----------------------------------------------------------*/

/* Private function prototypes------------------------------------------------*/
static uint8_t USBD_RADIO_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_RADIO_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_RADIO_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t USBD_RADIO_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_RADIO_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_RADIO_EP0_RxReady(USBD_HandleTypeDef *pdev);
static uint8_t USBD_RADIO_EP0_TxReady(USBD_HandleTypeDef *pdev);
static uint8_t USBD_RADIO_SOF(USBD_HandleTypeDef *pdev);
static uint8_t USBD_RADIO_IsoINIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_RADIO_IsoOutIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t* USBD_RADIO_GetCfgDesc(uint16_t *length);
static void* USBD_AUDIO_GetAudioHeaderDesc(uint8_t *pConfDesc);

/* Private variables --------------------------------------------------------*/
USBD_ClassTypeDef USBD_RADIO = {
	USBD_RADIO_Init,
	USBD_RADIO_DeInit,
	USBD_RADIO_Setup,
	USBD_RADIO_EP0_TxReady,
	USBD_RADIO_EP0_RxReady,
	USBD_RADIO_DataIn,
	USBD_RADIO_DataOut,
	USBD_RADIO_SOF,
	USBD_RADIO_IsoINIncomplete,
	USBD_RADIO_IsoOutIncomplete,
	/*
	 * The USB Device Library stalls if a full-speed device
	 * is queried for HS, other speed or qualifier descriptions
	 */
	NULL,
	USBD_RADIO_GetCfgDesc,
	NULL,
	NULL
};

// @formatter:off
__ALIGN_BEGIN static uint8_t USBD_RADIO_CfgDesc[] __ALIGN_END = {
	/* USB 2.0 Standard Configuration descriptor */
	USB_CONF_DESC_SIZE, 					/* bLength */
	USB_DESC_TYPE_CONFIGURATION, 			/* bDescriptorType */
	LOBYTE(0x006E), 						/* wTotalLength (110 bytes => 0x6E) */
	HIBYTE(0x006E),
	0x02, 									/* bNumInterfaces */
	0x01, 									/* bConfigurationValue */
	0x00, 									/* iConfiguration */
	0x80, 									/* bmAttributes: Bus Powered */
	USBD_MAX_POWER, 						/* bMaxPower: 100 (mA) */
	/* 9 bytes */

	/* USB 2.0 Standard Interface descriptor (AudioControl) */
	USB_IF_DESC_SIZE, 						/* bLength */
	USB_DESC_TYPE_INTERFACE, 				/* bDescriptorType */
	0x00, 									/* bInterfaceNumber */
	0x00, 									/* bAlternateSetting */
	0x00, 									/* bNumEndpoints */
	USB_DEVICE_CLASS_AUDIO, 				/* bInterfaceClass */
	AUDIO_SUBCLASS_AUDIOCONTROL, 			/* bInterfaceSubClass */
	AUDIO_PROTOCOL_UNDEFINED, 				/* bInterfaceProtocol */
	0x00, 									/* iInterface */
	/* 18 bytes */

	/* USB Audio 1.0 Class-specific AudioControl Interface Descriptor */
	AUDIO_IF_DESC_SIZE, 					/* bLength */
	AUDIO_DESC_TYPE_CS_INTERFACE, 			/* bDescriptorType */
	AUDIO_DESC_SUBTYPE_HEADER, 				/* bDescriptorSubtype */
	LOBYTE(0x0100), 						/* bcdADC: 1.0 */
	HIBYTE(0x0100),
	LOBYTE(0x0028),							/* wTotalLength (58 - 18 = 40 => 0x0028) */
	HIBYTE(0x0028),
	0x01, 									/* bInCollection (Only one interface belongs to this group) */
	0x01, 									/* baInterfaceNr (That identifier is 0x01) */
	/* 27 bytes */

	/* USB Audio 1.0 Class-specific Input Terminal Descriptor */
	AUDIO_INPUT_TERMINAL_DESC_SIZE, 		/* bLength */
	AUDIO_DESC_TYPE_CS_INTERFACE, 			/* bDescriptorType */
	AUDIO_DESC_SUBTYPE_INPUT_TERMINAL, 		/* bDescriptorSubtype */
	0x01, 									/* bTerminalId */
	LOBYTE(0x0710), 						/* wTerminalType AM/FM Radio receiver 0x0710 */
	HIBYTE(0x0710),
	0x00, 									/* bAssocTerminal */
	0x02, 									/* bNrChannels (Two) */
	LOBYTE(0x0003),							/* wChannelConfig 0x0003 Left Front, Right Front */
	HIBYTE(0x0003),
	0x00, 									/* iChannelNames */
	0x00, 									/* iTerminal */
	/* 39 bytes */

	/* USB Audio 1.0 Class-specific Feature Unit Descriptor */
	0x0A,									/* bLength */
	AUDIO_DESC_TYPE_CS_INTERFACE,			/* bDescriptorType */
	AUDIO_DESC_SUBTYPE_FEATURE_UNIT,		/* bDescriptorSubtype */
	AUDIO_FEATURE_UNIT_ID,					/* bUnitId */
	0x01,									/* bSourceId (The input terminal) */
	0x01,									/* bControlSize (one byte per bmaControl) */
	AUDIO_CONTROL_MUTE |					/* bmaControls (master control) */
	AUDIO_CONTROL_VOLUME,
	AUDIO_CONTROL_NONE,						/* bmaControls (logical channel 1) */
	AUDIO_CONTROL_NONE,						/* bmaControls (logical channel 2) */
	0x00,									/* iFeature */
	/* 49 bytes */

	/* USB Audio 1.0 Class-specific Output Terminal Descriptor */
	AUDIO_OUTPUT_TERMINAL_DESC_SIZE,		/* bLength */
	AUDIO_DESC_TYPE_CS_INTERFACE,			/* bDescriptorType */
	AUDIO_DESC_SUBTYPE_OUTPUT_TERMINAL,		/* bDescriptorSubtype */
	0x03,									/* bTerminalId */
	LOBYTE(0x0101),							/* wTerminalType (USB Streaming) */
	HIBYTE(0x0101),
	0x00,									/* bAssocTerminal */
	AUDIO_FEATURE_UNIT_ID,					/* bSourceId */
	0x00,									/* iTerminal */
	/* 58 bytes */

	/* USB 2.0 Standard Interface descriptor (AudioStreaming) */
	USB_IF_DESC_SIZE, 						/* bLength */
	USB_DESC_TYPE_INTERFACE, 				/* bDescriptorType */
	0x01, 									/* bInterfaceNumber */
	0x00, 									/* bAlternateSetting */
	0x00, 									/* bNumEndpoints (None; this is the "mute" endpoint which is active by default) */
	USB_DEVICE_CLASS_AUDIO, 				/* bInterfaceClass */
	AUDIO_SUBCLASS_AUDIOSTREAMING,			/* bInterfaceSubClass */
	AUDIO_PROTOCOL_UNDEFINED, 				/* bInterfaceProtocol */
	0x00, 									/* iInterface */
	/* 67 bytes */

	/* USB 2.0 Standard Interface descriptor (AudioStreaming) */
	USB_IF_DESC_SIZE, 						/* bLength */
	USB_DESC_TYPE_INTERFACE, 				/* bDescriptorType */
	0x01, 									/* bInterfaceNumber */
	0x01, 									/* bAlternateSetting */
	0x01, 									/* bNumEndpoints (One; this is the "real" endpoint which provides audio) */
	USB_DEVICE_CLASS_AUDIO, 				/* bInterfaceClass */
	AUDIO_SUBCLASS_AUDIOSTREAMING, 			/* bInterfaceSubClass */
	AUDIO_PROTOCOL_UNDEFINED, 				/* bInterfaceProtocol */
	0x00, 									/* iInterface */
	/* 76 bytes */

	/* USB Audio 1.0 Class-specific AudioStreaming Interface Descriptor */
	AUDIO_STREAMING_INTERFACE_DESC_SIZE, 	/* bLength */
	AUDIO_DESC_TYPE_CS_INTERFACE, 			/* bDescriptorType */
	AUDIO_DESC_SUBTYPE_AS_GENERAL, 			/* bDescriptorSubtype */
	0x03, 									/* bTerminalLink (Output terminal) */
	0x01, 									/* bDelay */
	LOBYTE(AUDIO_FORMAT_TYPE_I_PCM), 		/* wFormatTag */
	HIBYTE(AUDIO_FORMAT_TYPE_I_PCM),
	/* 83 bytes */

	/* USB Audio 1.0 Class-specific Audio Type I Format Descriptor */
	0x0B, 									/* bLength */
	AUDIO_DESC_TYPE_CS_INTERFACE, 			/* bDescriptorType */
	AUDIO_DESC_SUBTYPE_FORMAT_TYPE, 		/* bDescriptorSubtype */
	AUDIO_FORMAT_TYPE_I, 					/* bFormatType */
	AUDIO_CHANNELS, 						/* bNrChannels (Two) */
	(AUDIO_BITDEPTH/8U), 					/* bSubFrameSize (from bitdepth to bytes) */
	AUDIO_BITDEPTH, 						/* bBitResolution */
	0x01, 									/* bSamFreqType (Only one discrete frequency supported) */
	LOBYTE(USBD_AUDIO_FREQ),				/* tSamFreq (Frequency expressed with 3 bytes) */
	HIBYTE(USBD_AUDIO_FREQ),
	LOBYTE(USBD_AUDIO_FREQ >> 16),
	/* 94 bytes */

	/* USB Audio 1.0 Standard AudioStreaming Endpoint Descriptor (audio data, with synchronization endpoint) */
	USB_AS_ISOC_EP_DESC_SIZE, 				/* bLength */
	USB_DESC_TYPE_ENDPOINT, 				/* bDescriptorType */
	AUDIO_IN_EP, 							/* bEndpointAddress */
	AUDIO_EP_ATTRIB_ISOC |					/* bmAttributes (Isochronous, synchronous)*/
	AUDIO_EP_ATTRIB_ASYNC,
	LOBYTE(USB_TOTAL_BUF_SIZE),		 		/* wMaxPacketSize expressed with 2 bytes  */
	HIBYTE(USB_TOTAL_BUF_SIZE),
	0x02, 									/* bInterval */
	0x00, 									/* bRefresh */
	0x00,									/* bSynchAddress */
	/* 103 bytes */

	/* USB Audio 1.0 Class-specific AudioStreaming Isochronous Endpoint Descriptor */
	AUDIO_AS_ISOC_EP_DESC_SIZE, 			/* bLength */
	AUDIO_DESC_TYPE_CS_ENDPOINT, 			/* bDescriptorType */
	AUDIO_DESC_SUBTYPE_EP_GENERAL, 			/* bDescriptorSubtype */
	AUDIO_CS_EP_ATTRIB_SAMPLING,			/* bmAttributes */
	0x00, 									/* bLockDelayUnits (Undefined) */
	LOBYTE(0x0000), 						/* wLockDelay */
	HIBYTE(0x0000),
	/* 110 bytes */
};
// @formatter:on

/* Private user code ---------------------------------------------------------*/
/**
 * @brief  USBD_RADIO_Init
 *         Initialize the AUDIO interface
 * @param  pdev: device instance
 * @param  cfgidx: Configuration index
 * @retval status
 */
static uint8_t USBD_RADIO_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
	UNUSED(cfgidx);
	USBD_RADIO_HandleTypeDef *hradio;

	/* Allocate the radio structure, including the buffer */
	hradio = (USBD_RADIO_HandleTypeDef*) USBD_malloc(sizeof(USBD_RADIO_HandleTypeDef));

	if (hradio == NULL) {
		pdev->pClassDataCmsit[pdev->classId] = NULL;
		return (uint8_t) USBD_EMEM;
	}

	memset(hradio->byteBuffer, 0, DMA_TOTAL_BUF_SIZE);

	pdev->pClassDataCmsit[pdev->classId] = (void*) hradio;
	pdev->pClassData = pdev->pClassDataCmsit[pdev->classId];

	/* Initialize control structure */
	hradio->control.cmd = AUDIO_CS_NONE;
	(void) USBD_memset(hradio->control.data, 0, USB_MAX_EP0_SIZE);
	hradio->control.len = 0U;
	hradio->control.ifOrEp = 0U;
	hradio->control.unitId = 0U;

	if (pdev->dev_speed == USBD_SPEED_HIGH) {
		pdev->ep_in[AUDIO_IN_EP & EP_ADDR_MSK].bInterval = AUDIO_HS_BINTERVAL;
	} else /* LOW and FULL-speed endpoints */
	{
		pdev->ep_in[AUDIO_IN_EP & EP_ADDR_MSK].bInterval = AUDIO_FS_BINTERVAL;
	}

	/* Open EP1 IN */
	(void) USBD_LL_OpenEP(pdev, AUDIO_IN_EP, USBD_EP_TYPE_ISOC, USB_TOTAL_BUF_SIZE);

	pdev->ep_in[AUDIO_IN_EP & EP_ADDR_MSK].is_used = 1U;

	hradio->alt_setting = 0U;

	hradio->mute = 0U;
	hradio->volume = 0x31U;
	hradio->samplingFrequency = 0U;

	/* Initialize the Audio Hardware layer */
	if (((USBD_RADIO_ItfTypeDef*) pdev->pUserData[pdev->classId])->Init(USBD_AUDIO_FREQ, AUDIO_DEFAULT_VOLUME, 0U) != USBD_OK) {
		return (uint8_t) USBD_FAIL;
	}

	return (uint8_t) USBD_OK;
}

/**
 * @brief  USBD_RADIO_DeInit
 *         DeInitialize the AUDIO layer
 * @param  pdev: device instance
 * @param  cfgidx: Configuration index
 * @retval status
 */
static uint8_t USBD_RADIO_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
	UNUSED(cfgidx);

	/* Close EP IN */
	(void) USBD_LL_CloseEP(pdev, AUDIO_IN_EP);

	pdev->ep_in[AUDIO_IN_EP & EP_ADDR_MSK].is_used = 0U;
	pdev->ep_in[AUDIO_IN_EP & EP_ADDR_MSK].bInterval = 0U;

	/* DeInit  physical Interface components */
	if (pdev->pClassDataCmsit[pdev->classId] != NULL) {
		((USBD_RADIO_ItfTypeDef*) pdev->pUserData[pdev->classId])->DeInit(0U);

		(void) USBD_free(pdev->pClassDataCmsit[pdev->classId]);

		pdev->pClassDataCmsit[pdev->classId] = NULL;
		pdev->pClassData = NULL;
	}

	return (uint8_t) USBD_OK;
}

/**
 * @brief  USBD_RADIO_Setup
 *         Handle the AUDIO specific requests
 * @param  pdev: instance
 * @param  req: usb requests
 * @retval status
 */
static uint8_t USBD_RADIO_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
	USBD_RADIO_HandleTypeDef *hradio;
	uint16_t len;
	uint8_t *pbuf;
	uint16_t status_info = 0U;
	USBD_StatusTypeDef ret = USBD_OK;

	hradio = (USBD_RADIO_HandleTypeDef*) pdev->pClassDataCmsit[pdev->classId];

	if (hradio == NULL) {
		return (uint8_t) USBD_FAIL;
	}

	switch (req->bmRequest & USB_REQ_TYPE_MASK)
	{
	case USB_REQ_TYPE_CLASS:
		switch (req->bmRequest & USB_REQ_TYPE_TARGET_MASK)
		{
			case AUDIO_REQ_ENTITY_OR_IF:
				switch (req->bRequest)
				{
					case AUDIO_REQ_GET_CUR:
						switch(HIBYTE(req->wValue))
						{
							case AUDIO_CS_MUTE:
								if (req->wLength != 1) {
									// Only the first form is supported
									USBD_CtlError(pdev, req);
									ret = USBD_FAIL;
									break;
								}

								/* If a reception is already in progress, stall */
								if (hradio->control.cmd != AUDIO_CS_NONE) {
									USBD_CtlError(pdev, req);
									ret = USBD_FAIL;
									break;
								}

								hradio->control.cmd = HIBYTE(req->wValue);
								hradio->control.len = MIN(req->wLength, USB_MAX_EP0_SIZE);
								hradio->control.unitId = 0U;
								hradio->control.ifOrEp = 0U;

								hradio->control.data[0] = hradio->mute;

								(void) USBD_CtlSendData(pdev, &hradio->control.data[0], hradio->control.len);

								break;

							case AUDIO_CS_VOLUME:
								if (req->wLength != 2) {
									// Only the first form is supported
									USBD_CtlError(pdev, req);
									ret = USBD_FAIL;
									break;
								}

								/* If a reception is already in progress, stall */
								if (hradio->control.cmd != AUDIO_CS_NONE) {
									USBD_CtlError(pdev, req);
									ret = USBD_FAIL;
									break;
								}

								hradio->control.cmd = HIBYTE(req->wValue);
								hradio->control.len = MIN(req->wLength, USB_MAX_EP0_SIZE);
								hradio->control.unitId = 0U;
								hradio->control.ifOrEp = 0U;

								int16_t usbVolume = map_chip_to_usb_volume(hradio->volume);

								hradio->control.data[0] = LOBYTE(usbVolume);
								hradio->control.data[1] = HIBYTE(usbVolume);

								(void) USBD_CtlSendData(pdev, &hradio->control.data[0], hradio->control.len);

								break;

							default:
								USBD_CtlError(pdev, req);
								ret = USBD_FAIL;
								break;
						}
						break;

					case AUDIO_REQ_GET_MIN:
						switch(HIBYTE(req->wValue))
						{
							case AUDIO_CS_VOLUME:
								if (req->wLength != 2) {
									// Only the first form is supported
									USBD_CtlError(pdev, req);
									ret = USBD_FAIL;
									break;
								}

								/* If a reception is already in progress, stall */
								if (hradio->control.cmd != AUDIO_CS_NONE) {
									USBD_CtlError(pdev, req);
									ret = USBD_FAIL;
									break;
								}

								hradio->control.cmd = HIBYTE(req->wValue);
								hradio->control.len = MIN(req->wLength, USB_MAX_EP0_SIZE);
								hradio->control.unitId = 0U;
								hradio->control.ifOrEp = 0U;

								int16_t usbVolume = map_chip_to_usb_volume(SI4705_VOLUME_MIN_SETTING);

								hradio->control.data[0] = LOBYTE(usbVolume);
								hradio->control.data[1] = HIBYTE(usbVolume);

								(void) USBD_CtlSendData(pdev, &hradio->control.data[0], hradio->control.len);

								break;

							default:
								USBD_CtlError(pdev, req);
								ret = USBD_FAIL;
								break;
						}
						break;

					case AUDIO_REQ_GET_MAX:
						switch(HIBYTE(req->wValue))
						{
							case AUDIO_CS_VOLUME:
								/* Volume */
								if (req->wLength != 2) {
									// Only the first form is supported
									USBD_CtlError(pdev, req);
									ret = USBD_FAIL;
									break;
								}

								/* If a reception is already in progress, stall */
								if (hradio->control.cmd != AUDIO_CS_NONE) {
									USBD_CtlError(pdev, req);
									ret = USBD_FAIL;
									break;
								}

								hradio->control.cmd = HIBYTE(req->wValue);
								hradio->control.len = MIN(req->wLength, USB_MAX_EP0_SIZE);
								hradio->control.unitId = 0U;
								hradio->control.ifOrEp = 0U;

								int16_t usbVolume = map_chip_to_usb_volume(SI4705_VOLUME_MAX_SETTING);

								hradio->control.data[0] = LOBYTE(usbVolume);
								hradio->control.data[1] = HIBYTE(usbVolume);

								(void) USBD_CtlSendData(pdev, &hradio->control.data[0], hradio->control.len);

								break;

							default:
								USBD_CtlError(pdev, req);
								ret = USBD_FAIL;
								break;
						}
						break;

					case AUDIO_REQ_GET_RES:
						switch(HIBYTE(req->wValue))
						{
							case AUDIO_CS_VOLUME:
								/* Volume */
								if (req->wLength != 2) {
									// Only the first form is supported
									USBD_CtlError(pdev, req);
									ret = USBD_FAIL;
									break;
								}

								/* If a reception is already in progress, stall */
								if (hradio->control.cmd != AUDIO_CS_NONE) {
									USBD_CtlError(pdev, req);
									ret = USBD_FAIL;
									break;
								}

								hradio->control.cmd = HIBYTE(req->wValue);
								hradio->control.len = MIN(req->wLength, USB_MAX_EP0_SIZE);
								hradio->control.unitId = 0U;
								hradio->control.ifOrEp = 0U;

								hradio->control.data[0] = LOBYTE(0x0100);
								hradio->control.data[1] = HIBYTE(0x0100);

								(void) USBD_CtlSendData(pdev, &hradio->control.data[0], hradio->control.len);

								break;

							default:
								USBD_CtlError(pdev, req);
								ret = USBD_FAIL;
								break;
						}
						break;

					case AUDIO_REQ_SET_CUR:
						/* If a reception is already in progress, stall */
						if (hradio->control.cmd != AUDIO_CS_NONE) {
							USBD_CtlError(pdev, req);
							ret = USBD_FAIL;
							break;
						}

						hradio->control.cmd = HIBYTE(req->wValue);
						hradio->control.len = MIN(req->wLength, USB_MAX_EP0_SIZE);
						hradio->control.unitId = HIBYTE(req->wIndex);
						hradio->control.ifOrEp = LOBYTE(req->wIndex);

						/* Receive data; the rest is processed in USBD_RADIO_EP0_RxReady */
						(void) USBD_CtlPrepareRx(pdev, &hradio->control.data[0], hradio->control.len);

						break;

					default:
						USBD_CtlError(pdev, req);
						ret = USBD_FAIL;
						break;
				}
				break;

			case AUDIO_REQ_ENDPOINT:
				switch (req->bRequest)
				{
					case AUDIO_REQ_GET_CUR:
						switch(HIBYTE(req->wValue))
						{
							case AUDIO_CS_SAMPLING_FREQ: {
								if (req->wLength != 3U) {
									// Only the first form is supported
									USBD_CtlError(pdev, req);
									ret = USBD_FAIL;
									break;
								}

								/* If a reception is already in progress, stall */
								if (hradio->control.cmd != AUDIO_CS_NONE) {
									USBD_CtlError(pdev, req);
									ret = USBD_FAIL;
									break;
								}

								hradio->control.cmd = HIBYTE(req->wValue);
								hradio->control.len = MIN(req->wLength, USB_MAX_EP0_SIZE);
								hradio->control.unitId = HIBYTE(req->wIndex);
								hradio->control.ifOrEp = LOBYTE(req->wIndex);

								hradio->control.data[0] = LOBYTE(hradio->samplingFrequency);
								hradio->control.data[1] = HIBYTE(hradio->samplingFrequency);
								hradio->control.data[2] = LOBYTE(hradio->samplingFrequency >> 16);

								(void) USBD_CtlSendData(pdev, &hradio->control.data[0], hradio->control.len);

								break;
							}

							default:
								USBD_CtlError(pdev, req);
								ret = USBD_FAIL;
								break;
						}
						break;

					case AUDIO_REQ_SET_CUR:
						if (req->wLength != 3U) {
							// Only the first form is supported
							USBD_CtlError(pdev, req);
							ret = USBD_FAIL;
							break;
						}

						/* If a reception is already in progress, stall */
						if (hradio->control.cmd != AUDIO_CS_NONE) {
							USBD_CtlError(pdev, req);
							ret = USBD_FAIL;
							break;
						}

						hradio->control.cmd = HIBYTE(req->wValue);
						hradio->control.len = MIN(req->wLength, USB_MAX_EP0_SIZE);
						hradio->control.unitId = HIBYTE(req->wIndex);
						hradio->control.ifOrEp = LOBYTE(req->wIndex);

						/* Receive data; the rest is processed in USBD_RADIO_EP0_RxReady */
						(void) USBD_CtlPrepareRx(pdev, &hradio->control.data[0], hradio->control.len);

						break;

					default:
						USBD_CtlError(pdev, req);
						ret = USBD_FAIL;
						break;
				}
				break;

			default:
				USBD_CtlError(pdev, req);
				ret = USBD_FAIL;
				break;
		}
		break;

	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest)
		{
			case USB_REQ_GET_STATUS:
				if (pdev->dev_state == USBD_STATE_CONFIGURED) {
					(void) USBD_CtlSendData(pdev, (uint8_t*) &status_info, 2U);
				} else {
					USBD_CtlError(pdev, req);
					ret = USBD_FAIL;
				}
				break;

			case USB_REQ_GET_DESCRIPTOR:
				if ((req->wValue >> 8) == AUDIO_DESCRIPTOR_TYPE) {
					pbuf = (uint8_t*) USBD_AUDIO_GetAudioHeaderDesc(pdev->pConfDesc);

					if (pbuf != NULL) {
						len = MIN(USB_AUDIO_DESC_SIZ, req->wLength);
						(void) USBD_CtlSendData(pdev, pbuf, len);
					} else {
						USBD_CtlError(pdev, req);
						ret = USBD_FAIL;
					}
				}
				break;

			case USB_REQ_GET_INTERFACE:
				if (pdev->dev_state == USBD_STATE_CONFIGURED) {
					(void) USBD_CtlSendData(pdev, (uint8_t*) &hradio->alt_setting, 1U);
				} else {
					USBD_CtlError(pdev, req);
					ret = USBD_FAIL;
				}
				break;

			case USB_REQ_SET_INTERFACE:
				if (pdev->dev_state == USBD_STATE_CONFIGURED) {
					if ((uint8_t) (req->wIndex) <= USBD_MAX_NUM_INTERFACES) {
						hradio->alt_setting = (uint8_t) (req->wValue);
					} else {
						/* Call the error management function (command will be NAKed */
						USBD_CtlError(pdev, req);
						ret = USBD_FAIL;
					}
				} else {
					USBD_CtlError(pdev, req);
					ret = USBD_FAIL;
				}
				break;

			case USB_REQ_CLEAR_FEATURE:
				break;

			default:
				USBD_CtlError(pdev, req);
				ret = USBD_FAIL;
				break;
		}
		break;

	default:
		USBD_CtlError(pdev, req);
		ret = USBD_FAIL;
		break;
	}

	return (uint8_t) ret;
}

#ifndef USE_USBD_COMPOSITE
/**
 * @brief  USBD_RADIO_GetCfgDesc
 *         return configuration descriptor
 * @param  length : pointer data length
 * @retval pointer to descriptor buffer
 */
static uint8_t* USBD_RADIO_GetCfgDesc(uint16_t *length)
{
	*length = (uint16_t) sizeof(USBD_RADIO_CfgDesc);

	return USBD_RADIO_CfgDesc;
}
#endif /* USE_USBD_COMPOSITE  */

/**
 * @brief  USBD_RADIO_DataIn
 *         handle data IN Stage
 * @param  pdev: device instance
 * @param  epnum: endpoint index
 * @retval status
 */
static uint8_t USBD_RADIO_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
	UNUSED(pdev);
	UNUSED(epnum);

	/* Not supported yet, but we would react here when an IN transfer over non-EP0 endpoint has completed (e.g. a HID report was sent, audio sync data was provided) */
	return (uint8_t) USBD_OK;
}

/**
 * @brief  USBD_RADIO_DataOut
 *         handle data OUT Stage
 * @param  pdev: device instance
 * @param  epnum: endpoint index
 * @retval status
 */
static uint8_t USBD_RADIO_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
	UNUSED(pdev);
	UNUSED(epnum);

	/* Not supported yet, but we would react here when an OUT transfer over a non-EP0 endpoint has completed (e.g. a HID report was received, or an audio transfer was completed */
	return (uint8_t) USBD_OK;
}

/**
 * @brief  USBD_RADIO_EP0_RxReady
 *         handle EP0 Rx Ready event
 * @param  pdev: device instance
 * @retval status
 */
static uint8_t USBD_RADIO_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
	USBD_RADIO_HandleTypeDef *hradio;
	hradio = (USBD_RADIO_HandleTypeDef*) pdev->pClassDataCmsit[pdev->classId];

	if (hradio == NULL) {
		return (uint8_t) USBD_FAIL;
	}

	if (hradio->control.unitId == AUDIO_FEATURE_UNIT_ID)
	{
		// Mute or volume
		switch(hradio->control.cmd)
		{
			case AUDIO_CS_MUTE:
				hradio->mute = hradio->control.data[0];
				break;

			case AUDIO_CS_VOLUME: {
				int16_t newSetting = ((int16_t)hradio->control.data[0]) | (((int16_t)hradio->control.data[1]) << 8);
				hradio->volume = map_usb_to_chip_volume(newSetting);
				break;
			}

			default:
				break;
		}
	}
	else
	{
		// Sampling frequency (or pitch, if it was supported)
		switch(hradio->control.cmd)
		{
			case AUDIO_CS_SAMPLING_FREQ:
				hradio->samplingFrequency = ((uint32_t)hradio->control.data[0]) | (((uint32_t)hradio->control.data[1]) << 8) | (((uint32_t)hradio->control.data[2]) << 16);
				break;

			default:
				break;
		}
	}

	/* Handling of the response is complete; ready the cache for next operation */
	hradio->control.cmd = AUDIO_CS_NONE;
	(void) USBD_memset(hradio->control.data, 0, USB_MAX_EP0_SIZE);
	hradio->control.len = 0U;
	hradio->control.ifOrEp = 0U;
	hradio->control.unitId = 0U;

	return (uint8_t) USBD_OK;
}

/**
 * @brief  USBD_RADIO_EP0_TxReady
 *         handle EP0 Tx Ready event
 * @param  pdev: device instance
 * @retval status
 */
static uint8_t USBD_RADIO_EP0_TxReady(USBD_HandleTypeDef *pdev)
{
	USBD_RADIO_HandleTypeDef *hradio;
	hradio = (USBD_RADIO_HandleTypeDef*) pdev->pClassDataCmsit[pdev->classId];

	if (hradio == NULL) {
		return (uint8_t) USBD_FAIL;
	}

	/* Sending of the response is complete; ready the cache for next operation */
	hradio->control.cmd = AUDIO_CS_NONE;
	(void) USBD_memset(hradio->control.data, 0, USB_MAX_EP0_SIZE);
	hradio->control.len = 0U;
	hradio->control.ifOrEp = 0U;
	hradio->control.unitId = 0U;

	return (uint8_t) USBD_OK;
}

/**
 * @brief  USBD_RADIO_SOF
 *         handle SOF event
 * @param  pdev: device instance
 * @retval status
 */
static uint8_t USBD_RADIO_SOF(USBD_HandleTypeDef *pdev)
{
	UNUSED(pdev);

	/* Not supported yet */
	return (uint8_t) USBD_OK;
}

/**
 * @brief  USBD_RADIO_Sync
 *         Sends either the first or the second half of the DMA buffer to the host, depending on which one is ready
 * @param  pdev: device instance
 * @param  offset: audio offset
 * @retval status
 */
void USBD_RADIO_Sync(USBD_HandleTypeDef *pdev, AUDIO_OffsetTypeDef offset)
{
	USBD_RADIO_HandleTypeDef *hradio;
	hradio = (USBD_RADIO_HandleTypeDef*) pdev->pClassDataCmsit[pdev->classId];

	if (hradio == NULL) {
		return;
	}

	uint8_t* pdata = NULL;

	if (offset == AUDIO_OFFSET_HALF) {
		pdata = &hradio->byteBuffer[0];  // First half is ready
	} else if (offset == AUDIO_OFFSET_FULL) {
		pdata = &hradio->byteBuffer[DMA_TOTAL_BUF_SIZE / 2];  // Second half is ready
	}

	// Send it
	USBD_LL_Transmit(pdev, AUDIO_IN_EP, pdata, (uint32_t)USB_TOTAL_BUF_SIZE);
}

/**
 * @brief  USBD_RADIO_IsoINIncomplete
 *         handle data ISO IN Incomplete event
 * @param  pdev: device instance
 * @param  epnum: endpoint index
 * @retval status
 */
static uint8_t USBD_RADIO_IsoINIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
	UNUSED(pdev);
	UNUSED(epnum);

	return (uint8_t) USBD_OK;
}
/**
 * @brief  USBD_RADIO_IsoOutIncomplete
 *         handle data ISO OUT Incomplete event
 * @param  pdev: device instance
 * @param  epnum: endpoint index
 * @retval status
 */
static uint8_t USBD_RADIO_IsoOutIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
	UNUSED(pdev);
	UNUSED(epnum);

	return (uint8_t) USBD_OK;
}

/**
 * @brief  USBD_RADIO_RegisterInterface
 * @param  pdev: device instance
 * @param  fops: Audio interface callback
 * @retval status
 */
uint8_t USBD_RADIO_RegisterInterface(USBD_HandleTypeDef *pdev, USBD_RADIO_ItfTypeDef *fops)
{
	if (fops == NULL) {
		return (uint8_t) USBD_FAIL;
	}

	pdev->pUserData[pdev->classId] = fops;

	return (uint8_t) USBD_OK;
}

#ifdef USE_USBD_COMPOSITE
/**
  * @brief  USBD_AUDIO_GetEpPcktSze
  * @param  pdev: device instance (reserved for future use)
  * @param  If: Interface number (reserved for future use)
  * @param  Ep: Endpoint number (reserved for future use)
  * @retval status
  */
uint32_t USBD_AUDIO_GetEpPcktSze(USBD_HandleTypeDef *pdev, uint8_t If, uint8_t Ep)
{
  uint32_t mps;

  UNUSED(pdev);
  UNUSED(If);
  UNUSED(Ep);

  mps = AUDIO_PACKET_SZE_WORD(USBD_AUDIO_FREQ);

  /* Return the wMaxPacketSize value in Bytes (Freq(Samples)*2(Stereo)*2(HalfWord)) */
  return mps;
}
#endif /* USE_USBD_COMPOSITE */

/**
 * @brief  USBD_AUDIO_GetAudioHeaderDesc
 *         This function return the Audio descriptor
 * @param  pdev: device instance
 * @param  pConfDesc:  pointer to Bos descriptor
 * @retval pointer to the Audio AC Header descriptor
 */
static void* USBD_AUDIO_GetAudioHeaderDesc(uint8_t *pConfDesc)
{
	USBD_ConfigDescTypeDef *desc = (USBD_ConfigDescTypeDef*) (void*) pConfDesc;
	USBD_DescHeaderTypeDef *pdesc = (USBD_DescHeaderTypeDef*) (void*) pConfDesc;
	uint8_t *pAudioDesc = NULL;
	uint16_t ptr;

	if (desc->wTotalLength > desc->bLength) {
		ptr = desc->bLength;

		while (ptr < desc->wTotalLength) {
			pdesc = USBD_GetNextDesc((uint8_t*) pdesc, &ptr);
			if ((pdesc->bDescriptorType == AUDIO_DESC_TYPE_CS_INTERFACE)
					&& (pdesc->bDescriptorSubType == AUDIO_DESC_SUBTYPE_HEADER)) {
				pAudioDesc = (uint8_t*) pdesc;
				break;
			}
		}
	}

	return pAudioDesc;
}

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

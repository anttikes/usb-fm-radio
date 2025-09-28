/**
 ******************************************************************************
 * @file    usbd_radio.h
 * @author  Antti Keskinen
 * @brief   header file for the usbd_radio.c file.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) Antti Keskinen.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_RADIO_H
#define __USB_RADIO_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbd_ioreq.h"

#ifndef USBD_AUDIO_FREQ
#define USBD_AUDIO_FREQ                               48000U
#endif /* USBD_AUDIO_FREQ */

#ifndef AUDIO_REAL_FREQ
#define AUDIO_REAL_FREQ                               48000U
#endif /* AUDIO_REAL_FREQ */

#ifndef USBD_MAX_NUM_INTERFACES
#define USBD_MAX_NUM_INTERFACES                       1U
#endif /* USBD_AUDIO_FREQ */

#ifndef AUDIO_BITDEPTH
#define AUDIO_BITDEPTH								  16U
#endif

#ifndef AUDIO_CHANNELS
#define AUDIO_CHANNELS							      2U
#endif

/*
 * AUDIO_IN_BYTES_PER_SECOND represents the number of bytes per second that the desired
 * sampling frequency, sample bit depth and channel count produce
 *
 * AUDIO_IN_PACKET represents the amount of data transferred in a single USB 2.0 Full-speed frame (one frame per ms)
 * If High-speed was used here, the calculation would be different
 */
#define AUDIO_BYTES_PER_SECOND				          ((uint32_t)((USBD_AUDIO_FREQ * AUDIO_BITDEPTH * AUDIO_CHANNELS) / 8U))

#define AUDIO_BYTES_PER_FRAME                         ((uint16_t)(AUDIO_BYTES_PER_SECOND / 1000U))
//#define AUDIO_IN_BYTES_PER_FRAME                      (uint16_t)(((AUDIO_IN_BYTES_PER_SECOND + 999U) / 1000U + 31U) & ~31U)

/* Number of sub-packets in the DMA transfer buffer */
#define DMA_IN_PACKET_NUM                             4U

/* Number of packets in one PMA buffer */
#define USB_IN_PACKET_NUM							  2U

/* Total size of the DMA transfer buffer, in bytes */
#define DMA_TOTAL_BUF_SIZE                            ((uint16_t)(AUDIO_BYTES_PER_FRAME * DMA_IN_PACKET_NUM))

/* Total size of individual PMA buffer, in bytes */
#define USB_TOTAL_BUF_SIZE                            ((uint16_t)(AUDIO_BYTES_PER_FRAME * USB_IN_PACKET_NUM))

#ifndef AUDIO_HS_BINTERVAL
#define AUDIO_HS_BINTERVAL                            0x01U
#endif /* AUDIO_HS_BINTERVAL */

#ifndef AUDIO_FS_BINTERVAL
#define AUDIO_FS_BINTERVAL                            0x01U
#endif /* AUDIO_FS_BINTERVAL */

#define AUDIO_IN_EP                                   (0x01U | IN)

#define AUDIO_IF_DESC_SIZE                            0x09U
#define USB_AUDIO_DESC_SIZ                            0x09U
#define USB_AS_ISOC_EP_DESC_SIZE                      0x09U
#define AUDIO_AS_ISOC_EP_DESC_SIZE					  0x07U

#define AUDIO_DESCRIPTOR_TYPE                         0x21U
#define USB_DEVICE_CLASS_AUDIO                        0x01U
#define AUDIO_SUBCLASS_AUDIOCONTROL                   0x01U
#define AUDIO_SUBCLASS_AUDIOSTREAMING                 0x02U
#define AUDIO_PROTOCOL_UNDEFINED                      0x00U

/* Audio Descriptor Types */
#define AUDIO_DESC_TYPE_CS_INTERFACE                  0x24U
#define AUDIO_DESC_TYPE_CS_ENDPOINT                   0x25U

/* Audio Control Interface Descriptor Subtypes */
#define AUDIO_DESC_SUBTYPE_HEADER                     0x01U
#define AUDIO_DESC_SUBTYPE_INPUT_TERMINAL             0x02U
#define AUDIO_DESC_SUBTYPE_OUTPUT_TERMINAL            0x03U
#define AUDIO_DESC_SUBTYPE_FEATURE_UNIT               0x06U
#define AUDIO_DESC_SUBTYPE_AS_GENERAL                 0x01U
#define AUDIO_DESC_SUBTYPE_FORMAT_TYPE                0x02U
#define AUDIO_DESC_SUBTYPE_CLOCK_SOURCE               0x0AU

/* Audio Endpoint Descriptor Subtypes */
#define AUDIO_DESC_SUBTYPE_UNDEFINED                  0x00U
#define AUDIO_DESC_SUBTYPE_EP_GENERAL                 0x01U

#define AUDIO_INPUT_TERMINAL_DESC_SIZE                0x0CU
#define AUDIO_OUTPUT_TERMINAL_DESC_SIZE               0x09U
#define AUDIO_STREAMING_INTERFACE_DESC_SIZE           0x07U

#define AUDIO_CONTROL_NONE                            0x000U
#define AUDIO_CONTROL_MUTE                            0x001U
#define AUDIO_CONTROL_VOLUME                          0x002U
#define AUDIO_CONTROL_BASS                            0x004U
#define AUDIO_CONTROL_MID                             0x008U
#define AUDIO_CONTROL_TREBLE                          0x010U
#define AUDIO_CONTROL_EQUALIZER                       0x020U
#define AUDIO_CONTROL_AGC                             0x040U
#define AUDIO_CONTROL_DELAY                           0x080U
#define AUDIO_CONTROL_BOOST                           0x100U
#define AUDIO_CONTROL_LOUDNESS                        0x200U

#define AUDIO_FORMAT_TYPE_I_PCM                       0x0001U

#define AUDIO_FORMAT_TYPE_I                           0x01U
#define AUDIO_FORMAT_TYPE_III                         0x03U

#define AUDIO_EP_ATTRIB_ISOC						  0x01U
#define AUDIO_EP_ATTRIB_ASYNC						  0x04U
#define AUDIO_EP_ATTRIB_ADAPTIVE					  0x08U
#define AUDIO_EP_ATTRIB_SYNC						  0x0CU

#define AUDIO_CS_EP_ATTRIB_NONE						  0x00U
#define AUDIO_CS_EP_ATTRIB_SAMPLING					  0x01U
#define AUDIO_CS_EP_ATTRIB_PITCH					  0x02U
#define AUDIO_CS_EP_ATTRIB_MAXPACKET				  0x80U

#define AUDIO_ENDPOINT_GENERAL                        0x01U

#define USB_REQ_TYPE_TARGET_MASK                      0x03U

#define AUDIO_REQ_ENTITY_OR_IF						  0x01U
#define AUDIO_REQ_ENDPOINT							  0x02U

#define AUDIO_CS_NONE								  0x00U
#define AUDIO_CS_MUTE                            	  0x01U
#define AUDIO_CS_VOLUME                         	  0x02U
#define AUDIO_CS_BASS                           	  0x03U
#define AUDIO_CS_MID                              	  0x04U
#define AUDIO_CS_TREBLE                         	  0x05U
#define AUDIO_CS_EQUALIZER                      	  0x06U
#define AUDIO_CS_AGC                             	  0x07U
#define AUDIO_CS_DELAY                          	  0x08U
#define AUDIO_CS_BOOST                         	 	  0x09U
#define AUDIO_CS_LOUDNESS                       	  0x0AU

#define AUDIO_CS_SAMPLING_FREQ						  0x01U
#define AUDIO_CS_PITCH								  0x02U

#define AUDIO_REQ_GET_CUR                             0x81U
#define AUDIO_REQ_GET_MIN                             0x82U
#define AUDIO_REQ_GET_MAX                             0x83U
#define AUDIO_REQ_GET_RES                             0x84U
#define AUDIO_REQ_GET_MEM                             0x85U

#define AUDIO_REQ_SET_CUR                             0x01U
#define AUDIO_REQ_SET_MIN                             0x02U
#define AUDIO_REQ_SET_MAX                             0x03U
#define AUDIO_REQ_SET_RES                             0x04U
#define AUDIO_REQ_SET_MEM                             0x05U

// The unit IDs which have commands or controls are recommended to start from 0xEF downwards
// This ensures that they are easy to distinguish from potential requests directed to interfaces or endpoints
#define AUDIO_FEATURE_UNIT_ID                         0x02U

#define AUDIO_MAX_VOLUME							  63U
#define AUDIO_MIN_VOLUME							   0U

#define AUDIO_DEFAULT_VOLUME                          70U

/* Audio Commands enumeration */
typedef enum
{
	AUDIO_CMD_START = 1, AUDIO_CMD_PLAY, AUDIO_CMD_STOP,
} AUDIO_CMD_TypeDef;

typedef enum
{
	AUDIO_OFFSET_NONE = 0,
	AUDIO_OFFSET_HALF,
	AUDIO_OFFSET_FULL
} AUDIO_OffsetTypeDef;
/**
 * @}
 */

/** @defgroup USBD_CORE_Exported_TypesDefinitions
 * @{
 */
typedef struct
{
	/* Holds the identifier of the command whose data we are receiving */
	uint8_t cmd;

	/* If the command is targeting an interface or an endpoint, holds the address */
	uint8_t ifOrEp;

	/* If the command is targeting a unit, holds the unit ID */
	uint8_t unitId;

	/* Holds the data bytes associated with the command */
	uint8_t data[USB_MAX_EP0_SIZE];

	/* Holds the number of data bytes */
	uint8_t len;
} USBD_RADIO_ControlTypeDef;

typedef struct
{
	/* Indicates which one of the streaming interfaces is currently active */
	uint32_t alt_setting;

	/*
	 * The buffer where DMA copies data, and from which we move it to the low-level PMA buffer
	 * Union type is used to avoid casting the pointer type. Alignment is specified to ensure it works
	 * correctly with the DMA.
	 */
	__ALIGN_BEGIN union {
		uint8_t byteBuffer[DMA_TOTAL_BUF_SIZE];
		uint16_t halfWordBuffer[DMA_TOTAL_BUF_SIZE / 2];
	} __ALIGN_END;

	/* Holds the current mute status */
	uint8_t mute;

	/* Holds the current volume setting */
	uint8_t volume;

	/* Holds the current sampling frequency setting */
	uint32_t samplingFrequency;

	/* Maintains details of a command transmission that's currently in progress */
	USBD_RADIO_ControlTypeDef control;
} USBD_RADIO_HandleTypeDef;

typedef struct
{
	int8_t (*Init)(uint32_t AudioFreq, uint32_t Volume, uint32_t options);
	int8_t (*DeInit)(uint32_t options);
	int8_t (*AudioCmd)(uint8_t *pbuf, uint32_t size, uint8_t cmd);
	int8_t (*VolumeCtl)(uint8_t vol);
	int8_t (*MuteCtl)(uint8_t cmd);
	int8_t (*PeriodicTC)(uint8_t *pbuf, uint32_t size, uint8_t cmd);
	int8_t (*GetState)(void);
} USBD_RADIO_ItfTypeDef;

/*
 * Audio Class specification release 1.0
 */

/* Table 4-2: Class-Specific AC Interface Header Descriptor */
typedef struct
{
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint16_t bcdADC;
	uint16_t wTotalLength;
	uint8_t bInCollection;
	uint8_t baInterfaceNr;
} __PACKED USBD_SpeakerIfDescTypeDef;

/* Table 4-3: Input Terminal Descriptor */
typedef struct
{
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint8_t bTerminalID;
	uint16_t wTerminalType;
	uint8_t bAssocTerminal;
	uint8_t bNrChannels;
	uint16_t wChannelConfig;
	uint8_t iChannelNames;
	uint8_t iTerminal;
} __PACKED USBD_SpeakerInDescTypeDef;

/* USB Speaker Audio Feature Unit Descriptor */
typedef struct
{
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint8_t bUnitID;
	uint8_t bSourceID;
	uint8_t bControlSize;
	uint16_t bmaControls;
	uint8_t iTerminal;
} __PACKED USBD_SpeakerFeatureDescTypeDef;

/* Table 4-4: Output Terminal Descriptor */
typedef struct
{
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint8_t bTerminalID;
	uint16_t wTerminalType;
	uint8_t bAssocTerminal;
	uint8_t bSourceID;
	uint8_t iTerminal;
} __PACKED USBD_SpeakerOutDescTypeDef;

/* Table 4-19: Class-Specific AS Interface Descriptor */
typedef struct
{
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint8_t bTerminalLink;
	uint8_t bDelay;
	uint16_t wFormatTag;
} __PACKED USBD_SpeakerStreamIfDescTypeDef;

/* USB Speaker Audio Type III Format Interface Descriptor */
typedef struct
{
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint8_t bFormatType;
	uint8_t bNrChannels;
	uint8_t bSubFrameSize;
	uint8_t bBitResolution;
	uint8_t bSamFreqType;
	uint8_t tSamFreq2;
	uint8_t tSamFreq1;
	uint8_t tSamFreq0;
} USBD_SpeakerIIIFormatIfDescTypeDef;

/* Table 4-17: Standard AC Interrupt Endpoint Descriptor */
typedef struct
{
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bEndpointAddress;
	uint8_t bmAttributes;
	uint16_t wMaxPacketSize;
	uint8_t bInterval;
	uint8_t bRefresh;
	uint8_t bSynchAddress;
} __PACKED USBD_SpeakerEndDescTypeDef;

/* Table 4-21: Class-Specific AS Isochronous Audio Data Endpoint Descriptor        */
typedef struct
{
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptor;
	uint8_t bmAttributes;
	uint8_t bLockDelayUnits;
	uint16_t wLockDelay;
} __PACKED USBD_SpeakerEndStDescTypeDef;

/**
 * @}
 */

/** @defgroup USBD_CORE_Exported_Macros
 * @{
 */

/**
 * @}
 */

/** @defgroup USBD_CORE_Exported_Variables
 * @{
 */

extern USBD_ClassTypeDef USBD_RADIO;

/**
 * @}
 */

/** @defgroup USB_CORE_Exported_Functions
 * @{
 */
uint8_t USBD_RADIO_RegisterInterface(USBD_HandleTypeDef *pdev,
		USBD_RADIO_ItfTypeDef *fops);

void USBD_RADIO_Sync(USBD_HandleTypeDef *pdev, AUDIO_OffsetTypeDef offset);

#ifdef USE_USBD_COMPOSITE
uint32_t USBD_AUDIO_GetEpPcktSze(USBD_HandleTypeDef *pdev, uint8_t If, uint8_t Ep);
#endif /* USE_USBD_COMPOSITE */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif  /* __USB_RADIO_H */
/**
 * @}
 */

/**
 * @}
 */

/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : usb_descriptors.c
 * @brief          : Provides the USB descriptors for TinyUSB
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

#include "stm32f042x6.h"
#include "tusb.h"
#include "usb_descriptors.h"

//--------------------------------------------------------------------+
// Board support
//--------------------------------------------------------------------+

/**
 * @brief  Extracts the unique identifier of the STM32 chip from the registers
 * @param  id Pointer to the buffer which should be filled with the identifier
 * @param  maxChars Maximum number of ASCII characters that can fit into the buffer
 *
 * @retval Length of the unique identifier, in ASCII characters
 */
size_t board_get_unique_id(uint8_t id[], size_t maxChars) {
	(void) maxChars;

	volatile uint32_t* stm32_uuid = (volatile uint32_t*) UID_BASE;
	uint32_t* id32 = (uint32_t*) (uintptr_t) id;
	uint8_t const len = 12;

	id32[0] = stm32_uuid[0];
	id32[1] = stm32_uuid[1];
	id32[2] = stm32_uuid[2];

	return len;
}

/**
 * @brief  Helper method to get the unique identifier of the STM32 chip as a Unicode string
 * @param  desc_str1 Pointer to the second position in the buffer which should be filled with the identifier
 * @param  maxChars Maximum amount of ASCII characters that can fit into the buffer
 *
 * @retval Length of the unique identifier, in Unicode characters
 */
static inline size_t board_usb_get_serial(uint16_t desc_str1[], size_t maxChars) {
	uint8_t uid[16] TU_ATTR_ALIGNED(4);
	size_t uid_len;

	uid_len = board_get_unique_id(uid, sizeof(uid));

	if ( uid_len > maxChars / 2 ) uid_len = maxChars / 2;

	for ( size_t i = 0; i < uid_len; i++ ) {
		for ( size_t j = 0; j < 2; j++ ) {
			const char nibble_to_hex[16] = {
			  '0', '1', '2', '3', '4', '5', '6', '7',
			  '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
			};

			uint8_t const nibble = (uid[i] >> (j * 4)) & 0xf;
			desc_str1[i * 2 + (1 - j)] = nibble_to_hex[nibble]; // UTF-16-LE
		}
	}

	return 2 * uid_len;
}

//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+

// String Descriptor indices
enum {
	STRID_LANGID = 0,
	STRID_MANUFACTURER,
	STRID_PRODUCT,
	STRID_SERIAL,
};

// The string descriptors themselves
char const* string_desc_arr [] = {
    (const char[]) { 0x09, 0x04 }, // 0: is supported language is English (0x0409)
    "AK Design",                   // 1: Manufacturer
    "USB FM Radio",                // 2: Product
    NULL,                          // 3: Serials will use unique ID if possible
    "UAC2",                        // 4: Audio Interface
};

// Buffer to hold the Unicode representation of the string
static uint16_t _desc_str[32 + 1];

/**
 * @brief  Invoked when GET STRING DESCRIPTOR request is received
 * @param  index Index of the string descriptor to return
 * @param  langid Language identifier in which to return it
 *
 * @retval Pointer to the Unicode string descriptor buffer
 */
uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
	(void) langid;
	size_t chr_count;

	switch ( index ) {
		case STRID_LANGID:
			memcpy(&_desc_str[1], string_desc_arr[0], 2);
			chr_count = 1;
			break;

		case STRID_SERIAL:
			chr_count = board_usb_get_serial(_desc_str + 1, 32);
			break;

		default:
			// Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
			// https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors

			if ( !(index < sizeof(string_desc_arr) / sizeof(string_desc_arr[0])) ) return NULL;

			const char *str = string_desc_arr[index];

			// Cap at max char
			chr_count = strlen(str);
			size_t const max_count = sizeof(_desc_str) / sizeof(_desc_str[0]) - 1; // -1 for string type
			if ( chr_count > max_count ) chr_count = max_count;

			// Convert ASCII string into UTF-16
			for ( size_t i = 0; i < chr_count; i++ ) {
				_desc_str[1 + i] = str[i];
			}
			break;
	}

	// First byte is length of the descriptor (including type and length), second byte is string type
	_desc_str[0] = (uint16_t) ((TUSB_DESC_STRING << 8) | (2 * chr_count + 2));

	return _desc_str;
}

//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+
tusb_desc_device_t const desc_device =
{
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,

    // Use Interface Association Descriptor (IAD) for USB Audio 2.0
    .bDeviceClass       = TUSB_CLASS_MISC,
    .bDeviceSubClass    = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol    = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor           = 0x0483,
    .idProduct          = 0x5740,
    .bcdDevice          = 0x0002,

    .iManufacturer      = STRID_MANUFACTURER,
    .iProduct           = STRID_PRODUCT,
    .iSerialNumber      = STRID_SERIAL,

    .bNumConfigurations = 0x01
};

/**
 * @brief  Invoked when GET DEVICE DESCRIPTOR is received
 *
 * @retval Pointer to the device descriptor structure
 */
uint8_t const * tud_descriptor_device_cb(void)
{
	return (uint8_t const *) &desc_device;
}

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+
enum
{
	ITF_NUM_AUDIO_CONTROL = 0,
	ITF_NUM_AUDIO_STREAMING,
	ITF_NUM_AUDIO_TOTAL
};

#define ITF_NUM_TOTAL          ITF_NUM_AUDIO_TOTAL

#define CONFIG_TOTAL_LEN       (TUD_CONFIG_DESC_LEN + CFG_TUD_AUDIO * TUD_AUDIO_RADIO_RECEIVER_TWO_CH_1_FORMAT_DESC_LEN)

uint8_t const desc_configuration[] =
{
	/* Standard Configuration Descriptor */
	TUD_CONFIG_DESCRIPTOR(
		1,                                                       /* bConfigurationValue */
		ITF_NUM_TOTAL,                                           /* bNumInterfaces */
		0x00,                                                    /* iConfiguration */
		CONFIG_TOTAL_LEN,                                        /* wTotalLength ?? */
		0x80,                                                    /* bmAttributes ?? */
		100                                                      /* bMaxPower (in mA) */
	),

	/* Standard Interface Association Descriptor (IAD) */
	TUD_AUDIO_DESC_IAD(
		ITF_NUM_AUDIO_CONTROL,                                   /* bFirstInterface */
		ITF_NUM_AUDIO_TOTAL,                                     /* bInterfaceCount */
		0x00                                                     /* iFunction */
	),

	/* Standard AC Interface Descriptor(4.7.1) */
	TUD_AUDIO_DESC_STD_AC(
		ITF_NUM_AUDIO_CONTROL,                                   /* bInterfaceNumber */
		0x00,                                                    /* bNumEndpoints */
		0x00                                                     /* iInterface */
	),

	/* Class-Specific AC Interface Header Descriptor(4.7.2) */
	TUD_AUDIO_DESC_CS_AC(
		0x0200,												     /* bcdADC */
		AUDIO_FUNC_MICROPHONE,                                   /* bCategory */
		TUD_AUDIO_DESC_CLK_SRC_LEN							     /* wTotalLength */
		+ TUD_AUDIO_DESC_INPUT_TERM_LEN
		+ TUD_AUDIO_DESC_FEATURE_UNIT_TWO_CHANNEL_LEN
		+ TUD_AUDIO_DESC_OUTPUT_TERM_LEN,
		AUDIO_CTRL_NONE                                          /* bmControls */
	),

	/* Clock Source Descriptor(4.7.2.1) */
	TUD_AUDIO_DESC_CLK_SRC(
		ENTITY_ID_CLOCK_SOURCE,                                  /* bClockId */
		AUDIO_CLOCK_SOURCE_ATT_INT_FIX_CLK,                      /* bmAttributes */
		(AUDIO_CTRL_R << AUDIO_CLOCK_SOURCE_CTRL_CLK_FRQ_POS),   /* bmControls */
		ENTITY_ID_INPUT_TERMINAL,                                /* bAssocTerminal */
		0x00                                                     /* iClockSource */
	),

	/* Input Terminal Descriptor(4.7.2.4) */
	// Terminal id, terminal type, associated terminal, clock id, channel count, channel config, string index for channel names, control, string index
	TUD_AUDIO_DESC_INPUT_TERM(
		ENTITY_ID_INPUT_TERMINAL,                                /* bTerminalId */
		0x0710,                                                  /* wTerminalType */
		ENTITY_ID_OUTPUT_TERMINAL,                               /* bAssocTerminal */
		ENTITY_ID_CLOCK_SOURCE,                                  /* bCSourceID */
		0x02,                                                    /* bNrChannels */
		AUDIO_CHANNEL_CONFIG_FRONT_LEFT |                        /* bmChannelConfig */
		AUDIO_CHANNEL_CONFIG_FRONT_RIGHT,
		0x00,                                                    /* iChannelNames */
		AUDIO_CTRL_NONE,                                         /* bmControls */
		0x00                                                     /* iTerminal */
	),

	/* Feature Unit Descriptor(4.7.2.8) */
	TUD_AUDIO_DESC_FEATURE_UNIT_TWO_CHANNEL(
		ENTITY_ID_FEATURE_UNIT,                                  /* bUnitId */
		ENTITY_ID_INPUT_TERMINAL,                                /* bSourceId */
		AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_MUTE_POS |      /* bmaControls (master) */
		AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_VOLUME_POS,
		AUDIO_CTRL_NONE,                                         /* bmaControls (logical channel 1) */
		AUDIO_CTRL_NONE,                                         /* bmaControls (logical channel 2) */
		0x00                                                     /* iFeature */
	),

	/* Output Terminal Descriptor(4.7.2.5) */
	// Terminal id, terminal type, associated terminal, source terminal, clock id, controls, string index
	TUD_AUDIO_DESC_OUTPUT_TERM(
		ENTITY_ID_OUTPUT_TERMINAL,                               /* bTerminalId */
		AUDIO_TERM_TYPE_USB_STREAMING,                           /* wTerminalType */
		ENTITY_ID_INPUT_TERMINAL,                                /* bAssocTerminal */
		ENTITY_ID_FEATURE_UNIT,                                  /* bSourceId */
		ENTITY_ID_CLOCK_SOURCE,                                  /* bCSourceID */
		AUDIO_CTRL_NONE,                                         /* bmControls */
		0x00                                                     /* iTerminal */
	),

	/* Standard AS Interface Descriptor(4.9.1) */
	/* "Mute" interface */
	TUD_AUDIO_DESC_STD_AS_INT(
		ITF_NUM_AUDIO_STREAMING,                                 /* bInterfaceNumber */
		0x00,                                                    /* bAlternateSetting */
		0x00,                                                    /* bNumEndpoints */
		0x00                                                     /* iInterface */
	),

	/* Standard AS Interface Descriptor(4.9.1) */
	/* Real interface with endpoints */
	TUD_AUDIO_DESC_STD_AS_INT(
		ITF_NUM_AUDIO_STREAMING,                                 /* bInterfaceNumber */
		0x01,                                                    /* bAlternateSetting */
		0x01,                                                    /* bNumEndpoints */
		0x00                                                     /* iInterface */
	),

	/* Class-Specific AS Interface Descriptor(4.9.2) */
	TUD_AUDIO_DESC_CS_AS_INT(
		ENTITY_ID_OUTPUT_TERMINAL,                               /* bTerminalLink */
		AUDIO_CTRL_NONE,                                         /* bmControls */
		AUDIO_FORMAT_TYPE_I,                                     /* bFormatType */
		AUDIO_DATA_FORMAT_TYPE_I_PCM,                            /* bmFormats */
		0x02,                                                    /* bNrChannels */
		AUDIO_CHANNEL_CONFIG_FRONT_LEFT |                        /* bmChannelConfig */
		AUDIO_CHANNEL_CONFIG_FRONT_RIGHT,
		0x00                                                     /* iChannelNames */
	),

	/* Type I Format Type Descriptor(2.3.1.6 - Audio Formats) */
	TUD_AUDIO_DESC_TYPE_I_FORMAT(
		CFG_TUD_AUDIO_FUNC_1_FORMAT_1_N_BYTES_PER_SAMPLE_TX,     /* bSubslotSize */
		CFG_TUD_AUDIO_FUNC_1_SAMPLE_BIT_RESOLUTION               /* bBitResolution */
	),

	/* Standard AS Isochronous Audio Data Endpoint Descriptor(4.10.1.1) */
	TUD_AUDIO_DESC_STD_AS_ISO_EP(
		0x80 | EPNUM_AUDIO,                                      /* bEndpointAddress */
		TUSB_XFER_ISOCHRONOUS |                                  /* bmAttributes */
		TUSB_ISO_EP_ATT_ASYNCHRONOUS |
		TUSB_ISO_EP_ATT_DATA,
		CFG_TUD_AUDIO_FUNC_1_EP_IN_SZ_MAX,                       /* wMaxPacketSize */
		0x01                                                     /* bInterval */
	),

	/* Class-Specific AS Isochronous Audio Data Endpoint Descriptor(4.10.1.2) */
	TUD_AUDIO_DESC_CS_AS_ISO_EP(
		AUDIO_CS_AS_ISO_DATA_EP_ATT_NON_MAX_PACKETS_OK,          /* bmAttributes */
		AUDIO_CTRL_NONE,                                         /* bmControls */
		AUDIO_CS_AS_ISO_DATA_EP_LOCK_DELAY_UNIT_UNDEFINED,       /* bLockDelayUnits */
		0x0000                                                   /* wLockDelay */
	)
};

/**
 * @brief  Invoked when GET CONFIGURATION DESCRIPTOR is received
 *
 * @retval Pointer to the configuration descriptor structure
 */
uint8_t const * tud_descriptor_configuration_cb(uint8_t index)
{
	(void) index; // for multiple configurations

	return desc_configuration;
}

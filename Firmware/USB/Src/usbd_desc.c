/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file     usbd_desc.c
 * @brief    This file provides the USBD descriptors and string formatting method.
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_conf.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define USBD_VID                      0x0483
#define USBD_PID                      0x5740
#define USBD_LANGID_STRING            0x409   /* EN-US */

/* Private macro -------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
uint8_t* USBD_Class_DeviceDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t* USBD_Class_LangIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t* USBD_Class_ManufacturerStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t* USBD_Class_ProductStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t* USBD_Class_SerialStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);

/* Private variables ---------------------------------------------------------*/
USBD_DescriptorsTypeDef FS_Desc =
{
	USBD_Class_DeviceDescriptor,
	USBD_Class_LangIDStrDescriptor,
	USBD_Class_ManufacturerStrDescriptor,
	USBD_Class_ProductStrDescriptor,
	USBD_Class_SerialStrDescriptor,
	// To conserve memory we do not return configuration or interface
	// string descriptors
	NULL,
	NULL
};

__ALIGN_BEGIN uint8_t USBD_DeviceDesc[] __ALIGN_END =
{
	0x12, 						/* bLength */
	USB_DESC_TYPE_DEVICE, 		/* bDescriptorType */
	LOBYTE(0x0200), 			/* bcdUSB: 2.0 */
	HIBYTE(0x0200),
	0x00, 						/* bDeviceClass */
	0x00, 						/* bDeviceSubClass */
	0x00, 						/* bDeviceProtocol */
	USB_MAX_EP0_SIZE, 			/* bMaxPacketSize */
	LOBYTE(USBD_VID), 			/* idVendor */
	HIBYTE(USBD_VID),
	LOBYTE(USBD_PID), 			/* idProduct */
	HIBYTE(USBD_PID),
	LOBYTE(0x0002), 			/* bcdDevice: v0.2 */
	HIBYTE(0x0002),
	USBD_IDX_MFC_STR, 			/* iManufacturer */
	USBD_IDX_PRODUCT_STR, 		/* iProduct */
	USBD_IDX_SERIAL_STR, 		/* iSerial */
	USBD_MAX_NUM_CONFIGURATION 	/* bNumConfigurations */
};

__ALIGN_BEGIN uint8_t USBD_LangIDDesc[] __ALIGN_END =
{
	USB_LEN_LANGID_STR_DESC,
	USB_DESC_TYPE_STRING,
	LOBYTE(USBD_LANGID_STRING),
	HIBYTE(USBD_LANGID_STRING),
};

__ALIGN_BEGIN uint8_t USDB_ManufacturerStringDesc[] __ALIGN_END =
{
	0x14, 					/* bLength */
	USB_DESC_TYPE_STRING, 	/* bDescriptorType (STRING) */
	USBD_WIDE_CHAR(L'A'),
	USBD_WIDE_CHAR(L'K'),
	USBD_WIDE_CHAR(L' '),
	USBD_WIDE_CHAR(L'D'),
	USBD_WIDE_CHAR(L'e'),
	USBD_WIDE_CHAR(L's'),
	USBD_WIDE_CHAR(L'i'),
	USBD_WIDE_CHAR(L'g'),
	USBD_WIDE_CHAR(L'n'),
};

__ALIGN_BEGIN uint8_t USDB_ProductStringDesc[] __ALIGN_END =
{
	0x1A, 					/* bLength */
	USB_DESC_TYPE_STRING, 	/* bDescriptorType (STRING) */
	USBD_WIDE_CHAR(L'U'),
	USBD_WIDE_CHAR(L'S'),
	USBD_WIDE_CHAR(L'B'),
	USBD_WIDE_CHAR(L' '),
	USBD_WIDE_CHAR(L'F'),
	USBD_WIDE_CHAR(L'M'),
	USBD_WIDE_CHAR(L' '),
	USBD_WIDE_CHAR(L'R'),
	USBD_WIDE_CHAR(L'a'),
	USBD_WIDE_CHAR(L'd'),
	USBD_WIDE_CHAR(L'i'),
	USBD_WIDE_CHAR(L'o'),
};

/*
 * The unique ID of STM32F0x8 is 12 bytes.
 * Unicode representation has two bytes at the start for size and type,
 * and then 24 bytes for the characters for a total of 26 bytes (0x1A)
 */
__ALIGN_BEGIN uint8_t USBD_StringSerial[USB_SIZ_STRING_SERIAL] __ALIGN_END =
{
	USB_SIZ_STRING_SERIAL, /* bLength */
	USB_DESC_TYPE_STRING, /* bDescriptorType (STRING) */
};

//__ALIGN_BEGIN uint8_t USBD_StrDesc[USBD_MAX_STR_DESC_SIZ] __ALIGN_END;

/* Private functions ---------------------------------------------------------*/
static void IntToUnicode(uint32_t value, uint8_t *pbuf, uint8_t len);
static void Get_SerialNum(void);

/**
 * @brief  Returns the device descriptor.
 * @param  speed: Current device speed
 * @param  length: Pointer to data length variable
 * @retval Pointer to descriptor buffer
 */
uint8_t* USBD_Class_DeviceDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
	UNUSED(speed);

	*length = sizeof(USBD_DeviceDesc);

	return USBD_DeviceDesc;
}

/**
 * @brief  Returns the LangID string descriptor.
 * @param  speed: Current device speed
 * @param  length: Pointer to data length variable
 * @retval Pointer to descriptor buffer
 */
uint8_t* USBD_Class_LangIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
	UNUSED(speed);

	*length = sizeof(USBD_LangIDDesc);

	return USBD_LangIDDesc;
}

/**
 * @brief  Returns the manufacturer string descriptor.
 * @param  speed: Current device speed
 * @param  length: Pointer to data length variable
 * @retval Pointer to descriptor buffer
 */
uint8_t* USBD_Class_ManufacturerStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
	UNUSED(speed);

	*length = sizeof(USDB_ManufacturerStringDesc);

	return USDB_ManufacturerStringDesc;
}

/**
 * @brief  Returns the product string descriptor.
 * @param  speed: Current device speed
 * @param  length: Pointer to data length variable
 * @retval Pointer to descriptor buffer
 */
uint8_t* USBD_Class_ProductStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
	UNUSED(speed);

	*length = sizeof(USDB_ProductStringDesc);

	return USDB_ProductStringDesc;
}

/**
 * @brief  Returns the serial number string descriptor.
 * @param  speed: Current device speed
 * @param  length: Pointer to data length variable
 * @retval Pointer to descriptor buffer
 */
uint8_t* USBD_Class_SerialStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
	UNUSED(speed);

	*length = sizeof(USBD_StringSerial);

	/* Update the serial number string descriptor with the data from the unique ID*/
	Get_SerialNum();

	return USBD_StringSerial;
}

/**
 * @brief  Create the serial number string descriptor
 * @param  None
 * @retval None
 */
static void Get_SerialNum(void)
{
	uint32_t deviceserial0;
	uint32_t deviceserial1;
	uint32_t deviceserial2;

	deviceserial0 = *(uint32_t*) DEVICE_ID1;
	deviceserial1 = *(uint32_t*) DEVICE_ID2;
	deviceserial2 = *(uint32_t*) DEVICE_ID3;

	deviceserial0 += deviceserial2;

	if (deviceserial0 != 0U)
	{
		IntToUnicode(deviceserial0, &USBD_StringSerial[2], 8U);
		IntToUnicode(deviceserial1, &USBD_StringSerial[18], 4U);
	}
}

/**
 * @brief  Convert a 32-bite unsigned value to a wide character
 * @param  value: value to convert
 * @param  pbuf: pointer to the buffer
 * @param  len: buffer length
 * @retval None
 */
static void IntToUnicode(uint32_t value, uint8_t *pbuf, uint8_t len)
{
	uint8_t idx = 0U;

	for (idx = 0U; idx < len; idx++)
	{
		if (((value >> 28)) < 0xAU)
		{
			pbuf[2U * idx] = (value >> 28) + '0';
		}
		else
		{
			pbuf[2U * idx] = (value >> 28) + 'A' - 10U;
		}

		value = value << 4;

		pbuf[2U * idx + 1] = 0U;
	}
}

/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file     usbd_conf.c
 * @brief    USB Device configuration and interface file
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
#include "main.h"
#include "usbd_core.h"
#include "usbd_radio.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
PCD_HandleTypeDef hpcd_USB_FS;

/* Private function prototypes -----------------------------------------------*/
USBD_StatusTypeDef USBD_Get_USB_Status(HAL_StatusTypeDef hal_status);

/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
               LL Driver Interface (PCD --> USB Device Library)
 *******************************************************************************/

/**
 * @brief  Initializes the PCD MSP.
 * @param  hpcd PCD handle
 * @retval None
 */
void HAL_PCD_MspInit(PCD_HandleTypeDef *pcdHandle)
{
	if (pcdHandle->Instance == USB)
	{
		/* Peripheral clock enable */
		__HAL_RCC_USB_CLK_ENABLE();

		/* Peripheral interrupt init */
		HAL_NVIC_SetPriority(USB_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(USB_IRQn);
	}
}

/**
 * @brief  DeInitializes PCD MSP.
 * @param  hpcd PCD handle
 * @retval None
 */
void HAL_PCD_MspDeInit(PCD_HandleTypeDef *pcdHandle)
{
	if (pcdHandle->Instance == USB)
	{
		/* Peripheral clock disable */
		__HAL_RCC_USB_CLK_DISABLE();

		/* Peripheral interrupt Deinit*/
		HAL_NVIC_DisableIRQ(USB_IRQn);
	}
}

/**
 * @brief  Setup stage callback
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd)
{
	USBD_LL_SetupStage((USBD_HandleTypeDef*) hpcd->pData, (uint8_t*) hpcd->Setup);
}

/**
 * @brief  Data Out stage callback.
 * @param  hpcd: PCD handle
 * @param  epnum: Endpoint number
 * @retval None
 */
void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
	USBD_LL_DataOutStage((USBD_HandleTypeDef*) hpcd->pData, epnum, hpcd->OUT_ep[epnum].xfer_buff);
}

/**
 * @brief  Data In stage callback.
 * @param  hpcd: PCD handle
 * @param  epnum: Endpoint number
 * @retval None
 */
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
	USBD_LL_DataInStage((USBD_HandleTypeDef*) hpcd->pData, epnum, hpcd->IN_ep[epnum].xfer_buff);
}

/**
 * @brief  SOF callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd)
{
	USBD_LL_SOF((USBD_HandleTypeDef*) hpcd->pData);
}

/**
 * @brief  Reset callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd)
{
	USBD_SpeedTypeDef speed = USBD_SPEED_FULL;

	if (hpcd->Init.speed != PCD_SPEED_FULL)
	{
		Error_Handler();
	}

	/* Set Speed. */
	USBD_LL_SetSpeed((USBD_HandleTypeDef*) hpcd->pData, speed);

	/* Reset Device. */
	USBD_LL_Reset((USBD_HandleTypeDef*) hpcd->pData);
}

/**
 * @brief  Suspend callback.
 * When Low power mode is enabled the debug cannot be used (IAR, Keil doesn't support it)
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd)
{
	/* Inform USB library that core enters in suspend Mode. */
	USBD_LL_Suspend((USBD_HandleTypeDef*) hpcd->pData);

	/* Enter in STOP mode. */
	if (hpcd->Init.low_power_enable)
	{
		/* Set SLEEPDEEP bit and SleepOnExit of Cortex System Control Register. */
		SCB->SCR |= (uint32_t) ((uint32_t) (SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));
	}
}

/**
 * @brief  Resume callback.
 * When Low power mode is enabled the debug cannot be used (IAR, Keil doesn't support it)
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd)
{
	if (hpcd->Init.low_power_enable)
	{
		/* Reset SLEEPDEEP bit of Cortex System Control Register. */
		SCB->SCR &= (uint32_t) ~((uint32_t) (SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));

		/* Reconfigure system clock */
		SystemClock_Config();
	}

	USBD_LL_Resume((USBD_HandleTypeDef*) hpcd->pData);
}

/**
 * @brief  ISOOUTIncomplete callback.
 * @param  hpcd: PCD handle
 * @param  epnum: Endpoint number
 * @retval None
 */
void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
	USBD_LL_IsoOUTIncomplete((USBD_HandleTypeDef*) hpcd->pData, epnum);
}

/**
 * @brief  ISOINIncomplete callback.
 * @param  hpcd: PCD handle
 * @param  epnum: Endpoint number
 * @retval None
 */
void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
	USBD_LL_IsoINIncomplete((USBD_HandleTypeDef*) hpcd->pData, epnum);
}

/**
 * @brief  Connect callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd)
{
	USBD_LL_DevConnected((USBD_HandleTypeDef*) hpcd->pData);
}

/**
 * @brief  Disconnect callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd)
{
	USBD_LL_DevDisconnected((USBD_HandleTypeDef*) hpcd->pData);
}

/*******************************************************************************
               LL Driver Interface (USB Device Library --> PCD)
 *******************************************************************************/

/**
 * @brief  Initializes the Low Level portion of the Device driver.
 * @param  pdev: Device handle
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_Init(USBD_HandleTypeDef *pdev)
{
	/* Link the library and the PCD driver */
	hpcd_USB_FS.pData = pdev;
	pdev->pData = &hpcd_USB_FS;

	/* Initialize the USB PCD handle */
	hpcd_USB_FS.Instance = USB;
	hpcd_USB_FS.Init.dev_endpoints = 8;
	hpcd_USB_FS.Init.speed = PCD_SPEED_FULL;
	hpcd_USB_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
	hpcd_USB_FS.Init.low_power_enable = DISABLE;
	hpcd_USB_FS.Init.lpm_enable = DISABLE;
	hpcd_USB_FS.Init.battery_charging_enable = DISABLE;

	if (HAL_PCD_Init(&hpcd_USB_FS) != HAL_OK)
	{
		Error_Handler();
	}

	// Calculate the locations for the PMA buffers
	// First there is the Buffer Descriptor Table (BDT) which requires 4 bytes per endpoint and direction
	// For example, EP0 IN would take 4 bytes, EP0 OUT the next four, and so on

	uint32_t ep0out = 4 * 0x04; 					// // We have total of four entries (EP0 OUT, EP0 IN, EP1 DBL-1 and EP1 DBL-2)
	uint32_t ep0in  = ep0out + USB_MAX_EP0_SIZE;	// +  64 B (0x50)
	uint32_t ep1in0 = ep0in  + USB_MAX_EP0_SIZE;	// +  64 B (0x90)
	uint32_t ep1in1 = ep1in0 + USB_TOTAL_BUF_SIZE;	// Total buffer size is adjustable

	/* Register PMA endpoints for EP 0 (Standard control) */
	HAL_PCDEx_PMAConfig(&hpcd_USB_FS, 0x00, PCD_SNG_BUF, ep0out);
	HAL_PCDEx_PMAConfig(&hpcd_USB_FS, 0x80, PCD_SNG_BUF, ep0in);

	/* Register PMA endpoint for EP 1 IN (Audio, double-buffered so addresses are provided via LSB and MSB) */
	HAL_PCDEx_PMAConfig(&hpcd_USB_FS, AUDIO_IN_EP, PCD_DBL_BUF, ep1in0 | (ep1in1 << 16));

	// TODO: Register the endpoints for HID

	return USBD_OK;
}

/**
 * @brief  De-Initializes the Low Level portion of the Device driver.
 * @param  pdev: Device handle
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_DeInit(USBD_HandleTypeDef *pdev)
{
	HAL_StatusTypeDef hal_status = HAL_OK;
	USBD_StatusTypeDef usb_status = USBD_OK;

	hal_status = HAL_PCD_DeInit(pdev->pData);

	usb_status = USBD_Get_USB_Status(hal_status);

	return usb_status;
}

/**
 * @brief  Starts the Low Level portion of the Device driver.
 * @param  pdev: Device handle
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_Start(USBD_HandleTypeDef *pdev)
{
	HAL_StatusTypeDef hal_status = HAL_OK;
	USBD_StatusTypeDef usb_status = USBD_OK;

	hal_status = HAL_PCD_Start(pdev->pData);

	usb_status = USBD_Get_USB_Status(hal_status);

	return usb_status;
}

/**
 * @brief  Stops the Low Level portion of the Device driver.
 * @param  pdev: Device handle
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_Stop(USBD_HandleTypeDef *pdev)
{
	HAL_StatusTypeDef hal_status = HAL_OK;
	USBD_StatusTypeDef usb_status = USBD_OK;

	hal_status = HAL_PCD_Stop(pdev->pData);

	usb_status = USBD_Get_USB_Status(hal_status);

	return usb_status;
}

/**
 * @brief  Opens an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @param  ep_type: Endpoint Type
 * @param  ep_mps: Endpoint Max Packet Size
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_OpenEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t ep_type, uint16_t ep_mps)
{
	HAL_StatusTypeDef hal_status = HAL_OK;
	USBD_StatusTypeDef usb_status = USBD_OK;

	hal_status = HAL_PCD_EP_Open(pdev->pData, ep_addr, ep_mps, ep_type);

	usb_status = USBD_Get_USB_Status(hal_status);

	return usb_status;
}

/**
 * @brief  Closes an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_CloseEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
	HAL_StatusTypeDef hal_status = HAL_OK;
	USBD_StatusTypeDef usb_status = USBD_OK;

	hal_status = HAL_PCD_EP_Close(pdev->pData, ep_addr);

	usb_status = USBD_Get_USB_Status(hal_status);

	return usb_status;
}

/**
 * @brief  Flushes an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_FlushEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
	HAL_StatusTypeDef hal_status = HAL_OK;
	USBD_StatusTypeDef usb_status = USBD_OK;

	hal_status = HAL_PCD_EP_Flush(pdev->pData, ep_addr);

	usb_status = USBD_Get_USB_Status(hal_status);

	return usb_status;
}

/**
 * @brief  Sets a Stall condition on an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_StallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
	HAL_StatusTypeDef hal_status = HAL_OK;
	USBD_StatusTypeDef usb_status = USBD_OK;

	hal_status = HAL_PCD_EP_SetStall(pdev->pData, ep_addr);

	usb_status = USBD_Get_USB_Status(hal_status);

	return usb_status;
}

/**
 * @brief  Clears a Stall condition on an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_ClearStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
	HAL_StatusTypeDef hal_status = HAL_OK;
	USBD_StatusTypeDef usb_status = USBD_OK;

	hal_status = HAL_PCD_EP_ClrStall(pdev->pData, ep_addr);

	usb_status = USBD_Get_USB_Status(hal_status);

	return usb_status;
}

/**
 * @brief  Returns Stall condition.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval Stall (1: Yes, 0: No)
 */
uint8_t USBD_LL_IsStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
	PCD_HandleTypeDef *hpcd = (PCD_HandleTypeDef*) pdev->pData;

	if ((ep_addr & 0x80) == 0x80)
	{
		return hpcd->IN_ep[ep_addr & 0x7F].is_stall;
	}
	else
	{
		return hpcd->OUT_ep[ep_addr & 0x7F].is_stall;
	}
}

/**
 * @brief  Assigns a USB address to the device.
 * @param  pdev: Device handle
 * @param  dev_addr: Endpoint Number
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_SetUSBAddress(USBD_HandleTypeDef *pdev, uint8_t dev_addr)
{
	HAL_StatusTypeDef hal_status = HAL_OK;
	USBD_StatusTypeDef usb_status = USBD_OK;

	hal_status = HAL_PCD_SetAddress(pdev->pData, dev_addr);

	usb_status = USBD_Get_USB_Status(hal_status);

	return usb_status;
}

/**
 * @brief  Transmits data over an endpoint.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @param  pbuf: Pointer to data to be sent
 * @param  size: Data size
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_Transmit(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t *pbuf, uint32_t size)
{
	HAL_StatusTypeDef hal_status = HAL_OK;
	USBD_StatusTypeDef usb_status = USBD_OK;

	hal_status = HAL_PCD_EP_Transmit(pdev->pData, ep_addr, pbuf, size);

	usb_status = USBD_Get_USB_Status(hal_status);

	return usb_status;
}

/**
 * @brief  Prepares an endpoint for reception.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @param  pbuf: Pointer to data to be received
 * @param  size: Data size
 * @retval USBD Status
 */
USBD_StatusTypeDef USBD_LL_PrepareReceive(USBD_HandleTypeDef *pdev,	uint8_t ep_addr, uint8_t *pbuf, uint32_t size)
{
	HAL_StatusTypeDef hal_status = HAL_OK;
	USBD_StatusTypeDef usb_status = USBD_OK;

	hal_status = HAL_PCD_EP_Receive(pdev->pData, ep_addr, pbuf, size);

	usb_status = USBD_Get_USB_Status(hal_status);

	return usb_status;
}

/**
 * @brief  Returns the last transferred packet size.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint Number
 * @retval Received Data Size
 */
uint32_t USBD_LL_GetRxDataSize(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
	return HAL_PCD_EP_GetRxCount((PCD_HandleTypeDef*) pdev->pData, ep_addr);
}

#ifdef USBD_HS_TESTMODE_ENABLE
/**
  * @brief  Set High speed Test mode.
  * @param  pdev: Device handle
  * @param  testmode: test mode
  * @retval USBD Status
  */
USBD_StatusTypeDef USBD_LL_SetTestMode(USBD_HandleTypeDef *pdev, uint8_t testmode)
{
  UNUSED(pdev);
  UNUSED(testmode);

  return USBD_OK;
}
#endif /* USBD_HS_TESTMODE_ENABLE */

/**
 * @brief  Delays routine for the USB Device Library.
 * @param  Delay: Delay in ms
 * @retval None
 */
void USBD_LL_Delay(uint32_t Delay)
{
	HAL_Delay(Delay);
}

/**
 * @brief  Returns the USB status depending on the HAL status:
 * @param  hal_status: HAL status
 * @retval USB status
 */
USBD_StatusTypeDef USBD_Get_USB_Status(HAL_StatusTypeDef hal_status)
{
	USBD_StatusTypeDef usb_status = USBD_OK;

	switch (hal_status)
	{
	case HAL_OK:
		usb_status = USBD_OK;
		break;
	case HAL_ERROR:
		usb_status = USBD_FAIL;
		break;
	case HAL_BUSY:
		usb_status = USBD_BUSY;
		break;
	case HAL_TIMEOUT:
		usb_status = USBD_FAIL;
		break;
	default:
		usb_status = USBD_FAIL;
		break;
	}
	return usb_status;
}

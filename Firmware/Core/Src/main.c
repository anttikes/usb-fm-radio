/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "i2s.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "si4705.h"
#include "usbd_radio.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
int8_t Radio_Init(uint32_t AudioFreq, uint32_t Volume, uint32_t options);
int8_t Radio_DeInit(uint32_t options);
int8_t Radio_AudioCmd(uint8_t *pbuf, uint32_t size, uint8_t cmd);
int8_t Radio_VolumeCtl(uint8_t vol);
int8_t Radio_MuteCtl(uint8_t cmd);
int8_t Radio_PeriodicTC(uint8_t *pbuf, uint32_t size, uint8_t cmd);
int8_t Radio_GetState(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
USBD_RADIO_ItfTypeDef USBD_AUDIO_fops_FS = {
	Radio_Init,
	Radio_DeInit,
	Radio_AudioCmd,
	Radio_VolumeCtl,
	Radio_MuteCtl,
	Radio_PeriodicTC,
	Radio_GetState
};

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_I2C1_Init();
	MX_I2S1_Init();
	MX_USB_DEVICE_Init();
	/* USER CODE BEGIN 2 */

	// Bring Si4705 out of reset, and enable the oscillator
    HAL_GPIO_WritePin(RADIO_NRST_GPIO_Port, RADIO_NRST_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(RCLK_EN_GPIO_Port, RCLK_EN_Pin, GPIO_PIN_SET);

	// The oscillator has a max startup time of one second
	// The radio chip also has a startup time but it is much faster than the crystal
	HAL_Delay(1250);

	// Send "Power Up" to the radio
	if (PowerUp(&radioDevice, POWER_UP_ARGS_1_FUNCTION_FM, POWER_UP_ARGS_2_DIGITAL_OUTPUT_2) != HAL_OK)	{
		Error_Handler();
	}

	// Query the revision
	//	GetRevisionResponse_t revision = { 0 };
	//
	//	if (GetRevision(&radioDevice, &revision) != HAL_OK)
	//	{
	//		return (USBD_FAIL);
	//	}

	// Configure audio format to use 24 bit samples (by default it's 16 bits)
	//	if (SetProperty(&radioDevice, PROP_DIGITAL_OUTPUT_FORMAT, 0x0002) != HAL_OK)
	//	{
	//	    return (USBD_FAIL);
	//	}

	// Set volume to half
	if (SetProperty(&radioDevice, PROP_RX_VOLUME, 0x31U) != HAL_OK) {
		Error_Handler();
	}

	// Tune to Kasari
	if (TuneFreq(&radioDevice, FM_TUNE_FREQ_ARGS_NONE, 9410) != HAL_OK) {
		Error_Handler();
	}

	// Find the first available station, and wait for tune to complete
	//	if (SeekStart(&radioDevice, SEEK_START_ARGS_UP) != HAL_OK) {
	//		return (USBD_FAIL);
	//	}

	WaitForStatus(&radioDevice, STATUS_STCINT);

	// Instruct the DMA to start transferring samples to the audio buffer
	// This also enables DCLK and DFS which the radio chip needs in order
	// to enable its digital audio output
	USBD_RADIO_HandleTypeDef *hradio;

	hradio = (USBD_RADIO_HandleTypeDef*) hUsbDeviceFS.pClassDataCmsit[hUsbDeviceFS.classId];

	if (hradio == NULL) {
		return (USBD_FAIL);
	}

	// The DMA buffer is large enough to hold four USB 2.0 frames worth of data (4 ms)
	// The value provided here is the total number of one-channel samples that can fit into the buffer
	if (HAL_I2S_Receive_DMA(&hi2s1, &hradio->halfWordBuffer[0], DMA_TOTAL_BUF_SIZE / 2U) != HAL_OK) {
		Error_Handler();
	}

	// Instruct the radio chip to start providing audio samples at the "real frequency" of the I2S
	if (SetProperty(&radioDevice, PROP_DIGITAL_OUTPUT_SAMPLE_RATE, AUDIO_REAL_FREQ) != HAL_OK) {
		Error_Handler();
	}

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}

	/* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
	RCC_CRSInitTypeDef RCC_CRSInitStruct = {0};

	/* Initializes the RCC Oscillators */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
	RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI48;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
	RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV5;

	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/* Initializes the CPU, AHB and APB buses clocks */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	{
		Error_Handler();
	}

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB|RCC_PERIPHCLK_I2C1;
	PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_SYSCLK;
	PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;

	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		Error_Handler();
	}

	/* Enable the SYSCFG APB clock */
	__HAL_RCC_CRS_CLK_ENABLE();

	/* Configure CRS */
	RCC_CRSInitStruct.Prescaler = RCC_CRS_SYNC_DIV1;
	RCC_CRSInitStruct.Source = RCC_CRS_SYNC_SOURCE_USB;
	RCC_CRSInitStruct.Polarity = RCC_CRS_SYNC_POLARITY_RISING;
	RCC_CRSInitStruct.ReloadValue = __HAL_RCC_CRS_RELOADVALUE_CALCULATE(48000000,1000);
	RCC_CRSInitStruct.ErrorLimitValue = 34;
	RCC_CRSInitStruct.HSI48CalibrationValue = 32;

	HAL_RCCEx_CRSConfig(&RCC_CRSInitStruct);
}

/* USER CODE BEGIN 4 */
int8_t Radio_Init(uint32_t AudioFreq, uint32_t Volume, uint32_t options)
{
	UNUSED(AudioFreq);
	UNUSED(Volume);
	UNUSED(options);

	return (USBD_OK);
}

int8_t Radio_DeInit(uint32_t options)
{
	UNUSED(options);

	return (USBD_OK);
}

int8_t Radio_AudioCmd(uint8_t *pbuf, uint32_t size, uint8_t cmd)
{
	UNUSED(pbuf);
	UNUSED(size);
	UNUSED(cmd);

	return (USBD_OK);
}

int8_t Radio_VolumeCtl(uint8_t vol)
{
	UNUSED(vol);

	return (USBD_OK);
}

int8_t Radio_MuteCtl(uint8_t cmd)
{
	UNUSED(cmd);

	return (USBD_OK);
}

int8_t Radio_PeriodicTC(uint8_t *pbuf, uint32_t size, uint8_t cmd)
{
	UNUSED(pbuf);
	UNUSED(size);
	UNUSED(cmd);

	return (USBD_OK);
}

int8_t Radio_GetState(void)
{
	return (USBD_OK);
}

void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
	UNUSED(hi2s);

	USBD_RADIO_Sync(&hUsbDeviceFS, AUDIO_OFFSET_HALF);
}

void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
	UNUSED(hi2s);

	USBD_RADIO_Sync(&hUsbDeviceFS, AUDIO_OFFSET_FULL);
}

void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s)
{
	UNUSED(hi2s);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
	UNUSED(file);
	UNUSED(line);

	/* User can add his own implementation to report the file name and line number,
	   ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

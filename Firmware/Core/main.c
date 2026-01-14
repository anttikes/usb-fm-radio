/**
 ******************************************************************************
 * @file    main.c
 * @brief   Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "commands.h"
#include "common.h"
#include "device.h"
#include "dma.h"
#include "gpio.h"
#include "i2c.h"
#include "i2s.h"
#include "tim.h"
#include "tusb.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define DMA_BUFFER_LENGTH CFG_TUD_AUDIO_FUNC_1_EP_IN_SW_BUF_SZ / 8
#define DMA_BUFFER_START 0
#define DMA_BUFFER_MIDPOINT DMA_BUFFER_LENGTH / 2

// This comes from the IOC tool
#define REAL_AUDIO_FREQUENCY 48000

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
uint16_t i2sBuffer[DMA_BUFFER_LENGTH];

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* Private user code ---------------------------------------------------------*/

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    /* Initialize HAL */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_I2C1_Init();
    MX_I2S1_Init();
    MX_TIM14_Init();

    /* USB peripheral clock enable and interrupt priority */
    __HAL_RCC_USB_CLK_ENABLE();
    HAL_NVIC_SetPriority(USB_IRQn, 0, 0);

    // Bring Si4705 out of reset, and enable the oscillator
    HAL_GPIO_WritePin(RADIO_NRST_GPIO_Port, RADIO_NRST_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(RCLK_EN_GPIO_Port, RCLK_EN_Pin, GPIO_PIN_SET);

    // The oscillator has a max startup time of one second
    // The radio chip also has a startup time but it is much faster than the
    // crystal
    HAL_Delay(1250);

    // Power up the radio
    if (!PowerUp(&radioDevice,
                 POWER_UP_ARGS_1_FUNCTION_FM | POWER_UP_ARGS_1_GPO2_OUTPUT_ENABLE |
                     POWER_UP_ARGS_1_CTS_INTERRUPT_ENABLE,
                 POWER_UP_ARGS_2_DIGITAL_OUTPUT_2))
    {
        Error_Handler();
    }

    // Enable the other interrupt sources
    if (!SetInterruptSources(&radioDevice,
                             INTERRUPT_SOURCES_CTSIEN | INTERRUPT_SOURCES_STCIEN | INTERRUPT_SOURCES_STCREP))
    {
        Error_Handler();
    }

    // Set volume to half
    if (!SetProperty(&radioDevice, PROP_ID_RX_VOLUME, 0x31U))
    {
        Error_Handler();
    }

    // Allow GPO output, and drive GPO1 and GPO3 low to reduce oscillation
    if (!GPIOCtl(&radioDevice, GPIO_CTL_GPO1_OUTPUT_ENABLE | GPIO_CTL_GPO2_OUTPUT_ENABLE | GPIO_CTL_GPO3_OUTPUT_ENABLE))
    {
        Error_Handler();
    }

    if (!GPIOSet(&radioDevice, GPIO_SET_GPO2_OUTPUT_HIGH))
    {
        Error_Handler();
    }

    // Tune to Kasari
    if (!TuneFreq(&radioDevice, FM_TUNE_FREQ_ARGS_NONE, 9410))
    {
        Error_Handler();
    }

    // When using circular mode, the size parameter must equal the number of
    // elements in the receiving array
    if (HAL_I2S_Receive_DMA(&hi2s1, &i2sBuffer[0], DMA_BUFFER_LENGTH) != HAL_OK)
    {
        Error_Handler();
    }

    // Now that the DCLK and DFS are enabled we can instruct the radio chip to start providing audio samples
    if (!SetProperty(&radioDevice, PROP_ID_DIGITAL_OUTPUT_SAMPLE_RATE, REAL_AUDIO_FREQUENCY))
    {
        Error_Handler();
    }

    // Initialize TinyUSB
    tusb_rhport_init_t dev_init = {.role = TUSB_ROLE_DEVICE, .speed = TUSB_SPEED_FULL};

    tusb_init(BOARD_DEVICE_RHPORT_NUM, &dev_init);

    /* Infinite loop */
    while (1)
    {
        tud_task();

        ProcessCommand(&radioDevice);
    }
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

    /* Initialize the RCC Oscillators */
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

    /* Initialize the CPU, AHB and APB buses clocks */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
    {
        Error_Handler();
    }

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB | RCC_PERIPHCLK_I2C1;
    PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_SYSCLK;
    PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }

    /* Enable the CRS clock */
    __HAL_RCC_CRS_CLK_ENABLE();

    /* Configure CRS */
    RCC_CRSInitStruct.Prescaler = RCC_CRS_SYNC_DIV1;
    RCC_CRSInitStruct.Source = RCC_CRS_SYNC_SOURCE_USB;
    RCC_CRSInitStruct.Polarity = RCC_CRS_SYNC_POLARITY_RISING;
    RCC_CRSInitStruct.ReloadValue = __HAL_RCC_CRS_RELOADVALUE_CALCULATE(48000000, 1000);
    RCC_CRSInitStruct.ErrorLimitValue = 34;
    RCC_CRSInitStruct.HSI48CalibrationValue = 32;

    HAL_RCCEx_CRSConfig(&RCC_CRSInitStruct);
}

/* External callbacks --------------------------------------------------------*/

/**
 * @brief  Rx Transfer half completed callbacks
 * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
 *         the configuration information for I2S module
 * @retval None
 */
void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
    if (hi2s->Instance == SPI1)
    {
        tud_audio_write(&i2sBuffer[DMA_BUFFER_START], DMA_BUFFER_LENGTH);
    }
}

/**
 * @brief  Rx Transfer completed callbacks
 * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
 *         the configuration information for I2S module
 * @retval None
 */
void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
    if (hi2s->Instance == SPI1)
    {
        tud_audio_write(&i2sBuffer[DMA_BUFFER_MIDPOINT], DMA_BUFFER_LENGTH);
    }
}

/**
 * @brief  Period elapsed callback in non-blocking mode
 * @param  htim TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (radioDevice.currentState == RADIOSTATE_TUNED_TO_STATION ||
        radioDevice.currentState == RADIOSTATE_DIGITAL_OUTPUT_ENABLED)
    {
        bool succeeded = RSQStatus(&radioDevice, FM_RSQ_STATUS_ARGS_NONE);

        if (!succeeded)
        {
            // Queue is full; when the device is streaming data there's a noise
            // problem which sometimes causes the CTS signal to be missed,
            // and thus the current command never completes

            // To recover, we clear the queue, and send a fault report
            memset(radioDevice.commandQueue.commands, 0, 10 * sizeof(Command_t));
            radioDevice.commandQueue.front = 0;
            radioDevice.commandQueue.back = 0;
            radioDevice.commandQueue.count = 0;

            Command_t fault = {0};

            fault.state = COMMANDSTATE_RESPONSE_RECEIVED;
            fault.args.bytes[0] = 0xAA; // This isn't a real command identifier, so can't use 'opCode'

            EnqueueCommand(&radioDevice, &fault);
        }
    }
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    UNUSED(file);
    UNUSED(line);

    /* User can add his own implementation to report the file name and line
       number, ex: printf("Wrong parameters value: file %s on line %d\r\n", file,
       line) */
}
#endif /* USE_FULL_ASSERT */

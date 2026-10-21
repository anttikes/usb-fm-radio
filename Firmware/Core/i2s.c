/**
 ******************************************************************************
 * @file    i2s.c
 * @brief   This file provides code for the configuration
 *          of the I2S instances.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) Antti Keskinen
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "i2s.h"

/* Global variables ----------------------------------------------------------*/
I2S_HandleTypeDef hi2s1;
DMA_HandleTypeDef hdma_spi1_rx;

/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Initialize I2S1
 * @retval None
 */
void MX_I2S1_Init(void)
{
    hi2s1.Instance = SPI1;
    hi2s1.Init.Mode = I2S_MODE_MASTER_RX;
    hi2s1.Init.Standard = I2S_STANDARD_PHILIPS;
    hi2s1.Init.DataFormat = I2S_DATAFORMAT_16B;
    hi2s1.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
    hi2s1.Init.AudioFreq = I2S_AUDIOFREQ_48K;
    hi2s1.Init.CPOL = I2S_CPOL_LOW;

    if (HAL_I2S_Init(&hi2s1) != HAL_OK)
    {
        Error_Handler();
    }
}

/* External callbacks --------------------------------------------------------*/

/**
 * @brief  Invoked by HAL to perform lower level initialization
 * @param  hi2s Pointer to a I2S_HandleTypeDef structure that contains
 *         the configuration information for I2S module
 * @retval None
 */
void HAL_I2S_MspInit(I2S_HandleTypeDef *hi2s)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (hi2s->Instance == SPI1)
    {
        /* I2S1 clock enable */
        __HAL_RCC_SPI1_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /* I2S1 GPIO Configuration
        PA4     ------> I2S1_WS
        PA5     ------> I2S1_CK
        PA7     ------> I2S1_SD
        */
        GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF0_SPI1;

        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* I2S1 DMA Init */
        /* SPI1_RX Init */
        hdma_spi1_rx.Instance = DMA1_Channel2;
        hdma_spi1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_spi1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_spi1_rx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_spi1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_spi1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_spi1_rx.Init.Mode = DMA_CIRCULAR;
        hdma_spi1_rx.Init.Priority = DMA_PRIORITY_LOW;

        if (HAL_DMA_Init(&hdma_spi1_rx) != HAL_OK)
        {
            Error_Handler();
        }

        __HAL_LINKDMA(hi2s, hdmarx, hdma_spi1_rx);
    }
}

/**
 * @brief  Invoked by HAL to perform lower level de-initialization
 * @param  hi2s Pointer to a I2S_HandleTypeDef structure that contains
 *              the configuration information for I2S module
 * @retval None
 */
void HAL_I2S_MspDeInit(I2S_HandleTypeDef *hi2s)
{
    if (hi2s->Instance == SPI1)
    {
        /* Peripheral clock disable */
        __HAL_RCC_SPI1_CLK_DISABLE();

        /* I2S1 GPIO Configuration
        PA4     <------ I2S1_WS
        PA5     <------ I2S1_CK
        PA7     <------ I2S1_SD
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7);

        /* I2S1 DMA DeInit */
        HAL_DMA_DeInit(hi2s->hdmarx);
    }
}

/* Private functions ---------------------------------------------------------*/

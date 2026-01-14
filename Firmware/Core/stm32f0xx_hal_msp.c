/**
 ******************************************************************************
 * @file     stm32f0xx_hal_msp.c
 * @brief    This file provides code for the MSP Initialization
 *           and de-Initialization codes.
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* External functions --------------------------------------------------------*/

/**
 * Initializes the Global MSP.
 */
void HAL_MspInit(void)
{
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();

    /* System interrupt init*/

    /* Peripheral interrupt init */
    /* RCC_CRS_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(RCC_CRS_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(RCC_CRS_IRQn);
}

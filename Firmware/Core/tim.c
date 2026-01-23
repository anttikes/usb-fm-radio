/**
 ******************************************************************************
 * @file    tim.c
 * @brief   This file provides code for the configuration
 *          of the TIM instances.
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
#include "tim.h"

TIM_HandleTypeDef htim14;

/* TIM14 init function */
void MX_TIM14_Init(void)
{
    htim14.Instance = TIM14;
    htim14.Init.Prescaler = 799;
    htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim14.Init.Period = 47999;
    htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(&htim14) != HAL_OK)
    {
        Error_Handler();
    }
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM14)
    {
        /* TIM14 clock enable */
        __HAL_RCC_TIM14_CLK_ENABLE();

        /* TIM14 interrupt Init */
        HAL_NVIC_SetPriority(TIM14_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(TIM14_IRQn);
    }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM14)
    {
        /* Peripheral clock disable */
        __HAL_RCC_TIM14_CLK_DISABLE();

        /* TIM14 interrupt Deinit */
        HAL_NVIC_DisableIRQ(TIM14_IRQn);
    }
}

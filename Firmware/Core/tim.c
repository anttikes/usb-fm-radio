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
 *
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "tim.h"

TIM_HandleTypeDef htim16;
TIM_HandleTypeDef htim17;

/* TIM16 init function */
void MX_TIM16_Init(void)
{
    htim16.Instance = TIM16;
    htim16.Init.Prescaler = 799;
    htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim16.Init.Period = 47999;
    htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim16.Init.RepetitionCounter = 0;
    htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
    {
        Error_Handler();
    }
}

/* TIM17 init function */
void MX_TIM17_Init(void)
{
    htim17.Instance = TIM17;
    htim17.Init.Prescaler = 799;
    htim17.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim17.Init.Period = 47999;
    htim17.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim17.Init.RepetitionCounter = 0;
    htim17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(&htim17) != HAL_OK)
    {
        Error_Handler();
    }
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM16)
    {
        /* TIM16 clock enable */
        __HAL_RCC_TIM16_CLK_ENABLE();

        /* TIM16 interrupt Init */
        HAL_NVIC_SetPriority(TIM16_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(TIM16_IRQn);
    }
    else if (htim->Instance == TIM17)
    {
        /* TIM17 clock enable */
        __HAL_RCC_TIM17_CLK_ENABLE();

        /* TIM17 interrupt Init */
        HAL_NVIC_SetPriority(TIM17_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(TIM17_IRQn);
    }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM16)
    {
        /* Peripheral clock disable */
        __HAL_RCC_TIM16_CLK_DISABLE();

        /* TIM14 interrupt Deinit */
        HAL_NVIC_DisableIRQ(TIM16_IRQn);
    }
    else if (htim->Instance == TIM17)
    {
        /* Peripheral clock disable */
        __HAL_RCC_TIM17_CLK_DISABLE();

        /* TIM14 interrupt Deinit */
        HAL_NVIC_DisableIRQ(TIM17_IRQn);
    }
}

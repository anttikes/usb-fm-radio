/**
 ******************************************************************************
 * @file    main.h
 * @brief   Header for main.c file.
 *          This file contains the common defines of the application.
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

/* Private includes ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
void SystemClock_Config(void);

/* Private defines -----------------------------------------------------------*/
#define RADIO_NIRQ_Pin GPIO_PIN_13
#define RADIO_NIRQ_GPIO_Port GPIOC
#define RADIO_NIRQ_EXTI_IRQn EXTI4_15_IRQn
#define RADIO_NRST_Pin GPIO_PIN_14
#define RADIO_NRST_GPIO_Port GPIOC
#define RCLK_EN_Pin GPIO_PIN_15
#define RCLK_EN_GPIO_Port GPIOC

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

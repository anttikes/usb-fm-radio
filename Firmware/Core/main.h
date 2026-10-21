/**
 ******************************************************************************
 * @file    main.h
 * @brief   Header for main.c file.
 *          This file contains the common defines of the application.
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

/* Header guard --------------------------------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
#define RD_RST_Pin GPIO_PIN_1
#define RD_RST_GPIO_Port GPIOA
#define RD_IRQ_Pin GPIO_PIN_2
#define RD_IRQ_GPIO_Port GPIOA
#define RD_IRQ_EXTI_IRQn EXTI2_3_IRQn
#define OSC_EN_Pin GPIO_PIN_6
#define OSC_EN_GPIO_Port GPIOA

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
extern void Error_Handler(void);
extern void SystemClock_Config(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

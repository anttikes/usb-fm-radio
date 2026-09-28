/**
 ******************************************************************************
 * @file    stm32f0xx_it.h
 * @brief   This file contains the headers of the interrupt handlers.
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F0xx_IT_H__
#define __STM32F0xx_IT_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Private includes ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void NMI_Handler(void);
void HardFault_Handler(void);
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void RCC_CRS_IRQHandler(void);
void EXTI2_3_IRQHandler(void);
void DMA1_Channel2_3_IRQHandler(void);
void TIM16_IRQHandler(void);
void TIM17_IRQHandler(void);
void I2C1_IRQHandler(void);
void USB_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F0xx_IT_H__ */

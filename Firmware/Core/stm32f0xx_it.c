/**
 ******************************************************************************
 * @file    stm32f0xx_it.c
 * @brief   Interrupt Service Routines.
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
#include "stm32f0xx_it.h"
#include "i2c.h"
#include "i2s.h"
#include "main.h"
#include "tim.h"
#include "tusb.h"

/* Global variables ----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void)
{
    while (1)
    {
    }
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void)
{
    while (1)
    {
    }
}

/**
 * @brief This function handles System service call via SWI instruction.
 */
void SVC_Handler(void)
{
}

/**
 * @brief This function handles Pendable request for system service.
 */
void PendSV_Handler(void)
{
}

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
    HAL_IncTick();
}

/**
 * @brief This function handles EXTI line 2 and 3 interrupts.
 */
void EXTI2_3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(RD_IRQ_Pin);
}

/**
 * @brief This function handles DMA1 channel 2 and 3 interrupts.
 */
void DMA1_Channel2_3_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_spi1_rx);
}

/**
 * @brief This function handles TIM16 interrupt.
 */
void TIM16_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim16);
}

/**
 * @brief This function handles TIM17 interrupt.
 */
void TIM17_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim17);
}

/**
 * @brief This function handles I2C1 event global / I2C1 wake-up interrupt.
 */
void I2C1_IRQHandler(void)
{
    if (hi2c1.Instance->ISR & (I2C_FLAG_BERR | I2C_FLAG_ARLO | I2C_FLAG_OVR))
    {
        HAL_I2C_ER_IRQHandler(&hi2c1);
    }
    else
    {
        HAL_I2C_EV_IRQHandler(&hi2c1);
    }
}

/**
 * @brief This function handles USB global / USB wake-up interrupt.
 */
void USB_IRQHandler(void)
{
    tusb_int_handler(BOARD_DEVICE_RHPORT_NUM, true);
}

/* External callbacks --------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

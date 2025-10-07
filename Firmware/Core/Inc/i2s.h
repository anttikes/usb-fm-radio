/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    i2s.h
 * @brief   This file contains all the function prototypes for
 *          the i2s.c file
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __I2S_H__
#define __I2S_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern I2S_HandleTypeDef hi2s1;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_I2S1_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __I2S_H__ */


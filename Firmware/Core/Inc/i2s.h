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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __I2S_H__
#define __I2S_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

extern I2S_HandleTypeDef hi2s1;

void MX_I2S1_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __I2S_H__ */

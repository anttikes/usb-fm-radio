/**
 ******************************************************************************
 * @file    rds.h
 * @brief   Header for rds.c
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

/* Header guard --------------------------------------------------------------*/
#ifndef __RDS_H__
#define __RDS_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include <librdsparser.h>

/* Exported types */

/* Exported constants --------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/
extern rdsparser_t rdsParser;

/* Exported functions --------------------------------------------------------*/
extern bool RDSInit();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __RDS_H__ */

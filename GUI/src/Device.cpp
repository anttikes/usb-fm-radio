/**
 ******************************************************************************
 * @file    Device.cpp
 * @brief   Implements the Device class
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
#include "Device.h"

/* File-scope Constants & Macros --------------------------------------------*/

/* Anonymous Namespace (File-local helpers and variables) -------------------*/

/* Static Member Definitions ------------------------------------------------*/

/* Constructors & Destructor ------------------------------------------------*/

/**
 * @brief  Constructs the Device class
 * @param  deviceInfo Pointer to hid_device_info data structure that contains data for this device
 *
 * @retval None
 */
Device::Device(hid_device_info *deviceInfo)
{
    m_vendorId = deviceInfo->vendor_id;
    m_productId = deviceInfo->product_id;
    m_serialNumber = QString::fromWCharArray(deviceInfo->serial_number);
    m_manufacturer = QString::fromWCharArray(deviceInfo->manufacturer_string);
    m_product = QString::fromWCharArray(deviceInfo->product_string);
    m_path = QString::fromLatin1(deviceInfo->path);
}

/* Public Methods -----------------------------------------------------------*/

/**
 * @brief  Getter function for vendor identifier
 *
 * @retval The vendor ID
 */
unsigned short Device::vendorId() const
{
    return m_vendorId;
}

/**
 * @brief  Getter function for product identifier
 *
 * @retval The product ID
 */
unsigned short Device::productId() const
{
    return m_productId;
}

/**
 * @brief  Getter function for the serial number
 *
 * @retval The serial number
 */
const QString &Device::serialNumber() const
{
    return m_serialNumber;
}

/**
 * @brief  Getter function for manufacturer
 *
 * @retval The manufacturer
 */
const QString &Device::manufacturer() const
{
    return m_manufacturer;
}

/**
 * @brief  Getter function for product name
 *
 * @retval The product name
 */
const QString &Device::product() const
{
    return m_product;
}

/**
 * @brief  Getter function for device path
 *
 * @retval The device path
 */
const QString &Device::path() const
{
    return m_path;
}

/* Protected Methods --------------------------------------------------------*/

/* Private Methods ----------------------------------------------------------*/

/* External Callbacks / Event Handlers --------------------------------------*/

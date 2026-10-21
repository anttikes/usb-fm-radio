/**
 ******************************************************************************
 * @file    Device.h
 * @brief   Header for Device.cpp
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
#ifndef __DEVICE_H__
#define __DEVICE_H__

/* Includes ------------------------------------------------------------------*/
#include <QMetaObject>
#include <QString>
#include <QtQml/qqmlregistration.h>
#include <hidapi.h>

/* Forward Declarations ------------------------------------------------------*/

/* Class Definition ----------------------------------------------------------*/
class Device
{
    /* Meta-Object Macros -----------------------------------------------------*/
    Q_GADGET

    /* Properties ------------------------------------------------------------*/
    Q_PROPERTY(unsigned short vendorId READ vendorId CONSTANT)
    Q_PROPERTY(unsigned short productId READ productId CONSTANT)
    Q_PROPERTY(QString serialNumber READ serialNumber CONSTANT)
    Q_PROPERTY(QString manufacturer READ manufacturer CONSTANT)
    Q_PROPERTY(QString product READ product CONSTANT)
    Q_PROPERTY(QString path READ path CONSTANT)

  public:
    /* Enums & Type Aliases --------------------------------------------------*/

    /* Constructors & Destructor ---------------------------------------------*/
    explicit Device(hid_device_info *deviceInfo);

    /* Signals ---------------------------------------------------------------*/

    /* Public Slots ----------------------------------------------------------*/

    /* Public Methods --------------------------------------------------------*/
    unsigned short vendorId() const;
    unsigned short productId() const;
    const QString &serialNumber() const;
    const QString &manufacturer() const;
    const QString &product() const;
    const QString &path() const;

    /* Protected Methods -----------------------------------------------------*/

    /* Protected Slots -------------------------------------------------------*/

    /* Private Slots ---------------------------------------------------------*/

  private:
    /* Private Methods -------------------------------------------------------*/

    /* Private Members -------------------------------------------------------*/
    QString m_serialNumber;
    QString m_manufacturer;
    QString m_product;
    QString m_path;

    unsigned short m_vendorId;
    unsigned short m_productId;
};

#endif // __DEVICE_H__
/**
 ******************************************************************************
 * @file    DeviceWorker.h
 * @brief   Header for DeviceWorker.cpp
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
#ifndef __DEVICEWORKER_H__
#define __DEVICEWORKER_H__

/* Includes ------------------------------------------------------------------*/
#include "Device.h"
#include <QEventLoop>
#include <QObject>
#include <QRunnable>
#include <QTimer>

/* Forward Declarations ------------------------------------------------------*/

/* Class Definition ----------------------------------------------------------*/
class DeviceWorker : public QObject, public QRunnable
{
    /* Meta-Object Macros -----------------------------------------------------*/
    Q_OBJECT

    /* Properties ------------------------------------------------------------*/

  public:
    /* Enums & Type Aliases --------------------------------------------------*/

    /* Constructors & Destructor ---------------------------------------------*/
    explicit DeviceWorker(unsigned short vendorId, unsigned short productId);
    ~DeviceWorker();

    /* Signals ---------------------------------------------------------------*/

    /* Public Slots ----------------------------------------------------------*/

    /* Public Methods --------------------------------------------------------*/
    void run() override;
    void stop();

  protected slots:
    /* Protected Methods -----------------------------------------------------*/

    /* Protected Slots -------------------------------------------------------*/

    /* Signals ---------------------------------------------------------------*/
  signals:
    void devicesChanged(QList<Device> newDevices);

  private slots:
    /* Private Slots ---------------------------------------------------------*/
    void onTimerTimeout();

  private:
    /* Private Methods -------------------------------------------------------*/
    void enumerateDevices();

    /* Private Members -------------------------------------------------------*/
    QEventLoop *m_eventLoop;
    QTimer *m_timer;

    unsigned short m_vendorId;
    unsigned short m_productId;
};

#endif // __DEVICEWORKER_H__

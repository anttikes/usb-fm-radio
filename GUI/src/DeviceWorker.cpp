/**
 ******************************************************************************
 * @file    DeviceWorker.cpp
 * @brief   Implements the DeviceWorker class
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
#include "DeviceWorker.h"
#include <QDebug>

/* File-scope Constants & Macros --------------------------------------------*/

/* Anonymous Namespace (File-local helpers and variables) -------------------*/

/* Static Member Definitions ------------------------------------------------*/

/* Constructors & Destructor ------------------------------------------------*/

/**
 * @brief  Constructs the device enumeration worker
 * @param  vendorId Vendor identifier to enumerate
 * @param  productId Product identifier to enumerate
 *
 * @retval None
 */
DeviceWorker::DeviceWorker(unsigned short vendorId, unsigned short productId)
    : QRunnable(), m_eventLoop(nullptr), m_timer(nullptr), m_vendorId(vendorId), m_productId(productId)
{
}

/**
 * @brief  Stops the worker and releases its event loop and timer
 *
 * @retval None
 */
DeviceWorker::~DeviceWorker()
{
    stop();

    if (m_eventLoop)
    {
        delete m_eventLoop;
        m_eventLoop = nullptr;
    }

    if (m_timer)
    {
        delete m_timer;
        m_timer = nullptr;
    }
}

/* Public Methods -----------------------------------------------------------*/

/**
 * @brief  Stops the worker event loop
 *
 * @retval None
 */
void DeviceWorker::stop()
{
    if (m_eventLoop)
    {
        m_eventLoop->quit();
    }
}

/**
 * @brief  Runs periodic device enumeration in the worker thread
 *
 * @retval None
 */
void DeviceWorker::run()
{
    qDebug() << "[DeviceWorker] Device worker is starting up...";

    m_timer = new QTimer();
    m_eventLoop = new QEventLoop();

    connect(m_timer, &QTimer::timeout, this, &DeviceWorker::onTimerTimeout);

    m_timer->start(5000);

    enumerateDevices();

    m_eventLoop->exec();

    qDebug() << "[DeviceWorker] Device worker is shutting down...";
}

/* Protected Methods --------------------------------------------------------*/

/* Private Methods ----------------------------------------------------------*/

/**
 * @brief  Enumerates devices when the polling timer expires
 *
 * @retval None
 */
void DeviceWorker::onTimerTimeout()
{
    enumerateDevices();
}

/**
 * @brief  Enumerates matching HID devices and emits the updated device list
 *
 * @retval None
 */
void DeviceWorker::enumerateDevices()
{
    hid_device_info *devs = hid_enumerate(m_vendorId, m_productId);

    QList<Device> newDevices;

    for (hid_device_info *dev = devs; dev; dev = dev->next)
    {
        newDevices.append(Device(dev));
    }

    hid_free_enumeration(devs);

    emit devicesChanged(newDevices);
}

/* External Callbacks / Event Handlers --------------------------------------*/
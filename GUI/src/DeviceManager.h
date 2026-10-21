/**
 ******************************************************************************
 * @file    DeviceManager.h
 * @brief   Header for DeviceManager.cpp
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
#ifndef __DEVICEMANAGER_H__
#define __DEVICEMANAGER_H__

/* Includes ------------------------------------------------------------------*/
#include "Device.h"
#include "DeviceWorker.h"
#include "ReportWorker.h"
#include <QList>
#include <QObject>
#include <QTimer>
#include <QVariantList>
#include <QtQml/qqmlregistration.h>

/* Forward Declarations ------------------------------------------------------*/

/* Class Definition ----------------------------------------------------------*/
class DeviceManager : public QObject
{
    /* Meta-Object Macros -----------------------------------------------------*/
    Q_OBJECT
    QML_SINGLETON
    QML_ELEMENT

    /* Properties ------------------------------------------------------------*/
    Q_PROPERTY(QList<Device> devices READ devices NOTIFY devicesChanged)
    Q_PROPERTY(
        int selectedDeviceIndex READ selectedDeviceIndex WRITE setSelectedDeviceIndex NOTIFY selectedDeviceIndexChanged)

  public:
    /* Enums & Type Aliases --------------------------------------------------*/

    /* Constructors & Destructor ---------------------------------------------*/
    static DeviceManager *instance();

    explicit DeviceManager(QObject *parent = nullptr);
    ~DeviceManager();

    /* Signals ---------------------------------------------------------------*/

    /* Public Slots ----------------------------------------------------------*/

    /* Public Methods --------------------------------------------------------*/
    QList<Device> devices() const;

    int selectedDeviceIndex() const;
    void setSelectedDeviceIndex(int newIndex);

    /* Protected Methods -----------------------------------------------------*/

    /* Protected Slots -------------------------------------------------------*/

    /* Signals ---------------------------------------------------------------*/
  signals:
    void devicesChanged(QList<Device> newDevices);
    void selectedDeviceIndexChanged(int newIndex);
    void deviceStateReportReceived(DeviceStatusResponse_t report);
    void rsqStatusReportReceived(RSQStatusResponse_t report);
    void rdsProgrammeServiceReportReceived(RDSProgrammeServiceReport_t report);
    void rdsRadioTextReportReceived(RDSRadioTextReport_t report);

    /* Public Slots ----------------------------------------------------------*/
  public slots:
    void onDevicesChanged(QList<Device> newDevices);
    void onErrorThresholdExceeded();
    void beginSeek(bool seekUp);
    void tuneToFrequency(uint16_t frequency);

  private slots:
    /* Private Slots ---------------------------------------------------------*/
    void onSelectedDeviceIndexChanged(int newIndex);

  private:
    /* Private Methods -------------------------------------------------------*/

    /* Private Members -------------------------------------------------------*/
    QList<Device> m_devices;
    DeviceWorker *m_deviceWorker;
    ReportWorker *m_reportWorker;
    hid_device *m_currentDevice;

    int m_selectedDeviceIndex;

    static DeviceManager *s_instance;
};

#endif // __DEVICEMANAGER_H__
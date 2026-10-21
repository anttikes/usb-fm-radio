/**
 ******************************************************************************
 * @file    ReportWorker.h
 * @brief   Header for ReportWorker.cpp
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
#ifndef __REPORTWORKER_H__
#define __REPORTWORKER_H__

/* Includes ------------------------------------------------------------------*/
#include "Device.h"
#include <QEventLoop>
#include <QFile>
#include <QObject>
#include <QRunnable>
#include <QTimer>
#include <Reports.h>

/* Forward Declarations ------------------------------------------------------*/

/* Class Definition ----------------------------------------------------------*/
class ReportWorker : public QObject, public QRunnable
{
    /* Meta-Object Macro -----------------------------------------------------*/
    Q_OBJECT

  public:
    /* Enums & Type Aliases --------------------------------------------------*/

    /* Constructors & Destructor ---------------------------------------------*/
    explicit ReportWorker(hid_device *selectedDevice);
    ~ReportWorker();

    /* Signals ---------------------------------------------------------------*/

    /* Public Slots ----------------------------------------------------------*/

    /* Public Methods --------------------------------------------------------*/
    void run() override;
    void stop();

    inline bool isStopped() const
    {
        return m_stopped;
    }

    /* Protected Methods -----------------------------------------------------*/

    /* Protected Slots -------------------------------------------------------*/

    /* Signals ---------------------------------------------------------------*/
  signals:
    void deviceStateReportReceived(DeviceStatusResponse_t report);
    void rsqStatusReportReceived(RSQStatusResponse_t report);
    void rdsProgrammeServiceReportReceived(RDSProgrammeServiceReport_t report);
    void rdsRadioTextReportReceived(RDSRadioTextReport_t report);
    void errorThresholdExceeded();

  private:
    /* Private Methods -------------------------------------------------------*/
    void pollReports();

    /* Private Members -------------------------------------------------------*/
    hid_device *m_selectedDevice;
    QFile m_signalQualityLog;

    bool m_shouldStop;
    bool m_stopped;
};

#endif // __REPORTWORKER_H__

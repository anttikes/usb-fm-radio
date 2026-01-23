#ifndef __REPORTWORKER_H__
#define __REPORTWORKER_H__

#include "Device.h"
#include "Reports.h"
#include <QEventLoop>
#include <QObject>
#include <QRunnable>
#include <QTimer>

class ReportWorker : public QObject, public QRunnable
{
    Q_OBJECT

  public:
    explicit ReportWorker(hid_device *selectedDevice);
    ~ReportWorker();

    void run() override;
    void stop();

  private:
    void pollReports();

  signals:
    void rsqStatusReportReceived(RSQStatusReport report);

  private slots:
    void onTimerTimeout();

  private:
    hid_device *m_selectedDevice;
    QEventLoop *m_eventLoop;
    QTimer *m_timer;
};

#endif // __REPORTWORKER_H__

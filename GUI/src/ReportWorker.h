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
    void processRadioStateReport(unsigned char buf[32]);
    void processRSQStatusReport(unsigned char buf[32]);
    void stop();

    inline bool isStopped() const
    {
        return m_stopped;
    }

  private:
    void pollReports();

  signals:
    void radioStateReportReceived(RadioStateReport report);
    void rsqStatusReportReceived(RSQStatusReport report);

  private:
    bool m_shouldStop;
    bool m_stopped;
    hid_device *m_selectedDevice;
};

#endif // __REPORTWORKER_H__

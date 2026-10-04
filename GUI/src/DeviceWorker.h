#ifndef __DEVICEWORKER_H__
#define __DEVICEWORKER_H__

#include "Device.h"
#include <QEventLoop>
#include <QObject>
#include <QRunnable>
#include <QTimer>

class DeviceWorker : public QObject, public QRunnable
{
    Q_OBJECT

  public:
    explicit DeviceWorker(unsigned short vendorId, unsigned short productId);
    ~DeviceWorker();

    void run() override;
    void stop();

  private:
    void enumerateDevices();

  signals:
    void devicesChanged(QList<Device> newDevices);

  private slots:
    void onTimerTimeout();

  private:
    QEventLoop *m_eventLoop;
    QTimer *m_timer;

    unsigned short m_vendorId;
    unsigned short m_productId;
};

#endif // __DEVICEWORKER_H__

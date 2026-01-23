#include "DeviceWorker.h"
#include <QDebug>

DeviceWorker::DeviceWorker() : QRunnable(), m_eventLoop(nullptr), m_timer(nullptr)
{
}

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

void DeviceWorker::stop()
{
    if (m_eventLoop)
    {
        m_eventLoop->quit();
    }
}

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

void DeviceWorker::onTimerTimeout()
{
    enumerateDevices();
}

void DeviceWorker::enumerateDevices()
{
    hid_device_info *devs = hid_enumerate(0x0483, 0x5740);

    QList<Device> newDevices;

    for (hid_device_info *dev = devs; dev; dev = dev->next)
    {
        newDevices.append(Device(dev));
    }

    hid_free_enumeration(devs);

    emit devicesChanged(newDevices);
}
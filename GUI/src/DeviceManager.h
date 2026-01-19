#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include "Device.h"
#include "DeviceWorker.h"
#include "Reports.h"
#include <QList>
#include <QObject>
#include <QTimer>
#include <QVariantList>
#include <QtQml/qqmlregistration.h>

class DeviceManager : public QObject
{
    Q_OBJECT
    QML_SINGLETON
    QML_ELEMENT

    Q_PROPERTY(QList<Device> devices READ devices NOTIFY devicesChanged)
    Q_PROPERTY(
        int selectedDeviceIndex READ selectedDeviceIndex WRITE setSelectedDeviceIndex NOTIFY selectedDeviceIndexChanged)

  public:
    static DeviceManager *instance();

    explicit DeviceManager(QObject *parent = nullptr);
    ~DeviceManager();

    QList<Device> devices() const;

    int selectedDeviceIndex() const;
    void setSelectedDeviceIndex(int newIndex);

  signals:
    void devicesChanged(QList<Device> newDevices);
    void selectedDeviceIndexChanged(int newIndex);
    void rsqStatusReportReceived(RSQStatusReport report);

  public slots:
    void onDevicesChanged(QList<Device> newDevices);

  private slots:
    void onSelectedDeviceIndexChanged(int newIndex);
    void onReportPollerTimerTimeout();

  private:
    int m_selectedDeviceIndex;
    QList<Device> m_devices;
    DeviceWorker *m_deviceWorker;
    QTimer m_reportPollerTimer;
    hid_device *m_currentDevice;
    static DeviceManager *s_instance;
};

#endif // DEVICEMANAGER_H
#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include "Device.h"
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

    Q_PROPERTY(QList<Device *> devices READ devices NOTIFY devicesChanged)
    Q_PROPERTY(
        int selectedDeviceIndex READ selectedDeviceIndex WRITE setSelectedDeviceIndex NOTIFY selectedDeviceIndexChanged)

  public:
    static DeviceManager *instance();

    explicit DeviceManager(QObject *parent = nullptr);
    ~DeviceManager();

    QList<Device *> devices() const;

    int selectedDeviceIndex() const;
    void setSelectedDeviceIndex(int newIndex);

    Device *selectedDevice() const;

  signals:
    void devicesChanged(QList<Device *> newDevices);
    void selectedDeviceIndexChanged(int newIndex);

    void rsqStatusReportReceived(RSQStatusReport report);

  public slots:
    void enumerateDevices();

  private slots:
    void onDeviceDetectionTimerTimeout();
    void onSelectedDeviceIndexChanged(int newIndex);
    void onReportPollerTimerTimeout();

  private:
    QTimer m_deviceDetectionTimer;
    QTimer m_reportPollerTimer;
    QList<Device *> m_devices;
    int m_selectedDeviceIndex;
    hid_device *m_currentDevice;
    static DeviceManager *s_instance;
};

#endif // DEVICEMANAGER_H
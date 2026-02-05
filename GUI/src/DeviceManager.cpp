#include "DeviceManager.h"
#include <QDebug>
#include <QThreadPool>
#include <hidapi.h>

DeviceManager *DeviceManager::s_instance = nullptr;

DeviceManager::DeviceManager(QObject *parent)
    : QObject(parent), m_deviceWorker(nullptr), m_reportWorker(nullptr), m_currentDevice(nullptr),
      m_selectedDeviceIndex(-1)
{
    s_instance = this;

    m_deviceWorker = new DeviceWorker();

    connect(m_deviceWorker, &DeviceWorker::devicesChanged, this, &DeviceManager::onDevicesChanged);

    qDebug() << "[DeviceManager] Starting the device worker";

    QThreadPool::globalInstance()->start(m_deviceWorker);

    connect(this,
            &DeviceManager::selectedDeviceIndexChanged,
            this,
            &DeviceManager::onSelectedDeviceIndexChanged,
            Qt::QueuedConnection);
}

DeviceManager::~DeviceManager()
{
    m_devices.clear();

    if (m_deviceWorker)
    {
        m_deviceWorker->stop();
        // The worker is auto-deleted by the thread pool
    }

    if (m_reportWorker)
    {
        m_reportWorker->stop();
        // The worker is auto-deleted by the thread pool
    }

    if (m_currentDevice)
    {
        hid_close(m_currentDevice);
        m_currentDevice = nullptr;
    }
}

DeviceManager *DeviceManager::instance()
{
    return s_instance;
}

QList<Device> DeviceManager::devices() const
{
    return m_devices;
}

int DeviceManager::selectedDeviceIndex() const
{
    return m_selectedDeviceIndex;
}

void DeviceManager::setSelectedDeviceIndex(int newIndex)
{
    if (newIndex > m_devices.size())
    {
        return;
    }

    if (newIndex != m_selectedDeviceIndex)
    {
        m_selectedDeviceIndex = newIndex;

        emit selectedDeviceIndexChanged(m_selectedDeviceIndex);
    }
}

void DeviceManager::onDevicesChanged(QList<Device> newDevices)
{
    QString currentPath;
    if (m_selectedDeviceIndex != -1)
    {
        currentPath = m_devices[m_selectedDeviceIndex].path();
    }

    m_devices.clear();
    m_devices.append(newDevices);

    // Auto-choose the previously selected device if it is still present
    int newIndex = -1;
    if (!currentPath.isEmpty())
    {
        for (int i = 0; i < m_devices.size(); ++i)
        {
            if (m_devices[i].path() == currentPath)
            {
                newIndex = i;
                break;
            }
        }
    }

    // If not, auto-choose the first device
    if (newIndex == -1 && m_devices.size() == 1)
    {
        newIndex = 0;
    }

    // If a choice was made, determine if we need to announce it
    if (newIndex > -1)
    {
        m_selectedDeviceIndex = newIndex;

        QString newPath = m_devices[newIndex].path();

        if (newPath != currentPath)
        {
            qDebug() << "[DeviceManager] A new device has been selected";

            emit selectedDeviceIndexChanged(newIndex);
        }
    }
    else if (m_selectedDeviceIndex != -1)
    {
        // Previously selected device is now gone
        m_selectedDeviceIndex = -1;

        qDebug() << "[DeviceManager] Currently selected device was no longer found, unselecting it";

        emit selectedDeviceIndexChanged(m_selectedDeviceIndex);
    }
}

void DeviceManager::onSelectedDeviceIndexChanged(int newIndex)
{
    if (newIndex == -1)
    {
        if (m_reportWorker)
        {
            m_reportWorker->stop();
            m_reportWorker = nullptr;
        }

        if (m_currentDevice)
        {
            hid_close(m_currentDevice);
            m_currentDevice = nullptr;
        }
    }
    else
    {
        auto selectedDevice = m_devices[newIndex];

        m_currentDevice = hid_open(selectedDevice.vendorId(),
                                   selectedDevice.productId(),
                                   (const wchar_t *)selectedDevice.serialNumber().utf16());

        if (m_currentDevice)
        {
            m_reportWorker = new ReportWorker(m_currentDevice);

            connect(
                m_reportWorker, &ReportWorker::rsqStatusReportReceived, this, &DeviceManager::rsqStatusReportReceived);
            connect(m_reportWorker,
                    &ReportWorker::radioStateReportReceived,
                    this,
                    &DeviceManager::radioStateReportReceived);

            qDebug() << "[DeviceManager] Starting the report worker";

            QThreadPool::globalInstance()->start(m_reportWorker);
        }
        else
        {
            QString error = QString::fromWCharArray(hid_error(NULL));

            qDebug() << "[DeviceManager] Failed to open a HID device" << error << ". Retrying...";

            // Try to reopen the device after a short delay
            QTimer::singleShot(1000, this, [this, newIndex]() { this->onSelectedDeviceIndexChanged(newIndex); });
        }
    }
}

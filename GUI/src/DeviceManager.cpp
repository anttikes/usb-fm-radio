#include "DeviceManager.h"
#include <QDebug>
#include <QThreadPool>
#include <hidapi.h>

DeviceManager *DeviceManager::s_instance = nullptr;

DeviceManager::DeviceManager(QObject *parent)
    : QObject(parent), m_deviceWorker(nullptr), m_reportPollerTimer(this), m_currentDevice(nullptr),
      m_selectedDeviceIndex(-1)
{
    s_instance = this;

    m_deviceWorker = new DeviceWorker();

    connect(m_deviceWorker, &DeviceWorker::devicesChanged, this, &DeviceManager::onDevicesChanged);

    QThreadPool::globalInstance()->start(m_deviceWorker);

    connect(
        &m_reportPollerTimer, &QTimer::timeout, this, &DeviceManager::onReportPollerTimerTimeout, Qt::QueuedConnection);
    connect(this, &DeviceManager::selectedDeviceIndexChanged, this, &DeviceManager::onSelectedDeviceIndexChanged);
}

DeviceManager::~DeviceManager()
{
    m_devices.clear();

    if (m_deviceWorker)
    {
        m_deviceWorker->stop();
        // The worker is auto-deleted by the thread pool
    }

    m_reportPollerTimer.stop();

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
            emit selectedDeviceIndexChanged(newIndex);
        }
    }
    else if (m_selectedDeviceIndex != -1)
    {
        // Previously selected device is now gone
        m_selectedDeviceIndex = -1;

        emit selectedDeviceIndexChanged(m_selectedDeviceIndex);
    }
}

void DeviceManager::onSelectedDeviceIndexChanged(int newIndex)
{
    if (newIndex == -1)
    {
        m_reportPollerTimer.stop();

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
            m_reportPollerTimer.start(500);
        }
        else
        {
            QString error = QString::fromWCharArray(hid_error(NULL));

            qDebug() << "[DeviceManager] Failed to open a HID device" << error;

            // Try to reopen the device after a short delay
            QTimer::singleShot(1000, this, [this, newIndex]() { this->onSelectedDeviceIndexChanged(newIndex); });
        }
    }
}

void DeviceManager::onReportPollerTimerTimeout()
{
    unsigned char buf[32];

    int res = hid_read_timeout(m_currentDevice, buf, sizeof(buf), 250);
    if (res > 0)
    {
        // buf[0] contains the opCode of the report; see the CommandIdentifiers_t in firmware side
        CommandIdentifiers_t opCode = static_cast<CommandIdentifiers_t>(buf[0]);

        // buf[1] contains the report size; not used here
        // buf[2] contains the status byte; not used here

        switch (opCode)
        {
        case CommandIdentifiers_t::CMD_ID_FM_RSQ_STATUS: {
            RSQStatusReport report;

            report.blendInterrupt = buf[3] & 0x80;
            report.multHiInterrupt = buf[3] & 0x20;
            report.multLoInterrupt = buf[3] & 0x10;
            report.snrHiInterrupt = buf[3] & 0x08;
            report.snrLoInterrupt = buf[3] & 0x04;
            report.rssiHiInterrupt = buf[3] & 0x02;
            report.rssiLoInterrupt = buf[3] & 0x01;

            report.softMute = buf[4] & 0x08;
            report.afcRail = buf[4] & 0x02;
            report.validChannel = buf[4] & 0x01;

            report.pilot = buf[5] & 0x80;
            report.stereoBlend = buf[5] & 0x7F;

            report.receivedSignalStrength = buf[6];
            report.signalToNoiseRatio = buf[7];
            report.multipath = buf[8];
            report.frequencyOffset = (int8_t)buf[9];

            emit rsqStatusReportReceived(report);

            break;
        }
        }
    }
    else if (res < 0)
    {
        QString error = QString::fromWCharArray(hid_error(m_currentDevice));

        qDebug() << "[DeviceManager]: Error during HID read" << error;
    }
}
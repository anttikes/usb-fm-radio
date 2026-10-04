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

    // This worker looks for radio devices that have already been programmed
    m_deviceWorker = new DeviceWorker(0x0483, 0x5740);

    connect(m_deviceWorker, &DeviceWorker::devicesChanged, this, &DeviceManager::onDevicesChanged);

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

    if (m_reportWorker)
    {
        m_reportWorker->stop();

        while (!m_reportWorker->isStopped())
        {
            QThread::msleep(50);
        }

        m_reportWorker = nullptr;
    }

    if (m_deviceWorker)
    {
        m_deviceWorker->stop();
        m_deviceWorker = nullptr;
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

void DeviceManager::onErrorThresholdExceeded()
{
    qDebug() << "[DeviceManager] Report worker error threshold exceeded. Unselecting current device.";

    m_selectedDeviceIndex = -1;

    emit selectedDeviceIndexChanged(m_selectedDeviceIndex);
}

void DeviceManager::onSelectedDeviceIndexChanged(int newIndex)
{
    if (newIndex == -1)
    {
        if (m_reportWorker)
        {
            m_reportWorker->stop();

            while (!m_reportWorker->isStopped())
            {
                QThread::msleep(50);
            }

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
        Device selectedDevice = m_devices[newIndex];

        m_currentDevice = hid_open(selectedDevice.vendorId(),
                                   selectedDevice.productId(),
                                   (const wchar_t *)selectedDevice.serialNumber().utf16());

        if (m_currentDevice)
        {
            m_reportWorker = new ReportWorker(m_currentDevice);

            connect(
                m_reportWorker, &ReportWorker::rsqStatusReportReceived, this, &DeviceManager::rsqStatusReportReceived);
            connect(m_reportWorker,
                    &ReportWorker::deviceStateReportReceived,
                    this,
                    &DeviceManager::deviceStateReportReceived);

            connect(
                m_reportWorker, &ReportWorker::errorThresholdExceeded, this, &DeviceManager::onErrorThresholdExceeded);

            connect(m_reportWorker,
                    &ReportWorker::rdsProgrammeServiceReportReceived,
                    this,
                    &DeviceManager::rdsProgrammeServiceReportReceived);

            connect(m_reportWorker,
                    &ReportWorker::rdsRadioTextReportReceived,
                    this,
                    &DeviceManager::rdsRadioTextReportReceived);

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

void DeviceManager::beginSeek(bool seekUp)
{
    if (m_currentDevice)
    {
        qDebug() << "[DeviceManager]: Requesting seek up/down";

        uint8_t buf[MAX_REPORT_SIZE] = {0};

        buf[0] = 0x00; // Report ID; not used currently
        buf[1] = REPORT_IDENTIFIER_SEEK_START;
        buf[2] = 0x01; // Always wrap
        buf[3] = seekUp ? 0x01 : 0x00;

        int res = hid_write(m_currentDevice, buf, sizeof(buf));
        if (res < 0)
        {
            QString error = QString::fromWCharArray(hid_error(m_currentDevice));

            qDebug() << "[DeviceManager]: Error during HID write" << error;
        }
    }
    else
    {
        qDebug() << "[DeviceManager]: No device is currently selected; cannot send seek command.";
    }
}

void DeviceManager::tuneToFrequency(uint16_t frequency)
{
    if (m_currentDevice)
    {
        qDebug() << "[DeviceManager]: Requesting tune to frequency" << frequency;

        uint8_t buf[MAX_REPORT_SIZE] = {0};

        buf[0] = 0x00; // Report ID; not used currently
        buf[1] = REPORT_IDENTIFIER_TUNE_FREQ;
        buf[2] = frequency & 0xFF;        // Low byte of frequency
        buf[3] = (frequency >> 8) & 0xFF; // High byte of frequency

        int res = hid_write(m_currentDevice, buf, sizeof(buf));
        if (res < 0)
        {
            QString error = QString::fromWCharArray(hid_error(m_currentDevice));

            qDebug() << "[DeviceManager]: Error during HID write" << error;
        }
    }
    else
    {
        qDebug() << "[DeviceManager]: No device is currently selected; cannot send tune command.";
    }
}

#include "ReportWorker.h"
#include <QDebug>

ReportWorker::ReportWorker(hid_device *selectedDevice)
    : QRunnable(), m_eventLoop(nullptr), m_timer(nullptr), m_selectedDevice(selectedDevice)
{
}

ReportWorker::~ReportWorker()
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

void ReportWorker::stop()
{
    if (m_eventLoop)
    {
        m_eventLoop->quit();
    }
}

void ReportWorker::run()
{
    qDebug() << "[ReportWorker] Report worker is starting up...";

    m_timer = new QTimer();
    m_eventLoop = new QEventLoop();

    connect(m_timer, &QTimer::timeout, this, &ReportWorker::onTimerTimeout);

    m_timer->start(5000);

    pollReports();

    m_eventLoop->exec();

    qDebug() << "[ReportWorker] Report worker is shutting down...";
}

void ReportWorker::onTimerTimeout()
{
    pollReports();
}

void ReportWorker::pollReports()
{
    unsigned char buf[32];

    int res = hid_read_timeout(m_selectedDevice, buf, sizeof(buf), 250);
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
        case CommandIdentifiers_t::CMD_ID_RADIO_STATUS_REPORT: {
            RadioStateReport report;

            report.currentState = buf[2];

            // The frequency is sent from the device as two bytes measured in units of 10 kHz
            uint16_t combinedFrequency = (uint16_t)((buf[3] << 8) | (buf[4] << 0));
            report.currentFrequency = combinedFrequency / 100.0;

            emit radioStateReportReceived(report);

            break;
        }
        }
    }
    else if (res < 0)
    {
        QString error = QString::fromWCharArray(hid_error(m_selectedDevice));

        qDebug() << "[DeviceManager]: Error during HID read" << error;
    }
}
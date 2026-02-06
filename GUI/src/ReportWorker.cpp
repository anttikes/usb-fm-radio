#include "ReportWorker.h"
#include <QDebug>

ReportWorker::ReportWorker(hid_device *selectedDevice)
    : QRunnable(), m_selectedDevice(selectedDevice), m_shouldStop(false), m_stopped(false)
{
}

ReportWorker::~ReportWorker()
{
    stop();
}

void ReportWorker::stop()
{
    m_shouldStop = true;
}

void ReportWorker::run()
{
    qDebug() << "[ReportWorker] Report worker is starting up...";

    while (!m_shouldStop)
    {
        unsigned char buf[32];

        int res = hid_read_timeout(m_selectedDevice, buf, sizeof(buf), 250);
        if (res > 0)
        {
            // buf[0] contains the opCode of the report; see the CommandIdentifiers_t in firmware side
            CommandIdentifiers_t opCode = static_cast<CommandIdentifiers_t>(buf[0]);

            // buf[1] contains the report size; not used here
            // In some cases, buf[2] contains the status byte; not used here

            switch (opCode)
            {
            case CommandIdentifiers_t::CMD_ID_FM_RSQ_STATUS: {
                processRSQStatusReport(buf);

                break;
            }
            case CommandIdentifiers_t::CMD_ID_RADIO_STATUS_REPORT: {
                processRadioStateReport(buf);

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

    qDebug() << "[ReportWorker] Report worker is shutting down...";

    m_stopped = true;
}

void ReportWorker::processRadioStateReport(unsigned char buf[32])
{
    RadioStateReport report;

    report.currentState = buf[2];

    // The frequency is sent from the device as two bytes measured in units of 10 kHz
    uint16_t combinedFrequency = (uint16_t)((buf[3] << 8) | (buf[4] << 0));
    report.currentFrequency = combinedFrequency / 100.0;

    emit radioStateReportReceived(report);
}

void ReportWorker::processRSQStatusReport(unsigned char buf[32])
{
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
}

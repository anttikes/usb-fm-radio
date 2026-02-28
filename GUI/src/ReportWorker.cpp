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
        unsigned char buf[MAX_REPORT_SIZE];

        int res = hid_read_timeout(m_selectedDevice, buf, sizeof(buf), 250);
        if (res > 0)
        {
            // buf[0] contains the identifier of the report
            ReportIdentifier_t identifier = (ReportIdentifier_t)buf[0];

            switch (identifier)
            {
            case REPORT_IDENTIFIER_RADIO_STATUS: {
                RadioStatusResponse_t report;
                std::memcpy(&report, &buf[1], sizeof(RadioStatusResponse_t));

                emit radioStateReportReceived(report);

                break;
            }
            case REPORT_IDENTIFIER_RSQ_STATUS: {
                RSQStatusResponse_t report;
                std::memcpy(&report, &buf[1], sizeof(RSQStatusResponse_t));

                emit rsqStatusReportReceived(report);

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

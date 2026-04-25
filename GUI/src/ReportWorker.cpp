#include "ReportWorker.h"
#include <QDebug>
#include <QThread>

ReportWorker::ReportWorker(hid_device *selectedDevice)
    : QRunnable(), m_signalQualityLog("signal_quality_log.csv"), m_selectedDevice(selectedDevice), m_shouldStop(false),
      m_stopped(false)
{
    m_signalQualityLog.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
}

ReportWorker::~ReportWorker()
{
    stop();
}

void ReportWorker::stop()
{
    m_signalQualityLog.close();
    m_shouldStop = true;
}

void ReportWorker::run()
{
    qDebug() << "[ReportWorker] Report worker is starting up...";

    uint8_t errorCount = 0;
    double frequency = 0.0;

    while (!m_shouldStop)
    {
        if (errorCount >= 5)
        {
            // If we have errored out, just wait for the loop to end
            QThread::msleep(250);
            continue;
        }

        uint8_t buf[MAX_REPORT_SIZE];

        int res = hid_read_timeout(m_selectedDevice, buf, sizeof(buf), 250);
        if (res > 0)
        {
            errorCount = 0;

            // buf[0] contains the identifier of the report
            ReportIdentifier_t identifier = (ReportIdentifier_t)buf[0];

            switch (identifier)
            {
            case REPORT_IDENTIFIER_RADIO_STATUS: {
                RadioStatusResponse_t report;
                std::memcpy(&report, &buf[1], sizeof(RadioStatusResponse_t));

                double newFrequency = (double)(report.currentFrequency / 100.0);
                if (newFrequency != frequency)
                {
                    if (m_signalQualityLog.isOpen() && frequency > 0.0)
                    {
                        m_signalQualityLog.write("\n");
                    }

                    frequency = newFrequency;
                }

                emit radioStateReportReceived(report);

                break;
            }
            case REPORT_IDENTIFIER_RSQ_STATUS: {
                RSQStatusResponse_t report;
                std::memcpy(&report, &buf[1], sizeof(RSQStatusResponse_t));

                if (m_signalQualityLog.isOpen() && frequency > 0.0)
                {
                    QString logEntry = QString("%1 MHz\tRSSI: %2 dBuV\tNoise ratio: %3 dB\tStereo pilot detected: "
                                               "%4\tStereo blend: %5 %\tMultipath: %6\n")
                                           .arg(frequency, 0, 'f', 1)
                                           .arg(report.rssi)
                                           .arg(report.snr)
                                           .arg(report.pilot ? "Yes" : "No")
                                           .arg(report.stereoBlend)
                                           .arg(report.multipath);

                    m_signalQualityLog.write(logEntry.toUtf8());
                }

                emit rsqStatusReportReceived(report);

                break;
            }
            }
        }
        else if (res < 0)
        {
            QString error = QString::fromWCharArray(hid_error(m_selectedDevice));

            qDebug() << "[ReportWorker]: Error during HID read" << error;

            errorCount++;
        }

        if (errorCount >= 5)
        {
            qDebug() << "[ReportWorker]: Too many errors during HID read; signalling DeviceManager to reset.";

            emit disconnectCurrentDevice();
        }
    }

    qDebug() << "[ReportWorker] Report worker is shutting down...";

    m_stopped = true;
}

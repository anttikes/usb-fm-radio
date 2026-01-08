#include "tunercontrol.h"
#include <QDebug>
#include <hidapi.h>

CTunerControl::CTunerControl(QObject *parent) : QObject(parent)
{
    m_bInitialized = false;
}

CTunerControl::~CTunerControl()
{
    deInitialize();
}

bool CTunerControl::initialize()
{
    int res = hid_init();

    if (res < 0)
    {
        QString error = QString::fromWCharArray(hid_error(NULL));

        qDebug() << "Failed to initialize hidapi: " << error;

        return false;
    }

    m_bInitialized = true;

    return true;
}

void CTunerControl::deInitialize()
{
    int res = hid_exit();

    if (res < 0)
    {
        QString error = QString::fromWCharArray(hid_error(NULL));

        qDebug() << "Failed to de-initialize hidapi: " << error;
    }

    m_bInitialized = false;
}

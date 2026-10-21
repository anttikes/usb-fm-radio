/**
 ******************************************************************************
 * @file    main.cpp
 * @brief   Application entry point and HIDAPI lifecycle functions
 ******************************************************************************
 * @attention
 *
 * Copyright (c) Antti Keskinen
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "DeviceManager.h"
#include <QDebug>
#include <QDirIterator>
#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

/* Forward Declarations ------------------------------------------------------*/
bool hidapi_initialize();
void hidapi_deinitialize();

/* Main Entry Point ----------------------------------------------------------*/
int main(int argc, char *argv[])
{
    /* Not sure where an embedded resource (QML or otherwise) went? This will help you find it. */
    /*
    QDirIterator it(":", QDirIterator::Subdirectories);

    qDebug() << "--- Listing Embedded Resources ---";
    while (it.hasNext())
    {
        qDebug() << it.next();
    }
    qDebug() << "----------------------------------";
    */

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine(&app);

    QQuickStyle::setStyle("Basic");

    app.setWindowIcon(QIcon(":/qt/qml/USBRadio/Components/applicationIcon.png"));

    engine.loadFromModule("USBRadio.Components", "Main");

    if (hidapi_initialize() == false)
    {
        return -1;
    }

    int retVal = app.exec();

    hidapi_deinitialize();

    return retVal;
}

/* HIDAPI Lifecycle Functions ------------------------------------------------*/
bool hidapi_initialize()
{
    int res = hid_init();

    if (res < 0)
    {
        QString error = QString::fromWCharArray(hid_error(NULL));

        qDebug() << "Failed to initialize hidapi: " << error;

        return false;
    }

    return true;
}

void hidapi_deinitialize()
{
    int res = hid_exit();

    if (res < 0)
    {
        QString error = QString::fromWCharArray(hid_error(NULL));

        qDebug() << "Failed to de-initialize hidapi: " << error;
    }
}

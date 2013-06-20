/* 
 * 
 * iviLINK SDK, version 1.1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012, Luxoft Professional Corp., member of IBS group
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; version 2.1.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * 
 * 
 */


#ifndef ANDROID

#include <QApplication>
#include <QDesktopWidget>
#include <QDeclarativeContext>
#include "qmlapplicationviewer.h"
#include <QWidget>
#include <QRect>
#include "Logger.hpp"

#include "QMLDeviceManagerDialog.hpp"
#include "DeviceManager.hpp"

using namespace iviLink::DeviceManager;

int main(int argc, char *argv[])
{
    Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("applications.DeviceManager"));
    PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));

    LOG4CPLUS_INFO(logger, "DeviceManager main()");

    QApplication app(argc, argv);

    app.setApplicationName("DeviceManager");
    app.setQuitOnLastWindowClosed(true);
    QDeclarativeView  viewer;

    viewer.setAttribute(Qt::WA_TranslucentBackground);
    viewer.setAttribute(Qt::WA_NoSystemBackground);
    viewer.setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    viewer.setStyleSheet("background:transparent;");
    // show the application
    viewer.move(QApplication::desktop()->screen()->rect().center() - viewer.rect().center() );

    QMLDeviceManagerDialog * dialog = new QMLDeviceManagerDialog();
    DeviceManager* deviceManager = new DeviceManager( PREFIX "/database/" );

    viewer.rootContext()->setContextProperty("QMLDeviceManagerDialog", dialog);
    viewer.setSource(QUrl("qrc:///qml/DeviceManager/devicemanager.qml"));

    dialog->setDeviceManager(deviceManager);
    deviceManager->setGUI(dialog);

    viewer.show();

    dialog->init();
    deviceManager->start();

    return app.exec();

}
#endif //ANDROID

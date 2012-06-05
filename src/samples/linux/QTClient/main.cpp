/* 
 *  iviLINK SDK, version 0.9 (for preview only)                                      
 *    http://www.ivilink.net                                                         
 *  Cross Platform Application Communication Stack for In-Vehicle Applications       
 *                                                                                   
 *  Copyright (C) 2012, Luxoft Professional Corp., member of IBS group               
 *                                                                                   
 *  This library is free software; you can redistribute it and/or                    
 *  modify it under the terms of the GNU Lesser General Public                       
 *  License as published by the Free Software Foundation; version 2.1.               
 *                                                                                   
 *  This library is distributed in the hope that it will be useful,                  
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                
 *  Lesser General Public License for more details.                                  
 *                                                                                   
 *  You should have received a copy of the GNU Lesser General Public                 
 *  License along with this library; if not, write to the Free Software              
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA   
 * 
 * 
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <sys/un.h>
#include <pthread.h>

#include <QtGui/QApplication>
#include <QVariant>
#include <QDeclarativeEngine>
#include <QDeclarativeComponent>
#include <QDeclarativeContext>
#include <QObject>
#include <QGraphicsObject>
#include <QDebug>
#include "qmlapplicationviewer.h"
#include <qcoreapplication.h>

#include "CBtnProcessor.hpp"
#include "common.h"
#include "CStateUpdater.hpp"
#include "VideoContainer.h"
#include "utils/misc/include/Logger.hpp"
#include "utils/misc/include/logging.hpp"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("QTCLient.main"));

    PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));

    LOG4CPLUS_INFO(logger, "QTCLient Process started...");
    Log("QTCLient Process started...");

    //current state. to be moved to qml

    AirCondState currentState;
    // set default
    currentState.airRecirc = false;
    currentState.autoClimate = true;
    currentState.driverTemp = 20;
    currentState.dualClimate = false;
    currentState.fanLow = false;
    currentState.fanMiddle = false;
    currentState.fanTop = false;
    currentState.fanSpeed = 5;
    currentState.frontWindow = false;
    currentState.isAC = true;
    currentState.isOFF = false;
    currentState.passTemp = 20;
    currentState.rearWindow = false;

    CBtnProcessor * btnProcessor = new CBtnProcessor(); //(sockfd);

    QScopedPointer<QApplication> app(createApplication(argc, argv));

    app->setApplicationName("PhononTest");
    app->setQuitOnLastWindowClosed(true);

    qmlRegisterType<VideoContainer>("VideoContainer", 1, 0, "VideoContainer");

    QDeclarativeView view;
    view.setResizeMode(QDeclarativeView::SizeRootObjectToView);
    view.rootContext()->setContextProperty("BtnProcessor", btnProcessor);
    view.setSource(QUrl::fromLocalFile("qml/test06_seat_app/main.qml"));
    view.setGeometry(100, 100, 800, 480);
    view.setCursor(Qt::PointingHandCursor);
    view.show();

    QObject *rootObject = dynamic_cast<QObject *>(view.rootObject());

    VideoContainer * mContainer=(rootObject->findChild<VideoContainer*>("videoContainer"));
    btnProcessor->setContainer(mContainer);

    btnProcessor->setRootObject(rootObject);

    CStateUpdater Updater(btnProcessor,rootObject );

    return app->exec();
}

/* 
 * 
 * iviLINK SDK, version 1.0.1
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





#include <QtGui/QApplication>
#include <QDeclarativeContext>

#include "qmlapplicationviewer.h"
#include "crequestprocessor.h"
#include "cstateupdater.h"
#include "cclimatecond.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));

    // create the processor of pressing buttons
    CRequestProcessor * mpRequestProcessor = new CRequestProcessor; 

    // create the holder of conditioner state
    CClimateCond * mpCClimateCond = new CClimateCond(mpRequestProcessor);

    // create the UI state updater
    CStateUpdater * mpStateUpdater = new CStateUpdater(mpCClimateCond); 

    QScopedPointer<QApplication> app(createApplication(argc, argv));

    app->setApplicationName("ClimateTest");
    app->setQuitOnLastWindowClosed(true);

    QDeclarativeView viewer;
    viewer.rootContext()->setContextProperty("CRequestProcessor", mpRequestProcessor);
    viewer.rootContext()->setContextProperty("CStateUpdater", mpStateUpdater);
    viewer.setSource(QUrl::fromLocalFile("qml/ClimateHU_App/main.qml"));

    viewer.setGeometry(0, 0, 800, 600);
    viewer.setCursor(Qt::PointingHandCursor);

    // show the application 
    viewer.show(); 
    
    mpStateUpdater->initLocalRequest();

    return app->exec();
}

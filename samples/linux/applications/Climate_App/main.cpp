/* 
 * iviLINK SDK, version 1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012-2013, Luxoft Professional Corp., member of IBS group
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
 */ 


#ifndef ANDROID // because we wildcard this folder in Android.mk

#include <QtGui/QApplication>
#include <QDeclarativeContext>

#include "qmlapplicationviewer.h"

#include "Logger.hpp"
#include "ClimateRequestProcessor.hpp"
#include "ClimateStateUpdater.hpp"
#include "climate-app.hpp"

#include "ClimateState.hpp"

namespace {
    const iviLink::Service::Uid serviceUid= iviLink::Service::Uid("ClimateService");
    const iviLink::Profile::ApiUid
    senderUid    = iviLink::Profile::ApiUid("ClimateSenderProfile_PAPI_UID"),
    receiverUid  = iviLink::Profile::ApiUid("ClimateReceiverProfile_PAPI_UID");
}

// Special for state-app log declaration

Logger& log4StateApp()
{
    static Logger l = Logger::getInstance("stateapp.climate");
    return l;
}

int main_impl(int argc, char *argv[])
{
    using namespace std;
    using namespace std::tr1;

    PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("climateLog.properties"));
    climateAppPtr iapp= climateAppPtr(new climateApp( serviceUid,
                                                            senderUid,
                                                            receiverUid,
                                                            "climate-state.pb"));
    // create the processor of pressing buttons
    shared_ptr<ClimateRequestProcessor> requestProcessor(new ClimateRequestProcessor(iapp));

    // create the UI state updater
    shared_ptr<ClimateStateUpdater> stateUpdater(new ClimateStateUpdater(requestProcessor, iapp));

    QScopedPointer<QApplication> app(createApplication(argc, argv));

    app->setApplicationName("ClimateTest");
    app->setQuitOnLastWindowClosed(true);

    QDeclarativeView viewer;
    viewer.rootContext()->setContextProperty("ClimateRequestProcessor", requestProcessor.get());
    viewer.rootContext()->setContextProperty("ClimateStateUpdater", stateUpdater.get());
    viewer.setSource(QUrl::fromLocalFile("qml/Climate_App/main.qml"));

    viewer.setGeometry(0, 0, 800, 600);
    viewer.setCursor(Qt::PointingHandCursor);

    if (!QCoreApplication::arguments().contains("-caption") ) 
    {
        viewer.setCursor(QCursor(Qt::BlankCursor));
        viewer.setWindowFlags(Qt::FramelessWindowHint);
    }
    
    viewer.show();

    iapp.get()->initStateApp(stateUpdater.get());

    return app->exec();
}


Q_DECL_EXPORT int main(int argc, char *argv[])
{
    const int ret_code = main_impl(argc,argv);
    return ret_code;
}
#endif //ANDROID

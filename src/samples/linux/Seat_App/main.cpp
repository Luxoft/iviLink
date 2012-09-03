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



#ifndef ANDROID // because we wildcard this folder in Android.mk

#include <QtGui/QApplication>
#include <QDeclarativeContext>
#include "qmlapplicationviewer.h"

#include "utils/misc/Logger.hpp"

#include "crequestprocessor.h"
#include "cstateupdater.h"
#include "state-app.h"

namespace {
    const iviLink::Service::Uid service_uid= iviLink::Service::Uid("SeatService");
    const iviLink::Profile::ApiUid
    sender_uid    = iviLink::Profile::ApiUid("SeatSenderProfile_PAPI_UID"),
        receiver_uid  = iviLink::Profile::ApiUid("SeatReceiverProfile_PAPI_UID");
}

int main_impl(int argc, char *argv[])
{
    using namespace std;
    using namespace std::tr1;

    PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));
    shared_ptr<state_app> iapp= shared_ptr<state_app>( new state_app( service_uid,
                                                                      sender_uid,
                                                                      receiver_uid,
                                                                      "seat-state.bp" ) );
    // create the processor of pressing buttons
    shared_ptr<CRequestProcessor> rp( new CRequestProcessor( iapp ) );
    // create the UI state updater
    shared_ptr<CStateUpdater> su( new CStateUpdater( rp, iapp ) );

    QScopedPointer<QApplication> app(createApplication(argc, argv));

    app->setApplicationName("SeatTest");
    app->setQuitOnLastWindowClosed(true);

    QDeclarativeView viewer;
    viewer.rootContext()->setContextProperty("CRequestProcessor", rp.get());
    viewer.rootContext()->setContextProperty("CStateUpdater", su.get());
    viewer.setSource(QUrl::fromLocalFile("qml/Seat_App/main.qml"));

    viewer.setGeometry(0, 0, 800, 600);
    viewer.setCursor(Qt::PointingHandCursor);

    viewer.show();

    return app->exec();
}

Q_DECL_EXPORT int main(int argc, char *argv[])
{

    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // this little trick needed to ensure that all protobuf users destroyed before
    // the shutdown process
    const int ret_code = main_impl(argc,argv);

    // Shutdown protobuf library by hands
    google::protobuf::ShutdownProtobufLibrary();

    return ret_code;
}
#endif //ANDROID

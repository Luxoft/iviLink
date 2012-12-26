/* 
 * iviLINK SDK, version 1.1.19
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
 */ 
 

#ifndef ANDROID

#include <QtGui/QApplication>
#include <QDeclarativeContext>
#include <qmlapplicationviewer.h>

#include <iostream>

#include "common.hpp"
#include "CPlayerLogic.h"
#include "CConnectionControl.h"
#include "CGuiController.hpp"
#include "CMediaSource.h"
#include "CPlayer.h"
#include "CServer.h"
#include "CVideoWidget.hpp"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));
    
    QScopedPointer<QApplication> app(createApplication(argc, argv));
    
    //Entity sending events (emit) to the application Gui
    //Please note emitter inherits and implements IGuiController.
    CGuiController* guiController = new CGuiController; // deleted in CPlayerLogic's destructor
    
    //Object responsible for track. It does track management. It implements IMediaSource interface. CMediaplayer calls CMediaSource methods
    CMediaSource* source = new CMediaSource(UBUNTU_LOCAL_DIRECTORY);
    
    //inctance of vlc client object
    //vlc client is a object inheriting IPlayer
    CPlayer* player = new CPlayer;
    
    //inctance of server intended for local playback
    CServer* server = new CServer;

    //Object to show video
    CVideoWidget*  videoWidget = new CVideoWidget;
    
    //storing videoWidget
    player->setWidget(videoWidget);

    //Inherits CApp. Process callbacks from CApp
    //CConnectionControl(IGuiController * controller, IMediaSource * source, IPlayer * player, IServer * server);
    CConnectionControl * mConnectionControl = new CConnectionControl(guiController, source, player, server);
    mConnectionControl->init();

    //creating app window
    app->setApplicationName("MediaApplication");
    app->setQuitOnLastWindowClosed(true);
    QDeclarativeView  viewer;
    viewer.setGeometry(XLEFTPOS, YUPPOS, XRIGHTPOS, YDOWNPOS);
    viewer.rootContext()->setContextProperty("CEmitter", guiController); 
    viewer.rootContext()->setContextProperty("CPlayerLogic", mConnectionControl->getPlayerLogic()); 
    viewer.setSource(QUrl::fromLocalFile("qml/MediaApp/main.qml")); 
    viewer.setCursor(QCursor(Qt::BlankCursor));
    viewer.setWindowFlags(Qt::FramelessWindowHint);

    //video widget and control panel
    QDeclarativeView *view_control = new QDeclarativeView(0);
    view_control->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    view_control->rootContext()->setContextProperty("CPlayerLogic", mConnectionControl->getPlayerLogic());
    view_control->rootContext()->setContextProperty("videoWidget", videoWidget);
    view_control->setSource(QUrl::fromLocalFile("qml/MediaApp/vid_control.qml"));
    view_control->setGeometry(VID_XLEFTPOS, VID_YUPPOS, VID_XRIGHTPOS, VID_YDOWNPOS);
    view_control->setWindowFlags(Qt::FramelessWindowHint);   //remove frame
    videoWidget->setControlView(view_control);
    view_control->setWindowFlags(Qt::Popup | Qt::WindowStaysOnTopHint);

    
    // show the application
    viewer.show();

    return app->exec();

    delete mConnectionControl;
    delete videoWidget;
    delete server;
    delete player;
    delete source;
    delete guiController;
}

#endif //ANDROID

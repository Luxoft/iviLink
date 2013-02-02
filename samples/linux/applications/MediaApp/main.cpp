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

#ifndef ANDROID


#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <QtGui/QApplication>
#include <QDeclarativeContext>
#include <QDesktopWidget>
#include <qmlapplicationviewer.h>

#include <iostream>

#include "MediaAppCommon.hpp"
#include "PlayerLogic.hpp"
#include "CConnectionControl.h"
#include "CGuiController.hpp"
#include "MediaSource.hpp"
#include "CPlayer.h"
#include "CServer.h"
#include "CVideoWidget.hpp"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("mediaLog.properties"));

    QScopedPointer < QApplication > app(createApplication(argc, argv));

    //Entity sending events (emit) to the application Gui
    //Please note emitter inherits and implements IGuiController.
    CGuiController* guiController = new CGuiController; // deleted in CPlayerLogic's destructor

    //Object responsible for track. It does track management. It implements IMediaSource interface. CMediaplayer calls CMediaSource methods
    struct passwd *pw = getpwuid(getuid());
    const char *buf = pw->pw_dir;
    std::string homeDir = std::string(buf);
    homeDir += "/Music/";
    Playlist* source = new Playlist(homeDir);
    
    //inctance of vlc client object
    //vlc client is a object inheriting IPlayer
    CPlayer* player = new CPlayer;

    //inctance of server intended for local playback
    CServer* server = new CServer;

    //Object to show video
    CVideoWidget* videoWidget = new CVideoWidget;

    //storing videoWidget
    player->setWidget(videoWidget);

    //Inherits CApp. Process callbacks from CApp
    CConnectionControl * mConnectionControl = new CConnectionControl(guiController, source, player,
            server);
    mConnectionControl->init();

    //creating app window
    app->setApplicationName("MediaApplication");
    app->setQuitOnLastWindowClosed(true);
    QDeclarativeView viewer;
    viewer.setGeometry(XLEFTPOS, YUPPOS, XRIGHTPOS, YDOWNPOS);
    viewer.rootContext()->setContextProperty("CEmitter", guiController);
    viewer.rootContext()->setContextProperty("CPlayerLogic", mConnectionControl->getPlayerLogic());
    viewer.setSource(QUrl::fromLocalFile("qml/MediaApp/main.qml"));
    viewer.setCursor(QCursor(Qt::BlankCursor));
    viewer.setWindowFlags(Qt::FramelessWindowHint);

    //video widget and control panel
    QDeclarativeView *view_control = new QDeclarativeView(0);
    view_control->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    view_control->rootContext()->setContextProperty("CEmitter", guiController);
    view_control->rootContext()->setContextProperty("CPlayerLogic",
            mConnectionControl->getPlayerLogic());
    view_control->rootContext()->setContextProperty("videoWidget", videoWidget);
    view_control->setSource(QUrl::fromLocalFile("qml/MediaApp/vid_control.qml"));
    view_control->setGeometry(VID_XLEFTPOS, VID_YUPPOS, VID_XRIGHTPOS, VID_YDOWNPOS);
    view_control->setWindowFlags(Qt::FramelessWindowHint);   //remove frame
    videoWidget->setControlView(view_control);
    view_control->setWindowFlags(Qt::Popup | Qt::WindowStaysOnTopHint);
    view_control->move(QApplication::desktop()->screen()->rect().center().x()
                       - view_control->rect().center().x()
                       , QApplication::desktop()->screen()->rect().bottom()
                       - view_control->rect().height());

    // show the application
    viewer.show();

    return app->exec();
}

#endif //ANDROID

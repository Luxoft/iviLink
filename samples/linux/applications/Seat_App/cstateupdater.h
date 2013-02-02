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


#ifndef CSTATEUPDATER_H
#define CSTATEUPDATER_H

#ifndef ANDROID
#include <QObject>
#include <QString>
#include <QDebug>
#include <QDir>
#include <QDeclarativeView>
#else // no QT there
#include <jni.h>
#include "JniThreadHelper.hpp"
#endif //ANDROID
#include <cassert>

#include "crequestprocessor.h"

#include "IStateAppProgressBar.hpp"

#include "seat-app.hpp"
#include "Logger.hpp"

#include <tr1/memory>

#ifndef ANDROID
class CStateUpdater : public QObject,
                      public IStateAppProgressBar
{
    Q_OBJECT
#else
class CStateUpdater : public IStateAppProgressBar
{
#endif //ANDROID

public:
#ifndef ANDROID
    explicit CStateUpdater(std::tr1::shared_ptr<CRequestProcessor> rp,
                           seatAppPtr app);
#else //JavaVM and object for callbacks
    explicit CStateUpdater(std::tr1::shared_ptr<CRequestProcessor> rp,
                           seatAppPtr app,
                           JavaVM * pJm,
                           jobject bridge);
#endif //ANDROID
    virtual ~CStateUpdater();

    virtual void showLoadingScreen();
    virtual void hideLoadingScreen();

    void initRequest() 
    { 
        incomingNotification(); 
    };

private:
    void appStateCallback( seatApp::APP_STATE st );
    void incomingNotification();

    std::tr1::shared_ptr<CRequestProcessor> requestProcessor;
    seatAppPtr app;

#ifndef ANDROID

signals:

    /**
     * set of functions which connected with qml
     * and change UI state after changing seat state
     */

    void showSeat();
    void heaterDriver(int heat);
    void heaterPass(int heat);
    void currentSeatViewPass();
    void currentSeatViewDriver();
    void bottomX(int pos);
    void bottomY(int pos);
    void backAngle(int angle);
    void backX(int pos);
    void backY(int pos);
    void initRequestDone();

    void sigShowLoadingScreen();
    void sigHideLoadingScreen();
#else
    JavaVM * mpJM;
    jobject jCallbacks;

    jmethodID onHeaterDriver;
    jmethodID onHeaterPassenger;
    jmethodID onSetDriver;
    jmethodID onSetPassenger;
    jmethodID onBottomX;
    jmethodID onBottomY;
    jmethodID onBackAngle;
    jmethodID onBackX;
    jmethodID onBackY;
    jmethodID onShowSeat;
#endif //ANDROID

};

#endif // CSTATEUPDATER_H

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
#include "utils/android/JniThreadHelper.hpp"
#endif //ANDROID
#include <cassert>

#include "crequestprocessor.h"

#include "state-app.h"
#include "utils/misc/Logger.hpp"

#include <tr1/memory>

#ifndef ANDROID
class CStateUpdater : public QObject
{
    Q_OBJECT
#else
class CStateUpdater
{
#endif //ANDROID

 public:
    #ifndef ANDROID
    explicit CStateUpdater(std::tr1::shared_ptr<CRequestProcessor> rp,
                           std::tr1::shared_ptr<state_app> app);
    #else //JavaVM and object for callbacks
    explicit CStateUpdater(std::tr1::shared_ptr<CRequestProcessor> rp,
                           std::tr1::shared_ptr<state_app> app,
                           JavaVM * pJm,
                           jobject bridge);
    #endif //ANDROID
    ~CStateUpdater();
   
    void initRequest() { incomingNotification(); };

 private:
    void appStateCallback( state_app::APP_STATE st );
    void incomingNotification();

    std::tr1::shared_ptr<CRequestProcessor> rp;
    std::tr1::shared_ptr<state_app> app;
    
#ifndef ANDROID
    bool qmlInitialized;

public slots:
    void onQmlVisible();

 signals:

    /**
     * set of functions which connected with qml
     * and change UI state after changing seat state
     */

    void showSeat();
    void heaterDriver(int heat);
    void heaterPass(int heat);
    void current_seat_viewPass();
    void current_seat_viewDriver();
    void bottom_x(int pos);
    void bottom_y(int pos);
    void back_angle(int angle);
    void back_x(int pos);
    void back_y(int pos);
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

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


#ifndef ClimateStateUpdater_H
#define ClimateStateUpdater_H

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

#include "ClimateRequestProcessor.hpp"
#include "IStateAppProgressBar.hpp"

#include "climate-app.hpp"
#include "Logger.hpp"

#include <tr1/memory>

#ifndef ANDROID
class ClimateStateUpdater : public QObject,
                            public IStateAppProgressBar
{
    Q_OBJECT
#else
class ClimateStateUpdater : public IStateAppProgressBar
{
#endif //ANDROID

 public:
#ifndef ANDROID
    explicit ClimateStateUpdater(std::tr1::shared_ptr<ClimateRequestProcessor> rp,
                           climateAppPtr app);
#else //JavaVM and object for callbacks
    explicit ClimateStateUpdater(std::tr1::shared_ptr<ClimateRequestProcessor> rp,
                           climateAppPtr app,
                           JavaVM * pJm,
                           jobject bridge);
#endif //ANDROID
    virtual ~ClimateStateUpdater();

    virtual void showLoadingScreen();
    virtual void hideLoadingScreen();

    void initRequest()
    { 
        incomingNotification(); 
    };

 private:
    void appStateCallback( climateApp::APP_STATE st );
    void incomingNotification();

    std::tr1::shared_ptr<ClimateRequestProcessor> requestProcessor;
    climateAppPtr app;

#ifndef ANDROID

 signals:

    /**
     * set of functions which connected with qml
     * and change UI state after changing seat state
     */

    void showClimate();

    void autoState(bool status);
    void recircState(bool status);
    void acState(bool status);
    void frontState(bool status);
    void rearState(bool status);
    void fanLowState(bool status);
    void fanMidState(bool status);
    void fanHighState(bool status);
    void passTemp(int temp);
    void driverTemp(int temp);
    void syncState(bool status);
    void fanSpeed(int speed);
    void fanOffState(bool status);
    void sigShowLoadingScreen();
    void sigHideLoadingScreen();
#else
    JavaVM * mpJM;
    jobject jCallbacks;
    
    jmethodID autoState;
    jmethodID recircState;
    jmethodID acState;
    jmethodID frontState;
    jmethodID rearState;
    jmethodID fanLowState;
    jmethodID fanMidState;
    jmethodID fanHighState;
    jmethodID passTemp;
    jmethodID driverTemp;
    jmethodID syncState;
    jmethodID fanSpeed;
    jmethodID fanOffState;
#endif //ANDROID

};

#endif // ClimateStateUpdater_H

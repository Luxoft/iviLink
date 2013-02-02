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
#include <QDeclarativeEngine>
#include <QDeclarativeComponent>
#include <QDeclarativeContext>
#endif //ANDROID

#include "cstateupdater.h"
#include "seat_coords.hpp"

#include <fstream>
#include "BaseProfileApi.hpp"
#include "CServiceManager.hpp"

#include <tr1/utility>
#include <tr1/type_traits>

using namespace std;
using namespace std::tr1;

#ifndef ANDROID
CStateUpdater::CStateUpdater(std::tr1::shared_ptr<CRequestProcessor> requestProcessor,
                             seatAppPtr app)
    : requestProcessor(requestProcessor)
    , app(app)
    #else
CStateUpdater::CStateUpdater(std::tr1::shared_ptr<CRequestProcessor> requestProcessor,
                             seatAppPtr app,
                             JavaVM * pJm,
                             jobject bridge)
    : requestProcessor(requestProcessor)
    , app(app)
    , mpJM(pJm)
    , jCallbacks(bridge)
    #endif //ANDROID
{
    LOG4CPLUS_TRACE_METHOD(log4StateApp(), __PRETTY_FUNCTION__);
    assert(requestProcessor);
    assert(app);
    if (app)
    {
        using namespace placeholders;
        app->setStateCallback(bind( &CStateUpdater::incomingNotification, this));
        app->setAppStateCallback(bind(&CStateUpdater::appStateCallback, this, _1));
    }
    LOG4CPLUS_INFO(log4StateApp(), "CStateUpdater created");
#ifndef ANDROID
#else
    iviLink::Android::JniThreadHelper jth(mpJM);
    JNIEnv * env = jth.getEnv();
    jclass claz = env->GetObjectClass(jCallbacks);
    onHeaterDriver = env->GetMethodID(claz, "onHeaterDriver", "(I)V");
    onHeaterPassenger = env->GetMethodID(claz, "onHeaterPassenger", "(I)V");
    onSetDriver= env->GetMethodID(claz, "onSetDriver", "()V");
    onSetPassenger = env->GetMethodID(claz, "onSetPassenger", "()V");
    onBottomX = env->GetMethodID(claz, "onBottomX", "(I)V");
    onBottomY = env->GetMethodID(claz, "onBottomY", "(I)V");
    onBackAngle = env->GetMethodID(claz, "onBackAngle", "(I)V");
    onBackX = env->GetMethodID(claz, "onBackX", "(I)V");
    onBackY = env->GetMethodID(claz, "onBackY", "(I)V");
    onShowSeat = env->GetMethodID(claz, "onShowSeat", "()V");
#endif //ANDROID
}

CStateUpdater::~CStateUpdater()
{
}


void CStateUpdater::showLoadingScreen()
{
    LOG4CPLUS_TRACE_METHOD(log4StateApp(), __PRETTY_FUNCTION__);
#ifndef ANDROID
    emit sigShowLoadingScreen();
#else
    iviLink::Android::JniThreadHelper jth(mpJM);
    JNIEnv * env = jth.getEnv();
    jclass claz = env->GetObjectClass(jCallbacks);
    jmethodID mid = env->GetMethodID(claz, "onShowLoadingScreen", "()V");
    env->CallVoidMethod(jCallbacks, mid);
#endif //ANDROID
}

void CStateUpdater::hideLoadingScreen()
{
    LOG4CPLUS_TRACE_METHOD(log4StateApp(), __PRETTY_FUNCTION__);
#ifndef ANDROID
    emit sigHideLoadingScreen();
#else
    iviLink::Android::JniThreadHelper jth(mpJM);
    JNIEnv * env = jth.getEnv();
    jclass claz = env->GetObjectClass(jCallbacks);
    jmethodID mid = env->GetMethodID(claz, "onHideLoadingScreen", "()V");
    env->CallVoidMethod(jCallbacks, mid);
#endif //ANDROID
}

void CStateUpdater::appStateCallback(seatApp::APP_STATE)
{
    LOG4CPLUS_TRACE_METHOD(log4StateApp(), __PRETTY_FUNCTION__);
    if (app->isActive())
    {
        incomingNotification();
    }
}


//Callback for profile created on the other side; notification about seat state
void CStateUpdater::incomingNotification()
{
    LOG4CPLUS_TRACE_METHOD(log4StateApp(), __PRETTY_FUNCTION__);
    if (!app)
    {
        return;
    }

    ISeatReceiverProfile::state_t st;
    app->retrieveState(st);
    LOG4CPLUS_INFO(log4StateApp(), "Got state: " + st.serialize());
#ifndef ANDROID
    emit showSeat();

    emit heaterDriver(st.getDriverHeater());
    emit heaterPass(st.getPassengerHeater());

    const int sbottomX = rel2screenX(st.getCurrentX());
    const int sbottomY = rel2screenY(st.getCurrentY());
    emit bottomX(sbottomX);
    emit bottomY(sbottomY);
    emit backX(bottom2backX(sbottomX ) );
    emit backY(bottom2backY(sbottomY ) );
    emit backAngle(st.getCurrentSeatBackAngle());

    if(st.getCurrentSelection() == iviLink::DRIVER)
    {
        emit currentSeatViewDriver();
    }
    else
    {
        emit currentSeatViewPass();
    }
#else
    iviLink::Android::JniThreadHelper jth(mpJM);
    JNIEnv * env = jth.getEnv();

    env->CallVoidMethod(jCallbacks, onShowSeat);
    env->CallVoidMethod(jCallbacks, onBottomX, st.getCurrentX());
    env->CallVoidMethod(jCallbacks, onBottomY, st.getCurrentY());
    env->CallVoidMethod(jCallbacks, onBackX, st.getCurrentX());
    env->CallVoidMethod(jCallbacks, onBackY, st.getCurrentY());
    env->CallVoidMethod(jCallbacks, onBackAngle, st.getCurrentSeatBackAngle());

    if (st.getCurrentSelection() == iviLink::DRIVER)
    {
        env->CallVoidMethod(jCallbacks, onSetDriver);
        env->CallVoidMethod(jCallbacks, onHeaterDriver, st.getDriverHeater());
    }
    else
    {
        env->CallVoidMethod(jCallbacks, onSetPassenger);
        env->CallVoidMethod(jCallbacks, onHeaterPassenger, st.getPassengerHeater());
    }
#endif //ANDROID
}

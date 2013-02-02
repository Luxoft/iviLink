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

#include "ClimateStateUpdater.hpp"

#include <fstream>
#include "BaseProfileApi.hpp"
#include "CServiceManager.hpp"

#include <tr1/utility>
#include <tr1/type_traits>

using namespace std;
using namespace std::tr1;


#ifndef ANDROID
ClimateStateUpdater::ClimateStateUpdater(std::tr1::shared_ptr<ClimateRequestProcessor> requestProcessor,
                                         climateAppPtr app)
    : requestProcessor(requestProcessor)
    , app(app)
    #else
ClimateStateUpdater::ClimateStateUpdater(std::tr1::shared_ptr<ClimateRequestProcessor> requestProcessor,
                                         climateAppPtr app,
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
    if(app)
    {
        using namespace placeholders;
        app->setStateCallback( bind( &ClimateStateUpdater::incomingNotification, this ) );
        app->setAppStateCallback( bind( &ClimateStateUpdater::appStateCallback, this, _1 ) );
    }
    LOG4CPLUS_INFO(log4StateApp(), "ClimateStateUpdater created");
#ifndef ANDROID
#else
    iviLink::Android::JniThreadHelper jth(mpJM);
    JNIEnv * env = jth.getEnv();
    jclass claz = env->GetObjectClass(jCallbacks);
    LOG4CPLUS_INFO(log4StateApp(), "ClimateStateUpdater: constructor: got claz");
    autoState=env->GetMethodID(claz, "autoState", "(Z)V");
    recircState = env->GetMethodID(claz, "recircState", "(Z)V");
    acState = env->GetMethodID(claz, "acState", "(Z)V");
    frontState = env->GetMethodID(claz, "frontState", "(Z)V");
    rearState  = env->GetMethodID(claz, "rearState", "(Z)V");
    fanLowState = env->GetMethodID(claz, "fanLowState", "(Z)V");
    fanMidState = env->GetMethodID(claz, "fanMidState", "(Z)V");
    fanHighState = env->GetMethodID(claz, "fanHighState", "(Z)V");
    fanOffState = env->GetMethodID(claz, "fanOffState", "(Z)V");
    syncState = env->GetMethodID(claz, "syncState", "(Z)V");
    passTemp = env->GetMethodID(claz, "passTemp", "(I)V");
    driverTemp = env->GetMethodID(claz, "driverTemp", "(I)V");
    fanSpeed  = env->GetMethodID(claz, "fanSpeed", "(I)V");

#endif //ANDROID
}

ClimateStateUpdater::~ClimateStateUpdater()
{
}

void ClimateStateUpdater::showLoadingScreen()
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

void ClimateStateUpdater::hideLoadingScreen()
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

void ClimateStateUpdater::appStateCallback(climateApp::APP_STATE)
{
    if (app -> isActive())
    {
        incomingNotification();
    }
}


//Callback for profile created on the other side; notification about seat state
void ClimateStateUpdater::incomingNotification()
{
    if(!app) 
    {
        return;
    }

    IClimateReceiverProfile::state_t st;
    app->retrieveState(st);

#ifndef ANDROID

    emit fanSpeed          ( st. getFanSpeed() );
    emit driverTemp        ( st. getDriverTemperature() );
    emit passTemp          ( st. getPassTemperature() );
    emit autoState         ( st. isAuto() );
    emit syncState         ( st. isSync() );
    emit recircState       ( st. isRecirculation() );
    emit acState           ( st. isAC() );
    emit frontState        ( st. isFront() );
    emit rearState         ( st. isRear() );
    emit fanOffState       ( st. isOff() );
    emit fanHighState      ( st. isFanHigh() );
    emit fanMidState       ( st. isFanMiddle() );
    emit fanLowState       ( st. isFanLow() );

    emit showClimate();

#else
    iviLink::Android::JniThreadHelper jth(mpJM);
    JNIEnv * env = jth.getEnv();

    env->CallVoidMethod(jCallbacks, autoState, (jboolean)st.isAuto());
    env->CallVoidMethod(jCallbacks, recircState,(jboolean)st.isRecirculation());
    env->CallVoidMethod(jCallbacks, fanOffState,(jboolean)st.isOff());
    env->CallVoidMethod(jCallbacks, acState,(jboolean)st.isAC());
    env->CallVoidMethod(jCallbacks, frontState,(jboolean)st.isFront());
    env->CallVoidMethod(jCallbacks, rearState,(jboolean)st.isRear());
    env->CallVoidMethod(jCallbacks, fanLowState,(jboolean)st.isFanLow());
    env->CallVoidMethod(jCallbacks, fanMidState,(jboolean)st.isFanMiddle());
    env->CallVoidMethod(jCallbacks, fanHighState,(jboolean)st.isFanHigh());
    env->CallVoidMethod(jCallbacks, syncState,(jboolean) st.isSync());
    env->CallVoidMethod(jCallbacks, passTemp,(jint) st.getPassTemperature());
    env->CallVoidMethod(jCallbacks, driverTemp,(jint) st.getDriverTemperature());
    env->CallVoidMethod(jCallbacks, fanSpeed, (jint) st.getFanSpeed());
#endif //ANDROID
}


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




#ifndef ANDROID
#include <QDeclarativeEngine>
#include <QDeclarativeComponent>
#include <QDeclarativeContext>
#endif //ANDROID

#include "cstateupdater.h"

#include <fstream>
#include <google/protobuf/text_format.h>
#include "framework/public/appLib/CProfileApiBase.hpp"
#include "framework/libraries/ServiceManager/CServiceManager.hpp"

#include <tr1/utility>
#include <tr1/type_traits>

using namespace std;
using namespace std::tr1;


namespace {
    Logger log4 = Logger::getInstance("SeatClientSample.CStateUpdater");
}


#ifndef ANDROID
CStateUpdater::CStateUpdater(std::tr1::shared_ptr<CRequestProcessor> rp,
                             std::tr1::shared_ptr<state_app> app)
    : rp( rp )
    , app( app )
#else
CStateUpdater::CStateUpdater(std::tr1::shared_ptr<CRequestProcessor> rp,
                             std::tr1::shared_ptr<state_app> app,
                             JavaVM * pJm,
                             jobject bridge)
    : rp( rp )
    , app( app )
    , mpJM(pJm)
    , jCallbacks(bridge)
#endif //ANDROID
{
   LOG4CPLUS_TRACE_METHOD(log4, __PRETTY_FUNCTION__);
   assert(rp);
   assert( app );
   if( app )
   {
       using namespace placeholders;
       app->set_state_callback( bind( &CStateUpdater::incomingNotification, this ) );
       app->set_app_state_callback( bind( &CStateUpdater::appStateCallback, this, _1 ) );
   }
   LOG4CPLUS_INFO(log4, "CStateUpdater created");
   #ifndef ANDROID
   #else
   iviLink::Android::JniThreadHelper jth(mpJM);
   JNIEnv * env = jth.getEnv();

  jclass claz = env->GetObjectClass(jCallbacks);
  LOG4CPLUS_INFO(log4, "CStateUpdater: constructor: got claz");


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

CStateUpdater::~CStateUpdater()
{
}

void CStateUpdater::appStateCallback( state_app::APP_STATE )
{
   if(is_active(app))
       incomingNotification();
}


//Callback for profile created on the other side; notification about seat state
void CStateUpdater::incomingNotification()
{
    if( !app ) return;

    IClimateReceiverProfile::state_t st;
    app->retrive_state( st );

    #ifndef ANDROID

    emit fanSpeed          ( st. fan_speed       () );
    emit driverTemp        ( st. driver_temp     () );
    emit passTemp          ( st. pass_temp       () );
    emit autoState         ( st. auto_climate    () );
    emit syncState         ( st. dual_climate    () );
    emit recircState       ( st. air_recirc      () );
    emit acState           ( st. is_ac           () );
    emit frontState        ( st. front_window    () );
    emit rearState         ( st. rear_window     () );
    emit fanOffState       ( st. is_off          () );
    emit fanHighState      ( st. fan_mode_top    () );
    emit fanMidState       ( st. fan_mode_middle () );
    emit fanLowState       ( st. fan_mode_lows   () );

    emit showClimate();

    #else
    iviLink::Android::JniThreadHelper jth(mpJM);
    JNIEnv * env = jth.getEnv();

    env->CallVoidMethod(jCallbacks, autoState, (jboolean)st.auto_climate());
    env->CallVoidMethod(jCallbacks, recircState,(jboolean)st.air_recirc());
    env->CallVoidMethod(jCallbacks, fanOffState,(jboolean)st.is_off());
    env->CallVoidMethod(jCallbacks, acState,(jboolean)st.is_ac());
    env->CallVoidMethod(jCallbacks, frontState,(jboolean)st.front_window());
    env->CallVoidMethod(jCallbacks, rearState,(jboolean)st.rear_window());
    env->CallVoidMethod(jCallbacks, fanLowState,(jboolean)st.fan_mode_lows());
    env->CallVoidMethod(jCallbacks, fanMidState,(jboolean)st.fan_mode_middle());
    env->CallVoidMethod(jCallbacks, fanHighState,(jboolean)st.fan_mode_top());
    env->CallVoidMethod(jCallbacks, syncState,(jboolean) st.dual_climate());
    env->CallVoidMethod(jCallbacks, passTemp,(jint) st.pass_temp());
    env->CallVoidMethod(jCallbacks, driverTemp,(jint) st.driver_temp());
    env->CallVoidMethod(jCallbacks, fanSpeed, (jint) st.fan_speed());
    #endif //ANDROID
}


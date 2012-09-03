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
    , qmlInitialized(false)
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

#ifndef ANDROID
void CStateUpdater::onQmlVisible()
{
   LOG4CPLUS_INFO(log4, "got signal that GUI is ready");
   qmlInitialized= true;
   if( is_active(app) )
   {
      LOG4CPLUS_INFO(log4, "got signal that GUI is ready, init is done as well, sending init request");
      incomingNotification();
   }
}
#endif //ANDROID

void CStateUpdater::appStateCallback( state_app::APP_STATE )
{
   #ifndef ANDROID
    if( is_active(app) && qmlInitialized )
        incomingNotification();
   #else
   if(is_active(app)) incomingNotification();
   #endif //ANDROID
}


//Callback for profile created on the other side; notification about seat state
void CStateUpdater::incomingNotification()
{
    if( !app ) return;

    ISeatReceiverProfile::state_t st;
    app->retrive_state( st );
    #ifndef ANDROID
    emit heaterDriver(st.driver().heater());
    emit heaterPass(st.pass().heater());
    const iviLink::SeatState::Seat& seat = st.current_seat() == iviLink::SeatState::DRIVER ?
                st.driver() : st.pass();
    emit bottom_x(seat.bottom_x());
    emit bottom_y(seat.bottom_y());
    emit back_x(seat.back_x());
    emit back_y(seat.back_y());
    emit back_angle(seat.back_angle());

    emit st.current_seat() == iviLink::SeatState::DRIVER ?
        current_seat_viewDriver() :
        current_seat_viewPass();

    emit showSeat();
    #else
    iviLink::Android::JniThreadHelper jth(mpJM);
    JNIEnv * env = jth.getEnv();
    
    const iviLink::SeatState::Seat& seat = st.current_seat() == iviLink::SeatState::DRIVER ?
                st.driver() : st.pass();
    
    env->CallVoidMethod(jCallbacks, onShowSeat);    
    env->CallVoidMethod(jCallbacks, onBottomX, seat.bottom_x());
    env->CallVoidMethod(jCallbacks, onBottomY, seat.bottom_y()); 
    env->CallVoidMethod(jCallbacks, onBackX, seat.back_x());
    env->CallVoidMethod(jCallbacks, onBackY, seat.back_y());
    env->CallVoidMethod(jCallbacks, onBackAngle, seat.back_angle());
    if(st.current_seat() == iviLink::SeatState::DRIVER){
      env->CallVoidMethod(jCallbacks, onSetDriver);
      env->CallVoidMethod(jCallbacks, onHeaterDriver, st.driver().heater());
    } else {
      env->CallVoidMethod(jCallbacks, onSetPassenger);
      env->CallVoidMethod(jCallbacks, onHeaterPassenger, st.pass().heater());
    }
    #endif //ANDROID
}


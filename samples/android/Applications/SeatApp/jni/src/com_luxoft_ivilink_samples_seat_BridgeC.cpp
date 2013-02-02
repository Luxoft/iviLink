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


#include "com_luxoft_ivilink_samples_seat_BridgeC.hpp"
#include "crequestprocessor.h"
#include "cstateupdater.h"
#include "seat-app.hpp"

#include "Logger.hpp"
#include "StringConversion.hpp"
#include "JniThreadHelper.hpp"
#include "AppInfo.hpp"

#include <tr1/memory>
#include <cassert>

static JavaVM * gJm;
static jobject gBridge;

seatAppPtr gApp;
std::tr1::shared_ptr<CRequestProcessor> seatRequestProcessor;
std::tr1::shared_ptr<CStateUpdater> seatStateUpdater;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved) {
    gJm = jvm;
    return JNI_VERSION_1_6;
}

Logger& log4StateApp()
{
    static Logger l = Logger::getInstance("stateapp.seat");
    return l;
}


namespace {
const iviLink::Service::Uid serviceUid= iviLink::Service::Uid("SeatService");
const iviLink::Profile::ApiUid
senderUid    = iviLink::Profile::ApiUid("SeatSenderProfile_PAPI_UID"),
receiverUid  = iviLink::Profile::ApiUid("SeatReceiverProfile_PAPI_UID");
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_seat_BridgeC_startApp
(JNIEnv * env, jobject bridge, jstring launchInfo, jstring pathToServiceRepo, jstring internalPath, jstring statePath)
{
    Logger log = Logger::getInstance("iviLink.Samples.SeatApp.JNIlayer");
    LOG4CPLUS_DEBUG(log, "startApp");

    iviLink::Android::AppInfo appInfo;
    appInfo.serviceRepoPath = iviLink::Android::StringConversion::jToStd(pathToServiceRepo, env);
    appInfo.launchInfo = iviLink::Android::StringConversion::jToStd(launchInfo, env);
    appInfo.privateDirPath = iviLink::Android::StringConversion::jToStd(internalPath, env);
    appInfo.isValid = true;

    std::string serializeStatePath = iviLink::Android::StringConversion::jToStd(statePath, env);

    gBridge = env->NewGlobalRef(bridge);


    gApp = seatAppPtr(new seatApp(appInfo, serviceUid, senderUid, receiverUid, serializeStatePath));
    LOG4CPLUS_DEBUG(log, "created state_app");
    seatRequestProcessor = std::tr1::shared_ptr<CRequestProcessor>(new CRequestProcessor( gApp ));
    LOG4CPLUS_DEBUG(log, "created request processor");
    seatStateUpdater = std::tr1::shared_ptr<CStateUpdater>(new CStateUpdater(seatRequestProcessor,gApp, gJm, gBridge));
    LOG4CPLUS_DEBUG(log, "created state updater");
    gApp.get()->initStateApp(seatStateUpdater.get());
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_seat_BridgeC_driverHeaterRequest
(JNIEnv *, jobject)
{
    seatRequestProcessor->driverHeaterRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_seat_BridgeC_passHeaterRequest
(JNIEnv *, jobject)
{
    seatRequestProcessor->passHeaterRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_seat_BridgeC_driverSeatRequest
(JNIEnv *, jobject)
{
    seatRequestProcessor->driverSeatRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_seat_BridgeC_passSeatRequest
(JNIEnv *, jobject)
{
    seatRequestProcessor->passSeatRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_seat_BridgeC_backLeftRequest
(JNIEnv *, jobject)
{
    seatRequestProcessor->backLeftRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_seat_BridgeC_backRightRequest
(JNIEnv *, jobject)
{
    seatRequestProcessor->backRightRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_seat_BridgeC_bottomUpRequest
(JNIEnv *, jobject)
{
    seatRequestProcessor->bottomUpRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_seat_BridgeC_bottomDownRequest
(JNIEnv *, jobject)
{
    seatRequestProcessor->bottomDownRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_seat_BridgeC_bottomLeftRequest
(JNIEnv *, jobject)
{
    seatRequestProcessor->bottomLeftRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_seat_BridgeC_bottomRightRequest
(JNIEnv *, jobject)
{
    seatRequestProcessor->bottomRightRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_seat_BridgeC_saveState
(JNIEnv *, jobject)
{
    gApp->saveState();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_seat_BridgeC_loadState
(JNIEnv *, jobject)
{
    seatStateUpdater->initRequest();
}

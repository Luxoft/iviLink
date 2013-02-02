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


#include "com_luxoft_ivilink_samples_climate_Bridge.hpp"
#include "climate-app.hpp"
#include "ClimateRequestProcessor.hpp"
#include "ClimateStateUpdater.hpp"

#include "Logger.hpp"
#include "StringConversion.hpp"
#include "JniThreadHelper.hpp"
#include "AppInfo.hpp"

#include <tr1/memory>

static JavaVM * gJm;
static jobject bridgeJ;

climateAppPtr gApp;
std::tr1::shared_ptr<ClimateRequestProcessor> climateRequestProcessor;
std::tr1::shared_ptr<ClimateStateUpdater> climateStateUpdater;


JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved) {
    gJm = jvm;
    return JNI_VERSION_1_6;
}


Logger& log4StateApp()
{
    static Logger l = Logger::getInstance("stateapp.climate");
    return l;
}

namespace {
const iviLink::Service::Uid serviceUid= iviLink::Service::Uid("ClimateService");
const iviLink::Profile::ApiUid
senderUid    = iviLink::Profile::ApiUid("ClimateSenderProfile_PAPI_UID"),
receiverUid  = iviLink::Profile::ApiUid("ClimateReceiverProfile_PAPI_UID");
}


JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_startApp
(JNIEnv * env, jobject bridge, jstring serviceRepoPath, jstring launchInfo, jstring internalPath)
{
    Logger log = Logger::getInstance("iviLink.Samples.ClimateApp.JNIlayer");
    LOG4CPLUS_DEBUG(log, "startApp");

    iviLink::Android::AppInfo appInfo;
    appInfo.serviceRepoPath = iviLink::Android::StringConversion::jToStd(serviceRepoPath, env);
    appInfo.launchInfo = iviLink::Android::StringConversion::jToStd(launchInfo, env);
    appInfo.privateDirPath = iviLink::Android::StringConversion::jToStd(internalPath, env);
    appInfo.isValid = true;

    std::string serializeStatePath = appInfo.privateDirPath+"/app_state.pb";

    bridgeJ = env->NewGlobalRef(bridge);

    gApp = climateAppPtr(new climateApp(appInfo, serviceUid, senderUid, receiverUid, serializeStatePath));
    LOG4CPLUS_DEBUG(log, "created state_app");
    climateRequestProcessor = std::tr1::shared_ptr<ClimateRequestProcessor>(new ClimateRequestProcessor( gApp ));
    LOG4CPLUS_DEBUG(log, "created request processor");
    climateStateUpdater = std::tr1::shared_ptr<ClimateStateUpdater>(new ClimateStateUpdater(climateRequestProcessor,gApp, gJm, bridgeJ));
    LOG4CPLUS_DEBUG(log, "created state updater");
    gApp.get()->initStateApp(climateStateUpdater.get());
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_offPressed
(JNIEnv *, jobject)
{
    climateRequestProcessor->offRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_autoPressed
(JNIEnv *, jobject)
{
    climateRequestProcessor->autoRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_acPressed
(JNIEnv *, jobject)
{
    climateRequestProcessor->acRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_dualPressed
(JNIEnv *, jobject)
{
    climateRequestProcessor->syncRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_recircPressed
(JNIEnv *, jobject)
{
    climateRequestProcessor->recircRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_frontPressed
(JNIEnv *, jobject)
{
    climateRequestProcessor->frontRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_rearPressed
(JNIEnv *, jobject)
{
    climateRequestProcessor->rearRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_fanDownPressed
(JNIEnv *, jobject)
{
    climateRequestProcessor->fanLowRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_fanMiddlePressed
(JNIEnv *, jobject)
{
    climateRequestProcessor->fanMidRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_fanTopPressed
(JNIEnv *, jobject)
{
    climateRequestProcessor->fanHighRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_fanIncrease
(JNIEnv *, jobject)
{
    climateRequestProcessor->fanIncRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_fanDecrease
(JNIEnv *, jobject)
{
    climateRequestProcessor->fanDecRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_drvTempUp
(JNIEnv *, jobject)
{
    climateRequestProcessor->tempIncDrRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_drvTempDown
(JNIEnv *, jobject)
{
    climateRequestProcessor->tempDecDrRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_passTempUp
(JNIEnv *, jobject)
{
    climateRequestProcessor->tempIncPassRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_passTempDown
(JNIEnv *, jobject)
{
    climateRequestProcessor->tempDecPassRequest();
}


JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_saveState
(JNIEnv *, jobject)
{
    gApp->saveState();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_reloadState
(JNIEnv *, jobject)
{
    climateStateUpdater->initRequest();
}

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

#include "com_luxoft_ivilink_samples_climate_Bridge.hpp"
#include "state-app.h"
#include "crequestprocessor.h"
#include "cstateupdater.h"


#include "utils/misc/Logger.hpp"
#include "utils/android/StringConversion.hpp"
#include "utils/android/JniThreadHelper.hpp"
#include "utils/android/AppInfo.hpp"

#include <tr1/memory>

static JavaVM * gJm;
static jobject bridgeJ;

std::tr1::shared_ptr<state_app> gApp;
std::tr1::shared_ptr<CRequestProcessor> gRp;
std::tr1:: 	shared_ptr<CStateUpdater> gSu;


JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved) {
	gJm = jvm;
	return JNI_VERSION_1_6;
}


namespace {
    const iviLink::Service::Uid service_uid= iviLink::Service::Uid("ClimateService");
    const iviLink::Profile::ApiUid
    sender_uid    = iviLink::Profile::ApiUid("ClimateSenderProfile_PAPI_UID"),
        receiver_uid  = iviLink::Profile::ApiUid("ClimateReceiverProfile_PAPI_UID");
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
	
	std::string serializeStatePath = appInfo.privateDirPath+"/app_state.pb";
	
	bridgeJ = env->NewGlobalRef(bridge);
	
   	GOOGLE_PROTOBUF_VERIFY_VERSION;

   gApp = std::tr1::shared_ptr<state_app>(new state_app(appInfo, service_uid, sender_uid, receiver_uid, serializeStatePath));
	LOG4CPLUS_DEBUG(log, "created state_app");
   gRp = std::tr1::shared_ptr<CRequestProcessor>(new CRequestProcessor( gApp ));
	LOG4CPLUS_DEBUG(log, "created request processor");
   gSu = std::tr1::shared_ptr<CStateUpdater>(new CStateUpdater(gRp,gApp, gJm, bridgeJ));
	LOG4CPLUS_DEBUG(log, "created state updater");
	assert(gApp);
	assert(gRp);
	assert(gSu);
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_offPressed
(JNIEnv *, jobject)
{
	gRp->offRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_autoPressed
(JNIEnv *, jobject)
{
	gRp->autoRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_acPressed
(JNIEnv *, jobject)
{
	gRp->acRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_dualPressed
(JNIEnv *, jobject)
{
	gRp->syncRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_recircPressed
(JNIEnv *, jobject)
{
	gRp->recircRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_frontPressed
(JNIEnv *, jobject)
{
	gRp->frontRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_rearPressed
(JNIEnv *, jobject)
{
	gRp->rearRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_fanDownPressed
(JNIEnv *, jobject)
{
	gRp->fanLowRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_fanMiddlePressed
(JNIEnv *, jobject)
{
	gRp->fanMidRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_fanTopPressed
(JNIEnv *, jobject)
{
	gRp->fanHighRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_fanIncrease
(JNIEnv *, jobject)
{
	gRp->fanIncRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_fanDecrease
(JNIEnv *, jobject)
{
	gRp->fanDecRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_drvTempUp
(JNIEnv *, jobject)
{
	gRp->tempIncDrRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_drvTempDown
(JNIEnv *, jobject)
{
	gRp->tempDecDrRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_passTempUp
(JNIEnv *, jobject)
{
	gRp->tempIncPassRequest();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_passTempDown
(JNIEnv *, jobject)
{
	gRp->tempDecPassRequest();
}


JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_saveState
  (JNIEnv *, jobject)
  {
  	gApp->saveState();
  }

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_climate_Bridge_reloadState
  (JNIEnv *, jobject)
  {
  	gSu->initRequest();
  }

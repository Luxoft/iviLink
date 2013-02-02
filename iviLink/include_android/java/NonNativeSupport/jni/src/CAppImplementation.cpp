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
#include "CAppImplementation.hpp"
#include "JniThreadHelper.hpp"
#include "StringConversion.hpp"

CAppImplementation::CAppImplementation(iviLink::Android::AppInfo appInfo,
        const iviLink::Service::ListOfUids &services, jobject appExternal) :
        Application(services, appInfo), mjAppJava(appExternal), mAppInfo(appInfo), mLog(
                Logger::getInstance("CAppImplementation"))
{
    LOG4CPLUS_TRACE_METHOD(mLog, __PRETTY_FUNCTION__);
    iviLink::Android::JniThreadHelper jth(mAppInfo.javaVM);
    JNIEnv* env = jth.getEnv();
    jclass claz = env->GetObjectClass(mjAppJava);
    midInitDone = env->GetMethodID(claz, "initDone", "(Z)V");
    midIncomingServiceBeforeLoading = env->GetMethodID(claz, "incomingServiceBeforeLoading",
            "(Ljava/lang/String;)V");
    midIncomingServiceAfterLoading = env->GetMethodID(claz, "incomingServiceAfterLoading",
            "(Ljava/lang/String;)V");
    midServiceDropped = env->GetMethodID(claz, "serviceDropped", "(Ljava/lang/String;)V");
    midServiceLoadError = env->GetMethodID(claz, "serviceLoadError", "(Ljava/lang/String;)V");
    midLinkUp = env->GetMethodID(claz, "connectionEstablished", "()V");
    midLinkDown = env->GetMethodID(claz, "connectionLost", "()V");
}

void CAppImplementation::onInitDone(iviLink::ELaunchInfo launcher)
{
    LOG4CPLUS_TRACE_METHOD(mLog, __PRETTY_FUNCTION__);
    iviLink::Android::JniThreadHelper jth(mAppInfo.javaVM);
    JNIEnv* env = jth.getEnv();
    bool launchResult = (launcher == iviLink::LAUNCHED_BY_USER);
    env->CallVoidMethod(mjAppJava, midInitDone, static_cast<jboolean>(launchResult));
}

void CAppImplementation::callStringVoidMethod(const std::string& param, jmethodID methodID)
{
    LOG4CPLUS_TRACE_METHOD(mLog, __PRETTY_FUNCTION__);
    iviLink::Android::JniThreadHelper jth(mAppInfo.javaVM);
    JNIEnv* env = jth.getEnv();
    jstring jParam = iviLink::Android::StringConversion::stdToJ(param, env);
    env->CallVoidMethod(mjAppJava, methodID, jParam);
}

void CAppImplementation::onIncomingServiceBeforeLoading(const iviLink::Service::Uid &service)
{
    LOG4CPLUS_TRACE_METHOD(mLog, __PRETTY_FUNCTION__);
    callStringVoidMethod(service.value(), midIncomingServiceBeforeLoading);
}

void CAppImplementation::onIncomingServiceAfterLoading(const iviLink::Service::Uid &service)
{
    LOG4CPLUS_TRACE_METHOD(mLog, __PRETTY_FUNCTION__);
    callStringVoidMethod(service.value(), midIncomingServiceAfterLoading);
}

void CAppImplementation::onServiceDropped(const iviLink::Service::Uid &service)
{
    LOG4CPLUS_TRACE_METHOD(mLog, __PRETTY_FUNCTION__);
    callStringVoidMethod(service.value(), midServiceDropped);
}

void CAppImplementation::onServiceLoadError(const iviLink::Service::Uid &service)
{
    LOG4CPLUS_TRACE_METHOD(mLog, __PRETTY_FUNCTION__);
    callStringVoidMethod(service.value(), midServiceLoadError);
}

void CAppImplementation::onLinkUp()
{
    LOG4CPLUS_TRACE_METHOD(mLog, __PRETTY_FUNCTION__);
    iviLink::Android::JniThreadHelper jth(mAppInfo.javaVM);
    JNIEnv* env = jth.getEnv();
    env->CallVoidMethod(mjAppJava, midLinkUp);
}

void CAppImplementation::onPhysicalLinkDown()
{
    LOG4CPLUS_TRACE_METHOD(mLog, __PRETTY_FUNCTION__);
    iviLink::Android::JniThreadHelper jth(mAppInfo.javaVM);
    JNIEnv* env = jth.getEnv();
    env->CallVoidMethod(mjAppJava, midLinkDown);
}

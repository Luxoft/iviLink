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
#include "AndroidLaunchInfoProcessor.hpp"
#include "JniThreadHelper.hpp"
#include "StringConversion.hpp"

using namespace iviLink::Android;

namespace iviLink
{
namespace AMP
{

AndroidLaunchInfoProcessor::AndroidLaunchInfoProcessor(JavaVM * javaVM, jobject checker)
: mLogger(Logger::getInstance("iviLink.AMP.AndroidLaunchInfoProcessor"))
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mJavaVM = javaVM;
    JniThreadHelper jth(mJavaVM);
    JNIEnv * env = jth.getEnv();
    mChecksPerformer = env->NewGlobalRef(checker);
    jclass checkerClaz = env->GetObjectClass(mChecksPerformer);
    assert (checkerClaz != 0);
    mCheckMethod = env->GetMethodID(checkerClaz, "isLaunchInfoValid","(Ljava/lang/String;)Z");
    assert (mCheckMethod != 0);
    mTrimMethod = env->GetMethodID(checkerClaz, "trimLaunchInfo","(Ljava/lang/String;)Ljava/lang/String;");
    assert (mTrimMethod != 0);
}

bool AndroidLaunchInfoProcessor::isLaunchInfoValid(const std::string& launchInfo) const
{
    JniThreadHelper jth(mJavaVM);
    JNIEnv * env = jth.getEnv();
    jstring jLaunchInfo = StringConversion::stdToJ(launchInfo, env);
    return env->CallBooleanMethod(mChecksPerformer, mCheckMethod, jLaunchInfo);
}

std::string AndroidLaunchInfoProcessor::trimLaunchInfo(const std::string& launchInfo) const
{
    JniThreadHelper jth(mJavaVM);
    JNIEnv * env = jth.getEnv();
    jstring jLaunchInfo = StringConversion::stdToJ(launchInfo, env);
    jstring trimmedInfo = (jstring)env->CallObjectMethod(mChecksPerformer, mTrimMethod, jLaunchInfo);
    return StringConversion::jToStd(trimmedInfo, env);
}

AndroidLaunchInfoProcessor::~AndroidLaunchInfoProcessor ()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    JniThreadHelper jth(mJavaVM);
    JNIEnv * env = jth.getEnv();
    env->DeleteGlobalRef(mChecksPerformer);
}

}
}

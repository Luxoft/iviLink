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
#include <cassert>

#include "JniPlayer.hpp"
#include "StringConversion.hpp"
#include "JniThreadHelper.hpp"

JniPlayer::JniPlayer(JavaVM * pJM, jobject callbacks)
        : mpJM(pJM), mjCallbacks(callbacks), mLogger(
                Logger::getInstance("iviLink.samples.Media.CAndroidPlayerImpl"))
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    assert(mpJM);
    iviLink::Android::JniThreadHelper jth(mpJM);
    JNIEnv * env = jth.getEnv();
    jclass claz = env->GetObjectClass(mjCallbacks);
    midPlay = env->GetMethodID(claz, "play", "(Ljava/lang/String;Ljava/lang/String;)I");
    assert(midPlay != NULL);
    midResume = env->GetMethodID(claz, "resume", "()I");
    assert(midResume != NULL);
    midPause = env->GetMethodID(claz, "pause", "()V");
    assert(midPause != NULL);
    midStop = env->GetMethodID(claz, "stop", "()V");
    assert(midStop != NULL);
    midTrackEnded = env->GetMethodID(claz, "isTrackEnded", "()Z");
    assert(midTrackEnded != NULL);
}

void JniPlayer::stop()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    iviLink::Android::JniThreadHelper jth(mpJM);
    JNIEnv * env = jth.getEnv();
    env->CallVoidMethod(mjCallbacks, midStop);
}

void JniPlayer::pause()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    iviLink::Android::JniThreadHelper jth(mpJM);
    JNIEnv * env = jth.getEnv();
    env->CallVoidMethod(mjCallbacks, midPause);
}

int JniPlayer::play(const std::string& source, const std::string& avform)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    iviLink::Android::JniThreadHelper jth(mpJM);
    JNIEnv * env = jth.getEnv();
    jstring jSource = iviLink::Android::StringConversion::stdToJ(source, env);
    jstring jForm = iviLink::Android::StringConversion::stdToJ(avform, env);
    int res = static_cast<int>(env->CallIntMethod(mjCallbacks, midPlay, jSource, jForm));
    return res;
}

int JniPlayer::resume()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    iviLink::Android::JniThreadHelper jth(mpJM);
    JNIEnv * env = jth.getEnv();
    int res = static_cast<int>(env->CallIntMethod(mjCallbacks, midResume));
    return res;
}

void JniPlayer::volumeUp()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_WARN(mLogger, "This implementation does nothing");
}

void JniPlayer::volumeDown()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_WARN(mLogger, "This implementation does nothing");
}

bool JniPlayer::isTrackEnded()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    iviLink::Android::JniThreadHelper jth(mpJM);
    JNIEnv * env = jth.getEnv();
    bool result = static_cast<bool>(env->CallBooleanMethod(mjCallbacks, midTrackEnded));
    return result;
}

bool JniPlayer::isTrackInErrorState()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_TRACE(mLogger, "empty stub");
    return false;
}


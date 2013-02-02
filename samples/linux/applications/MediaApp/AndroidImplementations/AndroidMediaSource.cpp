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
#include "AndroidMediaSource.hpp"
#include "JniThreadHelper.hpp"
#include "StringConversion.hpp"
#include <cassert>


namespace
{
    std::list<std::string> InitializeList()
    {
        std::list<std::string> result;
        result.push_back("/alarms"); 
        result.push_back("/notifications"); 
        result.push_back("/ringtones"); 
        result.push_back("/media/alarms"); 
        result.push_back("/media/notifications"); 
        result.push_back("/media/ringtones"); 
        result.push_back("/media/audio/alarms"); 
        result.push_back("/media/audio/notifications"); 
        result.push_back("/media/audio/ringtones"); 
        result.push_back("/Android/data/"); 
        result.push_back("/Notifications/"); 
        return result;
    }
}

AndroidMediaSource::AndroidMediaSource(JavaVM * pJM, jobject callbacks, const std::string& pathToFiles)
:
    Playlist(pathToFiles),
    mpJM(pJM),
    mjCallbacks(callbacks),
    mLogger(Logger::getInstance("iviLink.samples.Media.CAndroidMediaSourceImpl"))
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    
}

void AndroidMediaSource::startVlcServer(const int pipeFds)
{
   LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
   iviLink::Android::JniThreadHelper jth(mpJM);
   JNIEnv * env = jth.getEnv();
   jclass claz = env->GetObjectClass(mjCallbacks);
   
   jmethodID launchVlcServer = env->GetMethodID(claz, "startVlcServer", "(I)Z");
   env->CallBooleanMethod(mjCallbacks,launchVlcServer, static_cast<jint>(pipeFds));
}

std::list<std::string> AndroidMediaSource::getFolderBlacklist() const
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    static std::list<std::string> blacklist = InitializeList();
    return blacklist;
}

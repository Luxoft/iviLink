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
#include <unistd.h>
#include <sys/param.h>

#include "CServerControl.hpp"
#include "MediaAppCommon.hpp"
#include "Exit.hpp"

static const char MediaServerName[]="MediaServer";

Logger CServerControl::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Profiles.CServerControl"));

CServerControl::CServerControl(IMediaControlServerProfile_API::Callbacks* callbacks)
: //mpMutex(new CMutex()),
 mVTranscode(VCODE)
, mATranscode(ACODE)
, mServerPath(MediaServerName)
, mFd(0)
, mStop ("stop;")
, mResume("resume;")
, mPause("pause;") 
, mAudio("audio")
, mVideo("video")
, mPlay ("play;")
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    assert(callbacks);
    pipe (fds); 
    char path[MAXPATHLEN];
    getcwd(path, MAXPATHLEN);
    LOG4CPLUS_INFO(msLogger, "!!!!!!!!!!!!!!!!!!!!!!!!!!");
    LOG4CPLUS_INFO(msLogger, "path: " + std::string(path));
    mServerPath = std::string(path) + "/" + mServerPath;
    LOG4CPLUS_INFO(msLogger, "full path: "+ mServerPath);
#ifndef ANDROID
    pid_t pid = fork();

    switch (pid) {
        case -1:
        {
            LOG4CPLUS_ERROR(msLogger, "after fork pid = -1"); 
            return;
            break;
        }
        case 0:
        {
            LOG4CPLUS_INFO(msLogger, "child");
            close(fds[1]);
            char fd_str[15];
            sprintf(fd_str, "%i", fds[0]);
            if (execl(mServerPath.c_str(), mServerPath.c_str(), fd_str,  NULL)) {
                 LOG4CPLUS_ERROR(msLogger, "execl(server) failed"); 
                 killProcess(1);
            }
            killProcess(1);
        }
        default:
        {
            LOG4CPLUS_INFO(msLogger, "parent");
            close(fds[0]);
            file = fdopen(fds[1], "w");
           break;
        }        
    }
#else
    callbacks->startVlcServer(fds[0], true);
    file = fdopen(fds[1], "w");
#endif //ANDROID
}

CServerControl::~CServerControl() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    //delete mpMutex;
    close(fds[1]);
}

bool CServerControl::sendCommand(std::string const& command) {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    std::string msg = command;
    bool res = false;
    //mpMutex->lock();
    res = writer(msg.c_str());
    //mpMutex->unlock();
    if (res)
        return 1;
    return 0;
}

bool CServerControl::sendPlay(std::string const& port, std::string const& path, std::string const& streamName, std::string const& avform) {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    std::string sout;
    if (avform == mAudio)
    {
        LOG4CPLUS_INFO(msLogger, "get audio source");
        sout = mPlay  + mATranscode + port + "/" + streamName + "}";
    } 
    else if (avform  == mVideo)
    {
        LOG4CPLUS_INFO(msLogger, "get video source");
        sout = mPlay  + mVTranscode + port + "/" + streamName + "}";
    }
    std::string msg = sout + ";" + path;
    bool res = false;
    //mpMutex->lock();
    res = writer(msg.c_str());
    //mpMutex->unlock();
    if (res)
        return 1;
    return 0;
}

int CServerControl::writer(const char* message) 
{   
       LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
       int result = fprintf(file, "%s\n", message); 
       /* Write the message to the stream, and send it off immediately.   */       
       fflush(file); 
       /* Snooze a while.  */ 
       return result;
} 

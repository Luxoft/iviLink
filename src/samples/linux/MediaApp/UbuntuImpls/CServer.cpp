/* 
 * iviLINK SDK, version 1.1.19
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
 */ 
 

#ifndef ANDROID

#include <iostream>
#include "CServer.h"

#define LOCAL_SOURCE "rtsp://127.0.0.1:5544/local_stream"

Logger CServer::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Applications.Media.CServer"));

CServer::CServer()
:
    mLocalSource(LOCAL_SOURCE)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));
    mMutex = new CRWMutex();
    mVlc = libvlc_new(0, NULL);
    assert(mVlc);
    mMedia_name = MEDIA_NAME;
}

CServer::~CServer() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    libvlc_vlm_release(mVlc);
    delete mMutex;
}

void CServer::play(std::string const& url, std::string const& form) {

    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    libvlc_vlm_release(mVlc);
     
    const char * mUrl = url.c_str();
    const char * mSout;
    if (form == AUDIO)
    {
        mSout = (std::string(ACODE) + std::string("5544/local_stream}")).c_str();
    }
    else if (form == VIDEO)
    {
        mSout = (std::string(VCODE) + "5544/local_stream}").c_str();
    }

    //mMutex->lockWrite();
    if (libvlc_vlm_add_broadcast(mVlc, mMedia_name, mUrl, mSout, 0, NULL, true, false) == ERROR_PLAY)
    {	
        LOG4CPLUS_WARN(msLogger, "!!!!!!!!!!!!!vlc server cannot start broadcast");
    }
    if (libvlc_vlm_play_media(mVlc, mMedia_name) == ERROR_PLAY)
    {
        LOG4CPLUS_WARN(msLogger, "!!!!!!!!!!!vlc server cannot start playing media");
    }
    //mMutex->unlockWrite();
}

void CServer::stop() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mMutex->lockWrite();
    if (libvlc_vlm_stop_media(mVlc, mMedia_name) == ERROR_PLAY)
    {

        LOG4CPLUS_WARN(msLogger, "vlc server cannot stop playing media");
    }
    libvlc_vlm_release(mVlc);
    mMutex->unlockWrite();
}

void CServer::pause() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mMutex->lockWrite();
    if (libvlc_vlm_pause_media(mVlc, mMedia_name) == ERROR_PLAY)
    {

        LOG4CPLUS_WARN(msLogger, "vlc server cannot pause playing media");
    }
    mMutex->unlockWrite();
}

void CServer::resume() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mMutex->lockWrite();
    if (libvlc_vlm_play_media(mVlc, mMedia_name) == ERROR_PLAY)
    {
        LOG4CPLUS_WARN(msLogger, "vlc server cannot start playing media");
    }
    mMutex->unlockWrite();
}

std::string CServer::getSource()
{
    return mLocalSource;
}
#endif //ANDROID

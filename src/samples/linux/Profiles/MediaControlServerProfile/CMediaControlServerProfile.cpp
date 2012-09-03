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



#include "CMediaControlServerProfile.hpp"

#include "framework/components/ChannelSupervisor/Tube/ChannelSupervisorTube.hpp"
#include "samples/linux/Profiles/mediaCommon/CSenderThread.hpp"
#include "utils/threads/CMutex.hpp"
#include "utils/threads/CSignalSemaphore.hpp"

#include <cstring>
#include <cassert>
#include <iostream>

Logger CMediaControlServerProfile::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Profiles.CMediaControlServerProfile"));

CMediaControlServerProfile::CMediaControlServerProfile(iviLink::Profile::IProfileCallbackProxy* pCbProxy)
: mChannelID(0)
, mpAppCallbacks(static_cast<IMediaControlServerProfile_API::Callbacks*> (pCbProxy))
, mpSenderThread(new CSenderThread(this))
, mpReqMutex(new CMutex())
, mpReqSemaphore(new CSignalSemaphore())
, mBe(true)
, mTag("CMediaControlServerProfile")
, mPath("/home/")
, mIp("127.0.0.1")
, mPort("5544")
, mRtsp("rtsp://")
, mStreamName("stream_")
, mpServControl(new CServerControl())
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));
    // get IP from ChannelSupervisorTube.hpp"
    std::string type;
    std::string remAdd;
    //the following function should be uncommented
    //if (iviLink::ChannelSupervisor::UnstableAPI::getConnectionAddr(type, mIp, remAdd) == ERR_OK && type == "TCP/IP")
    //{
      ;// we've obtained our IP address
    //}
    
}

CMediaControlServerProfile::~CMediaControlServerProfile() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mBe = false;

    mpSenderThread->disconnect();
    delete mpSenderThread;
    delete mpReqMutex;
    delete mpServControl;

    while (!mReqQueue.empty()) {
        CBuffer buf = mReqQueue.front();
        mReqQueue.pop();
        delete [] buf.getBuffer();
    }
}

void CMediaControlServerProfile::play(std::string const trackName, std::string const avform)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    std::string streamName = mStreamName + trackName;
    std::string path = mPath + trackName;
    std::string source = mRtsp + mIp + ":" + mPort + "/" + streamName;
    if (mpServControl->sendPlay(mPort, path, streamName, avform))
    {
        prepareRequest(PLAY, true, trackName, source, avform);
        mpAppCallbacks->onServerPlay(trackName, source, avform);
    }
}

void CMediaControlServerProfile::pause() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    prepareRequest(PAUSE, false, mTag, mTag, mTag);
    mpServControl->sendCommand(mpServControl->mPause);
}

void CMediaControlServerProfile::stopStreaming() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    prepareRequest(STOP, false, mTag, mTag, mTag);
    mpServControl->sendCommand(mpServControl->mStop);
}

void CMediaControlServerProfile::resume() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__); 
    if (mpServControl->sendCommand(mpServControl->mResume))
        prepareRequest(RESUME, false, mTag, mTag, mTag);
}

void CMediaControlServerProfile::sync() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    prepareRequest(SYNC, false,  mTag, mTag, mTag);
}

void CMediaControlServerProfile::unsync() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    prepareRequest(UNSYNC, false,  mTag, mTag, mTag);
}

void CMediaControlServerProfile::toggle() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    prepareRequest(TOGGLE, false,  mTag, mTag, mTag);
}

void CMediaControlServerProfile::setPath(std::string const& path) {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mPath = path;
}

void CMediaControlServerProfile::prepareRequest(PROCEDURES_CONTROL_IDS proc, bool has_events, std::string const& event_f, std::string const& event_s, std::string const& event_t) {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    const UInt32 size = 4;
    CBuffer buf(new UInt8[size], size);
    bool res = true;
    res = res && buf.write((UInt16) proc);
    if (has_events) {
        res = res && buf.write(event_f);
        res = res && buf.write(event_s);
        res = res && buf.write(event_t);
    }
    if (res) {
        mpReqMutex->lock();
        mReqQueue.push(buf);
        mpReqMutex->unlock();
        mpReqSemaphore->signal();
    } else {
        LOG4CPLUS_ERROR(msLogger, "Unable to prepare buffer - buffer size is not enough");
        delete [] buf.getBuffer();
    }
}

void CMediaControlServerProfile::onEnable() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mChannelID = iviLink::Channel::allocateChannel(mTag, this);
    if (mChannelID) {
        LOG4CPLUS_INFO(msLogger, "Channel allocated, starting the communication...");
        mpSenderThread->start();
    } else {
        LOG4CPLUS_WARN(msLogger, "allocate Channel failed");
    }
}

void CMediaControlServerProfile::onDisable() {
    CError err = iviLink::Channel::deallocateChannel(mChannelID);
    if (!err.isNoError()) {
        LOG4CPLUS_WARN(msLogger, "Unable to deallocate channel: " + static_cast<std::string> (err));
    }
}

//from CChannelHandler

void CMediaControlServerProfile::bufferReceived(const iviLink::Channel::tChannelId channel, iviLink::CBuffer const& buffer) {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (mChannelID != channel) {
        LOG4CPLUS_INFO(msLogger, "mChannelID != channel_id");
        return;
    } else {
        LOG4CPLUS_INFO(msLogger, "mChannelID == channel_id");
    }

    bool res = true;
    UInt16 tmp;
    if (!buffer.read(tmp)) {
        LOG4CPLUS_WARN(msLogger, "Unable to read request type from buffer");
        return;
    }

    PROCEDURES_CONTROL_IDS proc = static_cast<PROCEDURES_CONTROL_IDS> (tmp);
    switch (proc) {
        case STOP:
        {
            LOG4CPLUS_INFO(msLogger, "case STOP");
            mpAppCallbacks->onServerStop();
            mpServControl->sendCommand(mpServControl->mStop);            
        }
            break;
        case PAUSE:
        {
            LOG4CPLUS_INFO(msLogger, "case PAUSE");
            mpAppCallbacks->onServerPause();
            mpServControl->sendCommand(mpServControl->mPause);
            break;
        }
        case RESUME:
        {
            LOG4CPLUS_INFO(msLogger, "case RESUME");
            mpAppCallbacks->onServerResume();
            resume();
        }
            break;
        case SYNC:
        {
            LOG4CPLUS_INFO(msLogger, "case SYNC");
            mpAppCallbacks->onServerSync();
            break;
        }
        case UNSYNC:
        {
            LOG4CPLUS_INFO(msLogger, "case UNSYNC");
            mpAppCallbacks->onServerUnsync();
        }
            break;
        case TOGGLE:
        {
            LOG4CPLUS_INFO(msLogger, "case TOGGLE");
            mpAppCallbacks->onServerToggle();
        }
            break;
        case PLAY:
        {
            LOG4CPLUS_INFO(msLogger, "case PLAY");
            std::string trackName;
            std::string avform;
            res = res && buffer.read(trackName);
            res = res && buffer.read(avform);
            if (res)
               play(trackName, avform);
        }
            break;
        default:
        {
            LOG4CPLUS_WARN(msLogger, "Unknown request type");
            return;
        }
            break;
    }

    if (!res) {
        LOG4CPLUS_WARN(msLogger, "Unable to read event from buffer");
        return;
    }

}

void CMediaControlServerProfile::channelDeletedCallback(const UInt32 channel_id) {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

    if (mChannelID == channel_id)
        mChannelID = 0;

    CError err = iviLink::Channel::deallocateChannel(channel_id);
    if (!err.isNoError()) {
        LOG4CPLUS_WARN(msLogger, "Unable to deallocate channel: " + static_cast<std::string> (err));
    }
}

void CMediaControlServerProfile::connectionLostCallback() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
}

void CMediaControlServerProfile::senderLoop() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    while (mBe) {
        if (hasRequests()) {
            handleRequest();
        } else {
            mpReqSemaphore->wait();
        }
    }
}

bool CMediaControlServerProfile::hasRequests() {
    mpReqMutex->lock();
    bool res = !mReqQueue.empty();
    mpReqMutex->unlock();
    return res;
}

void CMediaControlServerProfile::handleRequest() {
    mpReqMutex->lock();
    CBuffer buf = mReqQueue.front();
    mReqQueue.pop();
    mpReqMutex->unlock();

    CError err = iviLink::Channel::sendBuffer(mChannelID, buf);
    if (!err.isNoError()) {
        LOG4CPLUS_INFO(msLogger, "CMediaControlServerProfile::handleRequest() :: Send error"
                + static_cast<std::string> (err));
    }

    delete [] buf.getBuffer();
}


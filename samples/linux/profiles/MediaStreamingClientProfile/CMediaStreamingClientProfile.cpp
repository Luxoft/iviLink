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


#include "CMediaStreamingClientProfile.hpp"

#include "CSenderThread.hpp"
#include "CMutex.hpp"
#include "CSignalSemaphore.hpp"
#include "Exit.hpp"

#include <cstring>
#include <cassert>
#include <iostream>

Logger CMediaStreamingClientProfile::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Profiles.CMediaStreamingClientProfile"));

CMediaStreamingClientProfile::CMediaStreamingClientProfile(iviLink::Profile::IProfileCallbackProxy* pCbProxy)
: mChannelID(0)
, mpAppCallbacks(static_cast<IMediaStreamingClientProfile_API::Callbacks*> (pCbProxy))
, mpSenderThread(new CSenderThread(this))
, mpReqMutex(new CMutex())
, mpReqSemaphore(new CSignalSemaphore())
, mBe(true)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));
    mInfo.auForm = "m4a"; 
    mInfo.viForm = "h264";
}

CMediaStreamingClientProfile::~CMediaStreamingClientProfile() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mBe = false;

    mpSenderThread->disconnect();
    delete mpSenderThread;
    delete mpReqMutex;

    while (!mReqQueue.empty()) {
        mReqQueue.pop();
    }
}

void CMediaStreamingClientProfile::streamingAccepted() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    prepareRequest(STREAMINGACCEPTED);
}

void CMediaStreamingClientProfile::streamingUnaccepted() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    prepareRequest(STREAMINGUNACCEPTED);
}

void CMediaStreamingClientProfile::prepareRequest(PROCEDURES_STREAMING_IDS proc)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   std::string msg;
   msg = static_cast<UInt8>(proc);
   mpReqMutex->lock();
   mReqQueue.push(msg);
   mpReqMutex->unlock();
   mpReqSemaphore->signal();     
}

void CMediaStreamingClientProfile::procInfo(std::string const& info) 
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    size_t pos = info.find(";"); 
    LOG4CPLUS_WARN(msLogger, info.substr(0, pos));
    LOG4CPLUS_WARN(msLogger, info.substr(pos + 1));
    if (mInfo.auForm == info.substr(0, pos) && mInfo.viForm == info.substr(pos + 1))
    { 
        streamingAccepted();
        mpAppCallbacks->onStreamingClientAccepted(true);
    }   
    else 
    {
        streamingUnaccepted();
        mpAppCallbacks->onStreamingClientUnaccepted(true);
    }
                          
}

void CMediaStreamingClientProfile::onEnable() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    iviLink::Error err = iviLink::Channel::allocateChannelAsClient(MEDIA_STREAMING_TAG, this, mChannelID, eRealTime);
    if (err.isNoError()) 
    {
        LOG4CPLUS_INFO(msLogger, "Channel allocated, starting the communication...");
        mpSenderThread->start();
    } else {
        LOG4CPLUS_WARN(msLogger, "allocate Channel failed");
        killProcess(1);
    }
}

void CMediaStreamingClientProfile::onDisable() {
    iviLink::Error err = iviLink::Channel::deallocateChannel(mChannelID);
    if (!err.isNoError()) {
        LOG4CPLUS_WARN(msLogger, "Unable to deallocate channel: " + static_cast<std::string> (err));
    }
}

//from CChannelHandler

void CMediaStreamingClientProfile::onBufferReceived(const iviLink::Channel::tChannelId channel, iviLink::Buffer const& buffer) 
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (mChannelID != channel) {
        LOG4CPLUS_INFO(msLogger, "mChannelID != channel_id");
        return;
    } else {
        LOG4CPLUS_INFO(msLogger, "mChannelID == channel_id");
    }

 
   UInt8 *incomingData = buffer.getBuffer();
   int read_size = buffer.getSize();

   LOG4CPLUS_INFO(msLogger, "Procedure ID = " + convertIntegerToString(incomingData[0]));


   if(incomingData[0] == SENDSTREAMINGINFO)
   {
        LOG4CPLUS_INFO(msLogger, "case SENDSTREAMINGINFO");
        std::string info((char*)(incomingData + 1), read_size - 1);
        LOG4CPLUS_INFO(msLogger, "info  = " + info);
        procInfo(info);
   }
   else
   {
        LOG4CPLUS_INFO(msLogger, "unknown procedure ID");
   }

}

void CMediaStreamingClientProfile::onChannelDeleted(const UInt32 channel_id) {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

    if (mChannelID == channel_id)
        mChannelID = 0;

    iviLink::Error err = iviLink::Channel::deallocateChannel(channel_id);
    if (!err.isNoError()) {
        LOG4CPLUS_WARN(msLogger, "Unable to deallocate channel: " + static_cast<std::string> (err));
    }
}

void CMediaStreamingClientProfile::onConnectionLost() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
}

void CMediaStreamingClientProfile::senderLoop() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    while (mBe) {
        if (hasRequests()) {
            handleRequest();
        } else {
            mpReqSemaphore->wait();
        }
    }
}

bool CMediaStreamingClientProfile::hasRequests() {
    mpReqMutex->lock();
    bool res = !mReqQueue.empty();
    mpReqMutex->unlock();
    return res;
}

void CMediaStreamingClientProfile::handleRequest()
{
   mpReqMutex->lock();
   std::string msg;
   msg = mReqQueue.front();
   mReqQueue.pop();
   mpReqMutex->unlock();

   iviLink::Error err = iviLink::Channel::sendBuffer(mChannelID, msg.c_str(), msg.length());
   if (!err.isNoError())
   {
      LOG4CPLUS_INFO(msLogger, "CMediaSourceServerProfile::handleRequest() :: Send error"
         + static_cast<std::string>(err));
   }
}

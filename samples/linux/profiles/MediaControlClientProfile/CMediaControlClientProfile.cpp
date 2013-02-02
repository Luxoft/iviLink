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


#include "CMediaControlClientProfile.hpp"

#include "CSenderThread.hpp"
#include "CMutex.hpp"
#include "CSignalSemaphore.hpp"
#include "Exit.hpp"

#include <cstring>
#include <cassert>
#include <iostream>

Logger CMediaControlClientProfile::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Profiles.CMediaControlClientProfile"));

CMediaControlClientProfile::CMediaControlClientProfile(iviLink::Profile::IProfileCallbackProxy* pCbProxy)
: mChannelID(0)
, mpAppCallbacks(static_cast<IMediaControlClientProfile_API::Callbacks*> (pCbProxy))
, mpSenderThread(new CSenderThread(this))
, mpReqMutex(new CMutex())
, mpReqSemaphore(new CSignalSemaphore())
, mBe(true)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));
}

CMediaControlClientProfile::~CMediaControlClientProfile() 
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mBe = false;

    mpSenderThread->disconnect();
    delete mpSenderThread;
    delete mpReqMutex;

    while (!mReqQueue.empty()) {
        mReqQueue.pop();
    }
}


void CMediaControlClientProfile::split(const std::string& str, const std::string& delimiters , std::vector<std::string>& tokens)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}


void CMediaControlClientProfile::play(std::string const& trackName,std::string const&  trackUid, std::string const& avform)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    prepareRequest(PLAY, true, trackName, trackUid, avform);
}

void CMediaControlClientProfile::pause() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    prepareRequest(PAUSE, false);
}

void CMediaControlClientProfile::stop() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    prepareRequest(STOP, false);
}

void CMediaControlClientProfile::serverStop() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    prepareRequest(SERVERSTOP, false);
}

void CMediaControlClientProfile::resume() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    prepareRequest(RESUME, false);
}

void CMediaControlClientProfile::sync() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    prepareRequest(SYNC, false);
}

void CMediaControlClientProfile::unsync() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    prepareRequest(UNSYNC, false);
}

void CMediaControlClientProfile::toggle() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    prepareRequest(TOGGLE, false);
}


void CMediaControlClientProfile::prepareRequest(PROCEDURES_CONTROL_IDS proc, bool has_events, std::string const& event_f, std::string const& event_s, std::string const& event_t)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   std::string msg;
   msg = static_cast<UInt8>(proc);
   if (has_events)
   { 
	    msg += event_f + ";" + event_s + ";" + event_t;
        LOG4CPLUS_INFO(msLogger, "message" + msg);
   }
   mpReqMutex->lock();
   mReqQueue.push(msg);
   mpReqMutex->unlock();
   mpReqSemaphore->signal();     


}

void CMediaControlClientProfile::onEnable() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    iviLink::Error err = iviLink::Channel::allocateChannelAsClient(MEDIA_CONTROL_TAG, this, mChannelID, eRealTime);
    if (err.isNoError()) {
        LOG4CPLUS_INFO(msLogger, "Channel allocated, starting the communication...");
        mpSenderThread->start();
    } else {
        LOG4CPLUS_WARN(msLogger, "allocate Channel failed");
        killProcess(1);
    }
}

void CMediaControlClientProfile::onDisable() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    iviLink::Error err = iviLink::Channel::deallocateChannel(mChannelID);
    if (!err.isNoError()) {
        LOG4CPLUS_WARN(msLogger, "Unable to deallocate channel: " + static_cast<std::string> (err));
    }
}

//from CChannelHandler

void CMediaControlClientProfile::onBufferReceived(const iviLink::Channel::tChannelId channel, iviLink::Buffer const& buffer) {
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

   if(incomingData[0] == STOP)
   {
            LOG4CPLUS_INFO(msLogger, "case STOP");
            mpAppCallbacks->onClientStop(true);
   }
   else if(incomingData[0] == PAUSE)
   {
            LOG4CPLUS_INFO(msLogger, "case PAUSE");
            mpAppCallbacks->onClientPause(true);
   }
   else if(incomingData[0] == RESUME)
   {
            LOG4CPLUS_INFO(msLogger, "case RESUME");
            mpAppCallbacks->onClientResume(true);
   }
   else if(incomingData[0] == SYNC)
   {
            LOG4CPLUS_INFO(msLogger, "case SYNC");
            mpAppCallbacks->onClientSync(true);
   }
   else if(incomingData[0] == UNSYNC)
   {
            LOG4CPLUS_INFO(msLogger, "case UNSYNC");
            mpAppCallbacks->onClientUnsync(true);
   }
   else if(incomingData[0] == TOGGLE)
   {
            LOG4CPLUS_INFO(msLogger, "case TOGGLE");
            mpAppCallbacks->onClientToggle(true);
   }
   else if(incomingData[0] == CHANGETRACK)
   {
            LOG4CPLUS_INFO(msLogger, "case CHANGETRACK");
            std::string message((char*)(incomingData + 1), read_size - 1);
            LOG4CPLUS_INFO(msLogger, "message  = " + message);
            vector<string> parse_result;
            split(message, ";", parse_result);
            istringstream buffer(parse_result[0]);
            int value;
            buffer >> value;
            mpAppCallbacks->onClientChangeHighlightedTrack(value, true);
   }
   else if(incomingData[0] == PLAY)
   {
            LOG4CPLUS_INFO(msLogger, "case PLAY");
            std::string message((char*)(incomingData + 1), read_size - 1);
            LOG4CPLUS_INFO(msLogger, "message  = " + message);
            vector<string> parse_result;
            split(message, ";", parse_result);
            if (parse_result.size() > 2)
            {
            	mpAppCallbacks->onClientPlay(parse_result[0],parse_result[1], parse_result[2], parse_result[3], true);
            }
            else 
	          {
		          LOG4CPLUS_INFO(msLogger, "cannot parse message");
            }

   }
   else
   {
        LOG4CPLUS_INFO(msLogger, "unknown procedure ID");
   }

}

void CMediaControlClientProfile::onChannelDeleted(const UInt32 channel_id) {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

    if (mChannelID == channel_id)
        mChannelID = 0;

    iviLink::Error err = iviLink::Channel::deallocateChannel(channel_id);
    if (!err.isNoError()) {
        LOG4CPLUS_WARN(msLogger, "Unable to deallocate channel: " + static_cast<std::string> (err));
    }
}

void CMediaControlClientProfile::onConnectionLost() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
}

void CMediaControlClientProfile::senderLoop() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    while (mBe) {
        if (hasRequests()) {
            handleRequest();
        } else {
            mpReqSemaphore->wait();
        }
    }
}

bool CMediaControlClientProfile::hasRequests() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mpReqMutex->lock();
    bool res = !mReqQueue.empty();
    mpReqMutex->unlock();
    return res;
}

void CMediaControlClientProfile::handleRequest()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   mpReqMutex->lock();
   std::string msg;
   msg = mReqQueue.front();
   mReqQueue.pop();
   mpReqMutex->unlock();


   iviLink::Error err = iviLink::Channel::sendBuffer(mChannelID, msg.c_str(), msg.length());
   if (!err.isNoError())
   {
      LOG4CPLUS_INFO(msLogger, "CMediaControlClienProfile::handleRequest() :: Send error"
         + static_cast<std::string>(err));
   }
}

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


#include "CMediaControlServerProfile.hpp"

#include "UnstableAPI.hpp"
#include "CSenderThread.hpp"
#include "CMutex.hpp"
#include "CSignalSemaphore.hpp"
#include "Exit.hpp"

#include <cstring>
#include <cassert>
#include <iostream>
#include <sstream>

Logger CMediaControlServerProfile::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Profiles.CMediaControlServerProfile"));

CMediaControlServerProfile::CMediaControlServerProfile(iviLink::Profile::IProfileCallbackProxy* pCbProxy)
: mChannelID(0)
, mpAppCallbacks(static_cast<IMediaControlServerProfile_API::Callbacks*> (pCbProxy))
, mpSenderThread(new CSenderThread(this))
, mpReqMutex(new CMutex())
, mpReqSemaphore(new CSignalSemaphore())
, mBe(true)
, mPath("/home/")
, mIp("127.0.0.1")
, mPort("5544")
, mRtsp("rtsp://")
, mStreamName("stream_")
, mPortInterval(100)
, mpServControl(new CServerControl(static_cast<IMediaControlServerProfile_API::Callbacks*> (pCbProxy)))
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));
    

    //convert mPort to int
    std::istringstream ist(mPort);
    ist >> mInitialPort;   
    
    // get IP from ChannelSupervisorTube.hpp"
    ConnectionInformation conInfo;
    if(! ::getConnectionInformation(&conInfo).isNoError())
    {
        LOG4CPLUS_FATAL(msLogger, "Unable to obtain IP address");
        return;
    }

    mIp = conInfo.getLocalAddress();
    LOG4CPLUS_INFO(msLogger, "After get IP address");
    LOG4CPLUS_INFO(msLogger, "local ip: " + conInfo.getLocalAddress());
    LOG4CPLUS_INFO(msLogger, "remote ip: " + conInfo.getRemoteAddress());
    LOG4CPLUS_INFO(msLogger, "connection type: " + conInfo.getConnectionType());
    if  (strcmp(conInfo.getConnectionType().c_str(), "TCP/IP") != 0)
    {
       LOG4CPLUS_WARN(msLogger, "May not work without TCP connection");
    }

}


CMediaControlServerProfile::~CMediaControlServerProfile() 
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mBe = false;

    mpSenderThread->disconnect();
    delete mpSenderThread;
    delete mpReqMutex;
    delete mpServControl;

    while (!mReqQueue.empty())
    {
        mReqQueue.pop();
    }
}

void CMediaControlServerProfile::checkPortNumber()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
#ifndef ANDROID
    std::string cmdNetstat = "netstat -at | grep ";
    std::string checkPort;
    Int32 result;
    do
    {
        checkPort = cmdNetstat + "'" + mPort + "'";
        result = system (checkPort.c_str());
        if (result == 0)
        {
            Int32 port;  
            std::istringstream ist(mPort);
            ist >> port;
            port++;
            LOG4CPLUS_INFO(msLogger, "port number "+ mPort + " is closed"); 
            if (port > mInitialPort + mPortInterval)
            {
                port = mInitialPort;
            }
            std::ostringstream ost;
            ost << port;
            mPort = ost.str();
            LOG4CPLUS_INFO(msLogger, "checking port number " + mPort);
         }   
    }    while (result == 0);
#endif// ANDROID
}

void CMediaControlServerProfile::split(const std::string& str, const std::string& delimiters , std::vector<std::string>& tokens)
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

void CMediaControlServerProfile::play(std::string const& trackName, std::string const& trackUid, std::string const& avform, bool needLock)
{

    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(msLogger,  "trackName: " + trackName + "avform: "  + avform + "trackUid: " + trackUid);
    std::string mTrackName = trackName;
    int position = mTrackName.find(" "); 
    while ( position != std::string::npos ) 
    {
      mTrackName.replace(position, 1, "+" );
      position = mTrackName.find( " ", position + 1 );
    }
    int randomNum = rand() % 1000 + 1;
    std::stringstream out;
    out << randomNum;

    std::string streamName = mStreamName + mTrackName.substr(0, mTrackName.find(".")-1) + out.str();
    LOG4CPLUS_TRACE(msLogger, "stream name: "+streamName);
    mPath = mpAppCallbacks->onMediaPathRequest(needLock);
    std::string path = mPath + trackName;

    //find the open port
    checkPortNumber();

    std::string source = mRtsp + mIp + ":" + mPort + "/" + streamName;
    if (mpServControl->sendPlay(mPort, path, streamName, avform))
    {
        prepareRequest(PLAY, true, trackName, trackUid, source, avform);
        mpAppCallbacks->onServerPlay(trackName, trackUid, source, avform, needLock);
    }
}

void CMediaControlServerProfile::changeHighlightedTrack(int newPosition) 
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    std::stringstream out;
    out << newPosition;
    std::string newPos = out.str();
    prepareRequest(CHANGETRACK, true, newPos);
}

void CMediaControlServerProfile::pause() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    prepareRequest(PAUSE, false);
    mpServControl->sendCommand(mpServControl->mPause);
}

void CMediaControlServerProfile::stopStreaming() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    prepareRequest(STOP, false);
    mpServControl->sendCommand(mpServControl->mStop);
}

void CMediaControlServerProfile::serverStop() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mpServControl->sendCommand(mpServControl->mStop);
}

void CMediaControlServerProfile::resume(bool needLock) {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__); 
    if (mpServControl->sendCommand(mpServControl->mResume))
    {
        prepareRequest(RESUME, false);
        mpAppCallbacks->onServerResume(needLock);
    }
}

void CMediaControlServerProfile::sync() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    prepareRequest(SYNC, false);
}

void CMediaControlServerProfile::unsync() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    prepareRequest(UNSYNC, false);
}

void CMediaControlServerProfile::toggle() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    prepareRequest(TOGGLE, false);
}

void CMediaControlServerProfile::setPath(std::string const& path) {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mPath = path;
}


void CMediaControlServerProfile::prepareRequest(PROCEDURES_CONTROL_IDS proc, bool has_events, std::string const& event_f, std::string const& event_s, std::string const& event_t, std::string const& event_fr)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   std::string msg;
   msg = static_cast<UInt8>(proc);
   if (has_events)
   { 
	msg += event_f + ";" + event_s + ";" + event_t + ";" + event_fr;
   }
   mpReqMutex->lock();
   mReqQueue.push(msg);
   mpReqMutex->unlock();
   mpReqSemaphore->signal();     


}

void CMediaControlServerProfile::onEnable() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    iviLink::Error err = iviLink::Channel::allocateChannelAsServer(MEDIA_CONTROL_TAG, this, mChannelID, eRealTime);
    if (err.isNoError()) 
    {
        LOG4CPLUS_INFO(msLogger, "Channel allocated, starting the communication...");
        mpSenderThread->start();
    } else {
        LOG4CPLUS_WARN(msLogger, "allocate Channel failed");
        killProcess(1);
    }
}

void CMediaControlServerProfile::onDisable() {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    iviLink::Error err = iviLink::Channel::deallocateChannel(mChannelID);
    if (!err.isNoError()) {
        LOG4CPLUS_WARN(msLogger, "Unable to deallocate channel: " + static_cast<std::string> (err));
    }
}

//from CChannelHandler

void CMediaControlServerProfile::onBufferReceived(const iviLink::Channel::tChannelId channel, iviLink::Buffer const& buffer) {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (mChannelID != channel) 
    {
        LOG4CPLUS_INFO(msLogger, "mChannelID != channel_id");
        return;
    } 
    else 
    {
        LOG4CPLUS_INFO(msLogger, "mChannelID == channel_id");
    }

   UInt8 *incomingData = buffer.getBuffer();
   int read_size = buffer.getSize();

   LOG4CPLUS_INFO(msLogger, "Procedure ID = " + convertIntegerToString(incomingData[0]));

   if(incomingData[0] == STOP)
   {
            LOG4CPLUS_INFO(msLogger, "case STOP");
            mpAppCallbacks->onServerStop(true);
            mpServControl->sendCommand(mpServControl->mStop);  

   }
   else if(incomingData[0] == SERVERSTOP)
   {
            LOG4CPLUS_INFO(msLogger, "case SERVERSTOP");
            serverStop();  
   }
   else if(incomingData[0] == PAUSE)
   {
            LOG4CPLUS_INFO(msLogger, "case PAUSE");
            mpAppCallbacks->onServerPause(true);
            mpServControl->sendCommand(mpServControl->mPause);
   }
   else if(incomingData[0] == RESUME)
   {
            LOG4CPLUS_INFO(msLogger, "case RESUME");
            resume(true);  
   }
   else if(incomingData[0] == SYNC)
   {
            LOG4CPLUS_INFO(msLogger, "case SYNC");
            mpAppCallbacks->onServerSync(true);
   }
   else if(incomingData[0] == UNSYNC)
   {
            LOG4CPLUS_INFO(msLogger, "case UNSYNC");
            mpAppCallbacks->onServerUnsync(true);
   }
   else if(incomingData[0] == TOGGLE)
   {
            LOG4CPLUS_INFO(msLogger, "case TOGGLE");
            mpAppCallbacks->onServerToggle(true);
   }
   else if(incomingData[0] == PLAY)
   {
            LOG4CPLUS_INFO(msLogger, "case PLAY");
            std::string message((char*)(incomingData + 1), read_size - 1);
            LOG4CPLUS_INFO(msLogger, "message  = " + message);
            vector<string> parse_result;
            split(message, ";", parse_result);
            if (parse_result.size() == 3)
            {
                LOG4CPLUS_INFO(msLogger, parse_result[0]);
                LOG4CPLUS_INFO(msLogger, parse_result[1]);
                LOG4CPLUS_INFO(msLogger, parse_result[2]);
                LOG4CPLUS_INFO(msLogger, "get message:" + parse_result[0] + " ; " + parse_result[1] + " ; " + parse_result[2]);
            	play(parse_result[0], parse_result[1], parse_result[2], true);
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

void CMediaControlServerProfile::onChannelDeleted(const UInt32 channel_id) {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

    if (mChannelID == channel_id)
        mChannelID = 0;

    iviLink::Error err = iviLink::Channel::deallocateChannel(channel_id);
    if (!err.isNoError()) {
        LOG4CPLUS_WARN(msLogger, "Unable to deallocate channel: " + static_cast<std::string> (err));
    }
}

void CMediaControlServerProfile::onConnectionLost() {
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

bool CMediaControlServerProfile::hasRequests() 
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mpReqMutex->lock();
    bool res = !mReqQueue.empty();
    mpReqMutex->unlock();
    return res;
}

void CMediaControlServerProfile::handleRequest()
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
      LOG4CPLUS_INFO(msLogger, "CMediaSourceServerProfile::handleRequest() :: Send error"
         + static_cast<std::string>(err));
   }
}


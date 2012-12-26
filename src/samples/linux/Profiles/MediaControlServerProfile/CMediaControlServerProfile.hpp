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
 

#ifndef CMEDIACONTROLSERVERPROFILE_HPP_
#define CMEDIACONTROLSERVERPROFILE_HPP_

#include <iostream>
#include <queue>
#include <string>
#include <sstream>

#include "Logger.hpp"
#include "profileLib.hpp"
#include "IMediaControlServerProfileAPI.hpp"
#include "ISender.hpp"
#include "common.h"
#include "CServerControl.hpp"

using iviLink::CBuffer;
using iviLink::Channel::tChannelId;

class CSenderThread;
class CMutex;
class CSignalSemaphore;
IVILINK_PROFILE_BEGIN

class CMediaControlServerProfile : public iviLink::Channel::CChannelHandler
                                  , public IMediaControlServerProfile_API
                                  , public ISender 
{

    IVILINK_PROFILE_DECLARE
    IVILINK_PROFILE_NAME(MediaControlServerProfile)
    IVILINK_PROFILE_VERSION(1)
    IVILINK_PROFILE_UID(MediaControlServerProfile_UID)
    IVILINK_PROFILE_API_UID(MediaControlServerProfile_API_UID)

   // from IMediaControlServerProfile_API
   virtual void prepareRequest(PROCEDURES_CONTROL_IDS proc, bool has_events, std::string const& event_f = "", std::string const& event_s = "", std::string const& event_t = "",std::string const& event_fr = "");

   virtual void stopStreaming();
   virtual void serverStop();
   virtual void play(std::string const&  trackName, std::string const&  trackUid, std::string const& avform); //send the msg to another side
   virtual void resume();
   virtual void pause(); 
   virtual void toggle(); 
   virtual void sync(); 
   virtual void unsync(); 
   virtual void setPath(std::string const& str);
   virtual void changeHighlightedTrack(int newPosition);

   virtual void onEnable();
   virtual void onDisable();

   //from CChannelHandler
   virtual void onBufferReceived(const iviLink::Channel::tChannelId channel, CBuffer const& buffer);
   virtual void onChannelDeleted(const UInt32 channel_id);
   virtual void onConnectionLost();

   //from ISender
   virtual void senderLoop();

   bool hasRequests();
   void handleRequest();
   void split(const std::string& str, const std::string& delimiters , std::vector<std::string>& tokens);

   CMediaControlServerProfile(iviLink::Profile::IProfileCallbackProxy* pCbProxy);
   virtual ~CMediaControlServerProfile();

   typedef std::queue<std::string>                  tReqQueue;

   iviLink::Channel::tChannelId                 mChannelID;

   IMediaControlServerProfile_API::Callbacks *  mpAppCallbacks;
   CSenderThread *                              mpSenderThread;
   CMutex *                                     mpReqMutex;
   CSignalSemaphore *                           mpReqSemaphore;
   bool                                         mBe;
   tReqQueue                                    mReqQueue;
   static Logger                                msLogger;
   std::string                                  mPath;
   std::string                                  mIp;
   std::string                                  mPort;
   std::string                                  mRtsp;
   std::string                                  mStreamName;
   CServerControl *                             mpServControl;
};

IVILINK_PROFILE_END(CMediaControlServerProfile)

#endif /* CMEDIACONTROLSERVERPROFILE_HPP_ */

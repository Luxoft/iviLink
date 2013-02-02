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


#ifndef CMEDIACONTROLCLIENTPROFILE_HPP_
#define CMEDIACONTROLCLIENTPROFILE_HPP_

#include <iostream>
#include <queue>
#include <vector>
#include <sstream>

#include "Logger.hpp"
#include "ProfileLibCommon.hpp"
#include "IMediaControlClientProfileAPI.hpp"
#include "ISender.hpp"
#include "MediaProfilesCommon.hpp"

using iviLink::Buffer;
using iviLink::Channel::tChannelId;

class CSenderThread;
class CMutex;
class CSignalSemaphore;

IVILINK_PROFILE_BEGIN

class CMediaControlClientProfile   : public iviLink::Channel::ChannelHandler
                                  , public IMediaControlClientProfile_API
                                  , public ISender
{
   IVILINK_PROFILE_DECLARE
   IVILINK_PROFILE_NAME(MediaControlClientProfile)
   IVILINK_PROFILE_VERSION(1)
   IVILINK_PROFILE_UID(MediaControlClientProfile_UID)
   IVILINK_PROFILE_API_UID(MediaControlClientProfile_API_UID)
  
   // from IMediaControlClientProfile_API
   virtual void prepareRequest(PROCEDURES_CONTROL_IDS proc, bool has_events, std::string const& event_f = "", std::string const& event_s = "",std::string const& event_t = "");

   virtual void stop();
   virtual void serverStop();
   virtual void play(std::string const&  trackName, std::string const&  trackUid, std::string const& avform); //send the msg to another side
   virtual void resume();
   virtual void pause(); 
   virtual void toggle(); 
   virtual void sync(); 
   virtual void unsync(); 

   virtual void onEnable();
   virtual void onDisable();

   //from CChannelHandler
   virtual void onBufferReceived(const iviLink::Channel::tChannelId channel, Buffer const& buffer);
   virtual void onChannelDeleted(const UInt32 channel_id);
   virtual void onConnectionLost();

   //from ISender
   virtual void senderLoop();

   void split(const std::string& str, const std::string& delimiters , std::vector<std::string>& tokens);

   bool hasRequests();
   void handleRequest();

   CMediaControlClientProfile(iviLink::Profile::IProfileCallbackProxy* pCbProxy);
   virtual ~CMediaControlClientProfile();

   typedef std::queue<std::string>                  tReqQueue;

   iviLink::Channel::tChannelId                 mChannelID;

   IMediaControlClientProfile_API::Callbacks *  mpAppCallbacks;
   CSenderThread *                              mpSenderThread;
   CMutex *                                     mpReqMutex;
   CSignalSemaphore *                           mpReqSemaphore;
   bool                                         mBe;
   tReqQueue                                    mReqQueue;
   static Logger                                msLogger;
};

IVILINK_PROFILE_END(CMediaControlClientProfile)

#endif /* CMEDIACONTROLCLIENTPROFILE_HPP_ */

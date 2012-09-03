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





#ifndef CMEDIACONTROLCLIENTPROFILE_HPP_
#define CMEDIACONTROLCLIENTPROFILE_HPP_


#include "utils/misc/Logger.hpp"
#include "framework/public/profileLib/profileLib.hpp"
#include "samples/linux/Profiles/ProfileAPI/IMediaControlClientProfileAPI.hpp"
#include "samples/linux/Profiles/mediaCommon/ISender.hpp"
#include "samples/linux/Profiles/mediaCommon/common.h"

#include <iostream>
#include <queue>

using iviLink::CBuffer;
using iviLink::Channel::tChannelId;

class CSenderThread;
class CMutex;
class CSignalSemaphore;

IVILINK_PROFILE_BEGIN

class CMediaControlClientProfile   : public iviLink::Channel::CChannelHandler
                                  , public IMediaControlClientProfile_API
                                  , public ISender
{
   IVILINK_PROFILE_DECLARE
   IVILINK_PROFILE_NAME(MediaControlClientProfile)
   IVILINK_PROFILE_VERSION(1)
   IVILINK_PROFILE_UID(MediaControlClientProfile_UID)
   IVILINK_PROFILE_API_UID(MediaControlClientProfile_API_UID)
  
   // from IMediaControlClientProfile_API
   virtual void prepareRequest(PROCEDURES_CONTROL_IDS proc, bool has_events, std::string const& event_f, std::string const& event_s);

   virtual void stop();
   virtual void play(std::string const&  trackName, std::string const& avform); //send the msg to another side
   virtual void resume();
   virtual void pause(); 
   virtual void toggle(); 
   virtual void sync(); 
   virtual void unsync(); 

   virtual void onEnable();
   virtual void onDisable();

   //from CChannelHandler
   virtual void bufferReceived(const iviLink::Channel::tChannelId channel, CBuffer const& buffer);
   virtual void channelDeletedCallback(const UInt32 channel_id);
   virtual void connectionLostCallback();

   //from ISender
   virtual void senderLoop();

   bool hasRequests();
   void handleRequest();

   CMediaControlClientProfile(iviLink::Profile::IProfileCallbackProxy* pCbProxy);
   virtual ~CMediaControlClientProfile();

   typedef std::queue<CBuffer>                  tReqQueue;

   iviLink::Channel::tChannelId                 mChannelID;

   IMediaControlClientProfile_API::Callbacks *  mpAppCallbacks;
   CSenderThread *                              mpSenderThread;
   CMutex *                                     mpReqMutex;
   CSignalSemaphore *                           mpReqSemaphore;
   bool                                         mBe;
   tReqQueue                                    mReqQueue;
   std::string                                  mTag;
   static Logger                                msLogger;
};

IVILINK_PROFILE_END(CMediaControlClientProfile)

#endif /* CMEDIACONTROLCLIENTPROFILE_HPP_ */

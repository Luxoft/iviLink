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


#ifndef CMEDIASOURCESERVERPROFILE_HPP_
#define CMEDIASOURCESERVERPROFILE_HPP_


#include "Logger.hpp"
#include "ProfileLibCommon.hpp"
#include "IMediaSourceServerProfileAPI.hpp"
#include "ISender.hpp"
#include "MediaProfilesCommon.hpp"


#include <iostream>
#include <queue>

using iviLink::Buffer;
using iviLink::Channel::tChannelId;

class CSenderThread;
class CMutex;
class CSignalSemaphore;

IVILINK_PROFILE_BEGIN

class CMediaSourceServerProfile   : public iviLink::Channel::ChannelHandler
                                  , public IMediaSourceServerProfile_API
                                  , public ISender
{
   IVILINK_PROFILE_DECLARE
   IVILINK_PROFILE_NAME(MediaSourceServerProfile)
   IVILINK_PROFILE_VERSION(1)
   IVILINK_PROFILE_UID(MediaSourceServerProfile_UID)
   IVILINK_PROFILE_API_UID(MediaSourceServerProfile_API_UID)
  

   // from IMediaSourceServerProfile_API
   virtual void prepareRequest(PROCEDURES_SOURCE_IDS proc, bool has_event, std::string const& event = "");

   virtual void recallTrackList();
   virtual void sendTrackList(std::string const& trackList);
   virtual void trackListChanged();


   virtual void onEnable();
   virtual void onDisable();

   //from CChannelHandler
   virtual void onBufferReceived(const iviLink::Channel::tChannelId channel, Buffer const& buffer);
   virtual void onChannelDeleted(const UInt32 channel_id);
   virtual void onConnectionLost();

   //from ISender
   virtual void senderLoop();

   bool hasRequests();
   void handleRequest();

   CMediaSourceServerProfile(iviLink::Profile::IProfileCallbackProxy* pCbProxy);
   virtual ~CMediaSourceServerProfile();

   typedef std::queue<std::string>                  tReqQueue;

   iviLink::Channel::tChannelId                 mChannelID;

   IMediaSourceServerProfile_API::Callbacks *   mpAppCallbacks;
   CSenderThread *                              mpSenderThread;
   CMutex *                                     mpReqMutex;
   CSignalSemaphore *                           mpReqSemaphore;
   bool                                         mBe;
   tReqQueue                                    mReqQueue;
   static Logger                                msLogger;
};

IVILINK_PROFILE_END(CMediaSourceServerProfile)

#endif /* CMEDIASOURCESERVERPROFILE_HPP_ */

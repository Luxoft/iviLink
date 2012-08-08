/* 
 * 
 * iviLINK SDK, version 1.0.1
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





#ifndef CSEATHUPROFILE_HPP_
#define CSEATHUPROFILE_HPP_

#include "utils/misc/Logger.hpp"
#include "framework/public/profileLib/profileLib.hpp"
#include "samples/linux/Profiles/ProfileAPI/ISeatHUProfile.hpp"
#include "samples/linux/Profiles/SeatClientProfile/ISender.hpp"
#include "samples/linux/Profiles/SeatHUProfile/EReqType.hpp"

using iviLink::CBuffer;
using iviLink::Channel::tChannelId;

class CSenderThread;
class CMutex;
class CSignalSemaphore;

IVILINK_PROFILE_BEGIN

class CSeatHUProfile : public iviLink::Channel::CChannelHandler
                     , public ISeatHUProfile
                     , public ISender
{
   IVILINK_PROFILE_DECLARE
   IVILINK_PROFILE_NAME(SeatHUProfile)
   IVILINK_PROFILE_VERSION(1)
   IVILINK_PROFILE_UID(SeatHUProfile_UID)

   //from ISeatHUProfile
   virtual void sendNotification(Json::Value state);

   virtual void onEnable();
   virtual void onDisable();

   //from CChannelHandler
   virtual void bufferReceived(const iviLink::Channel::tChannelId channel, iviLink::CBuffer const& buffer);
   virtual void channelDeletedCallback(const UInt32 channel_id);
   virtual void connectionLostCallback();

   //from ISender
   virtual void senderLoop();

   bool hasRequests();
   void handleRequest();

   CSeatHUProfile(iviLink::Profile::IProfileCallbackProxy* pCbProxy);
   virtual ~CSeatHUProfile();

   iviLink::Channel::tChannelId                 mChannelID;

   ISeatHUProfile::Callbacks *                  mpAppCallbacks;
   CSenderThread *                              mpSenderThread;
   CMutex *                                     mpReqMutex;
   CSignalSemaphore *                           mpReqSemaphore;
   bool                                         mBe;
   std::string                                  mTag;
   Json::Value                                  mSendState;
   bool                                         mHasRequest;
   static Logger                                msLogger;
};

IVILINK_PROFILE_END(CSeatHUProfile)

#endif /* CSEATHUPROFILE_HPP_ */

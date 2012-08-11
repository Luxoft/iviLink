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





#ifndef CCLIMATECLIENTPROFILE_HPP_
#define CCLIMATECLIENTPROFILE_HPP_

#include <queue>

#include "utils/misc/Logger.hpp"
#include "framework/public/profileLib/profileLib.hpp"
#include "samples/linux/Profiles/ProfileAPI/IClimateClientProfile.hpp"
#include "samples/linux/Profiles/ClimateClientProfile/ISender.hpp"
#include "samples/linux/Profiles/ClimateHUProfile/EReqType.hpp"

using iviLink::CBuffer;
using iviLink::Channel::tChannelId;

class CSenderThread;
class CMutex;
class CSignalSemaphore;

IVILINK_PROFILE_BEGIN

class CClimateClientProfile   : public iviLink::Channel::CChannelHandler
                              , public IClimateClientProfile
                              , public ISender
{
   IVILINK_PROFILE_DECLARE
   IVILINK_PROFILE_NAME(ClimateClientProfile)
   IVILINK_PROFILE_VERSION(1)
   IVILINK_PROFILE_UID(ClimateClientProfile_UID)

   // from IClimateClientProfile
   virtual void tempRequest(ETempEvent event);
   virtual void fanRequest(EFanEvent event);
   virtual void heaterRequest(EHeaterEvent event);
   virtual void generalRequest(EGeneralRequest event);
   virtual void initRequest();

   virtual void onEnable();
   virtual void onDisable();

   //from CChannelHandler
   virtual void bufferReceived(const iviLink::Channel::tChannelId channel, CBuffer const& buffer);
   virtual void channelDeletedCallback(const UInt32 channel_id);
   virtual void connectionLostCallback();

   //from ISender
   virtual void senderLoop();

   void prepareRequest(EReqType regType, bool has_event, UInt16 event);

   bool hasRequests();
   void handleRequest();

   CClimateClientProfile(iviLink::Profile::IProfileCallbackProxy* pCbProxy);
   virtual ~CClimateClientProfile();

   typedef std::queue<CBuffer>                  tReqQueue;

   iviLink::Channel::tChannelId                 mChannelID;

   IClimateClientProfile::Callbacks *           mpAppCallbacks;
   CSenderThread *                              mpSenderThread;
   tReqQueue                                    mReqQueue;
   CMutex *                                     mpReqMutex;
   CSignalSemaphore *                           mpReqSemaphore;
   bool                                         mBe;
   std::string                                  mTag;
   static Logger                                msLogger;
};

IVILINK_PROFILE_END(CClimateClientProfile)

#endif /* CCLIMATECLIENTPROFILE_HPP_ */

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


#ifndef CSCREEN_SHARING_PROFILE_IMPL_HPP
#define CSCREEN_SHARING_PROFILE_IMPL_HPP

#include "Logger.hpp"
#include "ProfileLibCommon.hpp"
#include "IScreenSharingProfileAPI.hpp"

using iviLink::Buffer;
using iviLink::Channel::tChannelId;

IVILINK_PROFILE_BEGIN

class CScreenSharingProfileImpl: public IScreenSharingProfile_API                              
                             , public iviLink::Channel::ChannelHandler
{
   IVILINK_PROFILE_DECLARE
   IVILINK_PROFILE_NAME(ScreenSharingProfileImpl)
   IVILINK_PROFILE_VERSION(1)
   IVILINK_PROFILE_UID(ScreenSharingProfileImpl_UID)
   IVILINK_PROFILE_API_UID(ScreenSharingProfile_PAPI_UID)

   CScreenSharingProfileImpl(iviLink::Profile::IProfileCallbackProxy* pCbProxy);
   virtual ~CScreenSharingProfileImpl();

   virtual void sendData(const std::string &data);

   //from CChannelHandler
   virtual void onBufferReceived(const tChannelId channel, Buffer const& buffer);
   virtual void onChannelDeleted(const UInt32 channel_id);
   virtual void onConnectionLost();
   virtual void onEnable();
   virtual void onDisable();

   //Channel ID
   tChannelId mChannelID;
   std::string mName;
   
   //Pointer to instance of class describing callbacks
   IScreenSharingProfile_API::Callbacks* mpAppCallbacks;

   static Logger msLogger;
};

IVILINK_PROFILE_END(CScreenSharingProfileImpl)

#endif //CSCREEN_SHARING_PROFILE_IMPL_HPP

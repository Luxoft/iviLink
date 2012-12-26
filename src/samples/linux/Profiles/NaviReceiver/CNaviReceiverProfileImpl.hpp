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
 

#ifndef CNAVI_RECEIVER_PROFILE_IMPL_HPP
#define CNAVI_RECEIVER_PROFILE_IMPL_HPP

#include "Logger.hpp"
//profile library header, must be included
#include "profileLib.hpp"
//abstract profile API class header, must be included
#include "INaviReceiverProfile.hpp"

//namespace required for reading data was sending by other side
using iviLink::CBuffer;
//namespace required for working with channels
using iviLink::Channel::tChannelId;

IVILINK_PROFILE_BEGIN

class CNaviReceiverProfileImpl: public INaviReceiverProfile_API
                               //the following class provides access to allocating and deallocatiing channels
                             , public iviLink::Channel::CChannelHandler
{

   //Profile declaration: name, version, UID, API UID
   IVILINK_PROFILE_DECLARE
   IVILINK_PROFILE_NAME(NaviReceiverProfileImpl)
   IVILINK_PROFILE_VERSION(1)
   IVILINK_PROFILE_UID(NaviReceiverProfileImpl_UID)
   IVILINK_PROFILE_API_UID(NaviReceiverProfile_PAPI_UID)

   CNaviReceiverProfileImpl(iviLink::Profile::IProfileCallbackProxy* pCbProxy);
   virtual ~CNaviReceiverProfileImpl();

   //from CChannelHandler

   /**
    * Callback that should be invoked when the data is received
    * @param channel - channel ID
    * @param buffer - buffer with data
    * @return none
    */
   virtual void onBufferReceived(const tChannelId channel, CBuffer const& buffer);

   /**
    * Callback that should be invoked when the channel is deleted
    * @param channel_id - channel ID
    * @return none
    */
   virtual void onChannelDeleted(const UInt32 channel_id);

   /**
    * Callback that should be invoked when the connection is lost
    * @return none
    */
   virtual void onConnectionLost();

   /**
    * Allocating channel
    * @return none
    */
   virtual void onEnable();

   /**
    * Deallocating channel
    * @return none
    */
   virtual void onDisable();

   //Channel ID
   tChannelId mChannelID;
   std::string mName;
   
   //Pointer to instance of class describing callbacks
   INaviReceiverProfile_API::Callbacks* mpAppCallbacks;

   static Logger msLogger;
};

IVILINK_PROFILE_END(CNaviReceiverProfileImpl)

#endif 

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


#ifndef CBASIC_SAMPLE_PROFILE_IMPL_HPP
#define CBASIC_SAMPLE_PROFILE_IMPL_HPP

#include "Logger.hpp"
//profile library header, must be included
#include "ProfileLibCommon.hpp"
//abstract profile API class header, must be included
#include "IBasicSampleProfileAPI.hpp"

//namespace required for reading data was sending by other side
using iviLink::Buffer;
//namespace required for working with channels
using iviLink::Channel::tChannelId;

IVILINK_PROFILE_BEGIN

// CBasicSampleProfileImpl class inherits IBasicSampleProfile_API and CChannelHandler classes
class CBasicSampleProfileImpl: public IBasicSampleProfile_API
                               //the following class provides access to allocating and deallocatiing channels
                             , public iviLink::Channel::ChannelHandler
{

   //Profile declaration: name, version, UID, API UID
   IVILINK_PROFILE_DECLARE
   IVILINK_PROFILE_NAME(BasicSampleProfileImpl)
   IVILINK_PROFILE_VERSION(3)
   IVILINK_PROFILE_UID(BasicSampleProfileImpl_UID)
   IVILINK_PROFILE_API_UID(BasicSampleProfile_PAPI_UID)

   CBasicSampleProfileImpl(iviLink::Profile::IProfileCallbackProxy* pCbProxy);
   virtual ~CBasicSampleProfileImpl();

   enum PROCEDURES_IDS
   {
      SEND_OPERANDS_PROCEDURE = 1,
      SEND_RESULT_PROCEDURE
   };

   // Profile domain section

   /**
    * Sending the summands
    * @param a, b - operands
    * @return none
    */
   virtual void sendOperands(int a, int b);

   /**
    * Sending the obtained result
    * @param a - result of operation
    * @return none
    */
   virtual void sendResult(int a);

   //from CChannelHandler

   /**
    * Callback that should be invoked when the data is received
    * @param channel - channel ID
    * @param buffer - buffer with data
    * @return none
    */
   virtual void onBufferReceived(const tChannelId channel, Buffer const& buffer);

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
   IBasicSampleProfile_API::Callbacks* mpAppCallbacks;

   static Logger msLogger;
};

IVILINK_PROFILE_END(CBasicSampleProfileImpl)

#endif //CBASIC_SAMPLE_PROFILE_IMPL_HPP

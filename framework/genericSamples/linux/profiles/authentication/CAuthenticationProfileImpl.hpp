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


#ifndef CAUTHENTICATION_PROFILE_IMPL_HPP
#define CAUTHENTICATION_PROFILE_IMPL_HPP

#include "Logger.hpp"
#include "ProfileLibCommon.hpp"
#include "IAuthenticationProfileAPI.hpp"
#include "CTrustList.hpp"
#include "CRSAEncryptDecrypt.hpp"

using iviLink::Buffer;
using iviLink::Channel::tChannelId;
using iviLink::CTrustList;

IVILINK_PROFILE_BEGIN

class CAuthenticationProfileImpl : public IAuthenticationProfile_API
                                 , public iviLink::Channel::ChannelHandler
{
   IVILINK_PROFILE_DECLARE
   IVILINK_PROFILE_NAME(AuthenticationProfileImpl)
   IVILINK_PROFILE_VERSION(1)
   IVILINK_PROFILE_UID(AuthenticationProfileImpl_UID)

   enum PROCEDURES_IDS
   {
      SEND_ENCRYPTED_PIN = 1,
      SEND_PUBLIC_KEY,
      SEND_UID,
      YOUR_UID_IS_OK,
      YOUR_UID_IS_NOK,
      READY_TO_EXIT
   };

// Profile domain section
   virtual void sendPIN(const int first_digit, const int second_digit, const int third_digit, const int fourth_digit);
   virtual void sendPublicKey();
   virtual void writeRemoteUIDToTrustList();
   virtual void readyToExit();

   virtual void onEnable();
   virtual void onDisable();

   //from CChannelHandler
   virtual void onBufferReceived(const iviLink::Channel::tChannelId channel, Buffer const& buffer);
   virtual void onChannelDeleted(const UInt32 channel_id);
   virtual void onConnectionLost();


   CAuthenticationProfileImpl(iviLink::Profile::IProfileCallbackProxy* pCbProxy);
   virtual ~CAuthenticationProfileImpl();

   void sendEncryptedPIN(const std::string& PIN);
   void sendEncryptedState(const std::string& state);
   void sendEncryptedString(const std::string& , const PROCEDURES_IDS);
   void sendProcedureId(const PROCEDURES_IDS);
   void sendPublicKey(const RSA::PublicKey publicKey);
   void sendTrustListUID(const iviLink::BaseUid uid);
   void sendYourUIDIsKnow();
   void sendYourUIDIsUnknown();
   void validateUIDs();

   static Logger msLogger;

   UInt32 mChannelID;
   IAuthenticationProfile_API::Callbacks* mpAppCallbacks;
   RSA::PublicKey remoteHostPublicKey;

   CTrustList * mpTrustList;

   iviLink::BaseUid mRemoteUID;
   bool remoteUIDIsChecked;
   bool remoteUIDIsOK;
   bool localUIDIsChecked;
   bool localUIDIsOK;
};

IVILINK_PROFILE_END(CAuthenticationProfileImpl)

#endif //CAUTHENTICATION_PROFILE_IMPL_HPP

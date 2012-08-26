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





#ifndef CAUTHENTICATION_PROFILE_IMPL_HPP
#define CAUTHENTICATION_PROFILE_IMPL_HPP

#include "utils/misc/Logger.hpp"
#include "framework/public/profileLib/profileLib.hpp"
#include "samples/linux/Profiles/ProfileAPI/IAuthenticationProfileAPI.hpp"
#include "framework/libraries/trust_list/CTrustList.hpp"
#include "CRSAEncryptDecrypt.hpp"

using iviLink::CBuffer;
using iviLink::Channel::tChannelId;
using iviLink::CTrustList;

IVILINK_PROFILE_BEGIN

class CAuthenticationProfileImpl : public IAuthenticationProfile_API
                                 , public iviLink::Channel::CChannelHandler
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
      YOUR_UID_IS_NOK
   };

// Profile domain section
   virtual void sendPIN(int first_digit, int second_digit, int third_digit, int fourth_digit);
   virtual void sendPublicKey();
   virtual void writeRemoteUIDToTrustList();

   virtual void onEnable();
   virtual void onDisable();

   //from CChannelHandler
   virtual void bufferReceived(const iviLink::Channel::tChannelId channel, CBuffer const& buffer);
   virtual void channelDeletedCallback(const UInt32 channel_id);
   virtual void connectionLostCallback();


   CAuthenticationProfileImpl(iviLink::Profile::IProfileCallbackProxy* pCbProxy);
   virtual ~CAuthenticationProfileImpl();

   void sendEncryptedPIN(string PIN);
   void sendPublicKey(RSA::PublicKey publicKey);
   void sendTrustListUID(iviLink::CUid uid);
   void sendYourUIDIsKnow();
   void sendYourUIDIsUnknown();
   void validateUIDs();

   static Logger msLogger;

   UInt32 mChannelID;
   IAuthenticationProfile_API::Callbacks* mpAppCallbacks;
   RSA::PublicKey remoteHostPublicKey;
   CTrustList mTrustList;
   iviLink::CUid mRemoteUID;
   bool remoteUIDIsChecked;
   bool remoteUIDIsOK;
   bool localUIDIsChecked;
   bool localUIDIsOK;
};

IVILINK_PROFILE_END(CAuthenticationProfileImpl)

#endif //CAUTHENTICATION_PROFILE_IMPL_HPP

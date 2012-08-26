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








#include <stdlib.h>
#include <cstring>
#include "3rd_party/cryptopp/cryptopp/queue.h"
#include "CAuthenticationProfileImpl.hpp"
#include "CRSAEncryptDecrypt.hpp"

using CryptoPP::ByteQueue;
using namespace std;

Logger CAuthenticationProfileImpl::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profiles.CAuthenticationProfileImpl"));

void CAuthenticationProfileImpl::sendPublicKey()
{
   RSA::PublicKey publicKey = CRSAEncryptDecrypt::getPublicKey();

   sendPublicKey(publicKey);
   sendTrustListUID(mTrustList.getOurUid());
}

void CAuthenticationProfileImpl::writeRemoteUIDToTrustList()
{
   LOG4CPLUS_INFO(msLogger, "CAuthenticationProfileImpl::writeRemoteUIDToTrustList()");

   mTrustList.addUid(mRemoteUID);
}

void CAuthenticationProfileImpl::sendPIN(int first_digit, int second_digit, int third_digit, int fourth_digit)
{
   LOG4CPLUS_INFO(msLogger, "CAuthenticationProfileImpl::sendPIN(" + convertIntegerToString(first_digit)
                                                                   + convertIntegerToString(second_digit)
                                                                   + convertIntegerToString(third_digit)
                                                                   + convertIntegerToString(fourth_digit) + ")");

   string PIN = string(convertIntegerToString(first_digit)
                     + convertIntegerToString(second_digit)
                     + convertIntegerToString(third_digit)
                     + convertIntegerToString(fourth_digit));

   RSA::PublicKey publicKey = CRSAEncryptDecrypt::getPublicKey();
   string encryptedPIN = CRSAEncryptDecrypt::encrypt(PIN, remoteHostPublicKey);

   sendEncryptedPIN(encryptedPIN);
}

void CAuthenticationProfileImpl::sendEncryptedPIN(string PIN)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

   UInt8* outgoingData = new UInt8[PIN.length() + 1];

   outgoingData[0] = static_cast<UInt8>(SEND_ENCRYPTED_PIN);

   LOG4CPLUS_INFO(msLogger, "Engrypted PIN length = " + convertIntegerToString(PIN.length()));
   LOG4CPLUS_INFO(msLogger, "Encrypted PIN  = " + PIN);

   memcpy(outgoingData + 1, (void*)const_cast<char*>(PIN.c_str()), PIN.length() + 1);

   iviLink::Channel::sendBuffer(mChannelID, outgoingData, PIN.length() + 1);

   delete[] outgoingData;
}

void CAuthenticationProfileImpl::sendPublicKey(RSA::PublicKey publicKey)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

   CRSAEncryptDecrypt::printPublicKey(publicKey);

   ByteQueue queue;
   publicKey.Save(queue);
   UInt32 queueSize = queue.CurrentSize();

   UInt8* outgoingData = new UInt8[queueSize + 1];

   outgoingData[0] = static_cast<UInt8>(SEND_PUBLIC_KEY);

   queue.Get(outgoingData + 1, queueSize);
   LOG4CPLUS_INFO(msLogger, "***** " + convertIntegerToString(queueSize + 1) + " bytes sent");

   iviLink::Channel::sendBuffer(mChannelID, outgoingData, queueSize + 1);

   delete[] outgoingData;
}

void CAuthenticationProfileImpl::sendTrustListUID(iviLink::CUid uid)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   LOG4CPLUS_INFO(msLogger, "uid = " + uid.value());

   UInt8* outgoingData = new UInt8[uid.value().length() + 1];

   outgoingData[0] = static_cast<UInt8>(SEND_UID);

   memcpy(outgoingData + 1, (void*)const_cast<char*>(uid.value().c_str()), uid.value().length() + 1);

   iviLink::Channel::sendBuffer(mChannelID, outgoingData, uid.value().length() + 1);

   delete[] outgoingData;
}

void CAuthenticationProfileImpl::sendYourUIDIsUnknown()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

   UInt8* outgoingData = new UInt8[1];

   outgoingData[0] = static_cast<UInt8>(YOUR_UID_IS_NOK);

   iviLink::Channel::sendBuffer(mChannelID, outgoingData, 1);

   delete[] outgoingData;
   }

void CAuthenticationProfileImpl::sendYourUIDIsKnow()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

   UInt8* outgoingData = new UInt8[1];

   outgoingData[0] = static_cast<UInt8>(YOUR_UID_IS_OK);

   iviLink::Channel::sendBuffer(mChannelID, outgoingData, 1);

   delete[] outgoingData;}

void CAuthenticationProfileImpl::bufferReceived(const iviLink::Channel::tChannelId channel, CBuffer const& buffer)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_TRACE(msLogger, "CBuffer write position = "+convertIntegerToString(buffer.getWritePosition())
         + "; CBuffer size = " + convertIntegerToString(buffer.getSize()) +
         "; Channel = "+ convertIntegerToString(channel));

   assert(this);

   UInt8 *incomingData = buffer.getBuffer();
   int read_size = buffer.getSize();

   LOG4CPLUS_INFO(msLogger, "Procedure ID = " + convertIntegerToString(incomingData[0]));

   if(incomingData[0] == SEND_ENCRYPTED_PIN)
   {
      LOG4CPLUS_INFO(msLogger, "Encrypted PIN received");

      string encryptedPIN((char*)(incomingData + 1), read_size - 1);

      LOG4CPLUS_INFO(msLogger, "Engrypted PIN length = " + convertIntegerToString(encryptedPIN.length()));
      LOG4CPLUS_INFO(msLogger, "Encrypted PIN  = " + encryptedPIN);


      string decryptedRemotePIN = CRSAEncryptDecrypt::decrypt(encryptedPIN, CRSAEncryptDecrypt::getPrivateKey());

      LOG4CPLUS_INFO(msLogger, "Decrypted remote PIN = " + decryptedRemotePIN);

      mpAppCallbacks->gotPIN(decryptedRemotePIN[0] - '0',
                             decryptedRemotePIN[1] - '0',
                             decryptedRemotePIN[2] - '0',
                             decryptedRemotePIN[3] - '0');

   }
   else if(incomingData[0] == SEND_PUBLIC_KEY)
   {
      LOG4CPLUS_INFO(msLogger, "PublicKey received");

      UInt8* keyBytes = new UInt8[read_size - 1];

      memcpy(keyBytes, incomingData + 1, read_size - 1);

      ByteQueue queue;
      queue.Put(keyBytes, read_size - 1);

      remoteHostPublicKey.Load(queue);

      CRSAEncryptDecrypt::printPublicKey(remoteHostPublicKey);
   }
   else if(incomingData[0] == SEND_UID)
   {
      LOG4CPLUS_INFO(msLogger, "Remote UID received");

      string uid((char*)(incomingData + 1), read_size - 1);

      LOG4CPLUS_INFO(msLogger, "UID = " + uid);

      mRemoteUID = iviLink::CUid(uid);

      if(mTrustList.isKnownUid(mRemoteUID))
      {
          remoteUIDIsChecked = true;
          remoteUIDIsOK = true;

          sendYourUIDIsKnow();
          validateUIDs();
      }
      else
      {
         remoteUIDIsChecked = true;
         remoteUIDIsOK = false;

         sendYourUIDIsUnknown();
         mpAppCallbacks->onAuthenticationIsRequired();
      }
   }
   else if(incomingData[0] == YOUR_UID_IS_OK)
   {
      localUIDIsChecked = true;
      localUIDIsOK = true;

      validateUIDs();
   }
   else if(incomingData[0] == YOUR_UID_IS_NOK)
   {
      localUIDIsChecked = true;
      localUIDIsOK = false;

      mpAppCallbacks->onAuthenticationIsRequired();
   }
}

void CAuthenticationProfileImpl::channelDeletedCallback(const UInt32 channel_id)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

   if (mChannelID == channel_id)
      mChannelID = 0;

   CError err = iviLink::Channel::deallocateChannel(channel_id);
   if (!err.isNoError())
   {
      LOG4CPLUS_WARN(msLogger, "Unable to deallocate channel: " + static_cast<std::string>(err));
   }
}

void CAuthenticationProfileImpl::connectionLostCallback()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

void CAuthenticationProfileImpl::validateUIDs()
{
   LOG4CPLUS_INFO(msLogger, "validateUIDs()");

   if(localUIDIsChecked && remoteUIDIsChecked)
   {
      if(localUIDIsOK && remoteUIDIsOK)
      {
         LOG4CPLUS_INFO(msLogger, "Both UIDs were checked and are OK");
         mpAppCallbacks->onAuthenticationIsNotRequired();
      }
      else
      {
         mpAppCallbacks->onAuthenticationIsRequired();
      }
   }
}


CAuthenticationProfileImpl::CAuthenticationProfileImpl(iviLink::Profile::IProfileCallbackProxy* pCbProxy)
   : mChannelID(0)
   , mpAppCallbacks(static_cast<IAuthenticationProfile_API::Callbacks*>(pCbProxy))
   , mRemoteUID()
   , remoteUIDIsChecked(false)
   , remoteUIDIsOK(false)
   , localUIDIsChecked(false)
   , localUIDIsOK(false)
{
   Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("applications.Authentication"));

   PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));

   LOG4CPLUS_INFO(msLogger, "CAuthenticationProfileImpl::CAuthenticationProfileImpl");
}

CAuthenticationProfileImpl::~CAuthenticationProfileImpl()
{
}

void CAuthenticationProfileImpl::onEnable()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   mChannelID = iviLink::Channel::allocateChannel("CAuthenticationProfileImpl", this);
   if (mChannelID)
   {
      LOG4CPLUS_INFO(msLogger, "Channel allocated, starting the communication...");
   }
   else
   {
      LOG4CPLUS_WARN(msLogger, "allocate Channel failed");
   }
}

void CAuthenticationProfileImpl::onDisable()
{
   CError err = iviLink::Channel::deallocateChannel(mChannelID);
   if (!err.isNoError())
   {
      LOG4CPLUS_WARN(msLogger, "Unable to deallocate channel: " + static_cast<std::string>(err));
   }
}

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


#include <stdlib.h>
#include <cstring>

#include "CAuthenticationProfileImpl.hpp"
#include "CRSAEncryptDecrypt.hpp"

using CryptoPP::ByteQueue;
using namespace std;

Logger CAuthenticationProfileImpl::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profiles.CAuthenticationProfileImpl"));

void CAuthenticationProfileImpl::sendPublicKey()
{
   RSA::PublicKey publicKey = CRSAEncryptDecrypt::getPublicKey(mpAppCallbacks->getInternalPath());
   sendPublicKey(publicKey);

   sendTrustListUID(mpTrustList->getOurUid());
}

void CAuthenticationProfileImpl::writeRemoteUIDToTrustList()
{
   LOG4CPLUS_INFO(msLogger, __PRETTY_FUNCTION__);
   mpTrustList->addUid(mRemoteUID);
}

void CAuthenticationProfileImpl::sendPIN(const int first_digit, const int second_digit, const int third_digit, const int fourth_digit)
{
   LOG4CPLUS_INFO(msLogger, "CAuthenticationProfileImpl::sendPIN(" + convertIntegerToString(first_digit)
                                                                   + convertIntegerToString(second_digit)
                                                                   + convertIntegerToString(third_digit)
                                                                   + convertIntegerToString(fourth_digit) + ")");

   string PIN = string(convertIntegerToString(first_digit)
                     + convertIntegerToString(second_digit)
                     + convertIntegerToString(third_digit)
                     + convertIntegerToString(fourth_digit));

   string encryptedPIN = CRSAEncryptDecrypt::encrypt(PIN, remoteHostPublicKey);

   sendEncryptedPIN(encryptedPIN);
}

void CAuthenticationProfileImpl::sendEncryptedPIN(const std::string& PIN)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   sendEncryptedString(PIN, SEND_ENCRYPTED_PIN);
}


void CAuthenticationProfileImpl::sendEncryptedString(const std::string& toSend, PROCEDURES_IDS procID) 
{
   UInt8* outgoingData = new UInt8[toSend.length() + 1];
   outgoingData[0] = static_cast<UInt8>(procID);
   memcpy(outgoingData + 1, (void*)const_cast<char*>(toSend.c_str()), toSend.length() + 1);
   iviLink::Channel::sendBuffer(mChannelID, outgoingData, toSend.length() + 1);
   delete[] outgoingData;
}

void CAuthenticationProfileImpl::sendProcedureId(const PROCEDURES_IDS id)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   UInt8* outgoingData = new UInt8[1];
   outgoingData[0] = static_cast<UInt8>(id);
   iviLink::Channel::sendBuffer(mChannelID, outgoingData, 1);
   delete[] outgoingData;
}

void CAuthenticationProfileImpl::sendPublicKey(RSA::PublicKey publicKey)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
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

void CAuthenticationProfileImpl::sendTrustListUID(const iviLink::BaseUid uid)
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
   sendProcedureId(YOUR_UID_IS_NOK);
}

void CAuthenticationProfileImpl::sendYourUIDIsKnow()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   sendProcedureId(YOUR_UID_IS_OK);
}

void CAuthenticationProfileImpl::readyToExit()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   sendProcedureId(READY_TO_EXIT);
}

void CAuthenticationProfileImpl::onBufferReceived(const iviLink::Channel::tChannelId channel, Buffer const& buffer)
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
      string decryptedRemotePIN = CRSAEncryptDecrypt::decrypt(encryptedPIN, CRSAEncryptDecrypt::getPrivateKey(mpAppCallbacks->getInternalPath()));
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
   }
   else if(incomingData[0] == SEND_UID)
   {
      LOG4CPLUS_INFO(msLogger, "Remote UID received");
      string uid((char*)(incomingData + 1), read_size - 1);
      LOG4CPLUS_INFO(msLogger, "UID = " + uid);
      mRemoteUID = iviLink::BaseUid(uid);

      if(mpTrustList->isKnownUid(mRemoteUID))
      {
          LOG4CPLUS_INFO(msLogger, "Remote UID is known");

          remoteUIDIsChecked = true;
          remoteUIDIsOK = true;

          sendYourUIDIsKnow();
          validateUIDs();
      }
      else
      {
         LOG4CPLUS_INFO(msLogger, "Remote UID is unknown");

         remoteUIDIsChecked = true;
         remoteUIDIsOK = false;

         sendYourUIDIsUnknown();
         validateUIDs();
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
      validateUIDs();
   }
   else if(incomingData[0] == READY_TO_EXIT)
   {
      mpAppCallbacks->onReadyToExit();
   }
}

void CAuthenticationProfileImpl::onChannelDeleted(const UInt32 channel_id)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

   if (mChannelID == channel_id) mChannelID = 0;

   iviLink::Error err = iviLink::Channel::deallocateChannel(channel_id);
   if (!err.isNoError())
   {
      LOG4CPLUS_WARN(msLogger, "Unable to deallocate channel: " + static_cast<std::string>(err));
   }
}

void CAuthenticationProfileImpl::onConnectionLost()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

void CAuthenticationProfileImpl::validateUIDs()
{
   static bool authDecided = false;
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   
   if(authDecided) 
   {
      LOG4CPLUS_INFO(msLogger, "Auth was already decided");
      return;
   }

   if(localUIDIsChecked && remoteUIDIsChecked)
   {
      if(localUIDIsOK && remoteUIDIsOK)
      {
         LOG4CPLUS_INFO(msLogger, "Both UIDs were checked and are OK");
         if(!authDecided) 
         {
            authDecided = true;
            mpAppCallbacks->onAuthenticationIsNotRequired();
         }
      }
      else
      {
         LOG4CPLUS_INFO(msLogger, "Both UIDs are checked and one of them is not OK: ");
         if(!localUIDIsOK) LOG4CPLUS_INFO(msLogger, "    ours is not OK");
         if(!remoteUIDIsOK) LOG4CPLUS_INFO(msLogger, "    theirs is not OK");
         if(!authDecided)
         {
            authDecided = true;
            mpAppCallbacks->onAuthenticationIsRequired();
         }
      }
   }
   else
   {

      if(!localUIDIsChecked)
      {
         LOG4CPLUS_INFO(msLogger, "Local UID is not checked yet");
      }

      if(!remoteUIDIsChecked)
      {
         LOG4CPLUS_INFO(msLogger, "Remote UID is not checked yet");
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
   mpTrustList = new CTrustList(mpAppCallbacks->getInternalPath());

   LOG4CPLUS_INFO(msLogger, "CAuthenticationProfileImpl::CAuthenticationProfileImpl");
}

CAuthenticationProfileImpl::~CAuthenticationProfileImpl()
{
   delete mpTrustList;
}

void CAuthenticationProfileImpl::onEnable()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   iviLink::Error err = iviLink::Channel::allocateChannel("CAuthenticationProfileImpl", this, mChannelID, eRealTime);
   if (err.isNoError())
   {
      LOG4CPLUS_INFO(msLogger, "Channel allocated, starting the communication...");
   }
   else
   {
      LOG4CPLUS_WARN(msLogger, "allocate Channel failed");
      mpAppCallbacks->onProfileError();
   }
}

void CAuthenticationProfileImpl::onDisable()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   iviLink::Error err = iviLink::Channel::deallocateChannel(mChannelID);
   if (!err.isNoError())
   {
      LOG4CPLUS_WARN(msLogger, "Unable to deallocate channel: " + static_cast<std::string>(err));
   }
}

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


#include "CCarrierAdapter.hpp"
#include "Frame.hpp"
#include "Logger.hpp"
#include "byteOrder.hpp"
#include "ConnectivityAgentError.hpp"

#include "IFrameReceiver.hpp"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <unistd.h>

using namespace iviLink::ConnectivityAgent::HAL;
using namespace iviLink::ConnectivityAgent;


Logger CCarrierAdapter::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.HAL.CCarrierAdapter"));

CCarrierAdapter::CCarrierAdapter() :
   mGenderType(eAnyGender),
   mFilledSize(0),
   mpFrameReceiver(NULL)
{

}

CCarrierAdapter::~CCarrierAdapter()
{

}

ConnectivityAgentError CCarrierAdapter::sendFrame(iviLink::ConnectivityAgent::HAL::Frame* pFrame)
{
   // Will be better to make member array or even static?
   LOG4CPLUS_INFO(logger, "CCarrierAdapter::sendFrame()=> frame = " + convertIntegerToString((intptr_t)pFrame));
   const UInt32 max_size = sizeof(iviLink::ConnectivityAgent::HAL::Frame);
   UInt8 pArray[max_size];

   UInt32 size = pFrame->getSize() + sizeof(iviLink::ConnectivityAgent::HAL::Frame::FrameHeader);

   CFrameConverter::frameToArray(pFrame, pArray);

   LOG4CPLUS_INFO(logger, "CCarrierAdapter::sendFrame number = "
           + convertIntegerToString(pFrame->mFrameHeader.number) + ",  channel = "
           + convertIntegerToString(pFrame->mFrameHeader.channel_id) + ", size "
           + convertIntegerToString(size) + ", pArray addr "
           + convertIntegerToString((intptr_t)&pArray)
           + "pArray " + convertIntegerToString((intptr_t)pArray));

   return sendPreparedArray(pArray, size);
}

void CCarrierAdapter::registerFrameReceiver(iviLink::ConnectivityAgent::L0::IFrameReceiver& receiver)
{
   mFrameReceiverLock.lock();
   mpFrameReceiver = &receiver;
   mFrameReceiverLock.unlock();
}

const char* CCarrierAdapter::getRemoteAddress() const
{
   return NULL;
}

const char* CCarrierAdapter::getLocalAddress() const
{
   return NULL;
}

EGenderType CCarrierAdapter::getGender() const
{
   return mGenderType;
}


void CCarrierAdapter::haveData()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   UInt8* pArray = reinterpret_cast<UInt8*>(&mFrame);

   // Reading as many as possible

   UInt32 size = sizeof(mFrame) - mFilledSize;
   ConnectivityAgentError receiveResult = receiveRawArray(pArray + mFilledSize, size);
   if (!receiveResult.isNoError())
   {
      return;
   }

   UInt32 saved_size = mFilledSize;
   mFilledSize += size;

   while (mFilledSize >= sizeof(mFrame.mFrameHeader))
   {
      // header done
      if (saved_size < sizeof(mFrame.mFrameHeader))
      {
         // header become ready right now
         CFrameConverter::arrayToFrame(pArray, &mFrame);
      }

      const UInt32 full_size = mFrame.getSize() + sizeof(iviLink::ConnectivityAgent::HAL::Frame::FrameHeader);
      if (full_size <= mFilledSize)
      {
         // frame done

         // ! The error returned is not processed 
         processFrame(mFrame);

         memmove(pArray, pArray + full_size, mFilledSize - full_size);
         mFilledSize -= full_size;
         saved_size = 0;
      }
      else
      {
         break;
      }
   } // while
}

ConnectivityAgentError CCarrierAdapter::processFrame(iviLink::ConnectivityAgent::HAL::Frame & frame)
{
   ConnectivityAgentError processResult = ConnectivityAgentError::NoError();

   mFrameReceiverLock.lock();
   if (mpFrameReceiver)
   {
      processResult = mpFrameReceiver->receiveFrame(frame);
      if(processResult.isNoError() || processResult.getCode() == ConnectivityAgentError::ERROR_RESEND_ACK)
      {
         if (!frame.isACK())
         {
            sendACK(frame);
         }
      }   
      else
      {
         LOG4CPLUS_INFO(logger, "CCarrierAdapter::receiveFrame ERROR mpFrameReceiver->receiveFrame returned "
               + convertIntegerToString((int)processResult.getCode()) + ". ACK is not send");
      }
   }
   mFrameReceiverLock.unlock();

   return processResult;
}

ConnectivityAgentError CCarrierAdapter::sendACK(iviLink::ConnectivityAgent::HAL::Frame &frame)
{
   LOG4CPLUS_INFO(logger, "CCarrierAdapter::sendACK() to frame number "
         + convertIntegerToString(frame.mFrameHeader.number) + ", ChID "
         + convertIntegerToString(frame.mFrameHeader.channel_id));

   Frame::FrameHeader fh;
   Frame& frameACK = *reinterpret_cast<Frame*>(&fh);

   frameACK.mFrameHeader.sizeAndFlags = 0;
   frameACK.mFrameHeader.channel_id = frame.mFrameHeader.channel_id;
   frameACK.mFrameHeader.number = frame.mFrameHeader.number;
   frameACK.setACK();
   return sendFrame(&frameACK);
}

void CCarrierAdapter::CFrameConverter::arrayToFrame(const UInt8* pArray, iviLink::ConnectivityAgent::HAL::Frame* pFrame)
{
   const iviLink::ConnectivityAgent::HAL::Frame* pFrameArray = reinterpret_cast<const iviLink::ConnectivityAgent::HAL::Frame*>(pArray);

   pFrame->mFrameHeader.sizeAndFlags = ByteOrder::ntoh16(pFrameArray->mFrameHeader.sizeAndFlags);
   pFrame->mFrameHeader.channel_id =   ByteOrder::ntoh32(pFrameArray->mFrameHeader.channel_id);
   pFrame->mFrameHeader.number =       ByteOrder::ntoh32(pFrameArray->mFrameHeader.number);

   if (pFrameArray != pFrame && pFrame->getSize() > 0)
   {
      std::copy(pFrameArray->data, pFrameArray->data + pFrame->getSize(), pFrame->data);
   }
}

void CCarrierAdapter::CFrameConverter::frameToArray(const iviLink::ConnectivityAgent::HAL::Frame* pFrame, UInt8* pArray)
{
   iviLink::ConnectivityAgent::HAL::Frame* pFrameArray = reinterpret_cast<iviLink::ConnectivityAgent::HAL::Frame*>(pArray);
   const UInt32 saved_size = pFrame->getSize();
   pFrameArray->mFrameHeader.sizeAndFlags = ByteOrder::hton16(pFrame->mFrameHeader.sizeAndFlags);
   pFrameArray->mFrameHeader.channel_id =   ByteOrder::hton32(pFrame->mFrameHeader.channel_id);
   pFrameArray->mFrameHeader.number =       ByteOrder::hton32(pFrame->mFrameHeader.number);

   if (pFrameArray != pFrame && saved_size > 0)
   {
      std::copy(pFrame->data, pFrame->data + saved_size, pFrameArray->data);
   }
}



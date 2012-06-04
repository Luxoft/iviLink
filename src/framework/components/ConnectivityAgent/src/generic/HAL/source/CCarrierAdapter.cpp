/* 
 *  iviLINK SDK, version 0.9 (for preview only)                                      
 *    http://www.ivilink.net                                                         
 *  Cross Platform Application Communication Stack for In-Vehicle Applications       
 *                                                                                   
 *  Copyright (C) 2012, Luxoft Professional Corp., member of IBS group               
 *                                                                                   
 *  This library is free software; you can redistribute it and/or                    
 *  modify it under the terms of the GNU Lesser General Public                       
 *  License as published by the Free Software Foundation; version 2.1.               
 *                                                                                   
 *  This library is distributed in the hope that it will be useful,                  
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                
 *  Lesser General Public License for more details.                                  
 *                                                                                   
 *  You should have received a copy of the GNU Lesser General Public                 
 *  License along with this library; if not, write to the Free Software              
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA   
 * 
 * 
 * 
 * 
 */





#include "CCarrierAdapter.hpp"
#include "Frame.hpp"
#include "IFrameReceiver.hpp"
#include"utils/misc/include/Logger.hpp"
#include"utils/misc/include/byteOrder.hpp"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <unistd.h>

Logger CCarrierAdapter::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.HAL.CCarrierAdapter"));

CCarrierAdapter::CCarrierAdapter()
{

}

CCarrierAdapter::~CCarrierAdapter()
{

}

ERROR_CODE CCarrierAdapter::sendFrame(Frame* pFrame)
{
   // Will be better to make member array or even static?
   LOG4CPLUS_INFO(logger, "CCarrierAdapter::sendFrame()=> frame = " + convertIntegerToString((intptr_t)pFrame));
   const UInt32 max_size = sizeof(Frame);
   UInt8 pArray[max_size];

   CFrameConverter::frameToArray(pFrame, pArray);

   UInt32 size = pFrame->getSize() + sizeof(Frame::FrameHeader);

   LOG4CPLUS_INFO(logger, "CCarrierAdapter::sendFrame number = "
           + convertIntegerToString(pFrame->mFrameHeader.number) + ",  channel = "
           + convertIntegerToString(pFrame->mFrameHeader.channel_id) + ", size "
           + convertIntegerToString(size) + ", pArray addr "
           + convertIntegerToString((intptr_t)&pArray)
           + "pArray " + convertIntegerToString((intptr_t)pArray));

   return sendPrepearedArray(pArray, size);
}

ERROR_CODE CCarrierAdapter::registerFrameReceiver(IFrameReceiver& receiver)
{
   mFrameReceiverLock.lock();
   mpFrameReceiver = &receiver;
   mFrameReceiverLock.unlock();
   return ERR_OK;
}

const char* CCarrierAdapter::getRemoteAddress() const
{
   return NULL;
}

const char* CCarrierAdapter::getLocalAddress() const
{
   return NULL;
}

ERROR_CODE CCarrierAdapter::receiveFrame(Frame& frame, bool retry)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__ );

   UInt8* pArray = reinterpret_cast<UInt8*>(&frame);
   const UInt32 headerSize = sizeof(Frame::FrameHeader);
   assert(headerSize > 0);
   UInt32 size = headerSize;

   ERROR_CODE errorCode = ERR_OK;
   if (!retry)
   {
      errorCode = this->receiveRawArray(pArray, size);

      if (errorCode == ERR_AGAIN)
      {
         return ERR_OK;
      }
   }

   if (errorCode == ERR_OK && size == headerSize)
   {
      if (!retry)
      {
         CFrameConverter::arrayToFrame(pArray, &frame);
      }

      UInt32 payload_size = frame.getSize();
      assert(payload_size <= sizeof(frame.data));

      if (payload_size > 0)
      {
         errorCode = this->receiveRawArray(frame.data, payload_size);
         /// @todo error handling
         LOG4CPLUS_INFO(logger, "todo: error handling");
      }

      if(errorCode == ERR_OK)
      {
         mFrameReceiverLock.lock();
         if (mpFrameReceiver)
         {
            errorCode = mpFrameReceiver->receiveFrame(frame);
            if(ERR_OK == errorCode || ERR_RESEND_ACK == errorCode)
            {
               if (!frame.isACK())
               {
                  sendACK(frame);
               }
            }   
            else
            {
               LOG4CPLUS_INFO(logger, "CCarrierAdapter::receiveFrame ERROR mpFrameReceiver->receiveFrame returned "
                     + convertIntegerToString((int)errorCode) + ". ACK is not send");
            }
         }

         mFrameReceiverLock.unlock();
      }
   }
   // TODO: normal error handling
   LOG4CPLUS_INFO(logger, "todo: normal error handling");
   return errorCode;
}
void CCarrierAdapter::resendACK(UInt32 channel_id, UInt32 frame_number)
{
   LOG4CPLUS_INFO(logger, "CCarrierAdapter::resendACK() to frame number " + convertIntegerToString(frame_number)
         + ", ChID " + convertIntegerToString(channel_id));

   Frame frameACK;
   frameACK.mFrameHeader.sizeAndFlags = 0;
   frameACK.mFrameHeader.channel_id = channel_id;
   frameACK.mFrameHeader.number = frame_number;
   frameACK.setACK();
   sendFrame(&frameACK);
}

ERROR_CODE CCarrierAdapter::sendACK(Frame &frame)
{
   LOG4CPLUS_INFO(logger, "CCarrierAdapter::sendACK() to frame number "
         + convertIntegerToString(frame.mFrameHeader.number) + ", ChID "
         + convertIntegerToString(frame.mFrameHeader.channel_id));

   Frame frameACK;
   frameACK.mFrameHeader.sizeAndFlags = 0;
   frameACK.mFrameHeader.channel_id = frame.mFrameHeader.channel_id;
   frameACK.mFrameHeader.number = frame.mFrameHeader.number;
   frameACK.setACK();
   return sendFrame(&frameACK);
}

void CCarrierAdapter::CFrameConverter::arrayToFrame(const UInt8* pArray, Frame* pFrame)
{
   const Frame* pFrameArray = reinterpret_cast<const Frame*>(pArray);

   pFrame->mFrameHeader.sizeAndFlags = ByteOrder::ntoh16(pFrameArray->mFrameHeader.sizeAndFlags);
   pFrame->mFrameHeader.channel_id =   ByteOrder::ntoh32(pFrameArray->mFrameHeader.channel_id);
   pFrame->mFrameHeader.number =       ByteOrder::ntoh32(pFrameArray->mFrameHeader.number);

   if (pFrameArray != pFrame && pFrame->getSize() > 0)
   {
      std::copy(pFrameArray->data, pFrameArray->data + pFrame->getSize(), pFrame->data);
   }
}

void CCarrierAdapter::CFrameConverter::frameToArray(const Frame* pFrame, UInt8* pArray)
{
   Frame* pFrameArray = reinterpret_cast<Frame*>(pArray);
   pFrameArray->mFrameHeader.sizeAndFlags = ByteOrder::hton16(pFrame->mFrameHeader.sizeAndFlags);
   pFrameArray->mFrameHeader.channel_id =   ByteOrder::hton32(pFrame->mFrameHeader.channel_id);
   pFrameArray->mFrameHeader.number =       ByteOrder::hton32(pFrame->mFrameHeader.number);

   if (pFrameArray != pFrame && pFrame->getSize() > 0)
   {
      std::copy(pFrame->data, pFrame->data + pFrame->getSize(), pFrameArray->data);
   }
}



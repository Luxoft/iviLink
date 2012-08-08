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









#include <cassert>

#include "CPmpCoreClbCtrl.hpp"
#include "PmpRequestFrame.hpp"
#include "utils/serialize/Serialize.hpp"
#include "utils/misc/byteOrder.hpp"

namespace iviLink
{

   namespace PMP
   {

      Logger CPmpCoreClbCtrl::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.interaction.CPmpCoreClbCtrl"));

      CPmpCoreClbCtrl::CPmpCoreClbCtrl(IPmpProtocol * pPtorocol, IPmpCoreClb * pCoreClb)
         : mpProtocol(pPtorocol), mpCoreClb(pCoreClb)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         assert(mpProtocol);
         assert(mpCoreClb);
      }

      CPmpCoreClbCtrl::~CPmpCoreClbCtrl()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
      }

      void CPmpCoreClbCtrl::onGetAvailableProfileComplements(PMPFrame * pFrame)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         std::list<Profile::Uid> complements;
         mpCoreClb->onGetAvailableProfileComplements(complements);

         UInt32 size = sizeof(PMPFrame) + 4;
         for (std::list<Profile::Uid>::iterator it = complements.begin();
               complements.end() != it; ++it)
         {
            size += stringInBufSize(it->value());
         }

         PMPFrame * pResp = reinterpret_cast<PMPFrame*>(new UInt8[size]);
         pResp->size = size;
         pResp->frameType = PMP_FRAME_TYPE_RESPONSE;
         pResp->id = pFrame->id;
         pResp->client = pFrame->client;
         pResp->reqType = pFrame->reqType;

         UInt32 count = complements.size();
         LOG4CPLUS_INFO(msLogger, "count: " + convertIntegerToString(count));
         count = ByteOrder::hton32(count);
         memcpy(pResp->data,&count,4);
         UInt32 pos = 4;
         for (std::list<Profile::Uid>::iterator it = complements.begin();
               complements.end() != it; ++it)
         {
            stringToBuffer(it->value(),pResp->data + pos,true);
            pos += stringInBufSize(it->value());
         }

         assert(pos == size-sizeof(PMPFrame));
         mpProtocol->makeResponse(*pResp);
         delete [] pResp;
      }

      void CPmpCoreClbCtrl::onReloadProfilesFromRepository (PMPFrame * pFrame)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpCoreClb->onReloadProfilesFromRepository();

         PMPFrame * pResp = reinterpret_cast<PMPFrame*>(new UInt8[sizeof(PMPFrame)]);
         pResp->size = sizeof(PMPFrame);
         pResp->frameType = PMP_FRAME_TYPE_RESPONSE;
         pResp->id = pFrame->id;
         pResp->client = pFrame->client;
         pResp->reqType = pFrame->reqType;

         mpProtocol->makeResponse(*pResp);
         delete [] pResp;
      }

      void CPmpCoreClbCtrl::onResetProfilesState(PMPFrame * pFrame)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpCoreClb->onResetProfilesState();

         PMPFrame * pResp = reinterpret_cast<PMPFrame*>(new UInt8[sizeof(PMPFrame)]);
         pResp->size = sizeof(PMPFrame);
         pResp->frameType = PMP_FRAME_TYPE_RESPONSE;
         pResp->id = pFrame->id;
         pResp->client = pFrame->client;
         pResp->reqType = pFrame->reqType;

         mpProtocol->makeResponse(*pResp);
         delete [] pResp;
      }

      void CPmpCoreClbCtrl::onLock(PMPFrame * pFrame)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         iviLink::CUid id(bufferToString(pFrame->data,true));
         bool res = mpCoreClb->onLock(id);

         PMPFrame * pResp = reinterpret_cast<PMPFrame*>(new UInt8[sizeof(PMPFrame)+1]);
         pResp->size = sizeof(PMPFrame)+1;
         pResp->frameType = PMP_FRAME_TYPE_RESPONSE;
         pResp->id = pFrame->id;
         pResp->client = pFrame->client;
         pResp->reqType = pFrame->reqType;
         pResp->data[0] = static_cast<UInt8>(res);

         mpProtocol->makeResponse(*pResp);
         delete [] pResp;
      }

      void CPmpCoreClbCtrl::onUnlock(PMPFrame * pFrame)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         iviLink::CUid id(bufferToString(pFrame->data,true));
         bool res = mpCoreClb->onUnlock(id);

         PMPFrame * pResp = reinterpret_cast<PMPFrame*>(new UInt8[sizeof(PMPFrame)+1]);
         pResp->size = sizeof(PMPFrame)+1;
         pResp->frameType = PMP_FRAME_TYPE_RESPONSE;
         pResp->id = pFrame->id;
         pResp->client = pFrame->client;
         pResp->reqType = pFrame->reqType;
         pResp->data[0] = static_cast<UInt8>(res);

         mpProtocol->makeResponse(*pResp);
         delete [] pResp;
      }

      void CPmpCoreClbCtrl::onLockAll(PMPFrame * pFrame)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpCoreClb->onLockAll();

         PMPFrame * pResp = reinterpret_cast<PMPFrame*>(new UInt8[sizeof(PMPFrame)]);
         pResp->size = sizeof(PMPFrame);
         pResp->frameType = PMP_FRAME_TYPE_RESPONSE;
         pResp->id = pFrame->id;
         pResp->client = pFrame->client;
         pResp->reqType = pFrame->reqType;

         mpProtocol->makeResponse(*pResp);
         delete [] pResp;
      }

      void CPmpCoreClbCtrl::onUnlockAll(PMPFrame * pFrame)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpCoreClb->onUnlockAll();

         PMPFrame * pResp = reinterpret_cast<PMPFrame*>(new UInt8[sizeof(PMPFrame)]);
         pResp->size = sizeof(PMPFrame);
         pResp->frameType = PMP_FRAME_TYPE_RESPONSE;
         pResp->id = pFrame->id;
         pResp->client = pFrame->client;
         pResp->reqType = pFrame->reqType;

         mpProtocol->makeResponse(*pResp);
         delete [] pResp;
      }

      void CPmpCoreClbCtrl::onDisableByClient(PMPFrame * pFrame)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         iviLink::CUid id(bufferToString(pFrame->data,true));
         bool res = mpCoreClb->onDisableByClient(id);

         PMPFrame * pResp = reinterpret_cast<PMPFrame*>(new UInt8[sizeof(PMPFrame)+1]);
         pResp->size = sizeof(PMPFrame)+1;
         pResp->frameType = PMP_FRAME_TYPE_RESPONSE;
         pResp->id = pFrame->id;
         pResp->client = pFrame->client;
         pResp->reqType = pFrame->reqType;
         pResp->data[0] = static_cast<UInt8>(res);

         mpProtocol->makeResponse(*pResp);
         delete [] pResp;
      }

      void CPmpCoreClbCtrl::onEnableByClient(PMPFrame * pFrame)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         CUid id(bufferToString(pFrame->data,true));
         bool res = mpCoreClb->onEnableByClient(id);

         PMPFrame * pResp = reinterpret_cast<PMPFrame*>(new UInt8[sizeof(PMPFrame)+1]);
         pResp->size = sizeof(PMPFrame)+1;
         pResp->frameType = PMP_FRAME_TYPE_RESPONSE;
         pResp->id = pFrame->id;
         pResp->client = pFrame->client;
         pResp->reqType = pFrame->reqType;
         pResp->data[0] = static_cast<UInt8>(res);

         mpProtocol->makeResponse(*pResp);
         delete [] pResp;
      }

      void CPmpCoreClbCtrl::onEnableByClientAll(PMPFrame * pFrame)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpCoreClb->onEnableByClientAll();

         PMPFrame * pResp = reinterpret_cast<PMPFrame*>(new UInt8[sizeof(PMPFrame)]);
         pResp->size = sizeof(PMPFrame);
         pResp->frameType = PMP_FRAME_TYPE_RESPONSE;
         pResp->id = pFrame->id;
         pResp->client = pFrame->client;
         pResp->reqType = pFrame->reqType;

         mpProtocol->makeResponse(*pResp);
         delete [] pResp;
      }

   }

}

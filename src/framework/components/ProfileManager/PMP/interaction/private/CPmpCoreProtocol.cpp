/* 
 * 
 * iviLINK SDK, version 1.0
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
#include <cstring>

#include "utils/misc/byteOrder.hpp"
#include "CPmpCoreProtocol.hpp"
#include "PmpRequestFrame.hpp"
#include "utils/serialize/Serialize.hpp"
#include "framework/components/ProfileManager/PMP/CPmpError.hpp"
namespace iviLink
{

   namespace PMP
   {

      Logger CPmpCoreProtocol::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.interaction.CPmpCoreProtocol"));

      CPmpCoreProtocol::CPmpCoreProtocol(IPmpProtocol * pPmpProtocol)
         : mpPmpProtocol(pPmpProtocol)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         assert(mpPmpProtocol);
      }

      CPmpCoreProtocol::~CPmpCoreProtocol()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
      }

      CError CPmpCoreProtocol::getAvailableProfileComplements(std::list<Profile::Uid> & complements)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         PMPFrame * pReq = new PMPFrame;
         pReq->size = sizeof(PMPFrame);
         pReq->reqType = PMP_REQ_CORE_GET_COMPLEMENTS;

         PMPFrame * pResp = mpPmpProtocol->makeCoreRequest(*pReq);
         delete pReq;

         if (!pResp)
         {
            LOG4CPLUS_ERROR(msLogger, "Network error");
            return CPmpError(CPmpError::ERROR_NETWORK);
         }

         UInt32 count;
         memcpy(&count,pResp->data,4);
         count = ByteOrder::ntoh32(count);
         int pos = 4;
         LOG4CPLUS_INFO(msLogger, "RES COUNT : " + convertIntegerToString(count));
         for (UInt32 i = 0; i<count; ++i)
         {
            std::string str = bufferToString(pResp->data+pos,true);
            complements.push_back(Profile::Uid(str));
            pos += stringInBufSize(str);
         }

         delete[] pResp;
         return CPmpError::NoPmpError();
      }

      CError CPmpCoreProtocol::reloadProfilesFromRepository()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         PMPFrame * pReq = new PMPFrame;
         pReq->size = sizeof(PMPFrame);
         pReq->reqType = PMP_REQ_CORE_RELOAD_PROFS_FROM_REPO;

         PMPFrame * pResp = mpPmpProtocol->makeCoreRequest(*pReq);
         delete pReq;

         if (!pResp)
         {
            return CPmpError(CPmpError::ERROR_NETWORK);
         }

         delete[] pResp;
         return CPmpError::NoPmpError();
      }

      CError CPmpCoreProtocol::resetProfileState()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         PMPFrame * pReq = new PMPFrame;
         pReq->size = sizeof(PMPFrame);
         pReq->reqType = PMP_REQ_CORE_RESET_PROF_STATE;

         PMPFrame * pResp = mpPmpProtocol->makeCoreRequest(*pReq);
         delete pReq;

         if (!pResp)
         {
            return CPmpError(CPmpError::ERROR_NETWORK);
         }

         delete[] pResp;
         return CPmpError::NoPmpError();
      }

      CError CPmpCoreProtocol::lock(iviLink::CUid id)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         LOG4CPLUS_INFO(msLogger, "id: " + id.value());
         UInt32 reqSize = sizeof(PMPFrame);
         reqSize += stringInBufSize(id.value());
         PMPFrame * pReq = reinterpret_cast<PMPFrame*>(new UInt8[reqSize]);
         pReq->size = reqSize;
         pReq->reqType = PMP_REQ_CORE_LOCK;
         stringToBuffer(id.value(), pReq->data,true);

         PMPFrame * pResp = mpPmpProtocol->makeCoreRequest(*pReq);
         delete [] pReq;

         if (!pResp)
         {
            return CPmpError(CPmpError::ERROR_NETWORK);
         }

         bool res = pResp->data[0];
         delete[] pResp;
         return res ? CPmpError::NoPmpError() : CPmpError(CPmpError::ERROR_FUNCTION_FAILED);
      }

      CError CPmpCoreProtocol::unlock(iviLink::CUid id)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         LOG4CPLUS_INFO(msLogger, "id: " + id.value());
         UInt32 reqSize = sizeof(PMPFrame);
         reqSize += stringInBufSize(id.value());
         PMPFrame * pReq = reinterpret_cast<PMPFrame*>(new UInt8[reqSize]);
         pReq->size = reqSize;
         pReq->reqType = PMP_REQ_CORE_UNLOCK;
         stringToBuffer(id.value(), pReq->data,true);

         PMPFrame * pResp = mpPmpProtocol->makeCoreRequest(*pReq);
         delete [] pReq;

         if (!pResp)
         {
            return CPmpError(CPmpError::ERROR_NETWORK);
         }

         bool res = pResp->data[0];
         delete[] pResp;
         return res ? CPmpError::NoPmpError() : CPmpError(CPmpError::ERROR_FUNCTION_FAILED);
      }

      CError CPmpCoreProtocol::lockAll()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         PMPFrame * pReq = new PMPFrame;
         pReq->size = sizeof(PMPFrame);
         pReq->reqType = PMP_REQ_CORE_LOCK_ALL;

         PMPFrame * pResp = mpPmpProtocol->makeCoreRequest(*pReq);
         delete pReq;

         if (!pResp)
         {
            return CPmpError(CPmpError::ERROR_NETWORK);
         }

         delete[] pResp;
         return CPmpError::NoPmpError();
      }

      CError CPmpCoreProtocol::unlockAll()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         PMPFrame * pReq = new PMPFrame;
         pReq->size = sizeof(PMPFrame);
         pReq->reqType = PMP_REQ_CORE_UNLOCK_ALL;

         PMPFrame * pResp = mpPmpProtocol->makeCoreRequest(*pReq);
         delete pReq;

         if (!pResp)
         {
            return CPmpError(CPmpError::ERROR_NETWORK);
         }

         delete[] pResp;
         return CPmpError::NoPmpError();
      }

      CError CPmpCoreProtocol::disableByClient(iviLink::CUid id)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         LOG4CPLUS_INFO(msLogger, "id: " + id.value());
         UInt32 reqSize = sizeof(PMPFrame);
         reqSize += stringInBufSize(id.value());
         PMPFrame * pReq = reinterpret_cast<PMPFrame*>(new UInt8[reqSize]);
         pReq->size = reqSize;
         pReq->reqType = PMP_REQ_CORE_DIS_BY_CLIENT;
         stringToBuffer(id.value(),pReq->data,true);

         PMPFrame * pResp = mpPmpProtocol->makeCoreRequest(*pReq);
         delete [] pReq;

         LOG4CPLUS_INFO(msLogger, "after pReq deleting");

         if (!pResp)
         {
            return CPmpError(CPmpError::ERROR_NETWORK);
         }

         bool res = pResp->data[0];
         delete[] pResp;
         return res ? CPmpError::NoPmpError() : CPmpError(CPmpError::ERROR_FUNCTION_FAILED);
      }

      CError CPmpCoreProtocol::enableByClient(iviLink::CUid id)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         LOG4CPLUS_INFO(msLogger, "id: " + id.value());
         UInt32 reqSize = sizeof(PMPFrame);
         reqSize += stringInBufSize(id.value());
         PMPFrame * pReq = reinterpret_cast<PMPFrame*>(new UInt8[reqSize]);
         pReq->size = reqSize;
         pReq->reqType = PMP_REQ_CORE_ENABLE_BY_CLNT;
         stringToBuffer(id.value(), pReq->data,true);

         PMPFrame * pResp = mpPmpProtocol->makeCoreRequest(*pReq);
         delete [] pReq;

         if (!pResp)
         {
            return CPmpError(CPmpError::ERROR_NETWORK);
         }

         bool res = pResp->data[0];
         delete[] pResp;
         return res ? CPmpError::NoPmpError() : CPmpError(CPmpError::ERROR_FUNCTION_FAILED);
      }

      CError CPmpCoreProtocol::enableByClientAll()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         PMPFrame * pReq = new PMPFrame;
         pReq->size = sizeof(PMPFrame);
         pReq->reqType = PMP_REQ_CORE_ENABLE_BY_CLNT_ALL;

         PMPFrame * pResp = mpPmpProtocol->makeCoreRequest(*pReq);
         delete pReq;

         if (!pResp)
         {
            return CPmpError(CPmpError::ERROR_NETWORK);
         }

         delete[] pResp;
         return CPmpError::NoPmpError();
      }

   }

}

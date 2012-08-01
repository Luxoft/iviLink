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







#include <unistd.h>

#include "utils/misc/byteOrder.hpp"
#include "CPmpProtocol.hpp"
#include "CPmpCoreClbCtrl.hpp"
#include "CPmpPimClbCtrl.hpp"

namespace iviLink
{

   namespace PMP
   {

      Logger CPmpProtocol::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.interaction.CPmpProtocol"));

      CPmpProtocol::CPmpProtocol()
         : mChannelId(0)
         , mBe(true)
         , mTag("ProfileManagerPProtocol")
         , mGenId(0)
         , mpCoreClbCtrl(0)
         , mpPimClbCtrl(0)
         , mReady(false)
      {
      }

      CPmpProtocol::~CPmpProtocol()
      {
         delete mpCoreClbCtrl;
         delete mpPimClbCtrl;
      }

      void CPmpProtocol::channelDeletedCallback(const unsigned int channelId)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         LOG4CPLUS_INFO(msLogger, "channelId = " + convertIntegerToString(static_cast<int>(channelId)));

         /// @todo: we will not reconnect. Reconnection is not specified yet and
         /// we have no watchdog in channel supervisor, so it brings 
         /// the problems such as half-allocated channels.
         //disconnect(false);
         LOG4CPLUS_WARN(msLogger, "TODO proper reconnection support");
         disconnect(true);
      }

      void CPmpProtocol::connectionLostCallback()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mConnectionLostSem.signal();
      }

      void CPmpProtocol::connect()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         while (mBe)
         {
            LOG4CPLUS_INFO(msLogger, "connect attempt");
            UInt32 chid = 0;
            CError err = iviLink::ChannelSupervisor::allocateChannel(this,mTag.c_str(), chid);
            if (err.isNoError())
            {
               mChannelIdCond.lock();
               LOG4CPLUS_INFO(msLogger, "ok chid = " +
                     convertIntegerToString(static_cast<int>(chid)));
               mChannelId = chid;
               mChannelIdCond.broadcast();
               mChannelIdCond.unlock();

               mConnectionLostSem.wait();
            }
            else
            {
               LOG4CPLUS_INFO(msLogger, static_cast<std::string>(err));
               sleep(1);
            }
         }
      }

      void CPmpProtocol::disconnect(bool final)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

         mChannelIdCond.lock();
         UInt32 chid = mChannelId;
         mChannelId = 0;
         mChannelIdCond.broadcast();
         mChannelIdCond.unlock();

         if (final)
            mBe = false;

         mConnectionLostSem.signal();

         if (chid != 0)
         {
            iviLink::ChannelSupervisor::deallocateChannel(chid, mTag.c_str());
         }
      }

      UInt32 CPmpProtocol::getChannelId() const
      {
         UInt32 chid = 0;
         mChannelIdCond.lock();

         while (mChannelId == 0)
         {
            LOG4CPLUS_INFO(msLogger, "CPmpProtocol waiting for established connection");
            mChannelIdCond.wait();
         }

         chid = mChannelId;

         mChannelIdCond.unlock();

         return chid;
      }

      void CPmpProtocol::dataReceivedCallback(const unsigned int channelId, const unsigned int readSize)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

         unsigned char * data = new unsigned char [readSize];
         unsigned int recSize = 0;
         CError err =  iviLink::ChannelSupervisor::receiveData(mChannelId,mTag.c_str(),data,recSize,readSize);
         if(!err.isNoError())
         {
            LOG4CPLUS_INFO(msLogger, static_cast<std::string>(err));
            delete [] data;
            return;
         }

         PMPFrame * pFrame = reinterpret_cast<PMPFrame*>(data);

         pFrame->id = ByteOrder::ntoh32(pFrame->id);
         pFrame->size = ByteOrder::ntoh32(pFrame->size);

         LOG4CPLUS_INFO(msLogger, "id = " + convertIntegerToString(static_cast<int>(pFrame->id)) +
               " size = " + convertIntegerToString(static_cast<int>(pFrame->size)) +
               " client = " + convertIntegerToString(static_cast<int>(pFrame->client)) +
               " reqType = " + convertIntegerToString(static_cast<int>(pFrame->reqType)));

         if (PMP_FRAME_TYPE_REQUEST == pFrame->frameType)
         {
            onRequest(pFrame);
         }
         else
         {
            onResponse(pFrame);
         }
      }

      void CPmpProtocol::onRequest(PMPFrame * pFrame)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         
         switch(pFrame->client)
         {
         case PMP_CLIENT_CORE:
            onCoreRequest(pFrame);
            break;
         case PMP_CLIENT_PI_SRV:
            onPimRequest(pFrame);
            break;
         case PMP_CLIENT_CMP_MGR:
            onCmpMgrRequest(pFrame);
            break;
         default:
            LOG4CPLUS_WARN(msLogger, "undefined client");
            break;
         }
         delete [] pFrame;
      }

      void CPmpProtocol::onCoreRequest(PMPFrame * pFrame)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

         if (!mpCoreClbCtrl)
         {
            LOG4CPLUS_WARN(msLogger, "no callback for request");
            return;

         }

         switch (pFrame->reqType)
         {
         case PMP_REQ_CORE_GET_COMPLEMENTS:
            LOG4CPLUS_INFO(msLogger, "PMP_REQ_CORE_GET_COMPLEMENTS");
            mpCoreClbCtrl->onGetAvailableProfileComplements(pFrame);
            break;
         case PMP_REQ_CORE_RELOAD_PROFS_FROM_REPO:
            LOG4CPLUS_INFO(msLogger, "PMP_REQ_CORE_RELOAD_PROFS_FROM_REPO");
            mpCoreClbCtrl->onReloadProfilesFromRepository(pFrame);
            break;
         case PMP_REQ_CORE_RESET_PROF_STATE:
            LOG4CPLUS_INFO(msLogger, "PMP_REQ_CORE_RESET_PROF_STATE");
            mpCoreClbCtrl->onResetProfilesState(pFrame);
            break;
         case PMP_REQ_CORE_LOCK:
            LOG4CPLUS_INFO(msLogger, "PMP_REQ_CORE_LOCK");
            mpCoreClbCtrl->onLock(pFrame);
            break;
         case PMP_REQ_CORE_UNLOCK:
            LOG4CPLUS_INFO(msLogger, "PMP_REQ_CORE_UNLOCK");
            mpCoreClbCtrl->onUnlock(pFrame);
            break;
         case PMP_REQ_CORE_LOCK_ALL:
            LOG4CPLUS_INFO(msLogger, "PMP_REQ_CORE_LOCK_ALL");
            mpCoreClbCtrl->onLockAll(pFrame);
            break;
         case PMP_REQ_CORE_UNLOCK_ALL:
            LOG4CPLUS_INFO(msLogger, "PMP_REQ_CORE_UNLOCK_ALL");
            mpCoreClbCtrl->onUnlockAll(pFrame);
            break;
         case PMP_REQ_CORE_DIS_BY_CLIENT:
            LOG4CPLUS_INFO(msLogger, "PMP_REQ_CORE_DIS_BY_CLIENT");
            mpCoreClbCtrl->onDisableByClient(pFrame);
            break;
         case PMP_REQ_CORE_ENABLE_BY_CLNT:
            LOG4CPLUS_INFO(msLogger, "PMP_REQ_CORE_ENABLE_BY_CLNT");
            mpCoreClbCtrl->onEnableByClient(pFrame);
            break;
         case PMP_REQ_CORE_ENABLE_BY_CLNT_ALL:
            LOG4CPLUS_INFO(msLogger, "PMP_REQ_CORE_ENABLE_BY_CLNT_ALL");
            mpCoreClbCtrl->onEnableByClientAll(pFrame);
            break;
         default:
            LOG4CPLUS_WARN(msLogger, "Undefined core request");
            break;
         }
      }

      void CPmpProtocol::onPimRequest(PMPFrame * pFrame)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

         if (!mpPimClbCtrl)
         {
            LOG4CPLUS_WARN(msLogger, "no callback for request");
            return;
         }

         switch (pFrame->reqType)
         {
         case PMP_REQ_PIM_CREATE_PROFILE:
            LOG4CPLUS_INFO(msLogger, "PMP_REQ_PIM_CREATE_PROFILE");
            mpPimClbCtrl->onCreateProfile(pFrame);
            break;
         case PMP_REQ_PIM_PROFILE_DIED:
            LOG4CPLUS_INFO(msLogger, "PMP_REQ_PIM_PROFILE_DIED");
            mpPimClbCtrl->onProfileDied(pFrame);
            break;
         default:
            LOG4CPLUS_WARN(msLogger, "Undefined PI request");
            break;
         }
      }

      void CPmpProtocol::onCmpMgrRequest(PMPFrame * pFrame)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         switch (pFrame->reqType)
         {
         case PMP_REQ_CMP_MGR_READY:
            LOG4CPLUS_INFO(msLogger, "PMP_REQ_CMP_MGR_READY");
            {
               mReadyMutex.lock();
               mReady = true;
               mReadyMutex.unlock();

               PMPFrame * pResp = reinterpret_cast<PMPFrame*>(new UInt8[sizeof(PMPFrame)]);
               pResp->size = sizeof(PMPFrame);
               pResp->frameType = PMP_FRAME_TYPE_RESPONSE;
               pResp->id = pFrame->id;
               pResp->client = pFrame->client;
               pResp->reqType = pFrame->reqType;

               makeResponse(*pResp);
               delete [] pResp;
            }
            LOG4CPLUS_INFO(msLogger, "PMP_REQ_CMP_MGR_READY - END");
            break;
         default:
            LOG4CPLUS_WARN(msLogger, "Undefined CMP MGR request");
            break;
         }
      }

      void CPmpProtocol::onResponse(PMPFrame * pFrame)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mRequestMutex.lock();
         if (pFrame->reqType == PMP_REQ_CMP_MGR_READY)
         {
            LOG4CPLUS_INFO(msLogger, "Incoming response CMP MGR READY");
         }


         if(mWaitMap.end() == mWaitMap.find(pFrame->id))
         {
            LOG4CPLUS_WARN(msLogger, "mWaitMap is empty");
            mRequestMutex.unlock();
            delete [] pFrame;
            return;
         }

         mResponseMap[pFrame->id] = pFrame;
         mWaitMap[pFrame->id]->signal();
         mRequestMutex.unlock();
      }

      void CPmpProtocol::bufferOverflowCallback(const unsigned int channelId)
      {

      }

      PMPFrame * CPmpProtocol::makeCoreRequest(PMPFrame & request)
      {
         request.client = PMP_CLIENT_CORE;
         request.frameType = PMP_FRAME_TYPE_REQUEST;
         request.id = generateId();
         return makeRequest(request);
      }

      PMPFrame * CPmpProtocol::makePimRequest(PMPFrame & request)
      {
         request.client = PMP_CLIENT_PI_SRV;
         request.frameType = PMP_FRAME_TYPE_REQUEST;
         request.id = generateId();
         return makeRequest(request);
      }

      PMPFrame * CPmpProtocol::makeCmpMgrRequest(PMPFrame & request)
      {
         request.client = PMP_CLIENT_CMP_MGR;
         request.frameType = PMP_FRAME_TYPE_REQUEST;
         request.id = generateId();
         return makeRequest(request);
      }

      PMPFrame * CPmpProtocol::makeRequest(PMPFrame & requestFrame)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         LOG4CPLUS_INFO(msLogger, "id = " + convertIntegerToString(static_cast<int>(requestFrame.id)) +
                        " size = " + convertIntegerToString(static_cast<int>(requestFrame.size)) +
                        " client = " + convertIntegerToString(static_cast<int>(requestFrame.client)) +
                        " reqType = " + convertIntegerToString(static_cast<int>(requestFrame.reqType)));

         CSignalSemaphore semaphore;
         tMessageId inMapId = requestFrame.id;
         
         mRequestMutex.lock();
         mWaitMap[inMapId] = &semaphore;
         mRequestMutex.unlock();

         requestFrame.id = ByteOrder::hton32(requestFrame.id);
         unsigned int size = requestFrame.size;
         requestFrame.size = ByteOrder::hton32(requestFrame.size);
         CError err = iviLink::ChannelSupervisor::sendData(getChannelId(), mTag.c_str(), reinterpret_cast<unsigned char *>(&requestFrame), size);
         if (!err.isNoError())
         {
            return 0;
         }

         semaphore.waitTimeout(30000);
         mRequestMutex.lock();
         mWaitMap.erase(inMapId);
         if (mResponseMap.end() == mResponseMap.find(inMapId))
         {
            mRequestMutex.unlock();
            return 0;
         }

         PMPFrame * pFrame = mResponseMap[inMapId];
         mResponseMap.erase(inMapId);
         mRequestMutex.unlock();
         return pFrame;
      }

      void CPmpProtocol::makeResponse(PMPFrame & response)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

         LOG4CPLUS_INFO(msLogger, "id = " + convertIntegerToString(static_cast<int>(response.id)) +
                        " size = " + convertIntegerToString(static_cast<int>(response.size)) +
                        " client = " + convertIntegerToString(static_cast<int>(response.client)) +
                        " reqType = " + convertIntegerToString(static_cast<int>(response.reqType)));

         response.id = ByteOrder::hton32(response.id);
         unsigned int size = response.size;
         response.size = ByteOrder::hton32(response.size);
         iviLink::ChannelSupervisor::sendData(getChannelId(), mTag.c_str(), reinterpret_cast<unsigned char *>(&response), size);
      }

      tMessageId CPmpProtocol::generateId ()
      {
         mGenIdMutex.lock();
         tMessageId res = mGenId++;
         mGenIdMutex.unlock();
         return res;
      }

      void CPmpProtocol::registerCoreClb(IPmpCoreClb * pCoreClb)
      {
         delete mpCoreClbCtrl;
         if (pCoreClb)
         {
            mpCoreClbCtrl = new CPmpCoreClbCtrl(this,pCoreClb);
         }
         else
         {
            mpCoreClbCtrl = 0;
         }
      }

      void CPmpProtocol::registerPimClb(IPmpPimClb * pPimClb)
      {
         delete mpPimClbCtrl;
         if (pPimClb)
         {
            mpPimClbCtrl = new CPmpPimClbCtrl(this,pPimClb);
         }
         else
         {
            mpPimClbCtrl = 0;
         }
      }

      bool CPmpProtocol::ready()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         PMPFrame * pReq = new PMPFrame;
         pReq->size = sizeof(PMPFrame);
         pReq->reqType = PMP_REQ_CMP_MGR_READY;

         PMPFrame * pResp = makeCmpMgrRequest(*pReq);
         delete pReq;

         if (!pResp)
         {
            return false;
         }

         delete[] pResp;
         return true;
      }

      bool CPmpProtocol::isReady() const
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mReadyMutex.lock();
         bool res = mReady;
         mReadyMutex.unlock();
         return res;
      }

   }

}

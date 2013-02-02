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


#include <cassert>
#include <unistd.h>

#include "AmpClient.hpp"
#include "Serialize.hpp"
#include "IAmpHandler.hpp"

namespace iviLink
{
   namespace AppMan
   {
      namespace Ipc
      {
         Logger AmpClient::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("AppMan.Ipc.Pmp.Client"));

         AmpClient::AmpClient()
            : mHandler(NULL)
            , mIpc(new iviLink::Ipc::CIpc(iviLink::Ipc::Address("AppManPmp"), *this))
            , mId(0)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            assert (mIpc);
         }

         AmpClient::~AmpClient()
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            delete mIpc;
         }

         bool AmpClient::connect()
         {
             LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

             BaseError err = mIpc->connect();
             return err.isNoError();
         }

         void AmpClient::disconnect()
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

            mIpc->disconnect();
         }

         void AmpClient::init(IAmpHandler * pHandler)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

            mHandler = pHandler;
         }

         void AmpClient::OnConnection(iviLink::Ipc::DirectionID dirId)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

            if (mHandler)
            {
                mHandler->onAmpConnection();
            }
         }

         void AmpClient::OnConnectionLost(iviLink::Ipc::DirectionID dirId)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

            if (mHandler)
            {
               mHandler->onAmpConnectionLost();
            }
         }

         void AmpClient::OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload,
               UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize,
               iviLink::Ipc::DirectionID dirId)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            if (mHandler)
            {
               UInt32 size = 0;
               memcpy(&size,pPayload,sizeof(size));
               UInt32 pos = sizeof(size);
               UInt32 type = 0;
               memcpy(&type,pPayload+pos,sizeof(type));
               pos += sizeof(type);
               mHandler->onAmpAppRequestError(iviLink::Service::SessionUid(bufferToString(pPayload+pos,false)));
               memcpy(pResponseBuffer,&type,sizeof(type));
               bufferSize = sizeof(type);
            }
            else
            {
                LOG4CPLUS_ERROR(msLogger, "No handler");
            }
         }

         void AmpClient::OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload,
                 UInt32 payloadSize, iviLink::Ipc::DirectionID dirId)
         {
             LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

             if (mHandler)
             {
                 UInt32 count;
                 memcpy(&count,pPayload,sizeof(count));
                 UInt32 pos = sizeof(count);

                 std::list<Service::Uid> services;

                 for (int i=0; i<count; ++i)
                 {
                     services.push_back(Service::Uid(bufferToString(pPayload+pos,false)));
                 }
                 mHandler->onWaitingServicesRequest(services);
             }
             else
             {
                 LOG4CPLUS_ERROR(msLogger, "No handler");
             }
         }

         bool AmpClient::applicationRequest(const iviLink::Service::SessionUid &session,
                 const iviLink::Service::Uid & service)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

            UInt32 type = 0;
            UInt32 size = sizeof(type) + sizeof(size);
            size += stringInBufSize(session.value());
            size += stringInBufSize(service.value());
            UInt8 * buf = new UInt8[size];
            memcpy(buf,&size,sizeof(size));
            UInt32 pos = sizeof(size);
            memcpy(buf+pos,&type,sizeof(type));
            pos += sizeof(type);
            stringToBuffer(session.value(),buf+pos,false);
            pos += stringInBufSize(session.value());
            stringToBuffer(service.value(),buf+pos,false);
            UInt8 * resp = new UInt8[1];
            UInt32 rSize;
            BaseError err = mIpc->request(genId(),buf,size,resp,rSize);
            delete [] buf;
            delete [] resp;
            return err.isNoError();
         }

         bool AmpClient::waitingServiceLoadRequest(const iviLink::Service::Uid & service)
         {
             LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
             if (service.value() == "")
             {
                 LOG4CPLUS_INFO(msLogger, "No UID");
                 return false;
             }
             UInt32 size = stringInBufSize(service.value());
             UInt8 * buf = new UInt8[size];
             stringToBuffer(service.value(),buf,false);
             BaseError err = mIpc->asyncRequest(genId(),buf,size);
             delete [] buf;
             return err.isNoError();
         }

         iviLink::Ipc::MsgID AmpClient::genId()
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            return ++mId;
         }

      }
   }
}

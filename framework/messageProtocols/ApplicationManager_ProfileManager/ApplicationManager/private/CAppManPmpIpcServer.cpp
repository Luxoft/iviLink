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

#include "CAppManPmpIpcServer.hpp"
#include "Serialize.hpp"
 
namespace iviLink
{
   namespace AppMan
   {
      namespace Ipc
      {
         Logger CAppManPmpIpcServer::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("AppMan.Ipc.AmpForPmp.Server"));

         CAppManPmpIpcServer::CAppManPmpIpcServer()
            : mpRequest(0)
            , mpConnectionStatus(0)
            , mpIpc(new iviLink::Ipc::CIpc(iviLink::Ipc::Address("AppManPmp"), *this))
            , mId(0)
            , mBe(true)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         }

         CAppManPmpIpcServer::~CAppManPmpIpcServer()
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            delete mpIpc;
         }

         void CAppManPmpIpcServer::OnConnection(iviLink::Ipc::DirectionID dirId)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            mpRequest->onProfileManagerConnection();
         }

         void CAppManPmpIpcServer::OnConnectionLost(iviLink::Ipc::DirectionID dirId)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            mConLostSem.signal();
            if (mpConnectionStatus)
            {
               LOG4CPLUS_INFO(msLogger,"OnConnectionLost() - before mpConnectionStatus->onConnectionLost()");
               mpConnectionStatus->onConnectionLost();
               LOG4CPLUS_INFO(msLogger,"OnConnectionLost() - after mpConnectionStatus->onConnectionLost()");
            }
         }

         void CAppManPmpIpcServer::OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload,
               UInt32 payloadSize, UInt8* const pResponseBuffer,
               UInt32& bufferSize, iviLink::Ipc::DirectionID dirId)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            if (mpRequest)
            {
               UInt32 size = 0;
               memcpy(&size,pPayload,sizeof(size));
               UInt32 pos = sizeof(size);
               UInt32 type = 0;
               memcpy(&type,pPayload+pos,sizeof(type));
               pos += sizeof(type);
               iviLink::Service::SessionUid session = iviLink::Service::SessionUid(bufferToString(pPayload+pos,false));
               pos += stringInBufSize(session.value());
               iviLink::Service::Uid service = iviLink::Service::Uid(bufferToString(pPayload+pos,false));
               mpRequest->applicationRequest(session,service);
               LOG4CPLUS_INFO(msLogger, "after applicationRequest");
               memcpy(pResponseBuffer,&type,sizeof(type));
               bufferSize = sizeof(type);
            }
         }

         void CAppManPmpIpcServer::OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload,
               UInt32 payloadSize, iviLink::Ipc::DirectionID dirId)
         {
             if (mpRequest)
             {
                 Service::Uid service = Service::Uid(bufferToString(pPayload,false));
                 mpRequest->waitingServiceReqeust(service);
             }
         }

         void CAppManPmpIpcServer::appRequestError(iviLink::Service::SessionUid session)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            UInt32 type = 0;
            UInt32 size = sizeof(type) + sizeof(size);
            size += stringInBufSize(session.value());
            UInt8 * buf = new UInt8[size];
            memcpy(buf,&size,sizeof(size));
            UInt32 pos = sizeof(size);
            memcpy(buf+pos,&type,sizeof(type));
            pos += sizeof(type);
            stringToBuffer(session.value(),buf+pos,false);
            UInt8 * resp = new UInt8[1];
            UInt32 rSize;
            mpIpc->request(genId(),buf,size,resp,rSize);
            delete [] buf;
            delete [] resp;
         }

         void CAppManPmpIpcServer::appManConnectionLost()
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            //nothing to do
         }

         iviLink::Ipc::MsgID CAppManPmpIpcServer::genId()
         {
            return ++mId;
         }

         void CAppManPmpIpcServer::init(IPmpRequest * pRequest, IPmpConnectionStatus * pConnectionStatus)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            mpRequest = pRequest;
            mpConnectionStatus = pConnectionStatus;
         }

         void CAppManPmpIpcServer::loop()
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            BaseError err = BaseError::NoError("","");
            for (int i = 1;mBe ; ++i)
            {
               LOG4CPLUS_INFO(msLogger,"loop() : connect trying number : " + convertIntegerToString(i));
               err = mpIpc->waitForConnection();
               LOG4CPLUS_INFO(msLogger, "loop() : connect ended");
               if (err.isNoError() && mBe)
               {
                  mNoConnection = false;
                  mConLostSem.wait();
                  mNoConnection = true;
               }
               if (!mBe)
               {
                  LOG4CPLUS_INFO(msLogger,"loop() :: mBe == false");
                  break;
               }

               LOG4CPLUS_WARN(msLogger,"loop() :: connection failed");
               usleep (250000);
            }
         }

         void CAppManPmpIpcServer::disconnect()
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            mpIpc->disconnect();
         }

         bool CAppManPmpIpcServer::checkConnection() const
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            return !mNoConnection;
         }

         bool CAppManPmpIpcServer::waitingServicesRequest(const std::list<Service::Uid> & services)
         {
             LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

             UInt32 count = services.size();
             UInt32 size = sizeof(count);

             for (std::list<Service::Uid>::const_iterator it = services.begin();
                     services.end() != it; ++it)
             {
                size += stringInBufSize(it->value());
             }

             UInt8 * buffer = new UInt8[size];

             memcpy(buffer,&count,sizeof(count));
             UInt32 pos = sizeof(count);
             for (std::list<Service::Uid>::const_iterator it = services.begin();
                     services.end() != it; ++it)
             {
                 stringToBuffer(it->value(), buffer + pos,false);
                 pos += stringInBufSize(it->value());
             }

             BaseError err = mpIpc->asyncRequest(genId(),buffer,size);

             delete [] buffer;

             if (err.isNoError())
             {
                 return true;
             }
             else
             {
                 LOG4CPLUS_ERROR(msLogger, "Error: " + static_cast<std::string>(err));
                 return false;
             }
         }

      }

   }

}

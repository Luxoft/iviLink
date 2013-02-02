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


#include <unistd.h>
#include <tr1/array>

#include "CAppManProtoClient.hpp"
#include "Serialize.hpp"

namespace iviLink
{

   namespace AppMan
   {

      namespace Ipc
      {
         Logger CAppManProtoClient::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("AppMan.Ipc.App.Client"));

         CAppManProtoClient::CAppManProtoClient()
         : mpHandler(0)
         , mpIpc(new iviLink::Ipc::CIpc(iviLink::Ipc::Address("AppManApp"), *this))
         , mId(0)
         , mAppInited(false)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         }

         CAppManProtoClient::~CAppManProtoClient()
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            delete mpIpc;
         }

          void CAppManProtoClient::init(IAppManProtoAmpToApp * pHandler)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            mpHandler = pHandler;
         }

         void CAppManProtoClient::loop(CSignalSemaphore * pStartSemaphore)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            mBe = true;
            BaseError err = BaseError::NoError("","");
            for (int i = 1;mBe ; ++i)
            {
               LOG4CPLUS_INFO(msLogger, "loop() : connect trying number : " + convertIntegerToString(i));
               mpIpc->disconnect();
               err = mpIpc->connect();
               if (mBe && err.isNoError())
               {
                  LOG4CPLUS_INFO(msLogger, "loop() : connect established");
                  mNoConnection = false;
                  pStartSemaphore->signal();
                  mConLostSem.wait();
                  mNoConnection = true;
               }
               if (!mBe)
               {
                  LOG4CPLUS_INFO(msLogger, "loop() :: mBe == false");
                  break;
               }
               LOG4CPLUS_INFO(msLogger, "loop() :: connection failed");
               usleep(50000);
            }
         }

         void CAppManProtoClient::disconnect()
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            mBe = false;
            mpIpc->disconnect();
         }

         bool CAppManProtoClient::checkConnection() const
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            return !mNoConnection;
         }

         void CAppManProtoClient::OnConnection(iviLink::Ipc::DirectionID dirId)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         }

         void CAppManProtoClient::OnConnectionLost(iviLink::Ipc::DirectionID dirId)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            mConLostSem.signal();
         }

          void CAppManProtoClient::onLinkUpNotify()
          {
              LOG4CPLUS_TRACE(msLogger, __PRETTY_FUNCTION__);
              assert( mpHandler );
              if( mpHandler )
                  mpHandler -> onLinkUpNotify();
          }

          void CAppManProtoClient::onLinkDownNotify()
          {
              LOG4CPLUS_TRACE(msLogger, __PRETTY_FUNCTION__);
              assert( mpHandler );
              if( mpHandler )
                  mpHandler -> onLinkDownNotify();
          }


         void CAppManProtoClient::OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload,
               UInt32 payloadSize, UInt8* const pResponseBuffer,
               UInt32& bufferSize, iviLink::Ipc::DirectionID dirId)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            if (!mpHandler)
            {
               return;
            }

            UInt32 size = 0;
            memcpy(&size,pPayload,sizeof(size));
            LOG4CPLUS_INFO(msLogger, "payloadSize: " + convertIntegerToString(payloadSize)
                  + " size: " + convertIntegerToString(size));
            UInt32 pos = sizeof(size);
            UInt8 type = 0;
            memcpy(&type,pPayload+pos,sizeof(type));
            pos += sizeof(type);

            pid_t pid = 0;
            memcpy(&pid,pPayload+pos,sizeof(pid));
            pos += sizeof(pid);

            if (C_PROTO_SES_REQ == type)
            {
               Service::SessionUid session(bufferToString(pPayload+pos,false));
               pos += stringInBufSize(session.value());
               Service::Uid service(bufferToString(pPayload+pos,false));

               if (mAppInited)
               {
                  mpHandler->sessionRequest(pid,session,service);
               }
               else
               {
                  mSessionReqsList.push_back(tSessionRequestInfo(pid,session,service));
               }
               memcpy(pResponseBuffer,&type,sizeof(type));
               bufferSize = sizeof(type);
            }
            else if (C_PROTO_APP_LAN == type)
            {
               std::string lInfo;
               mpHandler->getAppLaunchInfo(pid,lInfo);
               stringToBuffer(lInfo,pResponseBuffer,false);
               bufferSize = stringInBufSize(lInfo);
            }
            else if (C_PROTO_CHK_CON == type)
            {
               memcpy(pResponseBuffer,&pid,sizeof(pid));
               bufferSize = sizeof(pid);
            }
         }


        void CAppManProtoClient::OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload,
                  UInt32 payloadSize,  iviLink::Ipc::DirectionID dirId)
        {    
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);        
            UInt32 size = 0;
            memcpy(&size,pPayload,sizeof(size));
            LOG4CPLUS_INFO(msLogger, "payloadSize: " + convertIntegerToString(payloadSize)
                  + " size: " + convertIntegerToString(size));
            UInt32 pos = sizeof(size);
            UInt8 type = 0;
            memcpy(&type,pPayload+pos,sizeof(type));
            pos += sizeof(type);
            if( type == C_PROTO_LINK_UP_NOTIFY )
            {
                onLinkUpNotify();
                return;
            } 
            else if( type == C_PROTO_LINK_DOWN_NOTIFY )
            {
                onLinkDownNotify();
                return;
            }
            else if (C_PROTO_WAIT_SERV_REQ == type)
            {
                Service::Uid service = Service::Uid(bufferToString(pPayload+pos,false));
                mpHandler->waitingServiceRequest(0,service);
            }
        }

         EInitResult CAppManProtoClient::initApplication(pid_t pid, std::list<Service::Uid> listOfSupportedServices)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            UInt8 type = C_PROTO_INIT_APP;
            UInt32 count = listOfSupportedServices.size();
            UInt32 size = sizeof(size) + sizeof(type) + sizeof(pid) + sizeof(count);
            for (std::list<Service::Uid>::iterator it = listOfSupportedServices.begin();
                  listOfSupportedServices.end() != it; ++it)
            {
               size += stringInBufSize(it->value());
            }
            UInt8 * buf = new UInt8[size];
            memcpy(buf,&size,sizeof(size));
            UInt32 pos = sizeof(size);
            memcpy(buf+pos,&type,sizeof(type));
            pos += sizeof(type);
            memcpy(buf+pos,&pid,sizeof(pid));
            pos += sizeof(pid);
            memcpy(buf+pos,&count,sizeof(count));
            pos += sizeof(count);

            for (std::list<Service::Uid>::iterator it = listOfSupportedServices.begin();
                  listOfSupportedServices.end() != it; ++it)
            {
               stringToBuffer(it->value(),buf+pos,false);
               pos += stringInBufSize(it->value());
            }

            EInitResult result;
            UInt32 rSize = sizeof(result);
            BaseError err = mpIpc->request(genId(),buf,size,reinterpret_cast<UInt8*>(&result),rSize);
            mAppInited = true;
            delete [] buf;
            if (err.isNoError())
            {
               return result;
            }
            else
            {
               return EInitResult(ERROR_IPC);
            }
         }

          bool CAppManProtoClient::isLinkAlive()
          {
              assert(mpIpc);
              if(!mpIpc)
                  return false;

              // Really sory about this piss of code, but I'm out of time and nervous
              static const UInt8 type = C_PROTO_IS_LINK_ALIVE;
              static const UInt32 sz = sizeof(sz) + sizeof(type);
              std::tr1::array<UInt8,sz> raw;
              memcpy(&raw[0],&sz,sizeof(sz));
              memcpy(&raw[sizeof(sz)],&type,sizeof(type));
              std::tr1::array<UInt8,sizeof(bool)> rsp;

              UInt32 rsz= rsp.size();

              const BaseError err = mpIpc->request( genId(),
                                                 &raw[0], raw.size(),
                                                 &rsp[0], rsz );
              if( !err.isNoError() )
              {
                  LOG4CPLUS_ERROR(msLogger, "CAppManProtoClient::is_link_available(): " + err.operator std::string());
                  return false;
              }
              return rsz == 1 && rsp[0] != 0;
          }

         BaseError CAppManProtoClient::useService(pid_t pid, Service::Uid service, bool use)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            UInt8 type = C_PROTO_USE_SERV;
            UInt32 size = sizeof(size) + sizeof(pid) + stringInBufSize(service.value()) + sizeof(use);

            UInt8 * buf = new UInt8[size];
            memcpy(buf,&size,sizeof(size));
            UInt32 pos = sizeof(size);
            memcpy(buf+pos,&type,sizeof(type));
            pos += sizeof(type);
            memcpy(buf+pos,&pid,sizeof(pid));
            pos += sizeof(pid);

            stringToBuffer(service.value(),buf+pos,false);
            pos += stringInBufSize(service.value());

            memcpy(buf+pos,&use,sizeof(use));

            UInt32 rSize = sizeof(type);
            UInt8 * resp = new UInt8[rSize];
            BaseError err = mpIpc->request(genId(),buf,size,resp,rSize);
            delete [] buf;
            delete [] resp;
            return err;
         }

         BaseError CAppManProtoClient::registerService(pid_t pid, Service::Uid service)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            UInt8 type = C_PROTO_REG_SERV;
            UInt32 size = sizeof(size) + sizeof(pid) + stringInBufSize(service.value());

            UInt8 * buf = new UInt8[size];
            memcpy(buf,&size,sizeof(size));
            UInt32 pos = sizeof(size);
            memcpy(buf+pos,&type,sizeof(type));
            pos += sizeof(type);
            memcpy(buf+pos,&pid,sizeof(pid));
            pos += sizeof(pid);

            stringToBuffer(service.value(),buf+pos,false);
            pos += stringInBufSize(service.value());

            LOG4CPLUS_INFO(msLogger, "registerService() : value: " + service.value());
            LOG4CPLUS_INFO(msLogger, "registerService() : size: " + convertIntegerToString(stringInBufSize(service.value())));

            UInt32 rSize = sizeof(type);
            UInt8 * resp = new UInt8[rSize];

            BaseError err = mpIpc->request(genId(),buf,size,resp,rSize);
            delete [] buf;
            delete [] resp;
            return err;
         }

         iviLink::Ipc::MsgID CAppManProtoClient::genId()
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            return ++mId;
         }

         void CAppManProtoClient::checkSessionRequest()
         {
            for (tSessionRequestList::iterator it = mSessionReqsList.begin();
                  mSessionReqsList.end() != it; ++it)
            {
               mpHandler->sessionRequest(it->mPid,it->mSession,it->mUid);
            }
            mSessionReqsList.clear();
         }

      }

   }

}

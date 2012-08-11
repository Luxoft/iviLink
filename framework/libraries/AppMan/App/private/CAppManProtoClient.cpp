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









#include <unistd.h>

#include "CAppManProtoClient.hpp"
#include "utils/serialize/Serialize.hpp"

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
         {
            LOG4CPLUS_TRACE(msLogger,"CAppManProtoClient()");
         }

         CAppManProtoClient::~CAppManProtoClient()
         {
            LOG4CPLUS_TRACE(msLogger,"~CAppManProtoClient()");
            delete mpIpc;
         }

         void CAppManProtoClient::init(IAppManProtoAmpToApp * pHandler)
         {
            LOG4CPLUS_TRACE(msLogger,"init()");
            mpHandler = pHandler;
         }

         void CAppManProtoClient::loop()
         {
            LOG4CPLUS_TRACE(msLogger,"loop()");
            mBe = true;
            CError err = CError::NoError("","");
            for (int i = 1;mBe ; ++i)
            {
               LOG4CPLUS_INFO(msLogger, "loop() : connect trying number : " + convertIntegerToString(i));
               err = mpIpc->connect();
               if (mBe && err.isNoError())
               {
                  LOG4CPLUS_INFO(msLogger, "loop() : connect established");
                  mNoConnection = false;
                  mConLostSem.wait();
                  mNoConnection = true;
               }
               if (!mBe)
               {
                  LOG4CPLUS_INFO(msLogger, "loop() :: mBe == false");
                  break;
               }
               LOG4CPLUS_INFO(msLogger, "loop() :: connection failed");
               usleep(250000);
            }
         }

         void CAppManProtoClient::disconnect()
         {
            LOG4CPLUS_TRACE(msLogger,"disconnect()");
            mBe = false;
            mpIpc->disconnect();
         }

         bool CAppManProtoClient::checkConnection() const
         {
            LOG4CPLUS_TRACE(msLogger,"checkConnection()");
            return !mNoConnection;
         }

         void CAppManProtoClient::OnConnection(iviLink::Ipc::DirectionID dirId)
         {
            LOG4CPLUS_TRACE(msLogger,"OnConnection()");
         }

         void CAppManProtoClient::OnConnectionLost(iviLink::Ipc::DirectionID dirId)
         {
            LOG4CPLUS_TRACE(msLogger,"OnConnectionLost()");
            mConLostSem.signal();
         }

         void CAppManProtoClient::OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload,
               UInt32 payloadSize, UInt8* const pResponseBuffer,
               UInt32& bufferSize, iviLink::Ipc::DirectionID dirId)
         {
            LOG4CPLUS_TRACE(msLogger,"OnRequest()");
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

               mpHandler->sessionRequest(pid,session,service);
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

         EInitResult CAppManProtoClient::initApplication(pid_t pid, std::list<Service::Uid> listOfSupportedServices)
         {
            LOG4CPLUS_TRACE(msLogger,"initApplication()");
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
            CError err = mpIpc->request(genId(),buf,size,reinterpret_cast<UInt8*>(&result),rSize);
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

         CError CAppManProtoClient::useService(pid_t pid, Service::Uid service, bool use)
         {
            LOG4CPLUS_TRACE(msLogger,"useService()");
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
            CError err = mpIpc->request(genId(),buf,size,resp,rSize);
            delete [] buf;
            delete [] resp;
            return err;
         }

         CError CAppManProtoClient::registerService(pid_t pid, Service::Uid service)
         {
            LOG4CPLUS_TRACE(msLogger,"registerService()");
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

            CError err = mpIpc->request(genId(),buf,size,resp,rSize);
            delete [] buf;
            delete [] resp;
            return err;
         }

         iviLink::Ipc::MsgID CAppManProtoClient::genId()
         {
            LOG4CPLUS_TRACE(msLogger,"genId()");
            return ++mId;
         }

      }

   }

}

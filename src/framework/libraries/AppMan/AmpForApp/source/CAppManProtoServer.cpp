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





#include "framework/libraries/AppMan/AmpForApp/include/CAppManProtoServer.hpp"
#include "framework/components/ProfileRepository/include/Serialize.hpp"

namespace AXIS
{

   namespace AppMan
   {

      namespace Ipc
      {
         Logger CAppManProtoServer::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("AppMan.Ipc.AmpForApp"));

         CAppManProtoServer::CAppManProtoServer()
            : mpServer(0)
            , mpConnectionHandler(0)
            , mpIpc(new AXIS::Ipc::CIpc(AXIS::Ipc::Address("AppManApp"), *this))
            , mId(0)
            , mpClientsMutex(new CMutex())
         {
            LOG4CPLUS_TRACE(msLogger,"CAppManProtoServer()");
         }

         CAppManProtoServer::~CAppManProtoServer()
         {
            mpServer = 0;
            LOG4CPLUS_TRACE(msLogger,"~CAppManProtoServer()");
            mpIpc->endWaitForConnection();
            delete mpIpc;
            delete mpClientsMutex;
         }

         void CAppManProtoServer::init(IAppManProto * pServer)
         {
            LOG4CPLUS_TRACE(msLogger,"init()");
            mpServer = pServer;
         }

         void CAppManProtoServer::initConnectionHandler(IConnectionHandler * pHandler)
         {
            LOG4CPLUS_TRACE(msLogger,"initConnectionHandler()");
            mpConnectionHandler = pHandler;
         }

         bool CAppManProtoServer::startWaitingForClients()
         {
            LOG4CPLUS_TRACE(msLogger,"startWaitingForClients()");
            CError err = mpIpc->beginWaitForConnection();
            if (err.isNoError())
            {
               return true;
            }
            else
            {
               LOG4CPLUS_INFO(msLogger, static_cast<std::string>(err));
               return false;
            }
         }

         void CAppManProtoServer::OnConnection(AXIS::Ipc::DirectionID dirId)
         {
            LOG4CPLUS_TRACE(msLogger,"OnConnection with application");
         }

         void CAppManProtoServer::OnConnectionLost(AXIS::Ipc::DirectionID dirId)
         {
            LOG4CPLUS_TRACE(msLogger,"OnConnectionLost()");

            mpClientsMutex->lock();
            for (std::map<pid_t, AXIS::Ipc::DirectionID>::iterator it = mClients.begin();
                  mClients.end() != it; ++it)
            {
               if (it->second == dirId)
               {
                  mpConnectionHandler->onConnectionLost(it->first);
                  mClients.erase(it);
                  break;
               }
            }
            mpClientsMutex->unlock();
         }

         void CAppManProtoServer::OnRequest(AXIS::Ipc::MsgID id, UInt8 const* pPayload,
               UInt32 payloadSize, UInt8* const pResponseBuffer,
               UInt32& bufferSize, AXIS::Ipc::DirectionID dirId)
         {
            LOG4CPLUS_TRACE(msLogger,"OnRequest()");

            if (!mpServer)
            {
               LOG4CPLUS_ERROR(msLogger, "! No pointer on server");
               return;
            }
            else
            {
               LOG4CPLUS_INFO(msLogger, "Has pointer on server");
            }
            UInt32 size = 0;
            memcpy(&size,pPayload,sizeof(size));
            UInt32 pos = sizeof(size);
            UInt8 type = 0;
            memcpy(&type,pPayload+pos,sizeof(type));
            pos += sizeof(type);
            pid_t pid = 0;
            memcpy(&pid,pPayload+pos,sizeof(pid));
            pos += sizeof(pid);

            LOG4CPLUS_INFO(msLogger, " before mpClientsMutex->lock();");
            mpClientsMutex->lock();
            LOG4CPLUS_INFO(msLogger, " after mpClientsMutex->lock();");
            std::map<pid_t, AXIS::Ipc::DirectionID>::iterator dit = mClients.find(pid);
            if (mClients.end() == dit)
            {
               mClients[pid] = dirId;
            }
            mpClientsMutex->unlock();

            if (C_PROTO_INIT_APP == type)
            {
               LOG4CPLUS_INFO(msLogger, "initApplication");
               UInt32 count = 0;
               memcpy(&count,pPayload+pos,sizeof(count));
               pos += sizeof(count);

               std::list<Service::Uid> list;
               LOG4CPLUS_INFO(msLogger,"size: " + convertIntegerToString(count));
               for (UInt32 i = 0; i < count; ++i)
               {
                  list.push_back(Service::Uid(bufferToString(pPayload+pos,false)));
                  pos += stringInBufSize(list.back().value());
               }

               EInitResult res = mpServer->initApplication(pid,list);
               bufferSize = sizeof(res);
               memcpy(pResponseBuffer,&res,bufferSize);
            }
            else if (C_PROTO_USE_SERV == type)
            {
               LOG4CPLUS_INFO(msLogger, "useService");
               Service::Uid service(bufferToString(pPayload+pos,false));
               pos += stringInBufSize(service.value());
               bool use = false;
               memcpy(&use,pPayload+pos,sizeof(use));

               mpServer->useService(pid,service,use);
               memcpy(pResponseBuffer,&type,sizeof(type));
               bufferSize = sizeof(type);
            }
            else if (C_PROTO_REG_SERV == type)
            {
               LOG4CPLUS_INFO(msLogger, "registerService");
               Service::Uid service(bufferToString(pPayload+pos,false));
               UInt32 strSize = stringInBufSize(service.value());
               LOG4CPLUS_INFO(msLogger,"strSize: " + convertIntegerToString(strSize));
               pos += strSize;

               mpServer->registerService(pid,service);
               memcpy(pResponseBuffer,&type,sizeof(type));
               bufferSize = sizeof(type);
            }
         }

         CError CAppManProtoServer::sessionRequest(pid_t pid, AXIS::Service::SessionUid session,
                           AXIS::Service::Uid service)
         {
            LOG4CPLUS_TRACE(msLogger,"sessionRequest()");
            mpClientsMutex->lock();
            std::map<pid_t, AXIS::Ipc::DirectionID>::iterator dit = mClients.find(pid);
            if (mClients.end() == dit)
            {
               mpClientsMutex->unlock();
               return CError(1,"AppMan");
            }
            AXIS::Ipc::DirectionID dirId = dit->second;
            mpClientsMutex->unlock();

            UInt8 type = C_PROTO_SES_REQ;
            UInt32 size = sizeof(size) + sizeof(pid) + sizeof(type) + UInt32(stringInBufSize(session.value())) + UInt32(stringInBufSize(service.value()));

            UInt8 * buf = new UInt8[size];
            memcpy(buf,&size,sizeof(size));
            UInt32 pos = sizeof(size);
            memcpy(buf+pos,&type,sizeof(type));
            pos += sizeof(type);
            memcpy(buf+pos,&pid,sizeof(pid));
            pos += sizeof(pid);

            stringToBuffer(session.value(),buf+pos,false);
            pos += stringInBufSize(session.value());
            stringToBuffer(service.value(),buf+pos,false);

            UInt32 rSize = sizeof(type);
            UInt8 * resp = new UInt8[rSize];
            LOG4CPLUS_INFO(msLogger, "request size: " + convertIntegerToString(size));
            CError err = mpIpc->request(genId(),buf,size,resp,rSize,&dirId);
            delete [] buf;
            delete [] resp;
            return err;
         }

         CError CAppManProtoServer::getAppLaunchInfo(pid_t pid, std::string & launchInfo)
         {
            LOG4CPLUS_TRACE(msLogger,"getAppLaunchInfo()");
            mpClientsMutex->lock();
            std::map<pid_t, AXIS::Ipc::DirectionID>::iterator dit = mClients.find(pid);
            if (mClients.end() == dit)
            {
               mpClientsMutex->unlock();
               return CError(1,"AppMan");
            }
            AXIS::Ipc::DirectionID dirId = dit->second;
            mpClientsMutex->unlock();

            UInt8 type = C_PROTO_APP_LAN;
            UInt32 size = sizeof(size) + sizeof(type) + sizeof(pid);

            UInt8 * buf = new UInt8[size];
            memcpy(buf,&size,sizeof(size));
            UInt32 pos = sizeof(size);
            memcpy(buf+pos,&type,sizeof(type));
            pos += sizeof(type);
            memcpy(buf+pos,&pid,sizeof(pid));


            UInt32 rSize = 4000;
            UInt8 * resp = new UInt8[rSize];
            CError err = mpIpc->request(genId(),buf,size,resp,rSize,&dirId);
            if (!rSize)
            {
               launchInfo = "";
            }
            else
            {
               launchInfo = bufferToString(resp,false);
            }
            delete [] buf;
            delete [] resp;
            return err;
         }

         bool CAppManProtoServer::checkConnection(pid_t pid)
         {
            LOG4CPLUS_TRACE(msLogger,"checkConnection()");
            mpClientsMutex->lock();
            std::map<pid_t, AXIS::Ipc::DirectionID>::iterator it = mClients.find(pid);
            if (mClients.end() == it)
            {
               mpClientsMutex->unlock();
               return false;
            }
            AXIS::Ipc::DirectionID dirId = it->second;
            mpClientsMutex->unlock();

            UInt8 type = C_PROTO_CHK_CON;
            UInt32 size = sizeof(pid);

            UInt8 * buf = new UInt8[size];
            memcpy(buf,&pid,sizeof(pid));

            UInt32 rSize = size;
            UInt8 * resp = new UInt8[rSize];
            CError err = mpIpc->request(genId(),buf,size,resp,rSize,&dirId);

            bool res = false;
            if (rSize == size && err.isNoError())
            {
               res = true;
            }

            delete [] buf;
            delete [] resp;
            return res;
         }

         AXIS::Ipc::MsgID CAppManProtoServer::genId()
         {
            return ++mId;
         }

      }

   }

}

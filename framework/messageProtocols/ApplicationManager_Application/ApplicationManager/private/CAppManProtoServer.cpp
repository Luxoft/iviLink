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


#include <tr1/array>
#include <cassert>
#include <vector>

#include "IConnectionHandler.hpp"
#include "CAppManProtoServer.hpp"
#include "Serialize.hpp"
 
namespace iviLink
{

   namespace AppMan
   {

      namespace Ipc
      {
          using iviLink::Ipc::DirectionID;

         Logger CAppManProtoServer::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("AppMan.Ipc.AmpForApp"));

         CAppManProtoServer::CAppManProtoServer()
            : mpServer(0)
            , mpConnectionHandler(0)
            , mpIpc(new iviLink::Ipc::CIpc(iviLink::Ipc::Address("AppManApp"), *this))
            , mId(0)
            , mpClientsMutex(new CMutex())
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         }

         CAppManProtoServer::~CAppManProtoServer()
         {
            mpServer = 0;
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            mpIpc->endWaitForConnection();
            delete mpIpc;
            delete mpClientsMutex;
         }

         void CAppManProtoServer::init(IAppManProto * pServer)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            mpServer = pServer;
         }

         void CAppManProtoServer::initConnectionHandler(IConnectionHandler * pHandler)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            mpConnectionHandler = pHandler;
         }

         bool CAppManProtoServer::startWaitingForClients()
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            BaseError err = mpIpc->beginWaitForConnection();
            if (err.isNoError())
            {
               return true;
            }
            else
            {
               LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
               return false;
            }
         }

         void CAppManProtoServer::OnConnection(iviLink::Ipc::DirectionID dirId)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         }

         void CAppManProtoServer::OnConnectionLost(iviLink::Ipc::DirectionID dirId)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

            mpClientsMutex->lock();
            for (std::map<pid_t, iviLink::Ipc::DirectionID>::iterator it = mClients.begin();
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

         void CAppManProtoServer::OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload,
               UInt32 payloadSize, UInt8* const pResponseBuffer,
               UInt32& bufferSize, iviLink::Ipc::DirectionID dirId)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

            if (!mpServer)
            {
               LOG4CPLUS_ERROR(msLogger, "! No pointer on server");
               return;
            }
            UInt32 size = 0;
            memcpy(&size,pPayload,sizeof(size));
            UInt32 pos = sizeof(size);
            UInt8 type = 0;
            memcpy(&type,pPayload+pos,sizeof(type));
            pos += sizeof(type);

            // Process link connectivity request
            if( type == C_PROTO_IS_LINK_ALIVE )
            {
                const bool alive = isLinkAlive();
                memcpy(pResponseBuffer, &alive, bufferSize = sizeof(bool));
                LOG4CPLUS_INFO(msLogger, std::string("C_PROTO_IS_LINK_ALIVE return ") + (alive ? "true" : "false" ) );
                return;
            }

            pid_t pid = 0;
            memcpy(&pid,pPayload+pos,sizeof(pid));
            pos += sizeof(pid);
            LOG4CPLUS_INFO(msLogger, "PID : " + convertIntegerToString(pid));

            LOG4CPLUS_INFO(msLogger, " before mpClientsMutex->lock();");
            mpClientsMutex->lock();
            LOG4CPLUS_INFO(msLogger, " after mpClientsMutex->lock();");
            std::map<pid_t, iviLink::Ipc::DirectionID>::iterator dit = mClients.find(pid);
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

         BaseError CAppManProtoServer::sessionRequest(pid_t pid,
                 const iviLink::Service::SessionUid &session, const iviLink::Service::Uid & service)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            mpClientsMutex->lock();
            std::map<pid_t, iviLink::Ipc::DirectionID>::iterator dit = mClients.find(pid);
            if (mClients.end() == dit)
            {
               mpClientsMutex->unlock();
               return BaseError(1,"AppMan");
            }
            iviLink::Ipc::DirectionID dirId = dit->second;
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
            BaseError err = mpIpc->request(genId(),buf,size,resp,rSize,&dirId);
            delete [] buf;
            delete [] resp;
            return err;
         }

         BaseError CAppManProtoServer::waitingServiceRequest(pid_t pid,
                 const iviLink::Service::Uid & service)
         {
             LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
             mpClientsMutex->lock();
             std::map<pid_t, iviLink::Ipc::DirectionID>::iterator dit = mClients.find(pid);
             if (mClients.end() == dit)
             {
                mpClientsMutex->unlock();
                return BaseError(1,"AppMan");
             }
             iviLink::Ipc::DirectionID dirId = dit->second;
             mpClientsMutex->unlock();

             UInt8 type = C_PROTO_WAIT_SERV_REQ;
             UInt32 size = sizeof(size) + sizeof(type) + UInt32(stringInBufSize(service.value()));

             UInt8 * buf = new UInt8[size];

             memcpy(buf,&size,sizeof(size));
             UInt32 pos = sizeof(size);
             memcpy(buf+pos,&type,sizeof(type));
             pos += sizeof(type);

             stringToBuffer(service.value(),buf+pos,false);

             BaseError err = mpIpc->asyncRequest(genId(),buf,size,&dirId);
             delete [] buf;
             return err;
         }

         BaseError CAppManProtoServer::getAppLaunchInfo(pid_t pid, std::string & launchInfo)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            mpClientsMutex->lock();
            std::map<pid_t, iviLink::Ipc::DirectionID>::iterator dit = mClients.find(pid);
            if (mClients.end() == dit)
            {
               mpClientsMutex->unlock();
               return BaseError(1,"AppMan");
            }
            iviLink::Ipc::DirectionID dirId = dit->second;
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
            BaseError err = mpIpc->request(genId(),buf,size,resp,rSize,&dirId);
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

         iviLink::Ipc::MsgID CAppManProtoServer::genId()
         {
            return ++mId;
         }

          void CAppManProtoServer::onLinkUpNotify()
          {
              notify_all( C_PROTO_LINK_UP_NOTIFY );
          }

          void CAppManProtoServer::onLinkDownNotify()
          {
              notify_all( C_PROTO_LINK_DOWN_NOTIFY );
          }

          void CAppManProtoServer::notify( iviLink::Ipc::DirectionID id, UInt8 notification )
          {
              LOG4CPLUS_TRACE(msLogger, __PRETTY_FUNCTION__);
              typedef UInt32 sz_t;
              typedef UInt8  msg_t;

              static const UInt32 sz= sizeof(sz_t)+sizeof(msg_t);
              std::tr1::array<UInt8,sizeof(sz_t)+sizeof(msg_t)> raw;
              memcpy(&raw[0], &sz, sizeof(sz)); // sorry
              raw[sizeof(sz)] = notification;

              assert( mpIpc );
              if( mpIpc )
              {
                  UInt32 rsz= 0;
                  const BaseError err = mpIpc->request(genId(),
                                                    &raw[0], raw.size(),
                                                    &raw[0], rsz,  // No response
                                                    &id);
                  if( !err.isNoError() )
                      LOG4CPLUS_ERROR( msLogger, "CAppManProtoServer failed to send notification: " +
                                       err.operator std::string() );
              }
          }

          void CAppManProtoServer::notify_all( UInt8 notification )
          {
              typedef std::vector<DirectionID> clients_list_t;
              clients_list_t clients;
              {
                  MutexLocker lock(*mpClientsMutex);
                  clients.reserve( mClients.size() );
                  for(clients_map_t::const_iterator e= mClients.end(), i= mClients.begin(); i!=e; ++i)
                  {
                      clients.push_back( i->second );
                  }
              }

              typedef UInt32 sz_t;
              typedef UInt8  msg_t;

              static const UInt32 sz= sizeof(sz_t)+sizeof(msg_t);
              std::tr1::array<UInt8,sizeof(sz_t)+sizeof(msg_t)> raw;
              memcpy(&raw[0], &sz, sizeof(sz));
              raw[sizeof(sz)] = notification;
              for( clients_list_t::const_iterator e= clients.end(), i= clients.begin(); i!=e; ++i )
              {

                assert( mpIpc );
                if( mpIpc )
                {
                    const BaseError err = mpIpc->asyncRequest(genId(),
                                                      &raw[0], raw.size(),
                                                      &(*i));
                    if( !err.isNoError() )
                      LOG4CPLUS_ERROR( msLogger, "CAppManProtoServer failed to send notification: " +
                                       err.operator std::string() );
                }
                else
                {
                      LOG4CPLUS_ERROR( msLogger, "CAppManProtoServer no IPC!");
                }
              }
          }

          bool CAppManProtoServer::isLinkAlive()
          {
              return link_state_getter ? link_state_getter() : false;
          }

          void CAppManProtoServer::set_link_state_getter( std::tr1::function<bool ()> getter )
          {
              link_state_getter = getter;
          }

      }

   }

}

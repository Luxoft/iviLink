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

/***************************************************************************
 * Project           AXIS
 * (c) copyright     2012
 * Company           LUXOFT
 * @file             CAppManPmpIpcClient.hpp
 * @brief            Source file for AppMan-PMP server
 * @author           VPlachkov
 ***************************************************************************/

#include <cassert>
#include <unistd.h>

#include "framework/libraries/AppMan/AmpForPmp/include/CAppManPmpIpcServer.hpp"
#include "framework/components/ProfileRepository/include/Serialize.hpp"

namespace AXIS
{
   namespace AppMan
   {
      namespace Ipc
      {
         Logger CAppManPmpIpcServer::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("AppMan.Ipc.AmpForPmp.Server"));

         CAppManPmpIpcServer::CAppManPmpIpcServer()
            : mpRequest(0)
            , mpConnectionStatus(0)
            , mpIpc(new AXIS::Ipc::CIpc(AXIS::Ipc::Address("AppManPmp"), *this))
            , mId(0)
            , mBe(true)
         {
            LOG4CPLUS_TRACE(msLogger,"CAppManPmpIpcServer()");
         }

         CAppManPmpIpcServer::~CAppManPmpIpcServer()
         {
            LOG4CPLUS_TRACE(msLogger,"~CAppManPmpIpcServer()");
            delete mpIpc;
         }

         void CAppManPmpIpcServer::OnConnection(AXIS::Ipc::DirectionID dirId)
         {

         }

         void CAppManPmpIpcServer::OnConnectionLost(AXIS::Ipc::DirectionID dirId)
         {
            LOG4CPLUS_TRACE(msLogger,"OnConnectionLost()");
            mConLostSem.signal();
            if (mpConnectionStatus)
            {
               LOG4CPLUS_INFO(msLogger,"OnConnectionLost() - 1");
               mpConnectionStatus->onConnectionLost();
               LOG4CPLUS_INFO(msLogger,"OnConnectionLost() - 2");
            }
            LOG4CPLUS_TRACE(msLogger,"OnConnectionLost() - END");
         }

         void CAppManPmpIpcServer::OnRequest(AXIS::Ipc::MsgID id, UInt8 const* pPayload,
               UInt32 payloadSize, UInt8* const pResponseBuffer,
               UInt32& bufferSize, AXIS::Ipc::DirectionID dirId)
         {
            LOG4CPLUS_TRACE(msLogger,"OnRequest()");
            if (mpRequest)
            {
               UInt32 size = 0;
               memcpy(&size,pPayload,sizeof(size));
               UInt32 pos = sizeof(size);
               UInt32 type = 0;
               memcpy(&type,pPayload+pos,sizeof(type));
               pos += sizeof(type);
               AXIS::Service::SessionUid session = AXIS::Service::SessionUid(bufferToString(pPayload+pos,false));
               pos += stringInBufSize(session.value());
               AXIS::Service::Uid service = AXIS::Service::Uid(bufferToString(pPayload+pos,false));
               mpRequest->applicationRequest(session,service);
               LOG4CPLUS_INFO(msLogger, "after applicationRequest");
               memcpy(pResponseBuffer,&type,sizeof(type));
               bufferSize = sizeof(type);
            }
         }

         void CAppManPmpIpcServer::appRequestError(AXIS::Service::SessionUid session)
         {
            LOG4CPLUS_TRACE(msLogger,"appRequestError()");
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
            LOG4CPLUS_TRACE(msLogger,"appManConnectionLost()");
            //nothing to do
         }

         AXIS::Ipc::MsgID CAppManPmpIpcServer::genId()
         {
            return ++mId;
         }

         void CAppManPmpIpcServer::init(IPmpRequest * pRequest, IPmpConnectionStatus * pConnectionStatus)
         {
            LOG4CPLUS_TRACE(msLogger,"init()");
            mpRequest = pRequest;
            mpConnectionStatus = pConnectionStatus;
         }

         void CAppManPmpIpcServer::loop()
         {
            LOG4CPLUS_TRACE(msLogger,"loop()");
            CError err = CError::NoError("","");
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

               LOG4CPLUS_INFO(msLogger,"loop() :: connection failed");
               usleep (250000);
            }
         }

         void CAppManPmpIpcServer::disconnect()
         {
            LOG4CPLUS_TRACE(msLogger,"disconnect()");
            mpIpc->disconnect();
         }

         bool CAppManPmpIpcServer::checkConnection() const
         {
            LOG4CPLUS_TRACE(msLogger, "checkConnection()");
            return !mNoConnection;
         }

      }

   }

}

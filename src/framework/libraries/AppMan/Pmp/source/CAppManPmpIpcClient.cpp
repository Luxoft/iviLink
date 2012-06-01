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
 * @file             CAppManPmpIpcClient.cpp
 * @brief            Source file for AppMan PMP-client
 * @author           VPlachkov
 ***************************************************************************/

#include <cassert>
#include <unistd.h>

#include "framework/libraries/AppMan/Pmp/include/CAppManPmpIpcClient.hpp"
#include "framework/components/ProfileRepository/include/Serialize.hpp"

namespace AXIS
{
   namespace AppMan
   {
      namespace Ipc
      {
         Logger CAppManPmpIpcClient::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("AppMan.Ipc.Pmp.Client"));

         CAppManPmpIpcClient::CAppManPmpIpcClient()
            : mpHandler(0)
            , mpIpc(new AXIS::Ipc::CIpc(AXIS::Ipc::Address("AppManPmp"), *this))
            , mId(0)
         {
            LOG4CPLUS_TRACE(msLogger, "CAppManPmpIpcClient()");
         }

         CAppManPmpIpcClient::~CAppManPmpIpcClient()
         {
            LOG4CPLUS_TRACE(msLogger, "~CAppManPmpIpcClient()");
            delete mpIpc;
         }

         void CAppManPmpIpcClient::init(IPmpHandler * pHandler)
         {
            LOG4CPLUS_TRACE(msLogger, "init()");
            mpHandler = pHandler;
         }

         void CAppManPmpIpcClient::OnConnection(AXIS::Ipc::DirectionID dirId)
         {
            LOG4CPLUS_TRACE(msLogger, "OnConnection()");
         }

         void CAppManPmpIpcClient::OnConnectionLost(AXIS::Ipc::DirectionID dirId)
         {
            LOG4CPLUS_TRACE(msLogger, "OnConnectionLost()");
            mConLostSem.signal();
            if (mpHandler)
            {
               mpHandler->appManConnectionLost();
            }
         }

         void CAppManPmpIpcClient::OnRequest(AXIS::Ipc::MsgID id, UInt8 const* pPayload,
               UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize,
               AXIS::Ipc::DirectionID dirId)
         {
            LOG4CPLUS_TRACE(msLogger, "OnRequest()");
            if (mpHandler)
            {
               UInt32 size = 0;
               memcpy(&size,pPayload,sizeof(size));
               UInt32 pos = sizeof(size);
               UInt32 type = 0;
               memcpy(&type,pPayload+pos,sizeof(type));
               pos += sizeof(type);
               mpHandler->appRequestError(AXIS::Service::SessionUid(bufferToString(pPayload+pos,false)));
               memcpy(pResponseBuffer,&type,sizeof(type));
               bufferSize = sizeof(type);
            }
         }

         CError CAppManPmpIpcClient::applicationRequest(AXIS::Service::SessionUid session, AXIS::Service::Uid service)
         {
            LOG4CPLUS_TRACE(msLogger, "applicationRequest()");
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
            CError err = mpIpc->request(genId(),buf,size,resp,rSize);
            delete [] buf;
            delete [] resp;
            return err;
         }

         AXIS::Ipc::MsgID CAppManPmpIpcClient::genId()
         {
            LOG4CPLUS_TRACE(msLogger, "genId()");
            return ++mId;
         }

         void CAppManPmpIpcClient::loop()
         {
            LOG4CPLUS_TRACE(msLogger, "loop()");
            mBe = true;
            CError err = CError::NoError("","");
            for (int i = 1;mBe ; ++i)
            {
               LOG4CPLUS_INFO(msLogger, "loop() : connect trying number : " + convertIntegerToString(i));
               err = mpIpc->connect();
               LOG4CPLUS_INFO(msLogger, "loop() : connect ended");
               if (mBe && err.isNoError())
               {
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

         void CAppManPmpIpcClient::disconnect()
         {
            LOG4CPLUS_TRACE(msLogger, "disconnect()");
            mBe = false;
            mpIpc->disconnect();
         }

         bool CAppManPmpIpcClient::checkConnection() const
         {
            LOG4CPLUS_TRACE(msLogger, "checkConnection()");
            return !mNoConnection;
         }

      }
   }
}

/* 
 * 
 * iviLINK SDK, version 1.1.2
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
#include <iostream>
#include <string>
#include <sstream>
#include <string>
#include <unistd.h>

#include "utils/ipc/CIpc.hpp"
#include "utils/ipc/ICallbackHandler.hpp"
#include "utils/threads/CMutex.hpp"
#include "utils/misc/Types.hpp"

#include "framework/components/ChannelSupervisor/common/Messages/AllocateChannelRequest.hpp"
#include "framework/components/ChannelSupervisor/common/Messages/DeallocateChannelRequest.hpp"
#include "framework/components/ChannelSupervisor/common/Messages/AllocateChannelResponse.hpp"
#include "framework/components/ChannelSupervisor/common/Messages/DeallocateChannelResponse.hpp"
#include "framework/components/ChannelSupervisor/common/Messages/CAAllocDoneRequest.hpp"
#include "framework/components/ChannelSupervisor/common/Messages/CAAllocDoneResponse.hpp"
#include "framework/components/ChannelSupervisor/common/CSError.hpp"

#include "utils/xml/pugixml.hpp"

#include "NegotiatorClient.hpp"

using namespace iviLink;
using namespace ChannelSupervisor;
using namespace Messages;


#define MAX_TRY_COUNT   50

const char defErrMsg[] = "no error occurred";
using iviLink::Ipc::DirectionID;
using iviLink::Ipc::CIpc;
using iviLink::Ipc::ICallbackHandler;
using iviLink::Ipc::CIpcError;
using iviLink::Ipc::MsgID;

char const * gpNEGOTIATOR_IPC_ADDR = NULL;

const char ipcNegotiatorId[] = "ipc-negotiator";

void ParseResponse_( pugi::xml_document* doc, char* responseStr)
{
   pugi::xml_parse_result res;
   res = doc->load((const char *)responseStr);
}

CError noError()
{
   return CError::NoError("comm_test_cli");
}

char const* errStr(CError const& err)
{
   static std::string s;
   s = (std::string) err;
   return s.c_str();
}

struct iviLink::ChannelSupervisor::NegotiatorIPCClient: public ICallbackHandler
{
   CIpc* mpIpc;
   CSignalSemaphore connLostSem;
   CSignalSemaphore reqSem;
   iviLink::Ipc::MsgID id;

   NegotiatorIPCClient() :
      mpIpc(NULL),
      id(1)
   {
      char const* addr = NULL;
      if (gpNEGOTIATOR_IPC_ADDR != NULL)
      {
         addr = gpNEGOTIATOR_IPC_ADDR;
      }
      else
      {
         addr = ipcNegotiatorId;
      }

      mpIpc = new CIpc(addr, *this);
   }

   virtual void OnConnection(DirectionID dirId)
   {

   }
   virtual ~NegotiatorIPCClient()
   {
      delete mpIpc;
   }

   virtual void OnConnectionLost(DirectionID dirId)
   {
      connLostSem.signal();
   }

   virtual void OnRequest(MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
         UInt8* const pResponseBuffer, UInt32& bufferSize,DirectionID dirId)
   {
      bufferSize = 0;
   }

   void sendMessage(UInt8 const* pPayload, UInt32 payloadSize,
         UInt8* const pResponseBuffer, UInt32& bufferSize)
   {
      bool con_res = connect();
      assert(con_res);

      CError err = mpIpc->request(id++, pPayload, payloadSize, pResponseBuffer,
            bufferSize);
      if (err.isNoError())
      {
      }
      else
      {
      }
      mpIpc->disconnect();
   }

   bool connect()
   {
      CError err = CError::NoError();
      for (int tryNum = 1; tryNum <= MAX_TRY_COUNT; ++tryNum)
      {
         err = mpIpc->connect();
         if (err.isNoError())
            return true;

         sleep(2);
      }
      return false;
   }
};

CMutex NegotiatorClient::m_SingletonMutex;
NegotiatorClient* NegotiatorClient::m_NegotiatorInstance = 0;

Logger NegotiatorClient::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("ChannelSupervisor.NegotiatorClient"));
NegotiatorClient* NegotiatorClient::getInstance()
{
   m_SingletonMutex.lock();
   if (m_NegotiatorInstance == NULL)
   {
      m_NegotiatorInstance = new NegotiatorClient();
   }
   m_SingletonMutex.unlock();
   return m_NegotiatorInstance;
}

void NegotiatorClient::deleteInstance()
{
   m_SingletonMutex.lock();
   delete this;
   m_NegotiatorInstance = NULL;
   m_SingletonMutex.unlock();
}

CError NegotiatorClient::NegotiateChannel(std::string tag, UInt32 & channelId)
{
   LOG4CPLUS_TRACE(msLogger, "NegotiatorClient::NegotiateChannel()=>"+tag+" "+convertIntegerToString(channelId));

   AllocateChannelRequest * req = NULL;
   AllocateChannelResponse * res = NULL;

   CError err = CSError(CSError::ERROR_OTHER);

   //prepare message for channel allocation
   std::stringstream messageToBeSent;
   req = new AllocateChannelRequest(tag.c_str());
   req->Write(messageToBeSent);

   UInt32 bufSize = 1000;
   UInt8 buf[bufSize];
   memset(buf,0, bufSize);

   m_ipcClient->sendMessage((const UInt8 *) (messageToBeSent.str().c_str()),
         messageToBeSent.str().size(), buf, bufSize);

   if(buf[0] == 0)
   {
      LOG4CPLUS_TRACE(msLogger,"NegotiatorClient::NegotiateChannel()=> ERROR!! No data received from Negotiator processs");
      err = CSError(CSError::IPC_NO_DATA_ERROR, "no data received from negotiator ipc server");
   }
   else
   {
      //receive response
      LOG4CPLUS_TRACE(msLogger,"NegotiatorClient::NegotiateChannel()=> Received \n==========\n" 
                        + std::string((char*)buf)+ "\n===========\n");
      pugi::xml_document* doc = new pugi::xml_document();
      ParseResponse_(doc, (char*) (buf));
      res = new AllocateChannelResponse(doc);

      //get the error from the response
      ErrorCode errc;
      const char * errstr;
      res->GetError(errc, errstr);

      LOG4CPLUS_TRACE(msLogger,"NegotiatorClient::NegotiateChannel()=> Error code" + (std::string)errstr);
      if (errc == ERRORCODE_SUCCESS)
      {
         channelId = res->GetChannelId();
         LOG4CPLUS_TRACE(msLogger,"NegotiatorClient::NegotiateChannel()=> channedl id = "+ convertIntegerToString(channelId));

         err = CSError::NoCSError(defErrMsg);
      }
      else if (errc == ERRORCODE_TIMEOUT_OCCURRED)
      {
         err = CSError(CSError::NEGOTIATION_CHANNEL_TIMEOUT, "negotiation timeout");
      }
      else if (errc == ERRORCODE_FREE_CID_NOT_FOUND_IN_MAP)
      {
         err = CSError(CSError::NO_FREE_CID_IN_MAP, "no free cid");
      }
      else
      {
         err = CSError(CSError::NEGOTIATION_CHANNEL_ERROR, "negotiation channel error");
      }
   }

   if (res)
      delete res;
   res = 0;

   if (req)
      delete req;
   req = 0;

   return err;
}

CError NegotiatorClient::UpdateMapWithCID(std::string tag, UInt32 & channelId)
{
  LOG4CPLUS_TRACE(msLogger,"NegotiatorClient::UpdateMapWithCID()=> tag ="
                     + tag+"channel id ="+convertIntegerToString(channelId));

   CAAllocateDoneRequest * req = NULL;
   CAAllocateDoneResponse * res = NULL;

   CError err = CSError(CSError::ERROR_OTHER);

   //prepare message for channel allocation
   std::stringstream messageToBeSent;
   req = new CAAllocateDoneRequest(tag.c_str(), channelId);
   req->Write(messageToBeSent);
   LOG4CPLUS_TRACE(msLogger,"NegotiatorClient::UpdateMapWithCID()=>Send \n" 
                     + messageToBeSent.str() + "\n");

   UInt32 bufSize = 1000;
   UInt8 buf[bufSize];
   memset(buf,0, bufSize);
   m_ipcClient->sendMessage((const UInt8 *) (messageToBeSent.str().c_str()),
         messageToBeSent.str().size(), buf, bufSize);

   if(buf[0] == 0)
   {
      LOG4CPLUS_TRACE(msLogger,"NegotiatorClient::UpdateMapWithCID()=> ERROR!! No data received from Negotiator processs");
      err = CSError(CSError::IPC_NO_DATA_ERROR, "no data received from negotiator ipc server");
   }
   else
   {
      //receive response
     LOG4CPLUS_TRACE(msLogger,"NegotiatorClient::UpdateMapWithCID()=> Received \n==========\n" 
                       +(std::string)((char*)buf) + "\n===========\n");

      pugi::xml_document* doc = new pugi::xml_document();
      ParseResponse_(doc, (char*) (buf));
      res = new CAAllocateDoneResponse(doc);

      //get the error from the response
      ErrorCode errc;
      const char * errstr;
      res->GetError(errc, errstr);
      LOG4CPLUS_TRACE(msLogger,"NegotiatorClient::UpdateMapWithCID()=> Error code =" + (std::string)errstr);

      if (errc == ERRORCODE_SUCCESS)
      {
         channelId = res->GetChannelId();
         LOG4CPLUS_TRACE(msLogger,"Negotiated channel id"+convertIntegerToString(channelId));
         err = CSError::NoCSError(defErrMsg);
      }
      else if (errc == ERRORCODE_TIMEOUT_OCCURRED)
      {
         LOG4CPLUS_TRACE(msLogger,"Timeout occurred. Err string: " +(std::string)errstr);
         err = CSError(CSError::UPDATE_MAP_TIMEOUT, "update map timeout");
      }
      else if (errc == ERRORCODE_WRONG_CHANNELID)
      {
         LOG4CPLUS_TRACE(msLogger,"Wrong channel id. Err string: " +(std::string)errstr);
         err = CSError(CSError::UPDATE_MAP_WRONG_CID, "update map timeout");
      }
      else
      {
         LOG4CPLUS_TRACE(msLogger,"Error occurred. Err string: " +(std::string)errstr);
         err = CSError(CSError::UPDATE_MAP_ERROR, "error during map update");
      }
   }

   if (res)
      delete res;
   res = 0;

   if (req)
      delete req;
   req = 0;

   return err;
}

CError NegotiatorClient::FreeChannel(const UInt32 channelId,
      std::string tag)
{
   LOG4CPLUS_TRACE(msLogger,"NegotiatorClient::FreeChannel()");   
   DeallocateChannelRequest * req = NULL;
   DeallocateChannelResponse * res = NULL;

   CError err = CSError(CSError::ERROR_OTHER);

   //prepare message for channel deallocation
   std::stringstream messageToBeSent;
   req = new DeallocateChannelRequest(tag.c_str(), channelId);
   req->Write(messageToBeSent);
   LOG4CPLUS_TRACE(msLogger,"NegotiatorClient::FreeChannel()=>Send \n"+messageToBeSent.str()+"\n");

   //send data (blocking call, wait for response)
   UInt32 bufSize = 2048;
   UInt8 buf[bufSize];
   memset(buf,0, bufSize);
   m_ipcClient->sendMessage((const UInt8 *) (messageToBeSent.str().c_str()),
         messageToBeSent.str().size(), buf, bufSize);


   if (buf[0] == 0)
   {
      LOG4CPLUS_TRACE(msLogger,"NegotiatorClient::FreeChannel()=> ERROR!! No data received from Negotiator processs");
      err = CSError(CSError::IPC_NO_DATA_ERROR,
            "no data received from negotiator ipc server");
   }
   else
   {
      //receive response
      LOG4CPLUS_TRACE(msLogger,"NegotiatorClient::FreeChannel()=> Received \n==========\n"
                        +(std::string)((char*)buf) + "\n===========\n");
      pugi::xml_document* doc = new pugi::xml_document();
      ParseResponse_(doc, (char*) (buf));
      res = new DeallocateChannelResponse(doc);

      //get the error from the response
      ErrorCode errc;
      const char * errstr;
      res->GetError(errc, errstr);
      LOG4CPLUS_TRACE(msLogger,"err code"+convertIntegerToString((int)errc));

      if (errc == ERRORCODE_SUCCESS)
      {
         UInt32 cId = res->GetChannelId();
         LOG4CPLUS_TRACE(msLogger,"deallocated channel id"+convertIntegerToString(cId));
         if(cId != channelId)
            err = CSError(CSError::DEALLOCATION_CHANNEL_MAP_WRONG_CID, "wrong cid");
         else
            err = CSError::NoCSError(defErrMsg);
      }
      else if (errc == ERRORCODE_TIMEOUT_OCCURRED)
      {
         LOG4CPLUS_TRACE(msLogger,"Timeout occurred. Err string: " +(std::string)errstr);
         err = CSError(CSError::DEALLOCATION_CHANNEL_MAP_TIMEOUT, "deallocate map timeout");
      }
      else
      {
         LOG4CPLUS_TRACE(msLogger,"Error occurred. Err string: " +(std::string)errstr);
         err = CSError(CSError::DEALLOCATION_CHANNEL_MAP_ERROR, "error during map deallocation");
      }
   }

   if (res)
      delete res;
   res = 0;

   if (req)
      delete req;
   req = 0;

   return err;
}

NegotiatorClient::NegotiatorClient()
{
   m_ipcClient = new NegotiatorIPCClient();
}

NegotiatorClient::~NegotiatorClient()
{
   if (m_ipcClient)
      delete m_ipcClient;
   m_ipcClient = 0;
}

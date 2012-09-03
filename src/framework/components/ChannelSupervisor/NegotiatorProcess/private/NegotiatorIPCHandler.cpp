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











#include "utils/xml/pugixml.hpp"
#include "framework/components/ChannelSupervisor/common/Messages/Notification.hpp"
#include "framework/components/ChannelSupervisor/common/CSError.hpp"

//local messages
#include "framework/components/ChannelSupervisor/common/Messages/AllocateChannelRequest.hpp"
#include "framework/components/ChannelSupervisor/common/Messages/DeallocateChannelRequest.hpp"
#include "framework/components/ChannelSupervisor/common/Messages/AllocateChannelResponse.hpp"
#include "framework/components/ChannelSupervisor/common/Messages/DeallocateChannelResponse.hpp"
#include "framework/components/ChannelSupervisor/common/Messages/AllocateRequest.hpp"
#include "framework/components/ChannelSupervisor/common/Messages/DeallocateRequest.hpp"
#include "framework/components/ChannelSupervisor/common/Messages/AllocateResponse.hpp"
#include "framework/components/ChannelSupervisor/common/Messages/DeallocateResponse.hpp"
#include "framework/components/ChannelSupervisor/common/Messages/CAAllocDoneRequest.hpp"
#include "framework/components/ChannelSupervisor/common/Messages/CAAllocDoneResponse.hpp"
#include "framework/components/ChannelSupervisor/common/Messages/AllocateNotification.hpp"
#include "framework/components/ChannelSupervisor/common/Messages/MapCheckNotification.hpp"
#include "framework/components/ChannelSupervisor/common/Messages/UpdateMapNotification.hpp"
#include "framework/components/ChannelSupervisor/common/Messages/DeallocateNotification.hpp"
#include "framework/components/ChannelSupervisor/common/Messages/CAAllocationNotification.hpp"

#include "framework/components/ChannelSupervisor/common/Messages/CommandRejectResponse.hpp"

#include "framework/components/ChannelSupervisor/Tube/ChannelSupervisorTube.hpp"

#include "NegotiatorIPCHandler.hpp"
#include "NegotiatorStates.hpp"
#include "NegotiatorTube.hpp"

using namespace iviLink;
using namespace ChannelSupervisor;
using namespace Messages;

const int MAX_TRY_COUNT = 50;
const char defErrMsg[] = "no error occurred";
Logger NegotiatorIPCHandler::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("NegotiatorProcess.NegotiatorIPCHandler"));

NegotiatorIPCHandler::NegotiatorIPCHandler(NegotiatorTube * tube, NegotiaterStates * states, std::string const& sock_addr) :
   m_id(1),
   m_negotiatorTube(tube),
   m_negotiatorStates(states)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   char const* addr = ipcNegotiatorId;
   if (!sock_addr.empty())
      addr = sock_addr.c_str();

   m_ipc = new CIpc(addr, *this);
   m_map = Map::getInstance();
   m_state = IDLE;
}

NegotiatorIPCHandler::~NegotiatorIPCHandler()
{
   if (m_ipc)
   {
      delete m_ipc;
   }
   m_ipc = 0;
   if (m_negotiatorTube)
   {
      delete m_negotiatorTube;
   }
   m_negotiatorTube = NULL;
   if(m_negotiatorStates)
   {
      delete m_negotiatorStates;
   }
   m_negotiatorStates = NULL;
}

void NegotiatorIPCHandler::OnRequest(MsgID id, UInt8 const* pPayload, UInt32 payloadSize, UInt8* const pResponseBuffer,
      UInt32& bufferSize, DirectionID dirId)
{
   std::string response;
   ProcessIPCClientMessage(std::string((const Int8*) pPayload).substr(0, payloadSize), response);

   //put the response to the buffer
   bufferSize = response.size();
   memcpy(pResponseBuffer, response.c_str(), bufferSize);
}

void NegotiatorIPCHandler::OnConnectionLost(DirectionID dirId)
{
   m_connLostSem.signal();
}

bool NegotiatorIPCHandler::IPCConnect()
{
   CError err = CError::NoError();
   m_ipc->beginWaitForConnection();
   return false;
}

void NegotiatorIPCHandler::IPCWait()
{
   m_reqSem.wait();
}

void NegotiatorIPCHandler::dismissSema()
{
   LOG4CPLUS_TRACE(msLogger, "NegotiatorIPCHandler::dismissSema()");
   m_reqSem.signal();
}

void NegotiatorIPCHandler::IPCDisconnect()
{
   m_ipc->endWaitForConnection();
   m_ipc->disconnect();
   m_connLostSem.wait();
}

void NegotiatorIPCHandler::ProcessIPCClientMessage(std::string message, std::string &response)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, "NegotiatorIPCHandler::ProcessIPCClientMessage()==>START=====");
   Message * msg = NULL;
   CError err = CSError(CSError::ERROR_OTHER);

   LOG4CPLUS_TRACE(msLogger, "NegotiatorIPCHandler::ProcessIPCClientMessage() message = \n" + message);

   pugi::xml_document* doc = new pugi::xml_document();
   ParseResponse(doc, (Int8*) message.c_str());

   if (m_state == IDLE) //process allocate and deallocate
   {
      
      LOG4CPLUS_TRACE(msLogger, "NegotiatorIPCHandler::ProcessIPCClientMessage()==>IDLE=====");
      if (Notification::NOTIFICATIONTTYPE_ALLOCATE == Notification::GetNotificationType(doc))
      {
         LOG4CPLUS_TRACE(msLogger, "NegotiatorIPCHandler::ProcessIPCClientMessage()==>ALLOCATE=====");
         AllocateChannelRequest * request = new AllocateChannelRequest(doc);
         std::string tag = request->GetTag();
         UInt32 channel = 0;
         err = NegotiateChannel(tag, channel);
         LOG4CPLUS_TRACE(msLogger, "NegotiatorIPCHandler::ProcessIPCClientMessage()==>IDLE=====" + convertIntegerToString(channel));
         msg = new AllocateChannelResponse(tag.c_str(), channel);

         if (!err.isNoError())
         {
            if (err.getCode() == CSError::UPDATE_MAP_WRONG_CID)
               ((AllocateChannelResponse*) msg)->SetError(ERRORCODE_WRONG_CHANNELID, "wrong cid");
            else if (err.getCode() == CSError::UPDATE_MAP_TIMEOUT)
               ((AllocateChannelResponse*) msg)->SetError(ERRORCODE_TIMEOUT_OCCURRED, "timeout");
            else
               ((AllocateChannelResponse*) msg)->SetError(ERRORCODE_OTHER, "other error");
         }

         if (request)
         {
            delete request;
         }
         request = 0;

         m_state = NEGOTIATED;
      }
      else if (Notification::NOTIFICATIONTTYPE_DEALLOCATE == Notification::GetNotificationType(doc))
      {
         LOG4CPLUS_TRACE(msLogger, "NegotiatorIPCHandler::ProcessIPCClientMessage()==>DEALLOCATE=====");
         DeallocateChannelRequest * request = new DeallocateChannelRequest(doc);
         std::string tag = request->GetTag();
         UInt32 channel = request->GetChannelId();
         err = ChannelDeallocated(tag, channel);
         msg = new DeallocateChannelResponse(tag.c_str(), channel);

         if (!err.isNoError())
         {
            if (err.getCode() == CSError::DEALLOCATION_CHANNEL_MAP_WRONG_CID)
               ((DeallocateChannelResponse*) msg)->SetError(ERRORCODE_WRONG_CHANNELID, "wrong cid");
            else if (err.getCode() == CSError::DEALLOCATION_CHANNEL_MAP_TIMEOUT)
               ((DeallocateChannelResponse*) msg)->SetError(ERRORCODE_TIMEOUT_OCCURRED, "timeout");
            else
               ((DeallocateChannelResponse*) msg)->SetError(ERRORCODE_OTHER, "other error");
         }

         if (request)
            delete request;
         request = 0;

         m_state = IDLE;
      }
      else
      {
         msg = new CommandRejectResponse("");
      }
   }
   else if (m_state == NEGOTIATED) //process update map only
   {
      LOG4CPLUS_TRACE(msLogger, "NegotiatorIPCHandler::ProcessIPCClientMessage()==>CA_ALLOCATION_DONE=====");
      if (Request::REQUESTTYPE_CA_ALLOCATION_DONE == Request::GetRequestType(doc))
      {
         CAAllocateDoneRequest * request = new CAAllocateDoneRequest(doc);
         std::string tag = request->GetTag();
         UInt32 channel  = request->GetChannelId();
         err = UpdateMap(tag, channel);

         msg = new CAAllocateDoneResponse(tag.c_str(), channel);

         if (!err.isNoError())
         {
            if (err.getCode() == CSError::UPDATE_MAP_WRONG_CID)
               ((CAAllocateDoneResponse*) msg)->SetError(ERRORCODE_WRONG_CHANNELID, "wrong cid");
            else if (err.getCode() == CSError::UPDATE_MAP_TIMEOUT)
               ((CAAllocateDoneResponse*) msg)->SetError(ERRORCODE_TIMEOUT_OCCURRED, "timeout");
            else
               ((CAAllocateDoneResponse*) msg)->SetError(ERRORCODE_OTHER, "other error");
         }

         if (request)
         {
            delete request;
         }
         request = 0;

         m_state = IDLE;
      }
      else
      {
         LOG4CPLUS_TRACE(msLogger, "NegotiatorIPCHandler::ProcessIPCClientMessage()==>REJECT=====");
         msg = new CommandRejectResponse("");
      }
   }
   else //error here
   {
      LOG4CPLUS_TRACE(msLogger, "NegotiatorIPCHandler::ProcessIPCClientMessage()==>ERROR===== State = "+  convertIntegerToString((int)m_state));
      msg = new CommandRejectResponse("");
   }

   std::stringstream messageToBeSent;

   msg->Write(messageToBeSent);
   response = messageToBeSent.str();

   if (msg)
      delete msg;
   msg = NULL;
}

CError NegotiatorIPCHandler::NegotiateChannel(std::string tag, UInt32 & cid)
{
   LOG4CPLUS_TRACE(msLogger, "NegotiatorIPCHandler::NegotiateChannel()==>START");

   CError err = CSError(CSError::ERROR_OTHER);

   //get channel id from the map
   UInt32 channelId = m_map->getNextFreeChannelId();
   LOG4CPLUS_TRACE(msLogger, "NegotiatorIPCHandler::NegotiateChannel()==>channel"+  convertIntegerToString(channelId));

   Notification * noti = NULL;

   noti = new AllocateNotification(tag.c_str(), channelId);
   std::stringstream messageToBeSent;

   noti->Write(messageToBeSent);
   LOG4CPLUS_TRACE(msLogger, "NegotiatorIPCHandler::NegotiateChannel()=>Message: \n==========\n" + messageToBeSent.str()+"\n===========\n");
 
   m_negotiatorTube->sendDataTube(messageToBeSent.str().c_str());

   messageToBeSent.str(std::string());

   LOG4CPLUS_TRACE(msLogger, "NegotiatorIPCHandler::NegotiateChannel()==>WAIT FOR NEGOTIATED");
   m_negotiatorStates->mStateCond.lock();
   while(m_negotiatorStates->GetTagState(tag.c_str()) != NegotiaterStates::NEGOTIATED)
   {
      m_negotiatorStates->mStateCond.wait();
   }
   m_negotiatorStates->mStateCond.unlock();

   //here both messages received
   LOG4CPLUS_TRACE(msLogger, "NegotiatorIPCHandler::NegotiateChannel()==>NEGOTIATED"+  convertIntegerToString(channelId));

   //get remote notification
   AllocateNotification * remoteNotification = 
              (AllocateNotification *) m_negotiatorStates->GetNotification(tag.c_str());

   UInt32 remoteCid = 0;

   ErrorCode errc;
   const char * errstr;
   if(remoteNotification){
      remoteNotification->GetError(errc, errstr);
      LOG4CPLUS_TRACE(msLogger, "NegotiatorIPCHandler::NegotiateChannel()==>remote notification error"+(std::string)(errstr));

      if (errc == ERRORCODE_SUCCESS)
      {
         remoteCid = remoteNotification->GetChannelId();
         if (remoteCid >= channelId)   //use remote
         {
            if (!m_map->isChannelok(remoteCid))
            {
               if (noti)
                  delete noti;
               noti = 0;
               return err;
            }

            else
               channelId = remoteCid;
         }
      }
      else
      {
         if (noti)
            delete noti;
         noti = 0;

         return err;
      }
   } else {
      LOG4CPLUS_ERROR(msLogger, "(AllocateNotification *) m_negotiatorStates->GetNotification returned NULL!" );
   }

   if (noti)
      delete noti;
   noti = 0;
   //LOG4CPLUS_TRACE(msLogger, "NegotiatorIPCHandler::NegotiateChannel()=>Map Check...."+(std::string)(errstr)); - may cause crash

   //create map Notification message
   noti = new MapCheckNotification(tag.c_str(), channelId);

   //send the message
   noti->Write(messageToBeSent);
   LOG4CPLUS_TRACE(msLogger, "NegotiatorIPCHandler::NegotiateChannel()=>Message: \n==========\n" 
                              + messageToBeSent.str()+"\n===========\n");
   
   m_negotiatorTube->sendDataTube(messageToBeSent.str().c_str());

   LOG4CPLUS_TRACE(msLogger, "NegotiatorIPCHandler::NegotiateChannel()==>WAIT FOR CHECKEDINMAP");

   m_negotiatorStates->mStateCond.lock();
   while(m_negotiatorStates->GetTagState(tag.c_str()) != NegotiaterStates::CHECKEDINMAP)
   {
      m_negotiatorStates->mStateCond.wait();    
   }
   m_negotiatorStates->mStateCond.unlock();

   LOG4CPLUS_TRACE(msLogger, "NegotiatorIPCHandler::NegotiateChannel()==>CHECKEDINMAP" 
                        + convertIntegerToString(channelId));

   //get remote notification
   MapCheckNotification * mapRemoteNotification = 
               (MapCheckNotification *) m_negotiatorStates->GetNotification(tag.c_str());

   if(mapRemoteNotification) {
      mapRemoteNotification->GetError(errc, errstr);
      LOG4CPLUS_TRACE(msLogger, "NegotiatorIPCHandler::NegotiateChannel()==>remote Error"
                        + convertIntegerToString((int)errc)+(std::string)(errstr));
   
      if (errc == ERRORCODE_SUCCESS)
      {
         cid = channelId;
         err = CError(CError::NO_ERROR, "ChannelSupervisorProcess");
      }
      else
      {
         err = CSError(CSError::ERROR_OTHER);
      }
      if (noti)
         delete noti;
   } else {
      LOG4CPLUS_ERROR(msLogger, "(MapCheckNotification *) m_negotiatorStates->GetNotification returned NULL!" );
   }
   return err;
}

CError NegotiatorIPCHandler::ChannelDeallocated(std::string tag, UInt32 cid)
{
   CError err = CSError(CSError::ERROR_OTHER);

   {
      ChannelInfo info;
      if (!m_map->getChannelInfo(cid, info) || info.tag.empty())
         return CSError(CSError::DEALLOCATION_CHANNEL_MAP_WRONG_CID);
   }

   //store in the map
   m_map->invalidateChannelInfo(cid);

   // Notification * noti;

   // noti = new DeallocateNotification(tag.c_str(), cid);
   // std::stringstream messageToBeSent;

   // noti->Write(messageToBeSent);
   // LOG4CPLUS_TRACE(msLogger, "NegotiatorIPCHandler::ChannelDeallocated()=>Message: \n==========\n" 
   //                      + messageToBeSent.str()+"\n===========\n");

   // m_negotiatorTube->sendDataTube(messageToBeSent.str().c_str());


   // LOG4CPLUS_TRACE(msLogger, "NegotiatorIPCHandler::NegotiateChannel()==>WAIT FOR IDLE");
   // m_negotiatorStates->mStateCond.lock();
   // if(m_negotiatorStates->GetTagState(tag.c_str()) != NegotiaterStates::IDLE)
   // {
   //    LOG4CPLUS_TRACE(msLogger, "before m_negotiatorStates->mStateCond.wait();");
   //    m_negotiatorStates->mStateCond.wait();
   //    LOG4CPLUS_TRACE(msLogger, "after m_negotiatorStates->mStateCond.wait();");
   // }
   // m_negotiatorStates->mStateCond.unlock();

   // LOG4CPLUS_TRACE(msLogger, "NegotiatorIPCHandler::NegotiateChannel()==>IDLE");

   // //here both messages received

   // //get remote notification
   // DeallocateNotification * remoteNotification = 
   //             (DeallocateNotification *) m_negotiatorStates->GetNotification(tag.c_str());

   // ErrorCode errc;
   // const char * errstr;
   // remoteNotification->GetError(errc, errstr);

   // if (errc == ERRORCODE_SUCCESS)
   // {
   //    err = CSError::NoCSError("no error");
   //    LOG4CPLUS_TRACE(msLogger, "no error");
   // }
   // else
   // {
   //    LOG4CPLUS_TRACE(msLogger, "errc = " + convertIntegerToString((int)errc));
   // }
   // delete noti;

   return err;
}

CError NegotiatorIPCHandler::UpdateMap(std::string tag, UInt32 cid)
{
   CError err = CSError(CSError::ERROR_OTHER);

   //store in the map
   ChannelInfo chInfo;
   chInfo.tag = tag;

   chInfo.timeStamp = time(NULL);

   Notification * noti = new CAAllocationNotification(tag.c_str(), cid);
   std::stringstream messageToBeSent;

   noti->Write(messageToBeSent);
   LOG4CPLUS_TRACE(msLogger, "NegotiatorIPCHandler::UpdateMap()=>Message: \n==========\n" + messageToBeSent.str()+"\n===========\n");
   m_negotiatorTube->sendDataTube(messageToBeSent.str().c_str());

   LOG4CPLUS_TRACE(msLogger, "NegotiatorIPCHandler::NegotiateChannel()==>WAIT FOR CAALLOCATED");

   m_negotiatorStates->mStateCond.lock();
   while(m_negotiatorStates->GetTagState(tag.c_str()) != NegotiaterStates::CAALLOCATED)
   {
      m_negotiatorStates->mStateCond.wait();
   }
   m_negotiatorStates->mStateCond.unlock();

   LOG4CPLUS_TRACE(msLogger, "NegotiatorIPCHandler::NegotiateChannel()==>CAALLOCATED");
   
   delete noti;

   messageToBeSent.str(std::string());

   m_map->addChannelInfo(cid, chInfo);

   noti = new UpdateMapNotification(tag.c_str(), cid);
   noti->Write(messageToBeSent);

   LOG4CPLUS_TRACE(msLogger, "NegotiatorIPCHandler::UpdateMap()=>Message: \n==========\n" 
                        + messageToBeSent.str()+"\n===========\n");
   m_negotiatorTube->sendDataTube(messageToBeSent.str().c_str());

   LOG4CPLUS_TRACE(msLogger, "NegotiatorIPCHandler::UpdateMap()=> Current state is "+convertIntegerToString((int)m_negotiatorStates->GetTagState(tag.c_str())));
   m_negotiatorTube->sendDataTube(messageToBeSent.str().c_str());

   LOG4CPLUS_TRACE(msLogger, "NegotiatorIPCHandler::NegotiateChannel()==>WAIT FOR ALLOCDONE");

   m_negotiatorStates->mStateCond.lock();
   //if(m_negotiatorStates->GetTagState(tag.c_str()) != NegotiaterStates::ALLOCDONE)
   while(m_negotiatorStates->GetTagState(tag.c_str()) != NegotiaterStates::IDLE)
   {
      m_negotiatorStates->mStateCond.wait();
   }
   m_negotiatorStates->mStateCond.unlock();

   LOG4CPLUS_TRACE(msLogger, "NegotiatorIPCHandler::NegotiateChannel()==>ALLOCDONE / IDLE");

   //get remote notification
   UpdateMapNotification * remoteNotification = 
      (UpdateMapNotification *) m_negotiatorStates->GetNotification(tag.c_str());

   ErrorCode errc;
   const char * errstr;
   if(remoteNotification){
      remoteNotification->GetError(errc, errstr);

      if (errc == ERRORCODE_SUCCESS)
      {
         err = CSError::NoCSError("no error");
      }
      if (noti) delete noti;
   } else {
      LOG4CPLUS_ERROR(msLogger, "(UpdateMapNotification *) m_negotiatorStates->GetNotification returned NULL!");
   }
   return err;
}
void iviLink::ChannelSupervisor::ParseResponse(pugi::xml_document* doc, char* responseStr)
{
   pugi::xml_parse_result res;
   res = doc->load((const char *) responseStr);
}

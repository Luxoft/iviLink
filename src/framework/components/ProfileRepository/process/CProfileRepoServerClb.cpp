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





#include <unistd.h>

#include "utils/configurator/include/configurator.h"
#include "utils/misc/include/logging.hpp"
#include "framework/components/ProfileRepository/include/Serialize.hpp"

#include "CProfileRepoServerClb.hpp"

namespace AXIS
{

   namespace ProfileRepository
   {

      Logger CProfileRepoServerClb::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileRepository.Process.CProfileRepoServerClb"));

      CProfileRepoServerClb::CProfileRepoServerClb(conf::Configurator & config)
         : mpIpc(NULL)
         , mConfig(config)
      {
         std::string addr = config.getParam("ipc_addr");
         if (addr.empty())
         {
            addr = "AXISProfileRepository";
         }

         mpIpc = new Ipc::CIpc(addr, *this);
      }

      CProfileRepoServerClb::~CProfileRepoServerClb()
      {
         delete mpIpc;
         mpIpc = NULL;
      }

      void CProfileRepoServerClb::OnConnection(Ipc::DirectionID)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
      }

      void CProfileRepoServerClb::OnConnectionLost(Ipc::DirectionID)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mConLostSem.signal();
      }

      void CProfileRepoServerClb::OnRequest(Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize, Ipc::DirectionID)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         const RepoRequest * req = reinterpret_cast<const RepoRequest*>(pPayload);
         switch (req->type)
         {
         case REQ_ADD_PROFILE_API:
            addProfileAPI(req,payloadSize,pResponseBuffer,bufferSize);
            break;
         case REQ_REMOVE_PROFILE_API:
            removeProfileAPI(req,payloadSize,pResponseBuffer,bufferSize);
            break;
         case REQ_ADD_PROFILE:
            addProfile(req,payloadSize,pResponseBuffer,bufferSize);
            break;
         case REQ_REMOVE_PROFILE:
            removeProfile(req,payloadSize,pResponseBuffer,bufferSize);
            break;
         case REQ_ADD_PROFILE_IMPLEMENTATION:
            addProfileImplementation(req,payloadSize,pResponseBuffer,bufferSize);
            break;
         case REQ_REMOVE_PROFILE_IMPLEMENTATION_PL:
            removeProfileImplementationPl(req,payloadSize,pResponseBuffer,bufferSize);
            break;
         case REQ_FIND_PROFILES:
            findProfiles(req,payloadSize,pResponseBuffer,bufferSize);
            break;
         case REQ_GET_MANIFEST:
            getManifest(req,payloadSize,pResponseBuffer,bufferSize);
            break;
         case REQ_GET_PROFILE_LIST:
            getProfilesList(req,payloadSize,pResponseBuffer,bufferSize);
            break;
         }
      }

      int CProfileRepoServerClb::loop()
      {
         CError err = CError::NoError("ProfileRepository","loop");
         for (int i = 1; ; ++i)
         {
            LOG4CPLUS_INFO(msLogger, "CProfileRepoServerClb::loop() : connection try number: "+
                  convertIntegerToString(i));
            err = mpIpc->waitForConnection();
            LOG4CPLUS_INFO(msLogger, "CProfileRepoServerClb::loop() : waitForConnectionEnded()");
            if (err.isNoError())
            {
               LOG4CPLUS_INFO(msLogger,"CProfileRepoServerClb::loop() :: *** client connected to server ***");
               mConLostSem.wait();
            }
            else
            {
               LOG4CPLUS_INFO(msLogger, static_cast<std::string>(err));
            }
            LOG4CPLUS_INFO(msLogger, "failed connection");
            usleep (250000);
         }
         return 0;
      }

      void CProfileRepoServerClb::addProfileAPI(const RepoRequest * req, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         CError err = mApiDB.addProfileApi(bufferToString(req->data));
         UInt32 code = err.getCode();
         memcpy (pResponseBuffer, &code,4);
         bufferSize = 4;
      }

      void CProfileRepoServerClb::removeProfileAPI(const RepoRequest * req, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         UID uid(bufferToString(req->data));
         CError err = mApiDB.removeProfileApi(uid);
         UInt32 code = err.getCode();
         memcpy (pResponseBuffer, &code,4);
         bufferSize = 4;
      }

      void CProfileRepoServerClb::addProfile(const RepoRequest * req, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         CError err = mProfileDB.addProfile(bufferToString(req->data));
         UInt32 code = err.getCode();
         memcpy (pResponseBuffer, &code,4);
         bufferSize = 4;
      }

      void CProfileRepoServerClb::removeProfile(const RepoRequest * req, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         UID uid(bufferToString(req->data));
         CError err = mApiDB.removeProfileApi(uid);
         UInt32 code = err.getCode();
         memcpy (pResponseBuffer, &code,4);
         bufferSize = 4;
      }

      void CProfileRepoServerClb::addProfileImplementation(const RepoRequest * req, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         UID uid(bufferToString(req->data));
         int pos = stringInBufSize(uid.value);
         AXIS::LibDescriptor ld;
         ld.libPath = bufferToString(req->data + pos);
         pos += stringInBufSize(ld.libPath);
         ld.platform = bufferToString(req->data + pos);
         CError err = mProfileDB.addProfileImplementation(uid,ld);
         UInt32 code = err.getCode();
         memcpy (pResponseBuffer, &code, 4);
         bufferSize = 4;
      }

      void CProfileRepoServerClb::removeProfileImplementationPl(const RepoRequest * req, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         UID uid(bufferToString(req->data));
         int pos = stringInBufSize(uid.value);
         std::string platform = bufferToString(req->data + pos);
         CError err = mProfileDB.removeProfileImplementation(uid,platform);
         UInt32 code = err.getCode();
         memcpy (pResponseBuffer, &code, 4);
         bufferSize = 4;
      }

      void CProfileRepoServerClb::findProfiles(const RepoRequest * req, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         UID uid(bufferToString(req->data));
         int pos = stringInBufSize(uid.value);
         std::string platform = bufferToString(req->data + pos);
         pos += stringInBufSize(platform);
         UInt32 count;
         memcpy (&count,req->data+pos, 4);
         pos += 4;
         std::map <std::string, std::string> args;

         for (UInt32 i=0; i<count; ++i)
         {
            std::string key = bufferToString(req->data+pos);
            pos += stringInBufSize(key);
            std::string val = bufferToString(req->data+pos);
            pos += stringInBufSize(val);
            args[key] = val;
         }

         std::list<LibInfo> inf = mProfileDB.findProfiles(uid,args,platform);

         UInt32 rCount = inf.size();
         memcpy (pResponseBuffer,&rCount,sizeof(rCount));
         int outPos = sizeof(rCount);

         for (std::list<LibInfo>::const_iterator it = inf.begin(); inf.end() != it; ++it)
         {
            LOG4CPLUS_INFO(msLogger, "CProfileRepoServerClb::findProfiles() : FoundLib : " +
                  it->path);
            stringToBuffer(it->uid.value,pResponseBuffer + outPos);
            outPos += stringInBufSize(it->uid.value);
            stringToBuffer(it->platform,pResponseBuffer + outPos);
            outPos += stringInBufSize(it->platform);
            stringToBuffer(it->path,pResponseBuffer + outPos);
            outPos += stringInBufSize(it->path);
            memcpy (pResponseBuffer + outPos, &it->relevance,4);
            outPos += 4;
         }
         bufferSize = outPos;
      }

      void CProfileRepoServerClb::getManifest(const RepoRequest * req, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         UID uid(bufferToString(req->data));
         std::string strRes = mProfileDB.getManifest(uid);
         stringToBuffer(strRes,pResponseBuffer);
         bufferSize = stringInBufSize(strRes);
      }

      void CProfileRepoServerClb::getProfilesList(const RepoRequest * req, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         std::list<UID> list = mProfileDB.getProfilesList();
         UInt32 count = list.size();

         memcpy (pResponseBuffer, &count, 4);
         int pos = 4;
         for (std::list<UID>::const_iterator it = list.begin(); list.end() != it; ++it)
         {
            stringToBuffer(it->value, pResponseBuffer+pos);
            pos += stringInBufSize(it->value);
         }
         bufferSize = pos;
      }

   }

}

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











#include <cstring>
#include <cassert>
#include <unistd.h>

#include "CProfileRepoClient.hpp"
#include "utils/serialize/Serialize.hpp"
#include "utils/configurator/configurator.h"


namespace iviLink
{
   namespace ProfileRepository
   {

      Logger CProfileRepoClient::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileRepository.Lib.CProfileRepoClient"));

      CProfileRepoClient::CProfileRepoClient(conf::Configurator * pConfig)
         : mpIpc(NULL)
         , mGId(0)
         , mBe(true)
         , mNoConnection(true)
      {
         std::string addr;
         if (pConfig)
         {
            addr = pConfig->getParam("pmp_repo_ipc_address");
            if (addr.empty())
               addr = "AXISProfileRepository";
         }
         mpIpc = new Ipc::CIpc(addr, *this);
      }

      CProfileRepoClient::~CProfileRepoClient()
      {
         delete mpIpc;
         mpIpc = NULL;
      }

      void CProfileRepoClient::OnConnection(Ipc::DirectionID)
      {
         
      }

      void CProfileRepoClient::OnConnectionLost(Ipc::DirectionID)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mConLostSem.signal();
      }

      void CProfileRepoClient::OnRequest(Ipc::MsgID id, UInt8 const* pPayload,
            UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize,
            Ipc::DirectionID)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
      }

      CError CProfileRepoClient::addProfileApi(const std::string &manifestPath)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         UInt32 size = sizeof(RepoRequest) + stringInBufSize(manifestPath);
         UInt8 * pBuf = new UInt8[size];
         RepoRequest * req = reinterpret_cast<RepoRequest*>(pBuf);
         req->type = REQ_ADD_PROFILE_API;
         req->size = size;
         stringToBuffer(manifestPath,req->data);
         UInt32 respSize = 4;
         UInt8 * pResp = new UInt8[respSize];

         bool rm = makeRequest(req,pResp,respSize);

         UInt32 code = 0;
         if (respSize<4 || !rm)
         {
            code = CProfileRepoError::ERROR_IPC;
         }
         else
         {
            memcpy(&code,pResp,4);
         }

         delete []pResp;
         delete []pBuf;
         return errorByCode(code);
      }

      CError CProfileRepoClient::removeProfileApi(UID profileApiUid)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         UInt32 size = sizeof(RepoRequest) + stringInBufSize(profileApiUid.value);
         UInt8 * pBuf = new UInt8[size];
         RepoRequest * req = reinterpret_cast<RepoRequest*>(pBuf);
         req->type = REQ_REMOVE_PROFILE_API;
         req->size = size;
         stringToBuffer(profileApiUid.value,req->data);
         UInt32 respSize = 4;
         UInt8 * pResp = new UInt8[respSize];

         bool rm = makeRequest(req,pResp,respSize);

         UInt32 code = 0;
         if (respSize<4 || !rm)
         {
            code = CProfileRepoError::ERROR_IPC;
         }
         else
         {
            memcpy(&code,pResp,4);
         }

         delete []pResp;
         delete []pBuf;
         return errorByCode(code);
      }

      CError CProfileRepoClient::addProfile(const std::string &xmlManifestPath)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         UInt32 size = sizeof(RepoRequest) + stringInBufSize(xmlManifestPath);
         UInt8 * pBuf = new UInt8[size];
         RepoRequest * req = reinterpret_cast<RepoRequest*>(pBuf);
         req->type = REQ_ADD_PROFILE;
         req->size = size;
         stringToBuffer(xmlManifestPath,req->data);
         UInt32 respSize = 4;
         UInt8 * pResp = new UInt8[respSize];

         bool rm = makeRequest(req,pResp,respSize);

         UInt32 code = 0;
         if (respSize<4 || !rm)
         {
            code = CProfileRepoError::ERROR_IPC;
         }
         else
         {
            memcpy(&code,pResp,4);
         }

         delete []pResp;
         delete []pBuf;
         return errorByCode(code);
      }

      CError CProfileRepoClient::removeProfile(UID profileUid)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         UInt32 size = sizeof(RepoRequest) + stringInBufSize(profileUid.value);
         UInt8 * pBuf = new UInt8[size];
         RepoRequest * req = reinterpret_cast<RepoRequest*>(pBuf);
         req->type = REQ_REMOVE_PROFILE;
         req->size = size;
         stringToBuffer(profileUid.value,req->data);
         UInt32 respSize = 4;
         UInt8 * pResp = new UInt8[respSize];

         bool rm = makeRequest(req,pResp,respSize);

         UInt32 code = 0;
         if (respSize<4 || !rm)
         {
            code = CProfileRepoError::ERROR_IPC;
         }
         else
         {
            memcpy(&code,pResp,4);
         }

         delete []pResp;
         delete []pBuf;
         return errorByCode(code);
      }

      CError CProfileRepoClient::addProfileImplementation(UID profileID, const LibDescriptor& library)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         UInt32 sizeProfId = stringInBufSize(profileID.value);
         UInt32 sizeLibPath = stringInBufSize(library.libPath);
         UInt32 sizeLibPlat = stringInBufSize(library.platform);
         UInt32 size = sizeof(RepoRequest) + sizeProfId + sizeLibPath + sizeLibPlat;
         UInt8 * pBuf = new UInt8[size];
         RepoRequest * req = reinterpret_cast<RepoRequest*>(pBuf);
         req->type = REQ_ADD_PROFILE_IMPLEMENTATION;
         req->size = size;
         stringToBuffer(profileID.value,req->data);
         stringToBuffer(library.libPath,req->data+sizeProfId);
         stringToBuffer(library.platform,req->data+sizeProfId+sizeLibPath);

         UInt32 respSize = 4;
         UInt8 * pResp = new UInt8[respSize];

         bool rm = makeRequest(req,pResp,respSize);

         UInt32 code = 0;
         if (respSize<4 || !rm)
         {
            code = CProfileRepoError::ERROR_IPC;
         }
         else
         {
            memcpy(&code,pResp,4);
         }

         delete []pResp;
         delete []pBuf;
         return errorByCode(code);
      }

      CError CProfileRepoClient::removeProfileImplementation(UID profileUid, const std::string &platform)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         UInt32 sizeProfId = stringInBufSize(profileUid.value);
         UInt32 sizePlat = stringInBufSize(platform);
         UInt32 size = sizeof(RepoRequest) + sizeProfId + sizePlat;
         UInt8 * pBuf = new UInt8[size];
         RepoRequest * req = reinterpret_cast<RepoRequest*>(pBuf);
         req->type = REQ_REMOVE_PROFILE_IMPLEMENTATION_PL;
         req->size = size;
         stringToBuffer(profileUid.value,req->data);
         stringToBuffer(platform,req->data+sizePlat);

         UInt32 respSize = 4;
         UInt8 * pResp = new UInt8[respSize];

         bool rm = makeRequest(req,pResp,respSize);

         UInt32 code = 0;
         if (respSize<4 || !rm)
         {
            code = CProfileRepoError::ERROR_IPC;
         }
         else
         {
            memcpy(&code,pResp,4);
         }

         delete []pResp;
         delete []pBuf;
         return errorByCode(code);
      }

      CError CProfileRepoClient::findProfiles(UID id,
            const std::map<std::string, std::string> & profileArguments,
            std::list<LibInfo> &implementations, const std::string &platform)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         UInt32 size = sizeof(RepoRequest);
         UInt32 sizeId = stringInBufSize(id.value);
         size += sizeId;
         UInt32 sizePlat = stringInBufSize(platform);
         size += sizePlat;
         UInt32 count = profileArguments.size();
         size += 4; 
         for (std::map<std::string,std::string>::const_iterator it = profileArguments.begin();
               profileArguments.end() != it; ++it)
         {
            size += stringInBufSize(it->first);
            size += stringInBufSize(it->second);
         }
         
         UInt8 * pBuf = new UInt8[size];
         RepoRequest * req = reinterpret_cast<RepoRequest*>(pBuf);
         req->type = REQ_FIND_PROFILES;
         req->size = size;
         stringToBuffer(id.value,req->data);
         int pos = stringInBufSize(id.value);
         stringToBuffer(platform,req->data+pos);
         pos += stringInBufSize(platform);
         memcpy(req->data+pos,&count,4);
         pos += 4;
         for (std::map<std::string,std::string>::const_iterator it = profileArguments.begin();
               profileArguments.end() != it; ++it)
         {
            stringToBuffer(it->first,req->data+pos);
            pos += stringInBufSize(it->first);
            stringToBuffer(it->second,req->data+pos);
            pos += stringInBufSize(it->second);
         }

         UInt32 respSize = 4000;
         UInt8 * pResp = new UInt8[respSize];

         implementations.clear();
         bool rm = makeRequest(req,pResp,respSize);

         int code = 0;
         if (respSize < 4 || !rm)
         {
            code = CProfileRepoError::ERROR_IPC;
         }
         else
         {

            UInt32 rCount;
            memcpy(&rCount,pResp,4);
            pos = 4;
            for (UInt32 i = 0; i<rCount; ++i)
            {
               UID uid;
               uid.value = bufferToString(pResp + pos);
               pos += stringInBufSize(uid.value);
               std::string platf = bufferToString(pResp + pos);
               pos += stringInBufSize(platf);
               std::string path = bufferToString(pResp + pos);
               pos += stringInBufSize(path);
               UInt32 rel;
               memcpy (&rel,pResp+pos,4);
               pos += 4;
               implementations.push_back(LibInfo(uid,platf,path,rel));
            }
         }

         delete []pResp;
         delete []pBuf;

         return errorByCode(code);
      }

      std::string CProfileRepoClient::getManifest(UID uid)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         UInt32 size = sizeof(RepoRequest) + stringInBufSize(uid.value);
         UInt8 * pBuf = new UInt8[size];
         RepoRequest * req = reinterpret_cast<RepoRequest*>(pBuf);
         req->type = REQ_GET_MANIFEST;
         req->size = size;
         stringToBuffer(uid.value,req->data);

         UInt32 respSize = 4000;
         UInt8 * pResp = new UInt8[respSize];

         bool rm = makeRequest(req,pResp,respSize);

         std::string res;
         if (respSize && rm)
         {
            res = bufferToString(pResp);
         }

         delete []pResp;
         delete []pBuf;
         return res;
      }

      std::list<UID> CProfileRepoClient::getProfilesList()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         UInt32 size = sizeof(RepoRequest);
         UInt8 * pBuf = new UInt8[size];
         RepoRequest * req = reinterpret_cast<RepoRequest*>(pBuf);
         req->type = REQ_GET_PROFILE_LIST;
         req->size = size;

         UInt32 respSize = 4000;
         UInt8 * pResp = new UInt8[respSize];

         bool rm = makeRequest(req,pResp,respSize);

         std::list<UID> list;
         if (respSize >= 4 && rm)
         {
            UInt32 rCount;
            memcpy(&rCount,pResp,4);
            int pos = 4;
            for (UInt32 i=0; i<rCount; ++i)
            {
               list.push_back( UID(bufferToString(pResp+pos)) );
               pos += stringInBufSize(list.back().value);
            }
         }
         delete []pResp;
         delete []pBuf;
         return list;
      }

      bool CProfileRepoClient::makeRequest(const RepoRequest * req, UInt8* pResponseBuffer,
            UInt32& bufferSize)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         if (!checkConnection())
         {
            return false;
         }
         CError err = mpIpc->request(getId(), reinterpret_cast<UInt8 const*>(req), req->size, pResponseBuffer, bufferSize);
         return err.isNoError();
      }

      Ipc::MsgID CProfileRepoClient::getId()
      {
         return ++mGId;
      }

      void CProfileRepoClient::loop()
      {
         mBe = true;
         CError err = CProfileRepoError::NoProfileRepoError();
         for (int i = 1; mBe /*&& i <= 20*/; ++i)
         {
            LOG4CPLUS_INFO(msLogger, "CProfileRepoClient::loop() : connect trying number : " +
                  convertIntegerToString(i));

            err = mpIpc->connect();
            LOG4CPLUS_INFO(msLogger, "CProfileRepoClient::loop() : connect ended");
            if (mBe && err.isNoError())
            {
               mConnMutex.lock();
               {
                  mNoConnection = false;
               }
               mConnMutex.unlock();
               
               mConLostSem.wait();

               mConnMutex.lock();
               {
                  mNoConnection = true;
               }
               mConnMutex.unlock();
            }

            LOG4CPLUS_INFO(msLogger, "CProfileRepoClient::loop() :: connection failed");
            usleep(250000);
         }
      }

      CError CProfileRepoClient::errorByCode(UInt32 errCode)
      {
         if (errCode)
         {
            return CProfileRepoError(static_cast<CProfileRepoError::eCodes>(errCode));
         }
         else
         {
            return CProfileRepoError::NoProfileRepoError();
         }
      }

      void CProfileRepoClient::disconnect()
      {
         mBe = false;
         mpIpc->disconnect();
      }

      bool CProfileRepoClient::checkConnection() const
      {
         bool res = false;
         mConnMutex.lock();
         {
            res = !mNoConnection;
         }
         mConnMutex.unlock();
         return res;
      }

   }

}


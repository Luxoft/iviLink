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
#include <dlfcn.h>
#ifndef ANDROID
#else
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#endif //ANDROID

#include "utils/misc/CError.hpp"
#include "utils/misc/Logger.hpp"

#include "profileFactory.hpp"
#include "CProfileInternal.hpp"

using iviLink::Profile::CProfileInternal;

static const char gModuleName[] = "PIFactory";
#ifndef ANDROID
#else
//extern std::string mInternalPath;
#endif //ANDROID
static Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.Pmal.Pim.profileFactory"));

typedef std::map<iviLink::Profile::IUid, void*> tLibHandlesMap;
tLibHandlesMap gLibHandlesMap;


typedef iviLink::Profile::CProfileInternal* (*createProfileFunc)(iviLink::Profile::IUid, iviLink::Service::Uid const& sid, iviLink::Profile::IProfileCallbackProxy*);

namespace iviLink {
namespace PMAL {
namespace PIM {

#ifndef ANDROID
CError loadLib(char const* pPath, void*& pHandle)
#else
CError loadLib(char const* pPath, void*& pHandle, std::string backupPath)
#endif //ANDROID
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   LOG4CPLUS_DEBUG(logger, "profile lib path = '" + std::string(pPath) + "'");
   dlerror();
   assert(pPath != NULL);

   
   #ifndef ANDROID
   void* h = dlopen(pPath, RTLD_NOW);
   char* err = dlerror();
   if (h == NULL)
   {
      assert(err != NULL);
      return CError(ERROR_PROFILE_LIB_LOAD, gModuleName, CError::ERROR, err);
   }
   #else
   LOG4CPLUS_DEBUG(logger, "mInternalPath ="+backupPath);
   std::string stdpPath(pPath);
   std::string resultPath;
   int found=stdpPath.find_last_of("/\\");
   resultPath = backupPath +std::string("/")+ stdpPath.substr(found+1);
   if(! (access(resultPath.c_str(), F_OK) == 0 && access(resultPath.c_str(), R_OK) == 0)) {
      remove(resultPath.c_str()); //removing in case some lib is already there
      LOG4CPLUS_DEBUG(logger, "No file @ "+resultPath);
      LOG4CPLUS_DEBUG(logger, "Will proceed with copying");
      // caching profile *.so
      std::ifstream  src(pPath);
      std::ofstream  dst(resultPath.c_str());
      dst << src.rdbuf();
      dst.close();
      src.close();
   } else {
      LOG4CPLUS_DEBUG(logger, "Readable file exists there: "+resultPath);
   }
   //
   void* h = dlopen(resultPath.c_str(), RTLD_NOW);
   char* err = (char*)dlerror();
   if (h == NULL)
   {
      assert(err != NULL);
      return CError(ERROR_PROFILE_LIB_LOAD, gModuleName, CError::ERROR, err);
   }
   #endif //ANDROID

   pHandle = h;
   return CError::NoError(gModuleName);
}


CError findFunc(void* pLibHandle, void*& func)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   assert(pLibHandle);

   const char funcName[] = "createProfile";

   dlerror();
   void* f = dlsym(pLibHandle, funcName);

   #ifndef ANDROID
   char* err = dlerror();
   #else
   char* err = (char*)dlerror();
   #endif //ANDROID
   if (err != NULL)
   {
      return CError(ERROR_PROFILE_LIB_NO_CREATE_FUNC, gModuleName, CError::ERROR, err);
   }

   func = f;
   return CError::NoError(gModuleName);
}


CError getObject(void* pLibHandle, ProfileInitData const& initData, CProfileInternal*& pPIObject)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   assert(pLibHandle);
   void* func = NULL;
   CError err = findFunc(pLibHandle, func);
   if (!err.isNoError())
   {
      return err;
   }

   assert(func);

   // Here is warning on -pedantic level:
   // "ISO C++ forbids casting between pointer-to-function and pointer-to-object"
   // The only correct solution that I found is to obtain not the function 
   // pointer, but pointer to structure of function pointers.
   // I think, this warning will be not the problem on current target platforms,
   // but anyway.
   // See: http://www.trilithium.com/johan/2004/12/problem-with-dlsym/
   createProfileFunc createProfile = reinterpret_cast<createProfileFunc>(func);
   pPIObject = createProfile(initData.uid, initData.sid, initData.proxy);
   if (pPIObject == NULL)
   {
      return CError(ERROR_PROFILE_LIB_CREATE_PROFILE, gModuleName, CError::ERROR);
   }

   return CError::NoError(gModuleName);
}

#ifndef ANDROID
CError createProfileImpl(char const* pProfileLibPath, ProfileInitData const& initData, iviLink::Profile::CProfileInternal*& pProfile)
#else
CError createProfileImpl(char const* pProfileLibPath, ProfileInitData const& initData, iviLink::Profile::CProfileInternal*& pProfile, std::string backupPath)
#endif //ANDROID
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   void* handle = NULL;

   #ifndef ANDROID
   CError result = loadLib(pProfileLibPath, handle);
   #else
   CError result = loadLib(pProfileLibPath, handle, backupPath);
   #endif //ANDROID
   if (!result.isNoError())
   {
      LOG4CPLUS_ERROR(logger, static_cast<std::string>(result));
      return result;
   }

   assert(handle);

   gLibHandlesMap[initData.uid] = handle;

   CProfileInternal* prof = NULL;

   result = getObject(handle, initData, prof);
   if (!result.isNoError())
   {
      LOG4CPLUS_ERROR(logger, static_cast<std::string>(result));

      unloadProfileLib(initData.uid);

      return result;
   }

   assert(prof);
   pProfile = prof;

   return result;
}

void unloadProfileLib(iviLink::Profile::IUid const& piuid)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   tLibHandlesMap::iterator it = gLibHandlesMap.find(piuid);
   if (it != gLibHandlesMap.end())
   {
      void* handle = it->second;
      dlerror();
      if (dlclose(handle) != 0)
      {
         #ifndef ANDROID
         char* err = dlerror();
         #else
         char* err = (char*)dlerror();
         #endif //ANDROID
         CError e = CError(ERROR_PROFILE_LIB_UNLOAD, gModuleName, CError::ERROR, err);
         LOG4CPLUS_ERROR(logger, static_cast<std::string>(e));
      }
   }
}


}  // PIM
}  // ProfileManager
}  // AXIS


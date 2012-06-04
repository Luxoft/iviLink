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




#include <cassert>
#include <dlfcn.h>

#include "utils/misc/include/CError.hpp"
#include "utils/misc/include/Logger.hpp"

#include "../include/profileFactory.hpp"
#include "../include/CProfileInternal.hpp"

using AXIS::Profile::CProfileInternal;

static const char gModuleName[] = "PIFactory";
static Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.Pmal.Pim.profileFactory"));

typedef std::map<AXIS::Profile::IUid, void*> tLibHandlesMap;
tLibHandlesMap gLibHandlesMap;


typedef AXIS::Profile::CProfileInternal* (*createProfileFunc)(AXIS::Profile::IUid, AXIS::Service::Uid const& sid, AXIS::Profile::IProfileCallbackProxy*);

namespace AXIS {
namespace PMAL {
namespace PIM {


CError loadLib(char const* pPath, void*& pHandle)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   LOG4CPLUS_DEBUG(logger, "profile lib path = '" + std::string(pPath) + "'");
   dlerror();
   assert(pPath != NULL);

   void* h = dlopen(pPath, RTLD_NOW);
   char* err = dlerror();
   if (h == NULL)
   {
      assert(err != NULL);
      return CError(ERROR_PROFILE_LIB_LOAD, gModuleName, CError::ERROR, err);
   }

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

   char* err = dlerror();
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

   createProfileFunc createProfile = reinterpret_cast<createProfileFunc>(func);
   pPIObject = createProfile(initData.uid, initData.sid, initData.proxy);
   if (pPIObject == NULL)
   {
      return CError(ERROR_PROFILE_LIB_CREATE_PROFILE, gModuleName, CError::ERROR);
   }

   return CError::NoError(gModuleName);
}

CError createProfileImpl(char const* pProfileLibPath, ProfileInitData const& initData, AXIS::Profile::CProfileInternal*& pProfile)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   void* handle = NULL;

   CError result = loadLib(pProfileLibPath, handle);
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

void unloadProfileLib(AXIS::Profile::IUid const& piuid)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   tLibHandlesMap::iterator it = gLibHandlesMap.find(piuid);
   if (it != gLibHandlesMap.end())
   {
      void* handle = it->second;
      dlerror();
      if (dlclose(handle) != 0)
      {
         char* err = dlerror();
         CError e = CError(ERROR_PROFILE_LIB_UNLOAD, gModuleName, CError::ERROR, err);
         LOG4CPLUS_ERROR(logger, static_cast<std::string>(e));
      }
   }
}


}  // PIM
}  // ProfileManager
}  // AXIS


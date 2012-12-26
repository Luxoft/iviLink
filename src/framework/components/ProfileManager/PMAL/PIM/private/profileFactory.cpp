/* 
 * iviLINK SDK, version 1.1.19
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
 */ 
 

#include <cassert>
#include <dlfcn.h>
#include <algorithm>

#ifndef ANDROID
#else
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#endif //ANDROID

#include "CError.hpp"
#include "Logger.hpp"

#include "profileFactory.hpp"
#include "CProfileInternal.hpp"
#ifndef ANDROID
#else
#include "JniThreadHelper.hpp"
#include "StringConversion.hpp"
#include "NonnativeCallbacksWrapper.hpp"
#endif //ANDROID

#define JAVA_FILE_EXTENSION "jar"
#define C_PLUS_PLUS_FILE_EXTENSION "so"

using iviLink::Profile::CProfileInternal;

static const char gModuleName[] = "PIFactory";
static Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.Pmal.Pim.profileFactory"));

typedef std::map<iviLink::Profile::IUid, void*> tLibHandlesMap;
tLibHandlesMap gLibHandlesMap;


typedef iviLink::Profile::CProfileInternal* (*createProfileFunc)(iviLink::Profile::IUid, iviLink::Service::Uid const& sid, iviLink::Profile::IProfileCallbackProxy*);

namespace iviLink {
namespace PMAL {
namespace PIM {

bool stringEndsWith(std::string toCheck, std::string potentialEnding)
{
   return std::mismatch(potentialEnding.rbegin(), potentialEnding.rend(), toCheck.rbegin() )
          .first == potentialEnding.rend();
}

CError loadLib(char const* pPath, void*& pHandle, Android::AppInfo appInfo)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   LOG4CPLUS_INFO(logger, "profile lib path = '" + std::string(pPath) + "'");
   dlerror();
   assert(pPath != NULL);

   
   void* h = NULL;
#ifndef ANDROID
   h = dlopen(pPath, RTLD_NOW);
   char* err = dlerror();
   if (h == NULL)
   {
      assert(err != NULL);
      return CError(ERROR_PROFILE_LIB_LOAD, gModuleName, CError::ERROR, err);
   }
#else
   if (appInfo.isValid)
   {
      LOG4CPLUS_INFO(logger, "internal path =" + appInfo.privateDirPath);
      std::string stdpPath(pPath);
      std::string resultPath;   
      int found=stdpPath.find_last_of("/\\");
      resultPath = appInfo.privateDirPath + stdpPath.substr(found);
      LOG4CPLUS_INFO(logger, "resultPath =" + resultPath);
      if(! (access(resultPath.c_str(), F_OK) == 0 && access(resultPath.c_str(), R_OK) == 0)) {
         remove(resultPath.c_str()); //removing in case some lib is already there
         LOG4CPLUS_INFO(logger, "No file @ "+resultPath);
         LOG4CPLUS_INFO(logger, "Will proceed with copying");
         // caching profile *.so
         std::ifstream  source(pPath);
         std::ofstream  destination(resultPath.c_str());
         destination << source.rdbuf();
         destination.close();
         source.close();
      } else {
         LOG4CPLUS_INFO(logger, "Readable file exists there: "+resultPath);
      }
      //
      h = dlopen(resultPath.c_str(), RTLD_NOW);
      char* err = const_cast<char*>(dlerror());
      if (h == NULL)
      {
         assert(err != NULL);
         return CError(ERROR_PROFILE_LIB_LOAD, gModuleName, CError::ERROR, err);
      }
   }
   else
   {
      return CError(ERROR_PROFILE_LIB_LOAD
                  , gModuleName
                  , CError::ERROR
                  , "appInfo is not valid");
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
   char* err = const_cast<char*>(dlerror());
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

CError createProfileImpl(char const* pProfileLibPath, 
                         ProfileInitData const& initData, 
                         iviLink::Profile::CProfileInternal*& pProfile, 
                         Android::AppInfo appInfo)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   void* handle = NULL;
   CError result = CError::NoError(gModuleName);
   std::string cppExt(C_PLUS_PLUS_FILE_EXTENSION);
   std::string javaExt(JAVA_FILE_EXTENSION);
   if (stringEndsWith(std::string(pProfileLibPath), cppExt))
   {
      LOG4CPLUS_INFO(logger, "is a C++ file!");

      result = loadLib(pProfileLibPath, handle, appInfo);
      if (!result.isNoError())
      {
         LOG4CPLUS_ERROR(logger, static_cast<std::string>(result));
         return result;
      }
      assert(handle);
      gLibHandlesMap[initData.uid] = handle;
      CProfileInternal* prof = NULL;
      result = getObject(handle, initData, prof);
      pProfile = prof;
      if (!result.isNoError())
      {
         LOG4CPLUS_ERROR(logger, static_cast<std::string>(result));
         unloadProfileLib(initData.uid);
         return result;
      }
   } 
   else if (stringEndsWith(std::string(pProfileLibPath), javaExt))
   {
      LOG4CPLUS_INFO(logger, "is a Java file!");
#ifndef ANDROID
      return CError(ERROR_PROFILE_LIB_CREATE_PROFILE, gModuleName, CError::ERROR, "no known way to create a Java profile");
#else
      result = createJavaProfileImpl(pProfileLibPath, initData, pProfile, appInfo);
      if (!result.isNoError()) 
      {
         LOG4CPLUS_ERROR(logger, static_cast<std::string>(result));
         return result;
      }
#endif //ANDROID
   }
   else 
   {
      return CError(ERROR_PROFILE_LIB_CREATE_PROFILE, gModuleName, CError::ERROR, "unknown file extension");
   }

   assert(pProfile);

   return result;
}

#ifndef ANDROID
#else
CError createJavaProfileImpl(char const* pProfileLibPath, 
                             ProfileInitData const& initData, 
                             iviLink::Profile::CProfileInternal*& pProfile, 
                             Android::AppInfo appInfo)
{
   if (!appInfo.isValid)
   {
      return CError(ERROR_PROFILE_LIB_LOAD, gModuleName, CError::ERROR, "appInfo = 0");
   }

   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   LOG4CPLUS_INFO(logger, "internal path =" + appInfo.privateDirPath);
   if (dynamic_cast<NonnativeCallbacksWrapper*>(initData.proxy) == NULL) 
   {
      return CError(ERROR_PROFILE_LIB_LOAD, gModuleName, CError::ERROR, "Given application callbacks are of wrong type");
   }
   std::string stdpPath(pProfileLibPath);
   iviLink::Android::JniThreadHelper jth(appInfo.javaVM);
   JNIEnv * env = jth.getEnv();
   if (appInfo.profileCreatorClass == NULL) 
   {
      return CError(ERROR_PROFILE_LIB_LOAD, gModuleName, CError::ERROR, "profile creator class not specified - trying to load java lib from non-native app!");
   }
   jmethodID createMid = env->GetStaticMethodID(appInfo.profileCreatorClass, "createProfile", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;[B)[B");
   if (createMid == NULL) 
   {
      return CError(ERROR_PROFILE_LIB_LOAD, gModuleName, CError::ERROR, "could not find methodID for profile creation");
   }
   jstring path = iviLink::Android::StringConversion::stdToJ(stdpPath, env);
   jstring uid = iviLink::Android::StringConversion::stdToJ(initData.uid.value(), env);
   jstring sid = iviLink::Android::StringConversion::stdToJ(initData.sid.value(), env);   
   jbyteArray appCallbacks = env->NewByteArray(sizeof(initData.proxy)); 
   char serializedPointer[sizeof(initData.proxy)];
   memcpy(serializedPointer, &initData.proxy, sizeof(initData.proxy));
   env->SetByteArrayRegion(appCallbacks, 0, sizeof(initData.proxy), (jbyte*)serializedPointer);

   jbyteArray profileInternal= static_cast<jbyteArray>(env->CallStaticObjectMethod(appInfo.profileCreatorClass, createMid,  path, uid, sid, appCallbacks));
   if (profileInternal == NULL) 
   {
      return CError(ERROR_PROFILE_LIB_LOAD, gModuleName, CError::ERROR, "profileInternal is null");
   }
   int size = env->GetArrayLength(profileInternal);
   LOG4CPLUS_INFO(logger, "got byte array with pointer to profile: " + convertIntegerToString(size));
   char profileInternalNative[size];
   env->GetByteArrayRegion(profileInternal, 0, size, (jbyte*)(profileInternalNative));
   memcpy(&pProfile, profileInternalNative, size);
   return CError::NoError(gModuleName);
}
#endif //ANDROID

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
         char* err = const_cast<char*>(dlerror());
#endif //ANDROID
         CError e = CError(ERROR_PROFILE_LIB_UNLOAD, gModuleName, CError::ERROR, err);
         LOG4CPLUS_ERROR(logger, static_cast<std::string>(e));
      }
   }
}


}  // PIM
}  // ProfileManager
}  // AXIS


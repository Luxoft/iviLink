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


/**
 * \file macro.hpp
 * This header contains macros definitions for implementing profile proxies 
 * (please see samples/linux/Profiles/ProfileProxy/CBasicSampleProfileProxy.hpp).
 */

#ifndef MACRO_HPP_
#define MACRO_HPP_

/**
 * This macro should be put inside a void method declaration (instead of function body).
 * @param API_CLASS profile API class that was used in IVILINK_PROFILE_PROXY_BEGIN
 * @param FUNC name of a profile API function that should be called
 * @param ... list of arguments for the profile API function
 */

#define IVILINK_PROXY_VOID_FUNCTION(API_CLASS,FUNC, ...) \
		API_CLASS * pInstance = getProfile(); \
      if (pInstance) \
      { setError(BaseError::NoError("ProfileProxy","no error")); \
      pInstance->FUNC(__VA_ARGS__); } \
      else { setError(BaseError(1,"ProfileProxy",BaseError::IVILINK_ERROR, "No profile instance")); } \
      releaseProfile();
      
/**
 * This macro should be put inside a non-void method declaration (instead of function body).
 * @param API_CLASS profile API class that was used in IVILINK_PROFILE_PROXY_BEGIN
 * @param FUNC name of a profile API function that should be called
 * @param RET_TYPE type of return value of the profile API function
 * @param ... list of arguments
 */

#define IVILINK_PROXY_FUNCTION(API_CLASS,FUNC,RET_TYPE, ...) \
      API_CLASS * pInstance = getProfile(); \
      RET_TYPE res; \
      if (pInstance) \
      { setError(BaseError::NoError("ProfileProxy","no error")); \
      res = pInstance->FUNC(__VA_ARGS__); } \
      else { setError(BaseError(1,"ProfileProxy",BaseError::IVILINK_ERROR, "No profile instance")); } \
      releaseProfile(); \
      return res;

/**
 * This macro is for internal use only, please use IVILINK_PROXY_FUNCTION instead.
 */
 
#define IVILINK_PROXY_FUNCTION_CONST(API_CLASS,FUNC,RET_TYPE, ...) \
      const API_CLASS * pInstance = getProfile(); \
      RET_TYPE res; \
      if (pInstance) \
      { setError(BaseError::NoError("ProfileProxy","no error")); \
      res = pInstance->FUNC(__VA_ARGS__); } \
      else { setError(BaseError(1,"ProfileProxy",BaseError::IVILINK_ERROR, "No profile instance")); } \
      releaseProfile(); \
      return res;

/**
 * This macro should be put at the beginning 
 * of a profile proxy declaration.
 * @param PROFILE_PROXY name of profile proxy class
 * @param PROFILE_API name of a profile API class
 * @param PROFILE_API_UID UID of the profile API
 */

#define IVILINK_PROFILE_PROXY_BEGIN(PROFILE_PROXY, PROFILE_API, PROFILE_API_UID) \
class PROFILE_PROXY : public iviLink::App::ProfileProxy<PROFILE_API>, public PROFILE_API \
{ \
public: \
   PROFILE_PROXY(const iviLink::Service::Uid & service, iviLink::Android::AppInfo appInfo = iviLink::Android::AppInfo()) \
      : iviLink::App::ProfileProxy<PROFILE_API>(service, \
            iviLink::Profile::ApiUid(PROFILE_API_UID), appInfo) \
   { } \
   virtual std::string getName() const \
   { \
      IVILINK_PROXY_FUNCTION_CONST(PROFILE_API,getName,std::string) \
   } \
   virtual UInt32 getVersion() const \
   { \
      IVILINK_PROXY_FUNCTION_CONST(PROFILE_API,getVersion,UInt32) \
   } \
   virtual iviLink::Profile::Uid getUid() const \
   { \
      IVILINK_PROXY_FUNCTION_CONST(PROFILE_API,getUid,iviLink::Profile::Uid) \
   }   

/**
 * This macro should be put at the end
 * of a profile proxy declaration.
 */

#define IVILINK_PROFILE_PROXY_END };

#endif /* MACRO_HPP_ */

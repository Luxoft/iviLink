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


#ifndef PROFILEFACTORY_HPP_
#define PROFILEFACTORY_HPP_

#include "BaseError.hpp"

#include "ProfileInternal.hpp"
#include "ProfileHolder.hpp"

#include "AppInfo.hpp"

namespace iviLink
{
namespace PMAL
{
namespace PIM
{

enum EPIMErrors
{
   ERROR_PROFILE_LIB_LOAD = 1,
   ERROR_PROFILE_LIB_UNLOAD,
   ERROR_PROFILE_LIB_NO_CREATE_FUNC,
   ERROR_PROFILE_LIB_CREATE_PROFILE
};


struct ProfileInitData
{
   iviLink::Profile::IUid const& uid;
   iviLink::Service::Uid const& sid;
   Profile::IProfileCallbackProxy* proxy;

   ProfileInitData(Profile::IUid const& uid, 
                   Service::Uid const& sid, 
                   Profile::IProfileCallbackProxy* proxy) :
      uid(uid),
      sid(sid),
      proxy(proxy)
   {}
};


BaseError createProfileImpl(char const* pProfileLibPath, 
                         ProfileInitData const& initData, 
                         Profile::ProfileInternal*& pProfile, 
                         Android::AppInfo appInfo);
#ifndef ANDROID
#else
BaseError createJavaProfileImpl(char const* pProfileLibPath, 
                             ProfileInitData const& initData, 
                             Profile::ProfileInternal*& pProfile, 
                             Android::AppInfo appInfo);
#endif //ANDROID

void unloadProfileLib(iviLink::Profile::IUid const& piuid);

bool stringEndsWith(std::string toCheck, std::string potentialEnding);

}  // PIM
}  // ProfileManager
}  // AXIS

#endif /* PROFILEFACTORY_HPP_ */

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


#include <algorithm>

#include "CProfileInstanceMap.hpp"

using iviLink::PMAL::PIM::CProfileInstanceMap;

static const char gModuleName[] = "CProfileInstanceMap";
Logger iviLink::PMAL::PIM::CProfileInstanceMap::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.Pmal.Pim.ProfileInstanceMap"));


CProfileInstanceMap::CProfileInstanceMap()
{

}

CProfileInstanceMap::~CProfileInstanceMap()
{
   mMutex.lock();
   if (!mProfileMap.empty() || !mUidMap.empty())
   {
      LOG4CPLUS_WARN(msLogger, "Nonempty maps");
   }
   mMutex.unlock();
}

BaseError CProfileInstanceMap::registerProfile(iviLink::Profile::IUid const& profileUid, iviLink::Profile::ProfileInternal* pInstance)
{
   if (pInstance == NULL)
      return BaseError(1, gModuleName, BaseError::IVILINK_ERROR, "pInstance is NULL");

   BaseError err = BaseError::NoError(gModuleName);

   mMutex.lock();
   tProfileMap::iterator it = mProfileMap.find(profileUid);
   if (it != mProfileMap.end())
   {
      err = BaseError(1, gModuleName, BaseError::IVILINK_ERROR, "uid exists");
   }
   else
   {
      mProfileMap[profileUid] = pInstance;
      mUidMap[pInstance] = profileUid;
   }
   mMutex.unlock();

   return err;
}


BaseError CProfileInstanceMap::unregisterProfile(iviLink::Profile::ProfileInternal* pInstance)
{
   if (pInstance == NULL)
         return BaseError(1, gModuleName, BaseError::IVILINK_ERROR, "pInstance is NULL");

   BaseError err = BaseError::NoError(gModuleName);

   mMutex.lock();
   tUidMap::iterator it = mUidMap.find(pInstance);
   if (it == mUidMap.end())
   {
      err = BaseError(1, gModuleName, BaseError::IVILINK_ERROR, "profile not found");
   }
   else
   {
      iviLink::Profile::IUid uid = it->second;
      mProfileMap.erase(uid);
      mUidMap.erase(it);
   }
   mMutex.unlock();

   return err;
}

BaseError CProfileInstanceMap::find(iviLink::Profile::IUid const& profileUid, iviLink::Profile::ProfileInternal*& pInstance)
{
   BaseError err = BaseError::NoError(gModuleName);

   mMutex.lock();
   tProfileMap::iterator it = mProfileMap.find(profileUid);
   if (it == mProfileMap.end())
   {
      err = BaseError(1, gModuleName, BaseError::IVILINK_ERROR, "profile not found");
   }
   else
   {
      pInstance = it->second;
   }
   mMutex.unlock();

   return err;
}

BaseError CProfileInstanceMap::find(iviLink::Profile::ProfileInternal* const pInstance, iviLink::Profile::IUid& profileUid)
{
   BaseError err = BaseError::NoError(gModuleName);

   mMutex.lock();
   tUidMap::iterator it = mUidMap.find(pInstance);
   if (it == mUidMap.end())
   {
      err = BaseError(1, gModuleName, BaseError::IVILINK_ERROR, "profile not found");
   }
   else
   {
      profileUid = it->second;
   }
   mMutex.unlock();

   return err;
}

BaseError CProfileInstanceMap::isKnown(iviLink::Profile::IUid const& profileUid)
{
   BaseError err = BaseError::NoError(gModuleName);

   mMutex.lock();
   tProfileMap::const_iterator it = mProfileMap.find(profileUid);
   if (it == mProfileMap.end())
   {
      err = BaseError(1, gModuleName, BaseError::IVILINK_ERROR, "profile not found");
   }
   mMutex.unlock();

   return err;
}

BaseError CProfileInstanceMap::isKnown(iviLink::Profile::ProfileInternal* pInstance)
{
   BaseError err = BaseError::NoError(gModuleName);

   mMutex.lock();
   tUidMap::const_iterator it = mUidMap.find(pInstance);
   if (it == mUidMap.end())
   {
      err = BaseError(1, gModuleName, BaseError::IVILINK_ERROR, "profile not found");
   }
   mMutex.unlock();

   return err;
}

BaseError CProfileInstanceMap::getAllRegisterd(tProfilesList& profiles)
{
   BaseError err = BaseError::NoError(gModuleName);

   profiles.clear();

   mMutex.lock();
   for (tProfileMap::const_iterator it = mProfileMap.begin(); it != mProfileMap.end(); ++it)
   {
      profiles.push_back(std::make_pair(it->first, it->second));
   }
   mMutex.unlock();

   return err;
}

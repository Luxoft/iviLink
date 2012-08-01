/* 
 * 
 * iviLINK SDK, version 1.0
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

CError CProfileInstanceMap::registerProfile(iviLink::Profile::IUid const& profileUid, iviLink::Profile::CProfileInternal* pInstance)
{
   if (pInstance == NULL)
      return CError(1, gModuleName, CError::ERROR, "pInstance is NULL");

   CError err = CError::NoError(gModuleName);

   mMutex.lock();
   tProfileMap::iterator it = mProfileMap.find(profileUid);
   if (it != mProfileMap.end())
   {
      err = CError(1, gModuleName, CError::ERROR, "uid exists");
   }
   else
   {
      mProfileMap[profileUid] = pInstance;
      mUidMap[pInstance] = profileUid;
   }
   mMutex.unlock();

   return err;
}


CError CProfileInstanceMap::unregisterProfile(iviLink::Profile::CProfileInternal* pInstance)
{
   if (pInstance == NULL)
         return CError(1, gModuleName, CError::ERROR, "pInstance is NULL");

   CError err = CError::NoError(gModuleName);

   mMutex.lock();
   tUidMap::iterator it = mUidMap.find(pInstance);
   if (it == mUidMap.end())
   {
      err = CError(1, gModuleName, CError::ERROR, "profile not found");
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

CError CProfileInstanceMap::find(iviLink::Profile::IUid const& profileUid, iviLink::Profile::CProfileInternal*& pInstance)
{
   CError err = CError::NoError(gModuleName);

   mMutex.lock();
   tProfileMap::iterator it = mProfileMap.find(profileUid);
   if (it == mProfileMap.end())
   {
      err = CError(1, gModuleName, CError::ERROR, "profile not found");
   }
   else
   {
      pInstance = it->second;
   }
   mMutex.unlock();

   return err;
}

CError CProfileInstanceMap::find(iviLink::Profile::CProfileInternal* const pInstance, iviLink::Profile::IUid& profileUid)
{
   CError err = CError::NoError(gModuleName);

   mMutex.lock();
   tUidMap::iterator it = mUidMap.find(pInstance);
   if (it == mUidMap.end())
   {
      err = CError(1, gModuleName, CError::ERROR, "profile not found");
   }
   else
   {
      profileUid = it->second;
   }
   mMutex.unlock();

   return err;
}

CError CProfileInstanceMap::isKnown(iviLink::Profile::IUid const& profileUid)
{
   CError err = CError::NoError(gModuleName);

   mMutex.lock();
   tProfileMap::const_iterator it = mProfileMap.find(profileUid);
   if (it == mProfileMap.end())
   {
      err = CError(1, gModuleName, CError::ERROR, "profile not found");
   }
   mMutex.unlock();

   return err;
}

CError CProfileInstanceMap::isKnown(iviLink::Profile::CProfileInternal* pInstance)
{
   CError err = CError::NoError(gModuleName);

   mMutex.lock();
   tUidMap::const_iterator it = mUidMap.find(pInstance);
   if (it == mUidMap.end())
   {
      err = CError(1, gModuleName, CError::ERROR, "profile not found");
   }
   mMutex.unlock();

   return err;
}

CError CProfileInstanceMap::getAllRegisterd(tProfilesList& profiles)
{
   CError err = CError::NoError(gModuleName);

   profiles.clear();

   mMutex.lock();
   for (tProfileMap::const_iterator it = mProfileMap.begin(); it != mProfileMap.end(); ++it)
   {
      profiles.push_back(std::make_pair(it->first, it->second));
   }
   mMutex.unlock();

   return err;
}

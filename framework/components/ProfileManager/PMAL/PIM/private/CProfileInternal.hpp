/* 
 * 
 * iviLINK SDK, version 1.0.1
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







#ifndef CPROFILEINTERNAL_HPP__
#define CPROFILEINTERNAL_HPP__

#include <map>
#include <string>

#include "utils/misc/UID.hpp"
#include "utils/misc/CError.hpp"

#include "framework/public/appLib/CProfileApiBase.hpp"

namespace iviLink
{

   namespace PMAL
   {
      namespace PIM
      {
         class CProfileDestroyer;
         class CProfileEnabler;
      }
   }

   namespace Profile
   {

      /// Base profile interface, available to profile-manager
      class CProfileInternal
      {
      public:

         /// Returns profile instance UID
         IUid getProfileID() const
         {
            return mProfileID;
         }

         CError getError() const
         {
            return mpInstance->getError();
         }

         bool isEnabled() const
         {
            return mIsEnabled;
         }

         virtual CProfileApiBase * profileInstance()
         {
            return mpInstance;
         }

      protected:

         /// This is only class can destroy the profile.
         friend class iviLink::PMAL::PIM::CProfileDestroyer;
         friend class iviLink::PMAL::PIM::CProfileEnabler;

         explicit CProfileInternal(IUid const& id, CProfileApiBase * pProfileInstance)
            : mProfileID(id)
            , mIsEnabled(false)
            , mpInstance(pProfileInstance)
         {
         }

         // Only CProfileManager can destroy profile
         virtual ~CProfileInternal()
         {
            delete mpInstance;
         }

         void setEnabled(bool value)
         {
            if (mIsEnabled != value)
            {
               mIsEnabled = value;
               onEnabledChange();
            }
         }

      private:
         void onEnabledChange()
         {
            mpInstance->mEnabled = mIsEnabled;
            if (mIsEnabled)
            {
               mpInstance->onEnable();
            }
            else
            {
               mpInstance->onDisable();
            }
         }

         CProfileInternal(CProfileInternal const&);
         CProfileInternal& operator=(CProfileInternal const&);

         const Profile::IUid mProfileID;
         bool mIsEnabled;
         CProfileApiBase * mpInstance;
      };

   }
}

#endif // CPROFILEINTERNAL_HPP__

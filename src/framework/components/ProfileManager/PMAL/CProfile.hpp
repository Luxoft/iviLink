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







#ifndef IPROFILE_HPP__
#define IPROFILE_HPP__

#include <map>
#include <string>

#include "utils/misc/UID.hpp"
#include "utils/misc/CError.hpp"


#include "framework/components/ProfileManager/PMAL/PIM/private/CProfileInternal.hpp"

namespace iviLink
{
   namespace Profile
   {
      class CProfile;
   }
}

namespace iviLink
{
   namespace Profile
   {

      /// Base interface of profile, available for application
      class CProfile : public CProfileInternal
      {
      public:

         CProfile(IUid const& id, Service::Uid const& sid, CProfileApiBase * pProfileInstance)
            : CProfileInternal(id, pProfileInstance)
            , mSid(sid)
            , mLocked(false)
         {
         }

         Service::Uid const& getServiceUID() const
         {
            return mSid;
         }

         virtual CProfileApiBase * profileInstance()
         {
            mLocked = true;
            return CProfileInternal::profileInstance();
         }

         void release()
         {
            mLocked = false;
         }

         bool isLocked() const
         {
            return mLocked;
         }

      private:

         virtual ~CProfile()
         {
         }

         Service::Uid const mSid;
         bool mLocked;
      };

   }
}


#endif // IPROFILE_HPP__

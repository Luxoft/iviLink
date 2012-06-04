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



#ifndef CPROFILEINTERNAL_HPP__
#define CPROFILEINTERNAL_HPP__

#include <map>
#include <string>

#include "utils/misc/include/UID.hpp"

#include "utils/misc/include/CError.hpp"

namespace AXIS
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

   virtual bool wasError() const = 0;
   virtual CError getError() const = 0;

   bool isEnabled() const { return mIsEnabled; }

protected:

   /// This is only class can destroy the profile.
   friend class AXIS::PMAL::PIM::CProfileDestroyer;
   friend class AXIS::PMAL::PIM::CProfileEnabler;

   explicit CProfileInternal(IUid const& id) :
         mProfileID(id),
         mIsEnabled(false)
   {
   }

   // Only CProfileManager can destroy profile
   virtual ~CProfileInternal()
   {
   }

   virtual void onEnabledChange() = 0;

   void setEnabled(bool value)
   { 
      if (mIsEnabled != value) 
      { 
         mIsEnabled = value; 
         onEnabledChange(); 
      } 
   }

private:
   CProfileInternal(CProfileInternal const&);
   CProfileInternal& operator=(CProfileInternal const&);

   const Profile::IUid mProfileID;
   bool mIsEnabled;
};

class IProfileCallbackProxy: public IErrorHandler
{
public:
   virtual ~IProfileCallbackProxy()
   {
   }
};

}
}

#endif // CPROFILEINTERNAL_HPP__

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

#ifndef IPROFILE_HPP__
#define IPROFILE_HPP__

#include "utils/misc/include/UID.hpp"
#include "framework/components/ProfileManager/PMAL/PIM/include/CProfileInternal.hpp"

namespace AXIS
{
namespace Profile
{

/// Base interface of profile, available to application
class CProfile : public CProfileInternal
{
public:
   /// Returns profile name
   virtual std::string const & getName() const = 0;
   /// Returns profile version
   virtual UInt32 getVersion() const = 0;

   /// Returns profileApi UID
   virtual ApiUid const & getPAPI_UID() const = 0;

   Service::Uid const& getServiceUID() const { return mSid; }

protected:
   CProfile(IUid const& id, Service::Uid const& sid) :
      CProfileInternal(id),
      mSid(sid)
   {
   }

   virtual ~CProfile() { }

private:
   Service::Uid const mSid;
};

}
}


#endif // IPROFILE_HPP__

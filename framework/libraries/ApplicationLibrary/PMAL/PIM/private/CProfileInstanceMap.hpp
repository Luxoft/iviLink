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


#ifndef INSTANCEMAP_HPP_
#define INSTANCEMAP_HPP_

#include <map>
#include <vector>
#include <utility>

#include "Logger.hpp"
#include "CMutex.hpp"

#include "ProfileInternal.hpp"

namespace iviLink {
namespace PMAL {
namespace PIM {


class CProfileInstanceMap
{
public:
   CProfileInstanceMap();
   ~CProfileInstanceMap();


   /**
    * Registers profile instance
    * @param profileUid
    * @param pInstance
    * @return
    */
   BaseError registerProfile(iviLink::Profile::IUid const& profileUid, iviLink::Profile::ProfileInternal* pInstance);

   /**
    * Unregisters profile instance
    * @param pInstance
    * @return
    */
   BaseError unregisterProfile(iviLink::Profile::ProfileInternal* pInstance);

   /**
    *
    * @param profileUid
    * @param[out] pInstance
    * @return
    */
   BaseError find(iviLink::Profile::IUid const& profileUid, iviLink::Profile::ProfileInternal*& pInstance);

   /**
    *
    * @param pInstance
    * @param[out] profileUid
    * @return
    */
   BaseError find(iviLink::Profile::ProfileInternal* const pInstance, iviLink::Profile::IUid& profileUid);

   /**
    *
    * @param profileUid
    * @return
    */
   BaseError isKnown(iviLink::Profile::IUid const& profileUid);

   /**
    *
    * @param pInstance
    * @return
    */
   BaseError isKnown(iviLink::Profile::ProfileInternal* pInstance);

   typedef std::vector<std::pair<iviLink::Profile::IUid, iviLink::Profile::ProfileInternal*> > tProfilesList;

   /**
    * Returns the vector of all registered profiles
    * @param profiles
    * @return
    */
   BaseError getAllRegisterd(tProfilesList& profiles);


private:

   typedef std::map<iviLink::Profile::IUid, iviLink::Profile::ProfileInternal*> tProfileMap;
   tProfileMap mProfileMap;

   typedef std::map<iviLink::Profile::ProfileInternal*, iviLink::Profile::IUid> tUidMap;
   tUidMap mUidMap;

   CMutex mMutex;

   static Logger msLogger;

};

}  // namespace PIM
}  // namespace ProfileManager
}  // namespace AXIS


#endif /* INSTANCEMAP_HPP_ */

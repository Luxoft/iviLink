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


#ifndef CTRUSTLIST_HPP__
#define CTRUSTLIST_HPP__

#include <vector>

#include "BaseUid.hpp"
#include "Logger.hpp"

#include "CTrustListError.hpp"

namespace iviLink
{


class ITrustListStorage;

class CTrustList
{
public:

   iviLink::BaseUid getOurUid() const;

   bool isKnownUid(iviLink::BaseUid const& uid) const;

   /**
    * @retval IVILINK_NO_ERROR
    * @retval ERROR_STORAGE
    */
   CTrustListError addUid(iviLink::BaseUid const& uid);

   /**
    * @retval IVILINK_NO_ERROR
    * @retval ERROR_UNKNOWN_UID
    * @retval ERROR_STORAGE
    */
   CTrustListError removeUid(iviLink::BaseUid const& uid);

   /**
    * @retval IVILINK_NO_ERROR
    * @retval ERROR_STORAGE
    */
   CTrustListError clearList();

   typedef std::vector<iviLink::BaseUid> tUidVector;
   /**
    * @retval IVILINK_NO_ERROR
    * @retval ERROR_STORAGE
    */
   CTrustListError getKnownUids(tUidVector& result) const;

public:
   CTrustList(std::string pathToTrlist);

   ~CTrustList();

private:

   BaseUid generateNewUid() const;

   ITrustListStorage* mpStorage;

private:

   static Logger msLogger;
};

}  // namespace iviLink

#endif // CTRUSTLIST_HPP__


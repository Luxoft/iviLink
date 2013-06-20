/* 
 * 
 * iviLINK SDK, version 1.1.2
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
   std::string getOurName() const;

   bool isKnownUid(iviLink::BaseUid const& uid) const;

   /**
    * @retval IVILINK_NO_ERROR
    * @retval ERROR_STORAGE
    */
   CTrustListError addUid(iviLink::BaseUid const& uid);
   CTrustListError addUid(iviLink::BaseUid const& uid, std::string const& name);

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
   typedef std::pair<iviLink::BaseUid,std::string> tNameUid;
   typedef std::vector<tNameUid> tNameUidVector;
   /**
    * @retval IVILINK_NO_ERROR
    * @retval ERROR_STORAGE
    */
   CTrustListError getKnownUids(tNameUidVector& result) const;

   /**
    * @retval IVILINK_NO_ERROR
    * @retval ERROR_STORAGE
    */
   CTrustListError getLastConnection(iviLink::BaseUid& uid, time_t &time);

   /**
    * @retval IVILINK_NO_ERROR
    * @retval ERROR_STORAGE
    */
   CTrustListError setLastConnection(iviLink::BaseUid const& uid, time_t const &time);

   /**
    * @retval IVILINK_NO_ERROR
    * @retval ERROR_STORAGE
    */
   CTrustListError setLastConnection(iviLink::BaseUid const& uid);

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


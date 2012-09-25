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












#ifndef CAPPDATA_HPP_
#define CAPPDATA_HPP_

#include <set>

#include "utils/misc/UID.hpp"

namespace iviLink {
namespace PMP {
namespace PIM {

class CAppData
{
public:

   CAppData() {}

   bool hasService(iviLink::Service::Uid const& sid) const;
   void addService(iviLink::Service::Uid const& sid);
   void removeServie(iviLink::Service::Uid const& sid);

private:

   typedef std::set<iviLink::Service::Uid> tServices;
   tServices mServices;


};

}  // namespace PIM
}  // namespace ProfileManager
}  // namespace AXIS


#endif /* CAPPDATA_HPP_ */

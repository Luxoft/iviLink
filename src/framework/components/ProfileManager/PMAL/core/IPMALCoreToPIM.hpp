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









#ifndef ICORETOPIM_HPP_
#define ICORETOPIM_HPP_

#include "framework/components/ProfileManager/PMAL/CPMALError.hpp"
#include "utils/misc/UID.hpp"

namespace iviLink {
namespace PMAL {

class IPMALCoreToPIM
{
public:
   /**
    * Returns the path to profile library from its id
    * @param id is id of profile
    * @param[out] path to the library
    * @retval NO_ERROR
    * @retval ...
    */
   virtual CPMALError getProfileLibPath(Profile::Uid id, std::string& path) = 0;

   /**
    * Returns the status of profile
    * @param id is id of profile
    * @param[out] enabled is the status of profile
    * @retval NO_ERROR
    * @retval ...
    */
   virtual CPMALError isEnabledProfile(Profile::Uid id, bool& enabled) = 0;

protected:
   virtual ~IPMALCoreToPIM() { }
};


}  // namespace ProfileManager
}  // namespace AXIS



#endif /* ICORETOPIM_HPP_ */

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


#ifndef CPIUIDGENERATOR_HPP_
#define CPIUIDGENERATOR_HPP_

#include "BaseError.hpp"
#include "ProfileInternal.hpp"


namespace iviLink
{
namespace PMAL
{
namespace PIM
{

/**
 * Returns new PIUID unique on both sides
 * @param uid
 * @retval ...
 */
BaseError getPIUID(Profile::IUid& uid);

/**
 * Releases PIUID
 * @param uid
 * @return
 */
BaseError releasePIUID(Profile::IUid const& uid);

}
}
}


#endif /* CPIUIDGENERATOR_HPP_ */

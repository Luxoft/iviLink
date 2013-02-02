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


/**
 * \file common.hpp
 * This file contains enum declarations.
 */


#ifndef IVILINK_APP_LIB_COMMON_HPP_
#define IVILINK_APP_LIB_COMMON_HPP_

#include <list>

#include "UID.hpp"
#include "ProfileHolder.hpp"

 /**
  * \namespace iviLink
  * This namespace is a basic namespace for all framework's components and libraries.
  */

namespace iviLink
{
/**
 * \enum ELaunchInfo
 * ELaunchInfo is used to pass information about application's launch
 * (is a parameter in CApp's callback initDone()).
 */
enum ELaunchInfo
{
    LAUNCHED_BY_IVILINK, /*!< application was launched by Application Manager after a request from the other side */
    LAUNCHED_BY_USER, /*!< application was launched by user */
    ERROR_APP_NOT_INITED_IN_IVILINK /*!< error code */
};

/**
 * \enum ELoadServiceResult
 * ELoadServiceResult is supposed to represent results of loading a service
 * (currently it is not used).
 */
enum ELoadServiceResult
{
    LOAD_OK,    /*!< service was successfully loaded */
    LOAD_ERROR_APP_NOT_INITED, /*!< error code: tried to load service from a not initialized application */
    LOAD_ERROR_UNKNOWN_SERVICE, /*!< error code: requested service was not found by Service Manager */
    LOAD_ERROR_OTHER /*!< error code: unknown error */
};

namespace Service
{

typedef std::list<Uid> ListOfUids;

} // namespace Service
} // namespace iviLink

#endif

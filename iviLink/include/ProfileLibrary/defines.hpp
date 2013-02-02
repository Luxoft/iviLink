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
 * \file defines.hpp
 * This file contains useful macros for creating profiles. See
 * samples/linux/Profiles/BasicSampleProfile for examples.
 */

#ifndef DEFINES_HPP_
#define DEFINES_HPP_

/**
 * \def IVILINK_PROFILE_BEGIN
 * This macro should be put into the beginning of a header with profile implementation declaration.
 */
#define IVILINK_PROFILE_BEGIN \
   extern "C" iviLink::Profile::ProfileHolder* createProfile \
   (iviLink::Profile::IUid const& id, iviLink::Service::Uid const& sid, \
         iviLink::Profile::IProfileCallbackProxy* pCbProxy);
         
         
/**
 * \def IVILINK_PROFILE_END(PROFILE_CLASS)
 * This macro should be put into the end of a header with profile implementation declaration.
 * @param PROFILE_CLASS profile implementation class name
 */
#define IVILINK_PROFILE_END(PROFILE_CLASS) \
   extern "C" iviLink::Profile::ProfileHolder* createProfile \
         (iviLink::Profile::IUid const& id, iviLink::Service::Uid const& sid, \
               iviLink::Profile::IProfileCallbackProxy* pCbProxy) \
{ return new iviLink::Profile::ProfileHolder(id, sid, new PROFILE_CLASS(pCbProxy)); }


/**
 * \def IVILINK_PROFILE_DECLARE
 * This macro should be put into the beginning of a profile implementation declaration.
 */
#define IVILINK_PROFILE_DECLARE \
   friend iviLink::Profile::ProfileHolder* createProfile(iviLink::Profile::IUid const& id, \
         iviLink::Service::Uid const& sid, iviLink::Profile::IProfileCallbackProxy*);
         
/**
 * \def IVILINK_PROFILE_NAME(PROFILE_NAME)
 * This macro should be put into a profile implementation declaration.
 * @param PROFILE_NAME name of the profile implementation (see samples/linux/Profiles/xml/BasicSampleProfile_Impl.xml)
 */

#define IVILINK_PROFILE_NAME(PROFILE_NAME) \
   virtual std::string getName() const \
   { return std::string(#PROFILE_NAME ); }
   
/**
 * \def IVILINK_PROFILE_VERSION(VERSION)
 * This macro should be put into a profile implementation declaration.
 * @param VERSION profile implementation version (see samples/linux/Profiles/xml/BasicSampleProfile_Impl.xml)
 */

#define IVILINK_PROFILE_VERSION(VERSION) \
   virtual UInt32 getVersion() const \
   { return 3; }
   
/**
 * \def IVILINK_PROFILE_UID(PROFILE_UID)
 * This macro should be put into a profile implementation declaration.
 * @param PROFILE_UID UID of the profile implementation (see samples/linux/Profiles/xml/BasicSampleProfile_Impl.xml)
 */
#define IVILINK_PROFILE_UID(PROFILE_UID) \
   virtual iviLink::Profile::Uid getUid() const \
   { return iviLink::Profile::Uid(#PROFILE_UID ); }

#endif /* DEFINES_HPP_ */
